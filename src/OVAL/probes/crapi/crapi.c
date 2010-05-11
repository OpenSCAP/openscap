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
#include "crapi.h"
#include <config.h>

#if defined(HAVE_NSS3)
#include <nss.h>

int crapi_init (void *unused)
{
        return (NSS_NoDB_Init (NULL) == SECSuccess ? 0 : -1);
}
#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>
int crapi_init (void *unused)
{
        return (gcry_check_version(GCRYPT_VERSION) ? 0 : -1);
}
#else
int crapi_init (void *unused)
{
        return (0);
}
#endif
