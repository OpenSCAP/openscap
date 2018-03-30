/**
 * @file   symlink.c
 * @brief  implementation of the symlink_object
 * @author "Jan Černý" <jcerny@redhat.com>
 */

/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
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
 *   Jan Černý <jcerny@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <probe-api.h>
#include "common/debug_priv.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include <probe/probe.h>
#include <probe/option.h>

static int collect_symlink(SEXP_t *ent, probe_ctx *ctx)
{
	SEXP_t *ent_val, *item_sexp, *msg;
	struct stat sb;
	char *linkname;
	char resolved_name[PATH_MAX];

	ent_val = probe_ent_getval(ent);
	char *pathname = SEXP_string_cstr(ent_val);
	SEXP_free(ent_val);
	if (pathname == NULL) {
		return PROBE_EINVAL;
	}

	if (lstat(pathname, &sb) == -1) {
		if (errno == ENOENT) {
			/* File does not exist.
			 * Resulting item should have a status of "does not exist". */
			msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_INFO,
				"File '%s' does not exist.", pathname);
		} else {
			/* error */
			msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, strerror(errno));
			probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
		}
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		free(pathname);
		return 0;
	}

	if (!S_ISLNK(sb.st_mode)) {
		/* File is not a symlink.
		 * Resulting item should have a status "does not exist". */
		msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_INFO,
			"File '%s' is not a symlink.", pathname);
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		free(pathname);
		return 0;
	}

	linkname = oscap_realpath(pathname, resolved_name);
	if (linkname == NULL) {
		if (errno == ENOENT) {
			msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
				"Link target of symlink '%s' does not exist.", pathname);
		} else if (errno == ELOOP) {
			msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
				"Link '%s' is a circular link.", pathname);
		} else {
			msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, strerror(errno));
		}
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
		free(pathname);
		return 0;
	}


	item_sexp = probe_item_create(OVAL_UNIX_SYMLINK, NULL,
			"filepath", OVAL_DATATYPE_STRING, pathname,
			"canonical_path", OVAL_DATATYPE_STRING, linkname,
			NULL);
	probe_item_collect(ctx, item_sexp);

	free(pathname);
	return 0;
}

int probe_offline_mode_supported()
{
	return PROBE_OFFLINE_CHROOT;
}

int probe_main(probe_ctx *ctx, void *probe_arg)
{
	int err;
	SEXP_t *entity, *probe_in;
	oval_schema_version_t oval_version;

	probe_in = probe_ctx_getobject(ctx);

	oval_version = probe_obj_get_platform_schema_version(probe_in);
	if (oval_schema_version_cmp(oval_version, OVAL_SCHEMA_VERSION(5.11)) < 0) {
		// OVAL 5.10 and less
		return PROBE_EOPNOTSUPP;
	}

	entity = probe_obj_getent(probe_in, "filepath", 1);

	err = collect_symlink(entity, ctx);

	SEXP_free(entity);
	return err;
}
