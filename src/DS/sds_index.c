/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/scap_ds.h"
#include "common/list.h"

#include <libxml/xmlreader.h>

struct ds_stream_index
{
	struct oscap_stringlist* check_components;
	struct oscap_stringlist* checklist_components;
	struct oscap_stringlist* dictionary_components;
};

struct ds_stream_index* ds_stream_index_new(void)
{
	struct ds_stream_index* ret = oscap_alloc(sizeof(struct ds_stream_index));

	ret->check_components = oscap_stringlist_new();
	ret->checklist_components = oscap_stringlist_new();
	ret->dictionary_components = oscap_stringlist_new();

	return ret;
}

void ds_stream_index_free(struct ds_stream_index* s)
{
	oscap_stringlist_free(s->check_components);
	oscap_stringlist_free(s->checklist_components);
	oscap_stringlist_free(s->dictionary_components);

	oscap_free(s);
}

struct ds_sds_index
{
	struct oscap_htable* streams;
};

struct ds_sds_index* ds_sds_index_new(void)
{
	struct ds_sds_index* ret = oscap_alloc(sizeof(struct ds_sds_index));
	ret->streams = oscap_htable_new();

	return ret;
}

void ds_sds_index_free(struct ds_sds_index* s)
{
	oscap_htable_free(s->streams, (oscap_destruct_func)ds_stream_index_free);

	oscap_free(s);
}

struct ds_stream_index* ds_sds_index_get_stream(struct ds_sds_index* s, const char* stream_id)
{
	return oscap_htable_get(s->streams, stream_id);
}
