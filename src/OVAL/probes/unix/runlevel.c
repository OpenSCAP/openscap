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

#define _BSD_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <seap.h>
#include <probe-api.h>
#include <probe-entcmp.h>
#include <alloc.h>

#ifndef _A
#define _A(x) assert(x)
#endif

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

#if defined(__linux__)
static int get_runlevel_redhat (struct runlevel_req *req, struct runlevel_rep **rep)
{
	const char runlevel_list[] = {'0', '1', '2', '3', '4', '5', '6'};
	const char *init_path = "/etc/rc.d/init.d";
	const char *rc_path = "/etc/rc%c.d";
        char pathbuf[PATH_MAX];
        DIR *init_dir, *rc_dir;
        struct dirent *init_dp, *rc_dp;
	struct stat init_st, rc_st;
	struct runlevel_rep *rep_lst = NULL;

        _A(req != NULL);
        _A(rep != NULL);

        init_dir = opendir(init_path);
        if (init_dir == NULL) {
                _D("Can't open directory \"%s\": errno=%d, %s.\n",
                   init_path, errno, strerror (errno));
                return (-1);
        }
        if (fchdir(dirfd(init_dir)) != 0) {
                _D("Can't fchdir to \"%s\": errno=%d, %s.\n",
                   init_path, errno, strerror (errno));
                closedir(init_dir);
                return (-1);
        }

        while ((init_dp = readdir(init_dir)) != NULL) {
		char *service_name;
		int i;
		SEXP_t *r0;

                if (stat(init_dp->d_name, &init_st) != 0) {
                        _D("Can't stat file %s/%s: errno=%d, %s.\n",
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
				_D("Can't open directory \"%s\": errno=%d, %s.\n",
				   rc_path, errno, strerror (errno));
				continue;
			}
			if (fchdir(dirfd(rc_dir)) != 0) {
				_D("Can't fchdir to \"%s\": errno=%d, %s.\n",
				   rc_path, errno, strerror (errno));
				closedir(rc_dir);
				continue;
			}

			start = kill = false;

			while ((rc_dp = readdir(rc_dir)) != NULL) {
				if (stat(rc_dp->d_name, &rc_st) != 0) {
					_D("Can't stat file %s/%s: errno=%d, %s.\n",
					   rc_path, rc_dp->d_name, errno, strerror(errno));
					continue;
				}

				if (init_st.st_ino == rc_st.st_ino) {
					if (rc_dp->d_name[0] == 'S') {
						start = true;
						break;
					} else if (rc_dp->d_name[0] == 'K') {
						kill = true;
						break;
					} else {
						_D("Unexpected character in filename: %c, %s/%s.\n",
						   rc_dp->d_name[0], pathbuf, rc_dp->d_name);
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

static int is_common (void)
{
        return (1);
}

typedef struct {
        int (*distrop)(void);
        int (*get_runlevel)(struct runlevel_req *, struct runlevel_rep **);
} distro_tbl_t;

const distro_tbl_t distro_tbl[] = {
        { &is_debian,   &get_runlevel_debian   },
        { &is_redhat,   &get_runlevel_redhat   },
        { &is_slack,    &get_runlevel_slack    },
        { &is_gentoo,   &get_runlevel_gentoo   },
        { &is_arch,     &get_runlevel_arch     },
        { &is_mandriva, &get_runlevel_mandriva },
        { &is_suse,     &get_runlevel_suse     },
        { &is_common,   &get_runlevel_common   }
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

SEXP_t *probe_main (SEXP_t *object, int *err, void *arg)
{
        SEXP_t *probe_out;

        struct runlevel_req request_st;
        struct runlevel_rep *reply_st = NULL;

	request_st.service_name_ent = probe_obj_getent(object, "service_name", 1);
	if (request_st.service_name_ent == NULL) {
		_D("%s: element not found\n", "service_name");
		*err = PROBE_ENOELM;

		return NULL;
	}

	request_st.runlevel_ent = probe_obj_getent(object, "runlevel", 1);
	if (request_st.runlevel_ent == NULL) {
		SEXP_free(request_st.service_name_ent);
		_D("%s: element not found\n", "runlevel");
		*err = PROBE_ENOELM;

		return NULL;
	}

	if (get_runlevel(&request_st, &reply_st) == -1) {
		SEXP_t *item_sexp;

                _D("get_runlevel failed\n");

                item_sexp = probe_item_creat("runlevel_item", NULL, NULL);
                probe_obj_setstatus(item_sexp, OVAL_STATUS_ERROR);
		probe_out = SEXP_list_new(item_sexp, NULL);
		SEXP_free(item_sexp);
	} else {
		struct runlevel_rep *next_rep;
		SEXP_t *item_sexp;

		probe_out = SEXP_list_new(NULL);

		while (reply_st != NULL) {
			SEXP_t *r0, *r1, *r2, *r3;

			_D("get_runlevel: [0]=\"%s\", [1]=\"%s\", [2]=\"%d\", [3]=\"%d\"\n",
			   reply_st->service_name, reply_st->runlevel, reply_st->start, reply_st->kill);

			item_sexp = probe_item_creat("runlevel_item", NULL,
						     /* entities */
						     "service_name", NULL,
						     r0 = SEXP_string_newf("%s", reply_st->service_name),
						     "runlevel", NULL,
						     r1 = SEXP_string_newf("%s", reply_st->runlevel),
						     "start", NULL,
						     r2 = SEXP_number_newb(reply_st->start),
						     "kill", NULL,
						     r3 = SEXP_number_newb(reply_st->kill),
						     NULL);
			SEXP_list_add(probe_out, item_sexp);
			SEXP_vfree(r0, r1, r2, r3, item_sexp, NULL);

			next_rep = reply_st->next;
			oscap_free(reply_st->service_name);
			oscap_free(reply_st->runlevel);
			oscap_free(reply_st);
			reply_st = next_rep;
		}
        }

        *err = 0;

        return (probe_out);
}
