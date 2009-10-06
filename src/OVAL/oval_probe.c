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
#include "probes/probe-api.h"
#include "oval_system_characteristics_impl.h"

#ifndef _A
#define _A(x) assert(x)
#endif

/* KEEP THIS LIST SORTED! (by subtype) */
static const oval_probe_t __probe_tbl[] = {
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

static int probe_cmd_init (SEAP_CTX_t *ctx, struct oval_definition_model *model)
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

static const oval_probe_t *search_probe (oval_subtype_t subtype)
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

static SEXP_t *oval_value_to_sexp (struct oval_value *val, oval_datatype_t dtype)
{
                SEXP_t *val_sexp;

                /* CHECK: check in val? */
                switch (dtype) {
                case OVAL_DATATYPE_VERSION:
                        val_sexp = SEXP_string_newf ("%s", oval_value_get_text (val));
                        SEXP_datatype_set (val_sexp, "version");
                        break;

                case OVAL_DATATYPE_EVR_STRING:
                        val_sexp = SEXP_string_newf ("%s", oval_value_get_text (val));
                        SEXP_datatype_set (val_sexp, "evr_str");
                        break;

                case OVAL_DATATYPE_STRING:
                        val_sexp = SEXP_string_newf ("%s", oval_value_get_text (val));
                        break;

                case OVAL_DATATYPE_FLOAT:
                        val_sexp = SEXP_number_newf (oval_value_get_float (val));
                        break;

                case OVAL_DATATYPE_INTEGER:
                        val_sexp = SEXP_number_newi_32 (oval_value_get_integer (val));
                        break;

                case OVAL_DATATYPE_BOOLEAN:
                        val_sexp = SEXP_number_newb (oval_value_get_boolean (val));
                        SEXP_datatype_set (val_sexp, "bool");
                        break;

                default:
                        val_sexp = NULL;
                        break;
                }

                return val_sexp;
}

static SEXP_t *oval_entity_to_sexp (struct oval_entity *ent)
{
        SEXP_t *elm, *elm_name;
        SEXP_t *r0, *r1, *r2;
        
        elm_name = SEXP_list_new (r0 = SEXP_string_newf ("%s", oval_entity_get_name (ent)),
                                  /* operation */
                                  r1 = SEXP_string_new (":operation", 10),
                                  r2 = SEXP_number_newu_32 (oval_entity_get_operation (ent)),
                                  NULL);
        
        SEXP_free (r0);
        SEXP_free (r1);
        SEXP_free (r2);
        
        elm = SEXP_list_new (NULL);

        /* var_ref */
        if (oval_entity_get_varref_type (ent) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
                struct oval_variable *var;

                var = oval_entity_get_variable (ent);

                SEXP_list_add (elm_name,
                               r0 = SEXP_string_new (":var_ref", 8));
                SEXP_free (r0);

                SEXP_list_add (elm_name,
                               r0 = SEXP_string_newf ("%s", oval_variable_get_id (var)));
                SEXP_free (r0);
                
                SEXP_list_add (elm, elm_name);
                SEXP_free (elm_name);
                
        } else { /* value */
                SEXP_t *val_sexp;

                SEXP_list_add (elm, elm_name);
                SEXP_free (elm_name);

                val_sexp = oval_value_to_sexp (oval_entity_get_value (ent),
                                               oval_entity_get_datatype (ent));

                if (val_sexp == NULL) {
                        SEXP_free (elm);
                        elm = NULL;
                } else {
                        SEXP_list_add (elm, val_sexp);
                        SEXP_free (val_sexp);
                }
        }

        return (elm);
}

static SEXP_t *oval_varref_to_sexp (struct oval_entity *entity)
{
        unsigned int val_cnt = 0;
        SEXP_t *val_lst, *val_sexp, *varref, *id_sexp, *val_cnt_sexp;
        oval_datatype_t dt;
        struct oval_variable *var;
        struct oval_value_iterator *vit;
        struct oval_value *val;

        val_lst = SEXP_list_new (NULL);
        dt = oval_entity_get_datatype (entity);

        var = oval_entity_get_variable (entity);
        vit = oval_variable_get_values (var);
        while (oval_value_iterator_has_more (vit)) {
                val = oval_value_iterator_next (vit);

                val_sexp = oval_value_to_sexp (val, dt);
                if (val_sexp == NULL) {
                        SEXP_free(val_lst);
                        oval_value_iterator_free (vit);
                        return NULL;
                }

                SEXP_list_add (val_lst, val_sexp);
                SEXP_free (val_sexp);
                ++val_cnt;
        }
        oval_value_iterator_free (vit);

        id_sexp = SEXP_string_newf ("%s", oval_variable_get_id (var));
        val_cnt_sexp = SEXP_number_newu (val_cnt);

        varref = SEXP_list_new (id_sexp, val_cnt_sexp, val_lst);

        SEXP_free (id_sexp);
        SEXP_free (val_cnt_sexp);
        SEXP_free (val_lst);

        return varref;
}

static SEXP_t *oval_set_to_sexp (struct oval_setobject *set)
{
        SEXP_t *elm, *elm_name;
        SEXP_t *r0, *r1, *r2;

        elm_name = SEXP_list_new (r0 = SEXP_string_new ("set", 3),
                                  /* operation */
                                  r1 = SEXP_string_new (":operation", 10),
                                  r2 = SEXP_number_newu_32 (oval_setobject_get_operation (set)),
                                  NULL);

        SEXP_free (r0);
        SEXP_free (r1);
        SEXP_free (r2);

        elm = SEXP_list_new (elm_name, NULL);
        SEXP_free (elm_name);
        
        switch (oval_setobject_get_type (set)) {
        case OVAL_SET_AGGREGATE: {
                struct oval_setobject_iterator *sit;
                struct oval_setobject *subset;

                sit = oval_setobject_get_subsets (set);

                while (oval_setobject_iterator_has_more (sit)) {
                        subset = oval_setobject_iterator_next (sit);
                        SEXP_list_add (elm, r0 = oval_set_to_sexp (subset));
                        SEXP_free (r0);
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

                        subelm = SEXP_list_new (r0 = SEXP_string_new ("obj_ref", 7),
                                                r1 = SEXP_string_newf ("%s", oval_object_get_id (obj)),
                                                NULL);
                        SEXP_free (r0);
                        SEXP_free (r1);
                        
                        SEXP_list_add (elm, subelm);
                        
                        SEXP_free (subelm);
                }
                
                oval_object_iterator_free (oit);

                while (oval_state_iterator_has_more (sit)) {
                        ste    = oval_state_iterator_next (sit);

                        subelm = SEXP_list_new (r0 = SEXP_string_new ("filter", 6),
                                                r1 = SEXP_string_newf ("%s", oval_state_get_id (ste)),
                                                NULL);
                        SEXP_free (r0);
                        SEXP_free (r1);
                        
                        SEXP_list_add (elm, subelm);

                        SEXP_free (subelm);
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
        SEXP_t *r0;
        
        struct oval_behavior *behavior;
        
        elm_name = SEXP_list_new (r0 = SEXP_string_newf ("behaviors"),
                                  NULL);
        SEXP_free (r0);

        while (oval_behavior_iterator_has_more (bit)) {
                behavior  = oval_behavior_iterator_next (bit);
                attr_name = oval_behavior_get_key(behavior);
                attr_val  = oval_behavior_get_value(behavior);

                SEXP_list_add(elm_name, r0 = SEXP_string_newf(":%s", attr_name));
                SEXP_free (r0);
                
                if (attr_val != NULL) {
                        SEXP_list_add(elm_name, r0 = SEXP_string_new(attr_val, strlen (attr_val)));
                        SEXP_free (r0);
                }
        }
        
        oval_behavior_iterator_free (bit);
        
        r0 = SEXP_list_new (elm_name, NULL);
        SEXP_free (elm_name);
        
        return (r0);
}

SEXP_t *oval_object_to_sexp (const char *typestr, struct oval_object *object)
{
        SEXP_t *obj_sexp, *obj_name, *elm, *varrefs, *ent_lst, *lst, *stmp;
        SEXP_t *r0, *r1, *r2;

        struct oval_object_content_iterator *cit;
        struct oval_behavior_iterator *bit;
        struct oval_object_content *content;
        struct oval_entity *entity;

        /*
         * Object name & attributes (id)
         */
        obj_name = SEXP_list_new (r0 = SEXP_string_newf ("%s_object", typestr),
                                  r1 = SEXP_string_new (":id", 3),
                                  r2 = SEXP_string_newf ("%s", oval_object_get_id (object)),
                                  NULL);
        SEXP_free (r0);
        SEXP_free (r1);
        SEXP_free (r2);
        
        obj_sexp = SEXP_list_new (obj_name, NULL);

        SEXP_free (obj_name);
        
        /*
         * Object content
         */

        ent_lst = SEXP_list_new (NULL);
        varrefs = NULL;

        cit = oval_object_get_object_content (object);
        while (oval_object_content_iterator_has_more (cit)) {
                oval_check_t ochk;

                content = oval_object_content_iterator_next (cit);
                elm = NULL;
                lst = ent_lst;
                
                switch (oval_object_content_get_type (content)) {
                case OVAL_OBJECTCONTENT_ENTITY:
                        entity = oval_object_content_get_entity (content);
                        elm = oval_entity_to_sexp (entity);

                        if (elm == NULL)
                                break;

                        ochk = oval_object_content_get_varCheck(content);
                        if (ochk != OVAL_CHECK_UNKNOWN) {
                                probe_ent_attr_add(elm, "var_check",
                                                   r0 = SEXP_string_newf("%s", oval_check_get_text(ochk)));
                                SEXP_free (r0);
                        }

                        if (oval_entity_get_varref_type (entity) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
                                stmp = oval_varref_to_sexp (entity);

                                if (stmp == NULL) {
                                        SEXP_free (elm);
                                        elm = NULL;
                                        break;
                                }

                                if (varrefs == NULL) {
                                        varrefs = SEXP_list_new (r0 = SEXP_string_new ("varrefs", 7));
                                        SEXP_free (r0);
                                }
                                SEXP_list_add (varrefs, stmp);

                                lst = obj_sexp;
                        }
                        break;

                case OVAL_OBJECTCONTENT_SET:
                        elm = oval_set_to_sexp (oval_object_content_get_setobject (content));
                        break;

                case OVAL_OBJECTCONTENT_UNKNOWN:
                        break;
                }

                if (elm == NULL) {
                        SEXP_free (obj_sexp);
                        SEXP_free (ent_lst);
                        if (varrefs != NULL)
                                SEXP_free (varrefs);
                        oval_object_content_iterator_free (cit);

                        return (NULL);
                }

                SEXP_list_add (lst, elm);
                SEXP_free (elm);
        }

        if (varrefs != NULL) {
                SEXP_list_add (obj_sexp, varrefs);
                SEXP_free (varrefs);
        }
        stmp = SEXP_list_join (obj_sexp, ent_lst);
        SEXP_free (obj_sexp);
        SEXP_free (ent_lst);
        obj_sexp = stmp;

        oval_object_content_iterator_free (cit);

        /*
         * Object behaviors
         */

        bit = oval_object_get_behaviors (object);
        if (oval_behavior_iterator_has_more (bit)) {
                elm = oval_behaviors_to_sexp (bit);
                SEXP_list_add (obj_sexp, elm);
                SEXP_free (elm);
        }

        return (obj_sexp);
}

static SEXP_t *oval_state_to_sexp (struct oval_state *state)
{
        SEXP_t *ste, *ste_name, *ste_ent;
        SEXP_t *r0, *r1, *r2;
        char   buffer[128];
        size_t buflen;
        const oval_probe_t *probe;
        struct oval_state_content_iterator *contents;
        
        probe = search_probe (oval_state_get_subtype (state));

        if (probe == NULL) {
                _D("FAIL: unknown subtype: %d\n", oval_state_get_subtype (state));
                return (NULL);
        }

        buflen = snprintf (buffer, sizeof buffer, "%s_state", probe->typestr);
        _A(buflen < sizeof buffer);
        
        ste_name = SEXP_list_new (r0 = SEXP_string_new (buffer, buflen),
                                  r1 = SEXP_string_new  (":id", 3),
                                  r2 = SEXP_string_newf ("%s", oval_state_get_id (state)),
                                  NULL);

        SEXP_free (r0);
        SEXP_free (r1);
        SEXP_free (r2);

        ste = SEXP_list_new (ste_name, NULL);
        
        SEXP_free (ste_name);

        //entities = oval_state_entities (state);

        contents = oval_state_get_contents(state);
        
        /*
        while (oval_entity_iterator_has_more (entities)) {
                ste_ent = oval_entity_to_sexp (oval_entity_iterator_next (entities));
                SEXP_list_add (ste, ste_ent);
        }
        */
        while(oval_state_content_iterator_has_more(contents)){
                oval_check_t ochk;
        	struct oval_state_content *content = oval_state_content_iterator_next(contents);
                
                ste_ent = oval_entity_to_sexp (oval_state_content_get_entity(content));

                if (ste_ent == NULL) {
                        SEXP_free (ste);
                        ste = NULL;
                        break;
                }

                ochk = oval_state_content_get_var_check(content);
                if (ochk != OVAL_CHECK_UNKNOWN) {
                        probe_ent_attr_add(ste_ent, "var_check",
                                           r0 = SEXP_string_newf("%s", oval_check_get_text(ochk)));
                        SEXP_free (r0);
                }
                
                ochk = oval_state_content_get_ent_check(content);
                if (ochk != OVAL_CHECK_UNKNOWN) {
                        probe_ent_attr_add(ste_ent, "entity_check",
                                           r0 = SEXP_string_newf("%s", oval_check_get_text(ochk)));
                        SEXP_free (r0);
                }

                SEXP_list_add (ste, ste_ent);
                SEXP_free (ste_ent);
        }
        
        oval_state_content_iterator_free(contents);

        return (ste);
}


static struct oval_sysitem* oval_sysitem_from_sexp(SEXP_t *sexp)
{
	_A(sexp);
	SEXP_t *sval;
	char *key;
	char *val;

        key = probe_ent_getname (sexp);

	if (!key)
		return NULL;

	sval = probe_ent_getval (sexp);
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

	int datatype = probe_ent_getdatatype(sexp);
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

static struct oval_sysdata *oval_sysdata_from_sexp(SEXP_t *sexp)
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

struct oval_syschar *oval_object_probe (struct oval_object *object, struct oval_definition_model *model)
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
        sexp = oval_object_to_sexp (probe->typestr, object);
        
        if (sexp == NULL) {
                _D("Can't translate OVAL object to S-exp\n");
                return (NULL);
        }

        puts ("--- msg ---");
        SEXP_fprintfa (stdout, sexp);
        puts ("\n----------");
        
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
        struct oval_definition_model *model = (struct oval_definition_model *)arg;

        if (SEXP_stringp (sexp)) {
                id_str = SEXP_string_cstr (sexp);
                obj    = oval_definition_model_get_object (model, id_str);

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
        struct oval_definition_model *model = (struct oval_definition_model *)arg;

        ste_list = SEXP_list_new (NULL);

        SEXP_list_foreach (id, sexp) {
                if (SEXP_stringp (id)) {
                        id_str = SEXP_string_cstr (id);
                        ste    = oval_definition_model_get_state (model, id_str);

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
