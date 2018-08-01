/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef SEXP_OUTPUT_H
#define SEXP_OUTPUT_H

#include <stdio.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <sexp-types.h>
#include <strbuf.h>
#include "oscap_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SEXP_ostate SEXP_ostate_t;

OSCAP_API size_t SEXP_fprintfa (FILE *fp, const SEXP_t *s_exp);

OSCAP_API int SEXP_sbprintf_t (SEXP_t *s_exp, strbuf_t *sb);

#ifdef __cplusplus
}
#endif

#endif /* SEXP_OUTPUT_H */
