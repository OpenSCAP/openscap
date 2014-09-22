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

struct cpe_session *cpe_session_new(void)
{
	struct cpe_session *cpe = oscap_calloc(1, sizeof(struct cpe_session));
	cpe->dicts = oscap_list_new();
	cpe->lang_models = oscap_list_new();
	cpe->oval_sessions = oscap_htable_new();
	cpe->applicable_platforms = oscap_htable_new();
	return cpe;
}


