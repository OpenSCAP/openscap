/**
 * @file   inetlisteningserver.c
 * @brief  inetlisteningserver probe
 * @author "Steve Grubb" <sgrubb@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process an inetlisteningserver_object as defined in OVAL 5.4 and 5.5.
 *
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
 */

/*
 * inetlisten probe:
 *
 * protocol
 * local_address
 * local_port
 * local_full_address
 * program_name
 * foreign_address
 * foreign_port
 * foreign_full_addresss
 * pid
 * user_id
 */

#include "config.h"
#include "seap.h"
#include "probe-api.h"
#include "probe-entcmp.h"
#include "alloc.h"
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

/* This structure contains the information OVAL is asking or requesting */
struct server_info {
	SEXP_t *protocol_ent;
	SEXP_t *local_address_ent;
	SEXP_t *local_port_ent;
};

/* Convenience structure for the results being reported */
struct result_info {
	const char *proto;
	const char *laddr;
	unsigned lport;
	const char *raddr;
	unsigned rport;
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

/* Local data */
static int perm_warn = 0;
static struct server_info req;

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
		int fd, len, euid;

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
		if (sf == NULL)
			euid = 0;
		else {
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
			char line[256], ln[256], *s, *e;
			unsigned long inode;
			lnode node;

			if (ent->d_name[0] == '.')
				continue;
			snprintf(ln, 256, "%s/%s", buf, ent->d_name);
			if (readlink(ln, line, 255) < 0)
				continue;

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

static int eval_data(const char *type, const char *local_address,
	unsigned int local_port)
{
	SEXP_t *r0;

	r0 = SEXP_string_newf("%s", type);
	if (probe_entobj_cmp(req.protocol_ent, r0) != OVAL_RESULT_TRUE) {
		SEXP_free(r0);
		return 0;
	}
	SEXP_free(r0);

	r0 = SEXP_string_newf("%s", local_address);
	if (probe_entobj_cmp(req.local_address_ent, r0) != OVAL_RESULT_TRUE) {
		SEXP_free(r0);
		return 0;
	}
	SEXP_free(r0);

	r0 = SEXP_string_newf("%u", local_port);
	if (probe_entobj_cmp(req.local_port_ent, r0) != OVAL_RESULT_TRUE) {
		SEXP_free(r0);
		return 0;
	}
	SEXP_free(r0);

	return 1;
}

static void report_finding(struct result_info *res, llist *l, SEXP_t *probe_out)
{
	SEXP_t *r0, *r1, *r2, *r3, *r4, *r5, *r6, *r7, *r8, *r9, *item_sexp;
	lnode *n = list_get_cur(l);

	item_sexp = probe_item_creat("inetlisteningservers_item", NULL,
		/* entities */
		"protocol", NULL, r0 = SEXP_string_newf("%s", res->proto),
		"local_address", NULL, r1 = SEXP_string_newf("%s",
						res->laddr),
		"local_port", NULL, r2 = SEXP_string_newf("%u", res->lport),
		"local_full_address", NULL, r3 = SEXP_string_newf("%s:%u",
						res->laddr, res->lport),
		"program_name", NULL, r4 = SEXP_string_newf("%s", n->cmd),
		"foreign_address", NULL, r5 = SEXP_string_newf("%s",
						res->raddr),
		"foreign_port", NULL, r6 = SEXP_string_newf("%u", res->rport),
		"foreign_full_address", NULL, r7 = SEXP_string_newf("%s:%u",
						res->raddr, res->rport),
		"pid", NULL, r8 = SEXP_number_newi(n->pid),
		"user_id", NULL, r9 = SEXP_string_newf("%u", n->uid),
		NULL);
	SEXP_vfree(r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, NULL);
	SEXP_list_add(probe_out, item_sexp);
	SEXP_free(item_sexp);
}

static void addr_convert(const char *src, char *dest, int size)
{
	if (strlen(src) > 8) {
		struct in6_addr in6;
		sscanf(src, "%08X%08X%08X%08X",
			&in6.s6_addr32[0], &in6.s6_addr32[1],
			&in6.s6_addr32[2], &in6.s6_addr32[3]);
		inet_ntop(AF_INET6, &in6, dest, size);
	} else {
		int localaddr;
		sscanf(src, "%X",&localaddr);
		inet_ntop(AF_INET, &localaddr, dest, size);
	}
}


static int read_tcp(const char *proc, const char *type, llist *l,
		SEXP_t *probe_out)
{
	int line = 0;
	FILE *f;
	char buf[256];
	unsigned long rxq, txq, time_len, retr, inode;
	unsigned local_port, rem_port, uid;
	int d, state, timer_run, timeout;
	char rem_addr[128], local_addr[128], more[512];

	f = fopen(proc, "rt");
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
		more[0] = 0;
		sscanf(buf, "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X "
			"%lX:%lX %X:%lX %lX %d %d %lu %512s\n",
			&d, local_addr, &local_port, rem_addr, &rem_port,
			&state, &txq, &rxq, &timer_run, &time_len, &retr,
			&uid, &timeout, &inode, more);
		if (list_find_inode(l, inode)) {
			char src[NI_MAXHOST], dest[NI_MAXHOST];
			addr_convert(local_addr, src, NI_MAXHOST);
			addr_convert(rem_addr, dest, NI_MAXHOST);
			_D("Have tcp port: %s:%u\n", src, local_port);
			if (eval_data(type, src, local_port)) {
				struct result_info r;
				r.proto = type;
				r.laddr = src;
				r.lport = local_port;
				r.raddr = dest;
				r.rport = rem_port;
				report_finding(&r, l, probe_out);
			}
		}
	}
	fclose(f);
	return 0;
}

static int read_udp(const char *proc, const char *type, llist *l,
		SEXP_t *probe_out)
{
	int line = 0;
	FILE *f;
	char buf[256];
	unsigned long rxq, txq, time_len, retr, inode;
	int local_port, rem_port, d, state, timer_run, uid, timeout;
	char rem_addr[128], local_addr[128], more[512];

	f = fopen(proc, "rt");
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
		more[0] = 0;
		sscanf(buf, "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X "
			"%lX:%lX %X:%lX %lX %d %d %lu %512s\n",
			&d, local_addr, &local_port, rem_addr, &rem_port,
			&state, &txq, &rxq, &timer_run, &time_len, &retr,
			&uid, &timeout, &inode, more);
		if (list_find_inode(l, inode)) {
			char src[NI_MAXHOST], dest[NI_MAXHOST];
			addr_convert(local_addr, src, NI_MAXHOST);
			addr_convert(rem_addr, dest, NI_MAXHOST);
			_D("Have udp port: %s:%u\n", src, local_port);
			if (eval_data(type, src, local_port)) {
				struct result_info r;
				r.proto = type;
				r.laddr = src;
				r.lport = local_port;
				r.raddr = dest;
				r.rport = rem_port;
				report_finding(&r, l, probe_out);
			}
		}
	}
	fclose(f);
	return 0;
}

