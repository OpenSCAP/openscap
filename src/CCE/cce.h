#include "list_cstring.h"
#include "list_Refs.h"
#include <stdbool.h>

#ifndef _CCE_H
#define _CCE_H

struct CCE
{
	char* id;
	char* description; 
	struct list_cstring* parameters;
	struct list_cstring* technicalmechanisms;
	struct list_Refs* references;
};

/**
 * Initialize CCE structure
 */
void cce_init(struct CCE* cce);

/**
 * Clear CCE structure
 */
void cce_clear(struct CCE* cce);

/**
 * Vlaidate CCE XML file
 * @return result of validation
 */
bool cce_validate(const char *filename);

/**
 * Parse CCE XML file and store constents of entry with given ID into CCE structure.
 */
void cce_parse(char* docname, struct CCE* cce, char* id);

#endif
