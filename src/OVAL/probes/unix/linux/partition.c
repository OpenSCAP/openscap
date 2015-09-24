/**
 * @file   partition.c
 * @brief  partition probe implementation
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

#if defined(PROC_CHECK) && defined(__linux__)
#define _XOPEN_SOURCE /* for fdopen */
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/fs.h>

#ifdef HAVE_PROC_MAGIC
#include <linux/magic.h>
#else /* RHEL5 work-around */
#define PROC_SUPER_MAGIC 0x9fa0
#endif

#endif


#if defined(HAVE_BLKID_GET_TAG_VALUE)
#include <blkid/blkid.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <sys/statvfs.h>
#include <probe-api.h>
#include <mntent.h>
#include <pcre.h>

#include "common/debug_priv.h"

#ifndef MTAB_PATH
# define MTAB_PATH "/proc/mounts"
#endif

#ifndef MTAB_LINE_MAX
# define MTAB_LINE_MAX 4096
#endif

const char *__OVAL_fs_types[][2] = {
	{ "adfs",       "ADFS_SUPER_MAGIC" },
	{ "affs",       "AFFS_SUPER_MAGIC" },
	{ "afs",        "AFS_SUPER_MAGIC" },
	{ "autofs",     "AUTOFS_SUPER_MAGIC" },
	{ "coda",       "CODA_SUPER_MAGIC" },
	{ "cramfs",     "CRAMFS_MAGIC" },
	{ "cramfs",     "CRAMFS_MAGIC_WEND" },
	{ "debugfs",    "DEBUGFS_MAGIC" },
	{ "sysfs",      "SYSFS_MAGIC" },
	{ "securityfs", "SECURITYFS_MAGIC" },
	{ "selinuxfs",  "SELINUX_MAGIC" },
	{ "ramfs",      "RAMFS_MAGIC" },
	{ "tmpfs",      "TMPFS_MAGIC" },
	{ "hugetlbfs",  "HUGETLBFS_MAGIC" },
	{ "squashfs",   "SQUASHFS_MAGIC" },
	{ "efs",        "EFS_SUPER_MAGIC" },
	{ "ext2",       "EXT2_SUPER_MAGIC" },
	{ "ext3",       "EXT3_SUPER_MAGIC" },
	{ "xenfs",      "XENFS_SUPER_MAGIC" },
	{ "ext4",       "EXT4_SUPER_MAGIC" },
	{ "btrfs",      "BTRFS_SUPER_MAGIC" },
	{ "hpfs",       "HPFS_SUPER_MAGIC" },
	{ "iso9660",    "ISOFS_SUPER_MAGIC" },
	{ "jffs2",      "JFFS2_SUPER_MAGIC" },
	{ "anon",       "ANON_INODE_FS_MAGIC" },
	{ "minix",      "MINIX_SUPER_MAGIC" },
	{ "minix",      "MINIX_SUPER_MAGIC2" },
	{ "minix2",     "MINIX2_SUPER_MAGIC" },
	{ "minix2",     "MINIX2_SUPER_MAGIC2" },
	{ "minix3",     "MINIX3_SUPER_MAGIC" },
	{ "msdos",      "MSDOS_SUPER_MAGIC" },
	{ "ncpfs",      "NCP_SUPER_MAGIC" },
	{ "nfs",        "NFS_SUPER_MAGIC" },
	{ "openprom",   "OPENPROM_SUPER_MAGIC" },
	{ "proc",       "PROC_SUPER_MAGIC" },
	{ "qnx4",       "QNX4_SUPER_MAGIC" },
	{ "reiserfs",   "REISERFS_SUPER_MAGIC" },
	{ "reiserfs",   "REISERFS_SUPER_MAGIC_STRING" },
	{ "reiser2fs",  "REISER2FS_SUPER_MAGIC_STRING" },
	{ "reiser2fs",  "REISER2FS_JR_SUPER_MAGIC_STRING" },
	{ "smbfs",      "SMB_SUPER_MAGIC" },
	{ "usbdevfs",   "USBDEVICE_SUPER_MAGIC" },
	{ "usbfs",      "USBDEVICE_SUPER_MAGIC" },
	{ "cgroup",     "CGROUP_SUPER_MAGIC" },
	{ "futexfs",    "FUTEXFS_SUPER_MAGIC" },
	{ "stack",      "STACK_END_MAGIC" },
	{ "devpts",     "DEVPTS_SUPER_MAGIC" },
	{ "sockfs",     "SOCKFS_MAGIC" }
};

static const char *correct_fstype(char *type)
{
	register size_t i;

	for (i = 0; i < sizeof __OVAL_fs_types/(sizeof(char *) * 2); ++i) {
		if (strcmp(__OVAL_fs_types[i][0], type) == 0)
			return __OVAL_fs_types[i][1];
	}

	return "";
}

