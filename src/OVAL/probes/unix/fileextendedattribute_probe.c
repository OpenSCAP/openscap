/**
 * @file   fileextendedattribute_probe.c
 * @brief  fileextendedattribute probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <seap.h>
#include <probe-api.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>

#include <sys/types.h>
#if defined(HAVE_SYS_XATTR_H)
#  include <sys/xattr.h>
#elif defined(HAVE_ATTR_XATTR_H)
#  include <attr/xattr.h>
#else
#  error "This probe requires sys/xattr.h or attr/xattr.h, none were found!"
#endif

#include <probe/probe.h>
#include <probe/option.h>
#include "probe/entcmp.h"
#include "oval_fts.h"
#include "common/debug_priv.h"
#include "fileextendedattribute_probe.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef OS_APPLE
#define llistxattr(path, buf, size)   listxattr((path), (buf), (size), XATTR_NOFOLLOW)
#define lgetxattr(path, name, value, size)   getxattr((path), (name), (value), (size), 0, XATTR_NOFOLLOW)
#define lsetxattr(path, name, value, size, flags) setxattr((path), (name), (value), (size), 0, (flags) | XATTR_NOFOLLOW)
#endif

#define FILE_SEPARATOR '/'

struct cbargs {
        probe_ctx *ctx;
	int        error;
        SEXP_t    *attr_ent;
};

static int file_cb(const char *prefix, const char *p, const char *f, void *ptr, SEXP_t *gr_lastpath)
{
        char path_buffer[PATH_MAX];
        SEXP_t *item, xattr_name;
        struct cbargs *args = (struct cbargs *) ptr;
        const char *st_path;

        ssize_t xattr_count = -1;
        char   *xattr_buf = NULL;
        size_t  xattr_buflen = 0, i;

	if (f == NULL) {
		st_path = p;
	} else {
		const size_t p_len = strlen(p);
		/* Avoid 2 slashes */
		if (p_len >= 1 && p[p_len - 1] == FILE_SEPARATOR) {
			snprintf(path_buffer, sizeof path_buffer, "%s%s", p, f);
		} else {
			snprintf(path_buffer, sizeof path_buffer, "%s%c%s", p, FILE_SEPARATOR, f);
		}
		st_path = path_buffer;
	}

        SEXP_init(&xattr_name);

	char *st_path_with_prefix = oscap_path_join(prefix, st_path);
	do {
		/* estimate the size of the buffer */

		xattr_count = llistxattr(st_path_with_prefix, NULL, 0);

		if (xattr_count == 0) {
			free(st_path_with_prefix);
			return (0);
		}

		if (xattr_count < 0) {
			free(st_path_with_prefix);
				dI("FAIL: llistxattr(%s, %p, %zu): errno=%u, %s.", errno, strerror(errno));
				return 0;
		}

		/* allocate space for xattr names */
		xattr_buflen = xattr_count;
		xattr_buf    = realloc(xattr_buf, sizeof(char) * xattr_buflen);

		/* fill the buffer */
		xattr_count = llistxattr(st_path_with_prefix, xattr_buf, xattr_buflen);

		/* check & retry if needed */
	} while (errno == ERANGE);

        if (xattr_count < 0) {
                dI("FAIL: llistxattr(%s, %p, %zu): errno=%u, %s.", errno, strerror(errno));
                free(xattr_buf);
        }

        /* update lastpath if needed */
	if (!SEXP_emptyp(gr_lastpath)) {
		if (SEXP_strcmp(gr_lastpath, p) != 0) {
			SEXP_free_r(gr_lastpath);
			SEXP_string_new_r(gr_lastpath, p, strlen(p));
		}
	} else {
		SEXP_string_new_r(gr_lastpath, p, strlen(p));
	}

        i = 0;
        /* collect */
        do {
                SEXP_string_new_r(&xattr_name, xattr_buf + i, strlen(xattr_buf +i));

                if (probe_entobj_cmp(args->attr_ent, &xattr_name) == OVAL_RESULT_TRUE) {
                        ssize_t xattr_vallen = -1;
                        char   *xattr_val = NULL;

                        xattr_vallen = lgetxattr(st_path_with_prefix, xattr_buf + i, NULL, 0);
                retry_value:
                        if (xattr_vallen >= 0) {
				// Check possible buffer overflow
				if (sizeof(char) * (xattr_vallen + 1) <= sizeof(char) * xattr_vallen) {
					dE("Attribute is too long.");
					abort();
				}

				// Allocate buffer, '+1' is for trailing '\0'
 				xattr_val    = realloc(xattr_val, sizeof(char) * (xattr_vallen + 1));

				// we don't want to override space for '\0' by call of 'lgetxattr'
				// we pass only 'xattr_vallen' instead of 'xattr_vallen + 1'
                                xattr_vallen = lgetxattr(st_path_with_prefix, xattr_buf + i, xattr_val, xattr_vallen);

                                if (xattr_vallen < 0 || errno == ERANGE)
                                        goto retry_value;

				xattr_val[xattr_vallen] = '\0';

                                item = probe_item_create(OVAL_UNIX_FILEEXTENDEDATTRIBUTE, NULL,
                                                         "filepath", OVAL_DATATYPE_STRING, f == NULL ? NULL : st_path,
                                                         "path",     OVAL_DATATYPE_SEXP, gr_lastpath,
                                                         "filename", OVAL_DATATYPE_STRING, f == NULL ? "" : f,
                                                         "attribute_name", OVAL_DATATYPE_SEXP,   &xattr_name,
                                                         "value",          OVAL_DATATYPE_STRING, xattr_val,
                                                         NULL);

                                free(xattr_val);
                        } else {
                                dI("FAIL: lgetxattr(%s, %s, NULL, 0): errno=%u, %s.", errno, strerror(errno));

                                item = probe_item_create(OVAL_UNIX_FILEEXTENDEDATTRIBUTE, NULL, NULL);
                                probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);

                                if (xattr_val != NULL)
                                        free(xattr_val);
                        }

                        probe_item_collect(args->ctx, item); /* XXX: handle ENOMEM */
                }

                SEXP_free_r(&xattr_name);

                /* skip to next name */
                while (i < xattr_buflen && xattr_buf[i] != '\0')
                        ++i;
		++i;
        } while (xattr_buf + i < xattr_buf + xattr_buflen - 1);

        free(xattr_buf);
	free(st_path_with_prefix);
        return (0);
}

