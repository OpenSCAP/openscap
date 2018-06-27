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

/*
 * Sparse buffer API implementation
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <errno.h>

#include "spb.h"

spb_t *spb_new (void *buffer, size_t buflen, uint32_t balloc)
{
	spb_t *spb = malloc(sizeof(spb_t));

        spb->balloc = (balloc == 0 ? SPB_DEFAULT_BALLOC : balloc);
	spb->buffer = malloc(sizeof(spb_item_t) * spb->balloc);
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

uint32_t spb_bindex (spb_t *spb, spb_size_t idx)
{
        uint32_t w, s;

        w = spb->btotal;
        s = spb_size (spb);

        if (idx < s) {
                s = 0;

                /* Find the correct buffer using binary search */
                while (w > 0) {
                        if (idx > spb->buffer[s + w/2].gend) {
                                s += w/2 + 1;
                                w  = w - w/2 - 1;
                        } else {
                                w  = w/2;
                        }
                }
        } else
                s = w;
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

		spb->buffer = realloc(spb->buffer, sizeof(spb_item_t) * spb->balloc);
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
        register uint32_t b_idx;
        register uint8_t *b_dst = (uint8_t *)dst;

        b_idx = spb_bindex (spb, start);

        if (size == 0)
            return (0); /* No bytes to copy, return as success */

        if (b_idx < spb->btotal) {
                size_t l_off;
                size_t l_len;

                if (b_idx > 0) {
                        l_off = (size_t)(start - spb->buffer[b_idx - 1].gend - 1);
                        l_len = (size_t)(spb->buffer[b_idx].gend - spb->buffer[b_idx - 1].gend) - l_off;
                } else {
                        l_off = (size_t)(start);
                        l_len = (size_t)(spb->buffer[b_idx].gend + 1) - l_off;
                }

                if (size < l_len)
                        l_len = (size_t)size;

                memcpy (b_dst, (char *)(spb->buffer[b_idx].base) + l_off, l_len);

                b_dst += l_len;
                size  -= l_len;

                while (++b_idx < spb->btotal && size > 0) {
                        l_len = (size_t)(spb->buffer[b_idx].gend - spb->buffer[b_idx - 1].gend);

                        if (size < l_len)
                                l_len = (size_t)size;

                        memcpy (b_dst, spb->buffer[b_idx].base, l_len);

                        b_dst += l_len;
                        size  -= l_len;
                }

                if (size > 0)
                        return (1); /* less than size bytes were copyied */
                else
                        return (0);
        }
        /* else - the start offset is out of range*/

        errno = ERANGE;
        return (-1);
}

int spb_pick_raw (spb_t *spb, uint32_t bindex, spb_size_t start, spb_size_t size, void *dst)
{
        /* For now, just ignore the bindex argument and use spb_pick */
        return spb_pick (spb, start, size, dst);
}

/*
 * (cbarg) void *cb (void *cbarg, void *src, size_t size)
 */
int spb_pick_cb (spb_t *spb, spb_size_t start, spb_size_t size, void *cb (void *, void *, size_t), void *cbarg)
{
        register uint32_t b_idx;

        b_idx = spb_bindex (spb, start);

        if (b_idx < spb->btotal) {
                size_t l_off;
                size_t l_len;

                if (b_idx > 0) {
                        l_off = (size_t)(start - spb->buffer[b_idx - 1].gend - 1);
                        l_len = (size_t)(spb->buffer[b_idx].gend - spb->buffer[b_idx - 1].gend) - l_off;
                } else {
                        l_off = (size_t)(start);
                        l_len = (size_t)(spb->buffer[b_idx].gend + 1) - l_off;
                }

                if (size < l_len)
                        l_len = (size_t)size;

                cbarg = cb (cbarg, (char *)(spb->buffer[b_idx].base) + l_off, l_len);
                size -= l_len;

                while (++b_idx < spb->btotal && size > 0) {
                        l_len = (size_t)(spb->buffer[b_idx].gend - spb->buffer[b_idx - 1].gend);

                        if (size < l_len)
                                l_len = (size_t)size;

                        cbarg = cb (cbarg, spb->buffer[b_idx].base, l_len);
                        size  -= l_len;
                }

                if (size > 0)
                        return (1); /* less than size bytes were copyied */
                else
                        return (0);
        }
        /* else - the start offset is out of range*/

        errno = ERANGE;
        return (-1);
}

