/**
 * @file   sexp-manip.h
 * @brief  S-expression object manipulation functions - public header
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup OVAL
 * @{
 * @addtogroup PROBES
 * @{
 * @addtogroup SEXPRESSIONS
 * @{
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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */

#pragma once
#ifndef SEXP_MANIP_H
#define SEXP_MANIP_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sexp-types.h>
#include <helpers.h>
#include "oscap_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * number
 */

/**
 * Create a new sexp object from a value and a number type
 * @param t the desired number type
 * @param n pointer to the number value
 */
OSCAP_API SEXP_t *SEXP_number_new (SEXP_numtype_t t, const void *n) __attribute__ ((nonnull (2)));

/**
 * Create a new sexp object from a boolean value.
 * @param n the boolean value to store
 */
OSCAP_API SEXP_t *SEXP_number_newb (bool n);

/**
 * Create a new sexp object from an integer.
 * @param n the integer value to store
 */
OSCAP_API SEXP_t *SEXP_number_newi_8 (int8_t n);

/**
 * Create a new sexp object from an unsigned integer.
 * @param n the unsigned integer value to store
 */
OSCAP_API SEXP_t *SEXP_number_newu_8 (uint8_t n);

/**
 * Get unsigned integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
OSCAP_API uint8_t SEXP_number_getu_8 (const SEXP_t *s_exp);

/**
 * Create a new sexp object from an integer.
 * @param n the integer value to store
 */
OSCAP_API SEXP_t *SEXP_number_newi_16 (int16_t n);

/**
 * Create a new sexp object from an unsigned integer.
 * @param n the unsigned integer value to store
 */
OSCAP_API SEXP_t *SEXP_number_newu_16 (uint16_t n);

/**
 * Create a new sexp object from an integer.
 * The size of the integer may be architecture dependent.
 */
#define SEXP_number_newi SEXP_number_newi_32

/**
 * Create a new sexp object from an integer.
 * @param n the integer value to store
 */
OSCAP_API SEXP_t *SEXP_number_newi_32 (int32_t n);

/**
 * Get integer value from a sexp object.
 * The size of the integer may be architecture dependent.
 */
#define SEXP_number_geti SEXP_number_geti_32

/**
 * Get integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
OSCAP_API int32_t SEXP_number_geti_32 (const SEXP_t *s_exp);

/**
 * Get boolean value from a sexp object.
 * @param s_exp the queried sexp object
 */
OSCAP_API bool SEXP_number_getb (const SEXP_t *s_exp);

/**
 * Create a new sexp object from an unsigned integer.
 * The size of the integer may be architecture dependent.
 */
#define SEXP_number_newu SEXP_number_newu_32

/**
 * Create a new sexp object from an unsigned integer.
 * @param n the integer value to store
 */
OSCAP_API SEXP_t *SEXP_number_newu_32 (uint32_t n);

/**
 * Get unsigned integer value from a sexp object.
 * The size of the integer may be architecture dependent.
 */
#define  SEXP_number_getu SEXP_number_getu_32

/**
 * Get unsigned integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
OSCAP_API uint32_t SEXP_number_getu_32 (const SEXP_t *s_exp);

/**
 * Create a new sexp object from an integer.
 * @param n the integer value to store
 */
OSCAP_API SEXP_t *SEXP_number_newi_64 (int64_t n);

/**
 * Get integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
OSCAP_API int64_t SEXP_number_geti_64 (const SEXP_t *s_exp);

/**
 * Create a new sexp object from an unsigned integer.
 * @param n the integer value to store
 */
OSCAP_API SEXP_t *SEXP_number_newu_64 (uint64_t n);

/**
 * Get unsigned integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
OSCAP_API uint64_t SEXP_number_getu_64 (const SEXP_t *s_exp);

/**
 * Create a new sexp object from an floating point value.
 * @param n the floating point value to store
 */
OSCAP_API SEXP_t *SEXP_number_newf (double n);

/**
 * Get floating point value from a sexp object.
 * @param s_exp the queried sexp object
 */
OSCAP_API double  SEXP_number_getf (const SEXP_t *s_exp);

/**
 * Get a value from a sexp object according to a specified type
 * @param s_exp the queried sexp object
 * @param dst buffer for the value
 * @param type the desired number type
 */
OSCAP_API int SEXP_number_get (const SEXP_t *s_exp, void *dst, SEXP_numtype_t type);

/**
 * Get unsigned integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
OSCAP_API uint16_t SEXP_number_getu_16 (const SEXP_t *s_exp);

/**
 * Free the specified sexp object.
 * @param s_exp the object to be freed
 */
