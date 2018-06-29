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
#include <stdint.h>
#include <assert.h>

#include "common.h"
#include "debug_priv.h"

void *xmemdup (const void *src, size_t len)
{
        void *new;

        _A(src != NULL);

	new = malloc(len);
        memcpy (new, src, len);

        return (new);
}

uint32_t xnumdigits (size_t size)
{
        uint32_t i = 0;

        do {
                ++i;
                size /= 10;
        } while (size > 0);

        return (i);
}

int xstrncoll (const char *a, size_t alen,
               const char *b, size_t blen)
{
        int cmp;

        _A(a != NULL);
        _A(b != NULL);

        if (alen > blen) {
                cmp = strncmp (a, b, blen);
                return (cmp == 0 ?  1 : cmp);
        } else if (blen > alen) {
                cmp = strncmp (a, b, alen);
                return (cmp == 0 ? -1 : cmp);
        } else {
                return strncmp (a, b, alen);
        }
}

void xsrandom (unsigned long seed)
{
	srand(seed);
}

long xrandom (void)
{
	return rand();
}
