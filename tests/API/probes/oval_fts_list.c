
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sexp.h"
#include "oval_fts.h"
#include "probe-api.h"

static int create_path_sexpr(char *arg_operation, char *arg_argument, SEXP_t **result)
{
	*result = NULL;

	/* non-fatal error */
	if (strlen(arg_operation) == 0 && strlen(arg_argument) == 0) {
		return 0;
	}

	errno = 0;
	int operation_number = atoi(arg_operation);
	if (errno != 0) {
		fprintf(stderr, "Error parsing path->operation number: %s\n", arg_operation);
		return errno;
	} else if (operation_number < 0 || operation_number > 255) {
		fprintf(stderr, "Error parsing path->operation number -- out of bounds (0 <= operation number <= 255): %s\n", arg_operation);
		return 1;
	}

	uint8_t operation_value = (uint8_t)(operation_number & 0xFF);

	SEXP_t *s_path_str = SEXP_string_new("path", 4);
	SEXP_t *s_operation_str = SEXP_string_new(":operation", 10);
	SEXP_t *s_operation_number = SEXP_number_new(SEXP_NUM_UINT8, &operation_value);
	SEXP_t *s_path_argument = SEXP_string_new(arg_argument, strlen(arg_argument));
	SEXP_t *s_inner_list = SEXP_list_new(NULL);

	SEXP_list_add(s_inner_list, s_path_str);
	SEXP_list_add(s_inner_list, s_operation_str);
	SEXP_list_add(s_inner_list, s_operation_number);

	*result = SEXP_list_new(NULL);
	SEXP_list_add(*result, s_inner_list);
	SEXP_list_add(*result, s_path_argument);

	return 0;
}

static int create_filename_sexpr(char *arg_operation, char *arg_argument, SEXP_t **result)
{
	*result = NULL;

	/* non-fatal error */
	if (strlen(arg_operation) == 0 && strlen(arg_argument) == 0) {
		return 0;
	}

	errno = 0;
	int operation_number = atoi(arg_operation);
	if (errno != 0) {
		fprintf(stderr, "Error parsing filename->operation number: %s\n", arg_operation);
		return errno;
	} else if (operation_number < 0 || operation_number > 255) {
		fprintf(stderr, "Error parsing filename->operation number -- out of bounds (0 <= operation number <= 255): %s\n", arg_operation);
		return 1;
	}

	uint8_t operation_value = (uint8_t)(operation_number & 0xFF);

	SEXP_t *s_filename_str = SEXP_string_new("filename", 8);
	SEXP_t *s_operation_str = SEXP_string_new(":operation", 10);
	SEXP_t *s_operation_number = SEXP_number_new(SEXP_NUM_UINT8, &operation_value);
	SEXP_t *s_filename_argument = NULL;
	if (strcmp(arg_argument, "EMPTY_STRING") == 0) {
		s_filename_argument = SEXP_string_new("", 0);
	} else {
		s_filename_argument = SEXP_string_new(arg_argument, strlen(arg_argument));
	}

	SEXP_t *s_inner_list = SEXP_list_new(NULL);
	SEXP_list_add(s_inner_list, s_filename_str);
	SEXP_list_add(s_inner_list, s_operation_str);
	SEXP_list_add(s_inner_list, s_operation_number);

	*result = SEXP_list_new(NULL);
	SEXP_list_add(*result, s_inner_list);

	if (strlen(arg_argument) != 0) {
		SEXP_list_add(*result, s_filename_argument);
	}

	return 0;
}

static int create_filepath_sexpr(char *arg_operation, char *arg_argument, SEXP_t **result)
{
	*result = NULL;

	/* non-fatal error */
	if (strlen(arg_operation) == 0 && strlen(arg_argument) == 0) {
		return 0;
	}

	errno = 0;
	int operation_number = atoi(arg_operation);
	if (errno != 0) {
		fprintf(stderr, "Error parsing filepath->operation number: %s - %d\n", arg_operation, errno);
		return errno;
	} else if (operation_number < 0 || operation_number > 255) {
		fprintf(stderr, "Error parsing filepath->operation number -- out of bounds (0 <= operation number <= 255): %s\n", arg_operation);
		return 1;
	}

	uint8_t operation_value = (uint8_t)(operation_number & 0xFF);

	SEXP_t *s_filepath_str = SEXP_string_new("filepath", 8);
	SEXP_t *s_operation_str = SEXP_string_new(":operation", 10);
	SEXP_t *s_operation_number = SEXP_number_new(SEXP_NUM_UINT8, &operation_value);
	SEXP_t *s_filepath_argument = SEXP_string_new(arg_argument, strlen(arg_argument));
	SEXP_t *s_inner_list = SEXP_list_new(NULL);

	SEXP_list_add(s_inner_list, s_filepath_str);
	SEXP_list_add(s_inner_list, s_operation_str);
	SEXP_list_add(s_inner_list, s_operation_number);

	*result = SEXP_list_new(NULL);
	SEXP_list_add(*result, s_inner_list);
	SEXP_list_add(*result, s_filepath_argument);

	return 0;
}

