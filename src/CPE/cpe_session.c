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
 * Author:
 *     Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/alloc.h"
#include "common/_error.h"
#include "common/list.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "cpe_session_priv.h"
#include "CPE/public/cpe_dict.h"
#include "CPE/public/cpe_lang.h"
#include "OVAL/public/oval_agent_api.h"

struct cpe_session *cpe_session_new(void)
{
	struct cpe_session *cpe = oscap_calloc(1, sizeof(struct cpe_session));
	cpe->dicts = oscap_list_new();
	cpe->lang_models = oscap_list_new();
	cpe->oval_sessions = oscap_htable_new();
	cpe->applicable_platforms = oscap_htable_new();
	return cpe;
}

static inline void _xccdf_policy_destroy_cpe_oval_session(void* ptr)
{
	struct oval_agent_session* session = (struct oval_agent_session*)ptr;
	struct oval_definition_model* model = oval_agent_get_definition_model(session);
	oval_agent_destroy_session(session);
	oval_definition_model_free(model);
}

void cpe_session_free(struct cpe_session *session)
{
	if (session != NULL) {
		oscap_list_free(session->dicts, (oscap_destruct_func) cpe_dict_model_free);
		oscap_list_free(session->lang_models, (oscap_destruct_func) cpe_lang_model_free);
		oscap_htable_free(session->oval_sessions, (oscap_destruct_func) _xccdf_policy_destroy_cpe_oval_session);
		oscap_htable_free(session->applicable_platforms, NULL);
		oscap_free(session);
	}
}
