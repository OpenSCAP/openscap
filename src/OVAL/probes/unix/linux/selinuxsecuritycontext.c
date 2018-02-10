/**
 * @file   selinuxsecuritycontext.c
 * @brief  selinuxsecuritycontext probe
 * @author "Petr Lautrbach" <plautrba@redhat.com>
 *
 * 2011/07/01 plautrba@redhat.com
 *  This probe is able to process a selinuxsecuritycontext_object as defined in OVAL 5.9
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
 *      "Petr Lautrbach" <plautrba@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <seap.h>
#include <probe-api.h>
#include <probe/entcmp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>

#include <selinux/selinux.h>
#include <selinux/context.h>

#include "oval_fts.h"
#include "util.h"
#include "common/debug_priv.h"

#define FILE_SEPARATOR '/'

static void split_level(const char *level, char **sensitivity, char **category) {
	char *level_split;

	level_split = strchr(level, ':');
	if (level_split == NULL) {
		*sensitivity = strdup(level);
		*category = NULL;
	}
	else {
		*sensitivity = strndup(level, level_split - level);
		*category = strdup(level_split + 1);
	}
}

/* parse range according to
 * http://selinuxproject.org/page/NB_MLS#MLS_.2F_MCS_Range_Format
*/
static void split_range(const char *range, char **l_s, char **l_c, char **h_s, char **h_c) {
	char *level;
	const char *range_split;

	if (range == NULL) {
		dW("%s: range is NULL", __func__);
		*l_s = *l_c = *h_s = *h_c = NULL;
		return;
	}
	range_split = strchr(range, '-');
	if (range_split == NULL) {
		/* there is only 1 level */
		split_level(range, l_s, l_c);
		*h_s = NULL;
		*h_c = NULL;
	}
	else {
		/* low level */
		level = strndup(range, range_split - range);
		split_level(level, l_s, l_c);
		free(level);

		/* high level */
		level = strdup(range_split + 1);
		split_level(level, h_s, h_c);
		free(level);
	}
}

static int selinuxsecuritycontext_process_cb (SEXP_t *pid_ent, probe_ctx *ctx) {

	SEXP_t *pid_sexp, *item;
	security_context_t pid_context;
	context_t context;
	int pid_number;
	DIR *proc;
	struct dirent *dir_entry;
	const char *user, *role, *type, *range;
	char *l_sensitivity, *l_category, *h_sensitivity, *h_category;

	if ((proc = opendir("/proc")) == NULL) {
		dE("Can't open /proc dir: %s", strerror(errno));

		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
		return errno;
	}

	while ((dir_entry = readdir(proc))) {
		if (strspn(dir_entry->d_name, "0123456789") != strlen(dir_entry->d_name))
			continue;

		pid_number = atoi(dir_entry->d_name);
		pid_sexp = SEXP_number_newi_32(pid_number);
		if (probe_entobj_cmp(pid_ent, pid_sexp) == OVAL_RESULT_TRUE) {

			if (getpidcon(pid_number, &pid_context) == -1) {
				/* error getting pid selinux context */
				dW("Can't get selinux context for process %d", pid_number);
				SEXP_free(pid_sexp);
				continue;
			}

			context = context_new(pid_context);

			user = context_user_get(context);
			role = context_role_get(context);
			type = context_type_get(context);
			range = context_range_get(context);
			if (range != NULL) {
				split_range(range, &l_sensitivity, &l_category, &h_sensitivity, &h_category);
				item = probe_item_create(OVAL_LINUX_SELINUXSECURITYCONTEXT, NULL,
					"pid",     OVAL_DATATYPE_INTEGER, (int64_t)pid_number,
					"user",     OVAL_DATATYPE_STRING, user,
					"role",     OVAL_DATATYPE_STRING, role,
					"type",     OVAL_DATATYPE_STRING, type,
					"low_sensitivity", OVAL_DATATYPE_STRING, l_sensitivity,
					"low_category", OVAL_DATATYPE_STRING, l_category,
					"high_sensitivity", OVAL_DATATYPE_STRING, h_sensitivity,
					"high_category", OVAL_DATATYPE_STRING, h_category,
					NULL);
			}
			else {
				item = probe_item_create(OVAL_LINUX_SELINUXSECURITYCONTEXT, NULL,
					"pid",     OVAL_DATATYPE_INTEGER, (int64_t)pid_number,
					"user",     OVAL_DATATYPE_STRING, user,
					"role",     OVAL_DATATYPE_STRING, role,
					"type",     OVAL_DATATYPE_STRING, type,
					NULL);
			}

			probe_item_collect(ctx, item);

			context_free(context);
			freecon(pid_context);
		}
		SEXP_free(pid_sexp);
	}
	closedir(proc);

	return 0;
}


