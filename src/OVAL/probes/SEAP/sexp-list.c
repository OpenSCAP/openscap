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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>

#include "_sexp-core.h"
#include "common/bfind.h"
#include "_sexp-types.h"
#include "_sexp-value.h"
#include "_sexp-manip.h"
#include "_sexp-rawptr.h"
#include "_sexp-manip_r.h"
#include "_sexp-datatype.h"
#include "debug_priv.h"
#include "_sexp-list.h"
#include "_sexp-list_r.h"

/*
 * Lists
 */

SEXP_t *SEXP_list_new (SEXP_t *memb, ...)
{
        SEXP_t *list;
        va_list ap;

        va_start(ap, memb);
        list = SEXP_new ();
        list = SEXP_list_new_rv(list, memb, ap);
        va_end(ap);

        return (list);
}

void SEXP_list_free (SEXP_t *s_exp)
{
        SEXP_VALIDATE(s_exp);
        SEXP_free (s_exp);
}

bool SEXP_listp (const SEXP_t *s_exp)
{
        if (s_exp == NULL) {
                errno = EFAULT;
                return (false);
        }

        SEXP_VALIDATE(s_exp);

        return ((s_exp->s_valp & SEXP_VALT_MASK) == SEXP_VALTYPE_LIST);
}

size_t SEXP_list_length (const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;

        if (s_exp == NULL) {
                errno = EFAULT;
                return (0);
        }

        SEXP_VALIDATE(s_exp);

        SEXP_val_dsc (&v_dsc, s_exp->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return ((size_t)(-1));
        }

        return (SEXP_rawval_list_length ((struct SEXP_val_list *)v_dsc.mem));
}

SEXP_t *SEXP_list_first (const SEXP_t *list)
{
        SEXP_val_t v_dsc;
        SEXP_t    *s_exp;

        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        s_exp = SEXP_rawval_lblk_nth ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                      SEXP_LCASTP(v_dsc.mem)->offset + 1);

        return (s_exp == NULL ? NULL : SEXP_ref (s_exp));
}

SEXP_t *SEXP_listref_first (SEXP_t *list)
{
        SEXP_val_t v_dsc;
        SEXP_t    *s_exp;


        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        /* FIXME: check refs */

        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        s_exp = SEXP_rawval_lblk_nth ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                      SEXP_LCASTP(v_dsc.mem)->offset + 1);

        return (s_exp == NULL ? NULL : SEXP_softref (s_exp));
}

SEXP_t *SEXP_list_rest  (const SEXP_t *list)
{
	SEXP_t *sexp;

	sexp = SEXP_new();
	sexp = SEXP_list_rest_r(sexp, list);

	return (sexp);
}

SEXP_t *SEXP_listref_rest (SEXP_t *list)
{
        SEXP_VALIDATE(list);
        return (NULL);
}

SEXP_t *SEXP_list_last (const SEXP_t *list)
{
        SEXP_val_t v_dsc;
        struct SEXP_val_lblk *l_blk;

        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        l_blk = SEXP_VALP_LBLK(SEXP_rawval_lblk_last ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr));

        if (l_blk == NULL)
                return (NULL);

        return (SEXP_ref (l_blk->memb + (l_blk->real - 1)));
}

SEXP_t *SEXP_list_replace (SEXP_t *list, uint32_t n, const SEXP_t *n_val)
{
        SEXP_val_t v_dsc;
        SEXP_t    *o_val;

        if (list == NULL || n_val == NULL || n < 1) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        if (v_dsc.hdr->refs > 1) {
		uintptr_t uptr = SEXP_rawval_list_copy (list->s_valp);

		if (SEXP_rawval_decref (list->s_valp)) {
			/* TODO: handle this */
			abort();
		}

		list->s_valp = uptr;
		SEXP_val_dsc (&v_dsc, list->s_valp);
        }

        _A(n > 0);

        SEXP_LCASTP(v_dsc.mem)->b_addr = (void *) SEXP_rawval_lblk_replace ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                                                            SEXP_LCASTP(v_dsc.mem)->offset + n,
                                                                            n_val, &o_val);

        return (o_val);
}

SEXP_t *SEXP_listref_last (SEXP_t *list)
{
        SEXP_VALIDATE(list);
        return (NULL);
}

SEXP_t *SEXP_list_nth (const SEXP_t *list, uint32_t n)
{
        SEXP_val_t v_dsc;
        SEXP_t    *s_exp;

        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST || n < 1) {
                errno = EINVAL;
                return (NULL);
        }

        s_exp = SEXP_rawval_lblk_nth ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                      SEXP_LCASTP(v_dsc.mem)->offset + n);

#if !defined(NDEBUG)
        if (s_exp != NULL)
                SEXP_VALIDATE(s_exp);
