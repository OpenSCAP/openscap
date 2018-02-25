/*
 * Copyright 2018 Red Hat Inc., Durham, North Carolina.
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
 *      "Jan Černý" <jcerny@redhat.com>
 */


#ifndef OPENSCAP_PROBE_TABLE_H
#define OPENSCAP_PROBE_TABLE_H

#include "probe-api.h"

typedef void *(*probe_init_function_t)(void);
typedef int (*probe_main_function_t)(probe_ctx *ctx, void *arg);
typedef void (*probe_fini_function_t)(void *probe_arg);
typedef void (*probe_offline_mode_function_t)(void);

probe_init_function_t probe_table_get_init_function(oval_subtype_t type);
probe_main_function_t probe_table_get_main_function(oval_subtype_t type);
probe_fini_function_t probe_table_get_fini_function(oval_subtype_t type);
probe_offline_mode_function_t probe_table_get_offline_mode_function(oval_subtype_t type);

OSCAP_API void probe_table_list(FILE *output);
OSCAP_API int probe_table_size(void);

#endif /* OPENSCAP_OPENSCAP_PROBE_TABLE_H */
