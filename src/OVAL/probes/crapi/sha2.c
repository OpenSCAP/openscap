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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
#include "sha2.h"

#if defined(HAVE_NSS3)
#include <sechash.h>
#define CRAPI_ALGO_SHA224 HASH_AlgSHA224
#define CRAPI_ALGO_SHA256 HASH_AlgSHA256
#define CRAPI_ALGO_SHA384 HASH_AlgSHA384
#define CRAPI_ALGO_SHA512 HASH_AlgSHA512
#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>
#define CRAPI_ALGO_SHA224 GCRY_MD_SHA224
#define CRAPI_ALGO_SHA256 GCRY_MD_SHA256
#define CRAPI_ALGO_SHA384 GCRY_MD_SHA384
#define CRAPI_ALGO_SHA512 GCRY_MD_SHA512
#else
#error "No crypto library available!"
#endif

static int crapi_sha2_fd (int algo, int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;

	if (size == NULL || dst == NULL) {
		errno = EFAULT;
		return -1;
	}
#if defined(HAVE_NSS3)
	if (*size < HASH_ResultLen(algo)) {
#elif defined(HAVE_GCRYPT)
	if (*size < gcry_md_get_algo_dlen(algo)) {
#endif
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
                        ssize_t ret;

                        buffer = _buffer;
#if defined(HAVE_NSS3)
                        HASHContext *ctx;
                        ctx    = HASH_Create (algo);

                        if (ctx == NULL)
                                return (-1);
#elif defined(HAVE_GCRYPT)
                        gcry_md_hd_t hd;
                        gcry_md_open (&hd, algo, 0);
#endif

                        while ((ret = read (fd, buffer, sizeof _buffer)) == sizeof _buffer)
#if defined(HAVE_NSS3)
                                HASH_Update (ctx, (const unsigned char *)buffer, (unsigned int) sizeof _buffer);
#elif defined(HAVE_GCRYPT)
                                gcry_md_write (hd, (const void *)buffer, sizeof _buffer);
#endif

                        switch (ret) {
                        case 0:
                                break;
                        case -1:
                                return (-1);
                        default:
				if (ret <= 0) {
#if defined(HAVE_NSS3)
					HASH_Destroy(ctx);
#elif defined(HAVE_GCRYPT)
					gcry_md_close(hd);
#endif
					return -1;
				}
#if defined(HAVE_NSS3)
                                HASH_Update (ctx, (const unsigned char *)buffer, (unsigned int) ret);
#elif defined(HAVE_GCRYPT)
                                gcry_md_write (hd, (const void *)buffer, (size_t)ret);
#endif
                        }

#if defined(HAVE_NSS3)
                        HASH_End (ctx, dst, (unsigned int *)size, *size);
                        HASH_Destroy (ctx);
#elif defined(HAVE_GCRYPT)
                        gcry_md_final (hd);

                        buffer = (void *)gcry_md_read (hd, algo);
                        memcpy (dst, buffer, gcry_md_get_algo_dlen (algo));
                        gcry_md_close (hd);
#endif
#if _FILE_OFFSET_BITS == 32
                } else {
#if defined(HAVE_NSS3)
                        HASH_HashBuf (algo, (unsigned char *)dst, (unsigned char *)buffer, (unsigned int)buflen);
#elif defined(HAVE_GCRYPT)
                        gcry_md_hash_buffer (algo, dst, (const void *)buffer, buflen);
#endif
                        munmap (buffer, buflen);
                }
#endif /* _FILE_OFFSET_BITS == 32 */
        }
        return (0);
}

struct crapi_sha2_ctx {
#if defined(HAVE_NSS3)
        HASHContext *ctx;
#elif defined(HAVE_GCRYPT)
        gcry_md_hd_t ctx;
#endif
        void        *dst;
        size_t      *size;
};

static void *crapi_sha2_init (void *dst, void *size, int alg)
{
        struct crapi_sha2_ctx *ctx = malloc(sizeof(struct crapi_sha2_ctx));

#if defined(HAVE_NSS3)
        ctx->ctx  = HASH_Create (alg);
#elif defined(HAVE_GCRYPT)
        if (gcry_md_open (&ctx->ctx, alg, 0) != 0) {
		free(ctx);
		return NULL;
	}
#endif
        ctx->dst  = dst;
        ctx->size = size;

#if defined(HAVE_NSS3)
        if (ctx->ctx != NULL) {
                HASH_Begin (ctx->ctx);
        } else {
                free (ctx);
                ctx = NULL;
        }
#endif

        return (ctx);
}