void spb_free (spb_t *spb, spb_flags_t flags)
{
        if (spb == NULL) {
                errno = EFAULT;
                return;
        }

        if (flags & SPB_FLAG_FREE) {
                register uint32_t i;

                for (i = 0; i < spb->btotal; ++i)
			free(spb->buffer[i].base);
        }
#ifndef NDEBUG
        memset (spb->buffer, 0, sizeof (spb_item_t) * spb->balloc);
#endif
	free(spb->buffer);
#ifndef NDEBUG
        memset (spb, 0, sizeof (spb_t));
#endif
	free(spb);
        return;
}

spb_size_t spb_drop_head (spb_t *spb, spb_size_t size, spb_flags_t flags)
{
        spb_size_t e_sub = 0;
        uint32_t   b_idx;

        b_idx = spb_bindex (spb, size);

        if (b_idx > 0) {
                if (b_idx >= spb->btotal) {
                        /* free everything */
                        e_sub = spb->buffer[spb->btotal - 1].gend + 1;

                        if (flags & SPB_FLAG_FREE) {
                                register uint32_t i;

                                for (i = spb->btotal; i > 0; --i)
					free(spb->buffer[i - 1].base);
                        }

			spb->buffer = realloc(spb->buffer, sizeof(spb_item_t) * SPB_DEFAULT_BALLOC);
                        spb->btotal = 0;
                        spb->balloc = SPB_DEFAULT_BALLOC;
                } else {
                        /* free buffer 0..(b_idx - 1) */
                        register uint32_t i;

                        e_sub = spb->buffer[b_idx - 1].gend + 1;

                        if (flags & SPB_FLAG_FREE) {
                                for (i = b_idx - 1; i > 0; --i)
					free(spb->buffer[i - 1].base);
                        }

                        spb->btotal -= b_idx;
                        memmove (spb->buffer, spb->buffer + b_idx, sizeof (spb_item_t) * spb->btotal);

                        /*
                         * Shrink the pre-allocated memory if it's more than 2 times larger than
                         * the space needed for the remaining buffers.
                         */
                        if ((spb->balloc >> 1) > spb->btotal) {
                                spb->balloc >>= 1;
					spb->buffer = realloc(spb->buffer, sizeof(spb_item_t) * spb->balloc);
                        }

                        /*
                         * Update the global end index member in each remaining buffer item
                         */
                        for (i = spb->btotal; i > 0; --i)
                                spb->buffer[i - 1].gend -= e_sub;
                }
        }

        return (e_sub);
}

uint8_t spb_octet (spb_t *spb, spb_size_t idx)
{
        uint32_t b_idx;
        size_t   l_off;

        b_idx = spb_bindex (spb, idx);

        if (b_idx < spb->btotal) {
                l_off = (size_t)(b_idx > 0 ? idx - spb->buffer[b_idx - 1].gend - 1 : idx);
                return *((uint8_t *)(spb->buffer[b_idx].base) + l_off);
        }

        errno = ERANGE;

        return (uint8_t)(-1);
}

const uint8_t *spb_direct (spb_t *spb, spb_size_t start, spb_size_t size)
{
        uint32_t b_idx;
        size_t   l_off;

        b_idx = spb_bindex (spb, start);

        if (b_idx < spb->btotal) {
                if (start + size - 1 <= spb->buffer[b_idx].gend) {
                        l_off = (size_t)(b_idx > 0 ? start - spb->buffer[b_idx - 1].gend - 1 : start);
                        return ((const uint8_t *)(spb->buffer[b_idx].base) + l_off);
                }
        } else
                errno = ERANGE;

        return (NULL);
}
