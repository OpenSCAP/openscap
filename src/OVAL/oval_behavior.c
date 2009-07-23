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
	struct oval_value *value;
	struct oval_string_map *att_values;
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

struct oval_value *oval_behavior_value(struct oval_behavior *behavior)
{
	return (behavior)->value;
}

struct oval_iterator_string *oval_behavior_attribute_keys(struct oval_behavior
							  *behavior)
{
	return (struct oval_iterator_string *)oval_string_map_keys(behavior->
								   att_values);
}

char *oval_behavior_value_for_key(struct oval_behavior *behavior,
				  char *attributeKey)
{
	return (char *)oval_string_map_get_value(behavior->att_values,
						 attributeKey);
}

struct oval_behavior *oval_behavior_new()
{
	oval_behavior_t *behavior =
	    (oval_behavior_t *) malloc(sizeof(oval_behavior_t));
	behavior->value = NULL;
	behavior->att_values = oval_string_map_new();
	return behavior;
}

void oval_behavior_free(struct oval_behavior *behavior)
{
	void free_value(void *value) {
		oval_value_free(behavior->value);
	}
	if (behavior->value != NULL)
		free_value((void *)behavior->value);
	oval_string_map_free(behavior->att_values, &free_value);
	free(behavior);
}

void set_oval_behavior_value_for_key(struct oval_behavior *behavior,
				     struct oval_value *value, char *key)
{
	oval_string_map_put(behavior->att_values, key, (void *)value);
}

//typedef void (*oval_behavior_consumer)(struct oval_behavior_node *, void*);
int oval_behavior_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context,
			    oval_family_enum family,
			    oval_behavior_consumer consumer, void *user)
{
	oval_behavior_t *behavior = oval_behavior_new();
	xmlNode *node = xmlTextReaderCurrentNode(reader);
	if(node!=NULL && node->type==XML_ELEMENT_NODE){
		xmlElement *element = (xmlElement*)node;
		xmlAttribute *attributes = element->attributes;
		int idx;for(idx=0;attributes!=NULL;idx++){
			xmlAttribute *attr = attributes;
			attributes = (xmlAttribute*)attributes->next;
			char *value = (char *) xmlTextReaderGetAttribute(reader, attr->name);
			if (value != NULL) {
				oval_string_map_put(behavior->att_values, (char*) attr->name, value);
			}
		}
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

	struct oval_iterator_string *keys =
	    oval_behavior_attribute_keys(behavior);
	if (oval_iterator_string_has_more(keys)) {
		int i;
		for (i = 1; oval_iterator_string_has_more(keys); i++) {
			char *key = oval_iterator_string_next(keys);
			char *val = oval_behavior_value_for_key(behavior, key);
			printf("%s%s = [%s]\n", nxtindent, key, val);
		}
	} else {
		printf("%s <<NO FIELDS BOUND TO BEHAVIOR>>\n", nxtindent);
	}
}
