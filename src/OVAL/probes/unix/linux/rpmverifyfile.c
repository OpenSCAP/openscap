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

/* RPM headers */
#include <rpm/rpmfi.h>
#include <rpm/rpmcli.h>

#include "rpm-helper.h"

/* SEAP */
#include <probe-api.h>
#include <alloc.h>
#include <common/assume.h>
#include "debug_priv.h"
#include "probe/entcmp.h"

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

struct rpmverify_global {
	rpmts	   rpmts;
	pthread_mutex_t mutex;
};

static struct rpmverify_global g_rpm;

#define RPMVERIFY_LOCK	  \
	do { \
		int prev_cancel_state = -1; \
		if (pthread_mutex_lock(&g_rpm.mutex) != 0) { \
			dE("Can't lock mutex"); \
			return (-1); \
		} \
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_cancel_state); \
	} while(0)

#define RPMVERIFY_UNLOCK	  \
	do { \
		int prev_cancel_state = -1; \
		if (pthread_mutex_unlock(&g_rpm.mutex) != 0) { \
			dE("Can't unlock mutex. Aborting..."); \
			abort(); \
		} \
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &prev_cancel_state); \
	} while(0)

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

void *probe_init (void)
{
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
	struct rpmverify_global *r = (struct rpmverify_global *)ptr;

	rpmtsFree(r->rpmts);
	rpmFreeCrypto();
	rpmFreeRpmrc();
	rpmFreeMacros(NULL);
	rpmlogClose();
	pthread_mutex_destroy (&(r->mutex));

	return;
}

static int rpmverify_additem(probe_ctx *ctx, struct rpmverify_res *res)
{
	SEXP_t *item;

#define VF_RESULT(f) (res->oflags & (f) ? "not performed" : (res->vflags & RPMVERIFY_FAILURES ? "not performed" : (res->vflags & (f) ? "fail" : "pass")))
#define FF_RESULT(f) (res->fflags & (f) ? true : false)

	item = probe_item_create(OVAL_LINUX_RPMVERIFYFILE, NULL,
				 "name",		OVAL_DATATYPE_STRING, res->name,
				 "epoch", 		OVAL_DATATYPE_STRING, res->epoch,
				 "version", 		OVAL_DATATYPE_STRING, res->version,
				 "release", 	 	OVAL_DATATYPE_STRING, res->release,
				 "arch", 		OVAL_DATATYPE_STRING, res->arch,
				 "filepath",	    OVAL_DATATYPE_STRING, res->file,
				 "extended_name", 	OVAL_DATATYPE_STRING, res->extended_name,
				 "size_differs",	OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_FILESIZE),
				 "mode_differs",	OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_MODE),
				 "md5_differs",	 OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_MD5),
				 "device_differs",      OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_RDEV),
				 "link_mismatch",       OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_LINKTO),
				 "ownership_differs",   OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_USER),
				 "group_differs",       OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_GROUP),
				 "mtime_differs",       OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_MTIME),
#ifndef HAVE_LIBRPM44
				 "capabilities_differ", OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_CAPS),
#endif
				 "configuration_file",  OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_CONFIG),
				 "documentation_file",  OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_DOC),
				 "ghost_file",	  OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_GHOST),
				 "license_file",	OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_LICENSE),
				 "readme_file",	 OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_README),
				 NULL);

	return probe_item_collect(ctx, item) == 2 ? 1 : 0;
}

typedef struct {
	const char *a_name;
	uint64_t    a_flag;
} rpmverifyfile_bhmap_t;

const rpmverifyfile_bhmap_t rpmverifyfile_bhmap[] = {
	{ "nolinkto",      (uint64_t)VERIFY_LINKTO    },
	{ "nomd5",	 (uint64_t)VERIFY_MD5       },
	{ "nosize",	(uint64_t)VERIFY_SIZE      },
	{ "nouser",	(uint64_t)VERIFY_USER      },
	{ "nogroup",       (uint64_t)VERIFY_GROUP     },
	{ "nomtime",       (uint64_t)VERIFY_MTIME     },
	{ "nomode",	(uint64_t)VERIFY_MODE      },
	{ "nordev",	(uint64_t)VERIFY_RDEV      },
	{ "noconfigfiles", RPMVERIFY_SKIP_CONFIG      },
	{ "noghostfiles",  RPMVERIFY_SKIP_GHOST       }
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
					dI("omit verify attr: %s", rpmverifyfile_bhmap[i].a_name);
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
