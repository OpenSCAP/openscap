
#include "cce.h"

#ifndef _CCE_PRIV_H
#define _CCE_PRIV_H


bool validateFile(const char *filename);
void parseDoc(char* docname, struct CCE* cce, char* id);
void processNode(xmlTextReaderPtr reader, struct CCE* cce, char* id );
void processItem(xmlTextReaderPtr reader, struct CCE* cce);
void processDescription(xmlTextReaderPtr reader, struct CCE* cce);
void processParameter(xmlTextReaderPtr reader, struct CCE* cce);
void processTechMech(xmlTextReaderPtr reader, struct CCE* cce);
void processRefs(xmlTextReaderPtr reader, struct CCE* cce);

#endif