static int crapi_sha2_update (void *ctxp, void *bptr, size_t blen)
{
        struct crapi_sha2_ctx *ctx = (struct crapi_sha2_ctx *)ctxp;

#if defined(HAVE_NSS3)
        HASH_Update (ctx->ctx, (const unsigned char *)bptr, (unsigned int)blen);
#elif defined(HAVE_GCRYPT)
        gcry_md_write (ctx->ctx, (const void *)bptr, blen);
#endif
        return (0);
}

static int crapi_sha2_fini (void *ctxp, int alg)
{
        struct crapi_sha2_ctx *ctx = (struct crapi_sha2_ctx *)ctxp;

#if defined(HAVE_NSS3)
        HASH_End (ctx->ctx, ctx->dst, (unsigned int *)ctx->size, *ctx->size);
        HASH_Destroy (ctx->ctx);
#elif defined(HAVE_GCRYPT)
        void *buffer;

        gcry_md_final (ctx->ctx);
        buffer = (void *)gcry_md_read (ctx->ctx, alg);
        memcpy (ctx->dst, buffer, gcry_md_get_algo_dlen (alg));
        gcry_md_close (ctx->ctx);
#endif
        free (ctx);

        return (0);
}

static void crapi_sha2_free (void *ctxp)
{
#if defined(HAVE_NSS3)
        struct crapi_sha2_ctx *ctx = (struct crapi_sha2_ctx *)ctxp;

        HASH_Destroy (ctx->ctx);
        free (ctx);
#endif

        return;
}

void *crapi_sha224_init (void *dst, void *size)
{
        return crapi_sha2_init(dst, size, CRAPI_ALGO_SHA224);
}

int crapi_sha224_update (void *ctxp, void *bptr, size_t blen)
{
        return crapi_sha2_update(ctxp, bptr, blen);
}

int crapi_sha224_fini (void *ctxp)
{
        return crapi_sha2_fini(ctxp,  CRAPI_ALGO_SHA224);
}

void crapi_sha224_free (void *ctxp)
{
        crapi_sha2_free(ctxp);
}

int crapi_sha224_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (CRAPI_ALGO_SHA224, fd, dst, size);
}

void *crapi_sha256_init (void *dst, void *size)
{
        return crapi_sha2_init(dst, size, CRAPI_ALGO_SHA256);
}

int crapi_sha256_update (void *ctxp, void *bptr, size_t blen)
{
        return crapi_sha2_update(ctxp, bptr, blen);
}

int crapi_sha256_fini (void *ctxp)
{
        return crapi_sha2_fini(ctxp,  CRAPI_ALGO_SHA256);
}

void crapi_sha256_free (void *ctxp)
{
        crapi_sha2_free(ctxp);
}

int crapi_sha256_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (CRAPI_ALGO_SHA256, fd, dst, size);
}

void *crapi_sha384_init (void *dst, void *size)
{
        return crapi_sha2_init(dst, size, CRAPI_ALGO_SHA384);
}

int crapi_sha384_update (void *ctxp, void *bptr, size_t blen)
{
        return crapi_sha2_update(ctxp, bptr, blen);
}

int crapi_sha384_fini (void *ctxp)
{
        return crapi_sha2_fini(ctxp, CRAPI_ALGO_SHA384);
}

void crapi_sha384_free (void *ctxp)
{
        crapi_sha2_free(ctxp);
}

int crapi_sha384_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (CRAPI_ALGO_SHA384, fd, dst, size);
}

void *crapi_sha512_init (void *dst, void *size)
{
        return crapi_sha2_init(dst, size, CRAPI_ALGO_SHA512);
}

int crapi_sha512_update (void *ctxp, void *bptr, size_t blen)
{
        return crapi_sha2_update(ctxp, bptr, blen);
}

int crapi_sha512_fini (void *ctxp)
{
        return crapi_sha2_fini(ctxp, CRAPI_ALGO_SHA512);
}

void crapi_sha512_free (void *ctxp)
{
        crapi_sha2_free(ctxp);
}

int crapi_sha512_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (CRAPI_ALGO_SHA512, fd, dst, size);
}
