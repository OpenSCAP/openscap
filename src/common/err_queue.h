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

#pragma once
#ifndef _OSCAP_ERR_QUEUE_H
#define _OSCAP_ERR_QUEUE_H

#include "util.h"
#include "_error.h"
#include <stdbool.h>


/**
 * OpenSCAP Internal Error Queue.
 * This structure is the traditional FIFO object which allows to lookup
 * the last item.
 */
struct err_queue;

/**
 * Initialize new error_queue.
 * @memberof err_queue
 * @returns newly initialized error queue
 */
struct err_queue *err_queue_new(void);

/**
 * Push the new error at the end of the error queue.
 * @memberof err_queue
 * @param q Internal Error Queue
 * @param error new error to save in the queuei
 * @returns true on success
 */
bool err_queue_push(struct err_queue *q, struct oscap_err_t *error);

/**
 * Query if the queue is empty
 * @meberof err_queue
 * @param q Internal Error Queue
 * @returns false if there is some error in the queue
 */
bool err_queue_is_empty(struct err_queue *q);

/**
 * Pop the first object from the FIFO queue. This new object is no longer
 * tracked by err_queue and shall be disposed by caller.
 * @memberof err_queue
 * @param q Internal Error Queue
 * @returns The first item in the queue or NULL.
 */
struct oscap_err_t *err_queue_pop_first(struct err_queue *q);

/**
 * Return the last object from the FIFO queuqe. This object continues to
 * be a property of err_queue and shall not be modified nor disposed.
 * @memberof err_queue
 * @param q Internal Error Queue
 * @returns the last item in the queue or NULL
 */
const struct oscap_err_t *err_queue_get_last(struct err_queue *q);

/**
 * Dispose given err_queue. Dispose also stored messages if destructor is
 * supplied.
 * @memberof err_queue
 * @param q Internal Error Queue
 * @param destructor a function to dipose messages in the queue
 */
void err_queue_free(struct err_queue *q, oscap_destruct_func destructor);

/**
 * Get all the errors in the queue as a single string.
 * @memberof err_queue
 * @param q Internal Error Queue
 * @param result pointer, where to store the resulting string. Newly
 * allocated mmory will be assigned with this. And shall be disposed
 * by caller.
 * @returns zero on success
 */
int err_queue_to_string(struct err_queue *q, char **result);


#endif
