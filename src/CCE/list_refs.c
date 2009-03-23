/**
 * @file list_refs.c
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

#include "list_refs.h"

void list_refs_add(struct list_refs *list, char *source, char *value)
{
	if (!list->value) {
		list->value = value;
		list->source = source;
		return;
	}
	struct list_refs *current = list;
	while (current->next)
		current = current->next;
	struct list_refs *newitem =
	    (struct list_refs *)malloc(sizeof(struct list_refs));
	newitem->value = value;
	newitem->source = source;
	newitem->next = NULL;
	current->next = newitem;
	return;
}

void list_refs_clear(struct list_refs *list)
{
	struct list_refs *current = list;
	struct list_refs *next = current;
	do {
		current = next;
		if (current->source)
			xmlFree(current->source);
		if (current->value)
			xmlFree(current->value);
		next = current->next;
		free(current);
	} while (NULL != next);
	return;
}
