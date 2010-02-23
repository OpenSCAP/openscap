#pragma once
#ifndef SEXP_PARSE_H
#define SEXP_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <seap-debug.h>
#include <sexp-types.h>

typedef struct SEXP_psetup SEXP_psetup_t;

SEXP_psetup_t *SEXP_psetup_new  (void);
void           SEXP_psetup_free (SEXP_psetup_t *);

typedef struct SEXP_pstate SEXP_pstate_t;

SEXP_pstate_t *SEXP_pstate_new (void);
void           SEXP_pstate_free (SEXP_pstate_t *);
SEXP_pstate_t *SEXP_pstate_init (SEXP_pstate_t *);

SEXP_t *SEXP_parse_fd (SEXP_format_t fmt, int fd, size_t max, SEXP_pstate_t **state);
SEXP_t *SEXP_parse_buf (SEXP_format_t fmt, void *buf, size_t len, SEXP_pstate_t **state);
SEXP_t *SEXP_parse (const SEXP_psetup_t *setup, const char *buf, size_t buflen, SEXP_pstate_t **pstate);

#ifdef __cplusplus
}
#endif

#endif /* SEXP_PARSE_H */
