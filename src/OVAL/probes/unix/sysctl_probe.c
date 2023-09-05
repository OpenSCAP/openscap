/**
 * @file   sysctl_probe.c
 * @brief  implementation of the sysctl_object
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <probe/probe.h>
#include "probe-api.h"
#include "probe/entcmp.h"
#include "sysctl_probe.h"

#if defined(OS_FREEBSD)
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "common/debug_priv.h"

#define SYSCTL_CMD "/sbin/sysctl -ae"
#endif

#if defined(OS_LINUX)

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include "oval_fts.h"
#include "common/debug_priv.h"

#define PROC_SYS_DIR "/proc/sys"
#define PROC_SYS_MAXDEPTH 7

int sysctl_probe_offline_mode_supported(void)
{
        return PROBE_OFFLINE_OWN;
}

int sysctl_probe_main(probe_ctx *ctx, void *probe_arg)
{
        OVAL_FTS    *ofts;
        OVAL_FTSENT *ofts_ent;

        SEXP_t *name_entity, *probe_in;
        SEXP_t *r0, *r1, *r2, *r3;
        SEXP_t *ent_attrs, *bh_entity, *path_entity, *filename_entity;
        oval_schema_version_t over;
        int over_cmp;

        const char *ipv6_conf_path = "/proc/sys/net/ipv6/conf/";
        size_t ipv6_conf_path_len = strlen(ipv6_conf_path);

        probe_in    = probe_ctx_getobject(ctx);
        name_entity = probe_obj_getent(probe_in, "name", 1);
        over        = probe_obj_get_platform_schema_version(probe_in);
        over_cmp    = oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.10));

        if (name_entity == NULL) {
                dE("Missing \"name\" entity in the input object");
                return (PROBE_ENOENT);
        }

        /*
         * prepare behaviors
         */
        ent_attrs = probe_attr_creat("max_depth",           r0 = SEXP_string_newf("%d", PROC_SYS_MAXDEPTH),
                                     "recurse_direction",   r1 = SEXP_string_new("down", 4),
                                     "recurse_file_system", r2 = SEXP_string_new("all", 3),
                                     "recurse", r3 = SEXP_string_new("symlinks and directories", 24),
                                     NULL);
        bh_entity = probe_ent_creat1("behaviors", ent_attrs, NULL);
        SEXP_free(r0);
        SEXP_free(r1);
        SEXP_free(r2);
        SEXP_free(r3);
        SEXP_free(ent_attrs);

        /*
         * prepare path, filename
         */
        ent_attrs = probe_attr_creat("operation", r0 = SEXP_number_newi(OVAL_OPERATION_EQUALS),
                                     NULL);
        path_entity = probe_ent_creat1("path", ent_attrs, r1 = SEXP_string_new(PROC_SYS_DIR, strlen(PROC_SYS_DIR)));
        SEXP_free(r0);
        SEXP_free(r1);
        SEXP_free(ent_attrs);

        ent_attrs = probe_attr_creat("operation", r0 = SEXP_number_newi(OVAL_OPERATION_PATTERN_MATCH),
                                     NULL);
        filename_entity = probe_ent_creat1("filename", ent_attrs, r1 = SEXP_string_new(".*", 2));
        SEXP_free(r0);
        SEXP_free(r1);
        SEXP_free(ent_attrs);

        /*
         * collect sysctls
         *  XXX: use direct access for the "equals" op
         */
        const char *prefix = getenv("OSCAP_PROBE_ROOT");
        ofts = oval_fts_open_prefixed(prefix, path_entity, filename_entity, NULL, bh_entity, probe_ctx_getresult(ctx));

        if (ofts == NULL) {
                SEXP_free(path_entity);
                SEXP_free(filename_entity);
                SEXP_free(bh_entity);
                SEXP_free(name_entity);

                if (prefix != NULL) {
                        DIR *d = opendir(prefix);
                        if (d != NULL) {
                                closedir(d);
                                return PROBE_ESUCCESS;
                        }
                        dW("Can't open prefix directory '%s': %s", prefix, strerror(errno));
                }

                dE("oval_fts_open_prefixed(%s, %s, %s, ...) failed", prefix, PROC_SYS_DIR, ".*");
                return PROBE_EFATAL;
        }

        while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
                SEXP_t *se_mib;
                char    mibpath[PATH_MAX], *mib;
                size_t  miblen, mibstart;
                struct stat file_stat;

                if (prefix != NULL) {
                        snprintf(mibpath, sizeof mibpath, "%s/%s/%s", prefix, ofts_ent->path, ofts_ent->file);
                } else {
                        snprintf(mibpath, sizeof mibpath, "%s/%s", ofts_ent->path, ofts_ent->file);
                }

                /* Skip write-only files, eg. /proc/sys/net/ipv4/route/flush */
                if (stat(mibpath, &file_stat) == -1) {
                        dE("Stat failed on %s: %u, %s", mibpath, errno, strerror(errno));
                        oval_ftsent_free(ofts_ent);
                        continue;
                }
                /* the sysctl utility uses same condition in sysctl.c in ReadSetting() */
                if ((file_stat.st_mode & S_IRUSR) == 0) {
                        dD("Skipping write-only file %s", mibpath);
                        oval_ftsent_free(ofts_ent);
                        continue;
                }

                mibstart = 0;
                mibstart += prefix != NULL ? strlen(prefix)+1 : 0;
                mibstart += strlen(PROC_SYS_DIR)+1;
                mib    = strdup(mibpath + mibstart);
                miblen = strlen(mib);

                while (miblen > 0) {
                        if(mib[miblen - 1] == '/')
                                mib[miblen - 1] = '.';
                        --miblen;
                }

                dD("MIB: %s", mib);
                se_mib = SEXP_string_new(mib, strlen(mib));
                free(mib);

                if (probe_entobj_cmp(name_entity, se_mib) == OVAL_RESULT_TRUE) {
                        FILE   *fp;
                        SEXP_t *item;
                        char    sysval[8192];
                        char   *sysvals[512];
                        long i, l;
                        size_t s;

                        dD("MIB match");

                        /*
                         * read sysctl value
                         */
                        fp = fopen(mibpath, "r");

                        if (fp == NULL) {
                                dE("Can't read sysctl value from \"%s\": %u, %s",
                                   mibpath, errno, strerror(errno));
                                goto fail_item;
                        }

                        l = fread(sysval, 1, sizeof sysval - 1, fp);

                        if (ferror(fp)) {
                                /* Linux 4.1.0 introduced a per-NIC IPv6 stable_secret file.
                                 * The stable_secret file cannot be read until it is set,
                                 * so we skip it when it is not readable. Otherwise we collect it.
                                 */
                                if (strncmp(ofts_ent->path, ipv6_conf_path, ipv6_conf_path_len) == 0 &&
                                                strcmp(ofts_ent->file, "stable_secret") == 0) {
                                        dD("Skipping file %s", mibpath);
                                        oval_ftsent_free(ofts_ent);
                                        SEXP_free(se_mib);
                                        fclose(fp);
                                        continue;
                                } else {
                                        dE("An error occurred when reading from \"%s\" (fp=%p): l=%ld, %u, %s",
                                                mibpath, fp, l, errno, strerror(errno));
                                        goto fail_item;
                                }
                        }

                        fclose(fp);

                        /* Skip empty values as sysctl tool does.
                         * See https://bugzilla.redhat.com/show_bug.cgi?id=1473207
                         */
                        if (l == 0) {
                                dD("Skipping file '%s' because it has no value.", mibpath);
                                oval_ftsent_free(ofts_ent);
                                SEXP_free(se_mib);
                                continue;
                        }

                        /*
                         * sanitize the value
                         *  - only printable and whitespace chars allowed
                         *  - remove the last '\n'
                         */
                        sysvals[0] = sysval;

                        for(s = 0, i = 0; i < l && s < sizeof sysvals/sizeof(char *) - 1; ++i) {
                                if ((!isprint(sysval[i]) && !isspace(sysval[i]))
                                    || (over_cmp >= 0 && sysval[i] == '\n' /* OVAL 5.10 and above */))
                                {
                                        sysval[i] = '\0';
                                        sysvals[++s] = sysval + i + 1;
                                }
                        }

                        if (sysval[l - 1] == '\n')
                                sysval[l - 1] = '\0';
                        else
                                sysval[l] = '\0';

                        if (strlen(sysvals[s]) == 0)
                                sysvals[s] = NULL;
                        else
                                sysvals[++s] = NULL;

                        if (over_cmp >= 0) {
                                /* Only in OVAL 5.10 and above */
                                item = probe_item_create(OVAL_UNIX_SYSCTL, NULL,
                                                         "name",  OVAL_DATATYPE_SEXP,   se_mib,
                                                         "value", OVAL_DATATYPE_STRING_M, sysvals,
                                                         NULL);
                        } else {
                                item = probe_item_create(OVAL_UNIX_SYSCTL, NULL,
                                                         "name",  OVAL_DATATYPE_SEXP,   se_mib,
                                                         "value", OVAL_DATATYPE_STRING, sysval,
                                                         NULL);
                        }

                        goto add_item;
                fail_item:
                        if (fp != NULL)
                                fclose(fp);

                        item = probe_item_create(OVAL_UNIX_SYSCTL, NULL, NULL);
                        probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);
                add_item:
                        probe_item_collect(ctx, item);
                }

                oval_ftsent_free(ofts_ent);
                SEXP_free(se_mib);
        }

        oval_fts_close(ofts);
        SEXP_free(path_entity);
        SEXP_free(filename_entity);
        SEXP_free(bh_entity);
        SEXP_free(name_entity);

        return (0);
}

