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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#include "strto.h"

uint8_t strto_uint8_hex (const char *str, size_t len, char **endptr)
{
    uint64_t r = strto_uint64(str, len, endptr, 16);

    if (errno != 0 && r <= UINT16_MAX)
        return (uint8_t)r;
    else if (r > UINT8_MAX) {
        errno = ERANGE;
        return UINT8_MAX;
    } else
        return (uint8_t)r;
}

uint16_t strto_uint16_hex(const char *str, size_t len, char **endptr)
{
    uint64_t r = strto_uint64(str, len, endptr, 16);

    if (errno != 0 && r <= UINT16_MAX)
        return (uint16_t)r;
    else if (r > UINT16_MAX) {
        errno = ERANGE;
        return UINT16_MAX;
    } else
        return (uint16_t)r;
}

uint32_t strto_uint32_hex(const char *str, size_t len, char **endptr)
{
    uint64_t r = strto_uint64(str, len, endptr, 16);

    if (errno != 0 && r <= UINT32_MAX)
        return (uint32_t)r;
    else if (r > UINT32_MAX) {
        errno = ERANGE;
        return UINT32_MAX;
    } else
        return (uint32_t)r;
}

int64_t strto_int64 (const char *str, size_t len, char **endptr, int base)
{
    int errno_copy = 0;
    int64_t result = 0;
    char *null_str = calloc(len + 1, sizeof(str));

    memcpy(null_str, str, len);
    errno = 0;
    result = strtoll(null_str, endptr, base);
    errno_copy = errno;
    free(null_str);
    errno = errno_copy;

    return result;
}

uint64_t strto_uint64 (const char *str, size_t len, char **endptr, int base)
{
    int errno_copy = 0;
    int64_t result = 0;
    char *null_str = calloc(len + 1, sizeof(str));

    memcpy(null_str, str, len);
    errno = 0;
    result = strtoull(null_str, endptr, base);
    errno_copy = errno;
    free(null_str);
    errno = errno_copy;

    return result;
}

double strto_double (const char *str, size_t len, char **endptr)
{
    int errno_copy = 0;
    int64_t result = 0;
    char *null_str = calloc(len + 1, sizeof(str));

    memcpy(null_str, str, len);
    errno = 0;
    result = strtod(null_str, endptr);
    errno_copy = errno;
    free(null_str);
    errno = errno_copy;

    return result;
}
