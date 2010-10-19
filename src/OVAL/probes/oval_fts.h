#ifndef OVAL_FTS_H
#define OVAL_FTS_H

#include <sexp.h>
#include <fts.h>
#include <pcre.h>

typedef struct {
	FTS    *ofts_fts;

	SEXP_t  *ofts_path;
	char   **ofts_st_path;       /**< starting paths - NULL terminated */
	uint16_t ofts_st_path_count; /**< number of starting paths */
	uint16_t ofts_st_path_index; /**< current starting path */

	pcre       *ofts_path_regex;
	pcre_extra *ofts_path_regex_extra;

	SEXP_t *ofts_filename;
	SEXP_t *ofts_behaviors;

	bool ofts_nilfilename;

	int max_depth;
	int direction;
	int recurse;
	int filesystem;
} OVAL_FTS;

#define OVAL_RECURSE_DIRECTION_NONE 0 /* default */
#define OVAL_RECURSE_DIRECTION_DOWN 1
#define OVAL_RECURSE_DIRECTION_UP   2

#define OVAL_RECURSE_FILES    0x01
#define OVAL_RECURSE_DIRS     0x02
#define OVAL_RECURSE_SYMLINKS 0x04

#define OVAL_RECURSE_SYMLINKS_AND_DIRS (OVAL_RECURSE_SYMLINKS|OVAL_RECURSE_DIRS) /* default */
#define OVAL_RECURSE_FILES_AND_DIRS    (OVAL_RECURSE_FILES|OVAL_RECURSE_SYMLINKS)

#define OVAL_RECURSE_FS_LOCAL   0
#define OVAL_RECURSE_FS_DEFINED 1
#define OVAL_RECURSE_FS_ALL     2 /* default */

typedef struct {
	char  *filepath; /* path + file */
	size_t filepath_len;

	char  *file;     /* file - shares memory with filepath! */
	size_t file_len;

	char  *path;     /* path - separate memory */
	size_t path_len;

	bool  nilfile;
} OVAL_FTSENT;

/*
 * OVAL FTS public API
 */
OVAL_FTS    *oval_fts_open(SEXP_t *path, SEXP_t *filename, SEXP_t *filepath, SEXP_t *behaviors);
OVAL_FTSENT *oval_fts_read(OVAL_FTS *ofts);
int          oval_fts_close(OVAL_FTS *ofts);

void oval_ftsent_free(OVAL_FTSENT *ofts_ent);

#endif /* OVAL_FTS_H */
