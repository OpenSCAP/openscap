/**
 * @file   rpmverifyfile.c
 * @brief  rpmverifyfile probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 * @author "Petr Lautrbach" <plautrba@redhat.com>
 *
 */

/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pcre.h>

#include "rpm-helper.h"

/* Individual RPM headers */
#include <rpm/rpmfi.h>
#include <rpm/rpmcli.h>

/* SEAP */
#include <probe-api.h>
#include <alloc.h>
#include <common/assume.h>
#include "debug_priv.h"
#include "probe/entcmp.h"

#include <probe/probe.h>
#include <probe/option.h>

struct rpmverify_res {
	char *name;  /**< package name */
	const char *epoch;
	const char *version;
	const char *release;
	const char *arch;
	const char *file;  /**< filepath */
	char extended_name[1024];
	rpmVerifyAttrs vflags; /**< rpm verify flags */
	rpmVerifyAttrs oflags; /**< rpm verify omit flags */
	rpmfileAttrs   fflags; /**< rpm file flags */
};

#define RPMVERIFY_SKIP_CONFIG 0x1000000000000000
#define RPMVERIFY_SKIP_GHOST  0x2000000000000000
#define RPMVERIFY_RPMATTRMASK 0x00000000ffffffff

static struct rpm_probe_global g_rpm;

#define RPMVERIFY_LOCK   RPM_MUTEX_LOCK(&g_rpm.mutex)

#define RPMVERIFY_UNLOCK RPM_MUTEX_UNLOCK(&g_rpm.mutex)

/* modify passed-in iterator to test also given entity */
static int adjust_filter(rpmdbMatchIterator iterator, SEXP_t *ent, rpmTag rpm_tag) {
	oval_operation_t ent_op;
	char ent_str[1024];
	int ret = 0;

	if (ent) {
		ent_op = probe_ent_getoperation(ent, OVAL_OPERATION_EQUALS);
		PROBE_ENT_STRVAL(ent, ent_str, sizeof ent_str, /* void */, strcpy(ent_str, ""););

		switch (ent_op) {
		case OVAL_OPERATION_EQUALS:
			if (rpmdbSetIteratorRE (iterator, rpm_tag, RPMMIRE_STRCMP, ent_str) != 0)
				ret = -1;

			break;
		case OVAL_OPERATION_PATTERN_MATCH:
			if (rpmdbSetIteratorRE (iterator, rpm_tag, RPMMIRE_REGEX,
						(const char *)ent_str) != 0)
				ret = -1;

			break;
		default:
			ret = 0;
		}
	}
	return ret;
}

