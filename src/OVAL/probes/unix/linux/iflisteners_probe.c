/**
 * @file   iflisteners_probe.c
 * @brief  iflisteners probe
 * @author "Petr Lautrbach <plautrba@redhat.com>"
 *
 */

/*
 * Copyright 2009-2011 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *   Steve Grubb <sgrubb@redhat.com>
 *   Petr Lautrbach <plautrba@redhat.com>
 */

/*
 * iflisteners probe:
 *
 * interface_name
 * protocol
 * hw_address
 * program_name
 * pid
 * user_id
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <regex.h>

#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "util.h"
#include "common/debug_priv.h"
#include "_sexp-manip_r.h"

#include "iflisteners-proto.h"
#include "iflisteners_probe.h"

/* Convenience structure for the results being reported */
struct result_info {
	const char *interface_name;
	const char *protocol;
	const char *hw_address;
};

/* This structure is used to store information from scanning all runnning
 * processes. It will be used later to augment server info */
typedef struct _lnode {
  pid_t pid;            // process ID
  uid_t uid;            // effective user ID
  char *cmd;            // command run by user
  unsigned long inode;  // inode of socket
  struct _lnode* next;  // Next node pointer
} lnode;

/* This is the linked list head. Only data elements go here. */
typedef struct {
  lnode *head;          // List head
  lnode *cur;           // Pointer to current node
} llist;

struct interface_t {
  char interface_name[255];
  char hw_address[255];
};

/* Local data */
static int perm_warn = 0;

/* Function prototypes */
static inline lnode *list_get_cur(llist *l) { return l->cur; }
static void list_create(llist *l);
static void list_append(llist *l, lnode *node);
static void list_clear(llist* l);
static lnode *list_find_inode(llist *l, unsigned long i);


static void list_create(llist *l)
{
	l->head = NULL;
	l->cur = NULL;
}

static void list_append(llist *l, lnode *node)
{
	lnode* newnode;

	if (node == NULL || l == NULL)
		return;

	newnode = malloc(sizeof(lnode));
	if (newnode == NULL)
		return;

	newnode->pid = node->pid;
	newnode->uid = node->uid;
	newnode->inode = node->inode;
	// Take custody of the memory
	newnode->cmd = node->cmd;
	newnode->next = NULL;

	// if we are at top, fix this up
	if (l->head == NULL)
		l->head = newnode;
	else	// Otherwise add pointer to newnode
		l->cur->next = newnode;

	// make newnode current
	l->cur = newnode;
}

static void list_clear(llist* l)
{
	lnode* nextnode;
	register lnode* cur;

	cur = l->head;
	while (cur) {
		nextnode=cur->next;
		free(cur->cmd);
		free(cur);
		cur=nextnode;
	}
	l->head = NULL;
	l->cur = NULL;
}

static lnode *list_find_inode(llist *l, unsigned long i)
{
        register lnode* cur;

	cur = l->head;	/* start at the beginning */
	while (cur) {
		if (cur->inode == i) {
			l->cur = cur;
			return cur;
		} else
			cur = cur->next;
	}
	return NULL;
}

