#include <stdlib.h>
#include <string.h>
#include <seap.h>
#include <assert.h>
#include <errno.h>
#if defined(THREAD_SAFE)
# include <pthread.h>
#endif
#include "oval_probe.h"
#include "probes/probe.h"

#ifndef _A
#define _A(x) assert(x)
#endif

/* KEEP THIS LIST SORTED! (by subtype) */
const oval_probe_t __probe_tbl[] = {
        /*  9001 */ { LINUX_DPKG_INFO, "dpkginfo", "probe_dpkginfo" },
        /*  9003 */ { LINUX_RPM_INFO,  "rpminfo",  "probe_rpminfo"  },
        /*  9004 */ { LINUX_SLACKWARE_PKG_INFO_TEST, "slackwarepkginfo", "probe_slackwarepkginfo" },
        /* 13006 */ { UNIX_RUNLEVEL, "runlevel", "probe_runlevel" }
};

#define PROBETBLSIZE (sizeof __probe_tbl / sizeof (oval_probe_t))

#if defined(THREAD_SAFE)
static pthread_once_t __init_once = PTHREAD_ONCE_INIT;
static pthread_key_t  __key;

static void probe_sdtbl_free (void *arg)
{
        /* TODO */
        return;
}

static void probe_sdtbl_init (void)
{
        probe_sdtbl_t *ptbl;
        (void) pthread_key_create (&__key, probe_sdtbl_free);
        
        ptbl = malloc (sizeof (probe_sdtbl_t));
        ptbl->memb = NULL;
        ptbl->count = 0;
        SEAP_CTX_init (&(ptbl->ctx));
        
        (void) pthread_setspecific (key, (void *)ptbl);
        return;
}
#else
static probe_sdtbl_t __probe_sdtbl = PROBE_SDTBL_INITIALIZER;
#endif

const oval_probe_t *search_probe (oval_subtype_enum typenum)
{
        uint32_t w, s;
        
        w = PROBETBLSIZE;
        s = 0;
                
        while (w > 0) {
                if (typenum > __probe_tbl[s + w/2].typenum) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (typenum < __probe_tbl[s + w/2].typenum) {
                        w = w/2;
                } else {
                        return &(__probe_tbl[s + w/2]);
                }
        }
#undef cmp
        return (NULL);
}

int probe_sd_get (probe_sdtbl_t *tbl, oval_subtype_enum ptype)
{
        size_t w, s;
        
        _A(tbl != NULL);
        
        /* FIXME: duplicated code */
        w = tbl->count;
        s = 0;

        while (w > 0) {
                if (ptype > tbl->memb[s + w/2].typenum) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (ptype < tbl->memb[s + w/2].typenum) {
                        w = w/2;
                } else {
                        return (tbl->memb[s + w/2].sd);
                }
        }
        /* Not found */
        return (-1);
}

int probe_sd_cmp (const void *a, const void *b)
{
        return (((probe_sd_t *)a)->typenum - ((probe_sd_t *)b)->typenum);
}

int probe_sd_add (probe_sdtbl_t *tbl, oval_subtype_enum type, int sd)
{
        _A(tbl != NULL);
        _A(sd >= 0);

        tbl->memb = realloc (tbl->memb, sizeof (probe_sd_t) * (++tbl->count));
        tbl->memb[tbl->count - 1].typenum = type;
        tbl->memb[tbl->count - 1].sd = sd;

        qsort (tbl->memb, tbl->count, sizeof (probe_sd_t), probe_sd_cmp);
        
        return (0);
}

int probe_sd_del (probe_sdtbl_t *tbl, oval_subtype_enum type)
{
        _A(tbl != NULL);
        /* TODO */
        return (0);
}

