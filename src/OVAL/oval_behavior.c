/**
 * @file oval_behavior.c
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
#include "oval_string_map_impl.h"

typedef struct oval_behavior {
	char *value;
	char *key;
} oval_behavior_t;

int oval_iterator_behavior_has_more(struct oval_iterator_behavior *oc_behavior)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_behavior);
}

struct oval_behavior *oval_iterator_behavior_next(struct oval_iterator_behavior
						  *oc_behavior)
{
	return (struct oval_behavior *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_behavior);
}

char *oval_behavior_value(struct oval_behavior *behavior)
{
	return behavior->value;
}

char *oval_behavior_key(struct oval_behavior
							  *behavior)
{
	return behavior->key;
}

struct oval_behavior *oval_behavior_new()
{
	oval_behavior_t *behavior =
	    (oval_behavior_t *) malloc(sizeof(oval_behavior_t));
	behavior->value = NULL;
	behavior->key   = NULL;
	return behavior;
}

void oval_behavior_free(struct oval_behavior *behavior)
{
	if (behavior->value)free(behavior->value);
	if (behavior->key  )free(behavior->key  );
	behavior->key     = NULL;
	behavior->value  = NULL;
	free(behavior);
}

void set_behavior_keyval(struct oval_behavior *behavior, const char* key, const char* value)
{
	behavior->key   = strdup(key);
	behavior->value = strdup(value);
}

//typedef void (*oval_behavior_consumer)(struct oval_behavior_node *, void*);
int oval_behavior_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context,
			    oval_family_enum family,
			    oval_behavior_consumer consumer, void *user)
{
	oval_behavior_t *behavior = oval_behavior_new();
	while (xmlTextReaderMoveToNextAttribute(reader) == 1) {
		const char *name  = (const char *) xmlTextReaderConstName(reader);
		const char *value = (const char *) xmlTextReaderConstValue(reader);
		if (name && value)
			set_behavior_keyval(behavior, name, value);
	}
	(*consumer) (behavior, user);
	return 1;
}

void oval_behavior_to_print(struct oval_behavior *behavior, char *indent,
			    int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sBEHAVIOR.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sBEHAVIOR[%d].", indent, idx);

	printf("%s%s = [%s]\n", nxtindent, oval_behavior_key(behavior), oval_behavior_value(behavior));
}
