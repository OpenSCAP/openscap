#include <stdint.h>
#include "_sexp-value.h"
#include "public/sm_alloc.h"

int SEXP_val_new (SEXP_val_t *dst, size_t vmemsize, SEXP_type_t type)
{
        SEXP_val_t v_dsc;
        void      *s_val;

        sm_memalign (&s_val, SEXP_VALP_ALIGN,
                     sizeof (SEXP_valhdr_t) + vmemsize);
        
        SEXP_val_dsc (&v_dsc, (uintptr_t) s_val);
        
        v_dsc.hdr->refs = 1;
        v_dsc.hdr->size = vmemsize;
        v_dsc.type      = type;
        v_dsc.ptr       = SEXP_val_ptr (&v_dsc);
        
        return (0);
}

void SEXP_val_dsc (SEXP_val_t *dst, uintptr_t ptr)
{
        dst->ptr  = ptr;
        dst->hdr  = (SEXP_valhdr_t *)(ptr & SEXP_VALP_MASK);
        dst->mem  = (void *)((uint8_t *)(dst->hdr) + sizeof (SEXP_valhdr_t));
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
        uint32_t refs;
        size_t   size;

#if defined(HAVE_ATOMIC_FUNCTIONS)        
        refs = __sync_fetch_and_add (&(SEXP_VALP_HDR(valp)->refs), 1);
        size = __sync_fetch_and_add (&(SEXP_VALP_HDR(valp)->size), 0);
#else
        # warning "Atomic functions not available. " # __FUNCTION__ # " will return copies of values."
        /* TODO: copy the value here */
#endif
        return (refs > 0 && size > 0 ? valp : (uintptr_t) NULL);
}

/*
 * Return values:
 *  0 - refs  > 0
 *  1 - refs == 0
 */
int SEXP_rawval_decref (uintptr_t valp)
{
#if defined(HAVE_ATOMIC_FUNCTIONS)
        return (__sync_sub_and_fetch (&(SEXP_VALP_HDR(valp)->refs), 1) == 0);
#else
        # warning "Atomic function not available. " # __FUNCTION__ # " will always signal zero refs."
        return (1);
#endif
}

