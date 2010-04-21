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

#include <seap.h>
#include <probe-api.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <errno.h>
#include <crapi/digest.h>
#include "findfile.h"

#define FILE_SEPARATOR '/'

static pthread_mutex_t __filehash_probe_mutex;

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

static int filehash_cb (const char *p, const char *f, void *ptr)
{
        SEXP_t *itm, *r0, *r1, *r2, *r3;
        SEXP_t *res = (SEXP_t *) ptr;

        char   pbuf[PATH_MAX+1];
        size_t plen, flen;
        
        int fd;
        
        if (f == NULL)
                return (0);

        /*
         * Prepare path
         */
        plen = strlen (p);
        flen = strlen (f);
        
        if (plen + flen + 1 > PATH_MAX)
                return (-1);
        
        memcpy (pbuf, p, sizeof (char) * plen);
        
        if (p[plen - 1] != FILE_SEPARATOR) {
                pbuf[plen] = FILE_SEPARATOR;
                ++plen;
        }
        
        memcpy (pbuf + plen, f, sizeof (char) * flen);
        pbuf[plen+flen] = '\0';

        /*
         * Open the file
         */
        fd = open (pbuf, O_RDONLY);

        if (fd < 0) {
                SEXP_t *at;
                
                strerror_r (errno, pbuf, PATH_MAX);
                pbuf[PATH_MAX] = '\0';

                fd = open (p, O_RDONLY);
                at = probe_attr_creat ("status",  r0 = SEXP_number_newi_32 (OVAL_STATUS_ERROR), /* XXX: don't use newi_32 directly */
                                       "message", r1 = SEXP_string_newf ("%s", pbuf),
                                       NULL);
                
                SEXP_vfree (r0, r1, NULL);
                
                if (fd < 0) {
                        itm = probe_item_creat ("filehash_item", NULL,
                                                "path", at, NULL,
                                                NULL);
                } else {
                        close (fd);
                        itm = probe_item_creat ("filehash_item", NULL,
                                                "filename", at, NULL,
                                                NULL);
                }

                SEXP_free (at);
        } else {
                uint8_t md5_dst[16];
                size_t  md5_dstlen = sizeof md5_dst;
                char    md5_str[32+1];
                
                uint8_t sha1_dst[32];
                size_t  sha1_dstlen = sizeof sha1_dst;
                char    sha1_str[64+1];

                /*
                 * Compute hash values
                 */
                if (crapi_mdigest_fd (fd, 2,
                                      CRAPI_DIGEST_MD5,  &md5_dst,  &md5_dstlen,
                                      CRAPI_DIGEST_SHA1, &sha1_dst, &sha1_dstlen) != 0)
                {
                        close (fd);
                        return (-1);
                }

                close (fd);
                
                mem2hex (md5_dst,  sizeof md5_dst,  md5_str,  sizeof md5_str);
                mem2hex (sha1_dst, sizeof sha1_dst, sha1_str, sizeof sha1_str);
                
                /*
                 * Create and add the item
                 */
                itm = probe_item_creat ("filehash_item", NULL,
                                        /* entities */
                                        "path", NULL,
                                        r0 = SEXP_string_newf (p, plen),
                                        "filename", NULL,
                                        r1 = SEXP_string_newf (p, flen),
                                        "md5", NULL,
                                        r2 = SEXP_string_newf (md5_str, sizeof md5_str - 1),
                                        "sha1", NULL,
                                        r3 = SEXP_string_newf (sha1_str, sizeof sha1_str - 1),
                                        NULL);
                SEXP_vfree (r0, r1, r2, r3, NULL);
        }

        SEXP_list_add (res, itm);
        SEXP_free (itm);
        
        return (0);
}

void *probe_init (void)
{
        _LOGCALL_;
        
        /*
         * Initialize crypto API
         */
        if (crapi_init (NULL) != 0)
                return (NULL);
        
        /*
         * Initialize mutex.
         */
        switch (pthread_mutex_init (&__filehash_probe_mutex, NULL)) {
        case 0:
                return ((void *)&__filehash_probe_mutex);
        default:
                _D("Can't initialize mutex: errno=%u, %s.\n", errno, strerror (errno));
        }
        
        return (NULL);
}

void probe_fini (void *arg)
{
        _A((void *)arg == (void *)&__filehash_probe_mutex);
        
        /*
         * Destroy mutex.
         */
        (void) pthread_mutex_destroy (&__filehash_probe_mutex);
        
        return;
}

SEXP_t *probe_main (SEXP_t *probe_in, int *err, void *mutex)
{
        SEXP_t *path, *filename, *behaviors, *items;
        SEXP_t *r0, *r1, *r2;
        int     filecnt;
        
        if (mutex == NULL) {
                *err = PROBE_EINIT;
                return (NULL);
        }
        
        _A(mutex == &__filehash_probe_mutex);
        
        path      = probe_obj_getent (probe_in, "path",      1);
        filename  = probe_obj_getent (probe_in, "filename",  1);
        behaviors = probe_obj_getent (probe_in, "behaviors", 1);
        
        if (path == NULL || filename == NULL) {
                *err = PROBE_ENOELM;
                
                SEXP_free (behaviors);
                SEXP_free (path);
                SEXP_free (filename);
                
                return (NULL);
        }
        
        if (behaviors == NULL) {
                SEXP_t *bh_list;
                
                bh_list = probe_ent_creat ("behaviors",
                                           r0 = probe_attr_creat ("max_depth",
                                                                  r1 = SEXP_string_newf ("1"),
                                                                  "recurse_direction",
                                                                  r2 = SEXP_string_newf ("none"),
                                                                  NULL),
                                           NULL /* val */,
                                           NULL /* end */);
                
                behaviors = SEXP_list_first (bh_list);
         
                SEXP_vfree (bh_list,
                            r0, r1, r2, NULL);
        } else {
                if (!probe_ent_attrexists (behaviors, "max_depth")) {
                        probe_ent_attr_add (behaviors,
                                            "max_depth", r0 = SEXP_string_newf ("-1"));
                        SEXP_free (r0);
                }
                
                if (!probe_ent_attrexists (behaviors, "recurse_direction")) {
                        probe_ent_attr_add (behaviors,
                                            "recurse_direction", r0 = SEXP_string_newf ("none"));
                        SEXP_free (r0);
                }
        }
        
        _A(behaviors != NULL);
        
        items = SEXP_list_new (NULL);

        switch (pthread_mutex_lock (&__filehash_probe_mutex)) {
        case 0:
                break;
        default:
                _D("Can't lock mutex(%p): %u, %s.\n", &__filehash_probe_mutex, errno, strerror (errno));
                
                *err = PROBE_EFATAL;
                return (NULL);
        }

        filecnt = find_files (path, filename, behaviors, &filehash_cb, &items);
        *err    = 0;
        
        if (filecnt < 0) {
                /* error */
                SEXP_free (items);
                r0    = probe_obj_creat ("file_item", NULL,
                                        /* entities */                                        
                                        "path",     NULL, path,
                                         NULL);
                
                probe_item_setstatus(r0, OVAL_STATUS_ERROR);
                
                SEXP_list_add (items, r0);
                SEXP_free (r0);
        }
        
        SEXP_free (behaviors);
        SEXP_free (path);
        SEXP_free (filename);
        
        switch (pthread_mutex_unlock (&__filehash_probe_mutex)) {
        case 0:
                break;
        default:
                _D("Can't unlock mutex(%p): %u, %s.\n", &__filehash_probe_mutex, errno, strerror (errno));
                
                *err = PROBE_EFATAL;
                return (NULL);
        }
        
        return (items);
}
