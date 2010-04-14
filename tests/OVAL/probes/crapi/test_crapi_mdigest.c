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
        uint8_t md5_dst[16];
        size_t  md5_dstlen = sizeof md5_dst;

        uint8_t sha1_dst[20];
        size_t  sha1_dstlen = sizeof sha1_dst;

        char *orig_md5sum,  comp_md5sum[(sizeof md5_dst * 2) + 1];
        char *orig_sha1sum, comp_sha1sum[(sizeof sha1_dst * 2) + 1];
        char *filename;
        int   fd;

        if (argc != 4) {
                fprintf (stderr, "Usage: %s <file> <md5sum> <sha1sum>\n", argv[0]);
                return (1);
        }

        filename     = argv[1];
        orig_md5sum  = argv[2];
        orig_sha1sum = argv[3];
        
        fd = open (filename, O_RDONLY);
        
        if (fd < 0) {
                perror ("open");
                return (2);
        }

        if (crapi_mdigest_fd (fd, 2,
                             CRAPI_DIGEST_MD5,  &md5_dst,  &md5_dstlen,
                             CRAPI_DIGEST_SHA1, &sha1_dst, &sha1_dstlen) != 0)
        {
                fprintf (stderr, "crapi_digest() != 0\n");
                abort ();
        }

        mem2hex (md5_dst,  md5_dstlen,  comp_md5sum,  sizeof comp_md5sum);
        mem2hex (sha1_dst, sha1_dstlen, comp_sha1sum, sizeof comp_sha1sum);

        if (strcmp (orig_md5sum, comp_md5sum) != 0) {
                fprintf (stderr, "crapi_digest::MD5(%s) != %s (== %s)\n", filename, orig_md5sum, comp_md5sum);
                abort ();
        }

        if (strcmp (orig_sha1sum, comp_sha1sum) != 0) {
                fprintf (stderr, "crapi_digest::SHA1(%s) != %s (== %s)\n", filename, orig_sha1sum, comp_sha1sum);
                abort ();
        }

        close (fd);
        
        return (0);
}
