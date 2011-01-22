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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#if defined(OSCAP_THREAD_SAFE)
# include <pthread.h>
#endif

#include "oval_probe.h"
#include "oval_system_characteristics.h"
#include "common/_error.h"
#include "common/assume.h"

#include "oval_probe_impl.h"
#include "oval_system_characteristics_impl.h"
#include "common/util.h"
#include "common/bfind.h"
#include "common/debug_priv.h"

#include "_oval_probe_session.h"
#include "_oval_probe_handler.h"

/* Sorted by subtype (first column) */
oval_subtypedsc_t __s2n_tbl[] = {
        /*     2 */ {OVAL_SUBTYPE_SYSINFO,                  "system_info"         },
	/*  7001 */ {OVAL_INDEPENDENT_FAMILY,               "family"              },
        /*  7002 */ {OVAL_INDEPENDENT_FILE_MD5,             "filemd5"             },
        /*  7003 */ {OVAL_INDEPENDENT_FILE_HASH,            "filehash"            },
        /*  7004 */ {OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, "environmentvariable" },
	/*  7006 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54"   },
	/*  7007 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT,    "textfilecontent"     },
        /*  7009 */ {OVAL_INDEPENDENT_VARIABLE,             "variable"            },
	/*  7010 */ {OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent"      },
	/*  9001 */ {OVAL_LINUX_DPKG_INFO,                  "dpkginfo"            },
	/*  9002 */ {OVAL_LINUX_INET_LISTENING_SERVERS,     "inetlisteningservers"},
	/*  9003 */ {OVAL_LINUX_RPM_INFO,                   "rpminfo"             },
	/* 13001 */ {OVAL_UNIX_FILE,                        "file"                },
	/* 13003 */ {OVAL_UNIX_INTERFACE,                   "interface"           },
	/* 13004 */ {OVAL_UNIX_PASSWORD,                    "password"            },
	/* 13005 */ {OVAL_UNIX_PROCESS,                     "process"             },
	/* 13006 */ {OVAL_UNIX_RUNLEVEL,                    "runlevel"            },
	/* 13008 */ {OVAL_UNIX_SHADOW,                      "shadow"              },
	/* 13009 */ {OVAL_UNIX_UNAME,                       "uname"               }
};

#define __s2n_tbl_count (sizeof __s2n_tbl / sizeof(oval_subtypedsc_t))

static int __s2n_tbl_cmp(oval_subtype_t *type, oval_subtypedsc_t *dsc)
{
        return (*type - dsc->type);
}

/* Sorted by name (second column) */
oval_subtypedsc_t __n2s_tbl[] = {
	/*  9001 */ {OVAL_LINUX_DPKG_INFO,                  "dpkginfo"            },
        /*  7004 */ {OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE, "environmentvariable" },
	/*  7001 */ {OVAL_INDEPENDENT_FAMILY,               "family"              },
	/* 13001 */ {OVAL_UNIX_FILE,                        "file"                },
        /*  7003 */ {OVAL_INDEPENDENT_FILE_HASH,            "filehash"            },
        /*  7002 */ {OVAL_INDEPENDENT_FILE_MD5,             "filemd5"             },
	/*  9002 */ {OVAL_LINUX_INET_LISTENING_SERVERS,     "inetlisteningservers"},
	/* 13003 */ {OVAL_UNIX_INTERFACE,                   "interface"           },
	/* 13004 */ {OVAL_UNIX_PASSWORD,                    "password"            },
	/* 13005 */ {OVAL_UNIX_PROCESS,                     "process"             },
	/*  9003 */ {OVAL_LINUX_RPM_INFO,                   "rpminfo"             },
	/* 13006 */ {OVAL_UNIX_RUNLEVEL,                    "runlevel"            },
	/* 13008 */ {OVAL_UNIX_SHADOW,                      "shadow"              },
        /*     2 */ {OVAL_SUBTYPE_SYSINFO,                  "system_info"         },
	/*  7007 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT,    "textfilecontent"     },
	/*  7006 */ {OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, "textfilecontent54"   },
	/* 13009 */ {OVAL_UNIX_UNAME,                       "uname"               },
        /*  7009 */ {OVAL_INDEPENDENT_VARIABLE,             "variable"            },
	/*  7010 */ {OVAL_INDEPENDENT_XML_FILE_CONTENT,     "xmlfilecontent"      }
};

#define __n2s_tbl_count (sizeof __n2s_tbl / sizeof(oval_subtypedsc_t))

static int __n2s_tbl_cmp(const char *name, oval_subtypedsc_t *dsc)
{
        return strcmp(name, dsc->name);
}

#if defined(ENABLE_PROBES)
/*
 * Library side entity name cache. Initialization needs to be
 * thread-safe and is done by oval_probe_session_new. Freeing
 * of memory used by this cache is done at exit using a hook
 * registered with atexit().
 */
encache_t *OSCAP_GSYM(encache) = NULL;
struct id_desc_t OSCAP_GSYM(id_desc);
#endif

#define __ERRBUF_SIZE 128

const char *oval_subtype2str(oval_subtype_t subtype)
{
        oval_subtypedsc_t *d;

        d = oscap_bfind(__s2n_tbl, __s2n_tbl_count, sizeof(oval_subtypedsc_t), &subtype,
                        (int(*)(void *, void *))__s2n_tbl_cmp);

        return (d == NULL ? NULL : d->name);
}

oval_subtype_t oval_str2subtype(const char *str)
{
        oval_subtypedsc_t *d;

        d = oscap_bfind(__n2s_tbl, __n2s_tbl_count, sizeof(oval_subtypedsc_t), (void *)str,
                        (int(*)(void *, void *))__n2s_tbl_cmp);

        return (d == NULL ? OVAL_SUBTYPE_UNKNOWN : d->type);
}

static void _obj_collect_var_refs(struct oval_object *obj, struct oval_string_map *vm);
static void _var_collect_var_refs(struct oval_variable *var, struct oval_string_map *vm);

static void _comp_collect_var_refs(struct oval_component *comp, struct oval_string_map *vm)
{
	struct oval_object *obj;
	struct oval_variable *var;
	struct oval_component_iterator *cmp_itr;

	switch (oval_component_get_type(comp)) {
	case OVAL_COMPONENT_OBJECTREF:
		obj = oval_component_get_object(comp);
		_obj_collect_var_refs(obj, vm);
		break;
	case OVAL_COMPONENT_VARREF:
		var = oval_component_get_variable(comp);
		_var_collect_var_refs(var, vm);
		break;
	case OVAL_FUNCTION_ARITHMETIC:
	case OVAL_FUNCTION_BEGIN:
	case OVAL_FUNCTION_CONCAT:
	case OVAL_FUNCTION_END:
	case OVAL_FUNCTION_ESCAPE_REGEX:
	case OVAL_FUNCTION_REGEX_CAPTURE:
	case OVAL_FUNCTION_SPLIT:
	case OVAL_FUNCTION_SUBSTRING:
	case OVAL_FUNCTION_TIMEDIF:
		cmp_itr = oval_component_get_function_components(comp);
		while (oval_component_iterator_has_more(cmp_itr)) {
			struct oval_component *cmp;

			cmp = oval_component_iterator_next(cmp_itr);
			_comp_collect_var_refs(cmp, vm);
		}
		oval_component_iterator_free(cmp_itr);
		break;
	default:
		break;
	}
}

static void _var_collect_var_refs(struct oval_variable *var, struct oval_string_map *vm)
{
	char *var_id;

	var_id = oval_variable_get_id(var);
	oval_string_map_put(vm, var_id, var);

	if (oval_variable_get_type(var) == OVAL_VARIABLE_LOCAL) {
		struct oval_component *comp;

		comp = oval_variable_get_component(var);
		_comp_collect_var_refs(comp, vm);
	}
}

static void _ent_collect_var_refs(struct oval_entity *ent, struct oval_string_map *vm)
{
	if (oval_entity_get_varref_type(ent) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
		struct oval_variable *var;

		var = oval_entity_get_variable(ent);
		_var_collect_var_refs(var, vm);
	}
}

static void _ste_collect_var_refs(struct oval_state *ste, struct oval_string_map *vm)
{
	struct oval_state_content_iterator *cont_itr;

	cont_itr = oval_state_get_contents(ste);
	while (oval_state_content_iterator_has_more(cont_itr)) {
		struct oval_state_content *cont;
		struct oval_entity *ent;

		cont = oval_state_content_iterator_next(cont_itr);
		ent = oval_state_content_get_entity(cont);
		_ent_collect_var_refs(ent, vm);
	}
	oval_state_content_iterator_free(cont_itr);
}

static void _set_collect_var_refs(struct oval_setobject *set, struct oval_string_map *vm)
{
	struct oval_setobject_iterator *subset_itr;
	struct oval_object_iterator *obj_itr;
	struct oval_filter_iterator *fil_itr;

	switch (oval_setobject_get_type(set)) {
	case OVAL_SET_AGGREGATE:
		subset_itr = oval_setobject_get_subsets(set);
		while (oval_setobject_iterator_has_more(subset_itr)) {
			struct oval_setobject *subset;

			subset = oval_setobject_iterator_next(subset_itr);
			_set_collect_var_refs(subset, vm);
		}
		oval_setobject_iterator_free(subset_itr);
		break;
	case OVAL_SET_COLLECTIVE:
		obj_itr = oval_setobject_get_objects(set);
		while (oval_object_iterator_has_more(obj_itr)) {
			struct oval_object *obj;

			obj = oval_object_iterator_next(obj_itr);
			_obj_collect_var_refs(obj, vm);
		}
		oval_object_iterator_free(obj_itr);
		fil_itr = oval_setobject_get_filters(set);
		while (oval_filter_iterator_has_more(fil_itr)) {
			struct oval_filter *fil;
			struct oval_state *ste;

			fil = oval_filter_iterator_next(fil_itr);
			ste = oval_filter_get_state(fil);
			_ste_collect_var_refs(ste, vm);
		}
		oval_filter_iterator_free(fil_itr);
		break;
	default:
		break;
	}
}

static void _obj_collect_var_refs(struct oval_object *obj, struct oval_string_map *vm)
{
	struct oval_object_content_iterator *cont_itr;

	cont_itr = oval_object_get_object_contents(obj);
	while (oval_object_content_iterator_has_more(cont_itr)) {
		struct oval_object_content *cont;
		struct oval_entity *ent;
		struct oval_setobject *set;

		cont = oval_object_content_iterator_next(cont_itr);

		switch (oval_object_content_get_type(cont)) {
		case OVAL_OBJECTCONTENT_ENTITY:
			ent = oval_object_content_get_entity(cont);
			_ent_collect_var_refs(ent, vm);
			break;
		case OVAL_OBJECTCONTENT_SET:
			set = oval_object_content_get_setobject(cont);
			_set_collect_var_refs(set, vm);
			break;
		default:
			break;
		}
	}
	oval_object_content_iterator_free(cont_itr);
}

static void _syschar_add_bindings(struct oval_syschar *sc, struct oval_string_map *vm)
{
	struct oval_iterator *var_itr;

	var_itr = oval_string_map_values(vm);
	while (oval_collection_iterator_has_more(var_itr)) {
		struct oval_variable *var;
		struct oval_value_iterator *val_itr;
		struct oval_variable_binding *binding;

		var = oval_collection_iterator_next(var_itr);
		binding = oval_variable_binding_new(var, NULL);

		val_itr = oval_variable_get_values(var);
		while (oval_value_iterator_has_more(val_itr)) {
			struct oval_value *val;
			char *txt;

			val = oval_value_iterator_next(val_itr);
			txt = oval_value_get_text(val);
			txt = oscap_strdup(txt);
			oval_variable_binding_add_value(binding, txt);
		}
		oval_value_iterator_free(val_itr);

		oval_syschar_add_variable_binding(sc, binding);
	}
	oval_collection_iterator_free(var_itr);
}

int oval_probe_query_object(oval_probe_session_t *psess, struct oval_object *object, int flags, struct oval_syschar **out_syschar)
{
	char *oid;
	struct oval_syschar *sysc;
        oval_subtype_t type;
        oval_ph_t *ph;
	struct oval_string_map *vm;
	struct oval_syschar_model *model;
	int ret;

	oid = oval_object_get_id(object);
	model = psess->sys_model;

	sysc = oval_syschar_model_get_syschar(model, oid);
	if (sysc != NULL) {
		if (out_syschar)
			*out_syschar = sysc;
		return 0;
	}

	sysc = oval_syschar_new(model, object);
        type = oval_object_get_subtype(object);
        ph   = oval_probe_handler_get(psess->ph, type);

        if (ph == NULL) {
                char *msg = "OVAL object not supported.\n";

		dW(msg);
		oval_syschar_add_new_message(sysc, msg, OVAL_MESSAGE_LEVEL_WARNING);
		oval_syschar_set_flag(sysc, SYSCHAR_FLAG_NOT_COLLECTED);

		return 1;
        }

	if ((ret = ph->func(type, ph->uptr, PROBE_HANDLER_ACT_EVAL, sysc, flags)) != 0) {
		return ret;
	}

	if (!(flags & OVAL_PDFLAG_NOREPLY)) {
		vm = oval_string_map_new();
		_obj_collect_var_refs(object, vm);
		_syschar_add_bindings(sysc, vm);
		oval_string_map_free(vm, NULL);
	}

	if (out_syschar)
		*out_syschar = sysc;

	return 0;
}

int oval_probe_query_sysinfo(oval_probe_session_t *sess, struct oval_sysinfo **out_sysinfo)
{
	struct oval_sysinfo *sysinf;
        oval_ph_t *ph;
	int ret;

        ph = oval_probe_handler_get(sess->ph, OVAL_SUBTYPE_SYSINFO);

        if (ph == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, OVAL_EPROBENOTSUPP, "OVAL object not supported");
		return(-1);
        }

        sysinf = NULL;

	ret = ph->func(OVAL_SUBTYPE_SYSINFO, ph->uptr, PROBE_HANDLER_ACT_EVAL, NULL, &sysinf, 0);
	if (ret != 0)
		return(ret);

	*out_sysinfo = sysinf;
	return(0);
}

