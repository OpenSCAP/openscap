/*
 * Copyright 2011 Tresys Technology Inc., Columbia, Maryland.
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
 *      "Marshall Miller" <mmiller@tresys.com>
 */

#include "config.h"
#if !defined(HAVE_VASPRINTF)

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "vasprintf.h"

int vasprintf(char **strp, const char *fmt, va_list ap) {
    va_list ap2;
    char tmpchar[1];
    char *newstr;
    int s;

    va_copy(ap2, ap);
    s = vsnprintf(tmpchar, 1, fmt, ap);

    newstr = malloc(s+1);
    if (newstr == NULL) {
        va_end(ap2);
        return (-1);
    }
    s = vsnprintf(newstr, s+1, fmt, ap2);
    va_end(ap2);

    *strp = newstr;
    return (s);
}

#endif

