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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include "_sexp-types.h"
#include "_seap-types.h"
#include "sch_queue.h"
#include "seap-descriptor.h"
#include "common/debug_priv.h"
#include "../probe/probe_main.h"
#include "oval_definitions.h"


int sch_queue_connect(SEAP_desc_t *desc)
{
	sch_queuedata_t *data = malloc(sizeof(sch_queuedata_t));

	data->from_probe_queue = oscap_queue_new();
	data->from_probe_cnt = 0;
	pthread_cond_init(&data->from_probe_cond, NULL);
	pthread_mutex_init(&data->from_probe_mutex, NULL);

	data->to_probe_queue = oscap_queue_new();
	data->to_probe_cnt = 0;
	pthread_cond_init(&data->to_probe_cond, NULL);
	pthread_mutex_init(&data->to_probe_mutex, NULL);

	data->parent_thread_id = pthread_self();

	struct probe_common_main_argument *arg = malloc(sizeof(struct probe_common_main_argument));
	arg->subtype = desc->subtype;
	arg->queuedata = data;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	if (pthread_create(&data->probe_thread_id, &attr, &probe_common_main, arg)) {
		dE("Cannot create probe thread");
		return -1;
	}
	pthread_attr_destroy(&attr);

	desc->scheme_data = (void *) data;
	return 0;
}

SEXP_t *sch_queue_recvsexp(SEAP_desc_t *desc)
{
	sch_queuedata_t *data = (sch_queuedata_t *)desc->scheme_data;
	struct oscap_queue *queue;
	pthread_mutex_t *mutex;
	pthread_cond_t *cond;
	int *cnt;
	if (pthread_self() == data->parent_thread_id) {
		queue = data->from_probe_queue;
		mutex = &data->from_probe_mutex;
		cond = &data->from_probe_cond;
		cnt = &data->from_probe_cnt;
	} else {
		queue = data->to_probe_queue;
		mutex = &data->to_probe_mutex;
		cond = &data->to_probe_cond;
		cnt = &data->to_probe_cnt;
	}
	pthread_mutex_lock(mutex);
	while (*cnt == 0) {
		pthread_cond_wait(cond, mutex);
	}
	SEXP_t *sexp = oscap_queue_remove(queue);
	(*cnt)--;
	pthread_mutex_unlock(mutex);
	return sexp;
}

ssize_t sch_queue_sendsexp(SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags)
{
	sch_queuedata_t *data = (sch_queuedata_t *) desc->scheme_data;
	struct oscap_queue *queue;
	pthread_mutex_t *mutex;
	pthread_cond_t *cond;
	int *cnt;
	if (pthread_self() == data->parent_thread_id) {
		queue = data->to_probe_queue;
		mutex = &data->to_probe_mutex;
		cond = &data->to_probe_cond;
		cnt = &data->to_probe_cnt;
	} else {
		queue = data->from_probe_queue;
		mutex = &data->from_probe_mutex;
		cond = &data->from_probe_cond;
		cnt = &data->from_probe_cnt;
	}
	/* We want to send a SEXP, but the receiver expects a list of SEXPs. */
	SEXP_t *sexp_list = SEXP_list_new(sexp, NULL);
	pthread_mutex_lock(mutex);
	oscap_queue_add(queue, (void *) sexp_list);
	(*cnt)++;
	pthread_cond_broadcast(cond);
	pthread_mutex_unlock(mutex);
	return 0;
}

int sch_queue_close(SEAP_desc_t *desc, uint32_t flags)
{
	int ret = 0;
	sch_queuedata_t *data = (sch_queuedata_t *) desc->scheme_data;
	if (pthread_cancel(data->probe_thread_id) != 0) {
		dE("Could not cancel %s_probe main thread.", oval_subtype_get_text(desc->subtype));
		return -1;
	}
	void *status;
	const char *subtype_str = oval_subtype_get_text(desc->subtype);
	ret = pthread_join(data->probe_thread_id, &status);
	if (ret != 0) {
		dE("Return code of %s_probe main thread is %d.", subtype_str, ret);
	}
	oscap_queue_free(data->to_probe_queue, NULL);
	oscap_queue_free(data->from_probe_queue, NULL);
	return ret;
}
