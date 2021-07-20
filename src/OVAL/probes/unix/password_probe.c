/**
 * @file   password_probe.c
 * @brief  password probe
 * @author "Steve Grubb" <sgrubb@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process a password_object as defined in OVAL 5.4 and 5.5.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *   Steve Grubb <sgrubb@redhat.com>
 */

/*
 * password probe:
 *
 * username
 * password
 * user_id
 * group_id
 * gcos
 * home_dir
 * login_shell
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include <paths.h>
#if defined(OS_APPLE)
#include <utmp.h>
#elif defined(OS_FREEBSD)
#include <utmpx.h>
#else
#include <lastlog.h>
#endif

#include "_seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "common/debug_priv.h"
#include <probe/probe.h>
#include <probe/option.h>
#include "password_probe.h"

/* Convenience structure for the results being reported */
struct result_info {
        const char *username;
        const char *password;
        uid_t user_id;
        gid_t group_id;
        const char *gcos;
        const char *home_dir;
        const char *login_shell;
	int64_t last_login;
};

static void report_finding(struct result_info *res, probe_ctx *ctx, oval_schema_version_t over)
{
        SEXP_t *item;

        item = probe_item_create(OVAL_UNIX_PASSWORD, NULL,
                                 "username",    OVAL_DATATYPE_STRING, res->username,
                                 "password",    OVAL_DATATYPE_STRING, res->password,
                                 "user_id",     OVAL_DATATYPE_INTEGER, (int64_t)res->user_id,
                                 "group_id",    OVAL_DATATYPE_INTEGER, (int64_t)res->group_id,
                                 "gcos",        OVAL_DATATYPE_STRING, res->gcos,
                                 "home_dir",    OVAL_DATATYPE_STRING, res->home_dir,
                                 "login_shell", OVAL_DATATYPE_STRING, res->login_shell,
                                 NULL);

        if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.10)) >= 0) {
	        SEXP_t last_login;
	        SEXP_number_newi_64_r(&last_login, res->last_login);
	        probe_item_ent_add(item, "last_login", NULL, &last_login);
	        SEXP_free_r(&last_login);
        }

        probe_item_collect(ctx, item);
}

#if defined(OS_FREEBSD)
static time_t get_last_login(char *username) {
        struct utmpx *ut;
        time_t t = 0;

        /* Iterate over the entries of the utx.log file */
        while ((ut = getutxent()) != NULL) {
                if (strcmp(username,ut->ut_user) == 0) {
                        t = ut->ut_tv.tv_sec;
                        break;
                }
        }

        endutxent();

        return t;
}
#endif

static void _process_struct_passwd(struct passwd *pw, const char *lastlog, SEXP_t *un_ent, probe_ctx *ctx, oval_schema_version_t over)
{
	SEXP_t *un;
	struct result_info r;

	dI("Have user: %s", pw->pw_name);
	un = SEXP_string_newf("%s", pw->pw_name);
	if (probe_entobj_cmp(un_ent, un) != OVAL_RESULT_TRUE) {
		SEXP_free(un);
		return;
	}

	r.username = pw->pw_name;
	r.password = pw->pw_passwd;
	r.user_id = pw->pw_uid;
	r.group_id = pw->pw_gid;
	r.gcos = pw->pw_gecos;
	r.home_dir = pw->pw_dir;
	r.login_shell = pw->pw_shell;
	r.last_login = -1;

	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.10)) >= 0) {
#if defined(OS_FREEBSD)
		r.last_login = get_last_login(pw->pw_name);
#else
		FILE *ll_fp = fopen(lastlog, "r");

		if (ll_fp != NULL) {
			struct lastlog ll;

			if (fseeko(ll_fp, (off_t)pw->pw_uid * sizeof(ll), SEEK_SET) == 0)
				if (fread((char *)&ll, sizeof(ll), 1, ll_fp) == 1)
					r.last_login = (int64_t)ll.ll_time;
			fclose(ll_fp);
		}
#endif
	}

	report_finding(&r, ctx, over);
	SEXP_free(un);
}

static int read_password(SEXP_t *un_ent, probe_ctx *ctx, oval_schema_version_t over)
{
        struct passwd *pw;

	if (ctx->offline_mode & PROBE_OFFLINE_OWN) {
		const char *root = getenv("OSCAP_PROBE_ROOT");
		if (root == NULL)
			return 1;
		char *passwd_file_path = oscap_path_join(root, "/etc/passwd");
		char *lastlog_file_path = oscap_path_join(root, _PATH_LASTLOG);
		FILE *fp = fopen(passwd_file_path, "r");
		if (fp == NULL) {
			free(passwd_file_path);
			free(lastlog_file_path);
			return 1;
		}
		while ((pw = fgetpwent(fp))) {
			_process_struct_passwd(pw, lastlog_file_path, un_ent, ctx, over);
		}
		fclose(fp);
		free(passwd_file_path);
		free(lastlog_file_path);
	} else {
		while ((pw = getpwent())) {
			_process_struct_passwd(pw, _PATH_LASTLOG, un_ent, ctx, over);
		}
		endpwent();
	}

        return 0;
}

int password_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_OWN;
}

int password_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *ent, *obj;
	oval_schema_version_t over;

	obj = probe_ctx_getobject(ctx);

	if (obj == NULL)
		return PROBE_ENOOBJ;

	over = probe_obj_get_platform_schema_version(obj);
        ent = probe_obj_getent(obj, "username", 1);

        if (ent == NULL) {
                return PROBE_ENOVAL;
        }

        // Now we check the file...
	if (read_password(ent, ctx, over) != 0) {
		SEXP_free(ent);
		return PROBE_EINVAL;
	}
        SEXP_free(ent);

        return 0;
}
