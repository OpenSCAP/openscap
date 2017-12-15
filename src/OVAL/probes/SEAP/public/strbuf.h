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
#ifndef STRBUF_H
#define STRBUF_H

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include "oscap_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SEAP_STRBUF_MAX 8192

struct strblk {
        struct strblk *next;
        size_t         size;
        char           data[];
};

typedef struct {
        struct strblk *beg;
        struct strblk *lbo;
        size_t         blkmax;
        size_t         blkoff;
        size_t         size;
} strbuf_t;

OSCAP_API strbuf_t *strbuf_new  (size_t max);
OSCAP_API void      strbuf_free (strbuf_t *buf);

OSCAP_API int strbuf_add   (strbuf_t *buf, const char *str, size_t len);
OSCAP_API int strbuf_addf  (strbuf_t *buf, char *str, size_t len);
OSCAP_API int strbuf_add0  (strbuf_t *buf, const char *str);
OSCAP_API int strbuf_add0f (strbuf_t *buf, char *str);
OSCAP_API int strbuf_addc (strbuf_t *buf, char ch);

OSCAP_API size_t strbuf_size (strbuf_t *buf);
OSCAP_API int    strbuf_trunc  (strbuf_t *buf, size_t len);
OSCAP_API size_t strbuf_length (strbuf_t *buf);

OSCAP_API char *strbuf_cstr   (strbuf_t *buf);
OSCAP_API char *strbuf_cstr_r (strbuf_t *buf, char *str, size_t len);
OSCAP_API char *strbuf_copy (strbuf_t *buf, void *dst, size_t len);

OSCAP_API size_t strbuf_fwrite (FILE *fp, strbuf_t *buf);
OSCAP_API ssize_t strbuf_write  (strbuf_t *buf, int fd);

#ifdef __cplusplus
}
#endif

#endif /* STRBUF_H */
