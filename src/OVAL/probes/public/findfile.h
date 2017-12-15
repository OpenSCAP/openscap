/**
 * @file   findfile.h
 * @author "Peter Vrabec" <pvrabec@redhat.com>
 *
 * @addtogroup PROBEAUXAPI
 * @{
 */
/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      Peter Vrabec <pvrabec@redhat.com>
 */

#ifndef FINDFILE_H
#define FINDFILE_H

#include "seap.h"
#include "probe-api.h"
#include "fsdev.h"
#include "oscap_export.h"

#define MTAB_PATH "/etc/mtab"
#define LOCAL_FILESYSTEMS { "btrfs",            \
                        "ext2", "ext3", "ext4", \
                        "ffs",                  \
                        "gfs",                  \
                        "hpfs",                 \
                        "jfs",                  \
                        "reiser4", "reiserfs",  \
                        "ufs",                  \
                        "xfs",                  \
                        "zfs", NULL}

/**
 * Search the filesystem according to the specified path, filename and behaviors.
 * For all paths and files that satisfy these requirements call the provided callback function.
 * @param path the requested path in a sexp form, which may specify the matching operation or a var_ref
 * @param filenam the requested filename in a sexp form, which may specify the matching operation or a var_ref
 * @param behaviors the behaviors may specify the recursion depth or direction
 * @param cb for each match, the callback function cb is called with first argument being the matched path and the second being the matched filename
 * @param arg an optional argument to the callback function
 */
int find_files(SEXP_t *spath, SEXP_t *sfilename, SEXP_t *behaviors,
OSCAP_API 	       int (*cb) (const char *, const char *, void *), void *arg);

#endif
/// @}
