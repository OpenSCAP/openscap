/**
 * @file   oval_sexp.h
 * @brief  OVAL to/from S-exp conversion functions - header file
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 * @author "Lukas Kuklinek" <lkukline@redhat.com>
 * @author "Tomas Heinrich" <theinric@redhat.com>
 *
 * @addtogroup PROBEOVALSEXP
 * @{
 */
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
#include "../common/util.h"
#include "oval_definitions_impl.h"


/*
 * OVAL -> S-exp
 */
SEXP_t *oval_value_to_sexp(struct oval_value *val, oval_datatype_t dtype);

int oval_object_to_sexp(void *sess, const char *typestr, struct oval_syschar *syschar, SEXP_t **out_sexp);
int oval_state_to_sexp(void *sess, struct oval_state *state, SEXP_t **out_sexp);

/*
 * S-exp -> OVAL
 */
int oval_sexp_to_sysch(const SEXP_t *cobj, struct oval_syschar *syschar);

#endif				/* OVAL_SEXP_H */

/// @}
