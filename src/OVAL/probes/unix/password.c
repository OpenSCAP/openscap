/**
 * @file   password.c
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#include "seap.h"
#include "probe-api.h"
#include "probe-entcmp.h"
#include "alloc.h"

/* Convenience structure for the results being reported */
struct result_info {
        const char *username;
        const char *password;
        unsigned int user_id;
        unsigned int group_id;
        const char *gcos;
	const char *home_dir;
	const char *login_shell;
};

static void report_finding(struct result_info *res, SEXP_t *probe_out)
{
	SEXP_t *r0, *r1, *r2, *r3, *r4, *r5, *r6, *item_sexp;
		
	item_sexp = probe_obj_creat("password_item", NULL,
		/* entities */
		"username", NULL, r0 = SEXP_string_newf("%s", res->username),
		"password", NULL, r1 = SEXP_string_newf("%s", res->password),
		"user_id", NULL, r2 = SEXP_string_newf("%u", res->user_id),
		"group_id", NULL, r3 = SEXP_string_newf("%u", res->group_id),
		"gcos", NULL, r4 = SEXP_string_newf("%s", res->gcos),
		"home_dir", NULL, r5 = SEXP_string_newf("%s", res->home_dir),
		"login_shell", NULL, r6 = SEXP_string_newf("%s",
							res->login_shell),
		NULL);
	SEXP_vfree(r0, r1, r2, r3, r4, r5, r6, NULL);
	probe_cobj_add_item(probe_out, item_sexp);
	SEXP_free(item_sexp);
}

static int read_password(SEXP_t *un_ent, SEXP_t *probe_out)
{
	struct passwd *pw;

	while ((pw = getpwent())) {
		SEXP_t *un;

		_D("Have user: %s\n", pw->pw_name);
		un = SEXP_string_newf("%s", pw->pw_name);
		if (probe_entobj_cmp(un_ent, un) == OVAL_RESULT_TRUE) {
			struct result_info r;
			r.username = pw->pw_name;
			r.password = pw->pw_passwd;
			r.user_id = pw->pw_uid;
			r.group_id = pw->pw_gid;
			r.gcos = pw->pw_gecos;
			r.home_dir = pw->pw_dir;
			r.login_shell = pw->pw_shell;

			report_finding(&r, probe_out);
		}
		SEXP_free(un);
	}
	endpwent();
	return 0;
}

int probe_main(SEXP_t *object, SEXP_t *probe_out, void *arg)
{
	SEXP_t *ent;

	if (object == NULL || probe_out == NULL) {
		return (PROBE_EINVAL);
	}

	ent = probe_obj_getent(object, "username", 1);
	if (ent == NULL) {
		return PROBE_ENOVAL;
	}

	// Now we check the file...
	read_password(ent, probe_out);
	SEXP_free(ent);

	return 0;
}

