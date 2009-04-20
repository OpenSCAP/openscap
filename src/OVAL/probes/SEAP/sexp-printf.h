#pragma once
#ifndef SEXP_PRINTF_H
#define SEXP_PRINTF_H

#include <stdio.h>
#include <seap.h>

/* general */
int SEXP_printf (SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_fprintf (FILE *fp, SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_sprintf (char *str, SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_snprintf (char *str, size_t size, SEXP_format_t fmt, SEXP_t *sexp);
int SEXP_asprintf (char **ret, SEXP_format_t fmt, SEXP_t *sexp);

/* canonical */
#define SEXP_printf_canonical SEXP_printfc
#define SEXP_fprintf_canonical SEXP_fprintfc
#define SEXP_sprintf_canonical SEXP_sprintfc
#define SEXP_snprintf_canonical SEXP_snprintfc
#define SEXP_asprintf_canonical SEXP_asprintfc

int SEXP_printfc (SEXP_t *sexp);
int SEXP_fprintfc (FILE *fp, SEXP_t *sexp);
int SEXP_sprintfc (char *str, SEXP_t *sexp);
int SEXP_snprintfc (char *str, size_t size, SEXP_t *sexp);
int SEXP_asprintfc (char **ret, SEXP_t *sexp);

/* advanced */
#define SEXP_printf_advanced SEXP_printfa
#define SEXP_fprintf_advanced SEXP_fprintfa
#define SEXP_sprintf_advanced SEXP_sprintfa
#define SEXP_snprintf_advanced SEXP_snprintfa
#define SEXP_asprintf_advanced SEXP_asprintfa

int SEXP_printfa (SEXP_t *sexp);
int SEXP_fprintfa (FILE *fp, SEXP_t *sexp);
int SEXP_sprintfa (char *str, SEXP_t *sexp);
int SEXP_snprintfa (char *str, size_t size, SEXP_t *sexp);
int SEXP_asprintfa (char **ret, SEXP_t *sexp);

/* transport */
#define SEXP_printf_transport SEXP_printft
#define SEXP_fprintf_transport SEXP_fprintft
#define SEXP_sprintf_transport SEXP_sprintft
#define SEXP_snprintf_transport SEXP_snprintft
#define SEXP_asprintf_transport SEXP_asprintft

int SEXP_printft (SEXP_t *sexp);
int SEXP_fprintft (FILE *fp, SEXP_t *sexp);
int SEXP_sprintft (char *str, SEXP_t *sexp);
int SEXP_snprintft (char *str, size_t size, SEXP_t *sexp);
int SEXP_asprintft (char **ret, SEXP_t *sexp);

#endif /* SEXP_PRINTF_H */
