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

#ifndef _SEXP_LIST_R_H
#define _SEXP_LIST_R_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "_sexp-types.h"

SEXP_t *SEXP_list_new_rv(SEXP_t *sexp_mem, SEXP_t *memb, va_list alist);
SEXP_t *SEXP_list_new_r(SEXP_t *sexp_mem, SEXP_t *memb, ...);
SEXP_t *SEXP_list_rest_r (SEXP_t *rest, const SEXP_t *list);

#endif /* _SEXP_LIST_R_H */