static int create_behaviors_sexpr(char *arg_max_depth, char *arg_recurse, char *arg_recurse_direction, char *arg_recurse_file_system, SEXP_t **result)
{
	*result = NULL;

	/* non-fatal error */
	if (strlen(arg_max_depth) == 0 && strlen(arg_recurse) == 0 && strlen(arg_recurse_direction) == 0 && strlen(arg_recurse_file_system) == 0) {
		return 0;
	}

	SEXP_t *s_behaviors_str = SEXP_string_new("behaviors", 9);
	SEXP_t *s_max_depth_str = SEXP_string_new(":max_depth", 10);
	SEXP_t *s_max_depth_argument = SEXP_string_new(arg_max_depth, strlen(arg_max_depth));
	SEXP_t *s_recurse_str = SEXP_string_new(":recurse", 8);
	SEXP_t *s_recurse_argument = SEXP_string_new(arg_recurse, strlen(arg_recurse));
	SEXP_t *s_recurse_direction_str = SEXP_string_new(":recurse_direction", 18);
	SEXP_t *s_recurse_direction_argument = SEXP_string_new(arg_recurse_direction, strlen(arg_recurse_direction));
	SEXP_t *s_recurse_file_system_str = SEXP_string_new(":recurse_file_system", 20);
	SEXP_t *s_recurse_file_system_argument = SEXP_string_new(arg_recurse_file_system, strlen(arg_recurse_file_system));

	SEXP_t *s_inner_list = SEXP_list_new(NULL);

	SEXP_list_add(s_inner_list, s_behaviors_str);
	SEXP_list_add(s_inner_list, s_max_depth_str);
	SEXP_list_add(s_inner_list, s_max_depth_argument);
	SEXP_list_add(s_inner_list, s_recurse_str);
	SEXP_list_add(s_inner_list, s_recurse_argument);
	SEXP_list_add(s_inner_list, s_recurse_direction_str);
	SEXP_list_add(s_inner_list, s_recurse_direction_argument);
	SEXP_list_add(s_inner_list, s_recurse_file_system_str);
	SEXP_list_add(s_inner_list, s_recurse_file_system_argument);

	*result = SEXP_list_new(NULL);
	SEXP_list_add(*result, s_inner_list);

	return 0;
}

int main(int argc, char *argv[])
{
	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

	SEXP_t *path, *filename, *behaviors, *filepath, *result;

	int ret = 0;

	if (argc < 11) {
		fprintf(stderr, "Invalid usage -- too few arguments supplied.\n");
		fprintf(stderr, "The following arguments are required, but may be empty:\n\n");
		fprintf(stderr, "    argv[1]  - path operation\n");
		fprintf(stderr, "    argv[2]  - path argument (regex or paths)\n");
		fprintf(stderr, "    argv[3]  - filename operation\n");
		fprintf(stderr, "    argv[4]  - filename argument (regex or paths)\n");
		fprintf(stderr, "    argv[5]  - filepath operation\n");
		fprintf(stderr, "    argv[6]  - filepath argument (regex or paths)\n");
		fprintf(stderr, "    argv[7]  - behaviors max_depth\n");
		fprintf(stderr, "    argv[8]  - behaviors recurse\n");
		fprintf(stderr, "    argv[9]  - behaviors recurse_direction\n");
		fprintf(stderr, "    argv[10] - behaviors recurse_file_system\n");
		return 1;
	}

	ret = create_path_sexpr(argv[1], argv[2], &path);
	if (ret)
		return ret;

	ret = create_filename_sexpr(argv[3], argv[4], &filename);
	if (ret)
		return ret;

	ret = create_filepath_sexpr(argv[5], argv[6], &filepath);
	if (ret)
		return ret;

	ret = create_behaviors_sexpr(argv[7], argv[8], argv[9], argv[10], &behaviors);
	if (ret)
		return ret;


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

	return 0;
}
