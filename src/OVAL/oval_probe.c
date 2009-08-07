#include <stdlib.h>
#include <string.h>
#include <seap.h>
#include <sexp-manip.h>
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
        /*  7006 */ { INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54", "probe_textfilecontent54" },
        /*  7010 */ { INDEPENDENT_XML_FILE_CONTENT, "xmlfilecontent", "probe_xmlfilecontent" },
        /*  9001 */ { LINUX_DPKG_INFO, "dpkginfo", "probe_dpkginfo" },
        /*  9003 */ { LINUX_RPM_INFO,  "rpminfo",  "probe_rpminfo"  },
        /*  9004 */ { LINUX_SLACKWARE_PKG_INFO_TEST, "slackwarepkginfo", "probe_slackwarepkginfo" },
        /* 13006 */ { UNIX_RUNLEVEL, "runlevel", "probe_runlevel" }
};

#define PROBETBLSIZE (sizeof __probe_tbl / sizeof (oval_probe_t))

static SEXP_t *probe_cmd_obj_eval  (SEXP_t *sexp, void *arg);
static SEXP_t *probe_cmd_ste_fetch (SEXP_t *sexp, void *arg);

static int probe_cmd_init (SEAP_CTX_t *ctx, struct oval_object_model *model)
{
        _A(ctx != NULL);
        
        if (SEAP_cmd_register (ctx, PROBECMD_OBJ_EVAL, SEAP_CMDREG_USEARG,
                               &probe_cmd_obj_eval, (void *)model) != 0)
        {
                _D("FAIL: can't register command: %s: errno=%u, %s.\n",
                   "obj_eval", errno, strerror (errno));
                return (-1);
        }
        
        if (SEAP_cmd_register (ctx, PROBECMD_STE_FETCH, SEAP_CMDREG_USEARG,
                               &probe_cmd_ste_fetch, (void *)model) != 0)
        {
                _D("FAIL: can't register command: %s: errno=%u, %s.\n",
                   "ste_fetch", errno, strerror (errno));
                return (-1);
        }
        
        return (0);
}

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
        ptbl->ctx = SEAP_CTX_new ();

        /* TODO */
        
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
                SEXP_t *val_sexp;
                struct oval_value *val;
                
                val = oval_entity_value (ent);
                elm = SEXP_list_new ();
                SEXP_list_add (elm, elm_name);
                
                /* CHECK: check in val? */
                switch (oval_entity_datatype (ent)) {
                case OVAL_DATATYPE_VERSION:

                        val_sexp = SEXP_string_newf (oval_value_text (val));
                        SEXP_datatype_set (val_sexp, "version");
                        
                        goto add_string_val;
                case OVAL_DATATYPE_EVR_STRING:

                        val_sexp = SEXP_string_newf (oval_value_text (val));
                        SEXP_datatype_set (val_sexp, "evr_str");
                        
                        goto add_string_val;
                case OVAL_DATATYPE_STRING:
                        
                        val_sexp = SEXP_string_newf (oval_value_text (val));
                        
                add_string_val:
                        SEXP_list_add (elm, val_sexp);
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
                        val_sexp = SEXP_number_newhhu (oval_value_boolean (val));
                        
                        SEXP_datatype_set (val_sexp, "bool");
                        SEXP_list_add (elm, val_sexp);
                        
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

SEXP_t *oval_state_to_sexp (struct oval_state *state)
{
        SEXP_t *ste, *ste_name, *ste_id, *ste_ent;
        char buffer[128];
        const oval_probe_t *probe;
        struct oval_iterator_entity *entities;
        
        probe = search_probe (oval_state_subtype (state));

        if (probe == NULL) {
                _D("FAIL: unknown subtype: %d\n", oval_state_subtype (state));
                return (NULL);
        }

        ste      = SEXP_list_new ();
        ste_name = SEXP_list_new ();

        snprintf (buffer, sizeof buffer, "%s_state", probe->typestr);
        
        SEXP_list_add (ste_name, SEXP_string_newf (buffer));
        SEXP_list_add (ste_name, SEXP_string_newf (":id"));
        SEXP_list_add (ste_name, SEXP_string_newf (oval_state_id (state)));

        SEXP_list_add (ste, ste_name);
        
        entities = oval_state_entities (state);
        
        while (oval_iterator_entity_has_more (entities)) {
                ste_ent = oval_entity_to_sexp (oval_iterator_entity_next (entities));
                SEXP_list_add (ste, ste_ent);
        }
        
        return (ste);
}

struct oval_syschar *sexp_to_oval_state (SEXP_t *sexp)
{
        _A(sexp != NULL);
        /* TODO */
        return (NULL);
}

struct oval_syschar *probe_object (struct oval_object *object, struct oval_object_model *model)
{
        probe_sdtbl_t *ptbl = NULL;
        SEXP_t *sexp;
        int psd;
        SEAP_msg_t *msg;
                
        const  oval_probe_t *probe;
        struct oval_syschar *sysch = NULL;
        
        _A(object != NULL);
        _A(model  != NULL);

#if defined(THREAD_SAFE)
        pthread_once (&__init_once, probe_sdtbl_init);
        ptbl = pthread_getspecific (__key);
#else
        ptbl = &__probe_sdtbl;
        
        if (ptbl->ctx == NULL) {
                ptbl->ctx = SEAP_CTX_new ();
                
                if (probe_cmd_init (ptbl->ctx, model) != 0)
                        return (NULL);
        }
#endif
        _A(ptbl != NULL);

        _D("search_probe\n");

        probe = search_probe (oval_object_subtype(object));
        if (probe == NULL) {
                errno = EOPNOTSUPP;
                return (NULL);
        }
        
        _D("oval -> sexp\n");
        
        /* create S-exp */
        sexp  = oval_object_to_sexp (probe->typestr, object);
        
        psd = probe_sd_get (ptbl, oval_object_subtype (object));
        if (psd == -1) {
                char  *uri, *dir;
                size_t len;
                
                _D("new sd\n");

#if defined(PROBEPATH_ENV) /* insecure? */
                dir = getenv ("PROBEPATH");
                if (dir == NULL)
                        dir = OVAL_PROBE_DIR;
#else
                dir = OVAL_PROBE_DIR;
#endif
                len = (strlen (OVAL_PROBE_SCHEME) +
                       strlen (dir) + 1 +
                       strlen (probe->filename));
                
                uri = malloc (sizeof (char) * (len + 1));
                if (uri == NULL) {
                        /* ENOMEM */
                        return (NULL);
                }
                
                snprintf (uri, len + 1, "%s%s/%s",
                          OVAL_PROBE_SCHEME, dir, probe->filename);
                
                _D("uri: %s\n", uri);
                
                psd = SEAP_connect (ptbl->ctx, uri, 0);
                if (psd < 0) {
                        /* connect failed */
                        psd = errno;
                        free (uri);
                        errno = psd;
                        return (NULL);
                }

                _D("conn ok\n");

                free (uri);
                probe_sd_add (ptbl, oval_object_subtype (object), psd);
        }
        
        msg = SEAP_msg_new ();
        SEAP_msg_set (msg, sexp);
        
        puts ("--- msg ---");
        SEXP_printfa (sexp);
        puts ("\n----------");
        _D("send msg\n");
        
        if (SEAP_sendmsg (ptbl->ctx, psd, msg) != 0) {
                /* error */
                return (NULL);
        }

        /* free sexp, msg */
        
        _D("recv msg\n");
        
        if (SEAP_recvmsg (ptbl->ctx, psd, &msg) != 0) {
                /* error */
                return (NULL);
        }
        
        puts ("--- msg ---");
        SEXP_printfa (SEAP_msg_get(msg));
        puts ("\n----------");
        
        /* translate the result to oval state */
        sysch = sexp_to_oval_state (SEAP_msg_get(msg));
        
        /* cleanup */

        return (sysch);
}

static SEXP_t *probe_cmd_obj_eval (SEXP_t *sexp, void *arg)
{
        char   *id_str;
        struct oval_object *obj;
        struct oval_object_model *model = (struct oval_object_model *)arg;

        if (SEXP_stringp (sexp)) {
                id_str = SEXP_string_cstr (sexp);
                obj    = get_oval_object (model, id_str);

                if (obj == NULL) {
                        _D("FAIL: can't find obj: id=%s\n", id_str);
                        oscap_free (id_str);
                        return (NULL);
                }
                
                if (probe_object (obj, model) == NULL) {
                        _D("FAIL: obj eval failed: id=%s\n", id_str);
                        oscap_free (id_str);
                        return (NULL);
                }
                
                oscap_free (id_str);
                
                return (sexp);
        } else {
                _D("FAIL: invalid argument: type=%s\n", SEXP_strtype (sexp));
                return (NULL);
        }
}

static SEXP_t *probe_cmd_ste_fetch (SEXP_t *sexp, void *arg)
{
        SEXP_t *id, *ste_list, *ste_sexp;
        char   *id_str;
        struct oval_state *ste;
        struct oval_object_model *model = (struct oval_object_model *)arg;
        
        ste_list = SEXP_list_new ();
        
        SEXP_list_foreach (id, sexp) {
                if (SEXP_stringp (id)) {
                        id_str = SEXP_string_cstr (id);
                        ste    = get_oval_state (model, id_str);

                        if (ste == NULL) {
                                _D("FAIL: can't find ste: id=%s\n", id_str);
                                SEXP_list_free (ste_list);
                                oscap_free (id_str);
                        }
                        
                        ste_sexp = oval_state_to_sexp (ste);
                        SEXP_list_add (ste_list, ste_sexp);
                        
                        oscap_free (id_str);
                }
        }

        return (ste_list);
}
