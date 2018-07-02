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

#include "oval_probe_impl.h"
#include "oval_system_characteristics_impl.h"
#include "common/util.h"
#include "common/bfind.h"
#include "common/debug_priv.h"

#include "_oval_probe_session.h"
#include "_oval_probe_handler.h"
#include "oval_probe_ext.h"
#include "collectVarRefs_impl.h"

#ifdef _WIN32
#define X_OK 0
#endif

/*
 * Library side entity name cache. Initialization needs to be
 * thread-safe and is done by oval_probe_session_new. Freeing
 * of memory used by this cache is done at exit using a hook
 * registered with atexit().
 */
probe_ncache_t  *OSCAP_GSYM(ncache) = NULL;
struct id_desc_t OSCAP_GSYM(id_desc);

#define __ERRBUF_SIZE 128

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
	const char *type_name;
        oval_ph_t *ph;
	struct oval_string_map *vm;
	struct oval_syschar_model *model;
	int ret;

	oid = oval_object_get_id(object);
	model = psess->sys_model;

	type = oval_object_get_subtype(object);
	type_name = oval_subtype_get_text(type);
	dI("Querying %s object '%s', flags: %u.", type_name, oid, flags);

	sysc = oval_syschar_model_get_syschar(model, oid);
	if (sysc != NULL) {
		int variable_instance_hint = oval_syschar_get_variable_instance_hint(sysc);
		if (oval_syschar_get_variable_instance_hint(sysc) != oval_syschar_get_variable_instance(sysc)) {
			dI("Creating another syschar for variable_instance=%d)", variable_instance_hint);
			sysc = oval_syschar_new(model, object);
			oval_syschar_set_variable_instance(sysc, variable_instance_hint);
			oval_syschar_set_variable_instance_hint(sysc, variable_instance_hint);
		}
		else {
			oval_syschar_collection_flag_t sc_flg = oval_syschar_get_flag(sysc);
			const char *flag_text = oval_syschar_collection_flag_get_text(sc_flg);
			dI("System characteristics for %s_object '%s' already exist, flag: %s.", type_name, oid, flag_text);

			if (sc_flg != SYSCHAR_FLAG_UNKNOWN || (flags & OVAL_PDFLAG_NOREPLY)) {
				if (out_syschar)
					*out_syschar = sysc;
				return 0;
			}
		}
	} else {
		dI("Creating new syschar for %s_object '%s'.", type_name, oid);
		sysc = oval_syschar_new(model, object);
	}

	if (out_syschar)
		*out_syschar = sysc;

	ph = oval_probe_handler_get(psess->ph, type);

        if (ph == NULL) {
                char *msg = "OVAL object not supported.";

		dW("%s", msg);
		oval_syschar_add_new_message(sysc, msg, OVAL_MESSAGE_LEVEL_WARNING);
		oval_syschar_set_flag(sysc, SYSCHAR_FLAG_NOT_COLLECTED);

		return 1;
        }

	if ((ret = oval_probe_ext_handler(type, ph->uptr, PROBE_HANDLER_ACT_EVAL, sysc, flags)) != 0) {
		return ret;
	}

	if (!(flags & OVAL_PDFLAG_NOREPLY)) {
		vm = oval_string_map_new();
		oval_obj_collect_var_refs(object, vm);
		_syschar_add_bindings(sysc, vm);
		oval_string_map_free(vm, NULL);
	}

	return 0;
}

int oval_probe_query_sysinfo(oval_probe_session_t *sess, struct oval_sysinfo **out_sysinfo)
{
	struct oval_sysinfo *sysinf;
        oval_ph_t *ph;
	int ret;

	dI("Querying system information.");

	ph = oval_probe_handler_get(sess->ph, OVAL_INDEPENDENT_SYSCHAR_SUBTYPE);

        if (ph == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, "OVAL object not supported");
		return(-1);
        }

        if (ph->func == NULL) {
                oscap_seterr (OSCAP_EFAMILY_OVAL, "OVAL object not correctly defined");
		return(-1);
        }

        sysinf = NULL;

	ret = oval_probe_sys_handler(OVAL_INDEPENDENT_SYSCHAR_SUBTYPE, ph->uptr, PROBE_HANDLER_ACT_EVAL, NULL, &sysinf, 0);
	if (ret != 0)
		return(ret);

	*out_sysinfo = sysinf;
	return(0);
}

static int oval_probe_query_var_ref(oval_probe_session_t *sess, struct oval_state *state)
{
	struct oval_state_content_iterator *contents = oval_state_get_contents(state);
	while (oval_state_content_iterator_has_more(contents)) {
		struct oval_state_content *content = oval_state_content_iterator_next(contents);
		struct oval_entity * entity = oval_state_content_get_entity(content);
		if (oval_entity_get_varref_type(entity) == OVAL_ENTITY_VARREF_ATTRIBUTE) {
			oval_syschar_collection_flag_t flag;
			struct oval_variable *var = oval_entity_get_variable(entity);
			const char *state_id = oval_state_get_id(state);
			oval_variable_type_t var_type = oval_variable_get_type(var);
			const char *var_type_text = oval_variable_type_get_text(var_type);
			const char *var_id = oval_variable_get_id(var);
			dI("State '%s' references %s '%s'.", state_id,
			   var_type_text, var_id);

			int ret = oval_probe_query_variable(sess, var);
			if (ret == -1) {
				oval_state_content_iterator_free(contents);
				return ret;
			}

			flag = oval_variable_get_collection_flag(var);
			switch (flag) {
			case SYSCHAR_FLAG_COMPLETE:
			case SYSCHAR_FLAG_INCOMPLETE:
				break;
			default:
				oval_state_content_iterator_free(contents);
				return 0;
			}
		}
	}
	oval_state_content_iterator_free(contents);
	return 1;
}

int oval_probe_query_test(oval_probe_session_t *sess, struct oval_test *test)
{
	struct oval_object *object;
	struct oval_state_iterator *ste_itr;
	int ret;
	oval_subtype_t test_subtype, object_subtype;

	object = oval_test_get_object(test);
	if (object == NULL)
		return 0;
	test_subtype = oval_test_get_subtype(test);
	object_subtype = oval_object_get_subtype(object);
	if (test_subtype != object_subtype) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "%s_test '%s' is not compatible with %s_object '%s'.",
				oval_subtype_to_str(test_subtype), oval_test_get_id(test),
				oval_subtype_to_str(object_subtype), oval_object_get_id(object));
		return 0;
	}

	/* probe object */
	ret = oval_probe_query_object(sess, object, 0, NULL);
	if (ret == -1)
		return ret;
	/* probe objects referenced like this: test->state->variable->object */
	ste_itr = oval_test_get_states(test);
	while (oval_state_iterator_has_more(ste_itr)) {
		struct oval_state *state = oval_state_iterator_next(ste_itr);
		ret = oval_probe_query_var_ref(sess, state);
		if (ret < 1) {
			oval_state_iterator_free(ste_itr);
			return ret;
		}
	}
	oval_state_iterator_free(ste_itr);

	return 0;
}

