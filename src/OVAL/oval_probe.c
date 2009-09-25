#ifndef __STUB_PROBE

#include <stdlib.h>
#include <string.h>
#include <seap.h>
#include <sexp-manip.h>
#include <assert.h>
#include <errno.h>
#include <common/bfind.h>
#if defined(THREAD_SAFE)
# include <pthread.h>
#endif
#include "oval_probe.h"
#include "probes/probe.h"
#include "oval_system_characteristics_impl.h"

#ifndef _A
#define _A(x) assert(x)
#endif

/* KEEP THIS LIST SORTED! (by subtype) */
const oval_probe_t __probe_tbl[] = {
        /*  7001 */ { OVAL_INDEPENDENT_FAMILY,               "family",            "probe_family"            },
        /*  7006 */ { OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54", "probe_textfilecontent54" },
        /*  7010 */ { OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent",    "probe_xmlfilecontent"    },
        /*  9001 */ { OVAL_LINUX_DPKG_INFO,                  "dpkginfo",          "probe_dpkginfo"          },
        /*  9003 */ { OVAL_LINUX_RPM_INFO,                   "rpminfo",           "probe_rpminfo"           },
        /*  9004 */ { OVAL_LINUX_SLACKWARE_PKG_INFO_TEST,    "slackwarepkginfo",  "probe_slackwarepkginfo"  },
        /* 13001 */ { OVAL_UNIX_FILE,                        "file",              "probe_file"              },
        /* 13006 */ { OVAL_UNIX_RUNLEVEL,                    "runlevel",          "probe_runlevel"          }
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
        ptbl->flags = 0;

        /* TODO */
        
        (void) pthread_setspecific (__key, (void *)ptbl);
        return;
}
#else
static probe_sdtbl_t __probe_sdtbl = PROBE_SDTBL_INITIALIZER;
#endif

static int probe_subtype_cmp (void *a, void *b)
{
#define K1(p) (*(oval_subtype_t *)(p))
#define K2(p) (((oval_probe_t *)(p))->typenum)
        return ((int)(K1(a) - K2(b)));
#undef K1
#undef K2
}

const oval_probe_t *search_probe (oval_subtype_t subtype)
{
        return (oscap_bfind ((void *)__probe_tbl, PROBETBLSIZE, sizeof __probe_tbl[0], &subtype, probe_subtype_cmp));
}

static int probe_sd_get (probe_sdtbl_t *tbl, oval_subtype_t subtype)
{
        probe_sd_t *psd;
        
        _A(tbl != NULL);
        
        psd = oscap_bfind ((void *)(tbl->memb), tbl->count, sizeof (probe_sd_t), &subtype, probe_subtype_cmp);
        
        _D("tbl=%p, psd=%p\n", tbl, psd);
        
        return (psd == NULL ? -1 : psd->sd);
}

static int probe_sd_cmp (const void *a, const void *b)
{
        return (((probe_sd_t *)a)->typenum - ((probe_sd_t *)b)->typenum);
}

static int probe_sd_add (probe_sdtbl_t *tbl, oval_subtype_t type, int sd)
{
        _A(tbl != NULL);
        _A(sd >= 0);

        tbl->memb = realloc (tbl->memb, sizeof (probe_sd_t) * (++tbl->count));
        tbl->memb[tbl->count - 1].typenum = type;
        tbl->memb[tbl->count - 1].sd = sd;

        qsort (tbl->memb, tbl->count, sizeof (probe_sd_t), probe_sd_cmp);
        
        return (0);
}

static int probe_sd_del (probe_sdtbl_t *tbl, oval_subtype_t type)
{
        _A(tbl != NULL);
        /* TODO */
        return (0);
}

static SEXP_t *oval_entity_to_sexp (struct oval_entity *ent)
{
        SEXP_t *elm, *elm_name;
        

        elm_name = SEXP_list_new (SEXP_string_newf (oval_entity_get_name (ent)),
                                  /* operation */
                                  SEXP_string_new (":operation", 10),
                                  SEXP_number_newu_32 (oval_entity_get_operation (ent)),
                                  NULL);

        elm = SEXP_list_new (NULL);

        /* var_ref */
        if (oval_entity_get_varref_type (ent) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
                struct oval_variable *var;

                var = oval_entity_get_variable (ent);

                SEXP_list_add (elm_name,
                               SEXP_string_new (":var_ref", 8));
                SEXP_list_add (elm_name,
                               SEXP_string_newf (oval_variable_get_id (var)));
                SEXP_list_add (elm, elm_name);
                
        } else { /* value */
                SEXP_t *val_sexp;
                struct oval_value *val;
                
                val = oval_entity_get_value (ent);
                SEXP_list_add (elm, elm_name);
                
                /* CHECK: check in val? */
                switch (oval_entity_get_datatype (ent)) {
                case OVAL_DATATYPE_VERSION:

                        val_sexp = SEXP_string_newf (oval_value_get_text (val));
                        SEXP_datatype_set (val_sexp, "version");
                        
                        goto add_string_val;
                case OVAL_DATATYPE_EVR_STRING:

                        val_sexp = SEXP_string_newf (oval_value_get_text (val));
                        SEXP_datatype_set (val_sexp, "evr_str");
                        
                        goto add_string_val;
                case OVAL_DATATYPE_STRING:

                        val_sexp = SEXP_string_newf (oval_value_get_text (val));

                add_string_val:
                        SEXP_list_add (elm, val_sexp);
                        break;
                case OVAL_DATATYPE_FLOAT:
                        
                        SEXP_list_add (elm,
                                       SEXP_number_newf (oval_value_get_float (val)));
                        break;
                case OVAL_DATATYPE_INTEGER:
                        
                        SEXP_list_add (elm,
                                       SEXP_number_newi_32 (oval_value_get_integer (val)));
                        break;
                case OVAL_DATATYPE_BOOLEAN:
                        val_sexp = SEXP_number_newb (oval_value_get_boolean (val));

                        SEXP_datatype_set (val_sexp, "bool");
                        SEXP_list_add (elm, val_sexp);
                        
                        break;
                default:
                        break;
                }
        }
        
        return (elm);
}

static SEXP_t *oval_set_to_sexp (struct oval_set *set)
{
        SEXP_t *elm, *elm_name;
        
        elm_name = SEXP_list_new (SEXP_string_new ("set", 3),
                                  /* operation */
                                  SEXP_string_new (":operation", 10),
                                  SEXP_number_newu_32 (oval_setobject_get_operation (set)),
                                  NULL);
        
        elm = SEXP_list_new (elm_name, NULL);
        
        switch (oval_setobject_get_type (set)) {
        case OVAL_SET_AGGREGATE: {
                struct oval_setobject_iterator *sit;
                struct oval_set *subset;

                sit = oval_setobject_get_subsets (set);

                while (oval_setobject_iterator_has_more (sit)) {
                        subset = oval_setobject_iterator_next (sit);
                        SEXP_list_add (elm, oval_set_to_sexp (subset));
                }
                oval_setobject_iterator_free (sit);
        }
                break;
        case OVAL_SET_COLLECTIVE: {
                struct oval_object_iterator *oit;
                struct oval_state_iterator  *sit;
                struct oval_object *obj;
                struct oval_state  *ste;
                SEXP_t *subelm;

                oit = oval_setobject_get_objects (set);
                sit = oval_setobject_get_filters (set);

                while (oval_object_iterator_has_more (oit)) {
                        obj    = oval_object_iterator_next (oit);

                        subelm = SEXP_list_new (SEXP_string_new ("obj_ref", 7),
                                                SEXP_string_newf (oval_object_get_id (obj)),
                                                NULL);
                        
                        SEXP_list_add (elm, subelm);
                }
                oval_object_iterator_free (oit);

                while (oval_state_iterator_has_more (sit)) {
                        ste    = oval_state_iterator_next (sit);
                        
                        subelm = SEXP_list_new (SEXP_string_new ("filter", 6),
                                                SEXP_string_newf (oval_state_get_id (ste)),
                                                NULL);
                        
                        SEXP_list_add (elm, subelm);
                }
                oval_state_iterator_free (sit);
        }
                break;
        default:
                abort ();
        }
        
        return (elm);
}

static SEXP_t *oval_behaviors_to_sexp (struct oval_behavior_iterator *bit)
{
	char *attr_name, *attr_val;
        SEXP_t *elm_name;
        struct oval_behavior *behavior;

        elm_name = SEXP_list_new (SEXP_string_newf ("behaviors"),
                                  NULL);

        while (oval_behavior_iterator_has_more (bit)) {
                behavior = oval_behavior_iterator_next (bit);
                attr_name = oval_behavior_get_key(behavior);
                attr_val = oval_behavior_get_value(behavior);
                
                SEXP_list_add(elm_name, SEXP_string_newf(":%s", attr_name));
                
                if (attr_val != NULL) {
                        SEXP_list_add(elm_name, SEXP_string_newf(attr_val));
                }
        }
        oval_behavior_iterator_free (bit);

        return (SEXP_list_new (elm_name, NULL));
}

SEXP_t *oval_object_to_sexp (const char *typestr, struct oval_object *object)
{
        SEXP_t *obj_sexp, *obj_name, *elm;

        struct oval_object_content_iterator *cit;
        struct oval_behavior_iterator *bit;
        struct oval_object_content *content;
        struct oval_entity *entity;
        
        /*
         * Object name & attributes (id)
         */
        obj_name = SEXP_list_new (SEXP_string_newf ("%s_object", typestr),
                                  SEXP_string_new (":id", 3),
                                  SEXP_string_newf (oval_object_get_id (object)),
                                  NULL);
        
        obj_sexp = SEXP_list_new (obj_name, NULL);

        /*
         * Object content
         */

        cit = oval_object_get_object_content (object);
        while (oval_object_content_iterator_has_more (cit)) {
                content = oval_object_content_iterator_next (cit);
                elm = NULL;

                switch (oval_object_content_get_type (content)) {
                case OVAL_OBJECTCONTENT_ENTITY:

                        elm = oval_entity_to_sexp (oval_object_content_get_entity (content));
                        break;
                case OVAL_OBJECTCONTENT_SET:

                        elm = oval_set_to_sexp (oval_object_content_get_setobject (content));
                        break;
                case OVAL_OBJECTCONTENT_UNKNOWN:
                        break;
                }
                
                if (elm == NULL) {
                        SEXP_free (obj_sexp);
                        return (NULL);
                }
                
                SEXP_list_add (obj_sexp, elm);
        }
        oval_object_content_iterator_free (cit);

        /*
         * Object behaviors
         */

        bit = oval_object_get_behaviors (object);
        if (oval_behavior_iterator_has_more (bit)) {
                elm = oval_behaviors_to_sexp (bit);
                SEXP_list_add (obj_sexp, elm);
        }
        
        return (obj_sexp);
}

SEXP_t *oval_state_to_sexp (struct oval_state *state)
{
        SEXP_t *ste, *ste_name, *ste_id, *ste_ent;
        char buffer[128];
        const oval_probe_t *probe;
        //struct oval_entity_iterator *entities;
        struct oval_state_content_iterator *contents;

        probe = search_probe (oval_state_get_subtype (state));

        if (probe == NULL) {
                _D("FAIL: unknown subtype: %d\n", oval_state_get_subtype (state));
                return (NULL);
        }

        snprintf (buffer, sizeof buffer, "%s_state", probe->typestr);
        
        ste_name = SEXP_list_new (SEXP_string_newf (buffer),
                                  SEXP_string_new  (":id", 3),
                                  SEXP_string_newf (oval_state_get_id (state)),
                                  NULL);
        
        ste = SEXP_list_new (ste_name, NULL);
        
        //entities = oval_state_entities (state);

        contents = oval_state_get_contents(state);

        /*
        while (oval_entity_iterator_has_more (entities)) {
                ste_ent = oval_entity_to_sexp (oval_entity_iterator_next (entities));
                SEXP_list_add (ste, ste_ent);
        }
        */
        while(oval_state_content_iterator_has_more(contents)){
        	struct oval_state_content *content = oval_state_content_iterator_next(contents);
        	//Note that content includes entity and variable check constraints
            ste_ent = oval_entity_to_sexp (oval_state_content_get_entity(content));
            SEXP_list_add (ste, ste_ent);
        }
        oval_state_content_iterator_free(contents);

        return (ste);
}


struct oval_sysitem* oval_sysitem_from_sexp(SEXP_t *sexp)
{
	_A(sexp);
	SEXP_t *sval;
	char *key;
	char *val;

        key = probe_ent_getname (sexp);

	if (!key)
		return NULL;
        
	sval = probe_ent_getval (sexp, 1);
	switch (SEXP_typeof(sval)) {
		case SEXP_TYPE_STRING: {
			val = SEXP_string_cstr(sval);
			break;
		}
		case SEXP_TYPE_NUMBER: {
			size_t allocsize = 64;
			val = malloc(allocsize * sizeof(char));
			*val = '\0';

			switch (SEXP_number_type(sval)) {
                        case SEXP_NUM_DOUBLE:
                                snprintf(val, allocsize, "%f", SEXP_number_getf (sval));
                                break;
                        case SEXP_NUM_INT8:
                        case SEXP_NUM_INT16:
                        case SEXP_NUM_INT32:
                        case SEXP_NUM_INT64:
                                snprintf(val, allocsize, "%lld", SEXP_number_geti_64 (sval));
                                break;
                        case SEXP_NUM_UINT8:
                        case SEXP_NUM_UINT16:
                        case SEXP_NUM_UINT32:
                        case SEXP_NUM_UINT64:
                                snprintf(val, allocsize, "%llu", SEXP_number_getu_64 (sval));
                                break;
                        case SEXP_NUM_NONE:
                        default:
                                _A(false);
                                break;
			}
                        
			val[allocsize - 1] = '\0';
			val = oscap_realloc(val, strlen(val) + 1);
			break;
		}
		default: {
			_D("Unsupported type: %u", SEXP_typeof(sval));
			oscap_free(key);
			return NULL;
		}
	}

	int datatype = probe_ent_getdatatype(sexp, 1);
	if (datatype < 0)
		datatype = 0;

	int status = probe_ent_getstatus(sexp);

	struct oval_sysitem* item = oval_sysitem_new();

	oval_sysitem_set_status(item, status);
	oval_sysitem_set_name(item, key);
	oval_sysitem_set_mask(item, probe_ent_getmask(sexp));

	if (status == OVAL_STATUS_EXISTS)
		oval_sysitem_set_value(item, val);

	oval_sysitem_set_datatype(item, datatype);

	return item;
}

struct oval_sysdata *oval_sysdata_from_sexp(SEXP_t *sexp)
{
	_A(sexp);

	static int id_counter = 1;  /* TODO better ID generator */

	char *name;
	struct oval_sysdata* sysdata = NULL;
        
        name = probe_ent_getname (sexp);
        
	if (name == NULL)
		return NULL;
	else {
		char *endptr = strrchr(name, '_');

		if (strcmp(endptr, "_item") != 0)
			goto cleanup;

		*endptr = '\0'; // cut off the '_item' part
	}

	int type = 0;
	size_t size;for (size = 0; size < PROBETBLSIZE; ++size) {
		if (strcmp(__probe_tbl[size].typestr, name) == 0) {
			type = __probe_tbl[size].typenum;
			break;
		}
	}

	_D("Syschar entry type: %d '%s' => %s", type, name, (type ? "OK" : "FAILED to decode"));

	char *id = oscap_alloc(sizeof(char) * 16);
	SEXP_t *sub;
	struct oval_sysitem* sysitem;

	int status = probe_ent_getstatus(sexp);

	sprintf(id, "%d", id_counter++);
	sysdata = oval_sysdata_new(id);
	oval_sysdata_set_status(sysdata, status);
	oval_sysdata_set_subtype(sysdata, type);
	//oval_sysdata_set_subtype_name(sysdata, name);
	
	if (status == OVAL_STATUS_EXISTS) {
		int i;for (i = 2; (sub = SEXP_list_nth(sexp, i)) != NULL; ++i)
			if ((sysitem = oval_sysitem_from_sexp(sub)) != NULL)
				oval_sysdata_add_item(sysdata, sysitem);
	}

cleanup:
	oscap_free(name);
	return sysdata;
}

struct oval_syschar *sexp_to_oval_state (SEXP_t *sexp, struct oval_object* object)
{
        _A(sexp != NULL);
		struct oval_syschar *syschar = oval_syschar_new(object);
		oval_syschar_apply_sexp(syschar, sexp, object);
        return (syschar);
}

int oval_syschar_apply_sexp(struct oval_syschar *syschar, SEXP_t *sexp, struct oval_object* object)
{
	_A(sexp != NULL);
	_A(syschar != NULL);

	SEXP_t *s;
	struct oval_sysdata* sysdata;

	if (oval_syschar_get_object(syschar) == NULL)
		oval_syschar_set_object(syschar, object);
	else if (object == NULL)
		object = oval_syschar_get_object(syschar);

	_A(object == oval_syschar_get_object(syschar));

	SEXP_list_foreach (s, sexp) {
		sysdata = oval_sysdata_from_sexp(s);
		if (sysdata)
			oval_syschar_add_sysdata(syschar, sysdata);
	}
	
	return 1;
}

struct oval_syschar *oval_object_probe (struct oval_object *object, struct oval_object_model *model)
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
        
        if (ptbl->ctx == NULL)
                ptbl->ctx = SEAP_CTX_new ();
#endif
        _A(ptbl != NULL);

        if (!(ptbl->flags & PROBE_SDTBL_CMDDONE)) {
                if (probe_cmd_init (ptbl->ctx, model) != 0) {
                        _D("FAIL: SEAP cmd init failed\n");
                        return (NULL);
                }
                
                ptbl->flags |= PROBE_SDTBL_CMDDONE;
        }
        
        _D("search_probe\n");

        probe = search_probe (oval_object_get_subtype(object));
        if (probe == NULL) {
                errno = EOPNOTSUPP;
                return (NULL);
        }
        
        _D("oval -> sexp\n");
        
        /* create S-exp */
        sexp  = oval_object_to_sexp (probe->typestr, object);
        
        if (sexp == NULL) {
                _D("Can't translate OVAL object to S-exp\n");
                return (NULL);
        }

        psd = probe_sd_get (ptbl, oval_object_get_subtype (object));
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
                probe_sd_add (ptbl, oval_object_get_subtype (object), psd);
        }
        
        msg = SEAP_msg_new ();
        SEAP_msg_set (msg, sexp);
        
        puts ("--- msg ---");
        SEXP_fprintfa (stdout, sexp);
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
        SEXP_fprintfa (stdout, SEAP_msg_get(msg));
        puts ("\n----------");
        
        /* translate the result to oval state */
        sysch = sexp_to_oval_state (SEAP_msg_get(msg), object);
        
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
                obj    = oval_object_model_get_object (model, id_str);

                if (obj == NULL) {
                        _D("FAIL: can't find obj: id=%s\n", id_str);
                        oscap_free (id_str);
                        return (NULL);
                }
                
                if (oval_object_probe (obj, model) == NULL) {
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
        
        ste_list = SEXP_list_new (NULL);
        
        SEXP_list_foreach (id, sexp) {
                if (SEXP_stringp (id)) {
                        id_str = SEXP_string_cstr (id);
                        ste    = oval_object_model_get_state (model, id_str);

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
#endif
