/**
 * @file   ldap57.c
 * @brief  ldap57 probe
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

#include <stdbool.h>
#include <ldap.h>

#include "probe-api.h"
#include "common/assume.h"
#include "common/debug_priv.h"

#if defined(PROBE_LDAP_MUTEX)
# include <pthread.h>
static pthread_mutex_t __ldap57_probe_mutex;

void *probe_init(void)
{
        if (pthread_mutex_init(&__ldap57_probe_mutex, NULL) != 0) {
                return (NULL);
        }

        return (&__ldap57_probe_mutex);
}

void probe_fini(void *arg)
{
        pthread_mutex_detroy(&__ldap57_probe_mutex);
}
#endif

int probe_main(probe_ctx *ctx, void *mutex)
{
        LDAP        *ldp;
        LDAPMessage *ldpres, *entry;

        SEXP_t *se_ldap_behaviors = NULL, *se_relative_dn = NULL;
        SEXP_t *se_suffix = NULL, *se_attribute = NULL;
        SEXP_t *sa_scope, *sv_op;
        SEXP_t *item;
        SEXP_t *probe_in;

        char *relative_dn = NULL;
        char *suffix = NULL, *xattribute = NULL;
        char *uri_list, *uri, *uri_save, *attr;

        int   scope;
        char  base[2048];
        char *attrs[3];

        bool a_pattern_match = false, rdn_pattern_match = false;

        /* runtime */
#if defined(PROBE_LDAP_MUTEX)
        assume_r(mutex != NULL, PROBE_EINIT);
#endif
        probe_in = probe_ctx_getobject(ctx);
        se_ldap_behaviors = probe_obj_getent(probe_in, "behaviors", 1);

        if (se_ldap_behaviors != NULL) {
                sa_scope = probe_ent_getattrval(se_ldap_behaviors, "scope");
                SEXP_free(se_ldap_behaviors);

                if (sa_scope == NULL) {
                        dE("Atrribute `scope' is missing!");
                        return (PROBE_ENOATTR);
                }

                if (!SEXP_stringp(sa_scope)) {
                        dE("Invalid value type of the `scope' attribute.");
                        SEXP_free(sa_scope);
                        return (PROBE_EINVAL);
                }

                if (SEXP_strcmp(sa_scope, "ONE") == 0)
                        scope = LDAP_SCOPE_ONELEVEL;
                else if (SEXP_strcmp(sa_scope, "BASE") == 0)
                        scope = LDAP_SCOPE_BASE;
                else if (SEXP_strcmp(sa_scope, "SUBTREE") == 0)
                        scope = LDAP_SCOPE_SUBTREE;
                else {
                        dE("Invalid value of the `scope' attribute.");
                        SEXP_free(sa_scope);
                        return (PROBE_EINVAL);
                }

                SEXP_free(sa_scope);
        } else
                scope = LDAP_SCOPE_BASE;

