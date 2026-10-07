#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Exercise session-owned temporary directories before the sessions free them. */
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "common/public/oscap.h"
#include "DS/public/ds_sds_session.h"
#include "source/public/oscap_source.h"
#include "XCCDF/public/xccdf_session.h"

static int check_export(const char *tmpdir, oscap_document_type_t type)
{
	size_t pattern_size = strlen(tmpdir) + sizeof("/oscap.*/*");
	char *pattern = malloc(pattern_size);
	if (pattern == NULL)
		return 1;
	snprintf(pattern, pattern_size, "%s/oscap.*/*", tmpdir);
	glob_t paths = {0};
	int ret = 1;
	if (glob(pattern, 0, NULL, &paths) == 0 && paths.gl_pathc == 1) {
		struct stat st;
		if (stat(paths.gl_pathv[0], &st) == 0 && S_ISREG(st.st_mode) && st.st_size > 0) {
			struct oscap_source *source = oscap_source_new_from_file(paths.gl_pathv[0]);
			if (source != NULL && oscap_source_get_scap_type(source) == type)
				ret = 0;
			oscap_source_free(source);
		}
	}
	if (ret != 0)
		fprintf(stderr, "Expected one exported document of type %d under %s\n", type, pattern);
	globfree(&paths);
	free(pattern);
	return ret;
}

static int test_oval(const char *filename, const char *tmpdir)
{
	struct xccdf_session *session = xccdf_session_new(filename);
	if (session == NULL)
		return 1;
	int ret = 1;
	xccdf_session_set_oval_results_export(session, false);
	if (xccdf_session_load(session) == 0 &&
	    xccdf_session_evaluate(session) == 0 &&
	    xccdf_session_export_oval(session) == 0)
		ret = check_export(tmpdir, OSCAP_DOCUMENT_OVAL_RESULTS);
	xccdf_session_free(session);
	return ret;
}

static int test_datastream(const char *filename, const char *tmpdir)
{
	struct oscap_source *source = oscap_source_new_from_file(filename);
	if (source == NULL)
		return 1;
	struct ds_sds_session *session = ds_sds_session_new_from_source(source);
	int ret = 1;
	/* No explicit target: dumping components must use a directory in TMPDIR. */
	if (session != NULL && ds_sds_session_select_checklist(session, NULL, NULL, NULL) != NULL &&
	    ds_sds_session_dump_component_files(session) == 0)
		ret = check_export(tmpdir, OSCAP_DOCUMENT_XCCDF);
	ds_sds_session_free(session);
	oscap_source_free(source);
	return ret;
}

int main(int argc, char **argv)
{
	const char *tmpdir = getenv("TMPDIR");
	if (argc != 3 || tmpdir == NULL || tmpdir[0] == '\0')
		return 1;
	if (chmod(tmpdir, 0700) != 0)
		return 1;
	oscap_init();
	int ret = 1;
	if (strcmp(argv[1], "oval") == 0)
		ret = test_oval(argv[2], tmpdir);
	else if (strcmp(argv[1], "datastream") == 0)
		ret = test_datastream(argv[2], tmpdir);
	oscap_cleanup();
	return ret;
}
