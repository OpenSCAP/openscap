#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>

#ifndef _LIST_XMLCHAR_H
#define _LIST_XMLCHAR_H

struct list_XMLCHAR
{
	struct list_XMLCHAR* next;
	xmlChar* value;
};

void list_XMLCHAR_add(struct list_XMLCHAR* list, xmlChar*);
void list_XMLCHAR_clear(struct list_XMLCHAR* list);

#endif