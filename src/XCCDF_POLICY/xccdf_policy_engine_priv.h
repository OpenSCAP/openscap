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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *   Simon Lukasik <slukasik@redhat.com>
 *
 */

#pragma once
#ifndef _OSCAP_XCCDF_POLICY_ENGINE_PRIV_H
#define _OSCAP_XCCDF_POLICY_ENGINE_PRIV_H

#include "common/util.h"
#include "common/list.h"
#include "public/xccdf_policy.h"


/**
 * Typedef of callback structure with system identificator, callback function and usr data (optional)
 * On evaluation action will be selected checking system and appropriate callback registred by tool
 * for that system.
 */
struct xccdf_policy_engine;

/**
 * Create new checking engine structure
 * @param sys System name of the checking engine
 * @param eval_fn The eval function of newly created checking engine
 * @param usr User data structure
 * @param query_fn The query function of newly created checking engine
 * @returns newly created checking engine
 */
struct xccdf_policy_engine *xccdf_policy_engine_new(char *sys, xccdf_policy_engine_eval_fn eval_fn, void *usr, xccdf_policy_engine_query_fn query_fn);

/**
 * Filter function returning true if given callback is for the given checking engine,
 * false otherwise.
 */
bool xccdf_policy_engine_filter(struct xccdf_policy_engine *cb, const char *sysname);

/**
 * Execute the eval function of the given checking engine
 * @memberof xccdf_policy_engine
 * @param engine Checking engine
 * @param policy XCCDF Policy
 * @param definition_id ID of definition to evaluate
 * @param href_id The @href attribute of check-content-ref
 * @param value_bindings Value binding
 * @param check_import_it Check imports
 * @returns result of checking engine evaluation
 */
xccdf_test_result_type_t xccdf_policy_engine_eval(struct xccdf_policy_engine *engine, struct xccdf_policy *policy, const char *definition_id, const char *href_id, struct oscap_list *value_bindings, struct xccdf_check_import_iterator *check_import_it);

/**
 * Execute the query function of the given checking engine
 * @memberof xccdf_policy_engine
 * @param engine Checking Engine
 * @param query_type Type of a query over checking engine data.
 * @param query_data Additional data for the checking engine query.
 * @returns list of query results
 */
struct oscap_stringlist *xccdf_policy_engine_query(struct xccdf_policy_engine *engine, xccdf_policy_engine_query_t query_type, void *query_data);


#endif
