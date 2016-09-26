/**
 * @file   runlevel.c
 * @brief  runlevel probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 * @author "Tomas Heinrich" <theinric@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process a runlevel_object as defined in OVAL 5.4 and 5.5.
 *
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

/*
 * runlevel probe:
 *
 * runlevel_object(string service_name,
 *                 string runlevel)
 *
 * runlevel_state(string service_name,
 *                string runlevel,
 *                bool   start,
 *                bool   kill)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _BSD_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

#include <seap.h>
#include <probe-api.h>
#include <probe/probe.h>
#include <probe/entcmp.h>
#include <probe/option.h>
#include <alloc.h>
#include "common/debug_priv.h"

struct runlevel_req {
        SEXP_t *service_name_ent;
        SEXP_t *runlevel_ent;
};

struct runlevel_rep {
	char *service_name;
	char *runlevel;
	bool start;
	bool kill;
	struct runlevel_rep *next;
};

static int get_runlevel (struct runlevel_req *req, struct runlevel_rep **rep);

#if defined(__linux__) || defined(__GLIBC__) || (defined(__SVR4) && defined(__sun))
static int get_runlevel_sysv (struct runlevel_req *req, struct runlevel_rep **rep, bool suse, const char *init_path, const char *rc_path)
{
	const char runlevel_list[] = {'0', '1', '2', '3', '4', '5', '6'};

	char pathbuf[PATH_MAX];
	DIR *init_dir, *rc_dir;
	struct dirent *init_dp, *rc_dp;
	struct stat init_st, rc_st;
	struct runlevel_rep *rep_lst = NULL;

	_A(req != NULL);
	_A(rep != NULL);

	init_dir = opendir(init_path);
	if (init_dir == NULL) {
		dI("Can't open directory \"%s\": errno=%d, %s.",
		   init_path, errno, strerror (errno));
		return (-1);
	}

	while ((init_dp = readdir(init_dir)) != NULL) {
		char *service_name;
		unsigned int i;
		SEXP_t *r0;

		// Ensure that we are in the expected directory before
		// touching relative paths
		if (fchdir(dirfd(init_dir)) != 0) {
			dI("Can't fchdir to \"%s\": errno=%d, %s.",
			   init_path, errno, strerror (errno));
			closedir(init_dir);
			return -1;
		}

		if (stat(init_dp->d_name, &init_st) != 0) {
			dI("Can't stat file %s/%s: errno=%d, %s.",
			   init_path, init_dp->d_name, errno, strerror(errno));
			continue;
		}

		r0 = SEXP_string_newf("%s", init_dp->d_name);
		if (probe_entobj_cmp(req->service_name_ent, r0) != OVAL_RESULT_TRUE) {
			SEXP_free(r0);
			continue;
		}
		SEXP_free(r0);
		service_name = init_dp->d_name;

		for (i = 0; i < (sizeof (runlevel_list) / sizeof (runlevel_list[0])); ++i) {
			char runlevel[2] = {'\0', '\0'};
			bool start, kill;

			r0 = SEXP_string_newf("%c", runlevel_list[i]);
			if (probe_entobj_cmp(req->runlevel_ent, r0) != OVAL_RESULT_TRUE) {
				SEXP_free(r0);
				continue;
			}
			SEXP_free(r0);
			runlevel[0] = runlevel_list[i];

			snprintf(pathbuf, sizeof (pathbuf), rc_path, runlevel_list[i]);
			rc_dir = opendir(pathbuf);
			if (rc_dir == NULL) {
				dI("Can't open directory \"%s\": errno=%d, %s.",
				   rc_path, errno, strerror (errno));
				continue;
			}
			if (chdir(pathbuf) != 0) {
				dI("Can't fchdir to \"%s\": errno=%d, %s.",
				   rc_path, errno, strerror (errno));
				closedir(rc_dir);
				continue;
			}

			// On SUSE, the presence of a symbolic link to the init.d/<service> in 
			// a runlevel directory rcx.d implies that the sevice is started on x.
			
			if (suse) {			
				start = false;
				kill = true;
			}
			else
				start = kill = false;

			while ((rc_dp = readdir(rc_dir)) != NULL) {
				if (stat(rc_dp->d_name, &rc_st) != 0) {
					dI("Can't stat file %s/%s: errno=%d, %s.",
					   rc_path, rc_dp->d_name, errno, strerror(errno));
					continue;
				}

				if (init_st.st_ino == rc_st.st_ino) {

					if (suse) {
						if (rc_dp->d_name[0] == 'S') {
						
							start = true;
							kill = false;

							break;
						}
					}						
					else {
						if (rc_dp->d_name[0] == 'S') {
							start = true;
							break;
						} else if (rc_dp->d_name[0] == 'K') {
							kill = true;
							break;
						} else {
							dI("Unexpected character in filename: %c, %s/%s.",
							   rc_dp->d_name[0], pathbuf, rc_dp->d_name);
						}
					}
				}
			}
			closedir(rc_dir);

			if (rep_lst == NULL) {
				rep_lst = *rep = oscap_alloc(sizeof (struct runlevel_rep));
			} else {
				rep_lst->next = oscap_alloc(sizeof (struct runlevel_rep));
				rep_lst = rep_lst->next;
			}

			rep_lst->service_name = strdup(service_name);
			rep_lst->runlevel = strdup(runlevel);
			rep_lst->start = start;
			rep_lst->kill = kill;
			rep_lst->next = NULL;
		}
	}
	closedir(init_dir);

	return (1);
}

static int get_runlevel_redhat (struct runlevel_req *req, struct runlevel_rep **rep)
{
#if defined(__linux__) || defined(__GLIBC__)
	const char *init_path = "/etc/rc.d/init.d";
#elif defined(__SVR4) && defined(__sun)
	const char *init_path = "/etc/init.d";
#endif
	const char *rc_path = "/etc/rc%c.d";

	bool suse = false;
	return (get_runlevel_sysv (req, rep, suse, init_path, rc_path));
}

static int get_runlevel_debian (struct runlevel_req *req, struct runlevel_rep **rep)
{
        return (-1);
}

static int get_runlevel_slack (struct runlevel_req *req, struct runlevel_rep **rep)
{
        return (-1);
}

static int get_runlevel_gentoo (struct runlevel_req *req, struct runlevel_rep **rep)
{
        return (-1);
}

static int get_runlevel_arch (struct runlevel_req *req, struct runlevel_rep **rep)
{
        return (-1);
}

static int get_runlevel_mandriva (struct runlevel_req *req, struct runlevel_rep **rep)
{
        return (-1);
}

static int get_runlevel_suse (struct runlevel_req *req, struct runlevel_rep **rep)
{
	const char *init_path = "/etc/init.d";
	const char *rc_path = "/etc/init.d/rc%c.d";

	bool suse = true;
	return (get_runlevel_sysv (req, rep, suse, init_path, rc_path));
}

static int get_runlevel_wrlinux (struct runlevel_req *req, struct runlevel_rep **rep)
{
        return (-1);
}

static int get_runlevel_amznlinux (struct runlevel_req *req, struct runlevel_rep **rep)
{
	return (-1);
}

static int get_runlevel_common (struct runlevel_req *req, struct runlevel_rep **rep)
{
        return (-1);
}

#if !defined(LINUX_DISTRO)
# define LINUX_DISTRO generic
static int is_redhat (void)
{
        struct stat st;
        return (stat ("/etc/redhat-release", &st) == 0);
}

static int is_debian (void)
{
        struct stat st;
        return (stat ("/etc/debian_version", &st) == 0 ||
                stat ("/etc/debian_release", &st) == 0);
}

static int is_slack (void)
{
        struct stat st;
        return (stat ("/etc/slackware-release", &st) == 0);
}

static int is_gentoo (void)
{
        struct stat st;
        return (stat ("/etc/gentoo-release", &st) == 0);
}

static int is_arch (void)
{
        struct stat st;
        return (stat ("/etc/arch-release", &st) == 0);
}

static int is_mandriva (void)
{
        struct stat st;
        return (stat ("/etc/mandriva-release", &st) == 0);
}

static int is_suse (void)
{
        struct stat st;
        return (stat ("/etc/SuSE-release", &st)   == 0 ||
                stat ("/etc/sles-release", &st)   == 0 ||
                stat ("/etc/novell-release", &st) == 0);
}

static int is_solaris (void)
{
        struct stat st;
        return (stat ("/etc/release", &st)   == 0);
}

static int is_wrlinux (void)
{
        struct stat st;
        return (stat ("/etc/wrlinux-release", &st) == 0 );
}

static int is_amznlinux (void)
{
	struct stat st;
	return (stat ("/etc/os-release", &st) == 0);
}

static int is_common (void)
{
        return (1);
}

typedef struct {
        int (*distrop)(void);
        int (*get_runlevel)(struct runlevel_req *, struct runlevel_rep **);
} distro_tbl_t;

const distro_tbl_t distro_tbl[] = {
        { &is_debian,    &get_runlevel_debian   },
        { &is_redhat,    &get_runlevel_redhat   },
        { &is_slack,     &get_runlevel_slack    },
        { &is_gentoo,    &get_runlevel_gentoo   },
        { &is_arch,      &get_runlevel_arch     },
        { &is_mandriva,  &get_runlevel_mandriva },
        { &is_suse,      &get_runlevel_suse     },
        { &is_solaris,   &get_runlevel_redhat   },
        { &is_wrlinux,   &get_runlevel_wrlinux  },
        { &is_amznlinux, &get_runlevel_amznlinux},
        { &is_common,    &get_runlevel_common   }
};

#define DISTRO_TBL_SIZE ((sizeof distro_tbl)/sizeof (distro_tbl_t))

static int get_runlevel_generic (struct runlevel_req *req, struct runlevel_rep **rep)
{
        uint16_t i;

        _A(req != NULL);
        _A(rep != NULL);

        for (i = 0; i < DISTRO_TBL_SIZE; ++i)
                if (distro_tbl[i].distrop ())
                        return distro_tbl[i].get_runlevel (req, rep);

        abort ();

        /* NOTREACHED */
        return (-1);
}
#endif

