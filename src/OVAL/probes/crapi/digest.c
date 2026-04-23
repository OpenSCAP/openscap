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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "crapi.h"
#include "digest.h"

#if defined(HAVE_NSS3)
#include <sechash.h>
#elif defined(HAVE_GCRYPT)
#include <gcrypt.h>
#elif defined(HAVE_OPENSSL_CRYPTO)
#include <openssl/evp.h>
#include <openssl/opensslv.h>
#else
#error "No crypto library available!"
#endif

/*
 * Compatibility shim for OpenSSL < 1.1.0, which used EVP_MD_CTX_create() and
 * EVP_MD_CTX_destroy() instead of the EVP_MD_CTX_new() / EVP_MD_CTX_free()
 * names introduced in 1.1.0.
 */
#if defined(HAVE_OPENSSL_CRYPTO) && OPENSSL_VERSION_NUMBER < 0x10100000L
# define EVP_MD_CTX_new()     EVP_MD_CTX_create()
# define EVP_MD_CTX_free(c)   EVP_MD_CTX_destroy(c)
#endif

#if defined(HAVE_NSS3)
static int crapi_alg_t_to_lib_arg(crapi_alg_t alg)
{
	switch (alg) {
#ifdef OPENSCAP_ENABLE_MD5
	case CRAPI_DIGEST_MD5:
		return HASH_AlgMD5;
#endif
#ifdef OPENSCAP_ENABLE_SHA1
	case CRAPI_DIGEST_SHA1:
		return HASH_AlgSHA1;
#endif
	case CRAPI_DIGEST_SHA224:
		return HASH_AlgSHA224;
	case CRAPI_DIGEST_SHA256:
		return HASH_AlgSHA256;
	case CRAPI_DIGEST_SHA384:
		return HASH_AlgSHA384;
	case CRAPI_DIGEST_SHA512:
		return HASH_AlgSHA512;
	default:
		return -1;
	}
}
#elif defined(HAVE_GCRYPT)
static int crapi_alg_t_to_lib_arg(crapi_alg_t alg)
{
	switch (alg) {
#ifdef OPENSCAP_ENABLE_MD5
	case CRAPI_DIGEST_MD5:
		return GCRY_MD_MD5;
#endif
#ifdef OPENSCAP_ENABLE_SHA1
	case CRAPI_DIGEST_SHA1:
		return GCRY_MD_SHA1;
#endif
	case CRAPI_DIGEST_SHA224:
		return GCRY_MD_SHA224;
	case CRAPI_DIGEST_SHA256:
		return GCRY_MD_SHA256;
	case CRAPI_DIGEST_SHA384:
		return GCRY_MD_SHA384;
	case CRAPI_DIGEST_SHA512:
		return GCRY_MD_SHA512;
	case CRAPI_DIGEST_RMD160:
		return GCRY_MD_RMD160;
	default:
		return -1;
	}
}
#elif defined(HAVE_OPENSSL_CRYPTO)
static const EVP_MD *crapi_alg_t_to_evp_md(crapi_alg_t alg)
{
	switch (alg) {
#ifdef OPENSCAP_ENABLE_MD5
	case CRAPI_DIGEST_MD5:
		return EVP_md5();
#endif
#ifdef OPENSCAP_ENABLE_SHA1
	case CRAPI_DIGEST_SHA1:
		return EVP_sha1();
#endif
	case CRAPI_DIGEST_SHA224:
		return EVP_sha224();
	case CRAPI_DIGEST_SHA256:
		return EVP_sha256();
	case CRAPI_DIGEST_SHA384:
		return EVP_sha384();
	case CRAPI_DIGEST_SHA512:
		return EVP_sha512();
	default:
		return NULL;
	}
}
#endif

