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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assume.h>
#include <errno.h>
#include <unistd.h>

#include "crapi.h"
#include "rmd160.h"

#if defined(HAVE_NSS3)
#include <sechash.h>

void *crapi_rmd160_init (void *dst, void *size)
{
        return (NULL);
}

int crapi_rmd160_update (void *ctxp, void *bptr, size_t blen)
{
        return (-1);
}

int crapi_rmd160_fini (void *ctxp)
{
        return (-1);
}

void crapi_rmd160_free (void *ctxp)
{
        return;
}

int crapi_rmd160_fd (int fd, void *dst, size_t *size)
{
        errno = EOPNOTSUPP;
        return (-1);
}
#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>

struct crapi_rmd160_ctx {
        gcry_md_hd_t ctx;
        void        *dst;
        void        *size;
};

#define CRAPI_RMD160DST_LEN gcry_md_get_algo_dlen (GCRY_MD_RMD160)

void *crapi_rmd160_init (void *dst, void *size)
{
        struct crapi_rmd160_ctx *ctx = oscap_talloc (struct crapi_rmd160_ctx);

        if (gcry_md_open (&ctx->ctx, GCRY_MD_RMD160, 0) != 0) {
		free(ctx);
		return NULL;
	}

        ctx->dst  = dst;
        ctx->size = size;

        return (ctx);
}

int crapi_rmd160_update (void *ctxp, void *bptr, size_t blen)
{
        struct crapi_rmd160_ctx *ctx = (struct crapi_rmd160_ctx *)ctxp;

        gcry_md_write (ctx->ctx, (const void *)bptr, blen);
        return (0);
}

int crapi_rmd160_fini (void *ctxp)
{
        struct crapi_rmd160_ctx *ctx = (struct crapi_rmd160_ctx *)ctxp;
        void *buffer;

        gcry_md_final (ctx->ctx);
        buffer = (void *)gcry_md_read (ctx->ctx, GCRY_MD_RMD160);
        memcpy (ctx->dst, buffer, gcry_md_get_algo_dlen (GCRY_MD_RMD160));
        gcry_md_close (ctx->ctx);
        free(ctx);

        return (0);
}

void crapi_rmd160_free (void *ctxp)
{
        struct crapi_rmd160_ctx *ctx = (struct crapi_rmd160_ctx *)ctxp;

        gcry_md_close (ctx->ctx);
        free(ctx);

        return;
}

int crapi_rmd160_fd (int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;
        
        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (dst != NULL, -1, errno = EFAULT;);
        assume_r (*size >= gcry_md_get_algo_dlen (GCRY_MD_RMD160), -1, errno = ENOBUFS;);
        
        if (fstat (fd, &st) != 0)
                return (-1);
        else {
#if _FILE_OFFSET_BITS == 32
                buflen = st.st_size;
# if defined(__FreeBSD__)
                buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED | MAP_NOCORE, fd, 0);
# else
                buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED, fd, 0);        
# endif        
                if (buffer == NULL) {
#endif /* _FILE_OFFSET_BITS == 32 */
                        uint8_t _buffer[CRAPI_IO_BUFSZ];
                        gcry_md_hd_t hd;
                        ssize_t ret;
                
                        buffer = _buffer;
                        gcry_md_open (&hd, GCRY_MD_RMD160, 0);
                
                        while ((ret = read (fd, buffer, sizeof _buffer)) == sizeof _buffer)
                                gcry_md_write (hd, (const void *)buffer, sizeof _buffer);
                
                        switch (ret) {
                        case 0:
                                break;
                        case -1:
                                return (-1);
                        default:
                                assume_r (ret > 0, -1, gcry_md_close (hd););
                                gcry_md_write (hd, (const void *)buffer, (size_t)ret);
                        }
                
                        gcry_md_final (hd);
                
                        buffer = (void *)gcry_md_read (hd, GCRY_MD_RMD160);
                        memcpy (dst, buffer, gcry_md_get_algo_dlen (GCRY_MD_RMD160));
                        gcry_md_close (hd);
#if _FILE_OFFSET_BITS == 32
                } else {
                        gcry_md_hash_buffer (GCRY_MD_RMD160, dst, (const void *)buffer, buflen);
                        munmap (buffer, buflen);
                }
#endif /* _FILE_OFFSET_BITS == 32 */
        }
        return (0);
}
#else
# error "No crypto library available!"
#endif
