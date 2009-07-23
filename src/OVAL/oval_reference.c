/**
 * @file oval_reference.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"

typedef struct oval_reference {
	char *source;
	char *id;
	char *url;
} oval_reference_t;

int oval_iterator_reference_has_more(struct oval_iterator_reference
				     *oc_reference)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_reference);
}

struct oval_reference *oval_iterator_reference_next(struct
						    oval_iterator_reference
						    *oc_reference)
{
	return (struct oval_reference *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_reference);
}

char *oval_reference_source(struct oval_reference *ref)
{
	return ((struct oval_reference *)ref)->source;
}

char *oval_reference_id(struct oval_reference *ref)
{
	return ((struct oval_reference *)ref)->id;
}

char *oval_reference_url(struct oval_reference *ref)
{
	return ((struct oval_reference *)ref)->url;
}

struct oval_reference *oval_reference_new()
{
	struct oval_reference *ref =
	    (struct oval_reference *)malloc(sizeof(oval_reference_t));
	ref->id = NULL;
	ref->source = NULL;
	ref->url = NULL;
	return ref;
}

void oval_reference_free(struct oval_reference *ref)
{
	if (ref->id != NULL)
		free(ref->id);
	if (ref->source != NULL)
		free(ref->source);
	if (ref->url != NULL)
		free(ref->url);
	free(ref);
}

void set_oval_reference_source(struct oval_reference *ref, char *source)
{
	ref->source = source;
}

void set_oval_reference_id(struct oval_reference *ref, char *id)
{
	ref->id = id;
}

void set_oval_reference_url(struct oval_reference *ref, char *url)
{
	ref->url = url;
}

//typedef void (*oval_reference_consumer)(struct oval_reference*, void*);
int oval_reference_parse_tag(xmlTextReaderPtr reader,
			     struct oval_parser_context *context,
			     oval_reference_consumer consumer, void *user)
{
	struct oval_reference *ref = oval_reference_new();
	char *ref_id = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "ref_id");
	char *ref_url = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "ref_url");
	char *source = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "source");
	set_oval_reference_source(ref, source);
	set_oval_reference_id(ref, ref_id);
	set_oval_reference_url(ref, ref_url);
	(*consumer) (ref, user);
	return 1;
}

void oval_reference_to_print(struct oval_reference *ref, char *indent,
			     int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sREFERENCE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sREFERENCE[%d].", indent, idx);

	printf("%sSOURCE = %s\n", nxtindent, ref->source);
	printf("%sID     = %s\n", nxtindent, ref->id);
	printf("%sURL    = %s\n", nxtindent, ref->url);
}