OSCAP_API void    SEXP_number_free (SEXP_t *s_exp);

/**
 * Check whether the provided sexp object is a number.
 * @param s_exp the sexp object to be tested
 */
OSCAP_API bool    SEXP_numberp (const SEXP_t *s_exp);

/**
 * Get the number type of an object.
 * @param sexp the queried sexp object
 */
OSCAP_API SEXP_numtype_t SEXP_number_type (const SEXP_t *sexp);

/*
 * string
 */

/**
 * Create a new sexp object from a string.
 * @param string the string to be stored
 * @param strlen the length of the string in bytes
 */
OSCAP_API SEXP_t *SEXP_string_new  (const void *string, size_t strlen) __attribute__ ((nonnull (1)));

/**
 * Create a new sexp object from a format string.
 * @param format the format of the new string
 * @param ... arguments for the format
 */
OSCAP_API SEXP_t *SEXP_string_newf (const char *format, ...) __attribute__ ((format (printf, 1, 2), nonnull (1)));

/**
 * Free the specified sexp object.
 * @param s_exp the object to be freed
 */
OSCAP_API void SEXP_string_free (SEXP_t *s_exp);

/**
 * Check whether the provided sexp object is a string.
 * @param s_exp the sexp object to be tested
 */
OSCAP_API bool SEXP_stringp (const SEXP_t *s_exp);

/**
 * Get the length of a string in a sexp object.
 * @param s_exp the queried sexp object
 */
OSCAP_API size_t SEXP_string_length (const SEXP_t *s_exp);

/**
 * Compare a string in a sexp object with a C string.
 * @param s_exp the sexp object to be compared
 * @param str the C string to be compared
 */
OSCAP_API int SEXP_strcmp (const SEXP_t *s_exp, const char *str) __attribute__ ((nonnull (2)));

/**
 * Compare a string in a sexp object with a C string.
 * @param s_exp the sexp object to be compared
 * @param str the C string to be compared
 * @param n compare at most n bytes
 */
OSCAP_API int SEXP_strncmp (const SEXP_t *s_exp, const char *str, size_t n) __attribute__ ((nonnull (2)));

/**
 * Get the n-th byte of a string.
 * @param s_exp the sexp object holding the string
 * @param n the index of the desired character
 */
OSCAP_API int SEXP_string_nth (const SEXP_t *s_exp, size_t n);

/**
 * Get a C string from a sexp object.
 */
OSCAP_API char  *SEXP_string_cstr (const SEXP_t *s_exp);

/**
 * Get a C string from a sexp object.
 * The name is stored in the provided buffer.
 * @param s_exp the queried object
 * @param buf the buffer to store the name in
 * @param len the length of the buffer
 */
OSCAP_API size_t SEXP_string_cstr_r (const SEXP_t *s_exp, char *buf, size_t len) __attribute__ ((nonnull (2)));

/**
 * Obsolete function.
 */
OSCAP_API char  *SEXP_string_cstrp (const SEXP_t *s_exp);

/**
 * Get a C substring from a sexp object.
 * @param s_sexp the queried sexp object
 * @param beg the position of the fisrt character of the substring
 * @param len the length of the substring
 */
OSCAP_API char *SEXP_string_subcstr (const SEXP_t *s_exp, size_t beg, size_t len);

/**
 * Compare two sexp strings.
 * @param str_a the first string to compare
 * @param str_b the second string to compare
 */
OSCAP_API int SEXP_string_cmp (const SEXP_t *str_a, const SEXP_t *str_b);

/**
 * Try to cast the supplied sexp string to a boolean.
 * @param s_ext the string to be cast
 */
OSCAP_API bool SEXP_string_getb (const SEXP_t *s_exp);

/*
 * list
 */

/**
 * Create a new sexp list, optionally initialized with the provided sexp arguments.
 * The argument list needs to be terminated with NULL.
 * @param memb the first sexp object to be inserted into the new list. can be NULL.
 * @param ... arbitrary number of elements to be inserted
 */
OSCAP_API SEXP_t *SEXP_list_new (SEXP_t *memb, ...);

/**
 * Free the specified sexp object.
 * @param s_exp the object to be freed
 */
OSCAP_API void    SEXP_list_free (SEXP_t *s_exp);

/**
 * Check whether the provided sexp object is a list.
 * @param s_exp the sexp object to be tested
 */
OSCAP_API bool    SEXP_listp (const SEXP_t *s_exp);

/**
 * Get the length of the sexp list.
 * @param s_sexp the queried sexp object
 */
