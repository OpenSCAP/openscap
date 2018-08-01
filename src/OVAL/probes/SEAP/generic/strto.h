/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
#ifndef STRTO_H
#define STRTO_H

#include <stddef.h>
#include <stdint.h>
#include "../../../../common/util.h"

uint8_t strto_uint8_hex (const char *str, size_t len, char **endptr);
uint16_t strto_uint16_hex (const char *str, size_t len, char **endptr);
uint32_t strto_uint32_hex (const char *str, size_t len, char **endptr);
int64_t strto_int64 (const char *str, size_t len, char **endptr, int base);

uint64_t strto_uint64 (const char *str, size_t len, char **endptr, int base);

double strto_double (const char *str, size_t len, char **endptr);

#endif /* STRTO_H */
