/**
 * @file   shadow.c
 * @brief  shadow probe
 * @author "Steve Grubb" <sgrubb@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process a shadow_object as defined in OVAL 5.4 and 5.5.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "alloc.h"

#ifndef HAVE_SHADOW_H
int probe_main(SEXP_t *object, SEXP_t *probe_out, void *arg)
{
        SEXP_t *item_sexp;

	if (object == NULL || probe_out == NULL) {
		return (PROBE_EINVAL);
	}

	item_sexp = probe_item_creat ("shadow_item", NULL, NULL);
        probe_item_setstatus (item_sexp, OVAL_STATUS_NOTCOLLECTED);
	probe_cobj_add_item(probe_out, item_sexp);
        SEXP_free (item_sexp);

        return 0;
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
        SEXP_t *item;
        SEXP_t se_chl_mem, se_cha_mem, se_chr_mem;
        SEXP_t se_exw_mem, se_exi_mem, se_exd_mem;
        SEXP_t se_flg_mem;

        item = probe_item_create(OVAL_UNIX_SHADOW, NULL,
                                 "username",  OVAL_DATATYPE_STRING, res->username,
                                 "password",  OVAL_DATATYPE_STRING, res->password,
                                 "chg_lst",   OVAL_DATATYPE_SEXP, SEXP_string_newf_r(&se_chl_mem, "%li", res->chg_lst),
                                 "chg_allow", OVAL_DATATYPE_SEXP, SEXP_string_newf_r(&se_cha_mem, "%li", res->chg_allow),
                                 "chg_req",   OVAL_DATATYPE_SEXP, SEXP_string_newf_r(&se_chr_mem, "%li", res->chg_req),
                                 "exp_warn",  OVAL_DATATYPE_SEXP, SEXP_string_newf_r(&se_exw_mem, "%li", res->exp_warn),
                                 "exp_inact", OVAL_DATATYPE_SEXP, SEXP_string_newf_r(&se_exi_mem, "%li", res->exp_inact),
                                 "exp_date",  OVAL_DATATYPE_SEXP, SEXP_string_newf_r(&se_exd_mem, "%li", res->exp_date),
                                 "flag",      OVAL_DATATYPE_SEXP, SEXP_string_newf_r(&se_flg_mem, "%lu", res->flag),
                                 NULL);

	probe_cobj_add_item(probe_out, item);
	SEXP_free(item);
        SEXP_free_r(&se_chl_mem);
        SEXP_free_r(&se_cha_mem);
        SEXP_free_r(&se_chr_mem);
        SEXP_free_r(&se_exw_mem);
        SEXP_free_r(&se_exi_mem);
        SEXP_free_r(&se_exd_mem);
        SEXP_free_r(&se_flg_mem);
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

int probe_main(SEXP_t *object, SEXP_t *probe_out, void *arg, SEXP_t *filters)
{
	SEXP_t *ent;

        (void)filters;

	if (object == NULL || probe_out == NULL) {
		return (PROBE_EINVAL);
	}

	ent = probe_obj_getent(object, "username", 1);
	if (ent == NULL) {
		return PROBE_ENOVAL;
	}

	// Now we check the file...
	read_shadow(ent, probe_out);
	SEXP_free(ent);

	return 0;
}
#endif /* HAVE_SHADOW_H */