int fileextendedattribute_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_OWN;
}

void *fileextendedattribute_probe_init(void)
{
        /*
         * Initialize mutex.
         */
	pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
	switch (pthread_mutex_init (mutex, NULL)) {
        case 0:
		return ((void *)mutex);
        default:
                dI("Can't initialize mutex: errno=%u, %s.", errno, strerror (errno));
        }
#if 0
	probe_setoption(PROBEOPT_VARREF_HANDLING, false, "path");
	probe_setoption(PROBEOPT_VARREF_HANDLING, false, "filename");
#endif
        return (NULL);
}

void fileextendedattribute_probe_fini(void *arg)
{
        /*
         * Destroy mutex.
         */
	(void) pthread_mutex_destroy((pthread_mutex_t *)arg);
	free(arg);
}

int fileextendedattribute_probe_main(probe_ctx *ctx, void *mutex)
{
        SEXP_t *path, *filename, *behaviors;
        SEXP_t *filepath, *attribute_, *probe_in;
	int err;
        struct cbargs cbargs;
	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;
	SEXP_t gr_lastpath;

        if (mutex == NULL)
                return PROBE_EINIT;

        probe_in  = probe_ctx_getobject(ctx);

        path       = probe_obj_getent (probe_in, "path",      1);
        filename   = probe_obj_getent (probe_in, "filename",  1);
        behaviors  = probe_obj_getent (probe_in, "behaviors", 1);
        filepath   = probe_obj_getent (probe_in, "filepath", 1);
        attribute_ = probe_obj_getent (probe_in, "attribute_name", 1);

	/* we want either path+filename or filepath */
        if (((path == NULL || filename == NULL) && filepath == NULL)
            || attribute_ == NULL)
        {
                SEXP_free (behaviors);
                SEXP_free (path);
                SEXP_free (filename);
                SEXP_free (filepath);
                SEXP_free (attribute_);

                return PROBE_ENOELM;
        }

	probe_filebehaviors_canonicalize(&behaviors);

	switch (pthread_mutex_lock(mutex)) {
        case 0:
                break;
        default:
		dI("Can't lock mutex(%p): %u, %s.", mutex, errno, strerror(errno));

		SEXP_free(path);
		SEXP_free(filename);
		SEXP_free(filepath);
		SEXP_free(behaviors);
                SEXP_free(attribute_);

                return PROBE_EFATAL;
        }

        cbargs.ctx      = ctx;
	cbargs.error    = 0;
        cbargs.attr_ent = attribute_;

	const char *prefix = getenv("OSCAP_PROBE_ROOT");
	SEXP_init(&gr_lastpath);

	if ((ofts = oval_fts_open_prefixed(prefix, path, filename, filepath, behaviors, probe_ctx_getresult(ctx))) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			file_cb(prefix, ofts_ent->path, ofts_ent->file, &cbargs, &gr_lastpath);
			oval_ftsent_free(ofts_ent);
		}
		oval_fts_close(ofts);
	}
	if (!SEXP_emptyp(&gr_lastpath))
		SEXP_free_r(&gr_lastpath);

	err = 0;

	SEXP_free(path);
	SEXP_free(filename);
	SEXP_free(filepath);
	SEXP_free(behaviors);
        SEXP_free(attribute_);

	switch (pthread_mutex_unlock(mutex)) {
        case 0:
                break;
        default:
		dI("Can't unlock mutex(%p): %u, %s.", mutex, errno, strerror(errno));

                return PROBE_EFATAL;
        }

        return err;
}