size_t SEXP_rawval_list_length (struct SEXP_val_list *list)
{
        size_t length;
        struct SEXP_val_lblk *lblk;

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
        
        if (sm_memalign ((void **)(void *)&lblk, SEXP_LBLK_ALIGN, 2 << sz) != 0) {
                /* TODO: handle this */
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
        
#if defined(HAVE_ATOMIC_FUNCTIONS)
        /*
         * Atomicaly update the reference counter.
         * If the reference counter can't be updated
         * (it would overflow), create a copy of the
         * list.
         */
        for (;;) {
                refs = lblk->refs;
                
                if (refs < UINT16_MAX) {
                        if (__sync_bool_compare_and_swap (&lblk->refs, refs, refs + 1))
                                break;
                } else
                        return SEXP_rawval_list_copy (lblkp, 0);
        }
        
        return (lblkp);
#else
        # warning "Atomic functions not available. " # __FUNCTION__ # " will return copies of values."
        return SEXP_rawval_list_copy (lblkp, 0);
#endif
}

int SEXP_rawval_lblk_decref (uintptr_t lblkp)
{
#if defined(HAVE_ATOMIC_FUNCTIONS)
        return (__sync_sub_and_fetch (&SEXP_VALP_LBLK(lblkp)->refs, 1) == 0);
#else
# warning "Atomic function not available. " # __FUNCTION__ # " will always signal zero refs."
        return (1);
#endif
}

uintptr_t SEXP_rawval_lblk_fill (uintptr_t lblkp, SEXP_t *s_exp[], uint16_t s_exp_count)
{
        struct SEXP_val_lblk *lblk;

        lblk = SEXP_VALP_LBLK(lblkp);
        
        if (s_exp_count > (2 << (lblk->nxsz & 0xf)) - lblk->real)
                return ((uintptr_t) NULL);
        
        lblk->real = s_exp_count;
        
        for (; s_exp_count > 0; --s_exp_count) {
                lblk->memb[s_exp_count - 1].s_valp = SEXP_rawval_incref (s_exp[s_exp_count - 1]->s_valp);
                lblk->memb[s_exp_count - 1].s_type = s_exp[s_exp_count - 1]->s_type;
        }
        
        return (lblkp);
}

uintptr_t SEXP_rawval_lblk_add (uintptr_t lblkp, SEXP_t *s_exp)
{
        uintptr_t lb_prev;
        uintptr_t lb_head;
        struct SEXP_val_lblk *lblk;

        lb_head = lblkp;
        lblk    = SEXP_VALP_LBLK(lb_head);
        lb_prev = 0;
        
        while (lblk != NULL) {
                if (lblk->refs < 2) {
                        lb_prev = lblkp;
                        lblk    = SEXP_VALP_LBLK(lblk->nxsz);
                } else {
                        uintptr_t lb_ptr;
                        
                        /*
                         * We've encountered a block which belongs to more
                         * than one list so we have to create a copy of the
                         * rest of the list.
                         */
                        lb_ptr = SEXP_rawval_list_copy (lblkp, 0);
                        
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
        }
        
        _A(lb_prev != 0);

        (void)SEXP_rawval_lblk_add1 (lb_prev, s_exp);
        
        return (lb_head);
}

uintptr_t SEXP_rawval_lblk_add1 (uintptr_t lblkp, SEXP_t *s_exp)
{
        struct SEXP_val_lblk *lblk = SEXP_VALP_LBLK(lblkp);
       
        if (lblk->real < (2 << (lblk->nxsz & 0xf))) {
                lblk->memb[lblk->real].s_valp = SEXP_rawval_incref (s_exp->s_valp);
                lblk->memb[lblk->real].s_type = s_exp->s_type;
                
                return (lblkp);
        } else {
                uint8_t new_sz = ((lblk->nxsz & 0xf) + 1) % 16;
                
                lblkp = SEXP_rawval_lblk_new (new_sz);
                lblk->nxsz = (lblkp & SEXP_LBLKP_MASK) | (lblk->nxsz & 0xf);
                
                return (SEXP_rawval_lblk_add (lblkp, s_exp));
        }
        
        /* NOTREACHED */
        return ((uintptr_t) NULL);
}

uintptr_t SEXP_rawval_lblk_last (uintptr_t lblkp)
{
        struct SEXP_val_lblk *lblk;
        uintptr_t last;

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
        struct SEXP_val_lblk *lblk;
        
        lblk = SEXP_VALP_LBLK(lblkp);

        while (lblk != NULL) {
                if (lblk->real <= n) {
                        return (lblk->memb + (n - 1));
                } else {
                        n   -= lblk->real; 
                        lblk = SEXP_VALP_LBLK(lblk->nxsz);
                }
        }
        
        return (NULL);
}

uintptr_t SEXP_rawval_list_copy (uintptr_t lblkp, uint16_t n_skip)
{
        struct SEXP_val_lblk *lb_new, *lb_old;
        uintptr_t lb_next;
        uintptr_t lb_head;
        uint16_t  off_n;  /* offset in the new block */
        uint16_t  off_o;  /* offset in the old block */
        uint8_t  cur_sz;  /* size of the new block */
        uint8_t  old_sz;  /* size of the old block */
        
        lb_head = 0;
        off_n   = 0;
        off_o   = n_skip;
        lb_old  = SEXP_VALP_LBLK(lblkp);
        old_sz  = lb_old->nxsz & SEXP_LBLKS_MASK;
        cur_sz  = 0;
        lb_new  = (struct SEXP_val_lblk *)SEXP_rawval_lblk_new (cur_sz);
        lb_head = (uintptr_t)lb_new;
        
        while (lb_old != NULL) {
                if ((lb_old->real - off_o) == 0) {
                        /*
                         * move to the next old block
                         */
                        lb_old = SEXP_VALP_LBLK(lb_old->nxsz);
                        off_o  = 0;
                }
                
                /*
                 * allocate new block
                 */
                if (lb_new->real >= (2 << (cur_sz))) {
                        lb_next = SEXP_rawval_lblk_new (++cur_sz);
                        lb_new->nxsz = (lb_next & SEXP_LBLKP_MASK) | (lb_new->nxsz & SEXP_LBLKS_MASK);
                        lb_new  = SEXP_VALP_LBLK(lb_next);
                        off_n   = 0;
                }
                
                /*
                 * copy list items
                 */
                while (off_n < (2 << cur_sz) && off_o < (2 << old_sz)) {
                        lb_new->memb[off_n].s_valp = SEXP_rawval_incref (lb_old->memb[off_o].s_valp);
                        lb_new->memb[off_n].s_type = lb_old->memb[off_o].s_type;
                        
                        ++off_n;
                        ++off_o;
                        ++lb_new->real;
                }
        }
        
        return (lb_head);
}
