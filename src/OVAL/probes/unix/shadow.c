/*
* shadow.c - Check metadata associated with the UNIX shadow password file
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
 * shadow probe:
 *
 * username
 * password
 * chg_lst
 * chg_allow
 * chg_req
 * exp_warn
 * exp_inact
 * exp_date
 * flag
 */

#include "config.h"
#include "seap.h"
#include "probe-api.h"
#include "alloc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifndef HAVE_SHADOW_H
SEXP_t *probe_main(SEXP_t *object, int *err, void *arg)
{
        SEXP_t *item_sexp, *probe_out;
        
	item_sexp = probe_obj_creat ("shadow_item", NULL, NULL);
        probe_item_setstatus (item_sexp, OVAL_STATUS_NOTCOLLECTED);
        probe_out = SEXP_list_new (item_sexp, NULL);
        SEXP_free (item_sexp);
        
        return (probe_out);
}
#else
/* shadow.h is present */
#include <shadow.h>

/* Convenience structure for the results being reported */
struct result_info {
        const char *username;
        const char *password;
        long chg_lst;
        long chg_allow;
        long chg_req;
        long exp_warn;
        long exp_inact;
        long exp_date;
        unsigned long flag;
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
	}

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
	SEXP_t *r0, *r1, *r2, *r3, *r4, *r5, *r6, *r7, *r8, *item_sexp;
		
	item_sexp = probe_obj_creat("shadow_item", NULL,
		/* entities */
		"username", NULL, r0 = SEXP_string_newf("%s", res->username),
		"password", NULL, r1 = SEXP_string_newf("%s", res->password),
		"chg_lst", NULL, r2 = SEXP_string_newf("%li", res->chg_lst),
		"chg_allow", NULL, r3 = SEXP_string_newf("%li", res->chg_allow),
		"chg_req", NULL, r4 = SEXP_string_newf("%li", res->chg_req),
		"exp_warn", NULL, r5 = SEXP_string_newf("%li", res->exp_warn),
		"exp_inact", NULL, r6 = SEXP_string_newf("%li",res->exp_inact),
		"exp_date", NULL, r7 = SEXP_string_newf("%li", res->exp_date),
		"flag", NULL, r8 = SEXP_string_newf("%lu", res->flag),
		NULL);
	SEXP_vfree(r0, r1, r2, r3, r4, r5, r6, r7, r8, NULL);
	SEXP_list_add(probe_out, item_sexp);
	SEXP_free(item_sexp);
}

static int read_shadow(SEXP_t *probe_out)
{
	int err = 1;
	struct spwd *pw;

	while ((pw = getspent())) {
		_D("Have user: %s\n", pw->sp_namp);
		err = 0;
		if (eval_data(pw->sp_namp)) {
			struct result_info r;

			r.username = pw->sp_namp;
			r.password = pw->sp_pwdp;
			r.chg_lst = pw->sp_lstchg;
			r.chg_allow = pw->sp_min;
			r.chg_req = pw->sp_max;
			r.exp_warn = pw->sp_warn;
			r.exp_inact = pw->sp_inact;
			r.exp_date = pw->sp_expire;
			r.flag = pw->sp_flag;

			report_finding(&r, probe_out);
		}
	}
	endspent();
	return err;
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
	if (read_shadow(probe_out)) {
		*err = PROBE_EACCES;
		SEXP_free(val);
		return NULL;
	}

	*err = 0;
	return probe_out;
}
#endif /* HAVE_SHADOW_H */
