/**
 * @file   selinuxboolean_probe.c
 * @brief  selinuxboolean probe
 * @author "Petr Lautrbach" <plautrba@redhat.com>
 *
 *  This probe is able to process a selinuxboolean_object as defined in OVAL 5.8.
 *
 */

/*
 * Copyright 2009-2011 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *   Petr Lautrbach <plautrba@redhat.com>
 */

/*
 * selinuxboolean probe:
 *
 * name
 * current_status
 * pending_status
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#include <limits.h>

#include "_seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include <probe/probe.h>

#include <selinux/selinux.h>
#include "selinuxboolean_probe.h"


/* These definitions and three functions were commandeered from SELinux internals
 * and creatively improved to take the global prefix into account */

#define OLDSELINUXMNT "/selinux"
#define SELINUXMNT    "/sys/fs/selinux"
#define SELINUXFS     "selinuxfs"
#define SELINUX_MAGIC 0xf97cff8c

static int verify_selinuxmnt_prefixed(const char *prefix, const char *mnt)
{
	char path[PATH_MAX] = {0};
	struct statfs sfbuf;
	int rc;

	if (!prefix)
		return -1;

	snprintf(path, PATH_MAX, "%s%s", prefix, mnt);

	do {
		rc = statfs(path, &sfbuf);
	} while (rc < 0 && errno == EINTR);
	if (rc == 0) {
		if ((uint32_t)sfbuf.f_type == (uint32_t)SELINUX_MAGIC) {
			struct statvfs vfsbuf;
			rc = statvfs(path, &vfsbuf);
			if (rc == 0) {
				if (!(vfsbuf.f_flag & ST_RDONLY)) {
					set_selinuxmnt(path);
				}
				return 0;
			}
		}
	}

	return -1;
}

static int selinuxfs_exists_prefixed(const char *prefix)
{
	char path[PATH_MAX] = {0};
	int exists = 0;
	FILE *fp = NULL;
	char *buf = NULL;
	size_t len;
	ssize_t num;

	if (!prefix)
		return 0;

	snprintf(path, PATH_MAX, "%s/proc/filesystems", prefix);
	fp = fopen(path, "re");
	if (!fp)
		return 1; /* Fail as if it exists */
	__fsetlocking(fp, FSETLOCKING_BYCALLER);

	num = getline(&buf, &len, fp);
	while (num != -1) {
		if (strstr(buf, SELINUXFS)) {
			exists = 1;
			break;
		}
		num = getline(&buf, &len, fp);
	}

	free(buf);
	fclose(fp);
	return exists;
}

static int init_selinuxmnt_prefixed(const char *prefix)
{
	char path[PATH_MAX] = {0};
	char *buf = NULL, *p;
	FILE *fp = NULL;
	size_t len;
	ssize_t num;
	int res = -1;

	if (!prefix)
		return res;

	if (verify_selinuxmnt_prefixed(prefix, SELINUXMNT) == 0)
		return 0;

	if (verify_selinuxmnt_prefixed(prefix, OLDSELINUXMNT) == 0)
		return 0;

	/* Drop back to detecting it the long way. */
	if (!selinuxfs_exists_prefixed(prefix))
		goto out;

	/* At this point, the usual spot doesn't have an selinuxfs so
	 * we look around for it */
	snprintf(path, PATH_MAX, "%s/proc/mounts", prefix);
	fp = fopen(path, "re");
	if (!fp)
		goto out;

	__fsetlocking(fp, FSETLOCKING_BYCALLER);
	while ((num = getline(&buf, &len, fp)) != -1) {
		char *tmp;
		p = strchr(buf, ' ');
		if (!p)
			goto out;
		p++;
		tmp = strchr(p, ' ');
		if (!tmp)
			goto out;
		if (!strncmp(tmp + 1, SELINUXFS" ", strlen(SELINUXFS)+1)) {
			*tmp = '\0';
			break;
		}
	}

	/* If we found something, dup it */
	if (num > 0)
		res = verify_selinuxmnt_prefixed(prefix, p);

out:
	free(buf);
	if (fp)
		fclose(fp);
	return res;
}

/* End of borrowing from SELinux */

static int get_selinuxboolean(SEXP_t *ut_ent, probe_ctx *ctx)
{
	int err = 1, active, pending, len, i;
	SEXP_t *boolean, *item;
	char **booleans;

	if (security_get_boolean_names(&booleans, &len) == -1) {
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
		return err;
	}

	for (i = 0; i < len; i++) {
		boolean = SEXP_string_new(booleans[i], strlen(booleans[i]));
		if (probe_entobj_cmp(ut_ent, boolean) == OVAL_RESULT_TRUE) {
			active = security_get_boolean_active(booleans[i]);
			pending = security_get_boolean_pending(booleans[i]);
			item = probe_item_create(
				OVAL_LINUX_SELINUXBOOLEAN, NULL,
				"name", OVAL_DATATYPE_SEXP, boolean,
				"current_status",  OVAL_DATATYPE_BOOLEAN, active,
				"pending_status", OVAL_DATATYPE_BOOLEAN, pending,
				NULL);
			probe_item_collect(ctx, item);
		}
		SEXP_free(boolean);
	}

	for (i = 0; i < len; i++)
		free(booleans[i]);
	free(booleans);

	return 0;
}

int selinuxboolean_probe_offline_mode_supported(void)
{
	return PROBE_OFFLINE_OWN;
}

int selinuxboolean_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *name;
	int err;

	probe_in  = probe_ctx_getobject(ctx);
	name = probe_obj_getent(probe_in, "name", 1);

	if (name == NULL) {
		return PROBE_ENOVAL;
	}

	const char *prefix = getenv("OSCAP_PROBE_ROOT");
	if (prefix != NULL) {
		if (init_selinuxmnt_prefixed(prefix)) {
			SEXP_free(name);
			return PROBE_ESUCCESS;
		}
	}

	err = get_selinuxboolean(name, ctx);
	SEXP_free(name);

	return err;
}