SEXP_t *oval_entity_to_sexp (struct oval_entity *ent)
{
        SEXP_t *elm, *elm_name;
        
        elm_name = SEXP_list_new ();
        
        SEXP_list_add (elm_name,
                       SEXP_string_newf (oval_entity_name (ent)));
        
        /* operation */
        SEXP_list_add (elm_name,
                       SEXP_string_new (":operation", 10));
        SEXP_list_add (elm_name,
                       SEXP_number_newu (oval_entity_operation (ent)));
                
        /* var_ref */
        if (oval_entity_varref_type (ent) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
                struct oval_variable *var;
                
                var = oval_entity_variable (ent);
                
                SEXP_list_add (elm_name,
                               SEXP_string_new (":var_ref", 8));
                SEXP_list_add (elm_name,
                               SEXP_string_newf (oval_variable_id (var)));

                elm = SEXP_list_new ();
                SEXP_list_add (elm, elm_name);
                
        } else { /* value */
                struct oval_value *val;
                
                val = oval_entity_value (ent);
                elm = SEXP_list_new ();
                SEXP_list_add (elm, elm_name);
                
                /* CHECK: check in val? */
                switch (oval_entity_datatype (ent)) {
                case OVAL_DATATYPE_VERSION:
                case OVAL_DATATYPE_EVR_STRING:
                case OVAL_DATATYPE_STRING:
                        
                        SEXP_list_add (elm,
                                       SEXP_string_newf (oval_value_text (val)));
                        break;
                case OVAL_DATATYPE_FLOAT:
                        
                        SEXP_list_add (elm,
                                       SEXP_number_newf (oval_value_float (val)));
                        break;
                case OVAL_DATATYPE_INTEGER:
                        
                        SEXP_list_add (elm,
                                       SEXP_number_newd (oval_value_integer (val)));
                        break;
                case OVAL_DATATYPE_BOOLEAN:
                        
                        SEXP_list_add (elm,
                                       SEXP_number_newd (oval_value_boolean (val)));
                        break;
                default:
                        break;
                }
        }
        
        return (elm);
}

SEXP_t *oval_set_to_sexp (struct oval_set *set)
{
        SEXP_t *elm, *elm_name;
        
        elm_name = SEXP_list_new ();
        
        SEXP_list_add (elm_name, 
                       SEXP_string_new ("set", 3));
        
        /* operation */
        SEXP_list_add (elm_name,
                       SEXP_string_new (":operation", 10));
        SEXP_list_add (elm_name,
                       SEXP_number_newu (oval_set_operation (set)));

        elm = SEXP_list_new ();
        SEXP_list_add (elm, elm_name);
        
        switch (oval_set_type (set)) {
        case OVAL_SET_AGGREGATE: {
                struct oval_iterator_set *sit;
                struct oval_set *subset;
                
                sit = oval_set_subsets (set);
                
                while (oval_iterator_set_has_more (sit)) {
                        subset = oval_iterator_set_next (sit);
                        SEXP_list_add (elm, oval_set_to_sexp (subset));
                }
        }
                break;
        case OVAL_SET_COLLECTIVE: {
                struct oval_iterator_object *oit;
                struct oval_iterator_state  *sit;
                struct oval_object *obj;
                struct oval_state  *ste;
                SEXP_t *subelm;
                
                oit = oval_set_objects (set);
                sit = oval_set_filters (set);
                
                while (oval_iterator_object_has_more (oit)) {
                        obj    = oval_iterator_object_next (oit);
                        subelm = SEXP_list_new ();
                        
                        SEXP_list_add (subelm,
                                       SEXP_string_new ("obj_ref", 7));
                        SEXP_list_add (subelm,
                                       SEXP_string_newf (oval_object_id (obj)));
                        SEXP_list_add (elm, subelm);
                }
                
                while (oval_iterator_state_has_more (sit)) {
                        ste    = oval_iterator_state_next (sit);
                        subelm = SEXP_list_new ();
                        
                        SEXP_list_add (subelm,
                                       SEXP_string_new ("filter", 6));
                        SEXP_list_add (subelm,
                                       SEXP_string_newf (oval_state_id (ste)));
                        SEXP_list_add (elm, subelm);
                }
        }                
                break;
        default:
                abort ();
        }
        
        return (elm);
}

