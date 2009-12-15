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
SEXP_t *oval_object2sexp (const char *typestr, struct oval_object *object, struct oval_syschar_model *syschar_mode);
SEXP_t *oval_state2sexp (struct oval_state *state);

/*
 * S-exp -> OVAL
 */
struct oval_syschar *oval_sexp2sysch (const SEXP_t *s_exp, struct oval_syschar_model *model, struct oval_object *object);
int oval_sysch_apply_sexp (struct oval_syschar *sysch, const SEXP_t *s_list, struct oval_object *object);

OSCAP_HIDDEN_END;

#endif /* OVAL_SEXP_H */
