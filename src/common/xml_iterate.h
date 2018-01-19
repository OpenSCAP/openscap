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
 *   Simon Lukasik <slukasik@redhat.com>
 *
 */

#pragma once
#ifndef _OSCAP_XML_ITERATE_H
#define _OSCAP_XML_ITERATE_H

#include "util.h"
#include <libxml/tree.h>


/**
 * A callback function to be called on each xmlNode. Note that the node
 * can be modified or replaced.
 * @param node the current XML node
 * @param user_data
 * @returns the result of operation
 * @retval 0 indicates success of callback call
 * @retval 1 indicates hard failure of callback call and process is immediately terminated
 * @retval else idicates warn and walking through tree continues
 */
typedef int (*xml_iterate_callback) (xmlNode **node, void *user_data);

/**
 * Parse the input_text as XML and iterate through all nodes in minidom
 * using DFS order. Calls user_fn with user_data on each node. The user_fn
 * is allowed to modify the minidom tree.
 * @param input_text the string which shall be parsed
 * @param output_text the output document after tree walking. This attribute
 * might be NULL to skip the export (which might be useful in case when
 * user_fn does not modify the tree). The caller shall dispose *output_text.
 * @param user_fn Function which is called on each node
 * @param user_data Data supplied to the user_fn
 * @returns 0 on success, 1 on failure.
 */
int xml_iterate_dfs(const char *input_text, char **output_text, xml_iterate_callback user_fn, void *user_data);


#endif
