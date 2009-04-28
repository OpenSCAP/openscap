#pragma once
#ifndef SEXP_OUTPUT_H
#define SEXP_OUTPUT_H

#include <stdio.h>
#include <stdint.h>
#include <seap.h>

ssize_t SEXP_st_dprintf (int fd, SEXP_format_t fmt, SEXP_t *sexp, SEXP_ostate_t **ost);
ssize_t SEXP_st_dnprintf (int fd, size_t maxsz, SEXP_format_t fmt, SEXP_t *sexp, SEXP_ostate_t **ost);

/* canonical */
ssize_t SEXP_st_dprintc (int fd, SEXP_t *sexp, SEXP_ostate_t **ost);
ssize_t SEXP_st_dnprintc (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost);

/* advanced */
ssize_t SEXP_st_dprinta (int fd, SEXP_t *sexp, SEXP_ostate_t **ost);
ssize_t SEXP_st_dnprinta (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost);

/* transport */
ssize_t SEXP_st_dprintt (int fd, SEXP_t *sexp, SEXP_ostate_t **ost);
ssize_t SEXP_st_dnprintt (int fd, size_t maxsz, SEXP_t *sexp, SEXP_ostate_t **ost);

#endif /* SEXP_OUTPUT_H */
