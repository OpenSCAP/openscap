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

//#ifdef HAVE_CONFIG_H
#include <config.h>
//#endif

#include <stdint.h>
#include <string.h>

#include "_sexp-atomic.h"
#include "_sexp-value.h"
#include "public/sm_alloc.h"

int SEXP_val_new (SEXP_val_t *dst, size_t vmemsize, SEXP_type_t type)
{
        void *s_val;

        if (sm_memalign (&s_val, SEXP_VALP_ALIGN,
                         sizeof (SEXP_valhdr_t) + vmemsize) != 0)
        {
                return (-1);
        }

        SEXP_val_dsc (dst, (uintptr_t) s_val);

        dst->hdr->refs = 1;
        dst->hdr->size = vmemsize;
        dst->type      = type;
        dst->ptr       = SEXP_val_ptr (dst);
#if defined(SEAP_VERBOSE_DEBUG)
        dI(""
           "new value: hdr->refs = %u\n"
           "           hdr->size = %zu\n"
           "                type = %hhu\n"
           "                 ptr = %p\n",
           dst->hdr->refs, dst->hdr->size, dst->type, (void *)dst->ptr);
#endif
        return (0);
}

void SEXP_val_dsc (SEXP_val_t *dst, uintptr_t ptr)
{
        dst->ptr  = ptr;
        dst->hdr  = (SEXP_valhdr_t *)(ptr & SEXP_VALP_MASK);
        dst->mem  = (void *)(((uint8_t *)(dst->hdr)) + sizeof (SEXP_valhdr_t));
        dst->type = ptr & SEXP_VALT_MASK;
}

uintptr_t SEXP_val_ptr (SEXP_val_t *dsc)
{
        return ((dsc->ptr & SEXP_VALP_MASK) | (dsc->type & SEXP_VALT_MASK));
}

/*
 * Return values:
 *  (uintptr_t)NULL - empty value
 *  ...other values mean success.
 */
uintptr_t SEXP_rawval_incref (uintptr_t valp)
{
        return SEXP_atomic_inc_u32 (&(SEXP_VALP_HDR(valp)->refs)) > 0 ? valp : (uintptr_t) NULL;
}

/*
 * Return values:
 *  0 - refs  > 0
 *  1 - refs == 0
 */
int SEXP_rawval_decref (uintptr_t valp)
{
        return (SEXP_atomic_dec_u32 (&(SEXP_VALP_HDR(valp)->refs)) == 0);
}

SEXP_numtype_t SEXP_rawval_number_type (SEXP_val_t *dsc)
{
        return SEXP_NTYPEP(dsc->hdr->size, dsc->mem);
}

size_t SEXP_rawval_list_length (struct SEXP_val_list *list)
{
        size_t length;
        register struct SEXP_val_lblk *lblk;

        length = 0;
        lblk   = SEXP_VALP_LBLK(list->b_addr);

        while (lblk != NULL) {
                length += lblk->real;
                lblk    = SEXP_VALP_LBLK(lblk->nxsz);
        }

        return (length - list->offset);
}

uintptr_t SEXP_rawval_lblk_new (uint8_t sz)
{
        struct SEXP_val_lblk *lblk;

        _A(sz < 16);

        if (sm_memalign ((void **)(void *)&lblk, SEXP_LBLK_ALIGN,
                         sizeof (uintptr_t) + (2 * sizeof (uint16_t)) + (sizeof (SEXP_t) * (1 << sz))) != 0) {
                /* TODO: handle this */
                abort ();
                return ((uintptr_t) NULL);
        }

        lblk->nxsz = ((uintptr_t)(NULL) & SEXP_LBLKP_MASK) | ((uintptr_t)sz & SEXP_LBLKS_MASK);
        lblk->refs = 1;
        lblk->real = 0;

        return ((uintptr_t)lblk);
}

uintptr_t SEXP_rawval_lblk_incref (uintptr_t lblkp)
{
        struct SEXP_val_lblk *lblk;
        uint16_t refs;

        lblk = SEXP_VALP_LBLK(lblkp);

        /*
         * Atomicaly update the reference counter.
         * If the reference counter can't be updated
         * (it would overflow), create a copy of the
         * list.
         */
        for (;;) {
                refs = lblk->refs;

                if (refs < UINT16_MAX) {
                        if (SEXP_atomic_cas_u16 (&lblk->refs, refs, refs + 1))
                                break;
                } else
                        return SEXP_rawval_lblk_copy (lblkp, 0);
        }

        return (lblkp);
}

int SEXP_rawval_lblk_decref (uintptr_t lblkp)
{
        return (SEXP_atomic_dec_u16 (&SEXP_VALP_LBLK(lblkp)->refs) == 0);
}

