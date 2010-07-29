/**
 * @file   xinetd.c
 * @brief  xinetd probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
 *   Daniel Kopecek <dkopecek@redhat.com>
 */

#include <config.h>
#include <probe-api.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>
#include <common/assume.h>
#include <alloc.h>
#include <bfind.h>
#include <common/debug_priv.h>
#include "../SEAP/generic/rbt/rbt.h"

#define dI(...) oscap_dlprintf(DBG_I, __VA_ARGS__)
#define dW(...) oscap_dlprintf(DBG_W, __VA_ARGS__)
#define dE(...) oscap_dlprintf(DBG_E, __VA_ARGS__)

#define PATH_SEPARATOR '/'

/*
 * the code bellow implements a minimal xinetd configuration parser
 */

#define XICFG_PARSER_IGNORE_UNKNOWN 1
#define XICFG_PARSER_IGNORE_INVSECT 0
#define XICFG_PARSER_MAXFILESIZE    655360 /**< hard limit for configuration file size; 640K ought to be enough for anybody */

struct xiconf_attr {
	char    *name;   /* name of the attribute */
	size_t   offset; /* offset of the corresponding member in the xinetd_service_t structure */
	int (*op_assign)(void *, char *); /*  = */
	int (*op_insert)(void *, char *); /* += */
	int (*op_remove)(void *, char *); /* -= */
	int      sections; /* which type of sections are valid */
	int      pass_arg;
};

#define XIATTR_SECTION_NIL   0x01
#define XIATTR_SECTION_DEF   0x02
#define XIATTR_SECTION_SRV   0x04

#define XIATTR_OPARG_LOCAL   0 /* local service attribute */
#define XIATTR_OPARG_GLOBAL  1 /* xiconf_t structure */

#define XICONF_SECTION_DEFAULTS XIATTR_SECTION_DEF
#define XICONF_SECTION_SERVICE  XIATTR_SECTION_SRV

#define OPRES_SUCCESS 0
#define OPRES_ESYNTAX 1 /* Invalid syntax */
#define OPRES_EINVAL  2 /* Invalid operation */
#define OPRES_EFAULT  3 /* Invalid memory access */
#define OPRES_ELIMIT  4 /* Limit reached */

typedef struct {
	/*
	 * Attributes needed by the xinetd probe
	 */
	char    *id;
	char    *type;
	char    *flags;
	char    *socket_type;
	char    *name;
	char    *protocol;
	char    *user;
	char    *server;
	char    *server_args;
	char    *only_from;
	char    *no_access;

	uint16_t port;

	int disable;
	int wait;
	int def_disabled;
	int def_enabled;

	bool internal;
	bool unlisted;
	bool rpc;

	/*
	 * Unused attributes; Maybe we will need them in some future version.
	 */
#if 0
	bool groups;
	bool mdns;
	bool enabled;
	char *log_on_success;
	char *log_on_failure;
	int32_t instances;
	int32_t nice;
	int32_t rpc_number;
	int32_t per_source;
	float max_load;
	int64_t rlimit_as;
	int64_t rlimit_cpu;
	int64_t rlimit_data;
	int64_t rlimit_rss;
	int64_t rlimit_stack;
	char *group;
	char *access_times;
	char *log_type;
	char *rpc_version;
	char *env;
	char *passenv;
	char *redirect;
	char *bind;
	char *interface;
	char *banner;
	char *banner_success;
	char *banner_fail;
	char *cps;
	char *umask;
	char *deny_time;
	char *libwrap;
#endif /* 0 */
} xiconf_service_t;

typedef struct {
	int             fd;    /**< file descriptor */
	char           *cpath; /**< path to the configuration file */
	time_t          mtime; /**< modification time of the file */
	rbt_str_node_t *nodes; /**< node pointers in the service tree that belong to this file; */
	size_t          count; /**< number of node pointers */
	char           *inmem; /**< contents of the file mmaped or copied into memory */
	size_t          inlen; /**< in-memory size */
	size_t          inoff; /**< in-memory offset */
	int             flags; /**< flags; see bellow */
	unsigned int    depth; /**< include depth */
} xiconf_file_t;

