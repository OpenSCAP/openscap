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
#include "sha2.h"

#if defined(HAVE_NSS3)
#include <sechash.h>

static int crapi_sha2_fd (HASH_HashType algo, int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;

        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (*size >= HASH_ResultLen (algo), -1, errno = ENOBUFS;);
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
                        ctx    = HASH_Create (algo);

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
                        HASH_HashBuf (algo, (unsigned char *)dst, (unsigned char *)buffer, (unsigned int)buflen);
                        munmap (buffer, buflen);
                }
#endif /* _FILE_OFFSET_BITS == 32 */
        }
        return (0);
}

void *crapi_sha224_init (void *dst, void *size)
{
        return (NULL);
}

int crapi_sha224_update (void *ctxp, void *bptr, size_t blen)
{
        return (-1);
}

int crapi_sha224_fini (void *ctxp)
{
        return (-1);
}

void crapi_sha224_free (void *ctxp)
{
        return;
}

int crapi_sha224_fd (int fd, void *dst, size_t *size)
{
        return (-1);
}

void *crapi_sha256_init (void *dst, void *size)
{
        return (NULL);
}

int crapi_sha256_update (void *ctxp, void *bptr, size_t blen)
{
        return (-1);
}

int crapi_sha256_fini (void *ctxp)
{
        return (-1);
}

void crapi_sha256_free (void *ctxp)
{
        return;
}

int crapi_sha256_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (HASH_AlgSHA256, fd, dst, size);
}

void *crapi_sha384_init (void *dst, void *size)
{
        return (NULL);
}

int crapi_sha384_update (void *ctxp, void *bptr, size_t blen)
{
        return (-1);
}

int crapi_sha384_fini (void *ctxp)
{
        return (-1);
}

void crapi_sha384_free (void *ctxp)
{
        return;
}

int crapi_sha384_fd (int fd, void *dst, size_t *size)
{
        return (-1);
}

void *crapi_sha512_init (void *dst, void *size)
{
        return (NULL);
}

int crapi_sha512_update (void *ctxp, void *bptr, size_t blen)
{
        return (-1);
}

int crapi_sha512_fini (void *ctxp)
{
        return (-1);
}

void crapi_sha512_free (void *ctxp)
{
        return;
}

int crapi_sha512_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (HASH_AlgSHA512, fd, dst, size);
}

#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>

static int crapi_sha2_fd (int algo, int fd, void *dst, size_t *size)
{
        struct stat st;
        void   *buffer;
        size_t  buflen;

        assume_r (size != NULL, -1, errno = EFAULT;);
        assume_r (dst != NULL, -1, errno = EFAULT;);
        assume_r (*size >= gcry_md_get_algo_dlen (algo), -1, errno = ENOBUFS;);

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
                        gcry_md_open (&hd, algo, 0);

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

                        buffer = (void *)gcry_md_read (hd, algo);
                        memcpy (dst, buffer, gcry_md_get_algo_dlen (algo));
                        gcry_md_close (hd);
#if _FILE_OFFSET_BITS == 32
                } else {
			/* XXX: FIPS: Note that this function will abort the process if an unavailable algorithm is used. */
                        gcry_md_hash_buffer (algo, dst, (const void *)buffer, buflen);
                        munmap (buffer, buflen);
                }
#endif /* _FILE_OFFSET_BITS == 32 */
        }
        return (0);
}

struct crapi_sha2_ctx {
        gcry_md_hd_t ctx;
        void        *dst;
        void        *size;
};

static void *crapi_sha2_init(void *dst, void *size, int alg)
{
        struct crapi_sha2_ctx *ctx = oscap_talloc (struct crapi_sha2_ctx);

        if (gcry_md_open (&ctx->ctx, alg, 0) != 0) {
		free(ctx);
		return NULL;
	}

        ctx->dst  = dst;
        ctx->size = size;

        return (ctx);
}

static int crapi_sha2_update (void *ctxp, void *bptr, size_t blen)
{
        struct crapi_sha2_ctx *ctx = (struct crapi_sha2_ctx *)ctxp;

        gcry_md_write (ctx->ctx, (const void *)bptr, blen);
        return (0);
}

static int crapi_sha2_fini (void *ctxp, int alg)
{
        struct crapi_sha2_ctx *ctx = (struct crapi_sha2_ctx *)ctxp;
        void *buffer;

        gcry_md_final (ctx->ctx);
        buffer = (void *)gcry_md_read (ctx->ctx, alg);
        memcpy (ctx->dst, buffer, gcry_md_get_algo_dlen (alg));
        gcry_md_close (ctx->ctx);
        free(ctx);

        return (0);
}

static void crapi_sha2_free (void *ctxp)
{
        return;
}

void *crapi_sha224_init (void *dst, void *size)
{
        return crapi_sha2_init(dst, size, GCRY_MD_SHA224);
}

int crapi_sha224_update (void *ctxp, void *bptr, size_t blen)
{
        return crapi_sha2_update(ctxp, bptr, blen);
}

int crapi_sha224_fini (void *ctxp)
{
        return crapi_sha2_fini(ctxp, GCRY_MD_SHA224);
}

void crapi_sha224_free (void *ctxp)
{
        crapi_sha2_free(ctxp);
}

int crapi_sha224_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (GCRY_MD_SHA256, fd, dst, size);
}

void *crapi_sha256_init (void *dst, void *size)
{
        return crapi_sha2_init(dst, size, GCRY_MD_SHA256);
}

int crapi_sha256_update (void *ctxp, void *bptr, size_t blen)
{
        return crapi_sha2_update(ctxp, bptr, blen);
}

int crapi_sha256_fini (void *ctxp)
{
        return crapi_sha2_fini(ctxp, GCRY_MD_SHA256);
}

void crapi_sha256_free (void *ctxp)
{
        crapi_sha2_free(ctxp);
}

int crapi_sha256_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (GCRY_MD_SHA256, fd, dst, size);
}

void *crapi_sha384_init (void *dst, void *size)
{
        return crapi_sha2_init(dst, size, GCRY_MD_SHA384);
}

int crapi_sha384_update (void *ctxp, void *bptr, size_t blen)
{
        return crapi_sha2_update(ctxp, bptr, blen);
}

int crapi_sha384_fini (void *ctxp)
{
        return crapi_sha2_fini(ctxp, GCRY_MD_SHA384);
}

void crapi_sha384_free (void *ctxp)
{
        crapi_sha2_free(ctxp);
}

int crapi_sha384_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (GCRY_MD_SHA384, fd, dst, size);
}

void *crapi_sha512_init (void *dst, void *size)
{
        return crapi_sha2_init(dst, size, GCRY_MD_SHA512);
}

int crapi_sha512_update (void *ctxp, void *bptr, size_t blen)
{
        return crapi_sha2_update(ctxp, bptr, blen);
}

int crapi_sha512_fini (void *ctxp)
{
        return crapi_sha2_fini(ctxp, GCRY_MD_SHA512);
}

void crapi_sha512_free (void *ctxp)
{
        crapi_sha2_free(ctxp);
}

int crapi_sha512_fd (int fd, void *dst, size_t *size)
{
        return crapi_sha2_fd (GCRY_MD_SHA512, fd, dst, size);
}
#else
# error "No crypto library available!"
#endif
