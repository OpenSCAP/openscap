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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <crapi/crapi.h>
#include <crapi/digest.h>

static int mem2hex (uint8_t *mem, size_t mlen, char *str, size_t slen)
{
        const char ch[] = "0123456789abcdef";
        register size_t i;

	if (str == NULL) {
		return -1;
	}
        
        if (slen < (mlen * 2) + 1) {
                errno = ENOBUFS;
                return (-1);
        }

        for (i = 0; i < mlen; ++i) {
                str[i*2  ] = ch[(mem[i] & 0xf0) >> 4];
                str[i*2+1] = ch[(mem[i] & 0x0f)];
        }

        str[i*2] = '\0';

        return (0);
}

int main (int argc, char *argv[])
{
	if (argc != 4) {
		fprintf (stderr, "Usage: %s <file> <checksum> <algorithm>\n", argv[0]);
                return (1);
        }

	char *filename = argv[1];
	char *orig_sum = argv[2];
	char *algorithm_str = argv[3];

	crapi_alg_t algorithm;
	size_t dstlen = 0;

	if (!strcmp(algorithm_str, "md5")) {
#ifdef OPENSCAP_ENABLE_MD5
		algorithm = CRAPI_DIGEST_MD5;
		dstlen = 16;
#else
		return 1;
#endif
	} else if (!strcmp(algorithm_str, "sha1")) {
#ifdef OPENSCAP_ENABLE_SHA1
		algorithm = CRAPI_DIGEST_SHA1;
		dstlen = 20;
#else
		return 1;
#endif
	} else if (!strcmp(algorithm_str, "sha256")) {
		algorithm = CRAPI_DIGEST_SHA256;
		dstlen = 32;
	} else {
		return 1;
	}

	int fd = open(filename, O_RDONLY);

        if (fd < 0) {
                perror ("open");
                return (2);
        }

        if (crapi_init (NULL) != 0) {
                fprintf (stderr, "crapi_init() != 0\n");
                abort ();
        }

	uint8_t *dst = malloc(dstlen);

		size_t comp_sum_len = (dstlen * 2) + 1;
		char *comp_sum = malloc(comp_sum_len);

	if (crapi_digest_fd(fd, algorithm, dst, &dstlen) != 0) {
                fprintf (stderr, "crapi_digest() != 0\n");
                abort ();
        }

	mem2hex (dst, dstlen, comp_sum, comp_sum_len);

	if (strcmp(orig_sum, comp_sum) != 0) {
		fprintf (stderr, "crapi_digest::%s(%s) != %s (== %s)\n", algorithm_str, filename, orig_sum, comp_sum);
                abort ();
        }
	free(dst);
	free(comp_sum);

        close (fd);
        
        return (0);
}
