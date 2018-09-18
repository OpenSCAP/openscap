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
#include <errno.h>
#include <unistd.h>
#include "crapi.h"
#include "sha1.h"

#if defined(HAVE_NSS3)
#include <sechash.h>

#define CRAPI_SHA1DST_LEN SHA1_LENGTH

struct crapi_sha1_ctx {
        HASHContext *ctx;
        void        *dst;
        size_t      *size;
};

void *crapi_sha1_init (void *dst, void *size)
{
        struct crapi_sha1_ctx *ctx = malloc(sizeof(struct crapi_sha1_ctx));

        ctx->ctx  = HASH_Create (HASH_AlgSHA1);
        ctx->dst  = dst;
        ctx->size = size;

        if (ctx->ctx != NULL) {
                HASH_Begin (ctx->ctx);
        } else {
                free (ctx);
                ctx = NULL;
        }

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
        free (ctx);

        return (0);
}

void crapi_sha1_free (void *ctxp)
{
        struct crapi_sha1_ctx *ctx = (struct crapi_sha1_ctx *)ctxp;

        HASH_Destroy (ctx->ctx);
        free (ctx);
        
        return;
}
#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>

#define CRAPI_SHA1DST_LEN gcry_md_get_algo_dlen (GCRY_MD_SHA1)

struct crapi_sha1_ctx {
        gcry_md_hd_t ctx;
        void        *dst;
        void        *size;
};

void *crapi_sha1_init (void *dst, void *size)
{
        struct crapi_sha1_ctx *ctx = malloc(sizeof(struct crapi_sha1_ctx));

        if (gcry_md_open (&ctx->ctx, GCRY_MD_SHA1, 0) != 0) {
		free(ctx);
		return NULL;
	}

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
        free(ctx);

        return (0);
}

void crapi_sha1_free (void *ctxp)
{
        struct crapi_sha1_ctx *ctx = (struct crapi_sha1_ctx *)ctxp;

        gcry_md_close (ctx->ctx);
        free(ctx);

        return;
}
#else
# error "No crypto library available!"
#endif

int crapi_sha1_fd (int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;

	if (size == NULL || dst == NULL) {
		errno = EFAULT;
		return -1;
	}
	if (*size < CRAPI_SHA1DST_LEN) {
		errno = ENOBUFS;
		return -1;
	}

        if (fstat (fd, &st) != 0)
                return (-1);
        else {
#if _FILE_OFFSET_BITS == 32
                buflen = st.st_size;
# if defined(OS_FREEBSD)
                buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED | MAP_NOCORE, fd, 0);
# else
                buffer = mmap (NULL, buflen, PROT_READ, MAP_SHARED, fd, 0);        
# endif        
                if (buffer == NULL) {
#endif /* _FILE_OFFSET_BITS == 32 */
                        uint8_t _buffer[CRAPI_IO_BUFSZ];
                        void   *ctx;
                        ssize_t ret;
                        
                        buffer = _buffer;
                        ctx    = crapi_sha1_init (dst, size);
                        
                        while ((ret = read (fd, buffer, sizeof _buffer)) == sizeof _buffer)
                                crapi_sha1_update (ctx, buffer, sizeof _buffer);
                        
                        switch (ret) {
                        case 0:
                                break;
                        case -1:
                                return (-1);
                        default:
				if (ret <= 0) {
					crapi_sha1_free(ctx);
					return -1;
				}
                                crapi_sha1_update (ctx, buffer, (size_t) ret);
                        }

                        crapi_sha1_fini (ctx);
#if _FILE_OFFSET_BITS == 32
# if defined(HAVE_NSS3)
                } else {
                        SECStatus ret;
                        
                        ret = HASH_HashBuf (HASH_AlgSHA1, (unsigned char *)dst, (unsigned char *)buffer, (unsigned int)buflen);
                        munmap (buffer, buflen);

                        return (ret == SECSuccess ? 0 : -1);
                }
# elif defined(HAVE_GCRYPT)
                } else {
		/* XXX: FIPS: Note that this function will abort the process if an unavailable algorithm is used. */
                        gcry_md_hash_buffer (GCRY_MD_SHA1, dst, (const void *)buffer, buflen);
                        
                        if (munmap (buffer, buflen) != 0)
                                return (-1);
                }
# endif
#endif /* _FILE_OFFSET_BITS == 32 */
        }
        
        return (0);
}
