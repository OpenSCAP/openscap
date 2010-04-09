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
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assume.h>
#include <errno.h>
#include <unistd.h>
#include <alloc.h>
#include <config.h>
#include "crapi.h"
#include "sha1.h"

#if defined(HAVE_NSS3)
#include <sechash.h>

struct crapi_sha1_ctx {
        HASHContext *ctx;
        void        *dst;
        size_t      *size;
};

void *crapi_sha1_init (void *dst, void *size)
{
        struct crapi_sha1_ctx *ctx = oscap_talloc (struct crapi_sha1_ctx);

        ctx->ctx  = HASH_Create (HASH_AlgSHA1);
        ctx->dst  = dst;
        ctx->size = size;

        return (ctx);
}

int crapi_sha1_update (void *ctxp, void *bptr, size_t blen)
{
        struct crapi_sha1_ctx *ctx = (struct crapi_sha1_ctx *)ctxp;
        
        HASH_Update (ctx->ctx, (const unsigned char *)bptr, (unsigned int)blen);
        return (0);
}

int crapi_sha1_fini (void *ctxp)
{
        struct crapi_sha1_ctx *ctx = (struct crapi_sha1_ctx *)ctxp;

        HASH_End (ctx->ctx, ctx->dst, (unsigned int *)ctx->size, *ctx->size);
        HASH_Destroy (ctx->ctx);
        oscap_free (ctx);

        return (0);
}

void crapi_sha1_free (void *ctxp)
{
        struct crapi_sha1_ctx *ctx = (struct crapi_sha1_ctx *)ctxp;

        HASH_Destroy (ctx->ctx);
        oscap_free (ctx);
        
        return;
}

int crapi_sha1_fd (int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;
        
        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (*size >= SHA1_LENGTH, -1, errno = ENOBUFS;);
        assume_r (dst != NULL, -1, errno = EFAULT;);
        
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
                        HASHContext *ctx;
                        ssize_t ret;
                        
                        buffer = _buffer;
                        ctx    = HASH_Create (HASH_AlgSHA1);
                        
                        if (ctx == NULL)
                                return (-1);
                        
                        while ((ret = read (fd, buffer, sizeof _buffer)) == sizeof _buffer)
                                HASH_Update (ctx, (const unsigned char *)buffer, (unsigned int) sizeof _buffer);
                        
                        switch (ret) {
                        case 0:
                                break;
                        case -1:
                                return (-1);
                        default:
                                assume_r (ret > 0, -1, HASH_Destroy (ctx););
                                HASH_Update (ctx, (const unsigned char *)buffer, (unsigned int) ret);
                        }
                        
                        HASH_End (ctx, dst, (unsigned int *)size, *size);
                        HASH_Destroy (ctx);
#if _FILE_OFFSET_BITS == 32
                } else {
                        HASH_HashBuf (HASH_AlgSHA1, (unsigned char *)dst, (unsigned char *)buffer, (unsigned int)buflen);
                        munmap (buffer, buflen);
                }
#endif /* _FILE_OFFSET_BITS == 32 */
        }
        return (0);
}
#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>

struct crapi_sha1_ctx {
        gcry_md_hd_t ctx;
        void        *dst;
        void        *size;
};

void *crapi_sha1_init (void *dst, void *size)
{
        struct crapi_sha1_ctx *ctx = oscap_talloc (struct crapi_sha1_ctx);

        gcry_md_open (&ctx->ctx, GCRY_MD_SHA1, 0);
        ctx->dst  = dst;
        ctx->size = size;

        return (ctx);
}

int crapi_sha1_update (void *ctxp, void *bptr, size_t blen)
{
        struct crapi_sha1_ctx *ctx = (struct crapi_sha1_ctx *)ctxp;

        gcry_md_write (ctx->ctx, (const void *)bptr, blen);
        return (0);
}

int crapi_sha1_fini (void *ctxp)
{
        struct crapi_sha1_ctx *ctx = (struct crapi_sha1_ctx *)ctxp;
        void *buffer;

        gcry_md_final (ctx->ctx);
        buffer = (void *)gcry_md_read (ctx->ctx, GCRY_MD_SHA1);
        memcpy (ctx->dst, buffer, gcry_md_get_algo_dlen (GCRY_MD_SHA1));
        gcry_md_close (ctx->ctx);
        
        return (0);
}

void crapi_sha1_free (void *ctxp)
{
        struct crapi_sha1_ctx *ctx = (struct crapi_sha1_ctx *)ctxp;
        
        gcry_md_close (ctx->ctx);
        return;
}

int crapi_sha1_fd (int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;
        
        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (dst != NULL, -1, errno = EFAULT;);
        assume_r (*size >= gcry_md_get_algo_dlen (GCRY_MD_SHA1), -1, errno = ENOBUFS;);
        
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
                        gcry_md_open (&hd, GCRY_MD_SHA1, 0);
                        
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
                        
                        buffer = (void *)gcry_md_read (hd, GCRY_MD_SHA1);
                        memcpy (dst, buffer, gcry_md_get_algo_dlen (GCRY_MD_SHA1));
                        gcry_md_close (hd);
#if _FILE_OFFSET_BITS == 32
                } else {
                        gcry_md_hash_buffer (GCRY_MD_SHA1, dst, (const void *)buffer, buflen);
                        
                        if (munmap (buffer, buflen) != 0)
                                return (-1);
                }
#endif /* _FILE_OFFSET_BITS == 32 */
        }
        
        return (0);
}
#else
# error "No crypto library available!"
#endif
