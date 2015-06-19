/**
 * @file oval_collection_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#ifndef OVALCOLLECTION_H_
#define OVALCOLLECTION_H_
#include "../common/util.h"

OSCAP_HIDDEN_START;

//struct oval_collection;
//struct oval_iterator;

struct oval_collection *oval_collection_new(void);
void oval_collection_free(struct oval_collection *);
void oval_collection_free_items(struct oval_collection *, oscap_destruct_func);
int oval_collection_is_empty(struct oval_collection *collection);
void oval_collection_add(struct oval_collection *, void *);
struct oval_iterator *oval_collection_iterator(struct oval_collection *);
struct oval_iterator *oval_collection_iterator_new(void);
void oval_collection_iterator_add(struct oval_iterator *, void *);
bool oval_collection_iterator_has_more(struct oval_iterator *);
int oval_collection_iterator_remaining(struct oval_iterator *);
void *oval_collection_iterator_next(struct oval_iterator *);
void oval_collection_iterator_free(struct oval_iterator *);

struct oval_string_iterator;

OSCAP_HIDDEN_END;

#endif				/* OVALCOLLECTION_H_ */