#define XICONF_FILE_MMAPED  0x00000001 /**< try to mmap the file */
#define XICONF_FILE_PERSIST 0x00000002 /**< keep the file open/mmaped */
#define XICONF_FILE_DEAD    0x00000004 /**< this item can be skipped/deleted/reused for a different file */

typedef struct {
	xiconf_file_t   **cfile; /**< */
	size_t            count; /**< */
	rbt_t            *stree; /**< service tree */
	xiconf_service_t *defaults; /**< parsed defaults for services */
} xiconf_t;

xiconf_t *xiconf_parse(const char *path, unsigned int max_depth);
void xiconf_free(xiconf_t *xiconf);
int xiconf_update(xiconf_t *xiconf);
int xiconf_parse_section(xiconf_t *xiconf, xiconf_file_t *xifile, int type, char *name);
int xiconf_parse_service(xiconf_file_t *file, xiconf_service_t *service);
int xiconf_parse_defaults(xiconf_file_t *file, xiconf_service_t *defaults, rbt_t *stree);
xiconf_service_t *xiconf_getservice(xiconf_t *xiconf, char *name, char *prot);

int op_assign_bool(void *var, char *val);
int op_assign_num(void *var, char *val);
int op_assign_str(void *var, char *val);
int op_assign_strl(void *var, char *val);
int op_insert_strl(void *var, char *val);
int op_remove_strl(void *var, char *val);

#ifndef STR
# define STR(s) #s
#endif

#define XICONF_ATTR(name, op_a, op_r, op_i) \
	{ STR(name), offsetof(xiconf_service_t, name), (op_a), (op_r), (op_i) }

/*
 * Service only attribute.
 *
 * Flags set:
 *  - service section flag
 *
 * Functions implementing the operators get the pointer to the
 * corresponding xiconf_service_t structure item pointer (using
 * offsetof) as their first argument.
 */
#define XICONF_SO_ATTR(name, op_a, op_r, op_i) \
	{ STR(name), offsetof(xiconf_service_t, name), (op_a), (op_r), (op_i), XIATTR_SECTION_SRV, XIATTR_OPARG_LOCAL }

/*
 * Defaults only attribute.
 *
 * Flags set:
 *  - defaults section flag
 *
 * Type of the first argument to be passed to the operator
 * functions has to be specified in the last argument.
 * Recognized types are:
 *   XIATTR_OPTARG_LOCAL  - ptr(service + offsetof(attr_name))
 *   XIATTR_OPTARG_GLOBAL - pointer to the xiconf_t structure
 */
#define XICONF_DO_ATTR(name, op_a, op_r, op_i, off, arg) \
	{ STR(name), (off), (op_a), (op_r), (op_i), XIATTR_SECTION_DEF, (arg) }

/*
 * Service attribute which default attribute can be specified
 * in the defaults section.
 *
 * Flags set:
 *  - service section flag
 *  - defaults section flag
 *
 * Functions implementing the operators get the pointer to the
 * corresponding xiconf_service_t structure item pointer (using
 * offsetof) as their first argument.
 */
#define XICONF_SD_ATTR(name, op_a, op_r, op_i) \
	{ STR(name), offsetof(xiconf_service_t, name), (op_a), (op_r), (op_i), XIATTR_SECTION_SRV|XIATTR_SECTION_DEF, XIATTR_OPARG_LOCAL }

/*
 * Service attribute table; keep this table sorted by attribute name
 */
struct xiconf_attr xiattr_table[] = {
	/*
	 * XICONF_ATTR(attr_name, op_assign, op_insert, op_remove)
	 */
	//XICONF_ATTR(access_times, NULL, NULL, NULL),
	//XICONF_ATTR(banner, NULL, NULL, NULL),
	//XICONF_ATTR(banner_fail, NULL, NULL, NULL),
	//XICONF_ATTR(banner_success, NULL, NULL, NULL),
	//XICONF_ATTR(bind, NULL, NULL, NULL),
	//XICONF_ATTR(cps, NULL, NULL, NULL),
	//XICONF_ATTR(deny_time, NULL, NULL, NULL),

	XICONF_SO_ATTR(disable,
		       &op_assign_bool, NULL, NULL),
	XICONF_DO_ATTR(disabled,
		       NULL, NULL, NULL, 0, XIATTR_OPARG_GLOBAL),
	XICONF_DO_ATTR(enabled,
		       NULL, NULL, NULL, 0, XIATTR_OPARG_GLOBAL),

