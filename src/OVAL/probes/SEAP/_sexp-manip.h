#pragma once
#ifndef _SEXP_MANIP_H
#define _SEXP_MANIP_H

#include "generic/common.h"
#include "_sexp-types.h"
#include "public/sexp-manip.h"
#include "public/sm_alloc.h"

/* 
 * NUM_STORE(type, nsrc, voidp)
 *  stores the number at nsrc to voidp directly
 *  if sizeof(void *) is less than or equal to
 *  sizeof(type). Otherwise sizeof(type) bytes
 *  of memory is allocated and the number is
 *  stored there. The pointer to this memory
 *  block is stored in voidp.
 */

#define NUM_STORE(type, nsrc, voidp) do {                       \
                if (sizeof (type) <= sizeof (void *)) {         \
                        *((type *)(&(voidp))) = (type)(nsrc);   \
                } else {                                        \
                        (voidp) = sm_talloc (type);             \
                        *((type *)(voidp)) = (type)(nsrc);      \
                }                                               \
        } while (0)

/*
 * NUM(type, voidp)
 *  returns the number of type type stored
 *  at or in voidp.
 */
#define NUM(type, voidp) (sizeof (type) <= VOIDPTR_SIZE ? (*((type *)(&(voidp)))) : *((type *)(voidp)))

SEXP_t *LIST_add (LIST_t *list, SEXP_t *sexp);

#define LIST_GROW_LOW_TRESH 5
#define LIST_GROW_LOW 3
#define LIST_GROW 1.2
#define LIST_GROW_ADD 16
#define LIST_INIT_SIZE 1

static inline LIST_t *LIST_init (LIST_t *list)
{
        _A(list != NULL);

        list->memb  = sm_alloc (sizeof (SEXP_t) * LIST_INIT_SIZE);
        list->count = 0;
        list->size  = LIST_INIT_SIZE;
        
        return (list);
}

static inline LIST_t *LIST_new (void)
{
        LIST_t *list;

        list = sm_talloc (LIST_t);
        LIST_init (list);
        
        return (list);
}

#define LIST_STACK_GROW 1.2
#define LIST_STACK_INIT_SIZE 64

typedef struct {
        LIST_t **LIST_stack;
        uint16_t LIST_stack_cnt;
        uint16_t LIST_stack_size;
} LIST_stack_t;

static inline void LIST_stack_init (LIST_stack_t *stack)
{
        stack->LIST_stack  = sm_alloc (sizeof (LIST_t *) * (LIST_STACK_INIT_SIZE));
        stack->LIST_stack_size = LIST_STACK_INIT_SIZE;
        stack->LIST_stack_cnt = 0;
}

static inline LIST_stack_t *LIST_stack_new (void)
{
        LIST_stack_t *new;

        new = sm_talloc (LIST_stack_t);
        LIST_stack_init (new);
        return (new);
}

static inline LIST_t *LIST_stack_push (LIST_stack_t *stack, LIST_t *list)
{
        _A(stack != NULL);
        _A(list   != NULL);
        _A(stack->LIST_stack_size >= stack->LIST_stack_cnt);
        
        if (stack->LIST_stack_size == stack->LIST_stack_cnt) {
                /* Resize the stack */
                _D("LIST_stack_push(%p,%p): Resizing stack from %u (%zu bytes) to %u (%zu bytes).\n",
                   stack, list,
                   stack->LIST_stack_size,
                   sizeof (LIST_t *) * stack->LIST_stack_size,
                   (size_t)(stack->LIST_stack_size * (LIST_STACK_GROW)),
                   (size_t)(sizeof (LIST_t *) * (stack->LIST_stack_size * (LIST_STACK_GROW))));
                
                stack->LIST_stack_size *= LIST_STACK_GROW;
                stack->LIST_stack = sm_realloc (stack->LIST_stack,
                                                sizeof (LIST_t *) * stack->LIST_stack_size);
        }
        
        stack->LIST_stack[stack->LIST_stack_cnt++] = list;
        _D("LIST_stack_push(%p,%p): stack_size=%zu.\n", stack, list, stack->LIST_stack_cnt);
        
        return (list);
}

static inline LIST_t *LIST_stack_top (LIST_stack_t *stack)
{
        _A(stack != NULL);
        _A(stack->LIST_stack_cnt > 0);
        return (stack->LIST_stack[stack->LIST_stack_cnt - 1]);
}

static inline LIST_t *LIST_stack_bottom (LIST_stack_t *stack)
{
        _A(stack != NULL);
        _A(stack->LIST_stack_cnt > 0);
        return (stack->LIST_stack[0]);
}

static inline void LIST_stack_dec (LIST_stack_t *stack)
{
        _A(stack != NULL);
        _A(stack->LIST_stack_cnt > 0);
        --(stack->LIST_stack_cnt);
        return;
}

static inline void LIST_stack_ins (LIST_stack_t *stack, LIST_t *sexp)
{
        _A(stack != NULL);
        _A(stack->LIST_stack_cnt > 0);
        stack->LIST_stack[stack->LIST_stack_cnt - 1] = sexp;
        return;
}

static inline uint16_t LIST_stack_cnt (LIST_stack_t *stack)
{
        _A(stack != NULL);
        return (stack->LIST_stack_cnt);
}

#define VOIDPTR_SIZE (sizeof (void *))

#endif /* _SEAP_MANIP_H */
