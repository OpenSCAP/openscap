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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/select.h>

#include "public/sm_alloc.h"
#include "public/strbuf.h"
#include "_sexp-types.h"
#include "_sexp-output.h"
#include "_seap-types.h"
#include "_seap-scheme.h"
#include "sch_generic.h"
#include "seap-descriptor.h"

#define DATA(ptr) ((sch_genericdata_t *)(ptr))

int sch_generic_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags)
{
        (void)uri;
        (void)flags;
        return (-1);
}

int sch_generic_openfd (SEAP_desc_t *desc, int fd, uint32_t flags)
{
        sch_genericdata_t *data;

        data = sm_talloc (sch_genericdata_t);
        data->ifd = -1;
        data->ofd = -1;

        if (flags & SEAP_DESC_FDIN)
                data->ifd = fd;
        if (flags & SEAP_DESC_FDOUT)
                data->ofd = fd;

        desc->scheme_data = data;

        return (0);
}

int sch_generic_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags)
{
        sch_genericdata_t *data;

        data = sm_talloc (sch_genericdata_t);
        data->ifd = ifd;
        data->ofd = ofd;
        desc->scheme_data = data;

        return (0);
}

ssize_t sch_generic_recv (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags)
{
        return read (DATA(desc->scheme_data)->ifd, buf, len);
}

ssize_t sch_generic_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags)
{
        return write (DATA(desc->scheme_data)->ofd, buf, len);
}

ssize_t sch_generic_sendsexp (SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags)
{
        ssize_t   ret;
        strbuf_t *sb;

        ret = 0;
        sb  = strbuf_new (SEAP_STRBUF_MAX);

        if (SEXP_sbprintf_t (sexp, sb) != 0)
                ret = -1;
        else
                ret = strbuf_write (sb, DATA(desc->scheme_data)->ofd);

        strbuf_free (sb);

        return (ret);
}

int sch_generic_close (SEAP_desc_t *desc, uint32_t flags)
{
        sch_genericdata_t *data;

        data = (sch_genericdata_t *)desc->scheme_data;

        if (data->ifd != -1)
                close(data->ifd);
        if (data->ofd != -1)
                close(data->ofd);
        sm_free(data);

        return (0);
}

int sch_generic_select (SEAP_desc_t *desc, int ev, uint16_t timeout, uint32_t flags)
{
        fd_set *wptr, *rptr;
        fd_set  fset;
        int fd;
        struct timeval *tv_ptr, tv;

        FD_ZERO(&fset);
        tv_ptr = NULL;
        wptr   = NULL;
        rptr   = NULL;

        switch (ev) {
        case SEAP_IO_EVREAD:
                fd = DATA(desc->scheme_data)->ifd;
                FD_SET(fd, &fset);
                rptr = &fset;
                break;
        case SEAP_IO_EVWRITE:
                fd = DATA(desc->scheme_data)->ofd;
                FD_SET(fd, &fset);
                wptr = &fset;
                break;
        default:
                abort ();
        }

        if (timeout > 0) {
                tv.tv_sec  = (time_t)timeout;
                tv.tv_usec = 0;
                tv_ptr = &tv;
        }

        _A(!(wptr == NULL && rptr == NULL));
        _A(!(wptr != NULL && rptr != NULL));

        switch (select (fd + 1, rptr, wptr, NULL, tv_ptr)) {
        case -1:
                protect_errno {
                        dI("FAIL: errno=%u, %s.", errno, strerror (errno));
                }
                return (-1);
        case  0:
                errno = ETIMEDOUT;
                return (-1);
        default:
                return (FD_ISSET(fd, &fset) ? 0 : -1);
        }
        /* NOTREACHED */
        return (-1);
}
