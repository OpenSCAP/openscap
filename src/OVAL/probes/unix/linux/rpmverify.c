/**
 * @file   rpmverify.c
 * @brief  rpmverify probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
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
        char *file;  /**< filepath */
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

static int rpmverify_collect(probe_ctx *ctx,
                             const char *name, oval_operation_t name_op,
                             const char *file, oval_operation_t file_op,
			     SEXP_t *name_ent, SEXP_t *filepath_ent,
                             uint64_t flags,
                             void (*callback)(probe_ctx *, struct rpmverify_res *))
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

        switch (name_op) {
        case OVAL_OPERATION_EQUALS:
                match = rpmtsInitIterator (g_rpm.rpmts, RPMTAG_NAME, (const void *)name, 0);

                if (match == NULL) {
                        ret = 0;
                        goto ret;
                }

                ret = rpmdbGetIteratorCount (match);

                break;
	case OVAL_OPERATION_NOT_EQUAL:
                match = rpmtsInitIterator (g_rpm.rpmts, RPMDBI_PACKAGES, NULL, 0);

                if (match == NULL) {
                        ret = 0;
                        goto ret;
                }

                if (rpmdbSetIteratorRE (match, RPMTAG_NAME, RPMMIRE_GLOB, "*") != 0)
                {
                        ret = -1;
                        goto ret;
                }

                break;
        case OVAL_OPERATION_PATTERN_MATCH:
                match = rpmtsInitIterator (g_rpm.rpmts, RPMDBI_PACKAGES, NULL, 0);

                if (match == NULL) {
                        ret = 0;
                        goto ret;
                }

                if (rpmdbSetIteratorRE (match, RPMTAG_NAME, RPMMIRE_REGEX,
                                        (const char *)name) != 0)
                {
                        ret = -1;
                        goto ret;
                }

                break;
        default:
                /* not supported */
                dE("package name: operation not supported");
                ret = -1;
                goto ret;
        }

	assume_d(RPMTAG_BASENAMES != 0, -1);
	assume_d(RPMTAG_DIRNAMES  != 0, -1);

        while ((pkgh = rpmdbNextIterator (match)) != NULL) {
                rpmfi  fi;
		rpmTag tag[2] = { RPMTAG_BASENAMES, RPMTAG_DIRNAMES };
                struct rpmverify_res res;
                errmsg_t rpmerr;
		int i;
		SEXP_t *name_sexp;

                res.name = headerFormat(pkgh, "%{NAME}", &rpmerr);

		name_sexp = SEXP_string_newf("%s", res.name);
		if (probe_entobj_cmp(name_ent, name_sexp) != OVAL_RESULT_TRUE) {
			SEXP_free(name_sexp);
			continue;
		}
		SEXP_free(name_sexp);

                /*
                 * Inspect package files & directories
                 */
		for (i = 0; i < 2; ++i) {
		  fi = rpmfiNew(g_rpm.rpmts, pkgh, tag[i], 1);

		  while (rpmfiNext(fi) != -1) {
		    SEXP_t *filepath_sexp;

		    res.fflags = rpmfiFFlags(fi);
		    res.oflags = omit;

		    if (((res.fflags & RPMFILE_CONFIG) && (flags & RPMVERIFY_SKIP_CONFIG)) ||
			((res.fflags & RPMFILE_GHOST)  && (flags & RPMVERIFY_SKIP_GHOST)))
		      continue;

		    res.file   = strdup(rpmfiFN(fi));

		    filepath_sexp = SEXP_string_newf("%s", res.file);
		    if (probe_entobj_cmp(filepath_ent, filepath_sexp) != OVAL_RESULT_TRUE) {
		      SEXP_free(filepath_sexp);
		      oscap_free(res.file);
		      continue;
		    }
		    SEXP_free(filepath_sexp);

		    if (rpmVerifyFile(g_rpm.rpmts, fi, &res.vflags, omit) != 0)
		      res.vflags = RPMVERIFY_FAILURES;

		    callback(ctx, &res);
		    free(res.file);
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

	rpmFreeCrypto();
	rpmFreeRpmrc();
	rpmFreeMacros(NULL);
	rpmlogClose();

	// If probe_init() failed r->rpmts and r->mutex were not initialized
	if (r == NULL)
		return;

	rpmtsFree(r->rpmts);
	pthread_mutex_destroy (&(r->mutex));

        return;
}

static void rpmverify_additem(probe_ctx *ctx, struct rpmverify_res *res)
{
        SEXP_t *item;

#define VF_RESULT(f) (res->oflags & (f) ? "not performed" : (res->vflags & RPMVERIFY_FAILURES ? "not performed" : (res->vflags & (f) ? "fail" : "pass")))
#define FF_RESULT(f) (res->fflags & (f) ? true : false)

        item = probe_item_create(OVAL_LINUX_RPMVERIFY, NULL,
                                 "name",                OVAL_DATATYPE_STRING, res->name,
                                 "filepath",            OVAL_DATATYPE_STRING, res->file,
                                 "size_differs",        OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_FILESIZE),
                                 "mode_differs",        OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_MODE),
                                 "md5_differs",         OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_MD5),
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
                                 "ghost_file",          OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_GHOST),
                                 "license_file",        OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_LICENSE),
                                 "readme_file",         OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_README),
                                 NULL);

        probe_item_collect(ctx, item);
}

