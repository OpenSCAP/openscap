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
#ifndef _OSCAP_XCCDF_POLICY_REPORT_CB_PRIV_H
#define _OSCAP_XCCDF_POLICY_REPORT_CB_PRIV_H

#include "common/util.h"
#include "public/xccdf_policy.h"

OSCAP_HIDDEN_START;

/**
 * Typedef of callback structure with callback function and usr data (optional)
 * After rule evaluation action will be called the callback with user data.
 */
typedef struct callback_out_t {
	char * system;                 ///< Identificator of checking engine (output engine)
	int (*callback)(void*,void*);  ///< policy report callback {output,start}
	void * usr;                    ///< User data structure
} callback_out;

callback_out *reporter_new(char *report_type, void *output_func, void *usr);

int reporter_send_simple(callback_out *reporter, void *data);

OSCAP_HIDDEN_END;

#endif
