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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

#include "common.h"
#include "bitmap.h"
#include "common/debug_priv.h"

bitmap_t *bitmap_new (bitmap_size_t size)
{
        _A(size > 0);

	bitmap_t *bitmap = malloc(sizeof(bitmap_t));
        bitmap->size = (size / BITMAP_CELLSIZE) + 1;
        bitmap->realsize = 0;
        bitmap->cells = NULL;
        bitmap->count = 0;

        xsrandom ((unsigned long)clock () ^
                  (unsigned long)getpid ());

        return (bitmap);
}

int *bitmap_init (bitmap_t *bitmap, bitmap_size_t size)
{
        _A(bitmap != NULL);
        _A(size > 0);

        bitmap->cells = NULL;
        bitmap->size = (size / BITMAP_CELLSIZE) + 1;
        bitmap->realsize = 0;
        bitmap->count = 0;

        xsrandom ((unsigned long)clock () ^
                  (unsigned long)getpid ());

        return (0);
}

int *bitmap_reinit (bitmap_t *bitmap, bitmap_size_t size)
{
        _A(bitmap != NULL);
        _A(size > 0);

	free(bitmap->cells);

        bitmap->cells = NULL;
        bitmap->size = (size / BITMAP_CELLSIZE) + 1;
        bitmap->realsize = 0;
        bitmap->count = 0;

        return (0);
}

int bitmap_set (bitmap_t *bitmap, bitmap_bitn_t bitn)
{
        bitmap_size_t i;

        _A(bitmap != NULL);
        _A((size_t)bitn < (size_t)bitmap->size * BITMAP_CELLSIZE);

        i = bitn/BITMAP_CELLSIZE + 1;

        if (i > bitmap->realsize) {
                if (i <= bitmap->size) {
			bitmap->cells = realloc(bitmap->cells, sizeof(uint32_t) * i);

                        memset (bitmap->cells + bitmap->realsize, 0,
                                sizeof (bitmap_cell_t) * (i - bitmap->realsize));

                        bitmap->realsize = i;
                } else {
                        return (1);
                }
        }

        bitmap->cells[i - 1] |= 1 << (bitn % BITMAP_CELLSIZE);
        ++bitmap->count;

        return (0);
}

int bitmap_cas (bitmap_t *bitmap, bitmap_bitn_t bitn, int v)
{
        _A(bitmap != NULL);
        _A((size_t)bitn < (size_t)bitmap->size * BITMAP_CELLSIZE);
        _A(v == 0 || v == 1);

        abort ();

        return (1);
}

int bitmap_unset (bitmap_t *bitmap, bitmap_bitn_t bitn)
{
        bitmap_size_t i;

        _A(bitmap != NULL);
        _A((size_t)bitn < (size_t)bitmap->size * BITMAP_CELLSIZE);

        i = bitn/BITMAP_CELLSIZE + 1;

        if (i <= bitmap->realsize) {
                bitmap->cells[i - 1] &= ~(1 << (bitn % BITMAP_CELLSIZE));
                --bitmap->count;

                if (bitmap->realsize - i == 0) {
                        while (bitmap->cells[bitmap->realsize - 1] == 0)
                                --bitmap->realsize;

			bitmap->cells = realloc(bitmap->cells, sizeof(uint32_t) * bitmap->realsize);
                }
        }

        return (0);
}

int bitmap_clear (bitmap_t *bitmap)
{
        _A(bitmap != NULL);

        if (bitmap->realsize > 0) {
		free(bitmap->cells);
                bitmap->cells = NULL;
                bitmap->realsize = 0;
                bitmap->count = 0;
        }

        return (0);
}

