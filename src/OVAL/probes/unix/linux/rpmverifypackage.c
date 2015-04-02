/**
 * @file   rpmverifypackage.c
 * @brief  rpmverifypackage probe
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
#include <rpm/rpmdb.h>
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmmacro.h>
#include <rpm/rpmlog.h>
#include <rpm/rpmfi.h>
#include <rpm/header.h>
#include <rpm/rpmcli.h>
#include <popt.h>

#ifndef HAVE_HEADERFORMAT
# define HAVE_LIBRPM44 1 /* hack */
# define headerFormat(_h, _fmt, _emsg) headerSprintf((_h),( _fmt), rpmTagTable, rpmHeaderFormats, (_emsg))
#endif

#ifndef HAVE_RPMFREECRYPTO
# define rpmFreeCrypto() while(0)
#endif

#ifndef HAVE_RPMFREEFILESYSTEMS
# define rpmFreeFilesystems() while(0)
#endif

/* SEAP */
#include <probe-api.h>
#include <alloc.h>
#include <common/assume.h>
#include "debug_priv.h"
#include "probe/entcmp.h"

typedef struct {
	const char *a_name;
	uint64_t    a_flag;
	const char *a_option;
} rpmverifypackage_bhmap_t;

const rpmverifypackage_bhmap_t rpmverifypackage_bhmap[] = {
	{ "nodeps",        (uint64_t)VERIFY_DEPS      , "--nodeps"},
	{ "nodigest",      (uint64_t)VERIFY_DIGEST    , "--nodigest"},
	{ "noscripts",     (uint64_t)VERIFY_SCRIPT    , "--noscript"},
	{ "nosignature",   (uint64_t)VERIFY_SIGNATURE , "--nosignature"}
};

struct rpmverify_res {
	char *name;  /**< package name */
	char *epoch;
	char *version;
	char *release;
	char *arch;
	char extended_name[1024];
	uint64_t vflags; /**< rpm verify flags */
	uint64_t vresults;
};

#define RPMVERIFY_SKIP_CONFIG 0x1000000000000000
#define RPMVERIFY_SKIP_GHOST  0x2000000000000000
#define RPMVERIFY_RPMATTRMASK 0x00000000ffffffff

struct rpmverify_global {
	rpmts	   rpmts;
	pthread_mutex_t mutex;
};

static struct rpmverify_global g_rpm;

static struct poptOption optionsTable[] = {
	{ NULL, '\0', POPT_ARG_INCLUDE_TABLE, rpmcliAllPoptTable, 0,
	    "Common options for all rpm modes and executables:", NULL },
	{ NULL, '\0', POPT_ARG_INCLUDE_TABLE, rpmVerifyPoptTable, 0,
	    "Query options (with -q or --query):", NULL },
	POPT_AUTOALIAS
	POPT_AUTOHELP
	POPT_TABLEEND
};

#define RPMVERIFY_LOCK	  \
	do { \
		int prev_cancel_state = -1; \
		if (pthread_mutex_lock(&g_rpm.mutex) != 0) { \
			dE("Can't lock mutex\n"); \
			return (-1); \
		} \
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_cancel_state); \
	} while(0)

