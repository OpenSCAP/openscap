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

#ifndef _SEXP_LIST_H
#define _SEXP_LIST_H

/*
 * list
 */

/**
 * Create a new sexp list, optionally initialized with the provided sexp arguments.
 * The argument list needs to be terminated with NULL.
 * @param memb the first sexp object to be inserted into the new list. can be NULL.
 * @param ... arbitrary number of elements to be inserted
 */
SEXP_t *SEXP_list_new(SEXP_t *memb, ...);

/**
 * Free the specified sexp object.
 * @param s_exp the object to be freed
 */
void SEXP_list_free(SEXP_t *s_exp);

/**
 * Check whether the provided sexp object is a list.
 * @param s_exp the sexp object to be tested
 */
bool SEXP_listp(const SEXP_t *s_exp);

/**
 * Get the length of the sexp list.
 * @param s_sexp the queried sexp object
 */
size_t SEXP_list_length(const SEXP_t *s_exp);

/**
 * Get the first element of a list.
 * This function increments element's reference count
 * @param list the queried sexp object
 */
SEXP_t *SEXP_list_first(const SEXP_t *list);

/**
 * Get the rest of a list.
 * This function increments elements' reference count.
 * @param list the queried sexp object
 */
SEXP_t *SEXP_list_rest(const SEXP_t *list);

/**
 * Get the last element of a list.
 * This function increments element's reference count.
 * @param list the queried sexp object
 */
SEXP_t *SEXP_list_last(const SEXP_t *list);

/**
 * Get the n-th element of a list.
 * This function increments element's reference count.
 * @param list the queried sexp object
 * @param n the position of the element in the list
 */
SEXP_t *SEXP_list_nth(const SEXP_t *list, uint32_t n);

/**
 * Add an element to a list.
 * This function increments element's reference count.
 * @param list the modified sexp object
 * @param s_exp the element to be added
 */
SEXP_t *SEXP_list_add(SEXP_t *list, const SEXP_t *s_exp);

/**
 * Create a new list containing the concatenated contents of two lists.
 * This function increments element's reference count.
 * @param list_a the first list to be contatenated
 * @param list_b the list to be attached to the first one
 */
SEXP_t *SEXP_list_join(const SEXP_t *list_a, const SEXP_t *list_b);

/**
 * Push an element to the head of a list.
 * This function increments element's reference count.
 * @param list the modified sexp object
 * @param s_exp the element to be added
 */
SEXP_t *SEXP_list_push(SEXP_t *list, const SEXP_t *s_exp);

/**
 * Extract the first element of a list.
 * This function increments element's reference count.
 * @param list the modified sexp object
 */
SEXP_t *SEXP_list_pop(SEXP_t *list);

/**
 * Sort a list using `compare' as the comparison function.
 */
SEXP_t *SEXP_list_sort(SEXP_t *list, int(*compare)(const SEXP_t *, const SEXP_t *));

/**
 * Replace the n-th element of a list.
 * This function increments element's reference count.
 * @param list the modified sexp object
 * @param n the index of the element to be replaced
 * @param s_exp the element to be added
 * @return the replaced element
 */
SEXP_t *SEXP_list_replace(SEXP_t *list, uint32_t n, const SEXP_t *s_exp);

/**
 * Get the first element of a list.
 * This function creates a soft reference to the element.
 * @param list the queried sexp object
 */
SEXP_t *SEXP_listref_first(SEXP_t *list);

/**
 * Get the rest of a list.
 * This function creates a soft reference to the list.
 * @param list the queried sexp object
 */
SEXP_t *SEXP_listref_rest(SEXP_t *list);

/**
 * Get the last element of a list.
 * This function creates a soft reference to the element.
 * @param list the queried sexp object
 */
SEXP_t *SEXP_listref_last(SEXP_t *list);

/**
 * Get the n-th element of a list.
 * This function creates a soft reference to the element.
 * @param list the queried sexp object
 * @param n the position of the element in the list
 */
SEXP_t *SEXP_listref_nth(SEXP_t *list, uint32_t n);

typedef struct SEXP_it SEXP_it_t;

#define SEXP_IT_RECURSIVE 0x01
#define SEXP_IT_HARDREF   0x02

typedef struct SEXP_list_it SEXP_list_it;

SEXP_list_it *SEXP_list_it_new(const SEXP_t *list);
SEXP_t *SEXP_list_it_next(SEXP_list_it *it);
void SEXP_list_it_free(SEXP_list_it *it);

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

#endif /* _SEXP_LIST_H */
