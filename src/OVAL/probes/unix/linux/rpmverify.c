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

/* RPM headers */
#include <rpm/rpmdb.h>
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmmacro.h>
#include <rpm/rpmlog.h>

#ifdef HAVE_LIBRPM44
#include <rpm/header.h>
#define headerFormat(_h, _fmt, _emsg) headerSprintf((_h),( _fmt), rpmTagTable, rpmHeaderFormats, (_emsg))
#define rpmFreeCrypto() while(0)
#define rpmFreeFilesystems() while(0)
#endif

/* SEAP */
#include <probe-api.h>
#include <alloc.h>
#include <common/assume.h>
#include "debug_priv.h"
#include "probe/entcmp.h"

#ifndef _A
#define _A(x) assert(x)
#endif

struct rpmverify_res {
        char *name;  /**< package name */
        const char *file;  /**< filepath */
        rpmVerifyAttrs vflags; /**< rpm verify flags */
        rpmfileAttrs   fflags; /**< rpm file flags */
#if 0
        char *arch;
        char *epoch;
        char *release;
        char *version;
        char *evr;
        char *signature_keyid;
#endif
};

struct rpmverify_global {
        rpmts           rpmts;
        pthread_mutex_t mutex;
};

static struct rpmverify_global g_rpm;

#define RPMVERIFY_LOCK                                          \
        do {                                                    \
                if (pthread_mutex_lock(&g_rpm.mutex) != 0) {    \
                        dE("Can't lock mutex\n");               \
                        return (-1);                            \
                }                                               \
        } while(0)

#define RPMVERIFY_UNLOCK                                                \
        do {                                                            \
                if (pthread_mutex_unlock(&g_rpm.mutex) != 0) {          \
                        dE("Can't unlock mutex. Aborting...\n");        \
                        abort();                                        \
                }                                                       \
        } while(0)
#if 0
static void pkgh2rep (Header h, struct rpmverify_rep *r)
{
        errmsg_t rpmerr;
        char *str, *sid;
        size_t len;

        assume_d (h != NULL, /* void */);
        assume_d (r != NULL, /* void */);

        r->name = headerFormat (h, "%{NAME}", &rpmerr);

#if 0
        r->arch = headerFormat (h, "%{ARCH}", &rpmerr);
        str     = headerFormat (h, "%{EPOCH}", &rpmerr);

        if (strcmp (str, "(none)") == 0) {
                str    = oscap_realloc (str, sizeof (char) * 2);
                str[0] = '0';
                str[1] = '\0';
        }

        r->epoch   = str;
        r->release = headerFormat (h, "%{RELEASE}", &rpmerr);
        r->version = headerFormat (h, "%{VERSION}", &rpmerr);

        len = (strlen (r->epoch)   +
               strlen (r->release) +
               strlen (r->version) + 2);

        str = oscap_alloc (sizeof (char) * (len + 1));
        snprintf (str, len + 1, "%s:%s-%s",
                  r->epoch,
                  r->version,
                  r->release);

        r->evr = str;

        str = headerFormat (h, "%{SIGGPG:pgpsig}", &rpmerr);
        sid = strrchr (str, ' ');
        r->signature_keyid = (sid != NULL ? strdup (sid+1) : strdup ("0"));
        oscap_free (str);
#endif
}
#endif

