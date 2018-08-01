/**
 * @file   file_probe.c
 * @brief  file probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process a file_object as defined in OVAL 5.4 and 5.5.
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
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>

#if defined(HAVE_ACL_LIBACL_H)
# include <sys/types.h>
# include <acl/libacl.h>
#endif

#if defined(HAVE_SYS_ACL_H) && defined(OS_SOLARIS)
# include <sys/acl.h>
#endif

#include <probe/probe.h>
#include <probe/option.h>
#include "oval_fts.h"
#include "SEAP/generic/rbt/rbt.h"
#include "common/debug_priv.h"
#include "file_probe.h"
#include "_sexp-manip_r.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define FILE_SEPARATOR '/'

#define STR_REGULAR   "regular"
#define STR_DIRECTORY "directory"
#define STR_SYMLINK   "symbolic link"
#define STR_BLKSPEC   "block special"
#define STR_FIFO      "fifo"
#define STR_SOCKET    "socket"
#define STR_CHARSPEC  "character special"
#define STRLEN_PAIR(str) (str), strlen(str)
#if defined(OS_SOLARIS)
#define STR_DOOR      "door"
#define STR_PORT      "port"
#endif

struct gr_sexps {
	SEXP_t *gr_t_reg;
	SEXP_t *gr_t_dir;
	SEXP_t *gr_t_lnk;
	SEXP_t *gr_t_blk;
	SEXP_t *gr_t_fifo;
	SEXP_t *gr_t_sock;
	SEXP_t *gr_t_char;
#if defined(OS_SOLARIS)
	SEXP_t *gr_t_door;
	SEXP_t *gr_t_port;
#endif
};

static struct gr_sexps *gr_sexps_init()
{
	struct gr_sexps *s = malloc(sizeof(struct gr_sexps));
	s->gr_t_reg = SEXP_string_new(STRLEN_PAIR(STR_REGULAR));
	s->gr_t_dir = SEXP_string_new(STRLEN_PAIR(STR_DIRECTORY));
	s->gr_t_lnk = SEXP_string_new(STRLEN_PAIR(STR_SYMLINK));
	s->gr_t_blk = SEXP_string_new(STRLEN_PAIR(STR_BLKSPEC));
	s->gr_t_fifo = SEXP_string_new(STRLEN_PAIR(STR_FIFO));
	s->gr_t_sock = SEXP_string_new(STRLEN_PAIR(STR_SOCKET));
	s->gr_t_char = SEXP_string_new(STRLEN_PAIR(STR_CHARSPEC));
#if defined(OS_SOLARIS)
	s->gr_t_door = SEXP_string_new(STRLEN_PAIR(STR_DOOR));
	s->gr_t_port = SEXP_string_new(STRLEN_PAIR(STR_PORT));
#endif
	return s;
}

static void gr_sexps_free(struct gr_sexps *s)
{
	SEXP_free(s->gr_t_reg);
	SEXP_free(s->gr_t_dir);
	SEXP_free(s->gr_t_lnk);
	SEXP_free(s->gr_t_blk);
	SEXP_free(s->gr_t_fifo);
	SEXP_free(s->gr_t_sock);
	SEXP_free(s->gr_t_char);
#if defined(OS_SOLARIS)
	SEXP_free(s->gr_t_door);
	SEXP_free(s->gr_t_port);
#endif
	free(s);
}

static SEXP_t *se_filetype(struct gr_sexps *s, mode_t mode)
{
	switch (mode & S_IFMT) {
	case S_IFREG:
		return s->gr_t_reg;
	case S_IFDIR:
		return s->gr_t_dir;
	case S_IFLNK:
		return s->gr_t_lnk;
	case S_IFBLK:
		return s->gr_t_blk;
	case S_IFIFO:
		return s->gr_t_fifo;
	case S_IFSOCK:
		return s->gr_t_sock;
	case S_IFCHR:
		return s->gr_t_char;
#if defined(OS_SOLARIS)
	case S_IFDOOR:
		return s->gr_t_door;
	case S_IFPORT:
		return s->gr_t_port;
#endif
        default:
                abort ();
        }
        /* NOTREACHED */
        return (NULL);
}

struct cbargs {
        probe_ctx *ctx;
	int     error;
};

struct ID_cache {
	rbt_t *tree;
	uint32_t max;
};

