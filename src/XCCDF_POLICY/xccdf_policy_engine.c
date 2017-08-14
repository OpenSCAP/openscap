/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Authors:
 *    Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/util.h"
#include "common/list.h"
#include "common/_error.h"
#include "public/xccdf_policy.h"
#include "xccdf_policy_engine_priv.h"

struct xccdf_policy_engine {
	char *system;                           ///< Identificator of checking engine
	xccdf_policy_engine_eval_fn callback;   ///< format of callback function
	void * usr;                             ///< User data structure
	xccdf_policy_engine_query_fn query_fn;  ///< query callback function
};

struct xccdf_policy_engine *xccdf_policy_engine_new(char *sys, xccdf_policy_engine_eval_fn eval_fn, void *usr, xccdf_policy_engine_query_fn query_fn)
{
	struct xccdf_policy_engine *engine = malloc(sizeof(struct xccdf_policy_engine));
        if (engine != NULL) {
		engine->system = sys;
		engine->callback = eval_fn;
		engine->usr = usr;
		engine->query_fn = query_fn;
	}
	return engine;
}

bool xccdf_policy_engine_filter(struct xccdf_policy_engine *engine, const char *sysname)
{
	return oscap_strcmp(engine->system, sysname) == 0;
}

xccdf_test_result_type_t xccdf_policy_engine_eval(struct xccdf_policy_engine *engine, struct xccdf_policy *policy, const char *definition_id, const char *href_id, struct oscap_list *value_bindings, struct xccdf_check_import_iterator *check_import_it)
{
	xccdf_test_result_type_t ret = XCCDF_RESULT_NOT_CHECKED;
	if (engine == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XCCDF, "Unknown callback for given checking system. Set callback first");
	}
	else {
		struct xccdf_value_binding_iterator * binding_it = (struct xccdf_value_binding_iterator *) oscap_iterator_new(value_bindings);
		ret = engine->callback(policy, NULL, definition_id, href_id, binding_it, check_import_it, engine->usr);
		if (binding_it != NULL)
			xccdf_value_binding_iterator_free(binding_it);
	}
	return ret;
}

struct oscap_stringlist *xccdf_policy_engine_query(struct xccdf_policy_engine *engine, xccdf_policy_engine_query_t query_type, void *query_data)
{
	if (engine->query_fn == NULL)
		return NULL;
	return (struct oscap_stringlist *) engine->query_fn(engine->usr, query_type, query_data);
}
