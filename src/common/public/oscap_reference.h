/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 */


#pragma once
#ifndef OSCAP_REFERENCE_H_
#define OSCAP_REFERENCE_H_

#include "oscap_export.h"

/**
 * Dublin Core reference
 */
struct oscap_reference;

/// @see oscap_reference
struct oscap_reference_iterator;
/// @memberof oscap_reference_iterator
OSCAP_API bool oscap_reference_iterator_has_more(struct oscap_reference_iterator *it);
/// @memberof oscap_reference_iterator
OSCAP_API struct oscap_reference *oscap_reference_iterator_next(struct oscap_reference_iterator *it);
/// @memberof oscap_reference_iterator
OSCAP_API void oscap_reference_iterator_free(struct oscap_reference_iterator *it);
/// @memberof oscap_reference_iterator
OSCAP_API void oscap_reference_iterator_reset(struct oscap_reference_iterator *it);

/// @memberof oscap_reference
OSCAP_API struct oscap_reference *oscap_reference_new(void);
/// @memberof oscap_reference
OSCAP_API void oscap_reference_free(struct oscap_reference *ref);
/// @memberof oscap_reference
OSCAP_API struct oscap_reference *oscap_reference_clone(const struct oscap_reference *ref);

/// @memberof oscap_reference
OSCAP_API bool oscap_reference_get_is_dublincore(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_is_dublincore(struct oscap_reference *obj, bool newval);

/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_href(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_href(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_title(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_title(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_creator(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_creator(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_subject(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_subject(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_description(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_description(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_publisher(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_publisher(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_contributor(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_contributor(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_date(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_date(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_type(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_type(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_format(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_format(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_identifier(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_identifier(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_source(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_source(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_language(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_language(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_relation(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_relation(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_coverage(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_coverage(struct oscap_reference *obj, const char *newval);
/// @memberof oscap_reference
OSCAP_API const char *oscap_reference_get_rights(const struct oscap_reference *item);
/// @memberof oscap_reference
OSCAP_API bool oscap_reference_set_rights(struct oscap_reference *obj, const char *newval);

#endif