#if defined(HAVE_OPENSSL_CRYPTO)
/* Read fd and compute its digest using the streaming EVP API. */
static int crapi_digest_fd_stream(int fd, const EVP_MD *evp_md, void *dst, size_t *size)
{
	uint8_t buf[CRAPI_IO_BUFSZ];
	ssize_t ret;

	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	if (ctx == NULL)
		return -1;
	if (EVP_DigestInit_ex(ctx, evp_md, NULL) != 1) {
		EVP_MD_CTX_free(ctx);
		return -1;
	}
	while ((ret = read(fd, buf, sizeof buf)) == sizeof buf) {
		if (EVP_DigestUpdate(ctx, (const void *)buf, sizeof buf) != 1) {
			EVP_MD_CTX_free(ctx);
			return -1;
		}
	}
	switch (ret) {
	case 0:
		break;
	case -1:
		EVP_MD_CTX_free(ctx);
		return -1;
	default:
		if (ret <= 0) {
			EVP_MD_CTX_free(ctx);
			return -1;
		}
		if (EVP_DigestUpdate(ctx, (const void *)buf, (size_t)ret) != 1) {
			EVP_MD_CTX_free(ctx);
			return -1;
		}
	}
	unsigned int md_len = (unsigned int)*size;
	if (EVP_DigestFinal_ex(ctx, (unsigned char *)dst, &md_len) != 1) {
		EVP_MD_CTX_free(ctx);
		return -1;
	}
	*size = (size_t)md_len;
	EVP_MD_CTX_free(ctx);
	return 0;
}
#endif /* HAVE_OPENSSL_CRYPTO */