#define get_string(dst, se_dst, obj, ent_name)                          \
	do {								\
		SEXP_t *__sval;						\
									\
		__sval = probe_obj_getentval (obj, ent_name, 1);	\
									\
		if (__sval != NULL) {					\
                        (dst) = SEXP_string_cstr (__sval);              \
									\
                        if ((dst) == NULL) {                            \
                                SEXP_free(__sval);                      \
                                return (PROBE_EINVAL);                  \
                        }                                               \
                                                                        \
                        (se_dst) = __sval;                              \
		} else {						\
			return (PROBE_ENOATTR);				\
		}							\
	} while (0)

        get_string(suffix,      se_suffix,      probe_in, "suffix");
        get_string(relative_dn, se_relative_dn, probe_in, "relative_dn");
        get_string(xattribute,  se_attribute,   probe_in, "attribute");

        if ((sv_op = probe_ent_getattrval(se_relative_dn, "operation")) != NULL) {
                if (SEXP_number_geti_32(sv_op) == OVAL_OPERATION_PATTERN_MATCH)
                        rdn_pattern_match = true;
                SEXP_free(sv_op);
        }

        if ((sv_op = probe_ent_getattrval(se_attribute, "operation")) != NULL) {
                if (SEXP_number_geti_32(sv_op) == OVAL_OPERATION_PATTERN_MATCH)
                        a_pattern_match = true;
                SEXP_free(sv_op);
        }

        /*
         * Construct the attribute array for ldap_search_*
         *
         * nil   -> "1.1"
         * .*    -> "*"
         * "foo" -> "foo"
         */
        attrs[0] = "objectClass";

        if (xattribute == NULL)
                attrs[1] = oscap_strdup("1.1"); /* no attibutes */
        else if (a_pattern_match)
                attrs[1] = oscap_strdup("*");   /* collect all, we'll filter them afterwards */
        else
                attrs[1] = xattribute;     /* no pattern match, use the string directly */

        attrs[2] = NULL;

        /*
         * Construct `base'
         */
        assume_r(((relative_dn ? strlen(relative_dn) : 0) +
                  (     suffix ? strlen(suffix)      : 0) + 2) < (sizeof base/sizeof(char)),
                 PROBE_ERANGE);

        if (relative_dn != NULL) {
                strcpy(base, relative_dn);
                strcat(base, ",");
                strcat(base, suffix);
        } else
                strcpy(base, suffix);

        /*
         * Get URIs
         */
        if (ldap_get_option(NULL, LDAP_OPT_URI, &uri_list) != LDAP_OPT_SUCCESS) {
                item = probe_item_creat("ldap57_item", NULL, NULL);

                probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);
                probe_item_collect(ctx, item);

                dE("ldap_get_option failed");
                goto fail0;
        }

        /*
         * Query each URI
         */
        for (;;) {
                char *entry_dn = NULL;

                if ((uri = strtok_r(uri_list, " ,", &uri_save)) == NULL)
                        break;

                ldp = NULL;

                if (ldap_initialize(&ldp, uri) != LDAP_SUCCESS)
                        continue;

                if (ldap_search_ext_s(ldp, base, scope, NULL, attrs, 0,
                                      NULL /* serverctrls */,
                                      NULL /* clientctrls */,
                                      NULL /* timeout */,
                                      0, &ldpres) != LDAP_SUCCESS)
                {
                        item = probe_item_creat("ldap57_item", NULL, NULL);

                        probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);
                        probe_item_collect(ctx, item);

                        dE("ldap_search_ext_s failed");
                        goto fail0;
                }

                entry    = ldap_first_entry(ldp, ldpres);
                entry_dn = ldap_get_dn(ldp, entry);

                while (entry != NULL) {
                        BerElement *berelm = NULL;

                        attr = ldap_first_attribute(ldp, entry, &berelm);

                        /* XXX: pattern match filter */

                        while (attr != NULL) {
                                SEXP_t   *se_value = NULL;
                                ber_tag_t bertag   = LBER_DEFAULT;
                                ber_len_t berlen   = 0;
                                Sockbuf  *berbuf   = NULL;
                                SEXP_t    se_tmp_mem;

                                berbuf = ber_sockbuf_alloc();

                                /*
                                 * Prepare the value (record) entity. Collect only
                                 * primitive (i.e. simple) types.
                                 */
                                se_value = probe_ent_creat1("value", NULL, NULL);
                                probe_ent_setdatatype(se_value, OVAL_DATATYPE_RECORD);

                                /*
                                 * XXX: does ber_get_next() return LBER_ERROR after the last value?
                                 */
                                while ((bertag = ber_get_next(berbuf, &berlen, berelm)) != LBER_ERROR) {
                                        SEXP_t *field = NULL;
                                        oval_datatype_t field_type = OVAL_DATATYPE_UNKNOWN;

                                        switch(bertag & LBER_ENCODING_MASK) {
                                        case LBER_PRIMITIVE:
                                                dI("Found primitive value, bertag = %u", bertag);
						break;
                                        case LBER_CONSTRUCTED:
                                                dW("Don't know how to handle LBER_CONSTRUCTED values");
						/* FALLTHROUGH */
                                        default:
                                                dW("Skipping attribute value, bertag = %u", bertag);
                                                continue;
                                        }

                                        assume_d(bertag & LBER_PRIMITIVE, NULL);

                                        switch(bertag & LBER_BIG_TAG_MASK) {
                                        case LBER_BOOLEAN:
                                        {       /* LDAPTYPE_BOOLEAN */
                                                ber_int_t val = -1;

                                                if (ber_get_boolean(berelm, &val) == LBER_ERROR) {
                                                        dW("ber_get_boolean: LBER_ERROR");
                                                        /* XXX: set error status on field */
                                                        continue;
                                                }

                                                assume_d(val != -1, NULL);
                                                field = probe_ent_creat1("field", NULL, SEXP_number_newb_r(&se_tmp_mem, (bool)val));
                                                field_type = OVAL_DATATYPE_BOOLEAN;
                                                SEXP_free_r(&se_tmp_mem);
                                        }       break;
                                        case LBER_INTEGER:
                                        {       /* LDAPTYPE_INTEGER */
                                                ber_int_t val = -1;

                                                if (ber_get_int(berelm, &val) == LBER_ERROR) {
                                                        dW("ber_get_int: LBER_ERROR");
                                                        /* XXX: set error status on field */
                                                        continue;
                                                }

                                                field = probe_ent_creat1("field", NULL, SEXP_number_newi_r(&se_tmp_mem, (int)val));
                                                field_type = OVAL_DATATYPE_INTEGER;
                                                SEXP_free_r(&se_tmp_mem);
                                        }       break;
                                        case LBER_BITSTRING:
                                                /* LDAPTYPE_BIT_STRING */
                                                dW("LBER_BITSTRING: not implemented");
                                                continue;
                                        case LBER_OCTETSTRING:
                                        {       /*
                                                 * LDAPTYPE_PRINTABLE_STRING
                                                 * LDAPTYPE_NUMERIC_STRING
                                                 * LDAPTYPE_DN_STRING
                                                 * LDAPTYPE_BINARY (?)
                                                 */
                                                char *val = NULL;

                                                if (ber_get_stringa(berelm, &val) == LBER_ERROR) {
                                                        dW("ber_get_stringa: LBER_ERROR");
                                                        /* XXX: set error status on field */
                                                        continue;
                                                }

                                                assume_d(val != NULL, NULL);
                                                field = probe_ent_creat1("field", NULL, SEXP_string_new_r(&se_tmp_mem, val, strlen(val)));
                                                field_type = OVAL_DATATYPE_STRING;
                                                SEXP_free_r(&se_tmp_mem);
                                                ber_memfree(val);
                                        }       break;
                                        case LBER_NULL:
                                                /* XXX: no equivalent LDAPTYPE_? or empty */
                                                dI("LBER_NULL: skipped");
                                                continue;
                                        case LBER_ENUMERATED:
                                                /* XXX: no equivalent LDAPTYPE_? */
                                                dW("Don't know how to handle LBER_ENUMERATED type");
                                                continue;
                                        default:
                                                dW("Unknown attribute value type, bertag = %u", bertag);
                                                continue;
                                        }

                                        if (field != NULL) {
                                                assume_d(field_type != OVAL_DATATYPE_UNKNOWN, NULL);

                                                probe_ent_setdatatype(field, field_type);
                                                probe_ent_attr_add(field, "name", SEXP_string_new_r(&se_tmp_mem, attr, strlen(attr)));
                                                SEXP_list_add(se_value, field);
                                                SEXP_free_r(&se_tmp_mem);
                                                SEXP_free(field);
                                        }
                                }

                                ber_sockbuf_free(berbuf);

                                /*
                                 * Create the item
                                 */
                                item = probe_item_create(OVAL_INDEPENDENT_LDAP57, NULL,
                                                         "suffix",       OVAL_DATATYPE_STRING, suffix,
                                                         "relative_dn",  OVAL_DATATYPE_STRING, relative_dn, /* XXX: pattern match */
                                                         "attribute",    OVAL_DATATYPE_STRING, attr,
                                                         "object_class", OVAL_DATATYPE_STRING, "",
                                                         "ldaptype",     OVAL_DATATYPE_STRING, "",
                                                         NULL);

                                SEXP_list_add(item, se_value);
                                SEXP_free(se_value);

                                probe_item_collect(ctx, item);

                                attr = ldap_next_attribute(ldp, entry, berelm);
                        }

                        ber_free(berelm, 0);
                        ldap_memfree(entry_dn);

                        entry    = ldap_next_entry(ldp, entry);
                        entry_dn = ldap_get_dn(ldp, entry);
                }

                /*
                 * Close the LDAP connection and free resources
                 */
                ldap_unbind_ext_s(ldp, NULL, NULL);
        }

        ldap_memfree(uri_list);

fail0:
        SEXP_free(se_suffix);
        SEXP_free(se_relative_dn);
        SEXP_free(se_attribute);

        free(suffix);
        free(relative_dn);
        free(attrs[1]); /* attribute */

        return (0);
}