#elif defined(OS_FREEBSD)

int sysctl_probe_offline_mode_supported(void)
{
        return PROBE_OFFLINE_NONE;
}

int sysctl_probe_main(probe_ctx *ctx, void *probe_arg)
{
        FILE *fp;
        char output[LINE_MAX];
        const char* SEP = "=";
        char* mib;
        char* sysval;
        SEXP_t *se_mib;
        SEXP_t *name_entity, *probe_in;

        probe_in    = probe_ctx_getobject(ctx);
        name_entity = probe_obj_getent(probe_in, "name", 1);

        if (name_entity == NULL) {
                dE("Missing \"name\" entity in the input object");
                return (PROBE_ENOENT);
        }

        /* FreeBSD's sysctl(8) uses undocumented, and potentially unstable,
         * kernel interfaces to obtain the list of system properties and values.
         * Hence we call the executable and parse its output rather than
         * implement the functionality ourselves which risks breakage if/when
         * the interfaces change.
         */
        fp = popen(SYSCTL_CMD, "r");

        if (!fp) {
                dE("Failed to open output of %s", SYSCTL_CMD);
                return (PROBE_EFATAL);
        }

        while (fgets(output, sizeof(output), fp)) {
                char *strp;
                mib = strtok_r(output, SEP, &strp);
                sysval = strtok_r(NULL, SEP, &strp);

                if (!mib)
                        continue;

                if (!sysval)
                        continue;

                se_mib = SEXP_string_new(mib, strlen(mib));

                if (!se_mib) {
                        dE("Failed to allocate new SEXP_string for se_mib");
                        pclose(fp);
                        return (PROBE_ENOENT);
                }

                /* Remove newline */
                sysval[strlen(sysval)-1] = '\0';

                if (probe_entobj_cmp(name_entity, se_mib) == OVAL_RESULT_TRUE) {
                        SEXP_t *item;

                        item = probe_item_create(OVAL_UNIX_SYSCTL, NULL,
                                                "name", OVAL_DATATYPE_SEXP, se_mib,
                                                "value", OVAL_DATATYPE_STRING, sysval,
                                                NULL);

                        if (!item) {
                                dE("probe_item_create() returned a null item");
                                pclose(fp);
                                SEXP_free(se_mib);
                                return (PROBE_ENOENT);
                        }

                        probe_item_collect(ctx, item);
                }

                SEXP_free(se_mib);
        }

        pclose(fp);
        return (0);
}

#else

int sysctl_probe_offline_mode_supported(void)
{
        return PROBE_OFFLINE_NONE;
}

int sysctl_probe_main(probe_ctx *ctx, void *probe_arg)
{
        return(PROBE_EOPNOTSUPP);
}

#endif