static int oval_probe_query_criteria(oval_probe_session_t *sess, struct oval_criteria_node *cnode);

int oval_probe_query_objects(oval_probe_session_t *sess)
{
	struct oval_syschar_model * syschar_model;
	struct oval_definition_model *definition_model;

	syschar_model = sess->sys_model;
	definition_model = oval_syschar_model_get_definition_model(syschar_model);

	if (definition_model) {
		struct oval_object_iterator *objects = oval_definition_model_get_objects(definition_model);
		while (oval_object_iterator_has_more(objects)) {
			int ret;
			struct oval_object *object = oval_object_iterator_next(objects);
			ret = oval_probe_query_object(sess, object, 0, NULL);
			if (ret != 0) {
				oval_object_iterator_free(objects);
				return ret;
			}
		}
		oval_object_iterator_free(objects);
	}
	return 0;
}

int oval_probe_query_definition(oval_probe_session_t *sess, const char *id) {

	struct oval_syschar_model * syschar_model;
        struct oval_definition_model *definition_model;
	struct oval_definition *definition;
	int ret;

	syschar_model = sess->sys_model;
        definition_model = oval_syschar_model_get_definition_model(syschar_model);
	definition = oval_definition_model_get_definition(definition_model, id);
	if (definition == NULL)
		return -1;

	struct oval_criteria_node * cnode = oval_definition_get_criteria(definition);
	if (cnode == NULL)
		return -1;

	ret = oval_probe_query_criteria(sess, cnode);

	return ret;
}

