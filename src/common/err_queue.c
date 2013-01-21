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
 */

#include "_error.h"
#include "err_queue.h"
#include <stdlib.h>

struct err_queue {
	struct oscap_err_t *first;
	struct oscap_err_t *last;
};

struct err_queue *err_queue_new(void)
{
	return (struct err_queue *) calloc(1, sizeof(struct err_queue));
}

bool err_queue_push(struct err_queue *q, struct oscap_err_t *error)
{
	if (q == NULL || error == NULL) {
		assert(false);
		return false;
	}
	error->next = NULL;

	if (q->last == NULL) {
		q->first = error;
		q->last = error;
	}
	else {
		q->last->next = error;
		q->last = error;
	}
	return true;
}

bool err_queue_is_empty(struct err_queue *q)
{
	if (q == NULL) {
		assert(false);
		return true;
	}
	return q->last == NULL;
}

struct oscap_err_t *err_queue_pop_first(struct err_queue *q)
{
	if (q == NULL || q->first == NULL) {
		assert(false);
		return NULL;
	}

	struct oscap_err_t *pom = q->first;
	q->first = pom->next;
	if (q->last == pom)
		q->last = NULL;
	return pom;
}

const struct oscap_err_t *err_queue_get_last(struct err_queue *q)
{
	if (q == NULL || q->last == NULL) {
		assert(false);
		return NULL;
	}
	return q->last;
}

void err_queue_free(struct err_queue *q, oscap_destruct_func destructor)
{
	if (q == NULL)
		return;

	if (destructor != NULL)
		while (!err_queue_is_empty(q))
			destructor(err_queue_pop_first(q));
	free(q);
}