bitmap_bitn_t bitmap_setfree (bitmap_t *bitmap)
{
        bitmap_size_t i, j;

        _A(bitmap != NULL);

        for (i = 0; i < bitmap->realsize; ++i) {
                if (bitmap->cells[i] != ~(bitmap_cell_t)(0)) {
                        /* TODO: binary search */
                        for (j = 0; j < BITMAP_CELLSIZE; ++j) {
                                if (((bitmap->cells[i] >> j) & 1) == 0) {

                                        bitmap->cells[i] |= 1 << j;

                                        return ((i * BITMAP_CELLSIZE) + j);
                                }
                        }
                }
        }

        if (bitmap->realsize < bitmap->size)
                return (bitmap_set (bitmap,
                                    BITMAP_CELLSIZE * bitmap->realsize) ? -1 : (bitmap_bitn_t)(bitmap->realsize * BITMAP_CELLSIZE));
        else
                return (-1);
}

bitmap_bitn_t bitmap_getfree (bitmap_t *bitmap)
{
        _A(bitmap != NULL);

        return (-1);
}

void bitmap_free (bitmap_t *bitmap)
{
        _A(bitmap != NULL);

        if (bitmap->realsize > 0)
		free(bitmap->cells);
	free(bitmap);
}

#if defined(BITMAP_TEST)
#include <stdio.h>

void bitmap_print (bitmap_t *bitmap)
{
        bitmap_size_t i, j;

        printf ("bitmap@%p: size=%u, realsize=%u, cells@%p\n",
                (void *)bitmap, bitmap->size, bitmap->realsize, (void *)bitmap->cells);

        for (i = 0; i < bitmap->realsize; ++i) {
                printf ("[%u] hex: %08x | bin: ", i, bitmap->cells[i]);
                for (j = 0; j < BITMAP_CELLSIZE; ++j)
                        printf ("%u", bitmap->cells[i] & (1 << j) ? 1 : 0);
                printf ("\n");
        }

        return;
}

int main (void)
{
        bitmap_t *bmap1;
        bitmap_t  bmap2;

        printf ("Create, print.\n");

        bmap1 = bitmap_new (8);
        bitmap_print (bmap1);

        printf ("Set 1, 2, 5, 9, 56, 77, 79\n ");

        if (bitmap_set (bmap1, 1) == 0)
                printf ("ok ");
        if (bitmap_set (bmap1, 2) == 0)
                printf ("ok ");
        if (bitmap_set (bmap1, 5) == 0)
                printf ("ok ");
        if (bitmap_set (bmap1, 9) == 0)
                printf ("ok ");
        if (bitmap_set (bmap1, 56) == 0)
                printf ("ok ");
        if (bitmap_set (bmap1, 77) == 0)
                printf ("ok ");
        if (bitmap_set (bmap1, 79) == 0)
                printf ("ok ");

        printf ("\n");

        bitmap_print (bmap1);

        printf ("Unset 1, 9, 56, 77\n ");

        if (bitmap_unset (bmap1, 1) == 0)
                printf ("ok ");
        if (bitmap_unset (bmap1, 9) == 0)
                printf ("ok ");
        if (bitmap_unset (bmap1, 56) == 0)
                printf ("ok ");
        if (bitmap_unset (bmap1, 77) == 0)
                printf ("ok ");

        printf ("\n");

        bitmap_print (bmap1);

        printf ("Unset 79\n ");

        if (bitmap_unset (bmap1, 79) == 0)
                printf ("ok ");

        printf ("\n");

        bitmap_print (bmap1);

        printf ("Set 101, 119\n ");

        if (bitmap_set (bmap1, 101) == 0)
                printf ("ok ");
        if (bitmap_set (bmap1, 119) == 0)
                printf ("ok ");

        printf ("\n");

        bitmap_print (bmap1);

        printf ("setfree: %d, %d, %d\n",
                bitmap_setfree (bmap1), bitmap_setfree (bmap1), bitmap_setfree (bmap1));

        bitmap_print (bmap1);

        printf ("while setfree != -1\n");
        while (bitmap_setfree (bmap1) != -1);

        bitmap_print (bmap1);

        printf ("Clear\n");

        bitmap_clear (bmap1);

        bitmap_print (bmap1);

        printf ("while setfree != -1\n");
        while (bitmap_setfree (bmap1) != -1);

        bitmap_print (bmap1);

        printf ("Free\n");

        bitmap_free (bmap1);

        return (0);
}
#endif
