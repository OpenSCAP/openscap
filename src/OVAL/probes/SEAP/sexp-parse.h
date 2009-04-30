#pragma once
#ifndef SEXP_PARSE_H
#define SEXP_PARSE_H

#include <stddef.h>
#include <stdint.h>
#include "common.h"
#include "sexp-types.h"
#include "sexp-manip.h"

#define LIST_STACK_GROW 1.2
#define LIST_STACK_INIT_SIZE 64

typedef struct {
        LIST_t **LIST_stack;
        uint16_t LIST_stack_cnt;
        uint16_t LIST_stack_size;
} LIST_stack_t;

static inline void LIST_stack_init (LIST_stack_t *stack)
{
        stack->LIST_stack  = xmalloc (sizeof (LIST_t *) * (LIST_STACK_INIT_SIZE));
        stack->LIST_stack_size = LIST_STACK_INIT_SIZE;
        stack->LIST_stack_cnt = 0;
}

static inline LIST_stack_t *LIST_stack_new (void)
{
        LIST_stack_t *new;

        new = xmalloc (sizeof (LIST_stack_t));
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
                stack->LIST_stack = xrealloc (stack->LIST_stack,
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

typedef uint8_t SEXP_pflags_t;

#define PF_EOFOK 0x01
#define PF_ALL   0xff

/* S-expression parsing state */
typedef struct {
        char    *buffer;
        size_t   buffer_data_len;
        size_t   buffer_fail_off;
        /*
          size_t   buffer_size;
          bool_t   buffer_free;
        */
        LIST_stack_t  lstack;
        SEXP_pflags_t pflags;
} SEXP_pstate_t;

typedef struct {
        SEXP_format_t fmt;
        SEXP_pflags_t pflags;
} SEXP_psetup_t;

SEXP_psetup_t *SEXP_psetup_new (void);
void SEXP_psetup_init (SEXP_psetup_t *psetup);

/* S-expression parse function pointer */
/* typedef SEXP_t * (*SEXP_parser_t) (SEAP_CTX_t *, const char *, size_t, void **); */

SEXP_t *SEXP_parse (SEXP_psetup_t *setup, const char *buf, size_t buflen, SEXP_pstate_t **pstate);

#define PARSER(name) SEXP_parser_##name
#define DEFPARSER(name) SEXP_t * PARSER(name) (SEXP_psetup_t *setup, const char *buf, size_t buflen, SEXP_pstate_t **pstatep)

DEFPARSER(label);

#endif /* SEXP_PARSE_H */