	//XICONF_ATTR(env, NULL, NULL, NULL),

	XICONF_SO_ATTR(flags,
		       NULL, NULL, NULL),

	//XICONF_ATTR(group, NULL, NULL, NULL),
	//XICONF_ATTR(groups, NULL, NULL, NULL),

	XICONF_SO_ATTR(id,
		       &op_assign_str, NULL, NULL),

	//XICONF_ATTR(instances, NULL, NULL, NULL),
	//XICONF_ATTR(interface, NULL, NULL, NULL),
	//XICONF_ATTR(libwrap, NULL, NULL, NULL),
	//XICONF_ATTR(log_on_failure, NULL, NULL, NULL),
	//XICONF_ATTR(log_on_success, NULL, NULL, NULL),
	//XICONF_ATTR(log_type, NULL, NULL, NULL),
	//XICONF_ATTR(max_load, NULL, NULL, NULL),
	//XICONF_ATTR(mdns, NULL, NULL, NULL),
	//XICONF_ATTR(nice, NULL, NULL, NULL),

	XICONF_SD_ATTR(no_access,
		       &op_assign_strl, &op_remove_strl, &op_insert_strl),
	XICONF_SD_ATTR(only_from,
		       &op_assign_strl, &op_remove_strl, &op_insert_strl),

	//XICONF_ATTR(passenv, NULL, NULL, NULL),
	//XICONF_ATTR(per_source, NULL, NULL, NULL),

	XICONF_SO_ATTR(port,
		       &op_assign_num, NULL, NULL),
	XICONF_SO_ATTR(protocol,
		       &op_assign_str, NULL, NULL),

	//XICONF_ATTR(redirect, NULL, NULL, NULL),
	//XICONF_ATTR(rlimit_as, NULL, NULL, NULL),
	//XICONF_ATTR(rlimit_cpu, NULL, NULL, NULL),
	//XICONF_ATTR(rlimit_data, NULL, NULL, NULL),
	//XICONF_ATTR(rlimit_rss, NULL, NULL, NULL),
	//XICONF_ATTR(rlimit_stack, NULL, NULL, NULL),
	//XICONF_ATTR(rpc_number, NULL, NULL, NULL),
	//XICONF_ATTR(rpc_version, NULL, NULL, NULL),

	XICONF_SO_ATTR(server,
		       &op_assign_str, NULL, NULL),
	XICONF_SO_ATTR(server_args,
		       NULL, NULL, NULL),
	XICONF_SO_ATTR(socket_type,
		       NULL, NULL, NULL),
	XICONF_SO_ATTR(type,
		       NULL, NULL, NULL),

	//XICONF_ATTR(umask, NULL, NULL, NULL),

	XICONF_SO_ATTR(user,
		       &op_assign_str, NULL, NULL),
	XICONF_SO_ATTR(wait,
		       &op_assign_bool, NULL, NULL)
};

#define XIATTR_TABLE_COUNT (sizeof xiattr_table / sizeof(struct xiconf_attr))

static int xiattr_cmp(void *a, void *b)
{
	return strcmp((char *)a, ((struct xiconf_attr *)b)->name);
}

/*
 * Allocate and initialize a xinetd configuration structure
 */
static xiconf_t *xiconf_new(void)
{
	xiconf_t *xiconf;

	xiconf = oscap_talloc(xiconf_t);
	xiconf->cfile = oscap_alloc(sizeof(xiconf_file_t *));
	xiconf->count = 0;
	xiconf->stree = rbt_str_new();
	xiconf->defaults = NULL;

	return (xiconf);
}

static xiconf_service_t *xiconf_service_new(void)
{
	xiconf_service_t *service;

	service = oscap_talloc(xiconf_service_t);

	service->id    = NULL;
	service->type  = NULL;
	service->flags = NULL;
	service->socket_type = NULL;
	service->name     = NULL;
	service->protocol = NULL;
	service->user   = NULL;
	service->server = NULL;
	service->server_args = NULL;
	service->only_from   = NULL;
	service->no_access   = NULL;
	service->port    = 0;
	service->disable = 0;
	service->wait    = 0;
	service->def_disabled = 0;
	service->def_enabled  = 0;

	service->internal = 0;
	service->unlisted = 0;
	service->rpc      = 0;

	return (service);
}

