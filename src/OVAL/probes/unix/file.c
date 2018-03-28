/**
 * @file   file.c
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

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#undef OS_FREEBSD
#undef OS_LINUX
#undef OS_SOLARIS
#undef OS_SUNOS
#undef OS_WINDOWS
#undef OS_AIX

#if defined(__FreeBSD__)
# define OS_FREEBSD
#elif defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
# define OS_LINUX
#elif defined(sun) || defined(__sun)
# if defined(__SVR4) || defined(__svr4__)
#  define OS_SOLARIS
# else
#  define OS_SUNOS
# endif
#elif defined(_WIN32)
# define OS_WINDOWS
#elif defined(_AIX)
# define OS_AIX
#else
# error "Sorry, your OS isn't supported."
#endif

oval_schema_version_t over;

static SEXP_t *gr_true   = NULL, *gr_false  = NULL, *gr_t_reg  = NULL;
static SEXP_t *gr_t_dir  = NULL, *gr_t_lnk  = NULL, *gr_t_blk  = NULL;
static SEXP_t *gr_t_fifo = NULL, *gr_t_sock = NULL, *gr_t_char = NULL;
static SEXP_t  gr_lastpath;
#if defined(OS_SOLARIS)
static SEXP_t *gr_t_door = NULL, *gr_t_port = NULL;
#endif

static SEXP_t *se_filetype (mode_t mode)
{
        switch (mode & S_IFMT) {
        case S_IFREG:  return (gr_t_reg);
        case S_IFDIR:  return (gr_t_dir);
        case S_IFLNK:  return (gr_t_lnk);
        case S_IFBLK:  return (gr_t_blk);
        case S_IFIFO:  return (gr_t_fifo);
        case S_IFSOCK: return (gr_t_sock);
        case S_IFCHR:  return (gr_t_char);
#if defined(OS_SOLARIS)
	case S_IFDOOR: return (gr_t_door);
	case S_IFPORT: return (gr_t_port);
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

static rbt_t   *g_ID_cache     = NULL;
static uint32_t g_ID_cache_max = 0; /* 0 = unlimited */

static SEXP_t *ID_cache_get(int32_t id)
{
	SEXP_t *s_id = NULL, *s_id2 = NULL;

	if (rbt_i32_get(g_ID_cache, id, (void *)&s_id) == 0)
		return SEXP_ref(s_id); /* cache hit (first attempt) */

	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.8)) < 0) {
		s_id = SEXP_string_newf("%u", id);
	} else {
		s_id = SEXP_number_newu_32(id);
	}

	if (g_ID_cache_max == 0 || rbt_i32_size(g_ID_cache) < g_ID_cache_max) {
		if (rbt_i32_add(g_ID_cache, id, (void *)s_id, NULL) == 0)
			return SEXP_ref(s_id); /* insert succeeded */

		if (rbt_i32_get(g_ID_cache, id, (void *)&s_id2) == 0) {
			SEXP_free (s_id); /* cache hit (second attempt) */
			return SEXP_ref(s_id2);
		}
	}

	/* fallback */
	return (s_id);
}

static void ID_cache_init(uint32_t max)
{
	g_ID_cache_max = max;
	g_ID_cache     = rbt_i32_new();
}

static void ID_cache_free_cb(rbt_i32_node_t *n)
{
	SEXP_free(n->data);
}

static void ID_cache_free(void)
{
	rbt_i32_free_cb(g_ID_cache, ID_cache_free_cb);
	g_ID_cache     = NULL;
	g_ID_cache_max = 0;
}

static SEXP_t *get_atime(struct stat *st, SEXP_t *sexp)
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

static SEXP_t *get_ctime(struct stat *st, SEXP_t *sexp)
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

static SEXP_t *get_mtime(struct stat *st, SEXP_t *sexp)
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

#define MODEP(statp, bit) ((statp)->st_mode & (bit) ? gr_true : gr_false)