uintptr_t SEXP_rawval_lblk_fill (uintptr_t lblkp, SEXP_t *s_exp[], uint16_t s_exp_count)
{
        struct SEXP_val_lblk *lblk;

        lblk = SEXP_VALP_LBLK(lblkp);

        if (s_exp_count > (1 << (lblk->nxsz & 0xf)) - lblk->real)
                return ((uintptr_t) NULL);

        lblk->real = s_exp_count;

        for (; s_exp_count > 0; --s_exp_count) {
                lblk->memb[s_exp_count - 1].s_valp = SEXP_rawval_incref (s_exp[s_exp_count - 1]->s_valp);
                lblk->memb[s_exp_count - 1].s_type = s_exp[s_exp_count - 1]->s_type;
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
                lblk->memb[s_exp_count - 1].__magic0 = s_exp[s_exp_count - 1]->__magic0;
                lblk->memb[s_exp_count - 1].__magic1 = s_exp[s_exp_count - 1]->__magic1;
#endif
        }

        return (lblkp);
}

uintptr_t SEXP_rawval_lblk_add (uintptr_t lblkp, const SEXP_t *s_exp)
{
        uintptr_t lb_prev;
        uintptr_t lb_head;
        register struct SEXP_val_lblk *lblk;

        lblk = SEXP_VALP_LBLK(lblkp);

        if (lblk == NULL) {
                lb_head = SEXP_rawval_lblk_new (1);
                lb_prev = lb_head;
        } else {
                lb_head = lblkp;
                lb_prev = 0;

                do {
                        if (lblk->refs < 2) {
                                lb_prev = (uintptr_t)lblk;
                                lblk    = SEXP_VALP_LBLK(lblk->nxsz);
                        } else {
                                uintptr_t lb_ptr;

                                /*
                                 * We've encountered a block which belongs to more
                                 * than one list so we have to create a copy of the
                                 * rest of the list.
                                 */
                                lb_ptr = SEXP_rawval_lblk_copy (lblkp, 0);

                                if (lb_prev == 0)
                                        lb_head = lb_ptr;

                                /*
                                 * Update pointer in the previous block (if there is one)
                                 * to point at the copy. Also decrement the reference
                                 * counter in current block.
                                 */
                                if (lb_prev != 0)
                                        SEXP_VALP_LBLK(lb_prev)->nxsz = (lb_ptr & SEXP_LBLKP_MASK) | (SEXP_VALP_LBLK(lb_prev)->nxsz & SEXP_LBLKS_MASK);

                                SEXP_rawval_lblk_decref (lblkp);

                                /*
                                 * Get the last block without checking refs
                                 * (we don't need to because it's our copy)
                                 */
                                lb_prev = SEXP_rawval_lblk_last (lb_ptr);
                                break;
                        }
                } while (lblk != NULL);
        }

        _A(lb_prev != 0);
        _A(lb_head != 0);

        (void)SEXP_rawval_lblk_add1 (lb_prev, s_exp);

        return (lb_head);
}

uintptr_t SEXP_rawval_lblk_add1 (uintptr_t lblkp, const SEXP_t *s_exp)
{
        struct SEXP_val_lblk *lblk = SEXP_VALP_LBLK(lblkp);

        if (lblk->real < (1 << (lblk->nxsz & 0xf))) {

                lblk->memb[lblk->real].s_valp = SEXP_rawval_incref (s_exp->s_valp);
                lblk->memb[lblk->real].s_type = s_exp->s_type;
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
                lblk->memb[lblk->real].__magic0 = s_exp->__magic0;
                lblk->memb[lblk->real].__magic1 = s_exp->__magic1;
#endif
                ++lblk->real;

                return (lblkp);
        } else {
                uint8_t   new_sz;
                uintptr_t new_lb;

		new_sz = lblk->nxsz & SEXP_LBLKS_MASK;
		new_sz = new_sz == 15 ? 6 : new_sz + 1;

                new_lb     = SEXP_rawval_lblk_new (new_sz);
                lblk->nxsz = (new_lb & SEXP_LBLKP_MASK) | (lblk->nxsz & SEXP_LBLKS_MASK);

                /*
                 * We don't need to check the return value
                 * here because we are adding the S-exp to
                 * a newly allocated block and there is at
                 * least one free slot.
                 */
                SEXP_rawval_lblk_add1 (new_lb, s_exp);

                return (lblkp);
        }

        /* NOTREACHED */
        return ((uintptr_t) NULL);
}

