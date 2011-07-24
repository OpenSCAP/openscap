/**
 * @file   filehash58.c
 * @brief  filehash58 probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * 2011/07/01 plautrba@redhat.com
 *  This probe is able to process a filehash58_object as defined in OVAL 5.8
 *
 */

/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *      "Petr Lautrbach" <plautrba@redhat.com>
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

#include "oval_fts.h"
#include "util.h"

#define FILE_SEPARATOR '/'

static pthread_mutex_t __filehash58_probe_mutex;

#define CRAPI_INVALID -1

static const struct oscap_string_map CRAPI_ALG_MAP[] = {
	{CRAPI_DIGEST_MD5, "MD5"},
	{CRAPI_DIGEST_SHA1, "SHA-1"},
	{CRAPI_DIGEST_SHA256, "SHA-256"},
	{CRAPI_DIGEST_SHA512, "SHA-512"},
	{CRAPI_DIGEST_RMD160, "RMD-160"},
	{CRAPI_INVALID, NULL}
};

static const struct oscap_string_map CRAPI_ALG_MAP_SIZE[] = {
	{16, "MD5"},
	{20, "SHA-1"},
	{32, "SHA-256"},
	{64, "SHA-512"},
	{20, "RMD-160"},
	{0, NULL}
};


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

static int filehash58_cb (const char *p, const char *f, const char *h, probe_ctx *ctx)
{
	SEXP_t *itm, *r0, *r1;

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
			itm = probe_item_creat ("filehash58_item", NULL,
						"path", at, NULL,
						NULL);
		} else {
			close (fd);
			itm = probe_item_creat ("filehash58_item", NULL,
						"filename", at, NULL,
						NULL);
		}

		SEXP_free (at);
	} else {
		uint8_t hash_dst[1025];
		size_t  hash_dstlen;
		char    hash_str[2051];

		crapi_alg_t hash_type;

		hash_type = oscap_string_to_enum(CRAPI_ALG_MAP, h);
		hash_dstlen = oscap_string_to_enum(CRAPI_ALG_MAP_SIZE, h);

		/*
		 * Compute hash value
		 */
		if (crapi_mdigest_fd (fd, 1, hash_type, &hash_dst, &hash_dstlen) != 0) {
			close (fd);
			return (-1);
		}

		close (fd);

		mem2hex (hash_dst, hash_dstlen, hash_str, sizeof hash_str);

		/*
		 * Create and add the item
		 */
		itm = probe_item_create(OVAL_INDEPENDENT_FILE_HASH58, NULL,
					"filepath", OVAL_DATATYPE_STRING, pbuf,
					"path",     OVAL_DATATYPE_STRING, p,
					"filename", OVAL_DATATYPE_STRING, f,
					"hash_type",OVAL_DATATYPE_STRING, h,
					"hash",     OVAL_DATATYPE_STRING, hash_str,
					NULL);
	}

	probe_item_collect(ctx, itm);

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
	switch (pthread_mutex_init (&__filehash58_probe_mutex, NULL)) {
	case 0:
		return ((void *)&__filehash58_probe_mutex);
	default:
		_D("Can't initialize mutex: errno=%u, %s.\n", errno, strerror (errno));
	}

	return (NULL);
}

void probe_fini (void *arg)
{
	_A((void *)arg == (void *)&__filehash58_probe_mutex);

	/*
	 * Destroy mutex.
	 */
	(void) pthread_mutex_destroy (&__filehash58_probe_mutex);

	return;
}

int probe_main(probe_ctx *ctx, void *mutex)
{
	SEXP_t *probe_in;
	SEXP_t *path, *filename, *behaviors, *filepath, *hash_type;
	char hash_type_str[128];

	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

	if (mutex == NULL) {
		return (PROBE_EINIT);
	}

	_A(mutex == &__filehash58_probe_mutex);

	probe_in  = probe_ctx_getobject(ctx);

	path      = probe_obj_getent (probe_in, "path",      1);
	filename  = probe_obj_getent (probe_in, "filename",  1);
	behaviors = probe_obj_getent (probe_in, "behaviors", 1);
	filepath  = probe_obj_getent (probe_in, "filepath", 1);
	hash_type = probe_obj_getent (probe_in, "hash_type", 1);

	/* we want hash_type and either path+filename or filepath */
	if ( ! hash_type ||
		((path == NULL || filename == NULL) && filepath==NULL )
	) {
		SEXP_free (behaviors);
		SEXP_free (path);
		SEXP_free (filename);
		SEXP_free (filepath);
		SEXP_free (hash_type);

		return (PROBE_ENOELM);
	}

	PROBE_ENT_STRVAL(hash_type, hash_type_str, sizeof hash_type_str, return (PROBE_ENOELM);, return (PROBE_ENOELM););

	/* behaviours are not important if filepath is used */
	if(filepath != NULL && behaviors != NULL) {
		SEXP_free (behaviors);
		behaviors = NULL;
	}

	probe_filebehaviors_canonicalize(&behaviors);

	switch (pthread_mutex_lock (&__filehash58_probe_mutex)) {
	case 0:
		break;
	default:
		_D("Can't lock mutex(%p): %u, %s.\n", &__filehash58_probe_mutex, errno, strerror (errno));

		SEXP_free (behaviors);
		SEXP_free (path);
		SEXP_free (filename);
		SEXP_free (filepath);
		SEXP_free (hash_type);

		return (PROBE_EFATAL);
	}

	if ((ofts = oval_fts_open(path, filename, filepath, behaviors)) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			filehash58_cb(ofts_ent->path, ofts_ent->file, hash_type_str, ctx);
			oval_ftsent_free(ofts_ent);
		}

		oval_fts_close(ofts);
	}

	SEXP_free (behaviors);
	SEXP_free (path);
	SEXP_free (filename);
	SEXP_free (filepath);
        SEXP_free (hash_type);

	switch (pthread_mutex_unlock (&__filehash58_probe_mutex)) {
	case 0:
		break;
	default:
		_D("Can't unlock mutex(%p): %u, %s.\n", &__filehash58_probe_mutex, errno, strerror (errno));

		return (PROBE_EFATAL);
	}

	return 0;
}
