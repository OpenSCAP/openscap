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


#ifndef OSCAP_UTIL_H_
#define OSCAP_UTIL_H_

#include <stdbool.h>
#include <assert.h>
#include "public/oscap.h"
#include "alloc.h"
#include <stdarg.h>

#ifndef __attribute__nonnull__
#define __attribute__nonnull__(x) assert((x) != NULL)
#endif

/*
 * Start a list of declarations that should not be available from outside the
 * library.  Must be matched with OSCAP_HIDDEN_END.
 */
#ifndef OSCAP_HIDDEN_START
#define OSCAP_HIDDEN_START _Pragma("GCC visibility push(hidden)")
#endif

/*
 * Start a list of declarations that should not be available from outside the
 * library started by OSCAP_HIDDEN_END.
 */
#ifndef OSCAP_HIDDEN_END
#define OSCAP_HIDDEN_END _Pragma("GCC visibility pop")
#endif

OSCAP_HIDDEN_START

/**
 * Function pointer to an object destructor.
 */
typedef void (*oscap_destruct_func) (void *);

/**
 * Pointer to a clone function.
 */
typedef void* (*oscap_clone_func) (void *);

/**
 * Function pointer to an object consumer.
 */
typedef void (*oscap_consumer_func) (void *, void *);

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
	RTYPE SNAME##_get_##MNAME(const struct SNAME* item) { return (CONV(item->MEXP)); }

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
 * @see OSCAP_GENERIC_GETTER
 */
#define OSCAP_GETTER(RTYPE,SNAME,MNAME) \
	OSCAP_GENERIC_GETTER(RTYPE,SNAME,MNAME,MNAME)

#define ITERATOR_CAST(x) ((struct oscap_iterator*)(x))
#define OSCAP_ITERATOR(n) struct n##_iterator*
#define OSCAP_ITERATOR_FWD(n) struct n##_iterator;
#define OSCAP_ITERATOR_HAS_MORE(n) bool n##_iterator_has_more(OSCAP_ITERATOR(n) it) { return oscap_iterator_has_more(ITERATOR_CAST(it)); }
#define OSCAP_ITERATOR_NEXT(t,n) t n##_iterator_next(OSCAP_ITERATOR(n) it) { return oscap_iterator_next(ITERATOR_CAST(it)); }
#define OSCAP_ITERATOR_FREE(n) void n##_iterator_free(OSCAP_ITERATOR(n) it) { oscap_iterator_free(ITERATOR_CAST(it)); }
#define OSCAP_ITERATOR_RESET(n) void n##_iterator_reset(OSCAP_ITERATOR(n) it) { oscap_iterator_reset(ITERATOR_CAST(it)); }
#define OSCAP_ITERATOR_DETACH(t,n) t n##_iterator_detach(OSCAP_ITERATOR(n) it) { return oscap_iterator_detach(ITERATOR_CAST(it)); }
#define OSCAP_ITERATOR_GEN_T(t,n) OSCAP_ITERATOR_FWD(n) OSCAP_ITERATOR_HAS_MORE(n) OSCAP_ITERATOR_RESET(n) OSCAP_ITERATOR_NEXT(t,n) OSCAP_ITERATOR_FREE(n)
#define OSCAP_ITERATOR_GEN(n) OSCAP_ITERATOR_GEN_T(struct n*,n)

#define OSCAP_ITERATOR_REMOVE_T(t,n,destructor) \
		void n##_iterator_remove(OSCAP_ITERATOR(n) it) { destructor(oscap_iterator_detach(ITERATOR_CAST(it))); }
#define OSCAP_ITERATOR_REMOVE(n,destructor) OSCAP_ITERATOR_REMOVE_T(struct n*,n,destructor)
#define OSCAP_ITERATOR_REMOVE_F(n) OSCAP_ITERATOR_REMOVE(n, n##_free)


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
        struct ITYPE##_iterator* SNAME##_get_##MNAME(const struct SNAME* item) \
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
	RTYPE SNAME##_get_##MNAME(const struct SNAME* item, const char* key) \
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

#define OSCAP_SETTER_HEADER(SNAME, MTYPE, MNAME) bool SNAME##_set_##MNAME(struct SNAME *obj, MTYPE newval)

