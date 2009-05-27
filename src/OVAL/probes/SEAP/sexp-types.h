#pragma once
#ifndef SEXP_TYPES_H
#define SEXP_TYPES_H

#include <stddef.h>
#include <stdint.h>

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

#define ATOM_UNFIN    0x00
#define ATOM_LIST     0x01
#define ATOM_NUMBER   0x02
#define ATOM_STRING   0x04
#define ATOM_INVAL    0x0e
#define ATOM_EMPTY    0x0f

/*
 *  Atomic type: number
 */

/* Number types */
typedef uint8_t NUM_type_t;

#define NUM_NONE   0x00
#define NUM_INT8   0x01
#define NUM_UINT8  0x02

#define NUM_INT16  0x03
#define NUM_UINT16 0x04
#define NUM_SHORTINT  NUM_INT16
#define NUM_USHORTINT NUM_UINT16
#define NUM_SHORTUINT NUM_UINT16

#define NUM_INT32  0x05
#define NUM_UINT32 0x06
#define NUM_INT         NUM_INT32
#define NUM_LONGINT     NUM_INT32
#define NUM_UINT        NUM_UINT32
#define NUM_ULONGINT    NUM_UINT32
#define NUM_UNSIGNEDINT NUM_UINT32

#define NUM_INT64  0x07
#define NUM_UINT64 0x08
#define NUM_LLINT        NUM_INT64
#define NUM_LONGLONGINT  NUM_INT64
#define NUM_ULONGLONGINT NUM_UINT64
#define NUM_LONGLONGUINT NUM_UINT64

#define NUM_DOUBLE 0x09
#define NUM_FRACT  0x0a /* Not implemented */
#define NUM_BIGNUM 0x0b /* Not implemented */

/* Parsing states */
#define NUMTYPE_INV 0
#define NUMTYPE_INT 1
#define NUMTYPE_FLT 2
#define NUMTYPE_FRA 3
#define NUMTYPE_EXP 4

typedef struct {
        NUM_type_t type;
        void      *nptr;
} NUM_t;

/*
 * Atomic type: string
 */

typedef struct {
        size_t len;
        char  *str;
} STR_t;

/*
 * Atomic type: list
 */

typedef struct {
        void    *memb;
        uint32_t count; /* number of items in list  */
        uint32_t size;  /* size of allocated memory */
} LIST_t;

/*
 * S-exp object
 */ 

typedef struct __SEXP_t {

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
#define SEXP_MAGIC0     0xf3f3
#define SEXP_MAGIC0_INV 0xffff
        volatile uint16_t __magic0;
#endif

        ATOM_flags_t    flags;
        void           *handler;
        union {
                LIST_t list;
                STR_t  string;
                NUM_t  number;
        } atom;

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
#define SEXP_MAGIC1     0x6767
#define SEXP_MAGIC1_INV 0x0000
        volatile uint16_t __magic1;
#endif

} SEXP_t;

#define SEXP(ptr)      ((SEXP_t *)(ptr))
#define SEXP_TYPE(ptr) ((SEXP(ptr)->flags) & SEXP_TYPEMASK)

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

#define VOIDPTR_SIZE (sizeof (void *))

#if !defined(NDEBUG) || defined(VALIDADE_SEXP)
# include <stdio.h>
# include <stdlib.h>

#if !defined(__VALIDATE_TRESH_LIST_SIZE)
# define __VALIDATE_TRESH_LIST_SIZE  65535
#endif

#if !defined(__VALIDATE_TRESH_LIST_COUNT)
# define __VALIDATE_TRESH_LIST_COUNT 65535
#endif

#if !defined(__VALIDATE_TRESH_STRING_LEN)
# define __VALIDATE_TRESH_STRING_LEN 65535
#endif

static inline void __SEXP_VALIDATE(SEXP_t *ptr, const char *loc) {
        if (ptr == NULL) {
                fprintf (stderr, "%s: !!! NULL S-EXP OBJECT !!!\n", loc);
        } else {
                if ((ptr->__magic0 == SEXP_MAGIC0) &&
                    (ptr->__magic1 == SEXP_MAGIC1)) {
                        switch (SEXP_TYPE(ptr))
                        {
                        case ATOM_UNFIN:
                        case ATOM_INVAL:
                        case ATOM_EMPTY:
                                return;
                        case ATOM_LIST:
                                if (ptr->atom.list.size > __VALIDATE_TRESH_LIST_SIZE) {
                                        fprintf (stderr, "%s !!! LIST SIZE TRESHOLD EXCEEDED !!!\n", loc);
                                        break;
                                }
                                if (ptr->atom.list.count > __VALIDATE_TRESH_LIST_COUNT) {
                                        fprintf (stderr, "%s !!! LIST COUNT TRESHOLD EXCEEDED !!!\n", loc);
                                        break;
                                }
                                return;
                        case ATOM_NUMBER:
                                switch (ptr->atom.number.type)
                                {
                                case NUM_NONE:
                                case NUM_INT8:
                                case NUM_UINT8:
                                case NUM_INT16:
                                case NUM_UINT16:
                                case NUM_INT32:
                                case NUM_UINT32:
                                case NUM_INT64:
                                case NUM_UINT64:
                                case NUM_DOUBLE:
                                case NUM_FRACT:
                                case NUM_BIGNUM:
                                        return;
                                }
                                fprintf (stderr, "%s !!! INVALID NUMBER TYPE !!!\n", loc);
                                break;
                        case ATOM_STRING:
                                if (ptr->atom.string.len <= __VALIDATE_TRESH_STRING_LEN)
                                        return;
                                fprintf (stderr, "%s !!! STRING LENGTH TRESHOLD EXCEEDED !!!\n", loc);
                                break;
                        default:
                                fprintf (stderr, "%s !!! INVALID S-EXP OBJECT TYPE !!!\n", loc);
                        }
                }
                fprintf (stderr, "%s: !!! CORRUPTED S-EXP OBJECT !!!\n", loc);
        }
        abort ();
}

# define SEXP_VALIDATE(ptr) __SEXP_VALIDATE(ptr, __PRETTY_FUNCTION__)
#else
# define SEXP_VALIDATE(ptr) while(0)
#endif

/* S-expression format */
typedef uint8_t SEXP_format_t;

#define FMT_UNDEFINED  0
#define FMT_TRANSPORT  1
#define FMT_CANONICAL  2
#define FMT_ADVANCED   3
#define FMT_AUTODETECT 4

#endif /* SEXP_TYPES_H */
