/**
 * @file   filehash_probe.c
 * @brief  filehash probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process a filehash_object as defined in OVAL 5.4 and 5.5.
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

#include "_seap.h"
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
#include <common/debug_priv.h>
#include "filehash_probe.h"

#define FILE_SEPARATOR '/'

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

static int filehash_cb (const char *prefix, const char *p, const char *f, probe_ctx *ctx, oval_schema_version_t over)
{
        SEXP_t *itm;
        char   pbuf[PATH_MAX+1];
        size_t plen, flen;
	bool include_filepath;
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
	include_filepath = oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.6)) >= 0;

        /*
         * Open the file
         */
	if (prefix == NULL) {
		fd = open(pbuf, O_RDONLY);
	} else {
		char *path_with_prefix = oscap_path_join(prefix, pbuf);
		fd = open(path_with_prefix, O_RDONLY);
		free(path_with_prefix);
	}

        if (fd < 0) {
                strerror_r (errno, pbuf, PATH_MAX);
                pbuf[PATH_MAX] = '\0';

		itm = probe_item_create(OVAL_INDEPENDENT_FILE_HASH, NULL,
				"filepath", OVAL_DATATYPE_STRING, include_filepath ? pbuf : NULL,
				"path",     OVAL_DATATYPE_STRING, p,
				"filename", OVAL_DATATYPE_STRING, f,
				NULL
				);
		probe_item_add_msg(itm, OVAL_MESSAGE_LEVEL_ERROR,
				"Can't open \"%s\": errno=%d, %s.", pbuf, errno, strerror (errno));
		probe_item_setstatus(itm, SYSCHAR_STATUS_ERROR);

       } else {
                uint8_t md5_dst[16];
                size_t  md5_dstlen = sizeof md5_dst;
                char    md5_str[(sizeof md5_dst * 2) + 1];

                uint8_t sha1_dst[20];
                size_t  sha1_dstlen = sizeof sha1_dst;
                char    sha1_str[(sizeof sha1_dst * 2) + 1];

                /*
                 * Compute hash values
                 */
                if (crapi_mdigest_fd (fd, 2,
                                      CRAPI_DIGEST_MD5,  md5_dst,  &md5_dstlen,
                                      CRAPI_DIGEST_SHA1, sha1_dst, &sha1_dstlen) != 0)
                {
                        close (fd);
                        return (-1);
                }

                close (fd);

		md5_str[0] = '\0';
		sha1_str[0] = '\0';
                mem2hex (md5_dst,  md5_dstlen,  md5_str,  sizeof md5_str);
                mem2hex (sha1_dst, sha1_dstlen, sha1_str, sizeof sha1_str);

                /*
                 * Create and add the item
                 */
                itm = probe_item_create(OVAL_INDEPENDENT_FILE_HASH, NULL,
                                        "filepath", OVAL_DATATYPE_STRING, include_filepath ? pbuf : NULL,
                                        "path",     OVAL_DATATYPE_STRING, p,
                                        "filename", OVAL_DATATYPE_STRING, f,
                                        "md5",      OVAL_DATATYPE_STRING, md5_str,
                                        "sha1",     OVAL_DATATYPE_STRING, sha1_str,
                                        NULL);

		if (md5_dstlen == 0 || sha1_dstlen == 0)
			probe_item_setstatus(itm, SYSCHAR_STATUS_ERROR);
		if (md5_dstlen == 0)
			probe_item_add_msg(itm, OVAL_MESSAGE_LEVEL_ERROR,
					   "Unable to compute md5 hash value of \"%s\".", pbuf);
		if (sha1_dstlen == 0)
			probe_item_add_msg(itm, OVAL_MESSAGE_LEVEL_ERROR,
					   "Unable to compute sha1 hash value of \"%s\".", pbuf);
        }

        probe_item_collect(ctx, itm);

        return (0);
}

int filehash_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_OWN;
}

void *filehash_probe_init(void)
{
        /*
         * Initialize crypto API
         */
        if (crapi_init (NULL) != 0)
                return (NULL);

        /*
         * Initialize mutex.
         */
	pthread_mutex_t *filehash_probe_mutex = malloc(sizeof(pthread_mutex_t));
	switch (pthread_mutex_init(filehash_probe_mutex, NULL)) {
        case 0:
		return ((void *)filehash_probe_mutex);
        default:
                dI("Can't initialize mutex: errno=%u, %s.", errno, strerror (errno));
		free(filehash_probe_mutex);
        }


        return (NULL);
}

void filehash_probe_fini(void *arg)
{
        /*
         * Destroy mutex.
         */
	pthread_mutex_t *filehash_probe_mutex = (pthread_mutex_t *)arg;
	(void) pthread_mutex_destroy(filehash_probe_mutex);
	free(filehash_probe_mutex);
}

int filehash_probe_main(probe_ctx *ctx, void *arg)
{
        SEXP_t *path, *filename, *behaviors, *filepath, *probe_in;

	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;
	oval_schema_version_t over;

	pthread_mutex_t *filehash_probe_mutex = (pthread_mutex_t *)arg;
	if (filehash_probe_mutex == NULL) {
		return (PROBE_EINIT);
        }

        probe_in  = probe_ctx_getobject(ctx);

        path      = probe_obj_getent (probe_in, "path",      1);
        filename  = probe_obj_getent (probe_in, "filename",  1);
        behaviors = probe_obj_getent (probe_in, "behaviors", 1);
        filepath = probe_obj_getent (probe_in, "filepath", 1);
	over = probe_obj_get_platform_schema_version(probe_in);

        /* we want either path+filename or filepath */
        if ( (path == NULL || filename == NULL) && filepath==NULL ) {
                SEXP_free (behaviors);
                SEXP_free (path);
                SEXP_free (filename);
		SEXP_free (filepath);

		return (PROBE_ENOELM);
        }

	probe_filebehaviors_canonicalize(&behaviors);

	switch (pthread_mutex_lock(filehash_probe_mutex)) {
        case 0:
                break;
        default:
		dI("Can't lock mutex(%p): %u, %s.", filehash_probe_mutex, errno, strerror (errno));

		SEXP_free (behaviors);
		SEXP_free (path);
		SEXP_free (filename);
		SEXP_free (filepath);

		return (PROBE_EFATAL);
        }

	const char *prefix = getenv("OSCAP_PROBE_ROOT");
	if ((ofts = oval_fts_open_prefixed(prefix, path, filename, filepath, behaviors, probe_ctx_getresult(ctx))) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			filehash_cb(prefix, ofts_ent->path, ofts_ent->file, ctx, over);
			oval_ftsent_free(ofts_ent);
		}

		oval_fts_close(ofts);
	}

        SEXP_free (behaviors);
        SEXP_free (path);
        SEXP_free (filename);
        SEXP_free (filepath);

	switch (pthread_mutex_unlock(filehash_probe_mutex)) {
        case 0:
                break;
        default:
	dI("Can't unlock mutex(%p): %u, %s.", filehash_probe_mutex, errno, strerror (errno));

		return (PROBE_EFATAL);
        }

	return 0;
}