#define RPMVERIFY_UNLOCK	  \
	do { \
		int prev_cancel_state = -1; \
		if (pthread_mutex_unlock(&g_rpm.mutex) != 0) { \
			dE("Can't unlock mutex. Aborting...\n"); \
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
		if (rpm_tag == RPMTAG_EPOCH && strcmp(ent_str, "(none)") == 0) {
			return ret;
		}

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
			     SEXP_t *name_ent, SEXP_t *epoch_ent, SEXP_t *version_ent, SEXP_t *release_ent, SEXP_t *arch_ent,
			     uint64_t flags,
			     int (*callback)(probe_ctx *, struct rpmverify_res *))
{
	rpmdbMatchIterator match;
	Header pkgh;
	int  ret = -1;
	unsigned int i, j, rpmcli_argc = 0;
	const char * rpmcli_argv[10];
	poptContext rpmcli_context;
	QVA_t qva;

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

	rpmcli_argv[0] = "probe_rpmverifypackage";
	rpmcli_argv[1] = "--quiet";
	rpmcli_argv[2] = "--nofiles";

	while ((pkgh = rpmdbNextIterator (match)) != NULL) {
		SEXP_t *ent;
		struct rpmverify_res res;
		errmsg_t rpmerr;

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
		 * Verify package
		 */
		res.vflags = res.vresults = 0;

		/* if a --no<flag> is set then we don't run rpmVerify else
		 * we need to run rpm -V with all other --no<flags>
		 */
		for (i = 0; i < sizeof rpmverifypackage_bhmap/sizeof(rpmverifypackage_bhmap_t); ++i) {
			rpmcli_argc = 3;
			if (flags & rpmverifypackage_bhmap[i].a_flag)
				continue;

			for (j = 0; j < sizeof rpmverifypackage_bhmap/sizeof(rpmverifypackage_bhmap_t); ++j) {
				if (j == i)
					continue;

				rpmcli_argv[rpmcli_argc++] = rpmverifypackage_bhmap[j].a_option;
			}
			rpmcli_argv[rpmcli_argc++] = res.name;
			rpmcli_argv[rpmcli_argc] = NULL;

			rpmcli_context = rpmcliInit(rpmcli_argc, (char * const*)rpmcli_argv, optionsTable);
			qva = &rpmQVKArgs;
			rpmVerifyFlags verifyFlags = VERIFY_ALL;
			verifyFlags &= ~qva->qva_flags;
			qva->qva_flags = (rpmQueryFlags) verifyFlags;

			rpmReadConfigFiles ((const char *)NULL, (const char *)NULL);
			rpmts ts = rpmtsCreate();
			ret = rpmcliVerify(ts, qva, poptGetArgs(rpmcli_context));
			ts = rpmtsFree(ts);
			rpmcli_context = rpmcliFini(rpmcli_context);

			res.vflags |= rpmverifypackage_bhmap[i].a_flag;
			if (ret == 0)
				res.vresults |= rpmverifypackage_bhmap[i].a_flag;

		}
		if (callback(ctx, &res)) {
			ret = 1;
			goto ret;
		}
		free(res.name);
		free(res.epoch);
		free(res.version);
		free(res.release);
		free(res.arch);
	}

	match = rpmdbFreeIterator (match);
	ret   = 0;
ret:
	RPMVERIFY_UNLOCK;
	return (ret);
}

void *probe_init (void)
{
	if (rpmReadConfigFiles ((const char *)NULL, (const char *)NULL) != 0) {
		dI("rpmReadConfigFiles failed: %u, %s.\n", errno, strerror (errno));
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

static int rpmverifypackage_additem(probe_ctx *ctx, struct rpmverify_res *res)
{
	SEXP_t *item, *value;

	item = probe_item_create(OVAL_LINUX_RPMVERIFYPACKAGE, NULL,
				 "name",		OVAL_DATATYPE_STRING, res->name,
				 "epoch", 		OVAL_DATATYPE_STRING, res->epoch,
				 "version", 		OVAL_DATATYPE_STRING, res->version,
				 "release", 	 	OVAL_DATATYPE_STRING, res->release,
				 "arch", 		OVAL_DATATYPE_STRING, res->arch,
				 "extended_name", 	OVAL_DATATYPE_STRING, res->extended_name,
				 NULL);

	if (res->vflags & VERIFY_DEPS) {
		dI("VERIFY_DEPS %d\n", res->vresults & VERIFY_DEPS);
		value = probe_entval_from_cstr(OVAL_DATATYPE_BOOLEAN, (res->vresults & VERIFY_DEPS ? "1" : "0"), 1);
		probe_item_ent_add(item, "dependency_check_passed", NULL, value);
		SEXP_free(value);
	}
	if (res->vflags & VERIFY_SCRIPT) {
		dI("VERIFY_SCRIPT %d\n", res->vresults & VERIFY_SCRIPT);
		value = probe_entval_from_cstr(OVAL_DATATYPE_BOOLEAN, (res->vresults & VERIFY_SCRIPT ? "1" : "0"), 1);
		probe_item_ent_add(item, "verification_script_successful", NULL, value);
		SEXP_free(value);
	}

	return probe_item_collect(ctx, item) == 2 ? 1 : 0;
}
int probe_main (probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *bh_ent;
	SEXP_t *name_ent, *epoch_ent, *version_ent, *release_ent, *arch_ent;
	uint64_t collect_flags = 0;
	unsigned int i;

	/*
	 * Get refs to object entities
	 */
	probe_in = probe_ctx_getobject(ctx);

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

		for (i = 0; i < sizeof rpmverifypackage_bhmap/sizeof(rpmverifypackage_bhmap_t); ++i) {
			aval = probe_ent_getattrval(bh_ent, rpmverifypackage_bhmap[i].a_name);

			if (aval != NULL) {
				if (SEXP_strcmp(aval, "true") == 0) {
					dI("omit verify attr: %s\n", rpmverifypackage_bhmap[i].a_name);
					collect_flags |= rpmverifypackage_bhmap[i].a_flag;
				}

				SEXP_free(aval);
			}
		}

		SEXP_free(bh_ent);
	}

	if (rpmverify_collect(ctx,
			      name_ent, epoch_ent, version_ent, release_ent, arch_ent,
			      collect_flags,
			      rpmverifypackage_additem) != 0)
	{
		dE("An error ocured while collecting rpmverifypackage data\n");
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
	}

	SEXP_free(name_ent);
	SEXP_free(epoch_ent);
	SEXP_free(version_ent);
	SEXP_free(release_ent);
	SEXP_free(arch_ent);

	return 0;
}