static SEXP_t *ID_cache_get(struct ID_cache *cache, int32_t id, oval_schema_version_t over)
{
	SEXP_t *s_id = NULL, *s_id2 = NULL;

	if (rbt_i32_get(cache->tree, id, (void *)&s_id) == 0)
		return SEXP_ref(s_id); /* cache hit (first attempt) */

	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.8)) < 0) {
		s_id = SEXP_string_newf("%u", id);
	} else {
		s_id = SEXP_number_newu_32(id);
	}

	if (cache->max == 0 || rbt_i32_size(cache->tree) < cache->max) {
		if (rbt_i32_add(cache->tree, id, (void *)s_id, NULL) == 0)
			return SEXP_ref(s_id); /* insert succeeded */

		if (rbt_i32_get(cache->tree, id, (void *)&s_id2) == 0) {
			SEXP_free (s_id); /* cache hit (second attempt) */
			return SEXP_ref(s_id2);
		}
	}

	/* fallback */
	return (s_id);
}

static struct ID_cache *ID_cache_init(uint32_t max)
{
	struct ID_cache *cache = malloc(sizeof(struct ID_cache));
	cache->max = max;
	cache->tree = rbt_i32_new();
	return cache;
}

static void ID_cache_free_cb(rbt_i32_node_t *n)
{
	SEXP_free(n->data);
}

static void ID_cache_free(struct ID_cache *cache)
{
	rbt_i32_free_cb(cache->tree, ID_cache_free_cb);
	free(cache);
}

static SEXP_t *get_atime(struct stat *st, SEXP_t *sexp, oval_schema_version_t over)
{
	uint64_t t = (
#if defined(OS_FREEBSD)
		(uint64_t) st->st_atimespec.tv_sec
#elif defined(OS_LINUX) || defined(OS_SOLARIS)
		(uint64_t) st->st_atim.tv_sec
#else /* Use the legacy field */
		(uint64_t) st->st_atime
#endif
		);

	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.8)) < 0) {
		return SEXP_string_newf_r(sexp, "%llu", (long long unsigned) t);
	} else {
		return SEXP_number_newu_64_r(sexp, t);
	}
}

static SEXP_t *get_ctime(struct stat *st, SEXP_t *sexp, oval_schema_version_t over)
{
	uint64_t t = (
#if defined(OS_FREEBSD)
		(uint64_t) st->st_ctimespec.tv_sec
#elif defined(OS_LINUX) || defined(OS_SOLARIS)
		(uint64_t) st->st_ctim.tv_sec
#else /* Use the legacy field */
		(uint64_t) st->st_ctime
#endif
		);

	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.8)) < 0) {
		return SEXP_string_newf_r(sexp, "%llu", (long long unsigned) t);
	} else {
		return SEXP_number_newu_64_r(sexp, t);
	}
}

static SEXP_t *get_mtime(struct stat *st, SEXP_t *sexp, oval_schema_version_t over)
{
	uint64_t t = (
#if defined(OS_FREEBSD)
		(uint64_t) st->st_mtimespec.tv_sec
#elif defined(OS_LINUX) || defined(OS_SOLARIS)
		(uint64_t) st->st_mtim.tv_sec
#else /* Use the legacy field */
		(uint64_t) st->st_mtime
#endif
		);

	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.8)) < 0) {
		return SEXP_string_newf_r(sexp, "%llu", (long long unsigned) t);
	} else {
		return SEXP_number_newu_64_r(sexp, t);
	}
}

static SEXP_t *get_size(struct stat *st, SEXP_t *sexp)
{
	switch (sizeof(st->st_size)) {
	case 8:
		return SEXP_number_newu_64_r(sexp, st->st_size);
	case 4:
		return SEXP_number_newu_32_r(sexp, st->st_size);
	default: /* Should not happen. */
		abort();

		/* NOTREACHED */
		return NULL;
	}
}

static SEXP_t *MODEP(struct stat *statp, unsigned int bit)
{
	if (statp->st_mode & bit) {
		return SEXP_number_newb(true);
	} else {
		return SEXP_number_newb(false);
	}
}

static SEXP_t *has_extended_acl(const char *path)
{
#if defined(HAVE_ACL_EXTENDED_FILE)
	int has_acl = acl_extended_file(path);
	if (has_acl == -1) {
		dD("Getting extended ACL for file '%s' has failed, %s", path, strerror(errno));
		return NULL;
	}
	return (has_acl == 1) ? SEXP_number_newb(true) : SEXP_number_newb(false);
#elif defined(OS_SOLARIS)
	return acl_trivial(path) ? SEXP_number_newb(true) : SEXP_number_newb(false);
#else
	return NULL;
#endif
}

