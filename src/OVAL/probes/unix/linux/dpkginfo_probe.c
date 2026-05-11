/**
 * @file   dpkginfo_probe.c
 * @brief  dpkginfo probe
 * @author "Pierre Chifflier" <chifflier@wzdftpd.net>
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
 *      "Pierre Chifflier <chifflier@wzdftpd.net>"
 */


/*
 * dpkginfo probe:
 *
 *  dpkginfo_object(string name)
 *
 *  dpkginfo_state(string name,
 *                string arch,
 *                string epoch,
 *                string release,
 *                string version,
 *                string evr,
 *                string signature_keyid)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

/* SEAP */
#include "_seap.h"
#include <probe-api.h>

#include "common/debug_priv.h"
#include "public/oval_schema_version.h"

#include <probe/probe.h>
#include "probe/entcmp.h"

#include "dpkginfo-helper.h"

#include "dpkginfo_probe.h"

int dpkginfo_probe_offline_mode_supported(void) {
        return PROBE_OFFLINE_OWN;
}

int dpkginfo_probe_main (probe_ctx *ctx, void *arg)
{
	SEXP_t *val, *item, *ent, *obj;
        char *request_st = NULL;
        struct dpkginfo_reply_t *dpkginfo_replies = NULL;
	size_t dpkginfo_reply_count = 0;
        int errflag;
	oval_operation_t operation;

	obj = probe_ctx_getobject(ctx);
	ent = probe_obj_getent(obj, "name", 1);

        if (ent == NULL) {
                return (PROBE_ENOENT);
        }

        val = probe_ent_getval (ent);

        if (val == NULL) {
                dD("%s: no value", "name");
                SEXP_free (ent);
                return (PROBE_ENOVAL);
        }

        request_st = SEXP_string_cstr (val);
        SEXP_free (val);

        if (request_st == NULL) {
                switch (errno) {
                case EINVAL:
                        dD("%s: invalid value type", "name");
			SEXP_free (ent);
			return PROBE_EINVAL;
                        break;
                case EFAULT:
                        dD("%s: element not found", "name");
			SEXP_free (ent);
			return PROBE_ENOELM;
                        break;
		default:
			SEXP_free (ent);
			return PROBE_EUNKNOWN;
                }
        }

	val = probe_ent_getattrval(ent, "operation");
	if (val == NULL) {
		operation = OVAL_OPERATION_EQUALS;
	} else {
		operation = (oval_operation_t) SEXP_number_geti_32(val);
		SEXP_free(val);
	}

	switch (operation) {
	case OVAL_OPERATION_EQUALS:
	case OVAL_OPERATION_NOT_EQUAL:
	case OVAL_OPERATION_PATTERN_MATCH:
		break;
	default:
		SEXP_free(ent);
		free(request_st);
		return PROBE_EOPNOTSUPP;
	}

        /* get info from Debian package status */
        dpkginfo_replies = dpkginfo_get_all(&dpkginfo_reply_count, &errflag);

        if (dpkginfo_replies == NULL) {
                switch (errflag) {
		case 0: /* Not found */
		{
			dD("Package \"%s\" not found.", request_st);
			break;
		}
		case -1: /* Error */
		{
			dD("dpkginfo_get_all failed.");
			item = probe_item_create(OVAL_LINUX_DPKG_INFO, NULL,
					"name", OVAL_DATATYPE_STRING, request_st,
					NULL);
			probe_item_setstatus (item, SYSCHAR_STATUS_ERROR);
			probe_item_collect(ctx, item);
			break;
		}
		}
        } else { /* Ok */
		oval_datatype_t evr_string_type;
		oval_schema_version_t oval_version = probe_obj_get_platform_schema_version(obj);
		if (oval_schema_version_cmp(oval_version, OVAL_SCHEMA_VERSION(5.11.1)) >= 0) {
			evr_string_type = OVAL_DATATYPE_DEBIAN_EVR_STRING;
		} else {
			evr_string_type = OVAL_DATATYPE_EVR_STRING;
		}

                for (size_t i = 0; i < dpkginfo_reply_count; ++i) {
			SEXP_t *name = SEXP_string_newf("%s", dpkginfo_replies[i].name);

			if (probe_entobj_cmp(ent, name) != OVAL_RESULT_TRUE) {
				SEXP_free(name);
				continue;
			}

			struct dpkginfo_reply_t *dpkginfo_reply = dpkginfo_replies + i;
                        dD("%s: element found version %s", dpkginfo_reply->name, dpkginfo_reply->evr);
                        item = probe_item_create (OVAL_LINUX_DPKG_INFO, NULL,
                                        "name", OVAL_DATATYPE_SEXP, name,
                                        "arch", OVAL_DATATYPE_STRING, dpkginfo_reply->arch,
                                        "epoch", OVAL_DATATYPE_STRING, dpkginfo_reply->epoch,
                                        "release", OVAL_DATATYPE_STRING, dpkginfo_reply->release,
                                        "version", OVAL_DATATYPE_STRING, dpkginfo_reply->version,
					"evr", evr_string_type, dpkginfo_reply->evr,
                                        NULL);

			probe_item_collect(ctx, item);
                }
		dpkginfo_free_replies(dpkginfo_replies, dpkginfo_reply_count);
        }

	SEXP_free(ent);
        free(request_st);

        return (0);
}