#endif
        return (s_exp == NULL ? NULL : SEXP_ref (s_exp));
}

SEXP_t *SEXP_listref_nth (SEXP_t *list, uint32_t n)
{
        SEXP_val_t v_dsc;
        SEXP_t    *s_exp;

        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        /* FIXME: check refs */
        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST || n < 1) {
                errno = EINVAL;
                return (NULL);
        }

        s_exp = SEXP_rawval_lblk_nth ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr,
                                      SEXP_LCASTP(v_dsc.mem)->offset + n);

#if !defined(NDEBUG)
        if (s_exp != NULL)
                SEXP_VALIDATE(s_exp);
#endif
        return (s_exp == NULL ? NULL : SEXP_softref (s_exp));
}

SEXP_t *SEXP_list_add (SEXP_t *list, const SEXP_t *s_exp)
{
        SEXP_val_t v_dsc;

        if (list == NULL || s_exp == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        _A(list->s_valp != 0);
        SEXP_VALIDATE(list);
        SEXP_VALIDATE(s_exp);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        if (v_dsc.hdr->refs > 1) {
                /*
                 * Create a private copy of the value and
                 * decrement the reference counter in the
                 * original value.
                 */
                uintptr_t uptr = SEXP_rawval_list_copy (list->s_valp);

                if (SEXP_rawval_decref (list->s_valp)) {
                        /* TODO: handle this */
                        abort ();
                }

                list->s_valp = uptr;
                SEXP_val_dsc (&v_dsc, list->s_valp);

                uptr = SEXP_rawval_lblk_last ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr);
                SEXP_rawval_lblk_add1 (uptr, s_exp);
        } else {
                /*
                 * Only one reference exists to the value.
                 * However, list blocks have their own
                 * reference counter and some blocks can
                 * be shared. This case is handled by the
                 * function SEXP_rawval_list_add.
                 */
                SEXP_LCASTP(v_dsc.mem)->b_addr = (void *)SEXP_rawval_lblk_add ((uintptr_t)SEXP_LCASTP(v_dsc.mem)->b_addr, s_exp);
        }

        return (list);
}