static int file_cb(const char *prefix, const char *p, const char *f, void *ptr, oval_schema_version_t over, struct ID_cache *cache, struct gr_sexps *grs, SEXP_t *gr_lastpath)
{
        char path_buffer[PATH_MAX];
        SEXP_t *item;
        struct cbargs *args = (struct cbargs *) ptr;
        struct stat st;
        const char *st_path;

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

	char *st_path_with_prefix = oscap_path_join(prefix, st_path);
	if (lstat(st_path_with_prefix, &st) == -1) {
                dI("lstat failed when processing %s: errno=%u, %s.", st_path, errno, strerror (errno));
		/*
		 * Whatever the reason of this lstat error (for example the file may
		 * have disappeared) we don't want it to stop the whole file tree walk;
		 * so we just don't report the error.
		 */
		free(st_path_with_prefix);
		return 0;
        } else {
                SEXP_t *se_usr_id, *se_grp_id;
                SEXP_t  se_atime_mem, se_ctime_mem, se_mtime_mem, se_size_mem;
		SEXP_t *se_filepath, *se_acl;

		if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.6)) < 0
		    || f == NULL) {
			se_filepath = NULL;
		} else {
			se_filepath = SEXP_string_newf("%s", st_path);
		}

		se_usr_id = ID_cache_get(cache, st.st_uid, over);
		se_grp_id = st.st_gid != st.st_uid ? ID_cache_get(cache, st.st_gid, over) : SEXP_ref(se_usr_id);

		if (!SEXP_emptyp(gr_lastpath)) {
			if (SEXP_strcmp(gr_lastpath, p) != 0) {
				SEXP_free_r(gr_lastpath);
				SEXP_string_new_r(gr_lastpath, p, strlen(p));
			}
		} else
			SEXP_string_new_r(gr_lastpath, p, strlen(p));

		if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.7)) < 0) {
			se_acl = NULL;
		} else {
			se_acl = has_extended_acl(st_path_with_prefix);
		}
		free(st_path_with_prefix);

                item = probe_item_create(OVAL_UNIX_FILE, NULL,
                                         "filepath", OVAL_DATATYPE_SEXP, se_filepath,
                                         "path",     OVAL_DATATYPE_SEXP, gr_lastpath,
                                         "filename", OVAL_DATATYPE_STRING, f == NULL ? "" : f,
                                         "type",     OVAL_DATATYPE_SEXP, se_filetype(grs, st.st_mode),
                                         "group_id", OVAL_DATATYPE_SEXP, se_grp_id,
                                         "user_id",  OVAL_DATATYPE_SEXP, se_usr_id,
                                         "a_time",   OVAL_DATATYPE_SEXP, get_atime(&st, &se_atime_mem, over),
                                         "c_time",   OVAL_DATATYPE_SEXP, get_ctime(&st, &se_ctime_mem, over),
                                         "m_time",   OVAL_DATATYPE_SEXP, get_mtime(&st, &se_mtime_mem, over),
                                         "size",     OVAL_DATATYPE_SEXP, get_size(&st, &se_size_mem),
                                         "suid",     OVAL_DATATYPE_SEXP, MODEP(&st, S_ISUID),
                                         "sgid",     OVAL_DATATYPE_SEXP, MODEP(&st, S_ISGID),
                                         "sticky",   OVAL_DATATYPE_SEXP, MODEP(&st, S_ISVTX),
                                         "uread",    OVAL_DATATYPE_SEXP, MODEP(&st, S_IRUSR),
                                         "uwrite",   OVAL_DATATYPE_SEXP, MODEP(&st, S_IWUSR),
                                         "uexec",    OVAL_DATATYPE_SEXP, MODEP(&st, S_IXUSR),
                                         "gread",    OVAL_DATATYPE_SEXP, MODEP(&st, S_IRGRP),
                                         "gwrite",   OVAL_DATATYPE_SEXP, MODEP(&st, S_IWGRP),
                                         "gexec",    OVAL_DATATYPE_SEXP, MODEP(&st, S_IXGRP),
                                         "oread",    OVAL_DATATYPE_SEXP, MODEP(&st, S_IROTH),
                                         "owrite",   OVAL_DATATYPE_SEXP, MODEP(&st, S_IWOTH),
                                         "oexec",    OVAL_DATATYPE_SEXP, MODEP(&st, S_IXOTH),
					 "has_extended_acl", OVAL_DATATYPE_SEXP, se_acl,
                                         NULL);
		if (se_acl == NULL) {
			probe_item_ent_add(item, "has_extended_acl", NULL, SEXP_number_newb(true));
			probe_itement_setstatus(item, "has_extended_acl", 1, SYSCHAR_STATUS_DOES_NOT_EXIST);
		}

                SEXP_free(se_grp_id);
                SEXP_free(se_usr_id);
		SEXP_free(se_filepath);
                SEXP_free_r(&se_atime_mem);
                SEXP_free_r(&se_ctime_mem);
                SEXP_free_r(&se_mtime_mem);
                SEXP_free_r(&se_size_mem);

		/*
		 * Stop collecting if we hit the memory usage limit
		 * (return code == 2)
		 */
                return probe_item_collect(args->ctx, item) == 2 ? 1 : 0;
        }

        return (0);
}


