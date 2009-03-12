#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>

#ifndef _LIST_REFS_H
#define _LIST_REFS_H

struct list_Refs
{
	struct list_Refs* next;
	xmlChar* source;
	xmlChar* value;
};

void list_Refs_add(struct list_Refs* list, xmlChar* source, xmlChar* value);
void list_Refs_clear(struct list_Refs* list);

#endif