
/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

#ifndef __STUB_PROBE
#pragma once
#ifndef COMMON_H
#define COMMON_H

#define _STR(x) #x
#define  STR(x) _STR(x)

#ifndef __XCONCAT
# include <sys/cdefs.h>
# define __XCONCAT(a, b) __CONCAT(a,b)
#endif

#include <stddef.h>
#include <stdint.h>
#include "../../../../common/util.h"

OSCAP_HIDDEN_START;

void    *xmemdup (const void *src, size_t len);
uint32_t xnumdigits (size_t size);
int      xstrncoll (const char *a, size_t alen, const char *b, size_t blen);
void     xsrandom (unsigned long seed);
long     xrandom (void);

#include <errno.h>

#ifndef EDOOFUS
# define EDOOFUS 88
#endif

#define protect_errno for (int __XCONCAT(__e,__LINE__) = errno, __XCONCAT(__s,__LINE__) = 1; \
                           __XCONCAT(__s,__LINE__)-- ; errno = __XCONCAT(__e,__LINE__))

OSCAP_HIDDEN_END;

#endif /* COMMON_H */
#endif