int crapi_digest_fd(int fd, crapi_alg_t alg, void *dst, size_t *size)
{
	struct stat st;
	void *buffer;
	size_t buflen;

	if (size == NULL || dst == NULL) {
		errno = EFAULT;
		return -1;
	}

	/*
	 * Resolve the algorithm and verify the output buffer is large enough
	 * to hold the digest.
	 */
#if defined(HAVE_OPENSSL_CRYPTO)
	const EVP_MD *evp_md = crapi_alg_t_to_evp_md(alg);
	if (evp_md == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (*size < (size_t)EVP_MD_size(evp_md)) {
		errno = ENOBUFS;
		return -1;
	}
#else
	int lib_alg = crapi_alg_t_to_lib_arg(alg);
#if defined(HAVE_NSS3)
	if (*size < HASH_ResultLen(lib_alg)) {
#elif defined(HAVE_GCRYPT)
	if (*size < gcry_md_get_algo_dlen(lib_alg)) {
#endif
		errno = ENOBUFS;
		return -1;
	}
#endif /* HAVE_OPENSSL_CRYPTO */

	if (fstat (fd, &st) != 0)
		return (-1);
	else {
#if _FILE_OFFSET_BITS == 32
		buflen = st.st_size;
# if defined(OS_FREEBSD)
		buffer = mmap(NULL, buflen, PROT_READ, MAP_SHARED | MAP_NOCORE, fd, 0);
# else
		buffer = mmap(NULL, buflen, PROT_READ, MAP_SHARED, fd, 0);
# endif
		if (buffer == NULL) {
#endif /* _FILE_OFFSET_BITS == 32 */
#if defined(HAVE_OPENSSL_CRYPTO)
			if (crapi_digest_fd_stream(fd, evp_md, dst, size) != 0)
				return -1;
#else
			uint8_t _buffer[CRAPI_IO_BUFSZ];
			ssize_t ret;

			buffer = _buffer;
#if defined(HAVE_NSS3)
			HASHContext *ctx = HASH_Create (lib_alg);

			if (ctx == NULL)
				return (-1);
#elif defined(HAVE_GCRYPT)
			gcry_md_hd_t hd;
			gcry_md_open (&hd, lib_alg, 0);
#endif

			while ((ret = read(fd, buffer, sizeof _buffer)) == sizeof _buffer)
#if defined(HAVE_NSS3)
				HASH_Update(ctx, (const unsigned char *)buffer, (unsigned int) sizeof _buffer);
#elif defined(HAVE_GCRYPT)
				gcry_md_write(hd, (const void *)buffer, sizeof _buffer);
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
				HASH_Update(ctx, (const unsigned char *)buffer, (unsigned int) ret);
#elif defined(HAVE_GCRYPT)
				gcry_md_write(hd, (const void *)buffer, (size_t)ret);
#endif
			}

#if defined(HAVE_NSS3)
			HASH_End(ctx, dst, (unsigned int *)size, *size);
			HASH_Destroy(ctx);
#elif defined(HAVE_GCRYPT)
			gcry_md_final(hd);

			buffer = (void *)gcry_md_read(hd, lib_alg);
			memcpy (dst, buffer, gcry_md_get_algo_dlen(lib_alg));
			gcry_md_close (hd);
#endif
#endif /* !HAVE_OPENSSL_CRYPTO */

#if _FILE_OFFSET_BITS == 32
		} else {
#if defined(HAVE_OPENSSL_CRYPTO)
			unsigned int md_len = (unsigned int)*size;
			if (EVP_Digest(buffer, buflen, (unsigned char *)dst, &md_len, evp_md, NULL) != 1) {
				munmap(buffer, buflen);
				return -1;
			}
			*size = (size_t)md_len;
#elif defined(HAVE_NSS3)
			HASH_HashBuf(lib_alg, (unsigned char *)dst, (unsigned char *)buffer, (unsigned int)buflen);
#elif defined(HAVE_GCRYPT)
			gcry_md_hash_buffer(lib_alg, dst, (const void *)buffer, buflen);
#endif
			munmap(buffer, buflen);
		}
#endif /* _FILE_OFFSET_BITS == 32 */
	}
	return (0);
}

struct crapi_digest_ctx {
#if defined(HAVE_NSS3)
	HASHContext *ctx;
#elif defined(HAVE_GCRYPT)
	gcry_md_hd_t ctx;
#elif defined(HAVE_OPENSSL_CRYPTO)
	EVP_MD_CTX *ctx;
#endif
	void *dst;
	size_t *size;
};

struct digest_ctbl_t {
	struct crapi_digest_ctx *ctx;
	crapi_alg_t alg;
};

static void *crapi_digest_init (void *dst, void *size, crapi_alg_t alg)
{
	struct crapi_digest_ctx *ctx = malloc(sizeof(struct crapi_digest_ctx));

#if defined(HAVE_NSS3) || defined(HAVE_GCRYPT)
	int lib_alg = crapi_alg_t_to_lib_arg(alg);
#endif
#if defined(HAVE_NSS3)
	ctx->ctx  = HASH_Create(lib_alg);
#elif defined(HAVE_GCRYPT)
	if (gcry_md_open(&ctx->ctx, lib_alg, 0) != 0) {
		free(ctx);
		return NULL;
	}
#elif defined(HAVE_OPENSSL_CRYPTO)
	if (ctx == NULL)
		return NULL;
	const EVP_MD *evp_md = crapi_alg_t_to_evp_md(alg);
	if (evp_md == NULL) {
		free(ctx);
		errno = EINVAL;
		return NULL;
	}
	ctx->ctx = EVP_MD_CTX_new();
	if (ctx->ctx == NULL) {
		free(ctx);
		return NULL;
	}
	if (EVP_DigestInit_ex(ctx->ctx, evp_md, NULL) != 1) {
		EVP_MD_CTX_free(ctx->ctx);
		free(ctx);
		return NULL;
	}
#endif
	ctx->dst  = dst;
	ctx->size = size;

#if defined(HAVE_NSS3)
	if (ctx->ctx != NULL) {
		HASH_Begin(ctx->ctx);
	} else {
		free(ctx);
		ctx = NULL;
	}
#endif

	return (ctx);
}

static int crapi_digest_update(struct crapi_digest_ctx *ctx, void *bptr, size_t blen)
{
#if defined(HAVE_NSS3)
	HASH_Update(ctx->ctx, (const unsigned char *)bptr, (unsigned int)blen);
#elif defined(HAVE_GCRYPT)
	gcry_md_write(ctx->ctx, (const void *)bptr, blen);
#elif defined(HAVE_OPENSSL_CRYPTO)
	if (EVP_DigestUpdate(ctx->ctx, (const void *)bptr, blen) != 1)
		return -1;
#endif
	return (0);
}

static int crapi_digest_fini(struct crapi_digest_ctx *ctx, crapi_alg_t alg)
{
#if defined(HAVE_NSS3)
	unsigned int result_len;
	HASH_End(ctx->ctx, ctx->dst, &result_len, *ctx->size);
	*ctx->size = result_len;
	HASH_Destroy (ctx->ctx);
#elif defined(HAVE_GCRYPT)
	void *buffer;

	gcry_md_final(ctx->ctx);
	int lib_alg = crapi_alg_t_to_lib_arg(alg);
	buffer = (void *)gcry_md_read(ctx->ctx, lib_alg);
	memcpy(ctx->dst, buffer, gcry_md_get_algo_dlen(lib_alg));
	gcry_md_close(ctx->ctx);
#elif defined(HAVE_OPENSSL_CRYPTO)
	unsigned int md_len = (unsigned int)*ctx->size;
	if (EVP_DigestFinal_ex(ctx->ctx, (unsigned char *)ctx->dst, &md_len) != 1) {
		EVP_MD_CTX_free(ctx->ctx);
		free(ctx);
		return -1;
	}
	*ctx->size = (size_t)md_len;
	EVP_MD_CTX_free(ctx->ctx);
#endif
	free (ctx);

	return (0);
}

static void crapi_digest_free(struct crapi_digest_ctx *ctx)
{
#if defined(HAVE_NSS3)
	HASH_Destroy(ctx->ctx);
	free(ctx);
#elif defined(HAVE_OPENSSL_CRYPTO)
	EVP_MD_CTX_free(ctx->ctx);
	free(ctx);
#endif
	return;
}

int crapi_mdigest_fd (int fd, int num, ... /* crapi_alg_t alg, void *dst, size_t *size, ...*/)
{
	register int i;
	va_list ap;
	struct digest_ctbl_t *ctbl = malloc(num * sizeof(struct digest_ctbl_t));

	crapi_alg_t alg;
	void *dst;
	size_t *size;

	uint8_t fd_buf[CRAPI_IO_BUFSZ];
	ssize_t ret;

	if (num <= 0 || fd <= 0) {
		errno = EINVAL;
		free(ctbl);
		return -1;
	}
	for (i = 0; i < num; ++i)
		ctbl[i].ctx = NULL;

	va_start(ap, num);

	for (i = 0; i < num; ++i) {
		alg  = va_arg(ap, crapi_alg_t);
		dst  = va_arg(ap, void *);
		size = va_arg(ap, size_t *);

		ctbl[i].alg = alg;
		if ((ctbl[i].ctx = crapi_digest_init(dst, size, alg)) == NULL)
			*size = 0;
	}

	va_end (ap);

	while ((ret = read(fd, fd_buf, sizeof fd_buf)) == sizeof fd_buf) {
		for (i = 0; i < num; ++i) {
			if (ctbl[i].ctx == NULL)
				continue;
			if (crapi_digest_update(ctbl[i].ctx, fd_buf, sizeof fd_buf) != 0) {
				goto fail;
			}
		}
	}

	switch (ret) {
	case 0:
		break;
	case -1:
		goto fail;
	default:
		if (ret <= 0) {
			free(ctbl);
			return -1;
		}

		for (i = 0; i < num; ++i) {
			if (ctbl[i].ctx == NULL)
				continue;
			if (crapi_digest_update(ctbl[i].ctx, fd_buf, (size_t)ret) != 0) {
				goto fail;
			}
		}
	}

	for (i = 0; i < num; ++i) {
		if (ctbl[i].ctx == NULL)
			continue;
		crapi_digest_fini(ctbl[i].ctx, ctbl[i].alg);
	}
	free(ctbl);
	return (0);
fail:
	for (i = 0; i < num; ++i) {
		if (ctbl[i].ctx != NULL)
			crapi_digest_free(ctbl[i].ctx);
	}

	free(ctbl);
	return (-1);
}
