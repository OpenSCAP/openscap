#pragma once
#ifndef SEXP_TYPES_H
#define SEXP_TYPES_H

#include <stdint.h>
#include <seap-debug.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t SEXP_numtype_t;

/* Number types */
#define SEXP_NUM_NONE    0x00
#define SEXP_NUM_BOOL    0x01
#define SEXP_NUM_INT8    0x07
#define SEXP_NUM_UINT8   0x08
#define SEXP_NUM_INT16   0x0f
#define SEXP_NUM_UINT16  0x10
#define SEXP_NUM_INT32   0x1f
#define SEXP_NUM_UINT32  0x20
#define SEXP_NUM_INT64   0x3f
#define SEXP_NUM_UINT64  0x40
#define SEXP_NUM_DOUBLE  0x41

/* Aliases */

#define SEXP_NUM_BOOLEAN       SEXP_NUM_BOOL
#define SEXP_NUM_CHAR          SEXP_NUM_INT8
#define SEXP_NUM_UCHAR         SEXP_NUM_UINT8
#define SEXP_NUM_SHORTINT      SEXP_NUM_INT16
#define SEXP_NUM_USHORTINT     SEXP_NUM_UINT16
#define SEXP_NUM_SHORTUINT     SEXP_NUM_UINT16
#define SEXP_NUM_INT           SEXP_NUM_INT32
#define SEXP_NUM_LONGINT       SEXP_NUM_INT32
#define SEXP_NUM_UINT          SEXP_NUM_UINT32
#define SEXP_NUM_ULONGINT      SEXP_NUM_UINT32
#define SEXP_NUM_LONGUINT      SEXP_NUM_UINT32
#define SEXP_NUM_LONLONGINT    SEXP_NUM_INT64
#define SEXP_NUM_ULONGLONGINT  SEXP_NUM_UINT64
#define SEXP_NUM_LONGLONGUINT  SEXP_NUM_UINT64
#define SEXP_NUM_FLOAT         SEXP_NUM_DOUBLE

#if 0
# define NUM_FRACT  0x0a /* Not implemented */
# define NUM_BIGNUM 0x0b /* Not implemented */
#endif

typedef struct SEXP SEXP_t;

/* S-expression format */
typedef uint8_t SEXP_format_t;

#define SEXP_FMT_UNDEFINED  0
#define SEXP_FMT_TRANSPORT  1
#define SEXP_FMT_CANONICAL  2
#define SEXP_FMT_ADVANCED   3
#define SEXP_FMT_AUTODETECT 4

#include <assert.h>
#ifndef _A
# define _A(x) assert(x)
#endif

#define SEXP_TYPE_EMPTY  0
#define SEXP_TYPE_STRING 1
#define SEXP_TYPE_NUMBER 2
#define SEXP_TYPE_LIST   3

typedef uint8_t SEXP_type_t;

#ifdef __cplusplus
}
#endif

#endif /* SEXP_TYPES_H */