void xiconf_free(xiconf_t *xiconf)
{
	register size_t i;

	for (i = 0; i < xiconf->count; ++i) {
		oscap_free(xiconf->cfile[i]->cpath);
		/* TODO: rbt_del(); */
	}

	oscap_free(xiconf->cfile);
	/* TODO: rbt_free();   */
	/* TODO: free defaults */
	oscap_free(xiconf);

	return;
}

static void xiconf_service_free(xiconf_service_t *service)
{
	return;
}

static xiconf_file_t *xiconf_read(const char *path, int flags)
{
	int fd;
	struct stat st;
	xiconf_file_t *file;

	fd = open(path, O_RDONLY);

	if (fd < 0)
		return (NULL);

	if (fstat(fd, &st) < 0) {
		close (fd);
		return (NULL);
	}

	if (st.st_size > XICFG_PARSER_MAXFILESIZE) {
		/* You're probably trying to parse a wrong file... or 640k is not enough for you. */
		close (fd);

		return (NULL);
	}

	file = oscap_talloc(xiconf_file_t);

	/* initialize items that don't need to have extra memory allocated for them */
	file->fd    = fd;
	file->inlen = (size_t)st.st_size;
	file->inoff = 0;
	file->mtime = st.st_mtime;
	file->nodes = NULL;
	file->count = 0;
	file->flags = flags;

#if !defined(XICFG_PARSER_WITHOUT_MMAP) && defined(HAVE_MMAP)
	file->inmem = mmap (NULL, file->inlen, PROT_READ, MAP_SHARED, file->fd, 0);

	if (file->inmem == MAP_FAILED)
#endif
	{
		/* fallback method - copy the contents into memory */

		file->inmem = oscap_alloc(file->inlen);

		if (read (file->fd, file->inmem, file->inlen) != (ssize_t)file->inlen) {
			/* Can't read the contents of the file */
			close (fd);
			oscap_free(file);
			return (NULL);
		}

		/*
		 * We have the contents of the file and don't need the fd anymore.
		 * Unless the FILE_PERSIST flag is set, close fd now. The allocated
		 * memory will be freed after the parsing is done
		 */
		if (!(flags & XICONF_FILE_PERSIST)) {
			close(fd);
			file->fd = -1;
		}
	}

	/* now initialize item that do need extra memory to be allocated */
	file->cpath = strdup(path);

	return (file);
}

#define tmpbuf_def(size) char __tmpbuf[size]
#define tmpbuf_get(size) (((sizeof __tmpbuf)/sizeof(char))<(size)?oscap_alloc(sizeof(char)*(size)):__tmpbuf)
#define tmpbuf_free(ptr) do { if ((ptr) != __tmpbuf) oscap_free(ptr); (ptr) = NULL; } while(0)

