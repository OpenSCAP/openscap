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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tsort.h"

struct oscap_tsort_context {
	struct oscap_list *visited;
	struct oscap_list *cur_stack;
	struct oscap_list *result;
	oscap_tsort_edge_func edge_func;
	oscap_cmp_func cmp_func;
	void *userdata;
};

static struct oscap_tsort_context *oscap_tsort_context_new(oscap_tsort_edge_func edge_func, oscap_cmp_func cmp_func, void *userdata)
{
	struct oscap_tsort_context *ctx = calloc(1, sizeof(struct oscap_tsort_context));
	ctx->visited = oscap_list_new();
	ctx->cur_stack = oscap_list_new();
	ctx->result = oscap_list_new();
	ctx->edge_func = edge_func;
	ctx->cmp_func = cmp_func;
	ctx->userdata = userdata;
	return ctx;
}

static void oscap_tsort_context_free(struct oscap_tsort_context *ctx)
{
	if (ctx != NULL) {
		oscap_list_free(ctx->visited, NULL);
		oscap_list_free(ctx->cur_stack, NULL);
		oscap_list_free(ctx->result, NULL);
		free(ctx);
	}
}

static bool oscap_tsort_visit(void *node, struct oscap_tsort_context* ctx);

inline static bool oscap_tsort_visit_all(struct oscap_list *nodelist, struct oscap_tsort_context* ctx)
{
	bool ret = true;
	struct oscap_iterator *it = oscap_iterator_new(nodelist);

	while (oscap_iterator_has_more(it)) {
		if (!oscap_tsort_visit(oscap_iterator_next(it), ctx)) {
			ret = false;
			break;
		}
	}

	oscap_iterator_free(it);
	return ret;
}

static bool oscap_tsort_visit(void *node, struct oscap_tsort_context* ctx)
{
	// loop detection
	if (oscap_list_contains(ctx->cur_stack, node, ctx->cmp_func)) return false;

	// skip already visited node
	if (oscap_list_contains(ctx->visited, node, ctx->cmp_func)) return true;

	// mark as visited & update stack
	oscap_list_add(ctx->visited, node);
	oscap_list_push(ctx->cur_stack, node);

	// visit all next nodes (dependencies)
	struct oscap_list *next = ctx->edge_func(node, ctx->userdata);
	bool ret = oscap_tsort_visit_all(next, ctx);
	oscap_list_free(next, NULL);

	// update stack & add node to result
	if (ret) oscap_list_pop(ctx->cur_stack, NULL);
	oscap_list_add(ctx->result, node);

	return ret;
}

bool oscap_tsort(struct oscap_list *input, struct oscap_list **output, oscap_tsort_edge_func edge_func, oscap_cmp_func cmp_func, void *userdata)
{
	assert(edge_func != NULL);
	assert(input != NULL);

	if (output != NULL) *output = NULL;
	if (cmp_func == NULL) cmp_func = oscap_ptr_cmp;

	struct oscap_tsort_context *ctx = oscap_tsort_context_new(edge_func, cmp_func, userdata);
	bool ret = oscap_tsort_visit_all(input, ctx);

	if (output != NULL) {
		if (ret) {
			// the graph has been topologically sorted, return result
			*output = ctx->result;
			ctx->result = NULL;
		}
		else {
			// a loop has been ecountered in the graph, return the loop
			*output = ctx->cur_stack;
			ctx->cur_stack = NULL;
		}
	}

	oscap_tsort_context_free(ctx);

	return ret;
}