static void add_mnt_opt(char ***mnt_opts, uint8_t mnt_ocnt, char *opt)
{
	*mnt_opts = realloc(*mnt_opts, sizeof(char *) * (mnt_ocnt + 1));
	(*mnt_opts)[mnt_ocnt - 1] = opt;
	(*mnt_opts)[mnt_ocnt] = NULL;
}

#if defined(HAVE_BLKID_GET_TAG_VALUE)
static int collect_item(probe_ctx *ctx, oval_schema_version_t over, struct mntent *mnt_ent, blkid_cache blkcache)
#else
static int collect_item(probe_ctx *ctx, oval_schema_version_t over, struct mntent *mnt_ent)
#endif
{
        SEXP_t *item;
        char   *uuid = "", *tok, *save = NULL, **mnt_opts = NULL;
        uint8_t mnt_ocnt;
        struct statvfs stvfs;

        /*
         * Get FS stats
         */
        if (statvfs(mnt_ent->mnt_dir, &stvfs) != 0)
                return (-1);

        /*
         * Get UUID
         */
#if defined(HAVE_BLKID_GET_TAG_VALUE)
        uuid = blkid_get_tag_value(blkcache, "UUID", mnt_ent->mnt_fsname);
        if (uuid == NULL) {
	        uuid = "";
        }
#endif
        /*
         * Create a NULL-terminated array from the mount options
         */
        mnt_ocnt = 0;

        tok = strtok_r(mnt_ent->mnt_opts, ",", &save);

        do {
            add_mnt_opt(&mnt_opts, ++mnt_ocnt, tok);
        } while ((tok = strtok_r(NULL, ",", &save)) != NULL);

        /*
         * Check for "remount", "bind" and "move" mount options
         * These options can't be found in /proc/mounts,
         * we must use flags got by statvfs().
         */
        if (stvfs.f_flag & MS_REMOUNT) {
            add_mnt_opt(&mnt_opts, ++mnt_ocnt, "remount");
        }
        if (stvfs.f_flag & MS_BIND) {
            add_mnt_opt(&mnt_opts, ++mnt_ocnt, "bind");
        }
        if (stvfs.f_flag & MS_MOVE) {
            add_mnt_opt(&mnt_opts, ++mnt_ocnt, "move");
        }

        dI("mnt_ocnt = %d, mnt_opts[mnt_ocnt]=%p\n", mnt_ocnt, mnt_opts[mnt_ocnt]);

	/*
	 * "Correct" the type (this won't be (hopefully) needed in a later version
	 * of OVAL)
	 */
        if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.10)) < 0)
	        mnt_ent->mnt_type = (char *)correct_fstype(mnt_ent->mnt_type);

        /*
         * Create the item
         */
        item = probe_item_create(OVAL_LINUX_PARTITION, NULL,
                                 "mount_point",   OVAL_DATATYPE_STRING,   mnt_ent->mnt_dir,
                                 "device",        OVAL_DATATYPE_STRING,   mnt_ent->mnt_fsname,
                                 "uuid",          OVAL_DATATYPE_STRING,   uuid,
                                 "fs_type",       OVAL_DATATYPE_STRING,   mnt_ent->mnt_type,
                                 "mount_options", OVAL_DATATYPE_STRING_M, mnt_opts,
                                 "total_space",   OVAL_DATATYPE_INTEGER, (int64_t)stvfs.f_blocks,
                                 "space_used",    OVAL_DATATYPE_INTEGER, (int64_t)(stvfs.f_blocks - stvfs.f_bfree),
                                 "space_left",    OVAL_DATATYPE_INTEGER, (int64_t)stvfs.f_bfree,
                                 NULL);

#if defined(HAVE_BLKID_GET_TAG_VALUE)
        /*
         * If the partition doesn't have an UUID assigned, set the uuid entity status to
         * "does not exist" which means that the value was collected but does not exist
         * on the system.
         */
        if (strcmp(uuid, "") == 0) {
	        probe_itement_setstatus(item, "uuid", 1, SYSCHAR_STATUS_DOES_NOT_EXIST);
        }
#else
	/* Compiled without blkid library, we don't collect UUID */
	probe_itement_setstatus(item, "uuid", 1, SYSCHAR_STATUS_NOT_COLLECTED);
#endif /* HAVE_BLKID_GET_TAG_VALUE */

        probe_item_collect(ctx, item);
        oscap_free(mnt_opts);

        return (0);
}

