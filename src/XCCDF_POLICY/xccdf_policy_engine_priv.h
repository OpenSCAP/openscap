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
#include "public/xccdf_policy.h"

OSCAP_HIDDEN_START;

/**
 * Typedef of callback structure with system identificator, callback function and usr data (optional)
 * On evaluation action will be selected checking system and appropriate callback registred by tool
 * for that system.
 */
typedef struct callback_t {
	char *system;                           ///< Identificator of checking engine
	xccdf_policy_engine_eval_fn callback;   ///< format of callback function
	void * usr;                             ///< User data structure
	xccdf_policy_engine_query_fn query_fn;  ///< query callback function
} callback;

OSCAP_HIDDEN_END;

#endif
