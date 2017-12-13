/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */

#ifndef SEXP_MANIP_R_H
#define SEXP_MANIP_R_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "sexp-types.h"
#include "oscap_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#if     __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define _GNUC_PRINTF( format_idx, arg_idx )    \
  __attribute__((__format__ (__printf__, format_idx, arg_idx)))
#else   /* !__GNUC__ */
#define _GNUC_PRINTF( format_idx, arg_idx )
#endif  /* __GNUC__ */

OSCAP_API SEXP_t *SEXP_init(SEXP_t *sexp_mem);

OSCAP_API SEXP_t *SEXP_number_newb_r(SEXP_t *sexp_mem, bool n);
#define SEXP_number_newi_r SEXP_number_newi_32_r
OSCAP_API SEXP_t *SEXP_number_newi_32_r(SEXP_t *sexp_mem, int32_t n);
OSCAP_API SEXP_t *SEXP_number_newu_32_r(SEXP_t *sexp_mem, uint32_t n);
OSCAP_API SEXP_t *SEXP_number_newu_64_r(SEXP_t *sexp_mem, uint64_t n);
OSCAP_API SEXP_t *SEXP_number_newi_64_r(SEXP_t *sexp_mem, int64_t n);
OSCAP_API SEXP_t *SEXP_number_newf_r(SEXP_t *sexp_mem, double n);

OSCAP_API SEXP_t *SEXP_string_new_r(SEXP_t *sexp_mem, const void *string, size_t length);
OSCAP_API SEXP_t *SEXP_string_newf_r(SEXP_t *sexp_mem, const char *format, ...) _GNUC_PRINTF (2,3);
OSCAP_API SEXP_t *SEXP_string_newf_rv(SEXP_t *sexp_mem, const char *format, va_list ap);

OSCAP_API SEXP_t *SEXP_list_new_rv(SEXP_t *sexp_mem, SEXP_t *memb, va_list alist);
OSCAP_API SEXP_t *SEXP_list_new_r(SEXP_t *sexp_mem, SEXP_t *memb, ...);

OSCAP_API SEXP_t *SEXP_list_rest_r (SEXP_t *rest, const SEXP_t *list);

OSCAP_API int SEXP_unref_r(SEXP_t *s_exp);

#if defined(NDEBUG)
OSCAP_API void SEXP_free_r (SEXP_t *s_exp);
#else
#include <stdint.h>
OSCAP_API void __SEXP_free_r(SEXP_t *s_exp, const char *file, uint32_t line, const char *func);

__attribute__ ((unused)) static void SEXP_free_r(SEXP_t *sexp)
{
OSCAP_API         __SEXP_free_r(sexp, __FILE__, __LINE__, __PRETTY_FUNCTION__);
}

#define SEXP_free_r(ptr) __SEXP_free_r(ptr, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#endif

#ifdef __cplusplus
}
#endif

#endif /* SEXP_MANIP_R_H */
