/**
 * @file cce.h
 * \brief Interface to Common Configuration Enumeration (CCE)
 *
 * See more details at http://cce.mitre.org/
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
 *      Riley C. Porter <Riley.Porter@g2-inc.com>
 */

#include <stdbool.h>

#include "list_cstring.h"
#include "list_refs.h"

#ifndef _CCE_H
#define _CCE_H

struct cce {
	char *id;
	char *description;
	struct list_cstring *parameters;
	struct list_cstring *technicalmechanisms;
	struct list_refs *references;
};

/**
 * Initialize cce structure
 */
void cce_init(struct cce *cce);

/**
 * Clear cce structure
 */
void cce_clear(struct cce *cce);

/**
 * Vlaidate CCE XML file
 * @return result of validation
 */
bool cce_validate(const char *filename);

/**
 * Parse CCE XML file and store constents of entry with given ID into cce structure.
 */
void cce_parse(char *docname, struct cce *cce, char *id);

#endif