static int collect_process_info(llist *l)
{
	DIR *d, *f;
	struct dirent *ent;

	d = opendir("/proc");
	if (d == NULL)
		return 1;

	while (( ent = readdir(d) )) {
		FILE *sf;
		int pid, ppid;
		char buf[100];
		char *tmp, cmd[16], state, *text = NULL;
		int fd, len, euid = 0;

		// Skip non-process dir entries
		if(*ent->d_name<'0' || *ent->d_name>'9')
			continue;
		errno = 0;
		pid = strtol(ent->d_name, NULL, 10);
		if (errno)
			continue;

		// Parse up the stat file for the proc
		snprintf(buf, 32, "/proc/%d/stat", pid);
		fd = open(buf, O_RDONLY, 0);
		if (fd < 0)
			continue;
		len = read(fd, buf, sizeof buf - 1);
		close(fd);
		if (len < 40)
			continue;
		buf[len] = 0;
		tmp = strrchr(buf, ')');
		if (tmp)
			*tmp = 0;
		else
			continue;
		memset(cmd, 0, sizeof(cmd));
		sscanf(buf, "%d (%15c", &ppid, cmd);
		sscanf(tmp+2, "%c %d", &state, &ppid);

		// Skip kthreads
		if (pid == 2 || ppid == 2)
			continue;

		// Get the effective uid
		snprintf(buf, 32, "/proc/%d/status", pid);
		sf = fopen(buf, "rt");
		if (sf) {
			int line = 0;
			__fsetlocking(sf, FSETLOCKING_BYCALLER);
			while (fgets(buf, sizeof(buf), sf)) {
				if (line == 0) {
					line++;
					continue;
				}
				if (memcmp(buf, "Uid:", 4) == 0) {
					int id;
					sscanf(buf, "Uid: %d %d",
						&id, &euid);
					break;
				}
			}
			fclose(sf);
		}

		// Now lets get the inodes each process has open
		snprintf(buf, 32, "/proc/%d/fd", pid);
		f = opendir(buf);
		if (f == NULL) {
			if (errno == EACCES && perm_warn == 0) {
				/* Need DAC_OVERRIDE permission */
				perm_warn = 1;
			}
			// Process might have ended or something - ignore it
			free(text);
			continue;
		}
		// For each file in the fd dir...
		while (( ent = readdir(f) )) {
			char line[PATH_MAX], ln[PATH_MAX], *s, *e;
			unsigned long inode;
			lnode node;
			int lnlen;

			if (ent->d_name[0] == '.')
				continue;
			snprintf(ln, PATH_MAX, "%s/%s", buf, ent->d_name);
			if ((lnlen = readlink(ln, line, sizeof(line)-1)) < 0)
				continue;
			line[lnlen] = 0;

			// Only look at the socket entries
			if (memcmp(line, "socket:", 7) == 0) {
				// Type 1 sockets
				s = strchr(line+7, '[');
				if (s == NULL)
					continue;
				s++;
				e = strchr(s, ']');
				if (e == NULL)
					continue;
				*e = 0;
			} else if (memcmp(line, "[0000]:", 7) == 0) {
				// Type 2 sockets
				s = line + 8;
			} else
				continue;
			errno = 0;
			inode = strtoul(s, NULL, 10);
			if (errno)
				continue;
			node.pid = pid;
			node.uid = euid;
			node.cmd = strdup(cmd);
			node.inode = inode;
			// We make one entry for each socket inode
			list_append(l, &node);
		}
		closedir(f);
		free(text);
	}
	closedir(d);
	return 0;
}

static void report_finding(struct result_info *res, llist *l, probe_ctx *ctx, oval_schema_version_t over)
{
        SEXP_t *item, *user_id;
	lnode *n = list_get_cur(l);

	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.10)) < 0)
		user_id = SEXP_string_newf("%d", n->uid);
	else
		user_id = SEXP_number_newi_64((int64_t)n->uid);

	item = probe_item_create(OVAL_LINUX_IFLISTENERS, NULL,
                                 "interface_name",       OVAL_DATATYPE_STRING,  res->interface_name,
                                 "protocol",             OVAL_DATATYPE_STRING,  res->protocol,
                                 "hw_address",           OVAL_DATATYPE_STRING,  res->hw_address,
                                 "program_name",         OVAL_DATATYPE_STRING,  n->cmd,
                                 "pid",                  OVAL_DATATYPE_INTEGER, (int64_t)n->pid,
				 "user_id",              OVAL_DATATYPE_SEXP, user_id,
                                 NULL);

        probe_item_collect(ctx, item);
	SEXP_free(user_id);

}