static SEXP_t *has_extended_acl(const char *path)
{
#if defined(HAVE_ACL_EXTENDED_FILE)
	int has_acl = acl_extended_file(path);
	if (has_acl == -1) {
		dD("Getting extended ACL for file '%s' has failed, %s", path, strerror(errno));
		return NULL;
	}
	return (has_acl == 1) ? gr_true : gr_false;
#elif defined(OS_SOLARIS)
	return acl_trivial(path) ? gr_true : gr_false;
#else
	return NULL;
#endif
}

static int file_cb (const char *p, const char *f, void *ptr)
{
        char path_buffer[PATH_MAX];
        SEXP_t *item;
        struct cbargs *args = (struct cbargs *) ptr;
        struct stat st;
        const char *st_path;

	if (f == NULL) {
		st_path = p;
	} else {
		snprintf (path_buffer, sizeof path_buffer, "%s/%s", p, f);
		st_path = path_buffer;
	}

        if (lstat (st_path, &st) == -1) {
                dI("lstat failed when processing %s: errno=%u, %s.", st_path, errno, strerror (errno));
		/*
		 * Whatever the reason of this lstat error (for example the file may
		 * have disappeared) we don't want it to stop the whole file tree walk;
		 * so we just don't report the error.
		 */
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

		se_usr_id = ID_cache_get(st.st_uid);
		se_grp_id = st.st_gid != st.st_uid ? ID_cache_get(st.st_gid) : SEXP_ref(se_usr_id);

		if (!SEXP_emptyp(&gr_lastpath)) {
			if (SEXP_strcmp(&gr_lastpath, p) != 0) {
				SEXP_free_r(&gr_lastpath);
				SEXP_string_new_r(&gr_lastpath, p, strlen(p));
			}
		} else
			SEXP_string_new_r(&gr_lastpath, p, strlen(p));

		if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.7)) < 0) {
			se_acl = NULL;
		} else {
			se_acl = has_extended_acl(st_path);
		}

                item = probe_item_create(OVAL_UNIX_FILE, NULL,
                                         "filepath", OVAL_DATATYPE_SEXP, se_filepath,
                                         "path",     OVAL_DATATYPE_SEXP,  &gr_lastpath,
                                         "filename", OVAL_DATATYPE_STRING, f == NULL ? "" : f,
                                         "type",     OVAL_DATATYPE_SEXP, se_filetype(st.st_mode),
                                         "group_id", OVAL_DATATYPE_SEXP, se_grp_id,
                                         "user_id",  OVAL_DATATYPE_SEXP, se_usr_id,
                                         "a_time",   OVAL_DATATYPE_SEXP, get_atime(&st, &se_atime_mem),
                                         "c_time",   OVAL_DATATYPE_SEXP, get_ctime(&st, &se_ctime_mem),
                                         "m_time",   OVAL_DATATYPE_SEXP, get_mtime(&st, &se_mtime_mem),
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
			probe_item_ent_add(item, "has_extended_acl", NULL, gr_true);
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

static pthread_mutex_t __file_probe_mutex;

int probe_offline_mode_supported()
{
	return PROBE_OFFLINE_CHROOT;
}

void *probe_init (void)
{
        /*
         * Initialize true/false global reference.
         */
        gr_true  = SEXP_number_newb (true);
        gr_false = SEXP_number_newb (false);

        /*
         * Initialize file type string references.
         * (Used by strfiletype())
         */
#define STR_REGULAR   "regular"
#define STR_DIRECTORY "directory"
#define STR_SYMLINK   "symbolic link"
#define STR_BLKSPEC   "block special"
#define STR_FIFO      "fifo"
#define STR_SOCKET    "socket"
#define STR_CHARSPEC  "character special"
#define STRLEN_PAIR(str) (str), strlen(str)
#if defined(OS_SOLARIS)
#define	STR_DOOR	"door"
#define	STR_PORT	"port"
#endif

        gr_t_reg  = SEXP_string_new (STRLEN_PAIR(STR_REGULAR));
        gr_t_dir  = SEXP_string_new (STRLEN_PAIR(STR_DIRECTORY));
        gr_t_lnk  = SEXP_string_new (STRLEN_PAIR(STR_SYMLINK));
        gr_t_blk  = SEXP_string_new (STRLEN_PAIR(STR_BLKSPEC));
        gr_t_fifo = SEXP_string_new (STRLEN_PAIR(STR_FIFO));
        gr_t_sock = SEXP_string_new (STRLEN_PAIR(STR_SOCKET));
        gr_t_char = SEXP_string_new (STRLEN_PAIR(STR_CHARSPEC));
#if defined(OS_SOLARIS)
        gr_t_door = SEXP_string_new (STRLEN_PAIR(STR_DOOR));
        gr_t_port = SEXP_string_new (STRLEN_PAIR(STR_PORT));
#endif

	SEXP_init(&gr_lastpath);

	/*
	 * Initialize ID cache
	 */
	ID_cache_init(10000);

        /*
         * Initialize mutex.
         */
        switch (pthread_mutex_init (&__file_probe_mutex, NULL)) {
        case 0:
                return ((void *)&__file_probe_mutex);
        default:
                dI("Can't initialize mutex: errno=%u, %s.", errno, strerror (errno));
        }
#if 0
	probe_setoption(PROBEOPT_VARREF_HANDLING, false, "path");
	probe_setoption(PROBEOPT_VARREF_HANDLING, false, "filename");
#endif
        return (NULL);
}

void probe_fini (void *arg)
{
        _A((void *)arg == (void *)&__file_probe_mutex);

        /*
         * Release global reference.
         */
        SEXP_vfree (gr_true, gr_false, gr_t_reg,
                    gr_t_dir, gr_t_lnk, gr_t_blk,
                    gr_t_fifo, gr_t_sock, gr_t_char,
                    NULL);

	if (!SEXP_emptyp(&gr_lastpath))
		SEXP_free_r(&gr_lastpath);

	/*
	 * Free ID cache
	 */
	ID_cache_free();

        /*
         * Destroy mutex.
         */
        (void) pthread_mutex_destroy (&__file_probe_mutex);

        return;
}

int probe_main (probe_ctx *ctx, void *mutex)
{
        SEXP_t *path, *filename, *behaviors, *filepath, *probe_in;
	int err;
        struct cbargs cbargs;
	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

        if (mutex == NULL) {
                return PROBE_EINIT;
	}

        _A(mutex == &__file_probe_mutex);

        probe_in  = probe_ctx_getobject(ctx);

	over = probe_obj_get_platform_schema_version(probe_in);
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

        switch (pthread_mutex_lock (&__file_probe_mutex)) {
        case 0:
                break;
        default:
                dI("Can't lock mutex(%p): %u, %s.", &__file_probe_mutex, errno, strerror (errno));

		SEXP_free(path);
		SEXP_free(filename);
		SEXP_free(filepath);
		SEXP_free(behaviors);

                return PROBE_EFATAL;
        }

        cbargs.ctx     = ctx;
	cbargs.error   = 0;

	if ((ofts = oval_fts_open(path, filename, filepath, behaviors, probe_ctx_getresult(ctx))) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			if (file_cb(ofts_ent->path, ofts_ent->file, &cbargs) != 0) {
				oval_ftsent_free(ofts_ent);
				break;
			}
			oval_ftsent_free(ofts_ent);
		}
		oval_fts_close(ofts);
	}

	err = 0;

	SEXP_free(path);
	SEXP_free(filename);
	SEXP_free(filepath);
	SEXP_free(behaviors);

        switch (pthread_mutex_unlock (&__file_probe_mutex)) {
        case 0:
                break;
        default:
                dI("Can't unlock mutex(%p): %u, %s.", &__file_probe_mutex, errno, strerror (errno));

                return PROBE_EFATAL;
        }

        return err;
}
