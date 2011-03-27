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
                ldap_destroy(ldp);
                return (NULL);
        }

        return (&__ldap57_probe_mutex);
}

void probe_fini(void *arg)
{
        pthread_mutex_detroy(&__ldap57_probe_mutex);
}
#endif

int probe_main(SEXP_t *probe_in, SEXP_t *probe_out, void *mutex, SEXP_t *filters)
{
        LDAP        *ldp;
        LDAPMessage *ldpres, *entry;
        BerElement  *berptr = NULL;
        struct berval **vals;

        SEXP_t *se_ldap_behaviors = NULL, *se_relative_dn = NULL;
        SEXP_t *se_suffix = NULL, *se_attribute = NULL;
        SEXP_t *sa_scope, *sv_op;
        SEXP_t *item;

        char *relative_dn = NULL;
        char *suffix = NULL, *attribute = NULL;
        char *uri_list, *uri, *uri_save, *attr;

        int   scope;
        char  base[2048];
        char *attrs[3];

        bool a_pattern_match = false, rdn_pattern_match = false;

        (void)filters;

        assume_d(probe_in  != NULL, PROBE_EFAULT);
        assume_d(probe_out != NULL, PROBE_EFAULT);
        /* runtime */
#if defined(PROBE_LDAP_MUTEX)
        assume_r(mutex != NULL, PROBE_EINIT);
#endif
        se_ldap_behaviors = probe_obj_getent(probe_in, "behaviors", 1);

        if (se_ldap_behaviors != NULL) {
                sa_scope = probe_ent_getattrval(se_ldap_behaviors, "scope");
                SEXP_free(se_ldap_behaviors);

                if (sa_scope == NULL) {
                        dE("Atrribute `scope' is missing!\n");
                        return (PROBE_ENOATTR);
                }

                if (!SEXP_stringp(sa_scope)) {
                        dE("Invalid value type of the `scope' attribute.\n");
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
                        dE("Invalid value of the `scope' attribute.\n");
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
		__sval = probe_obj_getentval (obj, #ent_name, 1);	\
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
                }                                                       \
	} while (0)

        get_string(suffix,      se_suffix,      probe_in, "suffix");
        get_string(relative_dn, se_relative_dn, probe_in, "relative_dn");
        get_string(attribute,   se_attribute,   probe_in, "attribute");

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

        if (attribute == NULL)
                attrs[1] = strdup("1.1"); /* no attibutes */
        else if (a_pattern_match)
                attrs[1] = strdup("*");   /* collect all, we'll filter them afterwards */
        else
                attrs[1] = attribute;     /* no pattern match, use the string directly */

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

                probe_item_setstatus(item, OVAL_STATUS_ERROR);
                probe_cobj_add_item(probe_out, item);
                SEXP_free(item);

                goto fail0;
        }

        /*
         * Query each URI
         */
        for (;;) {
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

                        probe_item_setstatus(item, OVAL_STATUS_ERROR);
                        probe_cobj_add_item(probe_out, item);
                        SEXP_free(item);
                        ldap_destroy(ldp);

                        goto fail0;
                }

                entry = ldap_first_entry(ldp, ldpres);

                while (entry != NULL) {
                        attr = ldap_first_attribute(ldp, entry, &berptr);

                        while (attr != NULL) {
                                vals = ldap_get_values_len(ldp, entry, attribute);
                                item = probe_item_creat("ldap57_item", NULL,
                                                        "suffix",       NULL, se_suffix,
                                                        "relative_dn",  NULL, se_relative_dn, /* FIXME: get correct name if pattern match is used */
                                                        "attribute",    NULL, se_attribute, /* FIXME: ---||--- */
                                                        /* TODO */
                                                        "object_class", NULL, NULL,
                                                        "ldaptype",     NULL, NULL,
                                                        /* TODO: record+fields */
                                                        "value",        NULL, NULL,
                                                        NULL);

                                probe_cobj_add_item(probe_out, item);
                                SEXP_free(item);

                                attr = ldap_next_attribute(ldp, entry, berptr);
                                ldap_value_free_len(vals);
                        }

                        ber_free(berptr, 0);
                        entry = ldap_next_entry(ldp, entry);
                }

                ldap_destroy(ldp);
        }

        ldap_memfree(uri_list);

fail0:
        SEXP_free(se_suffix);
        SEXP_free(se_relative_dn);
        SEXP_free(se_attribute);

        oscap_free(suffix);
        oscap_free(relative_dn);

        oscap_free(attrs[0]);
        oscap_free(attrs[1]); /* attribute */

        return (0);
}