xiconf_t *xiconf_parse(const char *path, unsigned int max_depth)
{
	xiconf_t      *xiconf;
	xiconf_file_t *xifile; /* pointer to the currently parsed file */
	size_t         findex; /* index of currently parsed file */

	tmpbuf_def(512);

	if (path == NULL)
		return (NULL);

	xiconf = xiconf_new();

	if (xiconf == NULL)
		return (NULL);

	xifile = xiconf_read(path, 0);

	if (xifile == NULL) {
		xiconf_free(xiconf);
		return (NULL);
	}

	xifile->depth = 0;
	xiconf->cfile[0] = xifile;
	xiconf->count = 1;

	for (findex = 0; findex < xiconf->count; ++findex) {
		char  *buffer;
		register char  *l_pbeg, *l_pend;
		register size_t bufidx,  l_size;

		/* sanity checks */
		if (xifile->depth > max_depth && xiconf->count > findex)
			abort();

		xifile = xiconf->cfile[findex];

		for (;xifile->inoff < xifile->inlen;) {
			/* locate end-of-line & compute the line lenght */
			l_pbeg = xifile->inmem + xifile->inoff;
			l_pend = strchr(l_pbeg, '\n');

			if (l_pend == NULL) {
				l_pend = l_pbeg + xifile->inlen;
				l_size = xifile->inlen;
			} else
				l_size = (size_t)(l_pend - l_pbeg);

			xifile->inoff += l_size + 1; /* update the next line offset */

			/*
			 * copy the current line into a separate buffer
			 * where we can modify the data (change spaces to
			 * newlines, etc.)
			 */
			buffer = tmpbuf_get(l_size + 1);
			bufidx = 0;
			memcpy (buffer, l_pbeg, l_size);
			buffer[l_size] = ' ';
			*strchr(buffer,  ' ') = '\0';

			/* skip whitespaces before the keyword */
			while(isspace(buffer[bufidx])) ++bufidx;

			switch(buffer[bufidx]) {
			case  's':
				if (strcmp("ervice",  buffer + bufidx + 1) == 0) {
					/* skip the service keyword */
					bufidx += 8;

					if (bufidx >= l_size) {
						tmpbuf_free(buffer);
						return (NULL);
					}

					/* skip whitespaces before the keyword */
					while(isspace(buffer[bufidx])) ++bufidx;

					if (bufidx >= l_size) {
						tmpbuf_free(buffer);
						return (NULL);
					}

					*strchr(buffer + bufidx, ' ') = '\0';

					if (xiconf_parse_section (xiconf, xifile, XICONF_SECTION_SERVICE, buffer + bufidx) != 0) {
						tmpbuf_free(buffer);
						return (NULL);
					}
				}
				break;
			case  'd':
				if (strcmp("efaults", buffer + bufidx + 1) == 0) {
					if (xiconf_parse_section (xiconf, xifile, XICONF_SECTION_DEFAULTS, buffer + bufidx) != 0) {
						tmpbuf_free(buffer);
						return (NULL);
					}
				}
				break;
			case '#':
				/* comment */
			case '\0':
				/* blank line */
				break;
			case 'i':
				if (strcmp("ncludedir", buffer + bufidx + 1) == 0) {
					xiconf_file_t *xifile_inc;
					char          *incldir;
					size_t         incllen;
					char           pathbuf[PATH_MAX+1];
					DIR           *dirfp;
					struct dirent  dent, *dentp = NULL;

					bufidx += strlen("includedir");
					while(isspace(buffer[bufidx])) ++bufidx;
					incldir = buffer + bufidx + 1;
					buffer[l_size] = '\0';

					if (*incldir == '\0') {
						dW("Found includedir directive without an argument!\n");
						break;
					}

					if (xifile->depth + 1 > max_depth) {
						dE("include depth limit reached: %u\n", max_depth);
						break;
					}

					dI("includedir open: %s\n", incldir);
					dirfp = opendir (incldir);

					if (dirfp == NULL) {
						dW("Can't open includedir: %s; %d, %s.\n", incldir, errno, strerror (errno));
						break;
					}

					strcpy(pathbuf, incldir);
					incllen = strlen(incldir);

					if (pathbuf[incllen - 1] != PATH_SEPARATOR) {
						if (incllen < PATH_MAX) {
							pathbuf[incllen++] =  '/';
							pathbuf[incllen  ] = '\0';
						} else {
							dE("Length of the includedir argument is out of range: len=%zu, max=%zu\n",
							   incllen, PATH_MAX);
							closedir(dirfp);
							break;
						}
					}

					for (;;) {
						if (readdir_r (dirfp, &dent, &dentp) != 0) {
							dW("Can't read directory: %s; %d, %s.\n", incldir, errno, strerror (errno));
							closedir (dirfp);
							break;
						}

						if (dentp == NULL)
							break;

						if (fnmatch ("*~",  dent.d_name, FNM_PATHNAME) == 0 ||
						    fnmatch ("*.*", dent.d_name, FNM_PATHNAME) == 0)
						{
							dI("Skipping: %s\n", dent.d_name);
							continue;
						}

						strcpy(pathbuf + incllen, dent.d_name);
						dI("Reading included file: %s\n", pathbuf);
						xifile_inc = xiconf_read (pathbuf, 0);

						if (xifile_inc == NULL) {
							dW("Failed to read file: %s\n", pathbuf);
							continue;
						}

						xifile_inc->depth = xifile->depth + 1;
						xiconf->cfile = oscap_realloc(xiconf->cfile, sizeof(xiconf_file_t) * ++xiconf->count);
						xiconf->cfile[xiconf->count - 1] = xifile_inc;

						dI("Added new file to the cfile queue: %s; fi=%zu, ni=%zu\n",
						   pathbuf, findex, xiconf->count - 1);
					}

					dI("includedir close: %s\n", incldir);
					closedir(dirfp);
				} else if (strcmp("nclude", buffer + bufidx + 1) == 0) {
					// read the file and push into the stack
				}
				break;
			default:
				/* something */
				return (NULL);
			}

			tmpbuf_free(buffer);
		}
	}

	return (xiconf);
}

