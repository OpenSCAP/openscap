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

/*
 * Sparse buffer API implementation
 */

#include "spb.h"
#include "sm_alloc.h"

spb_t *spb_new (void *buffer, size_t buflen, uint32_t balloc)
{
        spb_t *spb;

        spb = sm_talloc (spb_t);
        spb->balloc = (balloc == 0 ? SPB_DEFAULT_BALLOC : balloc);
        spb->buffer = sm_alloc (sizeof (spb_item_t) * spb->balloc);
        spb->bflags = 0;

        if (buffer != NULL && buflen > 0) {
                spb->btotal = 1;
                spb->buffer[0].base = buffer;
                spb->buffer[0].gend = (spb_size_t)(buflen - 1);
        } else {
                spb->btotal = 0;
        }
        
        return (spb);
}

uint32_t spb_bindex (spb_t *spb, spb_size_t index)
{
        uint32_t w, s;

        w = spb->btotal;
        s = spb_size (spb);
        
        if (index < s) {
                s = 0;

                /* Find the correct buffer using binary search */
                while (w > 0) {
                        if (index > spb->buffer[s + w/2].gend) {
                                s += w/2 + 1;
                                w  = w - w/2 - 1;
                        } else if (index < spb->buffer[s + w/2].gend) {
                                w  = w/2;
                        } else
                                break;
                }
        }
        
        /*
         * `s' should now contain the index of the spb array item
         * that contains the buffer that represents the byte range
         * in the global buffer that cointains the requested index.
         * If `index' was out of range then `s' will hold a value
         * equal to or greater than the total number of buffers in
         * the spb.
         */
        return (s);
}

spb_size_t spb_size (spb_t *spb)
{
        return (spb->btotal > 0 ? spb->buffer[spb->btotal - 1].gend + 1 : 0);
}

int spb_add (spb_t *spb, void *buffer, size_t buflen)
{
        spb_size_t gend;
        
        if (spb->btotal >= spb->balloc) {
                if (spb->balloc < SPB_BALLOC_HIGHTRESH)
                        spb->balloc <<= 1;
                else
                        spb->balloc  += SPB_BALLOC_ADD;
                
                spb->buffer = sm_realloc (spb->buffer, sizeof (spb_item_t) * spb->balloc);
        }
        
        /* XXX: assume that btotal > 0 if btotal > balloc? */
        if (spb->btotal > 0)
                gend = spb->buffer[spb->btotal - 1].gend + buflen;
        else
                gend = buflen - 1;
        
        spb->buffer[spb->btotal].base = buffer;
        spb->buffer[spb->btotal].gend = gend;
        ++spb->btotal;
        
        return (0);
}

int spb_pick (spb_t *spb, spb_size_t start, spb_size_t size, void *dst)
{
        return (-1);
}

int spb_pick_raw (spb_t *spb, uint32_t bindex, spb_size_t start, spb_size_t size, void *dst)
{
        return (-1);
}

void spb_free (spb_t *spb)
{
#ifndef NDEBUG
        memset (spb->buffer, 0, sizeof (spb_item_t) * spb->balloc);
#endif
        sm_free (spb->buffer);
#ifndef NDEBUG
        memset (spb, 0, sizeof (spb_t));
#endif
        sm_free (spb);
        return;
}
