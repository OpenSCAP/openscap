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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */
#ifndef OVAL_PROBE_SESSION
#define OVAL_PROBE_SESSION

typedef struct oval_probe_session oval_probe_session_t;

#include "oval_probe_handler.h"
#include "oval_system_characteristics.h"

oval_probe_session_t *oval_probe_session_new(struct oval_syschar_model *model);
void oval_probe_session_destroy(oval_probe_session_t *sess);
int oval_probe_session_close(oval_probe_session_t *sess);
int oval_probe_session_reset(oval_probe_session_t *sess);
int oval_probe_session_sethandler(oval_probe_session_t *sess, oval_subtype_t *type, oval_probe_handler_t handler, void *ptr);

#endif /* OVAL_PROBE_SESSION */
