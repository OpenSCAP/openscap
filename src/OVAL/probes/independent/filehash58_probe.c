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
#include <probe/probe.h>
#include <probe/option.h>

#include "common/debug_priv.h"
#include "oval_fts.h"
#include "util.h"
#include "probe/entcmp.h"
#include "filehash58_probe.h"

#define FILE_SEPARATOR '/'

static pthread_mutex_t __filehash58_probe_mutex;

#define CRAPI_INVALID -1

static const struct oscap_string_map CRAPI_ALG_MAP[] = {
	{CRAPI_DIGEST_MD5, "MD5"},
	{CRAPI_DIGEST_SHA1, "SHA-1"},
	{CRAPI_DIGEST_SHA224, "SHA-224"},
	{CRAPI_DIGEST_SHA256, "SHA-256"},
	{CRAPI_DIGEST_SHA384, "SHA-384"},
	{CRAPI_DIGEST_SHA512, "SHA-512"},
	/* {CRAPI_DIGEST_RMD160, "RMD-160"}, OVAL doesn't support this */
	{CRAPI_INVALID, NULL}
};

static const struct oscap_string_map CRAPI_ALG_MAP_SIZE[] = {
	{16, "MD5"},
	{20, "SHA-1"},
	{28, "SHA-224"},
	{32, "SHA-256"},
	{48, "SHA-384"},
	{64, "SHA-512"},
	/* {20, "RMD-160"}, OVAL doesn't support this */
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

static int filehash58_cb(const char *prefix, const char *p, const char *f, const char *h, probe_ctx *ctx)
{
	SEXP_t *itm;

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

		itm = probe_item_create (OVAL_INDEPENDENT_FILE_HASH58, NULL,
					"filepath", OVAL_DATATYPE_STRING, pbuf,
					"path",     OVAL_DATATYPE_STRING, p,
					"filename", OVAL_DATATYPE_STRING, f,
					"hash_type",OVAL_DATATYPE_STRING, h,
					NULL);
		probe_item_add_msg(itm, OVAL_MESSAGE_LEVEL_ERROR,
			"Can't open \"%s\": errno=%d, %s.", pbuf, errno, strerror (errno));
		probe_item_setstatus(itm, SYSCHAR_STATUS_ERROR);
	} else {
		uint8_t hash_dst[1025];
		size_t  hash_dstlen = sizeof hash_dst;
		char    hash_str[2051];

		crapi_alg_t hash_type;

		hash_type = oscap_string_to_enum(CRAPI_ALG_MAP, h);
		hash_dstlen = oscap_string_to_enum(CRAPI_ALG_MAP_SIZE, h);

		/*
		 * Compute hash value
		 */
		if (crapi_mdigest_fd (fd, 1, hash_type, hash_dst, &hash_dstlen) != 0) {
			close (fd);
			return (-1);
		}

		close (fd);

		hash_str[0] = '\0';
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

		if (hash_dstlen == 0) {
			probe_item_add_msg(itm, OVAL_MESSAGE_LEVEL_ERROR,
					   "Unable to compute %s hash value of \"%s\".", h, pbuf);
			probe_item_setstatus(itm, SYSCHAR_STATUS_ERROR);
		}
	}

	probe_item_collect(ctx, itm);

	return (0);
}

int filehash58_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_OWN;
}

void *filehash58_probe_init(void)
{
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
		dI("Can't initialize mutex: errno=%u, %s.", errno, strerror (errno));
	}

	return (NULL);
}

void filehash58_probe_fini(void *arg)
{
	_A((void *)arg == (void *)&__filehash58_probe_mutex);

	/*
	 * Destroy mutex.
	 */
	(void) pthread_mutex_destroy (&__filehash58_probe_mutex);

	return;
}

int filehash58_probe_main(probe_ctx *ctx, void *mutex)
{
	SEXP_t *probe_in;
	SEXP_t *path, *filename, *behaviors, *filepath, *hash_type;
	char hash_type_str[128];
	int err = 0;

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
		err = PROBE_ENOENT;
		goto cleanup;
	}

	PROBE_ENT_STRVAL(hash_type, hash_type_str, sizeof hash_type_str, err = PROBE_ENOVAL;, err = PROBE_ENOVAL;);
	if (err != 0)
		goto cleanup;

	probe_filebehaviors_canonicalize(&behaviors);

	switch (pthread_mutex_lock (&__filehash58_probe_mutex)) {
	case 0:
		break;
	default:
		dI("Can't lock mutex(%p): %u, %s.", &__filehash58_probe_mutex, errno, strerror (errno));

		err = PROBE_EFATAL;
		goto cleanup;
	}

	const char *prefix = getenv("OSCAP_PROBE_ROOT");
	if ((ofts = oval_fts_open_prefixed(prefix, path, filename, filepath, behaviors, probe_ctx_getresult(ctx))) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			/* find hash types to compare with entity, think "not satisfy" */
			const struct oscap_string_map *p = CRAPI_ALG_MAP;
			while (p->value != CRAPI_INVALID) {
				SEXP_t *crapi_hash_type_sexp = SEXP_string_new(p->string, strlen(p->string));
				if (probe_entobj_cmp(hash_type, crapi_hash_type_sexp) == OVAL_RESULT_TRUE) {
					filehash58_cb(prefix, ofts_ent->path, ofts_ent->file, p->string, ctx);
				}

				SEXP_free(crapi_hash_type_sexp);
				p++;
			}
			oval_ftsent_free(ofts_ent);
		}

		oval_fts_close(ofts);
	}

cleanup:
	SEXP_free (behaviors);
	SEXP_free (path);
	SEXP_free (filename);
	SEXP_free (filepath);
        SEXP_free (hash_type);

	switch (pthread_mutex_unlock (&__filehash58_probe_mutex)) {
	case 0:
		break;
	default:
		dI("Can't unlock mutex(%p): %u, %s.", &__filehash58_probe_mutex, errno, strerror (errno));

		err = PROBE_EFATAL;
	}

	return err;
}