OSCAP_API size_t  SEXP_list_length (const SEXP_t *s_exp);

/**
 * Get the first element of a list.
 * This function increments element's reference count
 * @param list the queried sexp object
 */
OSCAP_API SEXP_t *SEXP_list_first (const SEXP_t *list);

/**
 * Get the rest of a list.
 * This function increments elements' reference count.
 * @param list the queried sexp object
 */
OSCAP_API SEXP_t *SEXP_list_rest  (const SEXP_t *list);

/**
 * Get the last element of a list.
 * This function increments element's reference count.
 * @param list the queried sexp object
 */
OSCAP_API SEXP_t *SEXP_list_last (const SEXP_t *list);

/**
 * Get the n-th element of a list.
 * This function increments element's reference count.
 * @param list the queried sexp object
 * @param n the position of the element in the list
 */
OSCAP_API SEXP_t *SEXP_list_nth (const SEXP_t *list, uint32_t n);

/**
 * Add an element to a list.
 * This function increments element's reference count.
 * @param list the modified sexp object
 * @param s_exp the element to be added
 */
OSCAP_API SEXP_t *SEXP_list_add (SEXP_t *list, const SEXP_t *s_exp);

/**
 * Create a new list containing the concatenated contents of two lists.
 * This function increments element's reference count.
 * @param list_a the first list to be contatenated
 * @param list_b the list to be attached to the first one
 */
OSCAP_API SEXP_t *SEXP_list_join (const SEXP_t *list_a, const SEXP_t *list_b);

/**
 * Push an element to the head of a list.
 * This function increments element's reference count.
 * @param list the modified sexp object
 * @param s_exp the element to be added
 */
OSCAP_API SEXP_t *SEXP_list_push (SEXP_t *list, const SEXP_t *s_exp);

/**
 * Extract the first element of a list.
 * This function increments element's reference count.
 * @param list the modified sexp object
 */
OSCAP_API SEXP_t *SEXP_list_pop (SEXP_t *list);

/**
 * Sort a list using `compare' as the comparison function.
 */
OSCAP_API SEXP_t *SEXP_list_sort(SEXP_t *list, int(*compare)(const SEXP_t *, const SEXP_t *));

/**
 * Replace the n-th element of a list.
 * This function increments element's reference count.
 * @param list the modified sexp object
 * @param n the index of the element to be replaced
 * @param s_exp the element to be added
 * @return the replaced element
 */
OSCAP_API SEXP_t *SEXP_list_replace (SEXP_t *list, uint32_t n, const SEXP_t *s_exp);

/**
 * Get the first element of a list.
 * This function creates a soft reference to the element.
 * @param list the queried sexp object
 */
OSCAP_API SEXP_t *SEXP_listref_first (SEXP_t *list);

/**
 * Get the rest of a list.
 * This function creates a soft reference to the list.
 * @param list the queried sexp object
 */
OSCAP_API SEXP_t *SEXP_listref_rest (SEXP_t *list);

/**
 * Get the last element of a list.
 * This function creates a soft reference to the element.
 * @param list the queried sexp object
 */
OSCAP_API SEXP_t *SEXP_listref_last (SEXP_t *list);

/**
 * Get the n-th element of a list.
 * This function creates a soft reference to the element.
 * @param list the queried sexp object
 * @param n the position of the element in the list
 */
OSCAP_API SEXP_t *SEXP_listref_nth (SEXP_t *list, uint32_t n);

typedef struct SEXP_it SEXP_it_t;

#define SEXP_IT_RECURSIVE 0x01
#define SEXP_IT_HARDREF   0x02

OSCAP_API SEXP_it_t *SEXP_listit_new (const SEXP_t *list, int flags);
OSCAP_API SEXP_t    *SEXP_listit_next(SEXP_it_t *it);
OSCAP_API SEXP_t    *SEXP_listit_prev (SEXP_it_t *it);
OSCAP_API SEXP_t    *SEXP_listit_length (SEXP_it_t *it);
OSCAP_API SEXP_t    *SEXP_listit_seek (SEXP_it_t *it, uint32_t n);
OSCAP_API void       SEXP_listit_free (SEXP_it_t *it);

typedef struct SEXP_list_it SEXP_list_it;

OSCAP_API SEXP_list_it *SEXP_list_it_new(const SEXP_t *list);
OSCAP_API SEXP_t *SEXP_list_it_next(SEXP_list_it *it);
OSCAP_API void SEXP_list_it_free(SEXP_list_it *it);

#if __STDC_VERSION__ >= 199901L
# include <common/util.h>

