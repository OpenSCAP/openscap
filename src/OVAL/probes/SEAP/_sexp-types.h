#pragma once
#ifndef _SEXP_TYPES_H
#define _SEXP_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "public/sexp-types.h"

/*
 * Atomic type: number
 */

struct NUM {
        NUM_type_t type;
        void      *nptr;
};

typedef struct NUM NUM_t;

/*
 * Atomic type: string
 */

struct STR {
        size_t len;
        char  *str;
};

typedef struct STR STR_t;

/*
 * Atomic type: list
 */

struct LIST {
        void    *memb;
        uint32_t count;
        uint32_t size;
};

typedef struct LIST LIST_t;

/*
 * S-exp flags
 *  There is currently only one flag:
 *   SEXP_FLAGFREE - If set, then the memory
 *                   holding the S-exp object
 *                   can be passed to free().
 */
typedef uint8_t ATOM_flags_t;

#define SEXP_TYPEMASK 0x7f
#define SEXP_FLAGMASK 0x80
#define SEXP_FLAGFREE 0x80

/* atomic types */
typedef uint8_t  ATOM_type_t;

/* keep in sync with SEXP_TYPE_* ! */
#define ATOM_UNFIN    0x00
#define ATOM_LIST     0x01
#define ATOM_NUMBER   0x02
#define ATOM_STRING   0x04
#define ATOM_INVAL    0x0e
#define ATOM_EMPTY    0x0f

/* Magic numbers */
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
# define SEXP_MAGIC0     0xf3f3
# define SEXP_MAGIC0_INV 0xffff
# define SEXP_MAGIC1     0x6767
# define SEXP_MAGIC1_INV 0x0000
#endif

#include "_sexp-datatype.h"

/* S-exp object */
struct SEXP {
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        volatile uint16_t __magic0;
#endif

        ATOM_flags_t     flags;
        SEXP_datatype_t *handler;
        
        union {
                struct LIST list;
                struct STR  string;
                struct NUM  number;
        } atom;

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        volatile uint16_t __magic1;
#endif
};

#define SEXP(ptr)      ((SEXP_t *)(ptr))
#define SEXP_TYPEOF(ptr) ((SEXP(ptr)->flags) & SEXP_TYPEMASK)
#define SEXP_TYPE(ptr) SEXP_TYPEOF(ptr)

/*
 *  SEXP_FREE returns the value of SEXP_FLAGFREE bit.
 */
#define SEXP_FREE(ptr) (((SEXP(ptr)->flags) & SEXP_FLAGFREE) == SEXP_FLAGFREE)

static inline void SEXP_SETTYPE(SEXP_t *sexp, ATOM_type_t type)
{
        sexp->flags = (sexp->flags & SEXP_FLAGMASK) | (type & SEXP_TYPEMASK);
        return;
}

static inline void SEXP_SETFLAG(SEXP_t *sexp, ATOM_flags_t flag)
{
        sexp->flags |= flag & SEXP_FLAGMASK;
        return;
}

#endif /* _SEXP_TYPES_H */
