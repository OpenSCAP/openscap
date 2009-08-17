#pragma once
#ifndef SEXP_TYPES_H
#define SEXP_TYPES_H

#include <stdint.h>
#include <seap-debug.h>

typedef uint8_t NUM_type_t;

#define NUM_NONE   0x00

#define NUM_INT8   0x01
#define NUM_CHAR   NUM_INT8

#define NUM_UINT8  0x02
#define NUM_UCHAR  NUM_UINT8

#define NUM_INT16    0x03
#define NUM_SHORTINT NUM_INT16

#define NUM_UINT16    0x04
#define NUM_USHORTINT NUM_UINT16
#define NUM_SHORTUINT NUM_UINT16

#define NUM_INT32   0x05
#define NUM_INT     NUM_INT32
#define NUM_LONGINT NUM_INT32

#define NUM_UINT32      0x06
#define NUM_UINT        NUM_UINT32
#define NUM_ULONGINT    NUM_UINT32
#define NUM_UNSIGNEDINT NUM_UINT32

#define NUM_INT64       0x07
#define NUM_LLINT       NUM_INT64
#define NUM_LONGLONGINT NUM_INT64

#define NUM_UINT64       0x08
#define NUM_ULONGLONGINT NUM_UINT64
#define NUM_LONGLONGUINT NUM_UINT64

#define NUM_DOUBLE 0x09

#define NUM_BOOL 0x0a
#define NUM_BOOLEAN NUM_BOOL

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

#define SEXP_TYPE_UNFIN  0x00
#define SEXP_TYPE_LIST   0x01
#define SEXP_TYPE_STRING 0x04
#define SEXP_TYPE_NUMBER 0x02
#define SEXP_TYPE_INVAL  0x0e
#define SEXP_TYPE_EMPTY  0x0f

typedef uint8_t SEXP_type_t;

#endif /* SEXP_TYPES_H */