int xiconf_update(xiconf_t *xiconf)
{
	dI("Not implemented yet.\n");
	return (0);
}

static int xiconf_service_merge_and_free(xiconf_service_t *dst, xiconf_service_t *src)
{
	oscap_free(src);
	return (0);
}

static int xiconf_service_merge_defaults(xiconf_service_t *dst, xiconf_service_t *def)
{
	return (0);
}

int xiconf_parse_section(xiconf_t *xiconf, xiconf_file_t *xifile, int type, char *name)
{
	xiconf_service_t *snew, *scur;
	char *buffer;
	register size_t bufidx;
	char  *l_pbeg;
	char  *l_pend;
	char  *l_next;
	size_t l_size;
	char *key, *op, *opval;
	void **opvar;
	int (*opfun)(void *, char *);
	struct xiconf_attr *xiattr;
	tmpbuf_def(128);

	if (type == XICONF_SECTION_DEFAULTS && xiconf->defaults != NULL) {
		dE("defaults section was already parsed!\n");
		return (-1);
	} else if (type == XICONF_SECTION_SERVICE && name == NULL) {
		dE("Don't know how to handle anonymous service sections!\n");
		return (-1);
	}

	/*
	 * Find the left brace. Only the left brace, whitespace
	 * characters before and after it and a newline character
	 * at the end is allowed here.
	 */
	while(isspace(xifile->inmem[xifile->inoff]))
		++xifile->inoff;
	if (xifile->inmem[xifile->inoff] != '{')
		return(-1);
	else
		++xifile->inoff;
	while(xifile->inmem[xifile->inoff] != '\n' &&
	      isspace(xifile->inmem[xifile->inoff]))
		++xifile->inoff;
	if (xifile->inmem[xifile->inoff] != '\n')
		return (-1);
	else
		++xifile->inoff;

	/*
	 * Now there should be only <keyword> <op> <value> items, comments
	 * blank lines or a right brace which signals and of the section.
	 */
	snew = xiconf_service_new();
	snew->name = strdup(name);

	for (;;) {
		/*
		 * Find out the line boundaries.
		 */
		l_pbeg = xifile->inmem + xifile->inoff;
		l_pend = strchr(l_pbeg, '\n');

		if (l_pend == NULL) {
			l_pend = l_pbeg + xifile->inlen;
			l_size = xifile->inlen;
		} else
			l_size = (size_t)(l_pend - l_pbeg);

		xifile->inoff += l_size + 1; /* update the next line offset */

		/*
		 * copy the current line into a separate buffer
		 * where we can modify the data (change spaces to
		 * newlines, etc.)
		 */
		buffer = tmpbuf_get(l_size + 1);
		bufidx = 0;

		memcpy (buffer, l_pbeg, l_size);

		buffer[l_size] = ' ';
		l_next = strchr(buffer,  ' ');
		*l_next = '\0';
		buffer[l_size] = '\0';

		/* skip whitespaces in the line buffer */
		while(isspace(buffer[bufidx])) ++bufidx;

		/*
		 * now there should be a attribute name, comment
		 * or the end-of-line.
		 */
		key = buffer + bufidx;

		switch (*key) {
		case  '#':
		case '\0':
			/* skip this line */
			break;
		case  '}':
			/* end of section reached */
			++bufidx;

			while(isspace(buffer[bufidx]))
				++bufidx;
			if (buffer[bufidx] != '\0')
				goto fail;

			goto finish_section;
		default:
			op  = key + strlen(key) + 1;

			while(isspace(*op)) ++op;

			if (op == l_pend)
				goto fail;

			xiattr = oscap_bfind(xiattr_table, XIATTR_TABLE_COUNT, sizeof(struct xiconf_attr), key, &xiattr_cmp);

			if (xiattr == NULL) {
				dW("Unknown keyword: %s\n", key);
#if XICFG_PARSER_IGNORE_UNKNOWN != 1
				goto fail;
#else
				break;
#endif
			}

			if (!(type & xiattr->sections)) {
				dW("Don't know how to handle attribute %s in this sections type (%d)\n",
				   key, type);
#if XICFG_PARSER_IGNORE_INVSECT != 1
				goto fail;
#else
				break;
#endif
			}

#define xiattr_ptr(ptr, off) ((void **)(((uint8_t *)(ptr))+(off)))

			switch (xiattr->pass_arg) {
			case XIATTR_OPARG_LOCAL:
				opvar = (void *)xiattr_ptr(snew, xiattr->offset);
				dI("local opvar\n");
				break;
			case XIATTR_OPARG_GLOBAL:
				opvar = (void *)xiconf;
				dI("global opvar\n");
				break;
			default:
				abort ();
			}

			if (*op == '=') {
				opfun = xiattr->op_assign;
				opval = op + 1;

				dI("assign(%p): var=%s (at %p+%zu = %p), val=%s\n",
				   opfun, xiattr->name, snew, xiattr->offset, opvar, opval);

			} else if (*op == '+' && *(op+1) == '=') {
				opfun = xiattr->op_insert;
				opval = op + 2;

				dI("insert(%p): var=%s (at %p+%zu = %p), val=%s\n",
				   opfun, xiattr->name, snew, xiattr->offset, opvar, opval);

			} else if (*op == '-' && *(op+1) == '=') {
				opfun = xiattr->op_remove;
				opval = op + 2;

				dI("remove(%p): var=%s (at %p+%zu = %p), val=%s\n",
				   opfun, xiattr->name, snew, xiattr->offset, opvar, opval);
			} else
				goto fail;

			if (opfun == NULL)
				break;
			if (opfun(opvar, opval) != 0)
				goto fail;
		}

		tmpbuf_free(buffer);
	}

finish_section:
	if (buffer != NULL) {
		dW("Line buffer not freed; freeing now...\n");
		tmpbuf_free(buffer);
	}

	switch (type) {
	case XICONF_SECTION_DEFAULTS:
		xiconf->defaults = snew;
		break;
	case XICONF_SECTION_SERVICE:
		if (snew->id == NULL)
			snew->id = snew->name;

		/*
		 * TODO: get a write lock
		 */
		rbt_str_get(xiconf->stree, snew->id, (void *)&scur);

		if (scur == NULL) {
			if (rbt_str_add (xiconf->stree, snew->id, snew) != 0) {
				dE("Can't add service (%s) into the service tree (%p)\n",
				   snew->id, xiconf->stree);

				xiconf_service_free(snew);
				return (-1);
			}

			scur = snew;
		} else {
			if (xiconf_service_merge_and_free(scur, snew) != 0) {
				dE("Failed to merge service tree records %p and %p (id=%s)\n",
				   scur, snew, snew->id);
				xiconf_service_free(snew);
				return (-1);
			}
		}

		if (xiconf_service_merge_defaults(scur, xiconf->defaults) != 0) {
			dE("Failed to merge service (%p, id=%s) with defaults (%p)\n",
			   scur, scur->id, xiconf->defaults);
			return (-1);
		}

		/* TODO: resolve values for all attributes */
		/* TODO: unlock the service tree */

		break;
	default:
		abort ();
	}

	return (0);
fail:
	tmpbuf_free(buffer);
	return (-1);
}

