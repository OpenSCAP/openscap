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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef _SEXP_MANIP_H
#define _SEXP_MANIP_H

#include "_sexp-core.h"
#include "../../../common/util.h"
#include "_sexp-types.h"


#define SEXP_LSTACK_INIT_SIZE      8
#define SEXP_LSTACK_GROWFAST_TRESH 64
#define SEXP_LSTACK_GROWSLOW_DIFF  8

typedef struct {
        SEXP_t   *p_list;
        size_t    l_size;
        size_t    l_real;
        SEXP_t  **l_sref; /* stack of soft refs to lists */
} SEXP_lstack_t;

SEXP_lstack_t *SEXP_lstack_new (void);
void SEXP_lstack_init (SEXP_lstack_t *stack);
void SEXP_lstack_free (SEXP_lstack_t *stack);
void SEXP_lstack_destroy (SEXP_lstack_t *stack);

SEXP_t *SEXP_lstack_push (SEXP_lstack_t *stack, SEXP_t *ref);
SEXP_t *SEXP_lstack_pop (SEXP_lstack_t *stack);
SEXP_t *SEXP_lstack_top (const SEXP_lstack_t *stack);
SEXP_t *SEXP_lstack_list (SEXP_lstack_t *stack);
size_t  SEXP_lstack_depth (SEXP_lstack_t *stack);

/*
 * number
 */

/**
 * Create a new sexp object from a value and a number type
 * @param t the desired number type
 * @param n pointer to the number value
 */
SEXP_t *SEXP_number_new(SEXP_numtype_t t, const void *n) __attribute__ ((nonnull (2)));

/**
 * Create a new sexp object from a boolean value.
 * @param n the boolean value to store
 */
SEXP_t *SEXP_number_newb(bool n);

/**
 * Create a new sexp object from an integer.
 * @param n the integer value to store
 */
SEXP_t *SEXP_number_newi_8(int8_t n);

/**
 * Create a new sexp object from an unsigned integer.
 * @param n the unsigned integer value to store
 */
SEXP_t *SEXP_number_newu_8(uint8_t n);

/**
 * Get unsigned integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
uint8_t SEXP_number_getu_8(const SEXP_t *s_exp);

/**
 * Create a new sexp object from an integer.
 * @param n the integer value to store
 */
SEXP_t *SEXP_number_newi_16(int16_t n);

/**
 * Create a new sexp object from an unsigned integer.
 * @param n the unsigned integer value to store
 */
SEXP_t *SEXP_number_newu_16(uint16_t n);

/**
 * Create a new sexp object from an integer.
 * The size of the integer may be architecture dependent.
 */
#define SEXP_number_newi SEXP_number_newi_32

/**
 * Create a new sexp object from an integer.
 * @param n the integer value to store
 */
SEXP_t *SEXP_number_newi_32(int32_t n);

/**
 * Get integer value from a sexp object.
 * The size of the integer may be architecture dependent.
 */
#define SEXP_number_geti SEXP_number_geti_32

/**
 * Get integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
int32_t SEXP_number_geti_32(const SEXP_t *s_exp);

/**
 * Get boolean value from a sexp object.
 * @param s_exp the queried sexp object
 */
bool SEXP_number_getb(const SEXP_t *s_exp);

/**
 * Create a new sexp object from an unsigned integer.
 * The size of the integer may be architecture dependent.
 */
#define SEXP_number_newu SEXP_number_newu_32

/**
 * Create a new sexp object from an unsigned integer.
 * @param n the integer value to store
 */
SEXP_t *SEXP_number_newu_32(uint32_t n);

/**
 * Get unsigned integer value from a sexp object.
 * The size of the integer may be architecture dependent.
 */
#define  SEXP_number_getu SEXP_number_getu_32

/**
 * Get unsigned integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
uint32_t SEXP_number_getu_32(const SEXP_t *s_exp);

/**
 * Create a new sexp object from an integer.
 * @param n the integer value to store
 */
SEXP_t *SEXP_number_newi_64(int64_t n);

/**
 * Get integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
int64_t SEXP_number_geti_64(const SEXP_t *s_exp);

/**
 * Create a new sexp object from an unsigned integer.
 * @param n the integer value to store
 */
SEXP_t *SEXP_number_newu_64(uint64_t n);

/**
 * Get unsigned integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
uint64_t SEXP_number_getu_64(const SEXP_t *s_exp);

/**
 * Create a new sexp object from an floating point value.
 * @param n the floating point value to store
 */
SEXP_t *SEXP_number_newf(double n);

/**
 * Get floating point value from a sexp object.
 * @param s_exp the queried sexp object
 */
double SEXP_number_getf(const SEXP_t *s_exp);

/**
 * Get a value from a sexp object according to a specified type
 * @param s_exp the queried sexp object
 * @param dst buffer for the value
 * @param type the desired number type
 */
int SEXP_number_get(const SEXP_t *s_exp, void *dst, SEXP_numtype_t type);

/**
 * Get unsigned integer value from a sexp object.
 * @param s_exp the queried sexp object
 */
uint16_t SEXP_number_getu_16(const SEXP_t *s_exp);

/**
 * Free the specified sexp object.
 * @param s_exp the object to be freed
 */
void SEXP_number_free(SEXP_t *s_exp);

/**
 * Check whether the provided sexp object is a number.
 * @param s_exp the sexp object to be tested
 */
bool SEXP_numberp(const SEXP_t *s_exp);

/**
 * Get the number type of an object.
 * @param sexp the queried sexp object
 */
SEXP_numtype_t SEXP_number_type(const SEXP_t *sexp);

/*
 * string
 */

