/**
 * @file cpedict.c
 * \brief Interface to Common Product Enumeration (CPE) Dictionary.
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libxml/tree.h>

#include "cpedict.h"

/*** Private ***/

/**
 * Load new CPE dictionary from XML node
 * @param node file name of dictionary to load
 * @return new dictionary
 * @retval NULL on failure
 */
cpe_dict_t *cpe_dict_new_xml(xmlNodePtr node);

/**
 * New dictionary item from XML
 * @param node cpe-item node
 * @return new dictionary item
 * @retval NULL on failure
 */
cpe_dict_item_t *cpe_dictitem_new_xml(xmlNodePtr node);

cpe_dict_check_t *cpe_dictcheck_new_xml(xmlNode * node);

/*** Public ***/

/**
 * Append item to a dynamically allocated list
 * @param type type of item being appended to a list
 * @param item variable holding item itself
 * @param list variable representing list itself
 * @param num variable representing number of items in list
 * @param alloc variable holding info on number of items allocated
 * @param init_alloc desired initial allocation in case of list does not exist yet
 */
#define APPEND_ITEM(type, item, list, num, alloc, init_alloc) do {    \
	if ((alloc) < (num) + 1) {                                        \
		type* old;                                                    \
		if ((alloc) <= 0) (alloc) = (init_alloc);                     \
		else (alloc) *= 2;                                            \
		old = (list);                                                 \
		(list) = realloc(old, (alloc) * sizeof(type));                \
		if ((list) == NULL) (list) = old;                             \
	}                                                                 \
	(list)[(num)++] = (item);                                         \
} while (0)

char *str_trim(char *str)
{
	int off, i;
	if (str == NULL)
		return NULL;
	for (i = 0; isspace(str[i]); ++i) ;
	off = i;
	while (str[i]) {
		str[i - off] = str[i];
		++i;
	}
	for (i -= off; isspace(str[--i]) && i >= 0;) ;
	str[++i] = '\0';
	return str;
}

cpe_dict_t *cpe_dict_new(const char *fname)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	cpe_dict_t *ret;

	if ((doc = xmlParseFile(fname)) == NULL)
		return NULL;

	if ((root = xmlDocGetRootElement(doc)) == NULL)
		return NULL;

	ret = cpe_dict_new_xml(root);

	xmlFreeDoc(doc);

	return ret;
}

cpe_dict_t *cpe_dict_new_xml(xmlNodePtr node)
{
	cpe_dict_t *ret;
	cpe_dict_item_t *item;
	xmlNodePtr cur;

	if (xmlStrcmp(node->name, BAD_CAST "cpe-list") != 0)
		return NULL;

	if ((ret = cpe_dict_new_empty()) == NULL)
		return NULL;

	for (node = node->xmlChildrenNode; node != NULL; node = node->next) {
		if (xmlStrcmp(node->name, BAD_CAST "cpe-item") == 0) {
			if ((item = cpe_dictitem_new_xml(node)) == NULL)
				continue;
			if (!cpe_dict_add_item(ret, item)) {
				cpe_dictitem_delete(item);
				cpe_dict_delete(ret);
				return NULL;
			}
		} else if (xmlStrcmp(node->name, BAD_CAST "generator") == 0) {
			for (cur = node->xmlChildrenNode; cur != NULL;
			     cur = cur->next) {
				if (xmlStrcmp
				    (cur->name, BAD_CAST "product_name") == 0)
					ret->generator.product_name =
					    (char *)xmlNodeGetContent(cur);
				if (xmlStrcmp
				    (cur->name,
				     BAD_CAST "product_version") == 0)
					ret->generator.product_version =
					    (char *)xmlNodeGetContent(cur);
				if (xmlStrcmp
				    (cur->name, BAD_CAST "schema_version") == 0)
					ret->generator.schema_version =
					    (char *)xmlNodeGetContent(cur);
				if (xmlStrcmp(cur->name, BAD_CAST "timestamp")
				    == 0)
					ret->generator.timestamp =
					    (char *)xmlNodeGetContent(cur);
			}
		}
	}

	return ret;
}

const size_t CPE_DICT_CPES_INITIAL_ALLOC = 8;

cpe_dict_t *cpe_dict_new_empty()
{
	cpe_dict_t *dict;

	dict = malloc(sizeof(cpe_dict_t));
	if (dict == NULL)
		return NULL;

	memset(dict, 0, sizeof(cpe_dict_t));

	dict->cpes_alloc_ = CPE_DICT_CPES_INITIAL_ALLOC;
	if ((dict->cpes = malloc(dict->cpes_alloc_ * sizeof(cpe_t *))) == NULL) {
		free(dict);
		return NULL;
	}

	return dict;
}

bool cpe_dict_add_item(cpe_dict_t * dict, cpe_dict_item_t * item)
{
	cpe_t **bak;

	if (dict == NULL || item == NULL)
		return false;

	if (dict->cpes_alloc_ < dict->item_n + 1) {
		dict->cpes_alloc_ *= 2;
		bak = dict->cpes;
		if ((dict->cpes =
		     realloc(bak,
			     dict->cpes_alloc_ * sizeof(cpe_t *))) == NULL) {
			dict->cpes = bak;
			return false;
		}
	}

	dict->cpes[dict->item_n++] = item->name;

	if (dict->first == NULL)
		dict->first = dict->last = item;
	else {
		dict->last->next = item;
		dict->last = item;
	}
	item->next = NULL;

	return true;
}

