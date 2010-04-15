
/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
SEXP_t *oval_object2sexp(const char *typestr, struct oval_object *object, struct oval_syschar_model *syschar_mode);
SEXP_t *oval_state2sexp(struct oval_state *state);

/*
 * S-exp -> OVAL
 */
struct oval_syschar *oval_sexp2sysch(const SEXP_t * s_exp, struct oval_syschar_model *model,
				     struct oval_object *object);
int oval_sysch_apply_sexp(struct oval_syschar *sysch, const SEXP_t * s_list, struct oval_object *object);

OSCAP_HIDDEN_END;

#endif				/* OVAL_SEXP_H */
