#pragma once
#ifndef SEXP_TYPES_H
#define SEXP_TYPES_H

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
#define NUM_INT32  0x05
#define NUM_UINT32 0x06
#define NUM_INT64  0x07
#define NUM_UINT64 0x08
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
        ATOM_flags_t    flags;
        void           *handler;
        union {
                LIST_t list;
                STR_t  string;
                NUM_t  number;
        } atom;
} SEXP_t;

/* S-expression format */
typedef uint8_t SEXP_format_t;

#define FMT_UNDEFINED  0
#define FMT_TRANSPORT  1
#define FMT_CANONICAL  2
#define FMT_ADVANCED   3
#define FMT_AUTODETECT 4

#endif /* SEXP_TYPES_H */
