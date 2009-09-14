#pragma once
#ifndef _SEXP_TYPES_H
#define _SEXP_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "public/sexp-types.h"
#include "_sexp-datatype.h"

/* Magic numbers */
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
# define SEXP_MAGIC0     0xf3f3
# define SEXP_MAGIC0_INV 0xffff
# define SEXP_MAGIC1     0x6767
# define SEXP_MAGIC1_INV 0x0000
#endif

struct SEXP {
#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        volatile uint16_t __magic0;
#endif

        SEXP_datatype_t *s_type;
        uintptr_t        s_valp;
        uint8_t          s_flgs;

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        volatile uint16_t __magic1;
#endif
};

#define SEXP_FLAG_SREF  0x01
#define SEXP_FLAG_INVAL 0x02
#define SEXP_FLAG_UNFIN 0x04

static inline void SEXP_flag_set (SEXP_t *s_exp, uint8_t flag)
{
        s_exp->s_flgs |= flag;
}

static inline void SEXP_flag_unset (SEXP_t *s_exp, uint8_t flag)
{
        s_exp->s_flgs &= ~flag;
}

static inline bool SEXP_flag_isset (SEXP_t *s_exp, uint8_t flag)
{
        return ((s_exp->s_flgs & flag) == flag);
}

#endif /* _SEXP_TYPES_H */