xiconf_service_t *xiconf_getservice(xiconf_t *xiconf, char *name, char *prot)
{
	return (NULL);
}

int op_assign_bool(void *var, char *val)
{
	assume_d(var != NULL, -1);

	*((bool *)(var)) = (bool) strtol (val, NULL, 2);

	return ((errno == EINVAL || errno == ERANGE) ? -1 : 0);
}

int op_assign_num(void *var, char *val)
{
	return (-1);
}

int op_assign_str(void *var, char *val)
{
	assume_d(var != NULL, -1);

	/* skip whitespaces */
	while(isspace(*val)) ++val;

	if (*val != '\0') {
		*((char **)(var)) = strdup(val);
		return (0);
	} else
		return (-1);
}

int op_assign_strl(void *var, char *val)
{
	return (-1);
}

int op_insert_strl(void *var, char *val)
{
	return (-1);
}

int op_remove_strl(void *var, char *val)
{
	return (-1);
}

/*
 * xinetd probe
 */

#ifndef XINETD_CONFPATH
#define XINETD_CONFPATH "/etc/xinetd.conf"
#endif

#ifndef XINETD_CONFDEPTH
#define XINETD_CONFDEPTH 32
#endif

#ifndef XINETD_TEST
void *probe_init(void)
{
	return xiconf_parse(XINETD_CONFPATH, XINETD_CONFDEPTH);
}

