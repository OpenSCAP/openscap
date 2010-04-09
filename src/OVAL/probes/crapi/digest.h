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
#ifndef CRAPI_DIGEST_H
#define CRAPI_DIGEST_H

#include <stdarg.h>
#include <stddef.h>

typedef enum {
        CRAPI_DIGEST_MD5    = 0x01,
        CRAPI_DIGEST_SHA1   = 0x02,
        CRAPI_DIGEST_SHA256 = 0x04,
        CRAPI_DIGEST_SHA512 = 0x08,
        CRAPI_DIGEST_RMD160 = 0x10
} crapi_alg_t;

#define CRAPI_DIGEST_CNT 5

int crapi_digest_fd (int fd, crapi_alg_t alg, void *dst, size_t *size);

struct digest_ctbl_t {
        void *ctx;
        void *(*init)  (void *, void *);
        int   (*update)(void *, void *, size_t);
        int   (*fini)  (void *);
        void  (*free)  (void *);
};

int crapi_mdigest_fd (int fd, int num, ... /*crapi_alg_t alg, void *dst, size_t *size, ...*/);

#endif /* CRAPI_DIGEST_H */
