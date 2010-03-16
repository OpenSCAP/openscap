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

#pragma once
#ifndef SPB_H
#define SPB_H

#include <stddef.h>
#include <inttypes.h>
#include <sys/cdefs.h>
#undef  __XC
#define __XC(a, b) __CONCAT(a, b)
#define _sym(n) __XC(__XC(_sym, __LINE__), n)

typedef uint32_t spb_flags_t;

#define SPB_FLAG_FREE 0x00000001

typedef uint64_t spb_size_t;
#define SPB_SZ_FMT "%"PRIo64

typedef struct {
        void       *base; /* buffer base address */
        spb_size_t  gend; /* sparse buffer index of the last byte of this buffer */
} spb_item_t;

typedef struct {
        spb_item_t *buffer; /* array of buffers representing the sparse buffer */
        uint32_t    btotal; /* number of buffer */
        uint32_t    balloc; /* number of allocated slots */
        uint32_t    bflags; /* flags - not used at the moment */
} spb_t;

#define SPB_FLAG_JOIN 0x00000001
#define SPB_FLAG_FILE 0x00000002

#define SPB_DEFAULT_BALLOC   32  /* default number of pre-allocated slots */
#define SPB_BALLOC_HIGHTRESH 512 /* the number of new slots is doubled until this limit is reached */
#define SPB_BALLOC_ADD       32  /* number of slot to add when we reached the high threshold */

/**
 * Create a new sparse buffer.
 * @param buffer first member of the sparse buffer
 * @param buflen length of the first buffer
 * @param balloc allocate space for `balloc' buffers (if more
 *        buffers will be added, the space will be automaticaly
 *        increased). If 0, default value will be used (SPB_DEFAULT_BALLOC).
 */
spb_t *spb_new (void *buffer, size_t buflen, uint32_t balloc);

void spb_free (spb_t *spb, spb_flags_t flags);

/**
 * Given the index of a byte in the sparse buffer, find the index
 * of the buffer that contains that byte.
 * @param spb sparse buffer
 * @param index index of the requested byte in the sparse buffer
 */
uint32_t spb_bindex (spb_t *spb, spb_size_t index);

/**
 * Get the size of the sparse buffer
 * @param spb sparse buffer
 */
spb_size_t spb_size (spb_t *spb);

/**
 * Iterate over octets starting at index start and ending at end.
 * The current octet is available in the `spb_oct' variable during
 * execution of each iteration.
 * @param spb sparse buffer
 * @param start starting index
 * @param end ending index (-1 means until End-Of-Buffer)
 */
#define spb_iterate_oct(spb, start, end, name) /* TODO */ while(0)

#define spb_iterate(spb, start, name, icode)                            \
        do {                                                            \
                spb_size_t _sym(istart) = (start);                      \
                spb_t     *_sym(ispb)   = (spb);                        \
                uint32_t   _sym(idx)    = spb_bindex(_sym(ispb), _sym(istart)); \
                size_t     _sym(l_off)  = (size_t)(_sym(idx) > 0 ? start - _sym(ispb)->buffer[_sym(idx) - 1].gend : start); \
                                                                        \
                for (; _sym(idx) < _sym(ispb)->btotal; ++_sym(idx)) {   \
                        register size_t   _sym(l);                      \
                        register uint8_t *_sym(b);                      \
                                                                        \
                        _sym(l) = (size_t)(_sym(idx) > 0 ?              \
                                           _sym(ispb)->buffer[_sym(idx)].gend - _sym(ispb)->buffer[_sym(idx) - 1].gend : \
                                           _sym(ispb)->buffer[_sym(idx)].gend + 1); \
                        _sym(b) = ((uint8_t *)(_sym(ispb)->buffer[_sym(idx)].base)) + _sym(l_off); \
                                                                        \
                        for (; _sym(l) > 0; --_sym(l), ++_sym(b)) {     \
                                (name) = *_sym(b);                      \
                                icode;                                  \
                        }                                               \
                                                                        \
                        if (_sym(l) > 0)                                \
                                break;                                  \
                                                                        \
                        _sym(l_off) = 0;                                \
                }                                                       \
        } while (0)

/**
 * Add new buffer to the sparse buffer
 * @param spb sparse buffer
 * @param buffer
 * @param buflen
 */
int spb_add (spb_t *spb, void *buffer, size_t buflen);

/**
 * Pick a buffer region from the sparse buffer and copy it.
 * @param spb sparse buffer
 * @param start starting index
 * @param size size of the region
 * @param dst destination memory
 */
int spb_pick (spb_t *spb, spb_size_t start, spb_size_t size, void *dst); 

/**
 * Pick a buffer region from the sparse buffer, starting with buffer
 * at index `bindex' and copy it. If the starting index isn't located
 * in the given starting buffer, signal an error.
 * @param spb sparse buffer
 * @param bindex index of the first buffer where the specified region begins
 * @param start starting index
 * @param size size of the region
 * @param dst destination memory
 */
int spb_pick_raw (spb_t *spb, uint32_t bindex, spb_size_t start, spb_size_t size, void *dst);

spb_size_t spb_drop_head (spb_t *spb, spb_size_t size, spb_flags_t flags);

#endif /* SPB_H */
