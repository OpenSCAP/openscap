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

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>

#include "_sexp-types.h"
#include "_seap-types.h"
#include "_seap-scheme.h"
#include "sch_queue.h"
#include "seap-descriptor.h"
#include "common/debug_priv.h"
#include "../probe/probe_main.h"
#include "oval_definitions.h"


int sch_queue_connect(SEAP_desc_t *desc, const char *uri, uint32_t flags)
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

ssize_t sch_queue_recv(SEAP_desc_t *desc, void **buf, size_t len, uint32_t flags)
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
	char *item = oscap_queue_remove(queue);
	(*cnt)--;
	size_t item_len = strlen(item);
	if (len < item_len) {
		/* Buffer 'buf' is not large enough to fit the received item */
		*buf = realloc(*buf, item_len);
	}
	strncpy(*buf, item, item_len);
	pthread_mutex_unlock(mutex);
	return item_len;
}

ssize_t sch_queue_sendsexp(SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags)
{
	sch_queuedata_t *data = (sch_queuedata_t *) desc->scheme_data;
	strbuf_t *sb = strbuf_new(SEAP_STRBUF_MAX);
	if (SEXP_sbprintf_t(sexp, sb) != 0) {
		return -1;
	}
	char *msg = oscap_strdup(strbuf_cstr(sb));

	ssize_t len = strlen(msg);
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
	pthread_mutex_lock(mutex);
	oscap_queue_add(queue, (void *) msg);
	(*cnt)++;
	pthread_cond_broadcast(cond);
	pthread_mutex_unlock(mutex);
	strbuf_free(sb);
	return len;
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
	oscap_queue_free(data->to_probe_queue, &free);
	oscap_queue_free(data->from_probe_queue, &free);
	return ret;
}