static int selinuxsecuritycontext_file_cb (const char *p, const char *f, probe_ctx *ctx)
{
	SEXP_t *item;

	char   pbuf[PATH_MAX+1];
	size_t plen, flen;

	security_context_t file_context;
	int file_context_size;
	context_t context;
	const char *user, *role, *type, *range;
	char *l_sensitivity, *l_category, *h_sensitivity, *h_category;
	int err = 0;

	/* directory */
	if (f == NULL)
		flen = 0;
	else
		flen = strlen (f);

	/*
	 * Prepare path
	 */
	plen = strlen (p);

	if (plen + flen + 1 > PATH_MAX)
		return (-1);

	memcpy (pbuf, p, sizeof (char) * plen);

	if (p[plen - 1] != FILE_SEPARATOR) {
		pbuf[plen] = FILE_SEPARATOR;
		++plen;
	}

	if (f != NULL)
		memcpy (pbuf + plen, f, sizeof (char) * flen);

	pbuf[plen+flen] = '\0';

	file_context_size = getfilecon(pbuf, &file_context);
	if (file_context_size == -1) {
		dE("Can't get context for %s: %s", pbuf, strerror(errno));

		item = probe_item_create(OVAL_LINUX_SELINUXSECURITYCONTEXT, NULL,
						"filepath", OVAL_DATATYPE_STRING, pbuf,
						"path",     OVAL_DATATYPE_STRING, p,
						"filename", OVAL_DATATYPE_STRING, f,
						NULL);

		probe_item_add_msg(item, OVAL_MESSAGE_LEVEL_ERROR,
			"Can't get context for %s: %s\n", pbuf, strerror(errno));
		probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);
		err = errno;

	}
	else {
		context = context_new(file_context);

		user = context_user_get(context);
		role = context_role_get(context);
		type = context_type_get(context);
		range = context_range_get(context);

		if (range != NULL) {
			split_range(range, &l_sensitivity, &l_category, &h_sensitivity, &h_category);
			item = probe_item_create(OVAL_LINUX_SELINUXSECURITYCONTEXT, NULL,
							"filepath", OVAL_DATATYPE_STRING, pbuf,
							"path",     OVAL_DATATYPE_STRING, p,
							"filename", OVAL_DATATYPE_STRING, f,
							"user",     OVAL_DATATYPE_STRING, user,
							"role",     OVAL_DATATYPE_STRING, role,
							"type",     OVAL_DATATYPE_STRING, type,
							"low_sensitivity", OVAL_DATATYPE_STRING, l_sensitivity,
							"low_category", OVAL_DATATYPE_STRING, l_category,
							"high_sensitivity", OVAL_DATATYPE_STRING, h_sensitivity,
							"high_category", OVAL_DATATYPE_STRING, h_category,
							"rawlow_sensitivity", OVAL_DATATYPE_STRING, l_sensitivity,
							"rawlow_category", OVAL_DATATYPE_STRING, l_category,
							"rawhigh_sensitivity", OVAL_DATATYPE_STRING, h_sensitivity,
							"rawhigh_category", OVAL_DATATYPE_STRING, h_category,
							NULL);
		}
		else {
			item = probe_item_create(OVAL_LINUX_SELINUXSECURITYCONTEXT, NULL,
							"filepath", OVAL_DATATYPE_STRING, pbuf,
							"path",     OVAL_DATATYPE_STRING, p,
							"filename", OVAL_DATATYPE_STRING, f,
							"user",     OVAL_DATATYPE_STRING, user,
							"role",     OVAL_DATATYPE_STRING, role,
							"type",     OVAL_DATATYPE_STRING, type,
							NULL);
		}

		context_free(context);
	}
	probe_item_collect(ctx, item);

	if (file_context != NULL)
		freecon(file_context);

	return (err);
}

int probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in;
	SEXP_t *path, *filename, *filepath, *pid, *behaviors = NULL;
	int process_pid, err = 0;

	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

	if ( ! is_selinux_enabled()) {
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_NOT_APPLICABLE);
		return 0;
	}

	probe_in  = probe_ctx_getobject(ctx);

	behaviors = probe_obj_getent (probe_in, "behaviors", 1);
	path      = probe_obj_getent (probe_in, "path",      1);
	filename  = probe_obj_getent (probe_in, "filename",  1);
	filepath  = probe_obj_getent (probe_in, "filepath", 1);
	pid       = probe_obj_getent (probe_in, "pid", 1);

	if (((path == NULL || filename == NULL) && filepath==NULL ) && pid == NULL) {
		err = PROBE_ENOENT;
		goto cleanup;
	}

	if (filepath || (path && filename)) {
		probe_filebehaviors_canonicalize(&behaviors);

		if ((ofts = oval_fts_open(path, filename, filepath, behaviors, probe_ctx_getresult(ctx))) != NULL) {
			while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
				selinuxsecuritycontext_file_cb(ofts_ent->path, ofts_ent->file, ctx);
				oval_ftsent_free(ofts_ent);
			}

			oval_fts_close(ofts);
		}
		SEXP_free(behaviors);
	}

	if (pid != NULL) {
		PROBE_ENT_I32VAL(pid, process_pid, err = PROBE_ENOVAL;, process_pid = 0;);
		if (err != 0)
			goto cleanup;

		if (process_pid == 0) {
			SEXP_t *nref, *nval, *pid2;

			nref = SEXP_list_first(probe_in);
			nval = SEXP_number_newu_32(getpid());
			pid2 = SEXP_list_new(nref, nval, NULL);
			SEXP_free(pid);
			SEXP_free(nref);
			SEXP_free(nval);
			pid = pid2;
		}

		selinuxsecuritycontext_process_cb(pid, ctx);
	}

cleanup:
	SEXP_free (path);
	SEXP_free (filename);
	SEXP_free (filepath);
	SEXP_free (pid);

	return err;
}