/**
 * @returns 0 on success
 */
static int oval_probe_query_criteria(oval_probe_session_t *sess, struct oval_criteria_node *cnode) {
	int ret;

	switch (oval_criteria_node_get_type(cnode)) {
	/* Criterion node is the final node that has a reference to a test */
	case OVAL_NODETYPE_CRITERION:{
		/* There should be a test .. */
		struct oval_test *test;
		struct oval_object *object;
		struct oval_state_iterator *ste_itr;

		test = oval_criteria_node_get_test(cnode);
		if (test == NULL)
			return 0;
		object = oval_test_get_object(test);
		if (object == NULL)
			return 0;
		/* probe object */
		ret = oval_probe_query_object(sess, object, 0, NULL);
		if (ret != 0)
			return ret;
		/* probe objects referenced like this: test->state->variable->object */
		ste_itr = oval_test_get_states(test);
		while (oval_state_iterator_has_more(ste_itr)) {
			struct oval_state *state = oval_state_iterator_next(ste_itr);
			struct oval_state_content_iterator *contents = oval_state_get_contents(state);
			while (oval_state_content_iterator_has_more(contents)) {
				struct oval_state_content *content = oval_state_content_iterator_next(contents);
				struct oval_entity * entity = oval_state_content_get_entity(content);
				if (oval_entity_get_varref_type(entity) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
					oval_syschar_collection_flag_t flag;
					struct oval_variable *var = oval_entity_get_variable(entity);

					ret = oval_probe_query_variable(sess, var);
					if (ret != 0) {
						oval_state_content_iterator_free(contents);
						oval_state_iterator_free(ste_itr);
						return ret;
					}

					flag = oval_variable_get_collection_flag(var);
					switch (flag) {
					case SYSCHAR_FLAG_COMPLETE:
					case SYSCHAR_FLAG_INCOMPLETE:
						break;
					default:
						oval_state_content_iterator_free(contents);
						oval_state_iterator_free(ste_itr);
						return 0;
					}
				}
			}
			oval_state_content_iterator_free(contents);
		}
		oval_state_iterator_free(ste_itr);

		return 0;

		}
		break;
                /* Criteria node is type of set that contains more criterias. Criteria node
                 * child can be also type of criteria, criterion or extended definition */
        case OVAL_NODETYPE_CRITERIA:{
                        /* group of criterion nodes, get subnodes, continue recursive */
                        struct oval_criteria_node_iterator *cnode_it = oval_criteria_node_get_subnodes(cnode);
                        if (cnode_it == NULL)
                                return 0;
                        /* we have subnotes */
                        struct oval_criteria_node *node;
                        while (oval_criteria_node_iterator_has_more(cnode_it)) {
                                node = oval_criteria_node_iterator_next(cnode_it);
                                ret = oval_probe_query_criteria(sess, node);
                                if (ret != 0) {
                                        oval_criteria_node_iterator_free(cnode_it);
                                        return ret;
                                }
                        }
                        oval_criteria_node_iterator_free(cnode_it);
			return 0;
                }
                break;
                /* Extended definition contains reference to definition, we need criteria of this
                 * definition to be evaluated completely */
        case OVAL_NODETYPE_EXTENDDEF:{
                        struct oval_definition *oval_def = oval_criteria_node_get_definition(cnode);
			struct oval_criteria_node *node =  oval_definition_get_criteria(oval_def);
                        return oval_probe_query_criteria(sess, node);
                }
                break;
        case OVAL_NODETYPE_UNKNOWN:
                break;
        }

	/* we shouldn't get here */
        return -1;
}
