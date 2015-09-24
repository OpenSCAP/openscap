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

#include "strto.h"

uint8_t strto_uint8_hex (const char *str, size_t len, char **endptr)
{
    uint64_t r = strto_uint64_hex(str, len, endptr);

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
    uint64_t r = strto_uint64_hex(str, len, endptr);

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
    uint64_t r = strto_uint64_hex(str, len, endptr);

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
        switch (base) {
        case 10: return strto_int64_dec (str, len, endptr);
        case 16: return strto_int64_hex (str, len, endptr);
        case  2: return strto_int64_bin (str, len, endptr);
        case  8: return strto_int64_oct (str, len, endptr);
        }

        /* Invalid number base */
        errno = EINVAL;
        return INT64_C(0);
}

int64_t strto_int64_bin (const char *str, size_t len, char **endptr)
{
        errno = 0;
        return INT64_C(0);
}

int64_t strto_int64_oct (const char *str, size_t len, char **endptr)
{
        errno = 0;
        return INT64_C(0);
}

int64_t strto_int64_dec (const char *str, size_t len, char **endptr)
{
        int64_t n;
        int8_t  p;
        char   *s;

        errno = 0;
        n = 0;
        p = 1;
        s = (char *)str;

        while (len > 0) {
                switch (*s) {
                case '-': p = -1; ++s; --len;
                        break;
                case '+': p =  1; ++s; --len;
                        break;
                case ' ': ++s; --len;
                        continue;
                }

                break;
        }

        while (len > 0) {
                if (*s < '0' || *s > '9')
                        break;

                n *= 10;

                if (n < 0) {
                        errno = ERANGE;
                        return (p == 1 ? INT64_MAX : INT64_MIN);
                }

                n += *s - '0';
                --len;
                ++s;
        }

        n *= p;

        if (len != 0) {
                if (s == str)
                        errno = EINVAL;
                if (endptr != NULL)
                        *endptr = s;
        } else if (n * p < 0 && !(n == INT64_MIN && s[-1] == '8')) {
                errno = ERANGE;
                return (p == 1 ? INT64_MAX : INT64_MIN);
        }

        return (n);
}

int64_t strto_int64_hex (const char *str, size_t len, char **endptr)
{
        errno = 0;
        return INT64_C(0);
}

uint64_t strto_uint64 (const char *str, size_t len, char **endptr, int base)
{
        switch (base) {
        case 10: return strto_uint64_dec (str, len, endptr);
        case 16: return strto_uint64_hex (str, len, endptr);
        case  2: return strto_uint64_bin (str, len, endptr);
        case  8: return strto_uint64_oct (str, len, endptr);
        }

        /* Invalid number base */
        errno = EINVAL;

        return UINT64_C(0);
}

uint64_t strto_uint64_bin (const char *str, size_t len, char **endptr)
{
        uint64_t n, t;
        char    *s;
        unsigned char c;

        errno = 0;
        n = t = 0;
        s = (char *)str;

        while (len > 0) {
                switch (*s) {
                case '+': ++s; --len;
                        break;
                case ' ': ++s; --len;
                        continue;
                }

                break;
        }

        while (len > 0) {
                if (*s != '0' && *s != '1')
                        break;

                n <<= 1;
                c  = *s - '0';

                if (n < t || (UINT64_MAX - n) < c) {
                        errno = ERANGE;
                        return (UINT64_MAX);
                }

                n += c;

                t = n;
                --len;
                ++s;
        }

        if (len != 0) {
                if (s == str)
                        errno = EINVAL;
                if (endptr != NULL)
                        *endptr = s;
        }

        return (n);
}

uint64_t strto_uint64_oct (const char *str, size_t len, char **endptr)
{
        uint64_t n, t;
        char    *s;
        unsigned char c;

        errno = 0;
        n = t = 0;
        s = (char *)str;

        while (len > 0) {
                switch (*s) {
                case '+': ++s; --len;
                        break;
                case ' ': ++s; --len;
                        continue;
                }

                break;
        }

        while (len > 0) {
                if (*s < '0' || *s > '7')
                        break;

                n *= 8;
                c  = *s - '0';

                if (n < t || (UINT64_MAX - n) < c) {
                        errno = ERANGE;
                        return (UINT64_MAX);
                }

                n += c;
                t  = n;
                --len;
                ++s;
        }

        if (len != 0) {
                if (s == str)
                        errno = EINVAL;
                if (endptr != NULL)
                        *endptr = s;
        }

        return (n);
}

uint64_t strto_uint64_dec (const char *str, size_t len, char **endptr)
{
        uint64_t n, t;
        char    *s;
        unsigned char c;

        errno = 0;
        n = t = 0;
        s = (char *)str;

        while (len > 0) {
                switch (*s) {
                case '+': ++s; --len;
                        break;
                case ' ': ++s; --len;
                        continue;
                }

                break;
        }

        while (len > 0) {
                if (*s < '0' || *s > '9')
                        break;

                n *= 10;
                c  = *s - '0';

                if (n < t || (UINT64_MAX - n) < c) {
                        errno = ERANGE;
                        return (UINT64_MAX);
                }

                n += c;
                t  = n;
                --len;
                ++s;
        }

        if (len != 0) {
                if (s == str)
                        errno = EINVAL;
                if (endptr != NULL)
                        *endptr = s;
        }

        return (n);
}

uint64_t strto_uint64_hex (const char *str, size_t len, char **endptr)
{
        uint64_t n, t;
        char    *s;
        unsigned char c;

        errno = 0;
        n = t = 0;
        s = (char *)str;

        while (len > 0) {
                switch (*s) {
                case '+': ++s; --len;
                        break;
                case ' ': ++s; --len;
                        continue;
                }

                break;
        }

        while (len > 0) {
                if (*s < '0' || *s > 'f')
                        break;

                n *= 16;

                if (*s <= '9')
                        c = *s - '0';
                else if (*s >= 'a')
                        c = *s - 'a' + 10;
                else if (*s >= 'A' && *s <= 'F')
                        c = *s - 'A' + 10;
                else
                        break;

                if (n < t || (UINT64_MAX - n) < c) {
                        errno = ERANGE;
                        return (UINT64_MAX);
                }

                n += c;
                t  = n;
                --len;
                ++s;
        }

        if (len != 0) {
                if (s == str)
                        errno = EINVAL;
                if (endptr != NULL)
                        *endptr = s;
        }

        return (n);
}

double strto_double (const char *str, size_t len, char **endptr)
{
        char s[256];

        errno = 0;
        /* FIXME: temporary solution */
        if (len < (sizeof s)/(sizeof (char))) {
                memcpy (s, str, sizeof (char) * len);
                s[len] = '\0';

                return strtod (s, endptr);
        } else {
                errno = ERANGE;
                return (0);
        }
}
