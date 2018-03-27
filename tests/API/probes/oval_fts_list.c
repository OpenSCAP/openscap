
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include "sexp.h"
#include "oval_fts.h"
#include "probe-api.h"

int main(int argc, char *argv[])
{
	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

	SEXP_t *path, *filename, *behaviors, *filepath, *result;

	SEXP_psetup_t *psetup = NULL;
	SEXP_pstate_t *pstate = NULL;

	psetup = SEXP_psetup_new();

	path      = SEXP_list_first(SEXP_parse(psetup, argv[1], strlen(argv[1]), &pstate));
	filename  = SEXP_list_first(SEXP_parse(psetup, argv[2], strlen(argv[2]), &pstate));
	filepath  = SEXP_list_first(SEXP_parse(psetup, argv[3], strlen(argv[3]), &pstate));
	behaviors = SEXP_list_first(SEXP_parse(psetup, argv[4], strlen(argv[4]), &pstate));
	result    = probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, NULL, NULL, NULL);

	fprintf(stderr,
		"path=%p\n"
		"filename=%p\n"
		"filepath=%p\n"
		"behaviors=%p\n", path, filename, filepath, behaviors);

	ofts = oval_fts_open_prefixed(NULL, path, filename, filepath, behaviors, result);

	if (ofts != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			printf("%s/%s\n", ofts_ent->path, ofts_ent->file ? ofts_ent->file : "");
			oval_ftsent_free(ofts_ent);
		}

		oval_fts_close(ofts);
	}

	SEXP_free(path);
	SEXP_free(filename);
	SEXP_free(filepath);
	SEXP_free(behaviors);
	SEXP_psetup_free(psetup);

	return 0;
}