static int rpmverify_collect(probe_ctx *ctx,
                             const char *name, oval_operation_t name_op,
                             const char *file, oval_operation_t file_op,
                             void (*callback)(probe_ctx *, struct rpmverify_res *))
{
	rpmdbMatchIterator match;
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
                dE("package name: operation not supported\n");
                ret = -1;
                goto ret;
        }

        while ((pkgh = rpmdbNextIterator (match)) != NULL) {
                rpmfi  fi;
                struct rpmverify_res res;
                errmsg_t rpmerr;

                res.name = headerFormat(pkgh, "%{NAME}", &rpmerr);

                /*
                 * Inspect package files
                 */
                fi = rpmfiNew(g_rpm.rpmts, pkgh, RPMTAG_FILENAMES, 1);

                while (rpmfiNext(fi) != -1) {
                        res.file   = rpmfiFN(fi);
                        res.fflags = rpmfiFFlags(fi);

                        if (rpmVerifyFile(g_rpm.rpmts, fi, &res.vflags, RPMVERIFY_NONE) != 0)
                                res.vflags = RPMVERIFY_FAILURES;

                        switch(file_op) {
                        case OVAL_OPERATION_EQUALS:
                                if (strcmp(res.file, file) != 0)
                                        continue;
                                break;
                        case OVAL_OPERATION_NOT_EQUAL:
                                if (strcmp(res.file, file) == 0)
                                        continue;
                                break;
                        case OVAL_OPERATION_PATTERN_MATCH:
                                ret = pcre_exec(re, NULL, res.file, strlen(res.file), 0, 0, NULL, 0);

                                switch(ret) {
                                case 0: /* match */
                                        break;
                                case -1:
                                        /* mismatch */
                                        continue;
                                default:
                                        dE("pcre_exec() failed!\n");
                                        ret = -1;
                                        goto ret;
                                }
                                break;
                        default:
                                /* unsupported operation */
                                dE("Operation \"%d\" on `filepath' not supported\n", file_op);
                                ret = -1;
                                goto ret;
                        }

                        callback(ctx, &res);
                }

                rpmfiFree(fi);
        }

	match = rpmdbFreeIterator (match);
ret:
        if (re != NULL)
                pcre_free(re);

        RPMVERIFY_UNLOCK;
        return (ret);
}

void *probe_init (void)
{
        if (rpmReadConfigFiles ((const char *)NULL, (const char *)NULL) != 0) {
                _D("rpmReadConfigFiles failed: %u, %s.\n", errno, strerror (errno));
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

static void rpmverify_additem(probe_ctx *ctx, struct rpmverify_res *res)
{
        SEXP_t *item;

#define VF_RESULT(f) (res->vflags & RPMVERIFY_FAILURES ? "not performed" : (res->vflags & (f) ? "fail" : "pass"))
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
                                 "capabilities_differ", OVAL_DATATYPE_STRING, VF_RESULT(RPMVERIFY_CAPS),
                                 "configuration_file",  OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_CONFIG),
                                 "documentation_file",  OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_DOC),
                                 "ghost_file",          OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_GHOST),
                                 "license_file",        OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_LICENSE),
                                 "readme_file",         OVAL_DATATYPE_BOOLEAN, FF_RESULT(RPMFILE_README),
                                 NULL);

        probe_item_collect(ctx, item);
}

int probe_main (probe_ctx *ctx, void *arg)
{
        SEXP_t *probe_in, *name_ent, *file_ent;
        char   file[PATH_MAX];
        size_t file_len = sizeof file;
        char   name[64];
        size_t name_len = sizeof name;
        oval_operation_t name_op, file_op;

        /*
         * Get refs to object entities
         */
        probe_in = probe_ctx_getobject(ctx);
        name_ent = probe_obj_getent(probe_in, "name", 1);
        file_ent = probe_obj_getent(probe_in, "filepath", 1);

        if (name_ent == NULL || file_ent == NULL) {
                dE("Missing \"name\" (%p) or \"filepath\" (%p) entity\n", name_ent, file_ent);

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
        PROBE_ENT_STRVAL(name_ent, name, name_len, /* void */, /* void */);
        PROBE_ENT_STRVAL(file_ent, file, file_len, /* void */, /* void */);

        SEXP_free(name_ent);
        SEXP_free(file_ent);

        dI("Collecting rpmverify data, query: n=\"%s\" (%d), f=\"%s\" (%d)\n",
           name, name_op, file, file_op);

        if (rpmverify_collect(ctx,
                              name, name_op,
                              file, file_op,
                              rpmverify_additem) != 0)
        {
                dE("An error ocured while collecting rpmverify data\n");
                probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
        }

        return 0;
}
