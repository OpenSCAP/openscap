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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */
#pragma once
#ifndef CRAPI_DIGEST_H
#define CRAPI_DIGEST_H

#include <stdarg.h>
#include <stddef.h>

typedef enum {
#ifdef OPENSCAP_ENABLE_MD5
        CRAPI_DIGEST_MD5    = 0x01,
#endif
#ifdef OPENSCAP_ENABLE_SHA1
        CRAPI_DIGEST_SHA1   = 0x02,
#endif
        CRAPI_DIGEST_SHA256 = 0x04,
        CRAPI_DIGEST_SHA512 = 0x08,
        CRAPI_DIGEST_RMD160 = 0x10,
        CRAPI_DIGEST_SHA224 = 0x20,
        CRAPI_DIGEST_SHA384 = 0x40
} crapi_alg_t;

int crapi_digest_fd (int fd, crapi_alg_t alg, void *dst, size_t *size);

int crapi_mdigest_fd (int fd, int num, ... /*crapi_alg_t alg, void *dst, size_t *size, ...*/);

#endif /* CRAPI_DIGEST_H */