void probe_fini(void *arg)
{
	if (arg != NULL)
		xiconf_free(arg);
}

SEXP_t *probe_main(SEXP_t *object, int *err, void *arg)
{
	SEXP_t *item_list, *eval;
	char    srv_name[256];
	char    srv_prot[256];

	xiconf_service_t *xsrv;
	xiconf_t         *xcfg = (xiconf_t *)arg;

	if (arg == NULL) {
		*err = PROBE_EINIT;
		return (NULL);
	}

	eval = probe_obj_getentval(object, "service_name", 1);

	if (eval == NULL) {
		*err = PROBE_ENOVAL;
		return (NULL);
	}

	if (SEXP_string_cstr_r (eval, srv_name, sizeof srv_name) == (size_t)-1) {
		*err = PROBE_ERANGE;
		SEXP_free (eval);
		return (NULL);
	}

	SEXP_free (eval);
	eval = probe_obj_getentval(object, "protocol", 1);

	if (eval == NULL) {
		oscap_free(srv_name);
		*err = PROBE_ENOVAL;
		return (NULL);
	}

	if (SEXP_string_cstr_r (eval, srv_prot, sizeof srv_prot) == (size_t)-1) {
		*err = PROBE_ERANGE;
		SEXP_free (eval);
		return (NULL);
	}

	SEXP_free (eval);

	_D("Updating xinetd configuration cache");

	if (xiconf_update(arg) != 0) {
		/* generate an error item */
		return(NULL);
	}

	_D("Looking for service %s/%s\n", srv_name, srv_prot);
	xsrv = xiconf_getservice(xcfg, srv_name, srv_prot);

	/* TODO: distinguish between an error and "not found" result */

	item_list = SEXP_list_new(NULL);

	if (xsrv != NULL) {
		SEXP_t *item;
		SEXP_t *r[0xC+1];
		register int   i;

#define SEXP_string_new1(str) SEXP_string_new((str), strlen(str))

		item = probe_item_creat("xinetd_item", NULL,
					"protocol",         NULL, r[0x0]= SEXP_string_new1(xsrv->protocol),
					"service_name",     NULL, r[0x1]= SEXP_string_new1(xsrv->name),
					"flags",            NULL, r[0x2]= SEXP_string_new1(xsrv->flags),
					"no_access",        NULL, r[0x3]= SEXP_string_new1(xsrv->no_access),
					"only_from",        NULL, r[0x4]= SEXP_string_new1(xsrv->only_from),
					"port",             NULL, r[0x5]= SEXP_number_newu(xsrv->port),
					"server",           NULL, r[0x6]= SEXP_string_new1(xsrv->server),
					"server_arguments", NULL, r[0x7]= SEXP_string_new1(xsrv->server_args),
					"socket_type",      NULL, r[0x8]= SEXP_string_new1(xsrv->socket_type),
					"type",             NULL, r[0x9]= SEXP_string_new1(xsrv->type),
					"user",             NULL, r[0xA]= SEXP_string_new1(xsrv->user),
					"wait",             NULL, r[0xB]= SEXP_number_newb(xsrv->wait),
					"disabled",         NULL, r[0xC]= SEXP_number_newb(xsrv->disable),
					NULL);

		SEXP_list_add(item_list, item);
		SEXP_free(item);

		/* TODO: get rid of useless S-exp refs */
		for (i = 0; i < 0xC+1; ++i)
			SEXP_free(r[i]);

	}

	return (item_list);
}
#endif
