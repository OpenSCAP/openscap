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
 *      Jan Černý <jcerny@redhat.com>
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "oscap_queue.h"

struct oscap_queue_item {
	void *data;
	struct oscap_queue_item *next;
};

struct oscap_queue {
	struct oscap_queue_item *begin;
	struct oscap_queue_item *end;
};

struct oscap_queue *oscap_queue_new()
{
	struct oscap_queue *queue = malloc(sizeof(struct oscap_queue));
	queue->begin = NULL;
	queue->end = NULL;
	return queue;
}

void oscap_queue_add(struct oscap_queue *queue, void *data)
{
	struct oscap_queue_item *temp = malloc(sizeof(struct oscap_queue_item));
	temp->data = data;
	temp->next = NULL;
	if (queue->begin == NULL) {
		queue->begin = temp;
	} else {
		queue->end->next = temp;
	}
	queue->end = temp;
}

void *oscap_queue_remove(struct oscap_queue *queue)
{
	if (queue->begin == NULL) {
		return NULL;
	}
	void *data = queue->begin->data;
	struct oscap_queue_item *temp = queue->begin;
	if (queue->begin == queue->end) {
		queue->end = NULL;
	}
	queue->begin = queue->begin->next;
	free(temp);
	return data;
}

void oscap_queue_free(struct oscap_queue *queue, oscap_destruct_func destructor)
{
	if (queue == NULL) {
		return;
	}
	struct oscap_queue_item *current, *temp;
	current = queue->begin;
	while (current != NULL) {
		temp = current;
		current = current->next;
		if (destructor != NULL) {
			free(temp->data);
		}
		free(temp);
	}
	free(queue);
}