void cpe_dict_delete(cpe_dict_t * dict)
{
	cpe_dict_item_t *item, *next;

	if (dict == NULL)
		return;

	for (item = dict->first; item != NULL; item = next) {
		next = item->next;
		cpe_dictitem_delete(item);
	}

	free(dict->cpes);
	free(dict->generator.product_name);
	free(dict->generator.product_version);
	free(dict->generator.schema_version);
	free(dict->generator.timestamp);
	free(dict);
}

cpe_dict_item_t *cpe_dictitem_new_empty()
{
	cpe_dict_item_t *item;

	item = malloc(sizeof(cpe_dict_item_t));
	if (item == NULL)
		return NULL;

	memset(item, 0, sizeof(cpe_dict_item_t));

	return item;
}

cpe_dict_item_t *cpe_dictitem_new_xml(xmlNodePtr node)
{
	cpe_dict_item_t *item;
	cpe_dict_check_t *check;
	xmlNodePtr cur;
	xmlChar *data;
	cpe_dict_reference_t ref = { NULL, NULL }
	, *pref;

	if (xmlStrcmp(node->name, BAD_CAST "cpe-item") != 0)
		return NULL;

	item = cpe_dictitem_new_empty();
	if (item == NULL)
		return NULL;

	data = xmlGetProp(node, BAD_CAST "name");
	if (data == NULL || (item->name = cpe_new((char *)data)) == NULL) {
		free(item);
		free(data);
		return NULL;
	}
	free(data);

	for (node = node->xmlChildrenNode; node != NULL; node = node->next) {
		if (item->title == NULL
		    && xmlStrcmp(node->name, BAD_CAST "title") == 0)
			item->title = str_trim((char *)xmlNodeGetContent(node));
		else if (xmlStrcmp(node->name, BAD_CAST "notes") == 0) {
			for (cur = node->xmlChildrenNode; cur != NULL;
			     cur = cur->next) {
				if (xmlStrcmp(cur->name, BAD_CAST "note") != 0)
					continue;
				data =
				    BAD_CAST str_trim((char *)
						      xmlNodeGetContent(cur));
				if (data)
					APPEND_ITEM(char *, (char *)data,
						    item->notes, item->notes_n,
						    item->notes_alloc_, 4);
			}
		} else if (xmlStrcmp(node->name, BAD_CAST "check") == 0) {
			check = cpe_dictcheck_new_xml(node);
			if (check)
				APPEND_ITEM(cpe_dict_check_t *, check,
					    item->check, item->check_n,
					    item->check_alloc_, 4);
		} else if (xmlStrcmp(node->name, BAD_CAST "references") == 0) {
			for (cur = node->xmlChildrenNode; cur != NULL;
			     cur = cur->next) {
				if (xmlStrcmp(cur->name, BAD_CAST "reference")
				    != 0)
					continue;
				if (data)
					APPEND_ITEM(cpe_dict_reference_t, ref,
						    item->references,
						    item->references_n,
						    item->references_alloc_, 4);
				pref =
				    item->references + item->references_n - 1;
				pref->content =
				    str_trim((char *)xmlNodeGetContent(cur));
				pref->href =
				    (char *)xmlGetProp(cur, BAD_CAST "href");
			}
		}
	}

	return item;
}

void cpe_dictitem_delete(cpe_dict_item_t * item)
{
	int i;

	if (item == NULL)
		return;

	cpe_delete(item->name);

	free(item->title);

	for (i = 0; i < (int)item->notes_n; ++i)
		free(item->notes[i]);
	free(item->notes);

	cpe_delete(item->depracated);
	free(item->depracation_date);

	for (i = 0; i < (int)item->references_n; ++i) {
		free(item->references[i].href);
		free(item->references[i].content);
	}
	free(item->references);

	for (i = 0; i < (int)item->check_n; ++i)
		cpe_dictcheck_delete(item->check[i]);
	free(item->check);

	free(item);
}

cpe_dict_check_t *cpe_dictcheck_new_xml(xmlNode * node)
{
	xmlChar *data;
	cpe_dict_check_t *check;

	if (xmlStrcmp(node->name, BAD_CAST "check") != 0)
		return NULL;

	if ((check = malloc(sizeof(cpe_dict_check_t))) == NULL)
		return NULL;
	memset(check, 0, sizeof(cpe_dict_check_t));

	data = xmlGetProp(node, BAD_CAST "system");
	if (data)
		check->system = (char *)data;

	data = xmlGetProp(node, BAD_CAST "href");
	if (data)
		check->href = (char *)data;

	check->identifier = str_trim((char *)xmlNodeGetContent(node));

	return check;
}

void cpe_dictcheck_delete(cpe_dict_check_t * check)
{
	if (check == NULL)
		return;
	free(check->identifier);
	free(check->system);
	free(check->href);
	free(check);
}
