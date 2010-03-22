/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
#include <unistd.h>
#include <seap-debug.h>
#include <sexp-types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SEXP_ostate SEXP_ostate_t;

size_t SEXP_fprintfa (FILE *fp, const SEXP_t *s_exp);

#if defined(STRBUF_H)
int SEXP_sbprintf_t (SEXP_t *s_exp, strbuf_t *sb);
#endif

#if 0
/* Generic */
int SEXP_printf (SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_fprintf (FILE *fp, SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_dprintf (int fd, SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_sprintf (char *str, SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_snprintf (char *str, size_t size, SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_asprintf (char **ret, SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_asnprintf (char **ret, size_t maxsz, SEXP_format_t fmt, SEXP_t *sexp);

ssize_t SEXP_st_dprintf (int fd, SEXP_format_t fmt, SEXP_t *sexp, SEXP_ostate_t **ost);
ssize_t SEXP_st_dnprintf (int fd, size_t maxsz, SEXP_format_t fmt, SEXP_t *sexp, SEXP_ostate_t **ost);

/* Canonical */
ssize_t SEXP_st_dprintc (int fd, SEXP_t *sexp, SEXP_ostate_t **ost);
ssize_t SEXP_st_dnprintc (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost);

#define SEXP_printf_canonical SEXP_printfc
#define SEXP_fprintf_canonical SEXP_fprintfc
#define SEXP_dprintf_canonical SEXP_dprintfc
#define SEXP_sprintf_canonical SEXP_sprintfc
#define SEXP_snprintf_canonical SEXP_snprintfc
#define SEXP_asprintf_canonical SEXP_asprintfc
#define SEXP_asnprintf_canonical SEXP_asnprintfc

int SEXP_printfc (SEXP_t *sexp);
int SEXP_fprintfc (FILE *fp, SEXP_t *sexp);
int SEXP_dprintfc (int fd, SEXP_t *sexp);
int SEXP_sprintfc (char *str, SEXP_t *sexp);
int SEXP_snprintfc (char *str, size_t size, SEXP_t *sexp);
int SEXP_asprintfc (char **ret, SEXP_t *sexp);
int SEXP_asnprintfc (char **ret, size_t maxsz, SEXP_t *sexp);

/* Advanced */
ssize_t SEXP_st_dprinta (int fd, SEXP_t *sexp, SEXP_ostate_t **ost);
ssize_t SEXP_st_dnprinta (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost);

#define SEXP_printf_advanced SEXP_printfa
#define SEXP_fprintf_advanced SEXP_fprintfa
#define SEXP_dprintf_advanced SEXP_dprintfa
#define SEXP_sprintf_advanced SEXP_sprintfa
#define SEXP_snprintf_advanced SEXP_snprintfa
#define SEXP_asprintf_advanced SEXP_asprintfa
#define SEXP_asnprintf_advanced SEXP_asnprintfa

int SEXP_printfa (SEXP_t *sexp);
int SEXP_fprintfa (FILE *fp, SEXP_t *sexp);
int SEXP_dprintfa (int fd, SEXP_t *sexp);
int SEXP_sprintfa (char *str, SEXP_t *sexp);
int SEXP_snprintfa (char *str, size_t size, SEXP_t *sexp);
int SEXP_asprintfa (char **ret, SEXP_t *sexp);
int SEXP_asnprintfa (char **ret, size_t maxsz, SEXP_t *sexp);

/* Transport */
ssize_t SEXP_st_dprintt (int fd, SEXP_t *sexp, SEXP_ostate_t **ost);
ssize_t SEXP_st_dnprintt (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost);

#define SEXP_printf_transport SEXP_printft
#define SEXP_fprintf_transport SEXP_fprintft
#define SEXP_dprintf_transport SEXP_dprintft
#define SEXP_sprintf_transport SEXP_sprintft
#define SEXP_snprintf_transport SEXP_snprintft
#define SEXP_asprintf_transport SEXP_asprintft
#define SEXP_asnprintf_transport SEXP_asnprintft

int SEXP_printft (SEXP_t *sexp);
int SEXP_fprintft (FILE *fp, SEXP_t *sexp);
int SEXP_dprintft (int fd, SEXP_t *sexp);
int SEXP_sprintft (char *str, SEXP_t *sexp);
int SEXP_snprintft (char *str, size_t size, SEXP_t *sexp);
int SEXP_asprintft (char **ret, SEXP_t *sexp);
int SEXP_asnprintft (char **ret, size_t maxsz, SEXP_t *sexp);
#endif /* 0 */

#ifdef __cplusplus
}
#endif

#endif /* SEXP_OUTPUT_H */