uintptr_t SEXP_rawval_lblk_last (uintptr_t lblkp)
{
        register struct SEXP_val_lblk *lblk;
        register uintptr_t last;

        last = lblkp;
        lblk = SEXP_VALP_LBLK(last);

        while ((lblk->nxsz & SEXP_LBLKP_MASK) != ((uintptr_t)NULL)) {
                last = lblk->nxsz;
                lblk = SEXP_VALP_LBLK(last);
        }

        return (last);
}

SEXP_t *SEXP_rawval_lblk_nth (uintptr_t lblkp, uint32_t n)
{
        register struct SEXP_val_lblk *lblk;

        lblk = SEXP_VALP_LBLK(lblkp);

        while (lblk != NULL) {
                if (lblk->real >= n) {
                        return (lblk->memb + (n - 1));
                } else {
                        n   -= lblk->real;
                        lblk = SEXP_VALP_LBLK(lblk->nxsz);
                }
        }

        return (NULL);
}

uintptr_t SEXP_rawval_lblk_replace (uintptr_t lblkp, uint32_t n, const SEXP_t *n_val, SEXP_t **o_val)
{
        uintptr_t lb_prev;
        uintptr_t lb_head;
        struct SEXP_val_lblk *lblk;
        SEXP_t   *memb;

        lblk = SEXP_VALP_LBLK(lblkp);

        _A(lblk != NULL);

        lb_head = lblkp;
        lb_prev = 0;

        while (n > lblk->real) {
                if (lblk->refs < 2) {
                        n      -= lblk->real;
                        lb_prev = (uintptr_t)lblk;
                        lblk    = SEXP_VALP_LBLK(lblk->nxsz);

                        if (lblk == NULL) {
                                (*o_val) = NULL;
                                return (lb_head);
                        }
                } else {
                        uintptr_t lb_ptr;

                        /*
                         * We've encountered a block which belongs to more
                         * than one list so we have to create a copy of the
                         * rest of the list.
                         */
                        lb_ptr = SEXP_rawval_lblk_copy ((uintptr_t)lblk, 0);

                        if (lb_prev == 0)
                                lb_head = lb_ptr;

                        /*
                         * Update pointer in the previous block (if there is one)
                         * to point at the copy. Also decrement the reference
                         * counter in current block.
                         */
                        if (lb_prev != 0)
                                SEXP_VALP_LBLK(lb_prev)->nxsz = (lb_ptr & SEXP_LBLKP_MASK) | (SEXP_VALP_LBLK(lb_prev)->nxsz & SEXP_LBLKS_MASK);

                        SEXP_rawval_lblk_decref ((uintptr_t)lblk);
                        memb = SEXP_rawval_lblk_nth (lb_ptr, n);

			if (memb == NULL)
				abort();

                        goto replace;
                }
        }

        _A(n > 0);

        memb = lblk->memb + (n - 1);
replace:
        _A(lb_head != 0);
        _A(memb != NULL);

        (*o_val) = SEXP_new ();
        (*o_val)->s_valp = memb->s_valp;
        (*o_val)->s_type = memb->s_type;
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        (*o_val)->__magic0 = memb->__magic0;
        (*o_val)->__magic1 = memb->__magic1;
#endif

        memb->s_valp = SEXP_rawval_incref (n_val->s_valp);
        memb->s_type = n_val->s_type;
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        memb->__magic0 = n_val->__magic0;
        memb->__magic1 = n_val->__magic1;
#endif
        return (lb_head);
}

int SEXP_rawval_lblk_cb (uintptr_t lblkp, int (*func) (SEXP_t *, void *), void *arg, uint32_t n)
{
        register struct SEXP_val_lblk *lblk;
        register uint16_t bi;

        lblk = SEXP_VALP_LBLK(lblkp);

        while (lblk != NULL) {
                if (lblk->real >= n) {
                        int ret;

                        bi = (uint16_t)n - 1;

                        do {
                                while (bi < lblk->real) {
                                        ret = func (lblk->memb + bi, arg);

                                        if (ret != 0)
                                                return (ret);
                                        else
                                                ++bi;
                                }

                                lblk = SEXP_VALP_LBLK(lblk->nxsz);
                                bi   = 0;
                        } while (lblk != NULL);

                        break;
                } else {
                        n   -= lblk->real;
                        lblk = SEXP_VALP_LBLK(lblk->nxsz);
                }
        }

        return (0);
}

