/**
 * @file   rpmverifypackage_probe.c
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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

#include "rpm-helper.h"
#include "probe-chroot.h"

/* Individual RPM headers */
#include <rpm/rpmfi.h>
#include <rpm/rpmcli.h>
#include <popt.h>

/* SEAP */
#include <probe-api.h>
#include "debug_priv.h"
#include "probe/entcmp.h"

#include <probe/probe.h>
#include <probe/option.h>

#include "rpmverifypackage_probe.h"

typedef struct {
	const char *a_name;
	uint64_t    a_flag;
	const char *a_option;
} rpmverifypackage_bhmap_t;

const rpmverifypackage_bhmap_t rpmverifypackage_bhmap[] = {
	{ "nodeps",        (uint64_t)VERIFY_DEPS      , "--nodeps"},
	{ "noscripts",     (uint64_t)VERIFY_SCRIPT    , "--noscript"},
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

struct verifypackage_global {
	struct rpm_probe_global rpm;
	struct probe_chroot chr;
};

static struct poptOption optionsTable[] = {
	{ NULL, '\0', POPT_ARG_INCLUDE_TABLE, rpmcliAllPoptTable, 0,
	    "Common options for all rpm modes and executables:", NULL },
	{ NULL, '\0', POPT_ARG_INCLUDE_TABLE, rpmVerifyPoptTable, 0,
	    "Query options (with -q or --query):", NULL },
	POPT_AUTOALIAS
	POPT_AUTOHELP
	POPT_TABLEEND
};

#define RPMVERIFY_LOCK   RPM_MUTEX_LOCK(&g_rpm->rpm.mutex)

#define RPMVERIFY_UNLOCK RPM_MUTEX_UNLOCK(&g_rpm->rpm.mutex)

#define CHROOT_ENTER() probe_chroot_enter(&g_rpm->chr)

#define CHROOT_LEAVE() probe_chroot_leave(&g_rpm->chr)

#define CHROOT_IS_SET() probe_chroot_is_set(&g_rpm->chr)

#define CHROOT_PATH() probe_chroot_get_path(&g_rpm->chr)

/* modify passed-in iterator to test also given entity */
static int adjust_filter(rpmdbMatchIterator iterator, SEXP_t *ent, rpmTag rpm_tag) {
	oval_operation_t ent_op;
	char ent_str[1024] = "";
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
			int (*callback)(probe_ctx *, struct rpmverify_res *),
			struct verifypackage_global *g_rpm)
{
	rpmdbMatchIterator match;
	Header pkgh;
	int  ret = -1;
	unsigned int i, j, rpmcli_argc = 0;
	const char * rpmcli_argv[10];
	poptContext rpmcli_context;
	QVA_t qva;

	RPMVERIFY_LOCK;

	match = rpmtsInitIterator (g_rpm->rpm.rpmts, RPMDBI_PACKAGES, NULL, 0);
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

	if (RPMTAG_BASENAMES == 0 || RPMTAG_DIRNAMES == 0) {
		return -1;
	}

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

			if (CHROOT_IS_SET())
			{
				rpmLibsPreload();
				if (CHROOT_ENTER() < 0) {
					ret = 1;
					goto ret;
				}
			}

			rpmcli_context = rpmcliInit(rpmcli_argc, (char * const*)rpmcli_argv, optionsTable);
			qva = &rpmQVKArgs;
			rpmVerifyFlags verifyFlags = VERIFY_ALL;
			verifyFlags &= ~qva->qva_flags;
			qva->qva_flags = (rpmQueryFlags) verifyFlags;

			// rpmcliFini() causes free of rpmrc, macros, ...
			// so we have to reload everything again
			rpmReadConfigFiles ((const char *)NULL, (const char *)NULL);

			/*
			* Fedora >=36 changed the default dbpath in librpm from /var/lib/rpm to /usr/lib/sysimage/rpm
			* See: https://fedoraproject.org/wiki/Changes/RelocateRPMToUsr
			* Therefore, when running openscap on a Fedora >=36 system scanning another systems (such as RHEL, SLES, Fedora<36)
			* openscap's librpm will try to read the rpm db from /usr/lib/sysimage/rpm which doesn't exist and therefore won't work.
			* In implementing this change, /var/lib/rpm is still a symlink to /usr/lib/sysimage/rpm
			* so /var/lib/rpm still works. So /var/lib/rpm is a dbpath that will work on all systems.
			* Therefore, set the dbpath to be /var/lib/rpm, allow openscap running on any system to scan any system.
			*/
			rpmPushMacro(NULL, "_dbpath", NULL, "/var/lib/rpm", RMIL_CMDLINE);

			rpmts ts = rpmtsCreate();
			char* const * args = (char* const *)poptGetArgs(rpmcli_context);

			if (CHROOT_IS_SET()){

				// plugins for offline mode can cause, that .so from
				// container are loaded - we don't want it
				DISABLE_PLUGINS(ts);
				CHROOT_LEAVE();
			} else {
				ret = rpmcliVerify(ts, qva, args);
			}

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

int rpmverifypackage_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_OWN;
}

void *rpmverifypackage_probe_init(void)
{
	const char* root = getenv("OSCAP_PROBE_ROOT");
	if ((root!= NULL) && (strlen(root) == 0)) {
		root = NULL;
	}

	struct verifypackage_global *g_rpm = malloc(sizeof(struct verifypackage_global));
	probe_chroot_init(&g_rpm->chr, root);

#ifdef RPM46_FOUND
	rpmlogSetCallback(rpmErrorCb, NULL);
#endif

	if (CHROOT_IS_SET()) {
		rpmLibsPreload();
		if (CHROOT_ENTER() < 0) {
			probe_chroot_free(&g_rpm->chr);
			free(g_rpm);
			return (NULL);
		}
	}

	if (rpmReadConfigFiles (NULL, (const char *)NULL) != 0) {
		dD("rpmReadConfigFiles failed: %u, %s.", errno, strerror (errno));
		g_rpm->rpm.rpmts = NULL;
		return ((void *)g_rpm);
	}


	g_rpm->rpm.rpmts = rpmtsCreate();

	if (CHROOT_IS_SET()) {
		CHROOT_LEAVE();

		// plugins for offline mode can cause, that .so from
		// container are loaded - we don't want it
		DISABLE_PLUGINS(g_rpm->rpm.rpmts);

		rpmtsSetRootDir(g_rpm->rpm.rpmts, CHROOT_PATH());
	}

	set_rpm_db_path();
	pthread_mutex_init(&(g_rpm->rpm.mutex), NULL);
	return ((void *)g_rpm);
}

void rpmverifypackage_probe_fini(void *ptr)
{
	struct verifypackage_global *r = (struct verifypackage_global *)ptr;

	rpmFreeCrypto();
	rpmFreeRpmrc();
	rpmFreeMacros(NULL);
	rpmlogClose();

	// If r is null, probe_init() failed during chroot
	if (r == NULL)
		return;

	// This will be always set by probe_init(), lets free it
	probe_chroot_free(&r->chr);

	// If r->rpm.rpmts was not initialized the mutex was not as well
	if (r->rpm.rpmts == NULL)
		return;

	rpmtsFree(r->rpm.rpmts);
	pthread_mutex_destroy (&(r->rpm.mutex));

	free(r);
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
		dD("VERIFY_DEPS %lu", res->vresults & VERIFY_DEPS);
		value = probe_entval_from_cstr(OVAL_DATATYPE_BOOLEAN, (res->vresults & VERIFY_DEPS ? "1" : "0"), 1);
		probe_item_ent_add(item, "dependency_check_passed", NULL, value);
		SEXP_free(value);
	}
	if (res->vflags & VERIFY_SCRIPT) {
		dD("VERIFY_SCRIPT %lu", res->vresults & VERIFY_SCRIPT);
		value = probe_entval_from_cstr(OVAL_DATATYPE_BOOLEAN, (res->vresults & VERIFY_SCRIPT ? "1" : "0"), 1);
		probe_item_ent_add(item, "verification_script_successful", NULL, value);
		SEXP_free(value);
	}

	return probe_item_collect(ctx, item) == 2 ? 1 : 0;
}
int rpmverifypackage_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *bh_ent;
	SEXP_t *name_ent, *epoch_ent, *version_ent, *release_ent, *arch_ent;
	uint64_t collect_flags = 0;
	unsigned int i;

	// arg is NULL if we were not able to chroot during probe_init()
	if (arg == NULL) {
		return PROBE_EINIT;
	}
	struct verifypackage_global *g_rpm = (struct verifypackage_global *)arg;

	// There was no rpm config files
	if (g_rpm->rpm.rpmts == NULL) {
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_NOT_APPLICABLE);
		return 0;
	}

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
					dD("omit verify attr: %s", rpmverifypackage_bhmap[i].a_name);
					collect_flags |= rpmverifypackage_bhmap[i].a_flag;
				}

				SEXP_free(aval);
			}
		}

		SEXP_free(bh_ent);
	}

	if (rpmverify_collect(ctx, name_ent, epoch_ent, version_ent, release_ent,
			arch_ent, collect_flags, rpmverifypackage_additem, g_rpm) != 0)
	{
		dE("An error ocured while collecting rpmverifypackage data");
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
	}

	SEXP_free(name_ent);
	SEXP_free(epoch_ent);
	SEXP_free(version_ent);
	SEXP_free(release_ent);
	SEXP_free(arch_ent);

	return 0;
}
