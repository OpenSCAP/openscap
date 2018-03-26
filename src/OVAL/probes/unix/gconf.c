/**
 * @file   gconf.c
 * @brief  gconf probe implementation
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 */

/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <probe-api.h>
#include <pcre.h>
#include <gconf/gconf.h>

#include "common/debug_priv.h"
#include "common/assume.h"
#include "oval_fts.h"

#ifndef MTAB_LINE_MAX
# define MTAB_LINE_MAX 4096
#endif

static int collect_item(probe_ctx *ctx, const char *source, GConfEntry *entry)
{
        GConfValue *gconf_value;
        char   *gconf_type;
        SEXP_t *sexp_value, *item;

        gconf_value = gconf_entry_get_value(entry);

        if (gconf_value != NULL) {
                switch(gconf_value->type) {
                case GCONF_VALUE_STRING:
                        gconf_type = "GCONF_VALUE_STRING";
                        sexp_value = SEXP_string_newf("%s", gconf_value_get_string(gconf_value));
                        break;
                case GCONF_VALUE_INT:
                        gconf_type = "GCONF_VALUE_INT";
                        sexp_value = SEXP_number_newi_32(gconf_value_get_int(gconf_value));
                        break;
                case GCONF_VALUE_FLOAT:
                        gconf_type = "GCONF_VALUE_FLOAT";
                        sexp_value = SEXP_number_newf(gconf_value_get_float(gconf_value));
                        break;
                case GCONF_VALUE_BOOL:
                        gconf_type = "GCONF_VALUE_BOOL";
                        sexp_value = SEXP_number_newb(gconf_value_get_bool(gconf_value));
                        break;
                case GCONF_VALUE_INVALID:
                case GCONF_VALUE_SCHEMA:
                case GCONF_VALUE_LIST:
                case GCONF_VALUE_PAIR:
                default:
                        dE("Unsupported GConfValue type: %d", gconf_value->type);
                        gconf_type = "GCONF_VALUE_INVALID";
                        sexp_value = NULL;
                }
        } else {
                gconf_type = "GCONF_VALUE_INVALID";
                sexp_value = NULL;
        }

        item = probe_item_create(OVAL_UNIX_GCONF, NULL,
                                 "key",         OVAL_DATATYPE_STRING,  entry->key,
                                 "source",      OVAL_DATATYPE_STRING,  source,
                                 "type",        OVAL_DATATYPE_STRING,  gconf_type,
                                 "is_writable", OVAL_DATATYPE_BOOLEAN, gconf_entry_get_is_writable(entry),
                                 //"mod_user",    OVAL_DATATYPE_STRING,  "",
                                 //"mod_time",    OVAL_DATATYPE_INTEGER, 0,
                                 "is_default",  OVAL_DATATYPE_BOOLEAN, gconf_entry_get_is_default(entry),
                                 "value",       OVAL_DATATYPE_SEXP, sexp_value,
                                 NULL);

        probe_item_collect(ctx, item);
	SEXP_free(sexp_value);

        return (0);
}

static int collect_item_direct(probe_ctx *ctx, const char *source, GConfEngine *engine, SEXP_t *key)
{
	char gconf_key[1024];
	int ret;
	GError *gconf_err = NULL;
	GConfEntry *gconf_entry;

	PROBE_ENT_STRVAL(key, gconf_key, sizeof gconf_key, return -1;, return 0;);

	gconf_entry = gconf_engine_get_entry(engine, gconf_key,
					     NULL, true, &gconf_err);

	if (gconf_entry != NULL) {
		ret = collect_item(ctx, source, gconf_entry);
		gconf_entry_unref(gconf_entry);
	} else {
		/* TODO: inspect gconf_err */
		ret = 0;
		if (gconf_err != NULL)
			g_error_free(gconf_err);
	}

	return (ret);
}

static int collect_item_regexp(probe_ctx *ctx, const char *source, GConfEngine *engine, SEXP_t *key)
{
        return (0);
}

