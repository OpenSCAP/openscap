/**
 * @file oval_probe_session.c
 * @brief OVAL probe session API implementation
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup PROBESESSION
 * @{
 */
/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "common/_error.h"
#include "common/bfind.h"
#include "common/debug_priv.h"


#include "public/oval_definitions.h"
#include "_oval_probe_session.h"
#include "_oval_probe_handler.h"
#include "oval_probe_impl.h"
#include "oval_probe_ext.h"
#include "probe-table.h"
#include "oval_types.h"

static void oval_probe_session_init(oval_probe_session_t *sess, struct oval_syschar_model *model)
{
        sess->ph = oval_phtbl_new();
        sess->sys_model = model;
        sess->flg = 0;
        sess->pext = oval_pext_new();
        sess->pext->model    = &sess->sys_model;
        sess->pext->sess_ptr = sess;

	oval_probe_handler_t *probe_handler;
	int probe_count = probe_table_size();
	for (int i = 0; i < probe_count; i++) {
		oval_subtype_t type = probe_table_at_index(i);
		if (type == OVAL_INDEPENDENT_SYSCHAR_SUBTYPE) {
			probe_handler = &oval_probe_sys_handler;
		} else {
			probe_handler = &oval_probe_ext_handler;
		}
		oval_probe_handler_set(sess->ph, type, probe_handler, sess->pext);
	}

        oval_probe_handler_set(sess->ph, OVAL_SUBTYPE_ALL, oval_probe_ext_handler, sess->pext); /* special case for reset */
}

oval_probe_session_t *oval_probe_session_new(struct oval_syschar_model *model)
{
        oval_probe_session_t *sess = malloc(sizeof(oval_probe_session_t));
        oval_probe_session_init(sess, model);
        return sess;
}

static void oval_probe_session_free(oval_probe_session_t *sess)
{
	if (sess == NULL) {
		dE("Invalid session (NULL)");
		return;
	}

	oval_phtbl_free(sess->ph);
	oval_pext_free(sess->pext);
}

void oval_probe_session_reinit(oval_probe_session_t *sess, struct oval_syschar_model *model)
{
	oval_probe_session_free(sess);

	oval_probe_session_init(sess, model);
}

void oval_probe_session_destroy(oval_probe_session_t *sess)
{
	oval_probe_session_free(sess);
	free(sess);
}

int oval_probe_session_reset(oval_probe_session_t *sess, struct oval_syschar_model *sysch)
{
        oval_ph_t *ph;

        if ((ph = oval_probe_handler_get(sess->ph, OVAL_SUBTYPE_ALL)) == NULL) {
		dE("No probe handler for OVAL_SUBTYPE_ALL");
		return (-1);
	}

        if (ph->func(OVAL_SUBTYPE_ALL, ph->uptr, PROBE_HANDLER_ACT_RESET) != 0) {
                return(-1);
        }
        if (sysch != NULL)
                sess->sys_model = sysch;

        return(0);
}

int oval_probe_session_abort(oval_probe_session_t *sess)
{
	oval_ph_t *ph;

	if ((ph = oval_probe_handler_get(sess->ph, OVAL_SUBTYPE_ALL) ) == NULL) {
		dE("No probe handler for OVAL_SUBTYPE_ALL");
		return (-1);
	}

        return ph->func(OVAL_SUBTYPE_ALL, ph->uptr, PROBE_HANDLER_ACT_ABORT);
}

struct oval_syschar_model *oval_probe_session_getmodel(oval_probe_session_t *sess)
{
	if (sess == NULL) {
		dE("Invalid session (NULL)");
		return (NULL);
	}

	return (sess->sys_model);
}

/// @}
