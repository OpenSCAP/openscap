/**
 * @file oval_behavior.c
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
#include "adt/oval_string_map_impl.h"

#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"

/***************************************************************************/
/* Variable definitions
 * */

typedef struct oval_behavior {
	struct oval_definition_model *model;
	char *value;
	char *key;
} oval_behavior_t;

/* End of variable definitions
 * */
/***************************************************************************/

bool oval_behavior_iterator_has_more(struct oval_behavior_iterator *oc_behavior)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_behavior);
}

struct oval_behavior *oval_behavior_iterator_next(struct oval_behavior_iterator
						  *oc_behavior)
{
	return (struct oval_behavior *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_behavior);
}

void oval_behavior_iterator_free(struct oval_behavior_iterator
				 *oc_behavior)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_behavior);
}

char *oval_behavior_get_value(struct oval_behavior *behavior)
{
	__attribute__nonnull__(behavior);

	return behavior->value;
}

char *oval_behavior_get_key(struct oval_behavior *behavior)
{
	__attribute__nonnull__(behavior);

	return behavior->key;
}

struct oval_behavior *oval_behavior_new(struct oval_definition_model *model)
{
	oval_behavior_t *behavior = (oval_behavior_t *) oscap_alloc(sizeof(oval_behavior_t));
	if (behavior == NULL)
		return NULL;

	behavior->model = model;
	behavior->value = NULL;
	behavior->key = NULL;
	return behavior;
}

struct oval_behavior *oval_behavior_clone(struct oval_definition_model *new_model, struct oval_behavior *old_behavior)
{
	struct oval_behavior *new_behavior = oval_behavior_new(new_model);
	oval_behavior_set_keyval
	    (new_behavior, oval_behavior_get_key(old_behavior), oval_behavior_get_value(old_behavior));
	return new_behavior;
}

void oval_behavior_free(struct oval_behavior *behavior)
{
	__attribute__nonnull__(behavior);

	if (behavior->value)
		free(behavior->value);
	if (behavior->key)
		free(behavior->key);
	behavior->key = NULL;
	behavior->value = NULL;
	free(behavior);
}

void oval_behavior_set_keyval(struct oval_behavior *behavior, const char *key, const char *value)
{
	__attribute__nonnull__(behavior);

	behavior->key = oscap_strdup(key);
	behavior->value = oscap_strdup(value);
}

//typedef void (*oval_behavior_consumer)(struct oval_behavior_node *, void*);
int oval_behavior_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context,
			    oval_family_t family, oval_behavior_consumer consumer, void *user)
{
	__attribute__nonnull__(context);

	while (xmlTextReaderMoveToNextAttribute(reader) == 1) {

		const char *name = (const char *)xmlTextReaderConstName(reader);
		const char *value = (const char *)xmlTextReaderConstValue(reader);

		if (name && value) {
			oval_behavior_t *behavior = oval_behavior_new(context->definition_model);
			oval_behavior_set_keyval(behavior, name, value);
			(*consumer) (behavior, user);
		}
	}
	return 0;
}