typedef struct {
        const char *a_name;
        uint64_t    a_flag;
} rpmverify_bhmap_t;

const rpmverify_bhmap_t rpmverify_bhmap[] = {
        { "nodeps",        (uint64_t)VERIFY_DEPS      },
        { "nodigest",      (uint64_t)VERIFY_DIGEST    },
        { "nofiles",       (uint64_t)VERIFY_FILES     },
        { "noscripts",     (uint64_t)VERIFY_SCRIPT    },
        { "nosignature",   (uint64_t)VERIFY_SIGNATURE },
        { "nolinkto",      (uint64_t)VERIFY_LINKTO    },
        { "nomd5",         (uint64_t)VERIFY_MD5       },
        { "nosize",        (uint64_t)VERIFY_SIZE      },
        { "nouser",        (uint64_t)VERIFY_USER      },
        { "nogroup",       (uint64_t)VERIFY_GROUP     },
        { "nomtime",       (uint64_t)VERIFY_MTIME     },
        { "nomode",        (uint64_t)VERIFY_MODE      },
        { "nordev",        (uint64_t)VERIFY_RDEV      },
        { "noconfigfiles", RPMVERIFY_SKIP_CONFIG      },
        { "noghostfiles",  RPMVERIFY_SKIP_GHOST       }
};

int probe_main (probe_ctx *ctx, void *arg)
{
        SEXP_t *probe_in, *name_ent, *file_ent, *bh_ent;
        char   file[PATH_MAX];
        size_t file_len = sizeof file;
        char   name[64];
        size_t name_len = sizeof name;
        oval_operation_t name_op, file_op;
        uint64_t collect_flags = 0;
        unsigned int i;

	// If probe_init() failed it's because there was no rpm config files
	if (arg == NULL) {
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_NOT_APPLICABLE);
		return 0;
	}

        /*
         * Get refs to object entities
         */
        probe_in = probe_ctx_getobject(ctx);
        name_ent = probe_obj_getent(probe_in, "name", 1);
        file_ent = probe_obj_getent(probe_in, "filepath", 1);

        if (name_ent == NULL || file_ent == NULL) {
                dE("Missing \"name\" (%p) or \"filepath\" (%p) entity", name_ent, file_ent);

                SEXP_free(name_ent);
                SEXP_free(file_ent);

                return (PROBE_ENOENT);
        }

        /*
         * Extract the requested operation for each entity
         */
        name_op = probe_ent_getoperation(name_ent, OVAL_OPERATION_EQUALS);
        file_op = probe_ent_getoperation(file_ent, OVAL_OPERATION_EQUALS);

        if (name_op == OVAL_OPERATION_UNKNOWN ||
            file_op == OVAL_OPERATION_UNKNOWN)
        {
                SEXP_free(name_ent);
                SEXP_free(file_ent);

                return (PROBE_EINVAL);
        }

        /*
         * Extract entity values
         */
        PROBE_ENT_STRVAL(name_ent, name, name_len, /* void */, strcpy(name, ""););
        PROBE_ENT_STRVAL(file_ent, file, file_len, /* void */, strcpy(file, ""););

        /*
         * Parse behaviors
         */
        bh_ent = probe_obj_getent(probe_in, "behaviors", 1);

        if (bh_ent != NULL) {
                SEXP_t *aval;

                for (i = 0; i < sizeof rpmverify_bhmap/sizeof(rpmverify_bhmap_t); ++i) {
                        aval = probe_ent_getattrval(bh_ent, rpmverify_bhmap[i].a_name);

                        if (aval != NULL) {
                                if (SEXP_strcmp(aval, "true") == 0) {
                                        dI("omit verify attr: %s", rpmverify_bhmap[i].a_name);
                                        collect_flags |= rpmverify_bhmap[i].a_flag;
                                }

                                SEXP_free(aval);
                        }
                }

                SEXP_free(bh_ent);
        }

        dI("Collecting rpmverify data, query: n=\"%s\" (%d), f=\"%s\" (%d)",
           name, name_op, file, file_op);

        if (rpmverify_collect(ctx,
                              name, name_op,
                              file, file_op,
			      name_ent, file_ent,
                              collect_flags,
                              rpmverify_additem) != 0)
        {
                dE("An error ocured while collecting rpmverify data");
                probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
        }

	SEXP_free(name_ent);
	SEXP_free(file_ent);

        return 0;
}