/* TODO: use alloca & softref_r here */
/**
 * Iterate through a list, assigning each element to a variable.
 * @param var the var variable is assigned one value from the list in each cycle
 * @param list the list the loop iterates through
 */
#define SEXP_list_foreach(var, list)                                    \
        for (uint32_t OSCAP_CONCAT(i,__LINE__) = 1; ((var) = SEXP_list_nth (list, OSCAP_CONCAT(i,__LINE__))) != NULL; ++OSCAP_CONCAT(i,__LINE__), SEXP_free (var), (var) = NULL)

/**
 * Iterate through a sublist, assigning each element to a variable.
 * @param var the var variable is assigned one value from the list in each cycle
 * @param list the list the loop iterates through
 * @param beg the index of the first element of the sublist
 * @param end the index of the last element of the sublist
 */
#define SEXP_sublist_foreach(var, list, beg, end)                       \
         for (uint32_t OSCAP_CONCAT(i,__LINE__) = (beg); OSCAP_CONCAT(i,__LINE__) <= ((size_t)(end)) && ((var) = SEXP_list_nth (list, OSCAP_CONCAT(i,__LINE__))) != NULL; ++OSCAP_CONCAT(i,__LINE__), SEXP_free (var), (var) = NULL)

#define SEXP_LIST_END (UINT32_MAX - 1)

#endif /* __STDC_VERSION__ >= 199901L */

/*
 * generic
 */
OSCAP_API SEXP_t *SEXP_new (void);

OSCAP_API bool SEXP_emptyp(SEXP_t *sexp);

/**
 * Create a new reference to a sexp object.
 * @param s_exp the object of which to increment the reference count
 */
OSCAP_API SEXP_t *SEXP_ref (const SEXP_t *s_exp);

OSCAP_API SEXP_t *SEXP_unref (SEXP_t *s_exp_o);

/**
 * Create a new soft reference to a sexp object.
 * @param s_exp the object to which create the soft reference
 */
OSCAP_API SEXP_t *SEXP_softref (SEXP_t *s_exp);

/**
 * Check whether an S-exp reference is a "soft" reference
 * @param s_exp the S-exp reference
 */
OSCAP_API bool SEXP_softrefp(const SEXP_t *s_exp);

/**
 * Return the value of the reference counter
 * @param ref
 */
OSCAP_API uint32_t SEXP_refs (const SEXP_t *ref);

OSCAP_API bool SEXP_eq (const SEXP_t *a, const SEXP_t *b);

/**
 * Compare reference pointers.
 */
OSCAP_API int SEXP_refcmp(const SEXP_t *a, const SEXP_t *b);

OSCAP_API bool SEXP_deepcmp(const SEXP_t *a, const SEXP_t *b);

/**
 * Free a sexp object.
 * @param s_exp the object to be freed
 */
OSCAP_API void     SEXP_free (SEXP_t *s_exp);

/**
 * Get the user data type of a sexp object.
 * @param s_exp the object to be queried
 */
OSCAP_API const char *SEXP_datatype (const SEXP_t *s_exp);

/**
 * Set the user data type of a sexp object.
 * @param s_exp the object to be modified
 */
OSCAP_API int SEXP_datatype_set (SEXP_t *s_exp, const char *name) __attribute__ ((nonnull (2)));

/**
 * Set the user data type of the nth sexp object in a list.
 * @param list list containing the object to be modified
 * @param n the position of the object
 * @param name name of the user data type
 */
OSCAP_API int SEXP_datatype_set_nth (SEXP_t *list, uint32_t n, const char *name) __attribute__ ((nonnull (3)));

/**
 * Get the type of a sexp object.
 * @param s_exp the object to be queried
 */
OSCAP_API SEXP_type_t SEXP_typeof (const SEXP_t *s_exp);

/**
 * Get a text description of the sexp object's type.
 * @param s_exp the object to be queried
 */
OSCAP_API const char *SEXP_strtype (const SEXP_t *s_exp);

OSCAP_API SEXP_t *SEXP_build (const char *s_str, ...);

OSCAP_API size_t SEXP_sizeof (const SEXP_t *s_exp);

#if !defined(NDEBUG)
# define SEXP_VALIDATE(s) __SEXP_VALIDATE(s, __FILE__, __LINE__, __PRETTY_FUNCTION__)
# include <stdlib.h>

OSCAP_API void __SEXP_VALIDATE(const SEXP_t *s_exp, const char *file, uint32_t line, const char *func);

#else
# define SEXP_VALIDATE(s)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SEXP_MANIP_H */
/// @}
/// @}
/// @}