static int get_interface(const int ent_ifindex, struct interface_t *interface) {
	DIR *d;
	struct dirent *d_ent;
	int ifindex;

	d = opendir("/sys/class/net");
	if (d == NULL)
		return 0;

	while (( d_ent = readdir(d) )) {
		char buf[PATH_MAX];
		FILE *fd;

		snprintf(buf, sizeof buf - 1, "/sys/class/net/%s/ifindex", d_ent->d_name);
		fd = fopen(buf, "rt");
		if (fd == NULL)
			continue;
		if (fscanf(fd, "%d\n", &ifindex) < 1) {
			fclose(fd);
			continue;
		}
		fclose(fd);

		if (ent_ifindex == ifindex) {
			strncpy(interface->interface_name, d_ent->d_name, sizeof interface->interface_name);
			interface->interface_name[sizeof interface->interface_name - 1] = '\0';
			snprintf(buf, sizeof buf - 1, "/sys/class/net/%s/address", d_ent->d_name);
			fd = fopen(buf, "rt");
			if (fd == NULL) {
				*(interface->hw_address) = '\0';
			}
			else {
				int buf_len;
				if (fscanf(fd, "%s\n", buf) < 1)
					*buf = '\0';

				buf_len = strlen(buf);
				if ((unsigned)buf_len > (sizeof interface->hw_address)) {
					buf_len = (sizeof interface->hw_address) - 1;
				}

				snprintf(interface->hw_address, sizeof interface->hw_address, "%.*s", buf_len, buf);
				fclose(fd);
			}

			closedir(d);


			return 1;
		}
	}
	closedir(d);
	return 0;
}

static int read_packet(llist *l, probe_ctx *ctx, oval_schema_version_t over, SEXP_t *interface_name_ent)
{
	int line = 0;
	FILE *f;
	char buf[256];

	void *s;
	int refcnt, sk_type, ifindex, running;
	unsigned long inode;
	unsigned rmem, uid, proto_num;
	struct interface_t interface;


	f = fopen("/proc/net/packet", "rt");
	if (f == NULL) {
		if (errno != ENOENT)
			return 1;
		else
			return 0;
	}
	__fsetlocking(f, FSETLOCKING_BYCALLER);
	while (fgets(buf, sizeof(buf), f)) {
		if (line == 0) {
			line++;
			continue;
		}
		/* follow structure from net/packet/af_packet.c */
		sscanf(buf,
			"%p %d %d %04x %d %d %u %u %lu\n",
			&s, &refcnt, &sk_type, &proto_num, &ifindex, &running, &rmem, &uid, &inode
		);
		if (list_find_inode(l, inode) && get_interface(ifindex, &interface)) {
			struct result_info r;
			SEXP_t *r0;
			dI("Have interface_name: %s, hw_address: %s",
					interface.interface_name, interface.hw_address);

			r0 = SEXP_string_newf("%s", interface.interface_name);
			if (probe_entobj_cmp(interface_name_ent, r0) != OVAL_RESULT_TRUE) {
				SEXP_free(r0);
				continue;
			}
			SEXP_free(r0);

			r.interface_name = interface.interface_name;
			r.protocol = oscap_enum_to_string(ProtocolType, proto_num);
			r.hw_address = interface.hw_address;
			report_finding(&r, l, ctx, over);
		}
	}
	fclose(f);
	return 0;
}

int iflisteners_probe_main(probe_ctx *ctx, void *arg)
{
        SEXP_t *object;
	int err;
	llist ll;
	oval_schema_version_t over;

        object = probe_ctx_getobject(ctx);
        over   = probe_obj_get_platform_schema_version(object);

	SEXP_t *interface_name_ent = probe_obj_getent(object, "interface_name", 1);
	if (interface_name_ent == NULL) {
		err = PROBE_ENOVAL;
		goto cleanup;
	}

	// Now start collecting the info
	list_create(&ll);
	if (collect_process_info(&ll) || perm_warn) {
		SEXP_t *msg;

		msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, "Permission error.");
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);

		err = 0;
		goto cleanup;
	}

	read_packet(&ll, ctx, over, interface_name_ent);

	list_clear(&ll);

	err = 0;
 cleanup:
	SEXP_free(interface_name_ent);

	return err;
}