int probe_main(probe_ctx *ctx, void *probe_arg)
{
        int probe_ret = 0;
        SEXP_t *probe_in, *gconf_src, *gconf_key, *key_opval;
	SEXP_t *gconf_src_val;
        oval_operation_t key_op;
        GConfEngine *gconf_engine = NULL;

        probe_in  = probe_ctx_getobject(ctx);
        if ((gconf_src = probe_obj_getent(probe_in, "source", 1)) == NULL)
		return (PROBE_ENOENT);

        if ((gconf_key = probe_obj_getent(probe_in, "key", 1)) == NULL) {
		SEXP_free(gconf_src);
		return (PROBE_ENOENT);
	}

        key_opval = probe_ent_getattrval(gconf_key, "operation");

        if (key_opval != NULL) {
                key_op = (oval_operation_t)SEXP_number_geti(key_opval);
                SEXP_free(key_opval);
        } else
                key_op = OVAL_OPERATION_EQUALS;

        gconf_src_val = probe_ent_getval(gconf_src);

        if (gconf_src_val != NULL) {
                /*
                 * Check that the value of the `source' entity
                 * is a string.
                 */
                if (!SEXP_stringp(gconf_src_val)) {
                        SEXP_free(gconf_src_val);
                        SEXP_free(gconf_src);
                        SEXP_free(gconf_key);

                        return (PROBE_EINVAL);
                }

		SEXP_free(gconf_src_val);
        } else {
                /*
                 * xsi:nil is set for the source entity so
                 * we wan't to have the gconf daemon as the
                 * source.
                 */
                gconf_engine = gconf_engine_get_default();

		if (gconf_engine == NULL) {
			dE("Unable to get the default GConf engine!");
			SEXP_free(gconf_src);
			SEXP_free(gconf_key);
			/* XXX: construct an error item */
			return (0);
		}
        }

        if (gconf_engine == NULL) {
                OVAL_FTS    *ofts;
                OVAL_FTSENT *ofts_ent;
                char gconf_addr[PATH_MAX];
		GError *gconf_err = NULL;
		SEXP_t *behaviors = NULL;

		probe_filebehaviors_canonicalize(&behaviors);

                if ((ofts = oval_fts_open(NULL, NULL, NULL, gconf_src, behaviors, probe_ctx_getresult(ctx))) != NULL) {
                        while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
                                assume_r(ofts_ent->path_len
                                         + ofts_ent->file_len + 2 <= PATH_MAX, PROBE_EFATAL);

                                strcpy(gconf_addr, ofts_ent->path);
                                gconf_addr[ofts_ent->path_len] = '/';
                                strcpy(gconf_addr + ofts_ent->path_len + 1, ofts_ent->file);

                                dI("GConf source: %s", gconf_addr);

                                gconf_engine = gconf_engine_get_for_address(gconf_addr, &gconf_err);

				if (gconf_engine != NULL) {
					switch(key_op) {
					case OVAL_OPERATION_EQUALS:
						collect_item_direct(ctx, gconf_addr, gconf_engine, gconf_key);
						break;
					case OVAL_OPERATION_PATTERN_MATCH:
						collect_item_regexp(ctx, gconf_addr, gconf_engine, gconf_key);
						break;
					default:
						dE("Unsupported operation on the `key' entity: %d.", key_op);
						probe_ret = PROBE_EOPNOTSUPP;
						abort(); /* XXX */
					}

					gconf_engine_unref(gconf_engine);
				} else {
					dW("Invalid GConf source, skipping!");
				}

				oval_ftsent_free(ofts_ent);
                        }
                        oval_fts_close(ofts);
                }

		SEXP_free(behaviors);
        } else {
                dI("GConf source: gconfd");

		switch(key_op) {
		case OVAL_OPERATION_EQUALS:
			collect_item_direct(ctx, NULL, gconf_engine, gconf_key);
			break;
		case OVAL_OPERATION_PATTERN_MATCH:
			collect_item_regexp(ctx, NULL, gconf_engine, gconf_key);
			break;
		default:
			dE("Unsupported operation on the `key' entity: %d.", key_op);
			probe_ret = PROBE_EOPNOTSUPP;
		}

                gconf_engine_unref(gconf_engine);
        }

	SEXP_free(gconf_src);
	SEXP_free(gconf_key);

        return (probe_ret);
}
