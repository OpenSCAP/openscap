#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include "list_xmlChar.h"
#include "list_Refs.h"

#ifndef _MAIN_H
#define _MAIN_H

struct CCE
{
	xmlChar* id;
	xmlChar* description; 
	struct list_XMLCHAR* parameters;
	struct list_XMLCHAR* technicalmechanisms;
	struct list_Refs* references;
};

void initCCE(struct CCE* cce);
void clearCCE(struct CCE* cce);

void validateFile(const char *filename);
void parseDoc(char* docname, struct CCE* cce, char* id);
void processNode(xmlTextReaderPtr reader, struct CCE* cce, char* id );
void processItem(xmlTextReaderPtr reader, struct CCE* cce);
void processDescription(xmlTextReaderPtr reader, struct CCE* cce);
void processParameter(xmlTextReaderPtr reader, struct CCE* cce);
void processTechMech(xmlTextReaderPtr reader, struct CCE* cce);
void processRefs(xmlTextReaderPtr reader, struct CCE* cce);

#endif