/**
 * Generete a setter function with a check.
 * Signature of the generated function will be as follows (substitute uppercase strings with actual params):
 * @code bool SNAME_get_MNAME(struct SNAME *obj, MTYPE *item); @endcode
 * @param SNAME Name of the structure.
 * @param MTYPE Type of the member item to set.
 * @param MNAME Name of the member item to set.
 * @param CHECK Code to check validity of the assignment (you can use obj and item variables here).
 * @param DELETER Function used to delete the old value (or empty string).
 * @param ASSIGNER Function used to assign value to the item.
 */
#define OSCAP_SETTER_GENERIC_CHECK(SNAME, MTYPE, MNAME, CHECK, DELETER, ASSIGNER) \
	OSCAP_SETTER_HEADER(SNAME, MTYPE, MNAME) \
	{ if (!(CHECK)) return false; DELETER(obj->MNAME); obj->MNAME = ASSIGNER(newval); return true; }

/**
 * Generete a setter function without a check.
 * @see OSCAP_SETTER_GENERIC_CHECK
 */
#define OSCAP_SETTER_GENERIC(SNAME, MTYPE, MNAME, DELETER, ASSIGNER) \
	OSCAP_SETTER_HEADER(SNAME, MTYPE, MNAME) \
	{ DELETER(obj->MNAME); obj->MNAME = ASSIGNER(newval); return true; }

/**
 * Generete a setter function without a check that does not delete the previous value.
 * @see OSCAP_SETTER_GENERIC_CHECK
 */
#define OSCAP_SETTER_GENERIC_NODELETE(SNAME, MTYPE, MNAME, ASSIGNER) \
	OSCAP_SETTER_HEADER(SNAME, MTYPE, MNAME) \
	{ obj->MNAME = ASSIGNER(newval); return true; }

/**
 * Generete a setter function using a simple assignment.
 * @see OSCAP_SETTER_GENERIC_CHECK
 */
#define OSCAP_SETTER_SIMPLE(SNAME, MTYPE, MNAME) \
	OSCAP_SETTER_GENERIC_NODELETE(SNAME, MTYPE, MNAME, )

/**
 * Assign a string value to a structure member.
 * @param SNAME Structure name
 * @param MNAME Name of structure member containing the string to be set.
 */
#define OSCAP_SETTER_STRING(SNAME, MNAME) \
	OSCAP_SETTER_GENERIC(SNAME, const char *, MNAME, free, oscap_strdup)

/**
 * Define both, getter and setter for a string structure member.
 */
#define OSCAP_ACCESSOR_STRING(SNAME, MNAME) \
	OSCAP_GETTER(const char*, SNAME, MNAME) OSCAP_SETTER_STRING(SNAME, MNAME)

/**
 * Define both, getter and setter for a text structure member.
 */
#define OSCAP_ACCESSOR_TEXT(SNAME, MNAME) \
	OSCAP_GETTER(struct oscap_text *, SNAME, MNAME) \
    OSCAP_SETTER_GENERIC(SNAME, struct oscap_text*, MNAME, oscap_text_free, )

/**
 * Define both, getter and setter for a structure member using simple get/set.
 */
#define OSCAP_ACCESSOR_SIMPLE(MTYPE, SNAME, MNAME) \
	OSCAP_GETTER(MTYPE, SNAME, MNAME) OSCAP_SETTER_SIMPLE(SNAME, MTYPE, MNAME)

/**
 * Define both, getter and setter for a structure member using simple get/set
 * using different expression to het the member.
 */
#define OSCAP_ACCESSOR_EXP(MTYPE, SNAME, MNAME, MEXP) \
    OSCAP_GENERIC_GETTER(MTYPE, SNAME, MNAME, MEXP) \
    OSCAP_SETTER_HEADER(SNAME, MTYPE, MNAME) { obj->MEXP = newval; return true; }

/**
 * Generate function to insert an item into a list.
 * Signature of the generated function will be as follows (substitute uppercase strings with actual params):
 * @code bool SNAME_add_FNAME(struct SNAME *obj, struct MTYPE *item); @endcode
 * @param SNAME structure typename (w/o the struct keyword)
 * @param FNAME how should function name part after the _add_ look like
 * @param MTYPE list member type
 * @param MNAME name of the list within the structure
 */
#define OSCAP_INSERTER(SNAME, FNAME, MTYPE, MNAME) \
	bool SNAME##_add_##FNAME(struct SNAME *obj, struct MTYPE *item) \
	{ oscap_list_add(obj->MNAME, item); return true; }

/* Generate iterator getter and list inserter */
#define OSCAP_IGETINS(ITYPE, SNAME, MNAME, FNAME) \
	OSCAP_IGETTER(ITYPE, SNAME, MNAME) OSCAP_INSERTER(SNAME, FNAME, ITYPE, MNAME)