/**
 * Create a new sexp object from a string.
 * @param string the string to be stored
 * @param strlen the length of the string in bytes
 */
SEXP_t *SEXP_string_new(const void *string, size_t strlen) __attribute__ ((nonnull (1)));

/**
 * Create a new sexp object from a format string.
 * @param format the format of the new string
 * @param ... arguments for the format
 */
SEXP_t *SEXP_string_newf(const char *format, ...) __attribute__ ((format (printf, 1, 2), nonnull (1)));

/**
 * Free the specified sexp object.
 * @param s_exp the object to be freed
 */
void SEXP_string_free(SEXP_t *s_exp);

/**
 * Check whether the provided sexp object is a string.
 * @param s_exp the sexp object to be tested
 */
bool SEXP_stringp(const SEXP_t *s_exp);

/**
 * Get the length of a string in a sexp object.
 * @param s_exp the queried sexp object
 */
size_t SEXP_string_length(const SEXP_t *s_exp);

/**
 * Compare a string in a sexp object with a C string.
 * @param s_exp the sexp object to be compared
 * @param str the C string to be compared
 */
int SEXP_strcmp(const SEXP_t *s_exp, const char *str) __attribute__ ((nonnull (2)));

/**
 * Compare a string in a sexp object with a C string.
 * @param s_exp the sexp object to be compared
 * @param str the C string to be compared
 * @param n compare at most n bytes
 */
int SEXP_strncmp(const SEXP_t *s_exp, const char *str, size_t n) __attribute__ ((nonnull (2)));

/**
 * Get the n-th byte of a string.
 * @param s_exp the sexp object holding the string
 * @param n the index of the desired character
 */
int SEXP_string_nth(const SEXP_t *s_exp, size_t n);

/**
 * Get a C string from a sexp object.
 */
char *SEXP_string_cstr(const SEXP_t *s_exp);

/**
 * Get a C string from a sexp object.
 * The name is stored in the provided buffer.
 * @param s_exp the queried object
 * @param buf the buffer to store the name in
 * @param len the length of the buffer
 */
size_t SEXP_string_cstr_r(const SEXP_t *s_exp, char *buf, size_t len) __attribute__ ((nonnull (2)));

/**
 * Obsolete function.
 */
char *SEXP_string_cstrp(const SEXP_t *s_exp);

/**
 * Get a C substring from a sexp object.
 * @param s_sexp the queried sexp object
 * @param beg the position of the fisrt character of the substring
 * @param len the length of the substring
 */
char *SEXP_string_subcstr(const SEXP_t *s_exp, size_t beg, size_t len);

/**
 * Compare two sexp strings.
 * @param str_a the first string to compare
 * @param str_b the second string to compare
 */
int SEXP_string_cmp(const SEXP_t *str_a, const SEXP_t *str_b);

/**
 * Try to cast the supplied sexp string to a boolean.
 * @param s_ext the string to be cast
 */
bool SEXP_string_getb(const SEXP_t *s_exp);

/*
 * generic
 */
bool SEXP_emptyp(SEXP_t *sexp);

/**
 * Create a new reference to a sexp object.
 * @param s_exp the object of which to increment the reference count
 */
SEXP_t *SEXP_ref(const SEXP_t *s_exp);

SEXP_t *SEXP_unref(SEXP_t *s_exp_o);

/**
 * Create a new soft reference to a sexp object.
 * @param s_exp the object to which create the soft reference
 */
SEXP_t *SEXP_softref(SEXP_t *s_exp);

/**
 * Check whether an S-exp reference is a "soft" reference
 * @param s_exp the S-exp reference
 */
bool SEXP_softrefp(const SEXP_t *s_exp);

/**
 * Return the value of the reference counter
 * @param ref
 */
uint32_t SEXP_refs(const SEXP_t *ref);

bool SEXP_eq(const SEXP_t *a, const SEXP_t *b);

/**
 * Compare reference pointers.
 */
int SEXP_refcmp(const SEXP_t *a, const SEXP_t *b);

bool SEXP_deepcmp(const SEXP_t *a, const SEXP_t *b);

void SEXP_free_lmemb(SEXP_t *s_exp);

/**
 * Get the user data type of a sexp object.
 * @param s_exp the object to be queried
 */
const char *SEXP_datatype(const SEXP_t *s_exp);

/**
 * Set the user data type of a sexp object.
 * @param s_exp the object to be modified
 */
int SEXP_datatype_set(SEXP_t *s_exp, const char *name) __attribute__ ((nonnull (2)));

/**
 * Set the user data type of the nth sexp object in a list.
 * @param list list containing the object to be modified
 * @param n the position of the object
 * @param name name of the user data type
 */
int SEXP_datatype_set_nth(SEXP_t *list, uint32_t n, const char *name) __attribute__ ((nonnull (3)));

/**
 * Get the type of a sexp object.
 * @param s_exp the object to be queried
 */
SEXP_type_t SEXP_typeof(const SEXP_t *s_exp);

/**
 * Get a text description of the sexp object's type.
 * @param s_exp the object to be queried
 */
const char *SEXP_strtype(const SEXP_t *s_exp);

SEXP_t *SEXP_build(const char *s_str, ...);

size_t SEXP_sizeof(const SEXP_t *s_exp);

#if !defined(NDEBUG)
# define SEXP_VALIDATE(s) __SEXP_VALIDATE(s, __FILE__, __LINE__, __PRETTY_FUNCTION__)
# include <stdlib.h>

void __SEXP_VALIDATE(const SEXP_t *s_exp, const char *file, uint32_t line, const char *func);

#else
# define SEXP_VALIDATE(s)
#endif

#endif /* _SEXP_MANIP_H */
