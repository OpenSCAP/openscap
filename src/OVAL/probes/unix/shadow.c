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
#include "probe-entcmp.h"
#include "alloc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifndef HAVE_SHADOW_H
SEXP_t *probe_main(SEXP_t *object, int *err, void *arg)
{
        SEXP_t *item_sexp, *probe_out;
        
	item_sexp = probe_item_creat ("shadow_item", NULL, NULL);
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

static void report_finding(struct result_info *res, SEXP_t *probe_out)
{
	SEXP_t *r0, *r1, *r2, *r3, *r4, *r5, *r6, *r7, *r8, *item_sexp;
		
	item_sexp = probe_item_creat("shadow_item", NULL,
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

static int read_shadow(SEXP_t *un_ent, SEXP_t *probe_out)
{
	int err = 1;
	struct spwd *pw;

	while ((pw = getspent())) {
		SEXP_t *un;

		_D("Have user: %s\n", pw->sp_namp);
		err = 0;
		un = SEXP_string_newf("%s", pw->sp_namp);
		if (probe_entobj_cmp(un_ent, un) == OVAL_RESULT_TRUE) {
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
		SEXP_free(un);
	}
	endspent();
	return err;
}

SEXP_t *probe_main(SEXP_t *object, int *err, void *arg)
{
	SEXP_t *probe_out, *ent;

	ent = probe_obj_getent(object, "username", 1);
	if (ent == NULL) {
		*err = PROBE_ENOVAL;
		return NULL;
	}
	probe_out = SEXP_list_new(NULL);
	*err = 0;

	// Now we check the file...
	if (read_shadow(ent, probe_out)) {
		*err = PROBE_EACCES;
		SEXP_free(probe_out);
		probe_out = NULL;
	}

	SEXP_free(ent);

	return probe_out;
}
#endif /* HAVE_SHADOW_H */