SEXP_t *SEXP_list_join (const SEXP_t *list_a, const SEXP_t *list_b)
{
        SEXP_t *list_j, *memb;

        if (list_a == NULL || list_b == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_VALIDATE(list_a);
        SEXP_VALIDATE(list_b);

        if (!SEXP_listp (list_a) || !SEXP_listp (list_b)) {
                errno = EINVAL;
                return (NULL);
        }

        list_j = SEXP_new ();
        list_j->s_valp = SEXP_rawval_list_copy (list_a->s_valp);

        SEXP_list_foreach (memb, list_b)
                SEXP_list_add (list_j, memb);

        return (list_j);
}

SEXP_t *SEXP_list_push (SEXP_t *list, const SEXP_t *s_exp)
{
        SEXP_VALIDATE(list);
        SEXP_VALIDATE(s_exp);
        errno = EOPNOTSUPP;
        return (NULL);
}

SEXP_t *SEXP_list_pop (SEXP_t *list)
{
        SEXP_t    *s_ref;
        SEXP_val_t v_dsc;
        struct SEXP_val_lblk *lblk;

        if (list == NULL) {
                errno = EINVAL;
                return (NULL);
        }

        SEXP_VALIDATE(list);

        SEXP_val_dsc (&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        s_ref = SEXP_list_first (list);

        if (v_dsc.hdr->refs > 1) {
                abort ();
        }

        lblk = SEXP_VALP_LBLK(SEXP_LCASTP(v_dsc.mem)->b_addr);

        if (lblk != NULL) {
                if (++SEXP_LCASTP(v_dsc.mem)->offset == lblk->real) {
                        SEXP_LCASTP(v_dsc.mem)->offset = 0;
                        SEXP_LCASTP(v_dsc.mem)->b_addr = SEXP_VALP_LBLK(lblk->nxsz);
                }

                SEXP_rawval_lblk_free1 ((uintptr_t)lblk, SEXP_free_lmemb);
        }

#if !defined(NDEBUG)
        if (s_ref != NULL)
                SEXP_VALIDATE(s_ref);
#endif
        return (s_ref);
}

#define SEXP_LISTIT_ARRAY_INC  32
#define SEXP_LISTIT_ARRAY_INIT 32

struct SEXP_list_it{
        struct SEXP_val_lblk *block;
        uint16_t index;
        uint16_t count;
};

SEXP_list_it *SEXP_list_it_new(const SEXP_t *list)
{
        SEXP_val_t v_dsc;

        if (list == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_val_dsc(&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

	SEXP_list_it *it = malloc(sizeof(SEXP_list_it));
        it->block = SEXP_LCASTP(v_dsc.mem)->b_addr;
        it->index = SEXP_LCASTP(v_dsc.mem)->offset;
        it->count = it->block != NULL ? it->block->real : 0;

        return (it);
}

SEXP_t *SEXP_list_it_next(SEXP_list_it *it)
{
        SEXP_t *item;

        if (it->block == NULL)
                return (NULL);

        item = it->block->memb + it->index;

        ++it->index;

        if (it->index == it->count) {
                it->block = SEXP_VALP_LBLK(it->block->nxsz);
                it->index = 0;
                it->count = it->block != NULL ? it->block->real : 0;
        }

        return (item);
}

void SEXP_list_it_free(SEXP_list_it *it)
{
	free(it);
}

SEXP_t *SEXP_list_sort(SEXP_t *list, int(*compare)(const SEXP_t *, const SEXP_t *))
{
        SEXP_val_t v_dsc;
        SEXP_list_it *list_it;
        size_t list_it_count, list_it_alloc;

        if (list == NULL || compare == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        SEXP_val_dsc(&v_dsc, list->s_valp);

        if (v_dsc.type != SEXP_VALTYPE_LIST) {
                errno = EINVAL;
                return (NULL);
        }

        /*
         * TODO: check reference counts and make copies of list
         * blocks if needed
         */

        /*
         * PASS #1: Sort each block and build the iterator array
         */
        list_it_count = 1;
        list_it_alloc = SEXP_LISTIT_ARRAY_INIT;
	list_it = malloc(sizeof(SEXP_list_it) * list_it_alloc);

        list_it[0].block = SEXP_LCASTP(v_dsc.mem)->b_addr;

        dD("Sorting blocks & building iterator array");

        while (list_it[list_it_count - 1].block != NULL) {
                /* initialize the rest of the iterator */
                list_it[list_it_count - 1].index = list_it_count == 1 ? SEXP_LCASTP(v_dsc.mem)->offset : 0;
                list_it[list_it_count - 1].count = list_it[list_it_count - 1].block->real;

                /* sort */
                qsort(list_it[list_it_count - 1].block->memb,
                      list_it[list_it_count - 1].count, sizeof(SEXP_t),
                      (int(*)(const void *, const void *))compare);

                /* reallocate the iterator array if needed */
                if (list_it_count == list_it_alloc) {
                        dD("Reallocating iterator array: %z -> %z",
                           list_it_alloc, list_it_alloc + SEXP_LISTIT_ARRAY_INC);

                        list_it_alloc += SEXP_LISTIT_ARRAY_INC;
			list_it = realloc(list_it, sizeof(SEXP_list_it) * list_it_alloc);
                }

                /* skip to the next block */
                list_it[list_it_count].block = SEXP_VALP_LBLK(list_it[list_it_count - 1].block->nxsz);
                ++list_it_count;
        }

        --list_it_count;
        dD("Iterator count = %zu", list_it_count);

        if (list_it_count > 0) {
                /*
                 * PASS #2: Mergesort all blocks
                 */
                for (register size_t i = 0; i < list_it_count - 1; ++i) {
                        while(list_it[i].index < list_it[i].count) {
                                SEXP_t *min_v, tmp_v, *first_v;
                                size_t  min_i = i;

                                /* search for minimal s_valp value */
                                min_v = first_v = list_it[i].block->memb + list_it[i].index;

                                for (register size_t j = i + 1; j < list_it_count; ++j) {
                                        if (compare(list_it[j].block->memb + list_it[j].index, min_v) < 0) {
                                                min_v = list_it[j].block->memb + list_it[j].index;
                                                min_i = j;
                                        }
                                }

                                if (min_v != first_v) {
                                        size_t dst_i = 0;

                                        /* save the old value & move the minval to it's place */
                                        tmp_v  = list_it[i].block->memb[list_it[i].index];
                                        list_it[i].block->memb[list_it[i].index] = *min_v;

					if (list_it[min_i].count > 1) {
						/*
						 * find a new place for the old values in the min.
						 * value source block
						 */
						(void)oscap_bfind_i(list_it[min_i].block->memb + 1,
								    list_it[min_i].count - 1, sizeof(SEXP_t), (void *)&tmp_v,
								    (int(*)(void *, void *))compare, &dst_i);

						dD("dst_i = %zu", dst_i);

						/* make place for the old value in the min. value source block */
						memmove(list_it[min_i].block->memb, list_it[min_i].block->memb + 1,
							sizeof(SEXP_t) * dst_i);
					}

                                        list_it[min_i].block->memb[dst_i] = tmp_v;
                                }

                                ++list_it[i].index;
                        }
                }
        }
        /*
         * Cleanup
         */
	free(list_it);

        return (list);
}
