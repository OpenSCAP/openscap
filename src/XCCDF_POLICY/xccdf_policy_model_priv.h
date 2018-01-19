/*
 * Copyright 2014--2016 Red Hat Inc., Durham, North Carolina.
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
#ifndef _OSCAP_XCCDF_POLICY_MODEL_PRIV_H
#define _OSCAP_XCCDF_POLICY_MODEL_PRIV_H

#include "common/util.h"
#include "CPE/cpe_session_priv.h"
#include "public/xccdf_policy.h"


#define XCCDF_POLICY_OUTCB_START "urn:xccdf:system:callback:start"
#define XCCDF_POLICY_OUTCB_END "urn:xccdf:system:callback:output"

/**
 * Remove checking engines with given system from xccdf_policy_model
 * @memberof xccdf_policy_model
 * @param model XCCDF Policy Model
 * @param sys sytem name of the callback
 */
void xccdf_policy_model_unregister_engines(struct xccdf_policy_model *model, const char *sys);

/**
 * Query whether the given list platforms qualifies as 'applicable'. When considering
 * policy_model CPE settings in the given policy model
 * @memberof xccdf_policy_model
 * @param model XCCDF Policy Model
 * @param platforms list of CPE platform identifiers
 * @returns true if the list of platforms qualifies as 'applicable'
 */
bool xccdf_policy_model_platforms_are_applicable(struct xccdf_policy_model *model, struct oscap_string_iterator *platforms);

/**
 * Query whether the given  item is applicable within given policy
 * @memberof xccdf_policy_model
 * @param model XCCDF Policy Model
 * @param item XCCDF Item
 * @returns true if the given item is applicable
 */
bool xccdf_policy_model_item_is_applicable(struct xccdf_policy_model *model, struct xccdf_item *item);

/**
 * Get CPE session assigned with the XCCDF Policy Model
 * @memberof xccdf_policy_model
 * @param model XCCDF Policy Model
 * @returns cpe_session or NULL
 */
struct cpe_session *xccdf_policy_model_get_cpe_session(struct xccdf_policy_model *model);

struct xccdf_policy *xccdf_policy_model_get_existing_policy_by_id(struct xccdf_policy_model *policy_model, const char *profile_id);

struct xccdf_policy *xccdf_policy_model_create_policy_by_id(struct xccdf_policy_model *policy_model, const char *id);


#endif
