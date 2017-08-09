/**
 * @file oval_reference.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oval_definitions_impl.h"
#include "adt/oval_collection_impl.h"
#include "common/debug_priv.h"
#include "common/util.h"

typedef struct oval_reference {
	struct oval_definition_model *model;
	char *source;
	char *id;
	char *url;
} oval_reference_t;

bool oval_reference_iterator_has_more(struct oval_reference_iterator
				      *oc_reference)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_reference);
}

struct oval_reference *oval_reference_iterator_next(struct
						    oval_reference_iterator
						    *oc_reference)
{
	return (struct oval_reference *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_reference);
}

void oval_reference_iterator_free(struct
				  oval_reference_iterator
				  *oc_reference)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_reference);
}

char *oval_reference_get_source(struct oval_reference *ref)
{
	__attribute__nonnull__(ref);

	return ((struct oval_reference *)ref)->source;
}

char *oval_reference_get_id(struct oval_reference *ref)
{
	__attribute__nonnull__(ref);

	return ((struct oval_reference *)ref)->id;
}

char *oval_reference_get_url(struct oval_reference *ref)
{
	__attribute__nonnull__(ref);

	return ((struct oval_reference *)ref)->url;
}

struct oval_reference *oval_reference_new(struct oval_definition_model *model)
{
	struct oval_reference *ref = (struct oval_reference *)malloc(sizeof(oval_reference_t));
	if (ref == NULL)
		return NULL;

	ref->id = NULL;
	ref->source = NULL;
	ref->url = NULL;
	ref->model = model;
	return ref;
}

struct oval_reference *oval_reference_clone
    (struct oval_definition_model *new_model, struct oval_reference *old_reference) {
	struct oval_reference *new_reference = oval_reference_new(new_model);
	char *id = oval_reference_get_id(old_reference);
	oval_reference_set_id(new_reference, id);
	char *source = oval_reference_get_source(old_reference);
	oval_reference_set_source(new_reference, source);
	char *url = oval_reference_get_url(old_reference);
	oval_reference_set_url(new_reference, url);
	return new_reference;
}

void oval_reference_free(struct oval_reference *ref)
{
	__attribute__nonnull__(ref);

	if (ref->id != NULL)
		free(ref->id);
	if (ref->source != NULL)
		free(ref->source);
	if (ref->url != NULL)
		free(ref->url);
	ref->id = NULL;
	ref->source = NULL;
	ref->url = NULL;
	free(ref);
}

void oval_reference_set_source(struct oval_reference *ref, char *source)
{
	__attribute__nonnull__(ref);
	if (ref->source != NULL)
		free(ref->source);
	ref->source = (source == NULL) ? NULL : oscap_strdup(source);
}

void oval_reference_set_id(struct oval_reference *ref, char *id)
{
	__attribute__nonnull__(ref);
	if (ref->id != NULL)
		free(ref->id);
	ref->id = (id == NULL) ? NULL : oscap_strdup(id);
}

void oval_reference_set_url(struct oval_reference *ref, char *url)
{
	__attribute__nonnull__(ref);
	if (ref->url != NULL)
		free(ref->url);
	ref->url = (url == NULL) ? NULL : oscap_strdup(url);
}

/*typedef void (*oval_reference_consumer)(struct oval_reference*, void*);*/
int oval_reference_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, oval_reference_consumer consumer, void *user)
{
	__attribute__nonnull__(context);

	struct oval_reference *ref = oval_reference_new(context->definition_model);
	char *ref_id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "ref_id");
	if (ref_id != NULL) {
		oval_reference_set_id(ref, ref_id);
		free(ref_id);
		ref_id = NULL;
	}
	char *ref_url = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "ref_url");
	if (ref_url != NULL) {
		oval_reference_set_url(ref, ref_url);
		free(ref_url);
		ref_url = NULL;
	}
	char *source = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "source");
	if (source != NULL) {
		oval_reference_set_source(ref, source);
		free(source);
		source = NULL;
	}
	(*consumer) (ref, user);
	return 0;
}