uintptr_t SEXP_rawval_list_copy (uintptr_t s_valp)
{
        SEXP_val_t v_dsc_o, v_dsc_c;

        if (SEXP_val_new (&v_dsc_c, sizeof (void *) + sizeof (uint16_t),
                          SEXP_VALTYPE_LIST) != 0)
        {
                /* TODO: handle this */
                return ((uintptr_t)NULL);
        }

        SEXP_val_dsc (&v_dsc_o, s_valp);

        SEXP_LCASTP(v_dsc_c.mem)->b_addr = (void *) SEXP_rawval_lblk_copy ((uintptr_t)SEXP_LCASTP(v_dsc_o.mem)->b_addr,
                                                                           (uintptr_t)SEXP_LCASTP(v_dsc_o.mem)->offset);
        SEXP_LCASTP(v_dsc_c.mem)->offset = 0;

        return (SEXP_val_ptr (&v_dsc_c));
}

uintptr_t SEXP_rawval_lblk_copy (uintptr_t lblkp, uint16_t n_skip)
{
        struct SEXP_val_lblk *lb_new, *lb_old;
        uintptr_t lb_next;
        uintptr_t lb_head;
        uint16_t  off_n;  /* offset in the new block */
        uint16_t  off_o;  /* offset in the old block */
        uint8_t  cur_sz;  /* size of the new block */

        lb_head = 0;
        off_n   = 0;
        off_o   = n_skip;
        lb_old  = SEXP_VALP_LBLK(lblkp);

        if (lb_old == NULL)
                return ((uintptr_t) NULL);

        cur_sz  = 0;
        lb_new  = (struct SEXP_val_lblk *)SEXP_rawval_lblk_new (cur_sz);
        lb_head = (uintptr_t)lb_new;

        while (lb_old != NULL) {
                if ((lb_old->real - off_o) == 0) {
                        /*
                         * move to the next old block
                         */
                        lb_old = SEXP_VALP_LBLK(lb_old->nxsz);

                        if (lb_old == NULL)
                                break;

                        off_o  = 0;
                }

                /*
                 * allocate new block
                 */
                if (lb_new->real >= (1 << (cur_sz))) {
                        lb_next = SEXP_rawval_lblk_new (++cur_sz);
                        lb_new->nxsz = (lb_next & SEXP_LBLKP_MASK) | (lb_new->nxsz & SEXP_LBLKS_MASK);
                        lb_new  = SEXP_VALP_LBLK(lb_next);
                        off_n   = 0;
                }

                /*
                 * copy list items
                 */
                while (off_n < (1 << cur_sz) && off_o < lb_old->real) {
                        lb_new->memb[off_n].s_valp = SEXP_rawval_incref (lb_old->memb[off_o].s_valp);
                        lb_new->memb[off_n].s_type = lb_old->memb[off_o].s_type;
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
                        lb_new->memb[off_n].__magic0 = lb_old->memb[off_o].__magic0;
                        lb_new->memb[off_n].__magic1 = lb_old->memb[off_o].__magic1;
#endif

                        ++off_n;
                        ++off_o;
                        ++lb_new->real;
                }
        }

        return (lb_head);
}

void SEXP_rawval_lblk_free (uintptr_t lblkp, void (*func) (SEXP_t *))
{
        if (SEXP_rawval_lblk_decref (lblkp)) {
                struct SEXP_val_lblk *lblk, *next;

                lblk = SEXP_VALP_LBLK(lblkp);
                next = SEXP_VALP_LBLK(lblk->nxsz);

                while (lblk->real > 0) {
                        --lblk->real;
                        func (lblk->memb + lblk->real);
                }

                sm_free (lblk);

                if (next != NULL)
                        SEXP_rawval_lblk_free ((uintptr_t)next, func);
        }

        return;
}

void SEXP_rawval_lblk_free1 (uintptr_t lblkp, void (*func) (SEXP_t *))
{
        if (SEXP_rawval_lblk_decref (lblkp)) {
                struct SEXP_val_lblk *lblk;

                lblk = SEXP_VALP_LBLK(lblkp);

                while (lblk->real > 0) {
                        --lblk->real;
                        func (lblk->memb + lblk->real);
                }

                sm_free (lblk);
        }

        return;
}

uintptr_t SEXP_rawval_copy(uintptr_t s_valp)
{
	uintptr_t uptr;
	SEXP_val_t v_dsc;
	SEXP_val_dsc(&v_dsc, s_valp);

	if (v_dsc.type != SEXP_VALTYPE_LIST) {
		SEXP_val_t v_dsc_copy;
		if (SEXP_val_new(&v_dsc_copy, v_dsc.hdr->size, v_dsc.type) != 0) {
			return ((uintptr_t)NULL);
		}
		memcpy(v_dsc_copy.mem, v_dsc.mem, v_dsc.hdr->size);
		uptr = SEXP_val_ptr(&v_dsc_copy);
	} else {
		uptr = SEXP_rawval_list_copy(s_valp);
	}
	return uptr;
}
