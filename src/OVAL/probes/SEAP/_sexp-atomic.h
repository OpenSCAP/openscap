/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
#ifndef _SEXP_ATOMIC_H
#define _SEXP_ATOMIC_H

#include <stdint.h>
#include <stdbool.h>

uint16_t SEXP_atomic_dec_u16 (volatile uint16_t *ptr);
uint16_t SEXP_atomic_inc_u16 (volatile uint16_t *ptr);
bool     SEXP_atomic_cas_u16 (volatile uint16_t *ptr, uint16_t old, uint16_t new);

uint32_t SEXP_atomic_dec_u32 (volatile uint32_t *ptr);
uint32_t SEXP_atomic_inc_u32 (volatile uint32_t *ptr);
bool     SEXP_atomic_cas_u32 (volatile uint32_t *ptr, uint32_t old, uint32_t new);

#endif /* _SEXP_ATOMIC_H */
