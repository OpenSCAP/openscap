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

#include "common/public/oscap.h"
#include "common/util.h"

OSCAP_HIDDEN_START;

struct cpe_session {
	struct oscap_list *dicts;                       ///< All CPE dictionaries except the one embedded in XCCDF
	struct oscap_list *lang_models;                 ///< All CPE lang models except the one embedded in XCCDF
	struct oscap_htable *oval_sessions;             ///< Caches CPE OVAL check results
	struct oscap_htable *applicable_platforms;
};

struct cpe_session *cpe_session_new(void);
void cpe_session_free(struct cpe_session *session);

OSCAP_HIDDEN_END;
#endif
