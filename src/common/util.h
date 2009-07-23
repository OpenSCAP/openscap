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
 * Common openscap utilities.
 */

#ifndef OSCAP_UTIL_H_
#define OSCAP_UTIL_H_

#include "alloc.h"

/**
 * Function pointer to an object destructor.
 */
typedef void(*oscap_destruct_func)(void*);

/**
 * Generate a getter function with an optional conversion.
 * Header of the generated function will look like this (substitute the capital letter sequences with the actual parametres):
 * @code RTYPE SNAME_MNAME(const struct SNAME*); @endcode
 * @param RTYPE return type
 * @param CONV conversion expression, i.e. either a type conversion of form '(type)' or an unary function identifier
 * @param SNAME name of the structure of which we are getting a member (without the struct keyword)
 * @param MNAME member name affects how the resultant function will be called
 * @param MEXP expression to get the member
 */
#define OSCAP_GENERIC_GETTER_CONV(RTYPE,CONV,SNAME,MNAME,MEXP) \
	RTYPE SNAME##_##MNAME(const struct SNAME* item) { return (CONV(item->MEXP)); }

/**
 * Generate a getter function.
 * @param RTYPE return type
 * @param SNAME name of the structure of which we are getting a member
 * @param MNAME member name affects how the resultant function will be called
 * @param MEXP expression to get the member
 * @see OSCAP_GENERIC_GETTER_CONV
 */
#define OSCAP_GENERIC_GETTER(RTYPE,SNAME,MNAME,MEXP) \
	OSCAP_GENERIC_GETTER_CONV(RTYPE,,SNAME,MNAME,MEXP)

/**
 * Generate a getter function with an explicit conversion to the target data type.
 * @param RTYPE return type
 * @param SNAME name of the structure of which we are getting a member
 * @param MNAME member name affects how the resultant function will be called
 * @param MEXP expression to get the member
 * @see OSCAP_GENERIC_GETTER_CONV
 */
#define OSCAP_GENERIC_GETTER_FORCE(RTYPE,SNAME,MNAME,MEXP) \
	OSCAP_GENERIC_GETTER_CONV(RTYPE,(RTYPE),SNAME,MNAME,MEXP)

/**
 * Generate a getter function with an explicit conversion to the target data type.
 * Member name and second part of the generated function's name must match.
 * @param RTYPE return type
 * @param SNAME name of the structure of which we are getting a member
 * @param MNAME member name affects how the resultant function will be called
 * @param MEXP expression to get the member
 * @see OSCAP_GENERIC_GETTER_FORCE
 */
#define OSCAP_GETTER_FORCE(RTYPE,SNAME,MNAME) \
	OSCAP_GENERIC_GETTER_FORCE(RTYPE,SNAME,MNAME,MNAME)

/**
 * Generate a getter function.
 * Member name and second part of the generated function's name must match.
 * @param RTYPE return type
 * @param SNAME name of the structure of which we are getting a member
 * @param MNAME member name affects how the resultant function will be called
 * @param MEXP expression to get the member
 * @see OSCAP_GENERIC_GETTER
 */
#define OSCAP_GETTER(RTYPE,SNAME,MNAME) \
	OSCAP_GENERIC_GETTER(RTYPE,SNAME,MNAME,MNAME)



#define ITERATOR_CAST(x) ((struct oscap_iterator*)(x))
#define OSCAP_ITERATOR(n) struct n##_iterator*
#define OSCAP_ITERATOR_FWD(n) struct n##_iterator;
#define OSCAP_ITERATOR_HAS_MORE(n) bool n##_iterator_has_more(OSCAP_ITERATOR(n) it) { return oscap_iterator_has_more(ITERATOR_CAST(it)); }
#define OSCAP_ITERATOR_NEXT(t,n) t n##_iterator_next(OSCAP_ITERATOR(n) it) { return oscap_iterator_next(ITERATOR_CAST(it)); }
#define OSCAP_ITERATOR_GEN_T(t,n) OSCAP_ITERATOR_FWD(n) OSCAP_ITERATOR_HAS_MORE(n) OSCAP_ITERATOR_NEXT(t,n)
#define OSCAP_ITERATOR_GEN(n) OSCAP_ITERATOR_GEN_T(struct n*,n)