SEXP_t *oval_object_to_sexp (const char *typestr, struct oval_object *object)
{
        SEXP_t *obj_sexp, *obj_name, *elm, *elm_value;

        struct oval_iterator_object_content *cit;
        struct oval_object_content *content;
        struct oval_entity *entity;
        
        obj_sexp = SEXP_list_new ();
        obj_name = SEXP_list_new ();


        /*
         * Object name & attributes (id)
         */
        SEXP_list_add (obj_name,
                       SEXP_string_newf ("%s_object", typestr));
        
        SEXP_list_add (obj_name,
                       SEXP_string_new (":id", 3));
        SEXP_list_add (obj_name,
                       SEXP_string_newf (oval_object_id (object)));
        
        SEXP_list_add (obj_sexp, obj_name);

        /*
         * Object content
         */

        cit = oval_object_object_content (object);
        while (oval_iterator_object_content_has_more (cit)) {
                content = oval_iterator_object_content_next (cit);
                elm_value = NULL;
                
                switch (oval_object_content_type (content)) {
                case OVAL_OBJECTCONTENT_ENTITY:
                        
                        elm_value = oval_entity_to_sexp (oval_object_content_entity (content));
                        break;
                case OVAL_OBJECTCONTENT_SET:
                        
                        elm_value = oval_set_to_sexp (oval_object_content_set (content));
                        break;
                case OVAL_OBJECTCONTENT_UNKNOWN:
                        break;
                }
                
                if (elm_value == NULL) {
                        SEXP_free (obj_sexp);
                        return (NULL);
                }

                SEXP_list_add (obj_sexp, elm_value);
        }
        
        return (obj_sexp);
}

struct oval_iterator_syschar *sexp_to_oval_state (SEXP_t *sexp)
{
        _A(sexp != NULL);
        /* TODO */
        return (NULL);
}

struct oval_iterator_syschar *probe_simple_object (struct oval_object *object,
                                                   struct oval_iterator_variable_binding *binding)
{
        probe_sdtbl_t *ptbl = NULL;
        SEXP_t *sexp;
        int psd;
        SEAP_msg_t *msg;
                
        const oval_probe_t *probe;
        struct oval_iterator_syschar *sysch = NULL;
        
        _A(object != NULL);

#if defined(THREAD_SAFE)
        pthread_once (&__init_once, probe_sdtbl_init);
        ptbl = pthread_getspecific (__key);
#else
        ptbl = &__probe_sdtbl;
#endif
        _A(ptbl != NULL);

        probe = search_probe (oval_object_subtype(object));
        if (probe == NULL) {
                errno = EOPNOTSUPP;
                return (NULL);
        }
        
        /* create S-exp */
        sexp  = oval_object_to_sexp (probe->typestr, object);
        
        psd = probe_sd_get (ptbl, oval_object_subtype (object));
        if (psd == -1) {
                char  *uri, *dir;
                size_t len;
                
                len = (strlen (OVAL_PROBE_SCHEME) +
                       strlen (OVAL_PROBE_DIR) + 1 +
                       strlen (probe->filename));
                
                uri = malloc (sizeof (char) * (len + 1));
                if (uri == NULL) {
                        /* ENOMEM */
                        return (NULL);
                }
                
#if defined(PROBEPATH_ENV) /* insecure? */
                dir = getenv ("PROBEPATH");
                if (dir == NULL)
                        dir = OVAL_PROBE_DIR;
#else
                dir = OVAL_PROBE_DIR;
#endif
                snprintf (uri, len + 1, "%s%s/%s",
                          OVAL_PROBE_SCHEME, dir, probe->filename);
                
                psd = SEAP_connect (&(ptbl->ctx), uri, 0);
                if (psd < 0) {
                        /* connect failed */
                        psd = errno;
                        free (uri);
                        errno = psd;
                        return (NULL);
                }

                free (uri);
                probe_sd_add (ptbl, oval_object_subtype (object), psd);
        }
        
        msg = SEAP_msg_new ();
        msg->sexp = sexp;

        if (SEAP_sendmsg (&(ptbl->ctx), psd, msg) != 0) {
                /* error */
                return (NULL);
        }

        /* free sexp, msg */
        
        if (SEAP_recvmsg (&(ptbl->ctx), psd, &msg) != 0) {
                /* error */
                return (NULL);
        }
        
        /* translate the result to oval state */
        sysch = sexp_to_oval_state (msg->sexp);
        
        /* cleanup */

        return (sysch);
}
