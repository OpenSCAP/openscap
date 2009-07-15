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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */


/**
 * @file
 * General OpenScap functions and types.
 * @author Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifndef OSCAP_H_
#define OSCAP_H_

/**
 * @struct oscap_iterator
 * XCCDF iterator.
 *
 * Any iterator name takes a form of <tt>struct OBJECT_iterator</tt>, where @c OBJECT
 * is a name of a concrete datatype the iterator iterates over.
 */

/** @struct oscap_string_iterator
 * String iterator.
 * @see oscap_iterator
 */
struct oscap_string_iterator;
/// @relates oscap_string_iterator
const char* oscap_string_iterator_next(struct oscap_string_iterator* it);
/// @relates oscap_string_iterator
bool oscap_string_iterator_has_more(struct oscap_string_iterator* it);


/**
 * Release library internal caches.
 *
 * This function should be called once you finish working with
 * any of the libraries included in OpenScap framework.
 * It frees internally allocated memory, e.g. cache of the XML parser.
 */
void oscap_cleanup(void);

#endif

