
#ifndef FINDFILE_H
#define FINDFILE_H

#include "seap.h"
#include "probe.h"

#define MTAB_PATH "/etc/mtab"
#define LOCAL_FILESYSTEMS {"ext2", "ext3", "reiserfs", "xfs", NULL}

int find_files(SEXP_t * path, SEXP_t *filename, SEXP_t *behaviors,
               int (*cb) (const char *, const char *, void *arg), void *arg);
#endif