int probe_main(probe_ctx *ctx, void *probe_arg)
{
        int probe_ret = 0;
        SEXP_t *mnt_entity, *mnt_opval, *mnt_entval, *probe_in;
        char    mnt_path[PATH_MAX];
        oval_operation_t mnt_op;
        FILE *mnt_fp;
        oval_schema_version_t obj_over;
#if defined(PROC_CHECK) && defined(__linux__)
        int   mnt_fd;
        struct statfs stfs;

        mnt_fd = open(MTAB_PATH, O_RDONLY);

        if (mnt_fd < 0)
                return (PROBE_ESYSTEM);

        if (fstatfs(mnt_fd, &stfs) != 0) {
                close(mnt_fd);
                return (PROBE_ESYSTEM);
        }

        if (stfs.f_type != PROC_SUPER_MAGIC) {
                close(mnt_fd);
                return (PROBE_EFATAL);
        }

        mnt_fp = fdopen(mnt_fd, "r");

        if (mnt_fp == NULL) {
                close(mnt_fd);
                return (PROBE_ESYSTEM);
        }
#else
        mnt_fp = fopen(MTAB_PATH, "r");

        if (mnt_fp == NULL)
                return (PROBE_ESYSTEM);
#endif
        probe_in   = probe_ctx_getobject(ctx);
        obj_over   = probe_obj_get_platform_schema_version(probe_in);
        mnt_entity = probe_obj_getent(probe_in, "mount_point", 1);

        if (mnt_entity == NULL) {
                fclose(mnt_fp);
                return (PROBE_ENOENT);
        }

        mnt_opval = probe_ent_getattrval(mnt_entity, "operation");

        if (mnt_opval != NULL) {
                mnt_op = (oval_operation_t)SEXP_number_geti(mnt_opval);
                SEXP_free(mnt_opval);
        } else
                mnt_op = OVAL_OPERATION_EQUALS;

        mnt_entval = probe_ent_getval(mnt_entity);

        if (!SEXP_stringp(mnt_entval)) {
                SEXP_free(mnt_entval);
                SEXP_free(mnt_entity);
		fclose(mnt_fp);
                return (PROBE_EINVAL);
        }

        SEXP_string_cstr_r(mnt_entval, mnt_path, sizeof mnt_path);
        SEXP_free(mnt_entval);
        SEXP_free(mnt_entity);

        if (mnt_fp != NULL) {
                char buffer[MTAB_LINE_MAX];
                struct mntent mnt_ent, *mnt_entp;

                pcre *re = NULL;
                const char *estr = NULL;
                int eoff = -1;
#if defined(HAVE_BLKID_GET_TAG_VALUE)
                blkid_cache blkcache;

                if (blkid_get_cache(&blkcache, NULL) != 0) {
                        endmntent(mnt_fp);
                        return (PROBE_EUNKNOWN);
                }
#endif
                if (mnt_op == OVAL_OPERATION_PATTERN_MATCH) {
                        re = pcre_compile(mnt_path, PCRE_UTF8, &estr, &eoff, NULL);

                        if (re == NULL) {
                                endmntent(mnt_fp);
                                return (PROBE_EINVAL);
                        }
                }

                while ((mnt_entp = getmntent_r(mnt_fp, &mnt_ent,
                                               buffer, sizeof buffer)) != NULL)
                {
			if (strcmp(mnt_entp->mnt_type, "rootfs") == 0)
			    continue;

                        if (mnt_op == OVAL_OPERATION_EQUALS) {
                                if (strcmp(mnt_entp->mnt_dir, mnt_path) == 0) {
#if defined(HAVE_BLKID_GET_TAG_VALUE)
	                                collect_item(ctx, obj_over, mnt_entp, blkcache);
#else
	                                collect_item(ctx, obj_over, mnt_entp);
#endif
                                        break;
                                }
			} else if (mnt_op == OVAL_OPERATION_NOT_EQUAL) {
				if (strcmp(mnt_entp->mnt_dir, mnt_path) != 0) {
					if (
#if defined(HAVE_BLKID_GET_TAG_VALUE)
						collect_item(ctx, obj_over, mnt_entp, blkcache)
#else
						collect_item(ctx, obj_over, mnt_entp)
#endif
					!= 0)
						break;
                                }
                        } else if (mnt_op == OVAL_OPERATION_PATTERN_MATCH) {
                                int rc;

                                rc = pcre_exec(re, NULL, mnt_entp->mnt_dir,
                                               strlen(mnt_entp->mnt_dir), 0, 0, NULL, 0);

                                if (rc == 0) {
	                                if (
#if defined(HAVE_BLKID_GET_TAG_VALUE)
		                                collect_item(ctx, obj_over, mnt_entp, blkcache)
#else
		                                collect_item(ctx, obj_over, mnt_entp)
#endif
                                        != 0)
		                                break;
                                }
                                /* XXX: check for pcre_exec error */
                        }
                }

                endmntent(mnt_fp);

                if (mnt_op == OVAL_OPERATION_PATTERN_MATCH)
                        pcre_free(re);
        }

        return (probe_ret);
}
