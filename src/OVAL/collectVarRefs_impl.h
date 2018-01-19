/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

#ifndef OVAL_COLLECTVARREFS_IMPL
#define OVAL_COLLECTVARREFS_IMPL

#include "public/oval_definitions.h"
#include "oval_definitions_impl.h"
#include "adt/oval_string_map_impl.h"


/* Collect all references to variables from the respective argument,
 * recursively. They are stored as pairs of (var id, var pointer).
 */
void oval_obj_collect_var_refs(struct oval_object *obj, struct oval_string_map *vm);
void oval_ste_collect_var_refs(struct oval_state *ste, struct oval_string_map *vm);


#endif