#define CONCAT(a, b) a ## b
#define GET_RUNLEVEL(d, q, p) CONCAT(get_runlevel_, d) (q, p)

static int get_runlevel (struct runlevel_req *req, struct runlevel_rep **rep)
{
        _A(req != NULL);
        _A(rep != NULL);
        return GET_RUNLEVEL(LINUX_DISTRO, req, rep);
}
#elif defined(__FreeBSD__)
static int get_runlevel (struct runlevel_req *req, struct runlevel_rep **rep)
{
        _A(req != NULL);
        _A(rep != NULL);
        return (-1);
}
#else
# error "Sorry, your OS isn't supported."
#endif

void *probe_init(void)
{
  probe_setoption(PROBEOPT_OFFLINE_MODE_SUPPORTED, PROBE_OFFLINE_CHROOT);
  return NULL;
}

int probe_main (probe_ctx *ctx, void *arg)
{
        SEXP_t *object;
        struct runlevel_req request_st;
        struct runlevel_rep *reply_st = NULL;

        object = probe_ctx_getobject(ctx);

	request_st.service_name_ent = probe_obj_getent(object, "service_name", 1);
	if (request_st.service_name_ent == NULL) {
		dI("%s: element not found", "service_name");

		return PROBE_ENOELM;
	}

	request_st.runlevel_ent = probe_obj_getent(object, "runlevel", 1);
	if (request_st.runlevel_ent == NULL) {
		SEXP_free(request_st.service_name_ent);
		dI("%s: element not found", "runlevel");

		return PROBE_ENOELM;
	}

	if (get_runlevel(&request_st, &reply_st) == -1) {
		SEXP_t *msg;

		msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, "get_runlevel failed.");
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
	} else {
		struct runlevel_rep *next_rep;
		SEXP_t *item;

		while (reply_st != NULL) {
			dI("get_runlevel: [0]=\"%s\", [1]=\"%s\", [2]=\"%d\", [3]=\"%d\"",
			   reply_st->service_name, reply_st->runlevel, reply_st->start, reply_st->kill);

                        item = probe_item_create(OVAL_UNIX_RUNLEVEL, NULL,
                                                 "service_name", OVAL_DATATYPE_STRING,  reply_st->service_name,
                                                 "runlevel",     OVAL_DATATYPE_STRING,  reply_st->runlevel,
                                                 "start",        OVAL_DATATYPE_BOOLEAN, reply_st->start,
                                                 "kill",         OVAL_DATATYPE_BOOLEAN, reply_st->kill,
                                                 NULL);

                        probe_item_collect(ctx, item);

			next_rep = reply_st->next;
			oscap_free(reply_st->service_name);
			oscap_free(reply_st->runlevel);
			oscap_free(reply_st);
			reply_st = next_rep;
		}
        }

        SEXP_free(request_st.runlevel_ent);
        SEXP_free(request_st.service_name_ent);

	return 0;
}