static int rpmverify_collect(probe_ctx *ctx,
			     const char *file, oval_operation_t file_op,
			     SEXP_t *name_ent, SEXP_t *epoch_ent, SEXP_t *version_ent, SEXP_t *release_ent, SEXP_t *arch_ent,
			     uint64_t flags,
			     int (*callback)(probe_ctx *, struct rpmverify_res *))
{
	rpmdbMatchIterator match;
	rpmVerifyAttrs omit = (rpmVerifyAttrs)(flags & RPMVERIFY_RPMATTRMASK);
	Header pkgh;
	pcre *re = NULL;
	int  ret = -1;

	/* pre-compile regex if needed */
	if (file_op == OVAL_OPERATION_PATTERN_MATCH) {
		const char *errmsg;
		int erroff;

		re = pcre_compile(file, PCRE_UTF8, &errmsg,  &erroff, NULL);

		if (re == NULL) {
			/* TODO */
			return (-1);
		}
	}

	RPMVERIFY_LOCK;

	match = rpmtsInitIterator (g_rpm.rpmts, RPMDBI_PACKAGES, NULL, 0);
	if (match == NULL) {
		ret = 0;
		goto ret;
	}

	if ((ret = adjust_filter(match, name_ent, RPMTAG_NAME)) == -1) {
		dE("can't adjust filter with name");
		goto ret;
	}
	if ((ret = adjust_filter(match, epoch_ent, RPMTAG_EPOCH)) == -1) {
		dE("can't adjust filter with epoch");
		goto ret;
	}
	if ((ret = adjust_filter(match, version_ent, RPMTAG_VERSION)) == -1) {
		dE("can't adjust filter with version");
		goto ret;
	}
	if ((ret = adjust_filter(match, release_ent, RPMTAG_RELEASE)) == -1) {
		dE("can't adjust filter with version");
		goto ret;
	}
	if ((ret = adjust_filter(match, arch_ent, RPMTAG_ARCH)) == -1) {
		dE("can't adjust filter with version");
		goto ret;
	}

	assume_d(RPMTAG_BASENAMES != 0, -1);
	assume_d(RPMTAG_DIRNAMES  != 0, -1);

	while ((pkgh = rpmdbNextIterator (match)) != NULL) {
		SEXP_t *ent;
		rpmfi  fi;
		rpmTag tag[2] = { RPMTAG_BASENAMES, RPMTAG_DIRNAMES };
		struct rpmverify_res res;
		errmsg_t rpmerr;
		int i;

		/*
+SEXP_t *probe_ent_from_cstr(const char *name, oval_datatype_t type,
+                            const char *value, size_t vallen)
		 */

#define COMPARE_ENT(XXX) \
		if (XXX ## _ent != NULL) { \
			ent = probe_entval_from_cstr( \
				probe_ent_getdatatype(XXX ## _ent), res.XXX, strlen(res.XXX) \
			); \
			if (ent != NULL && probe_entobj_cmp(XXX ## _ent, ent) != OVAL_RESULT_TRUE) { \
				SEXP_free(ent); \
				continue; \
			} \
			SEXP_free(ent); \
		}

		res.name = headerFormat(pkgh, "%{NAME}", &rpmerr);
		COMPARE_ENT(name);

		res.epoch = headerFormat(pkgh, "%{EPOCH}", &rpmerr);
		COMPARE_ENT(epoch);

		res.version = headerFormat(pkgh, "%{VERSION}", &rpmerr);
		COMPARE_ENT(version);
		res.release = headerFormat(pkgh, "%{RELEASE}", &rpmerr);
		COMPARE_ENT(release);
		res.arch = headerFormat(pkgh, "%{ARCH}", &rpmerr);
		COMPARE_ENT(arch);
		snprintf(res.extended_name, 1024, "%s-%s:%s-%s.%s", res.name,
			oscap_streq(res.epoch, "(none)") ? "0" : res.epoch,
			res.version, res.release, res.arch);

		/*
		 * Inspect package files & directories
		 */
		for (i = 0; i < 2; ++i) {
		  fi = rpmfiNew(g_rpm.rpmts, pkgh, tag[i], 1);

		  while (rpmfiNext(fi) != -1) {
				res.file = oscap_strdup(rpmfiFN(fi));
		    res.fflags = rpmfiFFlags(fi);
		    res.oflags = omit;

		    if (((res.fflags & RPMFILE_CONFIG) && (flags & RPMVERIFY_SKIP_CONFIG)) ||
					((res.fflags & RPMFILE_GHOST)  && (flags & RPMVERIFY_SKIP_GHOST))) {
					oscap_free(res.file);
					continue;
				}

		    switch(file_op) {
		    case OVAL_OPERATION_EQUALS:
					if (strcmp(res.file, file) != 0) {
						oscap_free(res.file);
						continue;
					}
		      break;
		    case OVAL_OPERATION_NOT_EQUAL:
					if (strcmp(res.file, file) == 0) {
						oscap_free(res.file);
						continue;
					}
		      break;
		    case OVAL_OPERATION_PATTERN_MATCH:
		      ret = pcre_exec(re, NULL, res.file, strlen(res.file), 0, 0, NULL, 0);

		      switch(ret) {
		      case 0: /* match */
			break;
		      case -1:
			/* mismatch */
			oscap_free(res.file);
			continue;
		      default:
			dE("pcre_exec() failed!");
			ret = -1;
			oscap_free(res.file);
			goto ret;
		      }
		      break;
		    default:
		      /* unsupported operation */
		      dE("Operation \"%d\" on `filepath' not supported", file_op);
		      ret = -1;
					oscap_free(res.file);
		      goto ret;
		    }

		    if (rpmVerifyFile(g_rpm.rpmts, fi, &res.vflags, omit) != 0)
		      res.vflags = RPMVERIFY_FAILURES;

		    if (callback(ctx, &res) != 0) {
			    ret = 0;
					oscap_free(res.file);
			    goto ret;
		    }
			oscap_free(res.file);
		  }

		  rpmfiFree(fi);
		}
	}

	match = rpmdbFreeIterator (match);
	ret   = 0;
ret:
	if (re != NULL)
		pcre_free(re);

	RPMVERIFY_UNLOCK;
	return (ret);
}

void probe_preload ()
{
	rpmLibsPreload();
}

void *probe_init (void)
{
	probe_setoption(PROBEOPT_OFFLINE_MODE_SUPPORTED, PROBE_OFFLINE_CHROOT);
#ifdef HAVE_RPM46
	rpmlogSetCallback(rpmErrorCb, NULL);
#else
	rpmlogSetCallback(rpmErrorCb);
#endif
	if (rpmReadConfigFiles ((const char *)NULL, (const char *)NULL) != 0) {
		dI("rpmReadConfigFiles failed: %u, %s.", errno, strerror (errno));
		return (NULL);
	}

	g_rpm.rpmts = rpmtsCreate();

	pthread_mutex_init(&(g_rpm.mutex), NULL);
	return ((void *)&g_rpm);
}

void probe_fini (void *ptr)
{
	struct rpm_probe_global *r = (struct rpm_probe_global *)ptr;

	rpmtsFree(r->rpmts);
	rpmFreeCrypto();
	rpmFreeRpmrc();
	rpmFreeMacros(NULL);
	rpmlogClose();
	pthread_mutex_destroy (&(r->mutex));

	return;
}

static void _add_ent_from_cstr(SEXP_t *item, const char *name, const char *value)
{
	SEXP_t *sexp_str = SEXP_string_new(value, strlen(value));
	probe_item_ent_add(item, name, NULL, sexp_str);
	SEXP_free(sexp_str);
}

static void _add_ent_from_flag(SEXP_t *item, const char *name, struct rpmverify_res *res, int flag)
{
	const char *result;
	if (res->oflags & flag || res->vflags & RPMVERIFY_FAILURES) {
		result = "not performed";
	} else if (res->vflags & flag) {
		result = "fail";
	} else {
		result = "pass";
	}
	_add_ent_from_cstr(item, name, result);
}

static void _add_ent_bool_from_flag(SEXP_t *item, const char *name, struct rpmverify_res *res, int flag)
{
	SEXP_t *sexp_str;
	sexp_str = SEXP_number_newb(res->fflags & flag);
	probe_item_ent_add(item, name, NULL, sexp_str);
	SEXP_free(sexp_str);
}

static int rpmverify_additem(probe_ctx *ctx, struct rpmverify_res *res)
{
	SEXP_t *item;
	oval_schema_version_t oval_version;

	item = probe_item_create(OVAL_LINUX_RPMVERIFYFILE, NULL,NULL);
	_add_ent_from_cstr(item, "name", res->name);
	_add_ent_from_cstr(item, "epoch", res->epoch);
	_add_ent_from_cstr(item, "version", res->version);
	_add_ent_from_cstr(item, "release", res->release);
	_add_ent_from_cstr(item, "arch", res->arch);
	_add_ent_from_cstr(item, "filepath", res->file);
	_add_ent_from_cstr(item, "extended_name", res->extended_name);
	_add_ent_from_flag(item, "size_differs", res, RPMVERIFY_FILESIZE);
	_add_ent_from_flag(item, "mode_differs", res, RPMVERIFY_MODE);
	_add_ent_from_flag(item, "md5_differs", res, RPMVERIFY_MD5); // deprecated since OVAL 5.11.1
	oval_version = probe_obj_get_platform_schema_version(probe_ctx_getobject(ctx));
	if (oval_schema_version_cmp(oval_version, OVAL_SCHEMA_VERSION(5.11.1)) >= 0) {
		_add_ent_from_flag(item, "filedigest_differs", res, RPMVERIFY_FILEDIGEST);
	}
	_add_ent_from_flag(item, "device_differs", res, RPMVERIFY_RDEV);
	_add_ent_from_flag(item, "link_mismatch", res, RPMVERIFY_LINKTO);
	_add_ent_from_flag(item, "ownership_differs", res, RPMVERIFY_USER);
	_add_ent_from_flag(item, "group_differs", res, RPMVERIFY_GROUP);
	_add_ent_from_flag(item, "mtime_differs", res, RPMVERIFY_MTIME);
#ifndef HAVE_LIBRPM44
	_add_ent_from_flag(item, "capabilities_differ", res, RPMVERIFY_CAPS);
#endif
	_add_ent_bool_from_flag(item, "configuration_file", res, RPMFILE_CONFIG);
	_add_ent_bool_from_flag(item, "documentation_file", res, RPMFILE_DOC);
	_add_ent_bool_from_flag(item, "ghost_file", res, RPMFILE_GHOST);
	_add_ent_bool_from_flag(item, "license_file", res, RPMFILE_LICENSE);
	_add_ent_bool_from_flag(item, "readme_file", res, RPMFILE_README);

	return probe_item_collect(ctx, item) == 2 ? 1 : 0;
}

typedef struct {
	const char *a_name;
	uint64_t    a_flag;
} rpmverifyfile_bhmap_t;

const rpmverifyfile_bhmap_t rpmverifyfile_bhmap[] = {
	{ "nolinkto",      (uint64_t)VERIFY_LINKTO    },
	{ "nomd5",	 (uint64_t)VERIFY_MD5       }, // deprecated since OVAL 5.11.1
	{ "nosize",	(uint64_t)VERIFY_SIZE      },
	{ "nouser",	(uint64_t)VERIFY_USER      },
	{ "nogroup",       (uint64_t)VERIFY_GROUP     },
	{ "nomtime",       (uint64_t)VERIFY_MTIME     },
	{ "nomode",	(uint64_t)VERIFY_MODE      },
	{ "nordev",	(uint64_t)VERIFY_RDEV      },
	{ "noconfigfiles", RPMVERIFY_SKIP_CONFIG      },
	{ "noghostfiles",  RPMVERIFY_SKIP_GHOST       },
	{ "nofiledigest", (uint64_t)VERIFY_FILEDIGEST },
	{ "nocaps", (uint64_t)VERIFY_CAPS }
};

int probe_main (probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *file_ent, *bh_ent;
	SEXP_t *name_ent, *epoch_ent, *version_ent, *release_ent, *arch_ent;
	char   file[PATH_MAX];
	size_t file_len = sizeof file;
	oval_operation_t file_op;
	uint64_t collect_flags = 0;
	unsigned int i;

	if (g_rpm.rpmts == NULL) {
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_NOT_APPLICABLE);
		return 0;
	}

	/*
	 * Get refs to object entities
	 */
	probe_in = probe_ctx_getobject(ctx);
	file_ent = probe_obj_getent(probe_in, "filepath", 1);

	if (file_ent == NULL) {
		dE("Missing \"filepath\" (%p) entity", file_ent);

		SEXP_free(file_ent);

		return (PROBE_ENOENT);
	}


	/*
	 * Extract the requested operation for each entity
	 */
	file_op = probe_ent_getoperation(file_ent, OVAL_OPERATION_EQUALS);

	if (file_op == OVAL_OPERATION_UNKNOWN)
	{
		SEXP_free(file_ent);

		return (PROBE_EINVAL);
	}

	PROBE_ENT_STRVAL(file_ent, file, file_len, /* void */, strcpy(file, ""););
	SEXP_free(file_ent);

	name_ent = probe_obj_getent(probe_in, "name", 1);
	epoch_ent = probe_obj_getent(probe_in, "epoch", 1);
	version_ent = probe_obj_getent(probe_in, "version", 1);
	release_ent = probe_obj_getent(probe_in, "release", 1);
	arch_ent = probe_obj_getent(probe_in, "arch", 1);

	/*
	 * Parse behaviors
	 */
	bh_ent = probe_obj_getent(probe_in, "behaviors", 1);

	if (bh_ent != NULL) {
		SEXP_t *aval;

		for (i = 0; i < sizeof rpmverifyfile_bhmap/sizeof(rpmverifyfile_bhmap_t); ++i) {
			aval = probe_ent_getattrval(bh_ent, rpmverifyfile_bhmap[i].a_name);

			if (aval != NULL) {
				if (SEXP_strcmp(aval, "true") == 0) {
					dD("omit verify attr: %s", rpmverifyfile_bhmap[i].a_name);
					collect_flags |= rpmverifyfile_bhmap[i].a_flag;
				}

				SEXP_free(aval);
			}
		}

		SEXP_free(bh_ent);
	}

	dI("Collecting rpmverifyfile data, query: f=\"%s\" (%d)",
	   file, file_op);

	if (rpmverify_collect(ctx,
			      file, file_op,
			      name_ent, epoch_ent, version_ent, release_ent, arch_ent,
			      collect_flags,
			      rpmverify_additem) != 0)
	{
		dE("An error ocured while collecting rpmverifyfile data");
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
	}

	SEXP_free(name_ent);
	SEXP_free(epoch_ent);
	SEXP_free(version_ent);
	SEXP_free(release_ent);
	SEXP_free(arch_ent);

	return 0;
}
