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
#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include "../../../../common/util.h"


typedef uint32_t bitmap_cell_t;
typedef uint16_t bitmap_size_t;
typedef int32_t  bitmap_bitn_t;

typedef struct {
#if defined(SEAP_THREAD_SAFE)
        uint8_t   locked;
#endif
        bitmap_size_t  size;  /* bit capacity = size * BITMAP_CELLSIZE */
        bitmap_size_t  realsize;
        bitmap_cell_t *cells;
        bitmap_bitn_t  count;
} bitmap_t;

#if defined(SEAP_THREAD_SAFE)
# define BITMAP_INITIALIZER { 0, 128, 0, NULL, 0 }
#else
# define BITMAP_INITIALIZER { 128, 0, NULL, 0 }
#endif

#define BITMAP_CELLSIZE (sizeof (bitmap_cell_t) * 8)

bitmap_t *bitmap_new (bitmap_size_t size);
int      *bitmap_init (bitmap_t *bitmap, bitmap_size_t size);
int      *bitmap_reinit (bitmap_t *bitmap, bitmap_size_t size);
int       bitmap_set (bitmap_t *bitmap, bitmap_bitn_t bitn);
int       bitmap_cas (bitmap_t *bitmap, bitmap_bitn_t bitn, int v);
int       bitmap_unset (bitmap_t *bitmap, bitmap_bitn_t bitn);
int       bitmap_clear (bitmap_t *bitmap);
bitmap_bitn_t bitmap_setfree (bitmap_t *bitmap);
bitmap_bitn_t bitmap_getfree (bitmap_t *bitmap);
void      bitmap_free (bitmap_t *bitmap);


#endif
