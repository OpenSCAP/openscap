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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include "debug_priv.h"
#include "xml_iterate.h"


static int xml_element_dfs_callback(xmlNode **node, xml_iterate_callback user_fn, void *user_data)
{
	int result = user_fn(node, user_data);
	if (result == 1)
		return result;
	xmlNode *child = (*node)->children;
	while (child != NULL) {
		int res = xml_element_dfs_callback(&child, user_fn, user_data);
		if (res == 1)
			return res;
		if (result == 0)
			result = res;
		child = child->next;
	}
	return result;
}

int xml_iterate_dfs(const char *input_text, char **output_text, xml_iterate_callback user_fn, void *user_data)
{
	char *input_document = NULL;
	xmlDocPtr doc = NULL;
	xmlNode *root = NULL;
	int res = 0;

	if (user_fn == NULL)
		return 1;

	input_document = oscap_sprintf("<x xmlns='http://www.w3.org/1999/xhtml'>%s</x>", input_text);
	if ((doc = xmlParseMemory(input_document, strlen(input_document))) == NULL) {
		dW("Could not xmlParseMemory: '%s'", input_document);
		free(input_document);
		return 1;
	}
	if ((root = xmlDocGetRootElement(doc)) == NULL) {
		dW("Could not xmlDocGetRootElement: '%s'", input_document);
		xmlFreeDoc(doc);
		free(input_document);
		return 1;
	}
	free(input_document);

	res = xml_element_dfs_callback(&root, user_fn, user_data);
	if (output_text != NULL) {
		// We cannot simply xmlDumpMemory, because we need to skip the upper <x/> element.
		if ((root = xmlDocGetRootElement(doc)) == NULL) {
			dW("Could not get xmlDocGetRootElement of result document.");
			xmlFreeDoc(doc);
			return 1;
		}
		xmlBuffer *buff = xmlBufferCreate();
		xmlNode *child = root->children;
		while (child != NULL) {
			int size = xmlNodeDump(buff, doc, child, 0, 0);
			if (size < 0) {
				dE("xmlNodeDump failed!");
			}
			else if (size == 0) {
				dI("xmlNodeDump returned zero.");
			}

			child = child->next;
		}
		/* Sadly, we cannot use xmlBufferDetach since we want to support older libxml. */
		*output_text = oscap_strdup((const char *) xmlBufferContent(buff));
		xmlBufferFree(buff);
	}
	xmlFreeDoc(doc);
	return res;
}
