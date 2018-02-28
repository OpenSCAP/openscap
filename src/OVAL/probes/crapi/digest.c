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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "crapi.h"
#include "digest.h"
#include "md5.h"
#include "sha1.h"
#include "sha2.h"
#include "rmd160.h"

int crapi_digest_fd (int fd, crapi_alg_t alg, void *dst, size_t *size)
{
	if (dst == NULL) {
		errno = EFAULT;
		return -1;
	}
	if (size == NULL) {
		errno = EFAULT;
		return -1;
	}

        switch (alg) {
        case CRAPI_DIGEST_MD5:
                return crapi_md5_fd (fd, dst, size);
        case CRAPI_DIGEST_SHA1:
                return crapi_sha1_fd (fd, dst, size);
        case CRAPI_DIGEST_SHA224:
                return crapi_sha224_fd (fd, dst, size);
        case CRAPI_DIGEST_SHA256:
                return crapi_sha256_fd (fd, dst, size);
        case CRAPI_DIGEST_SHA384:
                return crapi_sha384_fd (fd, dst, size);
        case CRAPI_DIGEST_SHA512:
                return crapi_sha512_fd (fd, dst, size);
        case CRAPI_DIGEST_RMD160:
                return crapi_rmd160_fd (fd, dst, size);
        }

        errno = EINVAL;
        return (-1);
}

int crapi_mdigest_fd (int fd, int num, ... /* crapi_alg_t alg, void *dst, size_t *size, ...*/)
{
        register int i;
        va_list ap;
        struct digest_ctbl_t *ctbl = malloc(num * sizeof(struct digest_ctbl_t));

        crapi_alg_t alg;
        void       *dst;
        size_t     *size;

        uint8_t fd_buf[CRAPI_IO_BUFSZ];
        ssize_t ret;

	if (num <= 0 || fd <= 0) {
		errno = EINVAL;
		return -1;
	}
        for (i = 0; i < num; ++i)
                ctbl[i].ctx = NULL;

        va_start (ap, num);

        for (i = 0; i < num; ++i) {
                alg  = va_arg (ap, crapi_alg_t);
                dst  = va_arg (ap, void *);
                size = va_arg (ap, size_t *);

                switch (alg) {
                case CRAPI_DIGEST_MD5:
                        ctbl[i].init   = &crapi_md5_init;
                        ctbl[i].update = &crapi_md5_update;
                        ctbl[i].fini   = &crapi_md5_fini;
                        ctbl[i].free   = &crapi_md5_free;
                        break;
                case CRAPI_DIGEST_SHA1:
                        ctbl[i].init   = &crapi_sha1_init;
                        ctbl[i].update = &crapi_sha1_update;
                        ctbl[i].fini   = &crapi_sha1_fini;
                        ctbl[i].free   = &crapi_sha1_free;
                        break;
                case CRAPI_DIGEST_SHA224:
                        ctbl[i].init   = &crapi_sha224_init;
                        ctbl[i].update = &crapi_sha224_update;
                        ctbl[i].fini   = &crapi_sha224_fini;
                        ctbl[i].free   = &crapi_sha224_free;
                        break;
                case CRAPI_DIGEST_SHA256:
                        ctbl[i].init   = &crapi_sha256_init;
                        ctbl[i].update = &crapi_sha256_update;
                        ctbl[i].fini   = &crapi_sha256_fini;
                        ctbl[i].free   = &crapi_sha256_free;
                        break;
                case CRAPI_DIGEST_SHA384:
                        ctbl[i].init   = &crapi_sha384_init;
                        ctbl[i].update = &crapi_sha384_update;
                        ctbl[i].fini   = &crapi_sha384_fini;
                        ctbl[i].free   = &crapi_sha384_free;
                        break;
                case CRAPI_DIGEST_SHA512:
                        ctbl[i].init   = &crapi_sha512_init;
                        ctbl[i].update = &crapi_sha512_update;
                        ctbl[i].fini   = &crapi_sha512_fini;
                        ctbl[i].free   = &crapi_sha512_free;
                        break;
                case CRAPI_DIGEST_RMD160:
                        ctbl[i].init   = &crapi_rmd160_init;
                        ctbl[i].update = &crapi_rmd160_update;
                        ctbl[i].fini   = &crapi_rmd160_fini;
                        ctbl[i].free   = &crapi_rmd160_free;
                        break;
                default:
                        va_end (ap);
                        goto fail;
                }

                if ((ctbl[i].ctx = ctbl[i].init (dst, size)) == NULL)
			*size = 0;
        }

        va_end (ap);

        while ((ret = read (fd, fd_buf, sizeof fd_buf)) == sizeof fd_buf) {
                for (i = 0; i < num; ++i) {
			if (ctbl[i].ctx == NULL)
				continue;
                        if (ctbl[i].update (ctbl[i].ctx, fd_buf, sizeof fd_buf) != 0) {
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
			return -1;
		}

                for (i = 0; i < num; ++i) {
			if (ctbl[i].ctx == NULL)
				continue;
                        if (ctbl[i].update (ctbl[i].ctx, fd_buf, (size_t)ret) != 0) {
                                goto fail;
                        }
                }
        }

        for (i = 0; i < num; ++i) {
		if (ctbl[i].ctx == NULL)
			continue;
                ctbl[i].fini (ctbl[i].ctx);
	}
        free(ctbl);
        return (0);
fail:
        for (i = 0; i < num; ++i)
                if (ctbl[i].ctx != NULL)
                        ctbl[i].free (ctbl[i].ctx);

        free(ctbl);
        return (-1);
}
