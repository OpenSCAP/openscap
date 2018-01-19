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

#include <stdio.h>
#include <stdint.h>
#include "../../../../common/util.h"


#if defined(WANT_XBASE64)
# ifndef XBASE64_H
# define XBASE64_H

# define XBASE64_PADDING_CHAR '_'

size_t xbase64_encode (const uint8_t *, size_t, char **);
size_t xbase64_decode (const char *, size_t, uint8_t **);

# endif /* XBASE64_H */
#endif /* WANT_XBASE64 */

#if defined(WANT_BASE64)
# ifndef BASE64_H
# define BASE64_H

# define BASE64_PADDING_CHAR '='

size_t base64_encode (const uint8_t *, size_t, char **);
size_t base64_decode (const char *, size_t, uint8_t **);

# endif /* BASE64_H */
#endif /* WANT_BASE64 */