static int read_raw(const char *proc, const char *type, llist *l,
		SEXP_t *probe_out)
{
	int line = 0;
	FILE *f;
	char buf[256];
	unsigned long rxq, txq, time_len, retr, inode;
	int local_port, rem_port, d, state, timer_run, uid, timeout;
	char rem_addr[128], local_addr[128], more[512];

	f = fopen(proc, "rt");
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
		more[0] = 0;
		sscanf(buf, "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X "
			"%lX:%lX %X:%lX %lX %d %d %lu %512s\n",
			&d, local_addr, &local_port, rem_addr, &rem_port,
			&state, &txq, &rxq, &timer_run, &time_len, &retr,
			&uid, &timeout, &inode, more);
		if (list_find_inode(l, inode)) {
			char src[NI_MAXHOST], dest[NI_MAXHOST];
			addr_convert(local_addr, src, NI_MAXHOST);
			addr_convert(rem_addr, dest, NI_MAXHOST);
			_D("Have raw port: %s:%u\n", src, local_port);
			if (eval_data(type, src, local_port)) {
				struct result_info r;
				r.proto = type;
				r.laddr = src;
				r.lport = local_port;
				r.raddr = dest;
				r.rport = rem_port;
				report_finding(&r, l, probe_out);
			}
		}
	}
	fclose(f);
	return 0;
}

SEXP_t *probe_main(SEXP_t *object, int *err, void *arg)
{
	llist ll;
	SEXP_t *probe_out = NULL, *item_sexp;

	req.protocol_ent = probe_obj_getent(object, "protocol", 1);
	if (req.protocol_ent == NULL) {
		*err = PROBE_ENOVAL;
		goto cleanup;
	}

	req.local_address_ent = probe_obj_getent(object, "local_address", 1);
	if (req.local_address_ent == NULL) {
		*err = PROBE_ENOVAL;
		goto cleanup;
	}

	req.local_port_ent = probe_obj_getent(object, "local_port", 1);
	if (req.local_port_ent == NULL) {
		*err = PROBE_ENOVAL;
		goto cleanup;
	}

	probe_out = SEXP_list_new(NULL);

	// Now start collecting the info
	list_create(&ll);
	if (collect_process_info(&ll) || perm_warn) {
		_D("Permission error\n");
		/* We had a bad day... */
		SEXP_t *r1, *r2, *r3;

		r1 = probe_ent_getval(req.protocol_ent);
		r2 = probe_ent_getval(req.local_address_ent);
		r3 = probe_ent_getval(req.local_port_ent);
		item_sexp = probe_item_creat("inetlisteningservers_item", NULL,
			"protocol", NULL, r1,
			"local_address", NULL, r2,
			"local_port", NULL, r3,
			"local_full_address", NULL, NULL,
			"program_name", NULL, NULL,
			"foreign_address", NULL, NULL,
			"foreign_port", NULL, NULL,
			"foreign_full_address", NULL, NULL,
			"pid", NULL, NULL,
			"user_id", NULL, NULL,
			NULL);
		probe_item_setstatus(item_sexp, OVAL_STATUS_ERROR);
		SEXP_list_add(probe_out, item_sexp);
		SEXP_free(item_sexp);
		SEXP_vfree(r1, r2, r3, NULL);
	}

	// Now we check the tcp socket list...
	read_tcp("/proc/net/tcp", "tcp", &ll, probe_out);
	read_tcp("/proc/net/tcp6", "tcp", &ll, probe_out);

	// Next udp sockets...
	read_udp("/proc/net/udp", "udp", &ll, probe_out);
	read_udp("/proc/net/udp6", "udp", &ll, probe_out);

	// Next, raw sockets...not exactly part of standard yet. They
	// can be used to send datagrams, so we will pretend they are udp
	read_raw("/proc/net/raw", "udp", &ll, probe_out);
	read_raw("/proc/net/raw6", "udp", &ll, probe_out);

	list_clear(&ll);

	*err = 0;
 cleanup:
	SEXP_vfree(req.protocol_ent, req.local_address_ent, req.local_port_ent, NULL);

	return probe_out;
}

