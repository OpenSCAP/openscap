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
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <limits.h>
#ifdef HAVE_UIO_H
#include <sys/uio.h>
#endif
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

#include "common/debug_priv.h"
#include "strbuf.h"

strbuf_t *strbuf_new  (size_t max)
{
        strbuf_t *buf;

        buf = malloc (sizeof (strbuf_t));

        if (buf != NULL) {
                buf->beg = NULL;
                buf->lbo = NULL;
                buf->blkmax = max;
                buf->blkoff = 0;
                buf->size   = 0;
        }

        return (buf);
}

void strbuf_free (strbuf_t *buf)
{
        struct strblk *blk, *next;

        next = buf->beg;

        while (next != NULL) {
                blk  = next;
                next = next->next;
                free (blk);
        }

        memset (buf, 0, sizeof (strbuf_t));
        free (buf);
        return;
}

size_t strbuf_size (strbuf_t *buf)
{
        return (buf->size);
}

static struct strblk *__strblk_new (size_t len)
{
        struct strblk *blk;

        blk = malloc (sizeof (struct strblk *) + sizeof (size_t) + (sizeof (char) * len));
        blk->next = NULL;
        blk->size = 0;

        return (blk);
}

static int __strbuf_add (strbuf_t *buf, char *str, size_t len)
{
        struct strblk *cur, *prev;
        size_t cpylen;

        if (buf->beg != NULL)
                if (buf->lbo != NULL)
                        cur = buf->lbo->next;
                else
                        cur = buf->beg;
        else {
                cur = buf->beg = __strblk_new (buf->blkmax);

                if (cur == NULL)
                        return (-1);
        }

        prev = buf->lbo;

        for (;;) {
                cpylen = buf->blkmax - buf->blkoff;

                if (cpylen >= len)
                        cpylen = len;

                memcpy (cur->data + buf->blkoff, str, sizeof (char) * cpylen);

                buf->size += cpylen;
                cur->size += cpylen;

                len -= cpylen;
                str += cpylen;

                buf->blkoff += cpylen;

                assert (buf->blkoff <= buf->blkmax);

                if (buf->blkoff == buf->blkmax) {
                        cur->next = __strblk_new (buf->blkmax);
                        prev = cur;
                        cur  = cur->next;
                        buf->blkoff = 0;
                }

                if (len == 0)
                        break;
        }

        buf->lbo = prev;

        return (0);
}

int strbuf_add (strbuf_t *buf, const char *str, size_t len)
{
        return __strbuf_add (buf, (char *)str, len);
}

int strbuf_addf (strbuf_t *buf, char *str, size_t len)
{
        int ret;

        ret = __strbuf_add (buf, str, len);

        if (ret == 0) {
                free (str);
                return (0);
        } else
                return (ret);
}

int strbuf_add0 (strbuf_t *buf, const char *str)
{
        return __strbuf_add (buf, (char *)str, strlen (str));
}

int strbuf_addc (strbuf_t *buf, char ch)
{
        /* XXX: direct? */
        return __strbuf_add (buf, &ch, 1);
}

int strbuf_add0f (strbuf_t *buf, char *str)
{
        int ret;

        ret = __strbuf_add (buf, str, strlen (str));

        if (ret == 0) {
                free (str);
                return (0);
        }

        return (ret);
}

int strbuf_trunc (strbuf_t *buf, size_t len)
{
        return (0);
}

size_t strbuf_length (strbuf_t *buf)
{
        return (buf->size);
}

char *strbuf_cstr (strbuf_t *buf)
{
        struct strblk *cur;
        char  *stroff, *strbeg;

	strbeg = malloc(buf->size + 1); /* +1 for terminating '\0' */
        stroff = strbeg;

        if (strbeg == NULL)
                return (NULL);

        cur = buf->beg;

        while (cur != NULL) {
                memcpy (stroff, cur->data, sizeof (char) * cur->size);
                stroff += cur->size;
                cur = cur->next;
        }
	strbeg[buf->size] = '\0';

        return (strbeg);
}

char *strbuf_cstr_r (strbuf_t *buf, char *str, size_t len)
{
        struct strblk *cur;
        char *stroff;

        if ((len - 1) < buf->size) {
                errno = ENOBUFS;
                return (NULL);
        }

        stroff = str;
        cur    = buf->beg;

        while (cur != NULL) {
                memcpy (stroff, cur->data, sizeof (char) * cur->size);
                stroff += cur->size;
                cur = cur->next;
        }

        return (str);
}

char *strbuf_copy (strbuf_t *buf, void *dst, size_t len)
{
        register struct strblk *cur;
        register uint8_t *mem;

        if (len < buf->size) {
                errno = ENOBUFS;
                return (NULL);
        }

        mem = (uint8_t *)dst;
        cur = buf->beg;

        while (cur != NULL) {
                memcpy (mem, cur->data, sizeof (char) * cur->size);
                mem += cur->size;
                cur  = cur->next;
        }

        return (dst);
}

size_t strbuf_fwrite (FILE *fp, strbuf_t *buf)
{
        struct strblk *cur;
        size_t size;

        cur   = buf->beg;
        size  = 0;

        while (cur != NULL) {
                size += fwrite (cur->data, sizeof (char),
                                cur->next == NULL ? buf->blkoff : cur->size, fp);
                cur   = cur->next;
        }

        return (size);
}

#ifdef HAVE_UIO_H

ssize_t strbuf_write (strbuf_t *buf, int fd)
{
        struct strblk *cur;
        ssize_t rsize, wsize;

        struct iovec  *iov;
        int            ioc; /* helper/index */
	int            iot; /* total number of buffers */
	int            iow; /* total number of buffer that will be passed to the next writev */

	rsize = 0;
	iov   = NULL;
        cur   = buf->beg;
	iot   = (buf->size / buf->blkmax) + 1;

	dD("total I/O vectors = %d", iot);

	while (iot > 0) {
		/*
		 * Prepare I/O vector
		 */
		if (iot > IOV_MAX) {
			dD("iot (%d) > IOV_MAX (%d)", iot, IOV_MAX);
			iow  = IOV_MAX;
			iot -= IOV_MAX;
		} else {
			dD("iot (%d) < IOV_MAX (%d)", iot, IOV_MAX);
			iow  = iot;
			iot  = 0;
		}

		iov = realloc (iov, sizeof (struct iovec) * iow);
		ioc = 0;

		while (cur != NULL && ioc < iow) {
			iov[ioc].iov_base = cur->data;
			iov[ioc].iov_len  = cur->size;

			++ioc;
			cur = cur->next;
		}

		dD("ioc = %d", ioc);

		/*
		 * Write
		 */
		wsize = writev (fd, iov, ioc);

		if (wsize < 0) {
			dE("writev(%d, %p, %d) failed: %u, %s.", fd, iov, ioc, errno, strerror (errno));
                        free(iov);
			return (-1);
		}

		rsize += wsize;
	}

        free (iov);
	dD("total bytes written: %zu", (size_t)rsize);

        return (rsize);
}

#else

ssize_t strbuf_write (strbuf_t *buf, int fd)
{
	struct strblk *cur;
	size_t size;

	cur = buf->beg;
	size = 0;

	while (cur != NULL) {
		size += write(fd, cur->data, cur->next == NULL ? buf->blkoff : cur->size);
		cur = cur->next;
	}

	return (size);
}

#endif