/**
 * Generate an iterator getter function.
 * Signature of the generated function will be as follows (substitute uppercase strings with actual params):
 * @code struct ITYPE_iterator* SNAME_MNAME(const struct SNAME*); @endcode
 * @param ITYPE iterator type (without the '_iterator' suffix)
 * @param SNAME name of type the structure containing the list
 * @param MANME list itself
 * @param CONV convert expression
 */
#define OSCAP_IGETTER_CONV(ITYPE,SNAME,MNAME,CONV) \
        struct ITYPE##_iterator* SNAME##_##MNAME(const struct SNAME* item) \
        { return oscap_iterator_new((CONV(item))->MNAME); }


/**
 * Generate an iterator getter function.
 * Signature of the generated function will be as follows (substitute uppercase strings with actual params):
 * @code struct ITYPE_iterator* SNAME_MNAME(const struct SNAME*); @endcode
 * @param ITYPE iterator type (without the '_iterator' suffix)
 * @param SNAME name of type the structure containing the list
 * @param MANME list itself
 */
#define OSCAP_IGETTER(ITYPE,SNAME,MNAME) OSCAP_IGETTER_CONV(ITYPE,SNAME,MNAME,)

/**
 * Generate an iterator and its getter.
 * @see OSCAP_IGETTER
 * @see OSCAP_ITERATOR_GEN
 */
#define OSCAP_IGETTER_GEN(ITYPE,SNAME,MNAME) OSCAP_IGETTER(ITYPE,SNAME,MNAME) OSCAP_ITERATOR_GEN(ITYPE)

/**
 * Generete a geter function from a hash table.
 * Signature of the generated function will be as follows (substitute uppercase strings with actual params):
 * @code RTYPE SNAME_MNAME(const struct SNAME* item, const char* key); @endcode
 * @param RTYPE type of the hash table item
 * @param SNAME name of the structure
 * @param MNAME structure member name
 * @param MEXP expression to get the member variable (i.e. the hash table)
 */
#define OSCAP_HGETTER_EXP(RTYPE,SNAME,MNAME,MEXP) \
	RTYPE SNAME##_##MNAME(const struct SNAME* item, const char* key) \
	{ return oscap_htable_get(item->MEXP, key); }

/**
 * Generete a geter function from a hash table.
 * Signature of the generated function will be as follows (substitute uppercase strings with actual params):
 * @code RTYPE SNAME_MNAME(const struct SNAME* item, const char* key); @endcode
 * @param RTYPE type of the hash table item
 * @param SNAME name of the structure
 * @param MNAME structure member name
 */
#define OSCAP_HGETTER(RTYPE,SNAME,MNAME) OSCAP_HGETTER_EXP(RTYPE,SNAME,MNAME,MNAME)


/**
 * Generete a geter function from a hash table.
 * Signature of the generated function will be as follows (substitute uppercase strings with actual params):
 * @code struct RTYPE* SNAME_MNAME(const struct SNAME* item, const char* key); @endcode
 * @param RTYPE type of the hash table item
 * @param SNAME name of the structure
 * @param MNAME structure member name
 */
#define OSCAP_HGETTER_STRUCT(RTYPE,SNAME,MNAME) OSCAP_HGETTER_EXP(struct RTYPE*,SNAME,MNAME,MNAME)


/**
 * Define mapping between symbolic constant and its string representation.
 *
 * It is supposed to define array of these structures, ending with element
 * with the @a string member set to NULL. Value of such member also defines
 * the default value for strings not defined elsewhere.
 */
struct oscap_string_map {
	int value;           ///< integer/enum value
	const char* string;  ///< string representation of the value
};

/**
 * Convert a string to an enumeration constant.
 * @param map An array of oscap_string_map structures that defines mapping between constants and strings.
 * @param str string to be converted
 * @relates oscap_string_map
 */
int oscap_string_to_enum(const struct oscap_string_map* map, const char* str);

/**
 * Convert an enumeration constant to its corresponding string representation.
 * @param map An array of oscap_string_map structures that defines mapping between constants and strings.
 * @param val value to be converted
 * @relates oscap_string_map
 */
const char* oscap_enum_to_string(const struct oscap_string_map* map, int val);


#endif // OSCAP_UTIL_H_

