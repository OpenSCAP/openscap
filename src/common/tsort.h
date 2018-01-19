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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */


// topological sort

#include "list.h"

#ifndef OSCAP_TSORT_H_
#define OSCAP_TSORT_H_


// returns nodest with an edge from given node
typedef struct oscap_list* (*oscap_tsort_edge_func)(void *node, void *userdata);

/*
 * Topological sort.
 *
 * Performs a topological sort on a directed graph.
 *
 * @a edge_func is supposed to return an oscap_list of nodes edges coming from the current node
 * (passed to it as an argument) are pointing to. It takes the node as a first argument
 * and a pointer passed as @a userdata as a second argument.
 *
 * The purpose of @a cmp_func is to compare nodes. If it is NULL raw pointer comparison
 * is used, which should be good enough in most cases.
 *
 * Pointer @a output will be set to a list with result. If you want to just check whether
 * there is a topological order defined on the graph (i.e. it is an acyclic graph), pass NULL.
 * If the function manages to find a topological order of the nodes (returns true),
 * it is returned in this variable. Otherwise, it will contain an encountered loop.
 * You are responsible to free this list.
 *
 * @param input set of nodes to sort
 * @param output this pointer will be set to the result of the algorithm
 * @param edge_func function to return target nodes of outgoing edges from the current one
 * @param cmp_func node compasrison function
 * @param userdata arbitrary data pointer to be forwarded to the edge_func
 * @returns whether the algorithm managed to topologically sort the graph
 * @retval true input was sorted, result is in the *output pointer
 * @retval false a loop was detected, *output points to the encountered loop
 */
bool oscap_tsort(struct oscap_list *input, struct oscap_list **output, oscap_tsort_edge_func edge_func, oscap_cmp_func cmp_func, void *userdata);


#endif // OSCAP_TSORT_H_

