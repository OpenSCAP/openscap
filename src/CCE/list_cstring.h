#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>

#ifndef _LIST_CSTRING_H
#define _LIST_CSTRING_H

struct list_cstring
{
	struct list_cstring* next;
	char* value;
};

void list_cstring_add(struct list_cstring* list, char*);
void list_cstring_clear(struct list_cstring* list);

#endif
