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
 *
 */
#ifndef OSCAP_CPE_CPE_SESSION_PRIV_H
#define OSCAP_CPE_CPE_SESSION_PRIV_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/list.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "OVAL/public/oval_agent_api.h"

OSCAP_HIDDEN_START;

struct cpe_session {
	struct oscap_list *dicts;                       ///< All CPE dictionaries except the one embedded in XCCDF
	struct oscap_list *lang_models;                 ///< All CPE lang models except the one embedded in XCCDF
	struct oscap_htable *oval_sessions;             ///< Caches CPE OVAL check results
	struct oscap_htable *applicable_platforms;
	struct oscap_htable *sources_cache;             ///< Not owned cache [path -> oscap_source]
	bool thin_results;                              ///< Should OVAL results related to CPE be exported as THIN?
};

struct cpe_session *cpe_session_new(void);
void cpe_session_free(struct cpe_session *session);
void cpe_session_set_thin_results(struct cpe_session *session, bool thin_results);
struct oval_agent_session *cpe_session_lookup_oval_session(struct cpe_session *cpe, const char *prefixed_href);
bool cpe_session_add_cpe_lang_model_source(struct cpe_session *session, struct oscap_source *source);
bool cpe_session_add_cpe_dict_source(struct cpe_session *session, struct oscap_source *source);
bool cpe_session_add_cpe_autodetect_source(struct cpe_session *session, struct oscap_source *source);
void cpe_session_set_cache(struct cpe_session *session, struct oscap_htable *sources_cache);

OSCAP_HIDDEN_END;
#endif
