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

#ifndef OSCAP_QUEUE_H
#define OSCAP_QUEUE_H

#include "util.h"

struct oscap_queue;

/*
 * Initialize a new queue
 */
struct oscap_queue *oscap_queue_new(void);

/*
 * Enqueue an item at the end of the queue
 */
void oscap_queue_add(struct oscap_queue *queue, void *data);

/*
 * Remove an item from the beginning of the queue
 */
void *oscap_queue_remove(struct oscap_queue *queue);

/*
 * Dispose the queue
 */
void oscap_queue_free(struct oscap_queue *queue, oscap_destruct_func destructor);

#endif //OSCAP_QUEUE_H
