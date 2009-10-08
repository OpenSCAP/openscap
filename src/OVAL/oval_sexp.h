#pragma once
#ifndef OVAL_SEXP_H
#define OVAL_SEXP_H

#include <seap.h>
#include <config.h>
#include "../common/util.h"
#include "oval_definitions_impl.h"

OSCAP_HIDDEN_START;

/*
 * OVAL -> S-exp
 */
SEXP_t *oval_object_to_sexp (const char *typestr, struct oval_object *object);

/*
 * S-exp -> OVAL
 */
SEXP_t *oval_state_to_sexp (struct oval_state *state);
struct oval_syschar *sexp_to_oval_state (SEXP_t *sexp, struct oval_object* object);
int oval_syschar_apply_sexp (struct oval_syschar *syschar, SEXP_t *sexp, struct oval_object* object);

OSCAP_HIDDEN_END;

#endif /* OVAL_SEXP_H */