int file_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_OWN;
}

void *file_probe_init(void)
{
        /*
         * Initialize mutex.
         */
	pthread_mutex_t *file_probe_mutex = malloc(sizeof(pthread_mutex_t));
	switch (pthread_mutex_init (file_probe_mutex, NULL)) {
        case 0:
		return ((void *)file_probe_mutex);
        default:
		free(file_probe_mutex);
                dI("Can't initialize mutex: errno=%u, %s.", errno, strerror (errno));
        }
#if 0
	probe_setoption(PROBEOPT_VARREF_HANDLING, false, "path");
	probe_setoption(PROBEOPT_VARREF_HANDLING, false, "filename");
#endif
        return (NULL);
}

void file_probe_fini(void *arg)
{
        /*
         * Destroy mutex.
         */
	(void) pthread_mutex_destroy((pthread_mutex_t *)arg);
	free(arg);
}

int file_probe_main(probe_ctx *ctx, void *mutex)
{
        SEXP_t *path, *filename, *behaviors, *filepath, *probe_in;
	int err;
        struct cbargs cbargs;
	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

        if (mutex == NULL) {
                return PROBE_EINIT;
	}

        probe_in  = probe_ctx_getobject(ctx);

	oval_schema_version_t over = probe_obj_get_platform_schema_version(probe_in);
        path      = probe_obj_getent (probe_in, "path",      1);
        filename  = probe_obj_getent (probe_in, "filename",  1);
        behaviors = probe_obj_getent (probe_in, "behaviors", 1);
        filepath =  probe_obj_getent (probe_in, "filepath", 1);

	/* we want either path+filename or filepath */
        if ( (path == NULL || filename == NULL) && filepath==NULL ) {
                SEXP_free (behaviors);
                SEXP_free (path);
                SEXP_free (filename);
                SEXP_free (filepath);

                return PROBE_ENOELM;
        }

	probe_filebehaviors_canonicalize(&behaviors);

        switch (pthread_mutex_lock (mutex)) {
        case 0:
                break;
        default:
                dI("Can't lock mutex(%p): %u, %s.", mutex, errno, strerror (errno));

		SEXP_free(path);
		SEXP_free(filename);
		SEXP_free(filepath);
		SEXP_free(behaviors);

                return PROBE_EFATAL;
        }

        cbargs.ctx     = ctx;
	cbargs.error   = 0;

	const char *prefix = getenv("OSCAP_PROBE_ROOT");
	SEXP_t gr_lastpath;
	SEXP_init(&gr_lastpath);
	struct ID_cache *cache = ID_cache_init(10000);
	struct gr_sexps *grs = gr_sexps_init();

	if ((ofts = oval_fts_open_prefixed(prefix, path, filename, filepath, behaviors, probe_ctx_getresult(ctx))) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			if (file_cb(prefix, ofts_ent->path, ofts_ent->file, &cbargs, over, cache, grs, &gr_lastpath) != 0) {
				oval_ftsent_free(ofts_ent);
				break;
			}
			oval_ftsent_free(ofts_ent);
		}
		oval_fts_close(ofts);
	}
	ID_cache_free(cache);
	gr_sexps_free(grs);

	if (!SEXP_emptyp(&gr_lastpath))
		SEXP_free_r(&gr_lastpath);

	err = 0;

	SEXP_free(path);
	SEXP_free(filename);
	SEXP_free(filepath);
	SEXP_free(behaviors);

        switch (pthread_mutex_unlock (mutex)) {
        case 0:
                break;
        default:
                dI("Can't unlock mutex(%p): %u, %s.", mutex, errno, strerror (errno));

                return PROBE_EFATAL;
        }

        return err;
}