/* Generate iterator getter, list inserter, and iterator manipulation functions. */
#define OSCAP_IGETINS_GEN(ITYPE, SNAME, MNAME, FNAME) \
	OSCAP_IGETTER_GEN(ITYPE, SNAME, MNAME) OSCAP_INSERTER(SNAME, FNAME, ITYPE, MNAME)

/**
 * Define mapping between symbolic constant and its string representation.
 *
 * It is supposed to define array of these structures, ending with element
 * with the @a string member set to NULL. Value of such member also defines
 * the default value for strings not defined elsewhere.
 */
struct oscap_string_map {
	int value;		/* integer/enum value */
	const char *string;	/* string representation of the value */
};

/**
 * Convert a string to an enumeration constant.
 * @param map An array of oscap_string_map structures that defines mapping between constants and strings.
 * @param str string to be converted
 * @memberof oscap_string_map
 */
int oscap_string_to_enum(const struct oscap_string_map *map, const char *str);

/**
 * Convert an enumeration constant to its corresponding string representation.
 * @param map An array of oscap_string_map structures that defines mapping between constants and strings.
 * @param val value to be converted
 * @memberof oscap_string_map
 */
const char *oscap_enum_to_string(const struct oscap_string_map *map, int val);

/**
 * Use strdup on string, if string is NULL, return NULL
 * @param str String we want to duplicate
 */
char *oscap_strdup(const char *str);

/**
 * Use strtol on string, if string is NULL, return NaN
 * @param str String we want to duplicate
 * @param endptr see man strtol
 * @param base see man strtol
 */
float oscap_strtol(const char *str, char **endptr, int base);

/**
 * Split a string.
 * Split string using given delimiter.
 * Produces NULL-terminated array of strings.
 * Modifies its first argument!
 * @param str String we want to split
 * @param delim Delimiter of string parts
 */
char **oscap_split(char *str, const char *delim);


/// Use strcmp on strings, NULL safe
int oscap_strcmp(const char *s1, const char *s2);
/// Check for string equality
bool oscap_streq(const char *s1, const char *s2);
/// Trim whitespace (modifies its argument!)
char *oscap_trim(char *str);
/// Print to a newly allocated string using a va_list.
char *oscap_vsprintf(const char *fmt, va_list ap);

// FIXME: This is there because of the SCE engine using this particular function
OSCAP_HIDDEN_END;

/// Print to a newly allocated string using varialbe arguments.
char *oscap_sprintf(const char *fmt, ...);

OSCAP_HIDDEN_START;

/// In a list of key-value pairs (odd indicies are keys, even values), find a value for given key
const char *oscap_strlist_find_value(char ** const kvalues, const char *key);
/// Right trim @a ch characters (modifies its first argument!)
char *oscap_rtrim(char *str, char ch);
/// Convert string to uppercase (modifies its first argument!)
void oscap_strtoupper(char *str);

// check pointer equality
bool oscap_ptr_cmp(void *node1, void *node2);

/**
 * find file with given name and mode in given paths
 * @param filename filename to be found
 * @param mode desired file mode (e.g. R_OK for readable files)
 * @param pathvar environment variable to check for path, can be NULL
 * @param path path where to search the file
 */
char *oscap_find_file(const char *filename, int mode, const char *pathvar, const char *path);

/**
 * A helper function to expand given shorthand IPv6
 *
 * example:
 * oscap_expand_ipv6("::1") returns "0:0:0:0:0:0:0:1"
 *
 * @par
 * Asset Identification specification requires fully expanded IPv6 addresses.
 * Despite this being greatly discouraged by RFC 5952 we have to comply and
 * expand given IPv6, otherwise the resulting AI content would be invalid.
 */
char *oscap_expand_ipv6(const char *input);

#ifndef OSCAP_CONCAT
# define OSCAP_CONCAT1(a,b) a ## b
# define OSCAP_CONCAT(a,b) OSCAP_CONCAT1(a,b)
#endif

#define OSCAP_GSYM(s) OSCAP_CONCAT(___G_, s)

#define protect_errno                                                   \
        for (int OSCAP_CONCAT(__e,__LINE__)=errno, OSCAP_CONCAT(__s,__LINE__)=1; OSCAP_CONCAT(__s,__LINE__)--; errno=OSCAP_CONCAT(__e,__LINE__))

OSCAP_HIDDEN_END;

#endif				/* OSCAP_UTIL_H_ */
