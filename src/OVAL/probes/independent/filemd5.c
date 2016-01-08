/**
 * @file   filemd5.c
 * @brief  filemd5 probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process a filemd5_object as defined in OVAL 5.4 and 5.5.
 *
 */

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
#include <crapi/crapi.h>
#include <probe/probe.h>
#include <probe/option.h>

#include "oval_fts.h"

#define FILE_SEPARATOR '/'

static pthread_mutex_t __filemd5_probe_mutex;

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

static int filehash_cb (const char *p, const char *f, void *ptr, const SEXP_t *filters)
{
        SEXP_t *itm, *r0, *r1;
        SEXP_t *cobj = (SEXP_t *) ptr;

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
		itm = probe_item_create(OVAL_INDEPENDENT_FILE_MD5, NULL,
                                        "filepath", OVAL_DATATYPE_STRING, pbuf,
                                        "path",     OVAL_DATATYPE_STRING, p,
                                        "filename", OVAL_DATATYPE_STRING, f,
					NULL);
		probe_item_add_msg(itm, OVAL_MESSAGE_LEVEL_ERROR,
			"Can't get context for %s: %s\n", pbuf, strerror(errno));
		probe_item_setstatus(itm, SYSCHAR_STATUS_ERROR);

	} else {
                uint8_t md5_dst[16];
                size_t  md5_dstlen = sizeof md5_dst;
                char    md5_str[32+1];

                /*
                 * Compute hash values
                 */
                if (crapi_digest_fd (fd, CRAPI_DIGEST_MD5,  &md5_dst,  &md5_dstlen) != 0)
                {
                        close (fd);
                        return (-1);
                }

                close (fd);

                mem2hex (md5_dst,  sizeof md5_dst,  md5_str,  sizeof md5_str);

                /*
                 * Create and add the item
                 */
                itm = probe_item_create(OVAL_INDEPENDENT_FILE_MD5, NULL,
                                        "filepath", OVAL_DATATYPE_STRING, pbuf,
                                        "path",     OVAL_DATATYPE_STRING, p,
                                        "filename", OVAL_DATATYPE_STRING, f,
                                        "md5",      OVAL_DATATYPE_STRING, md5_str,
                                        NULL);
        }

	probe_cobj_add_item(cobj, itm, filters);
        SEXP_free (itm);

        return (0);
}

void *probe_init (void)
{
        /*
         * Initialize crypto API
         */
        if (crapi_init (NULL) != 0)
                return (NULL);

        /*
         * Initialize mutex.
         */
        switch (pthread_mutex_init (&__filemd5_probe_mutex, NULL)) {
        case 0:
                return ((void *)&__filemd5_probe_mutex);
        default:
                dI("Can't initialize mutex: errno=%u, %s.", errno, strerror (errno));
        }

        probe_setoption(PROBEOPT_OFFLINE_MODE_SUPPORTED, PROBE_OFFLINE_CHROOT);

        return (NULL);
}

void probe_fini (void *arg)
{
        _A((void *)arg == (void *)&__filemd5_probe_mutex);

        /*
         * Destroy mutex.
         */
        (void) pthread_mutex_destroy (&__filemd5_probe_mutex);

        return;
}

int probe_main (SEXP_t *probe_in, SEXP_t *probe_out, void *mutex, SEXP_t *filters)
{
        SEXP_t *path, *filename, *behaviors, *filepath;
	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

        (void)filters;

	if (probe_in == NULL || probe_out == NULL) {
		return (PROBE_EINVAL);
	}

        if (mutex == NULL) {
		return (PROBE_EINIT);
        }

        _A(mutex == &__filemd5_probe_mutex);

        path      = probe_obj_getent (probe_in, "path",      1);
        filename  = probe_obj_getent (probe_in, "filename",  1);
        behaviors = probe_obj_getent (probe_in, "behaviors", 1);
        filepath = probe_obj_getent (probe_in, "filepath", 1);

        /* we want either path+filename or filepath */
        if ( (path == NULL || filename == NULL) && filepath==NULL ) {
                SEXP_free (behaviors);
                SEXP_free (path);
                SEXP_free (filename);
                SEXP_free (filepath);

		return (PROBE_ENOELM);
        }

	probe_filebehaviors_canonicalize(&behaviors);

        switch (pthread_mutex_lock (&__filemd5_probe_mutex)) {
        case 0:
                break;
        default:
                dI("Can't lock mutex(%p): %u, %s.", &__filemd5_probe_mutex, errno, strerror (errno));

		SEXP_free (behaviors);
		SEXP_free (path);
		SEXP_free (filename);
		SEXP_free (filepath);

		return (PROBE_EFATAL);
        }

	if ((ofts = oval_fts_open(path, filename, filepath, behaviors)) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			filehash_cb(ofts_ent->path, ofts_ent->file, probe_out, filters);
			oval_ftsent_free(ofts_ent);
		}

		oval_fts_close(ofts);
	}

        SEXP_free (behaviors);
        SEXP_free (path);
        SEXP_free (filename);
        SEXP_free (filepath);

        switch (pthread_mutex_unlock (&__filemd5_probe_mutex)) {
        case 0:
                break;
        default:
                dI("Can't unlock mutex(%p): %u, %s.", &__filemd5_probe_mutex, errno, strerror (errno));

		return (PROBE_EFATAL);
        }

        return 0;
}
