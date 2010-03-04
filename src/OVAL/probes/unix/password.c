/*
* password.c - Check metadata associated with the UNIX password file
* Copyright (c) 2010 Red Hat Inc., Durham, North Carolina.
* All Rights Reserved. 
*
* This software may be freely redistributed and/or modified under the
* terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2, or (at your option) any
* later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; see the file COPYING. If not, write to the
* Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

#include "config.h"
#include "seap.h"
#include "probe-api.h"
#include "alloc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <errno.h>
#include <pwd.h>

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

oval_operation_t username_op;
char *username = NULL;

static int option_compare(oval_operation_t op, const char *s1, const char * s2)
{
	if (op == OVAL_OPERATION_EQUALS) {
		if (strcmp(s1, s2) == 0)
			return 1;
		else
			return 0;
	} else if (op == OVAL_OPERATION_NOT_EQUAL) {
		if (strcmp(s1, s2) == 0)
			return 0;
		else
			return 1;
	} else if (op == OVAL_OPERATION_PATTERN_MATCH)
		return 0; // FIXME - not implemented

	// All matched
	return 1;
}

static int eval_data(const char *cur_username)
{
	if (username) {
		if (option_compare(username_op, cur_username, username) == 0)
			return 0;
	}
	return 1;
}

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
	SEXP_list_add(probe_out, item_sexp);
	SEXP_free(item_sexp);
}

static int read_password(SEXP_t *probe_out)
{
	struct passwd *pw;

	while ((pw = getpwent())) {
		_D("Have user: %s\n", pw->pw_name);
		if (eval_data(pw->pw_name)) {
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
	}
	endpwent();
	return 0;
}

SEXP_t *probe_main(SEXP_t *object, int *err, void *arg)
{
	SEXP_t *probe_out, *val = NULL, *ent;

	ent = probe_obj_getent(object, "username", 1);
	if (ent)
		val = probe_ent_getval(ent);
	if (ent == NULL || val == NULL) {
		*err = PROBE_ENOVAL;
		return NULL;
	}
	username = SEXP_string_cstr(val);
	if (username == NULL) {
		_D("username error\n");
		switch (errno) {
			case EINVAL:
				*err = PROBE_EINVAL;
				break;
			case EFAULT:
				*err = PROBE_ENOELM;
				break;
		}
		return NULL;
	}
	SEXP_free(val);
	val = probe_ent_getattrval(ent, "operation");
	if (val == NULL)
		username_op = OVAL_OPERATION_EQUALS;
	else {
		username_op = (oval_operation_t) SEXP_number_geti_32(val);
		switch (username_op) {
			case OVAL_OPERATION_EQUALS:
			case OVAL_OPERATION_NOT_EQUAL:
			case OVAL_OPERATION_PATTERN_MATCH:
				break;
			default:
				*err = PROBE_EOPNOTSUPP;
				SEXP_free(val);
				oscap_free(username);
				return (NULL);
		}
		SEXP_free(val);
	}
	SEXP_free(ent);
	probe_out = SEXP_list_new(NULL);

	// Now we check the file...
	read_password(probe_out);

	*err = 0;
	return probe_out;
}

