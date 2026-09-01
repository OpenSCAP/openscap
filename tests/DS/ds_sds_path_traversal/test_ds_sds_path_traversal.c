/*
 * Copyright 2026 Red Hat, LLC, Raleigh, North Carolina.
 * All Rights Reserved.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ds_sds_session.h"
#include "oscap_source.h"
#include "scap_ds.h"
#include "DS/ds_common.h"

#define DATASTREAM_ID "scap_test_datastream"
#define XCCDF_ID "scap_test_cref_xccdf"

static int failures;

static void fail(const char *msg)
{
	fprintf(stderr, "FAIL: %s\n", msg);
	failures++;
}

static bool file_exists(const char *path)
{
	struct stat st;

	return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static bool file_contains(const char *path, const char *text)
{
	char buffer[256];
	FILE *file = fopen(path, "r");
	if (file == NULL)
		return false;
	size_t count = fread(buffer, 1, sizeof(buffer) - 1, file);
	buffer[count] = '\0';
	fclose(file);
	return strstr(buffer, text) != NULL;
}

static bool write_marker(const char *path)
{
	FILE *file = fopen(path, "w");
	if (file == NULL)
		return false;
	bool success = fputs("untouched", file) >= 0;
	if (fclose(file) != 0)
		success = false;
	return success;
}

static bool dir_is_empty(const char *path)
{
	DIR *dir = opendir(path);

	if (dir == NULL)
		return true;

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			closedir(dir);
			return false;
		}
	}

	closedir(dir);
	return true;
}

static char *path_join(const char *dir, const char *file)
{
	size_t dir_len = strlen(dir);
	size_t file_len = strlen(file);
	char *path = malloc(dir_len + file_len + 2);

	if (path == NULL)
		return NULL;

	memcpy(path, dir, dir_len);
	path[dir_len] = '/';
	memcpy(path + dir_len + 1, file, file_len + 1);
	return path;
}

static int setup_workdirs(char *root_template, char **outside_dir, char **target_dir)
{
	if (mkdtemp(root_template) == NULL) {
		perror("mkdtemp");
		return -1;
	}

	*outside_dir = path_join(root_template, "outside");
	*target_dir = path_join(root_template, "target");
	if (*outside_dir == NULL || *target_dir == NULL) {
		free(*outside_dir);
		free(*target_dir);
		return -1;
	}

	if (mkdir(*outside_dir, 0700) != 0 || mkdir(*target_dir, 0700) != 0) {
		perror("mkdir");
		free(*outside_dir);
		free(*target_dir);
		return -1;
	}

	return 0;
}

static void remove_tree(const char *path)
{
	struct stat st;
	if (lstat(path, &st) != 0)
		return;
	if (!S_ISDIR(st.st_mode)) {
		unlink(path);
		return;
	}
	DIR *dir = opendir(path);
	if (dir != NULL) {
		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL) {
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
				char *child = path_join(path, entry->d_name);
				if (child != NULL) {
					remove_tree(child);
					free(child);
				}
			}
		}
		closedir(dir);
	}
	rmdir(path);
}

static void cleanup_workdirs(const char *root, char *outside_dir, char *target_dir)
{
	free(outside_dir);
	free(target_dir);
	if (root != NULL)
		remove_tree(root);
}

static char *fixture_path(const char *srcdir, const char *name)
{
	size_t srcdir_len = strlen(srcdir);
	size_t name_len = strlen(name);
	char *path = malloc(srcdir_len + name_len + 2);

	if (path == NULL)
		return NULL;

	memcpy(path, srcdir, srcdir_len);
	path[srcdir_len] = '/';
	memcpy(path + srcdir_len + 1, name, name_len + 1);
	return path;
}

static int run_sds_dump(const char *fixture, const char *target_dir, const char *xccdf_id)
{
	const char *datastream_id = DATASTREAM_ID;
	struct oscap_source *source = oscap_source_new_from_file(fixture);
	struct ds_sds_session *session;
	int ret = 0;

	if (source == NULL)
		return -1;

	session = ds_sds_session_new_from_source(source);
	if (session == NULL) {
		oscap_source_free(source);
		return -1;
	}

	if (ds_sds_index_select_checklist(ds_sds_session_get_sds_idx(session),
			&datastream_id, &xccdf_id) != 0) {
		ret = -1;
		goto cleanup;
	}

	ds_sds_session_set_datastream_id(session, datastream_id);

	if (ds_sds_session_set_target_dir(session, target_dir) != 0) {
		ret = -1;
		goto cleanup;
	}

	if (ds_sds_session_register_component_with_dependencies(session, "checklists", xccdf_id, NULL) != 0) {
		ret = -1;
		goto cleanup;
	}

	if (ds_sds_session_dump_component_files(session) != 0)
		ret = -1;

cleanup:
	ds_sds_session_free(session);
	oscap_source_free(source);
	return ret;
}

static void test_path_is_null(void) {
	if (!ds_path_is_null(NULL))
		fail("NULL path must be unsafe");
	if (!ds_path_is_null(""))
		fail("empty path must be unsafe");
}

static void test_path_has_upward_component(void)
{
	if (!ds_path_has_upward_component("/"))
		fail("root path must be unsafe");
	if (!ds_path_has_upward_component("/abs/path"))
		fail("absolute path must be unsafe");
	if (!ds_path_has_upward_component(".."))
		fail("'..' must be unsafe");
	if (!ds_path_has_upward_component("foo/.."))
		fail("'foo/..' must be unsafe");
	if (!ds_path_has_upward_component("../../../pwned.xml"))
		fail("traversal path must be unsafe");
	if (!ds_path_has_upward_component("..\\outside\\pwned.xml"))
		fail("backslash traversal path must be unsafe");
	if (ds_path_is_valid_filename("a/../b") || ds_path_is_valid_filename(".") ||
			ds_path_is_valid_filename("a/"))
		fail("invalid output file names must be rejected");
	if (!ds_path_is_valid_filename("services/obsolete/foo.sh"))
		fail("nested output file names must be accepted");
	if (ds_path_has_upward_component("foo.xml"))
		fail("'foo.xml' must be safe");
	if (ds_path_has_upward_component("services/obsolete/foo.sh"))
		fail("nested relative path must be safe");
}

static void test_xml_catalog_traversal(const char *srcdir)
{
	char root_template[] = "/tmp/openscap_path_traversal_xml_XXXXXX";
	char *outside_dir = NULL;
	char *target_dir = NULL;
	char *fixture = fixture_path(srcdir, "traversal_catalog_name.ds.xml");
	char *outside_file = NULL;
	char *target_file = NULL;
	int ret;

	if (fixture == NULL) {
		fail("could not build fixture path");
		return;
	}

	if (setup_workdirs(root_template, &outside_dir, &target_dir) != 0) {
		free(fixture);
		return;
	}

	outside_file = path_join(outside_dir, "pwned.oval.xml");
	target_file = path_join(target_dir, "pwned.oval.xml");
	if (outside_file == NULL || target_file == NULL) {
		fail("could not build output paths");
		goto cleanup;
	}

	ret = run_sds_dump(fixture, target_dir, XCCDF_ID);
	if (ret == 0)
		fail("malicious XML catalog name must be rejected");

	if (!dir_is_empty(outside_dir))
		fail("XML traversal must not write outside the target directory");

	if (file_exists(target_file))
		fail("rejected XML component must not be dumped");

cleanup:
	free(outside_file);
	free(target_file);
	cleanup_workdirs(root_template, outside_dir, target_dir);
	free(fixture);
}

static void test_sce_catalog_traversal(const char *srcdir)
{
	char root_template[] = "/tmp/openscap_path_traversal_sce_XXXXXX";
	char *outside_dir = NULL;
	char *target_dir = NULL;
	char *fixture = fixture_path(srcdir, "traversal_sce_catalog.ds.xml");
	char *outside_file = NULL;
	char *target_file = NULL;
	int ret;

	if (fixture == NULL) {
		fail("could not build fixture path");
		return;
	}

	if (setup_workdirs(root_template, &outside_dir, &target_dir) != 0) {
		free(fixture);
		return;
	}

	outside_file = path_join(outside_dir, "evil.sh");
	target_file = path_join(target_dir, "evil.sh");
	if (outside_file == NULL || target_file == NULL) {
		fail("could not build output paths");
		goto cleanup;
	}

	ret = run_sds_dump(fixture, target_dir, XCCDF_ID);
	if (ret == 0)
		fail("malicious SCE catalog name must be rejected");

	if (!dir_is_empty(outside_dir))
		fail("SCE traversal must not write outside the target directory");

	if (file_exists(target_file))
		fail("rejected SCE script must not be dumped");

cleanup:
	free(outside_file);
	free(target_file);
	cleanup_workdirs(root_template, outside_dir, target_dir);
	free(fixture);
}

static void test_sce_dotdot_rejected(const char *srcdir)
{
	char root_template[] = "/tmp/openscap_path_traversal_dotdot_XXXXXX";
	char *outside_dir = NULL;
	char *target_dir = NULL;
	char *fixture = fixture_path(srcdir, "traversal_sce_dotdot.ds.xml");
	int ret;

	if (fixture == NULL) {
		fail("could not build fixture path");
		return;
	}

	if (setup_workdirs(root_template, &outside_dir, &target_dir) != 0) {
		free(fixture);
		return;
	}

	ret = run_sds_dump(fixture, target_dir, XCCDF_ID);
	if (ret == 0)
		fail("SCE catalog name '..' must be rejected");

	if (!dir_is_empty(outside_dir))
		fail("rejected SCE path must not write outside the target directory");

	cleanup_workdirs(root_template, outside_dir, target_dir);
	free(fixture);
}

static void test_legitimate_subdir_preserved(const char *srcdir)
{
	char root_template[] = "/tmp/openscap_path_traversal_subdir_XXXXXX";
	char *outside_dir = NULL;
	char *target_dir = NULL;
	char *fixture = fixture_path(srcdir, "legitimate_subdir_sce.ds.xml");
	char *expected = NULL;
	int ret;

	if (fixture == NULL) {
		fail("could not build fixture path");
		return;
	}

	if (setup_workdirs(root_template, &outside_dir, &target_dir) != 0) {
		free(fixture);
		return;
	}

	expected = path_join(target_dir, "services/obsolete/disable_nfs_exports.sh");
	if (expected == NULL) {
		fail("could not build expected SCE path");
		goto cleanup;
	}

	ret = run_sds_dump(fixture, target_dir, XCCDF_ID);
	if (ret != 0)
		fail("legitimate nested SCE path should dump successfully");

	if (!file_exists(expected))
		fail("legitimate nested SCE path must preserve subdirectory layout");
	struct stat st;
	if (stat(expected, &st) != 0 || (st.st_mode & 0777) != 0700)
		fail("dumped SCE script must be executable only by its owner");

cleanup:
	free(expected);
	cleanup_workdirs(root_template, outside_dir, target_dir);
	free(fixture);
}

static void test_component_id_traversal(const char *srcdir)
{
	char root_template[] = "/tmp/openscap_path_traversal_id_XXXXXX";
	char *outside_dir = NULL, *target_dir = NULL;
	char *fixture = fixture_path(srcdir, "traversal_component_id.ds.xml");
	if (fixture == NULL || setup_workdirs(root_template, &outside_dir, &target_dir) != 0) {
		fail("could not set up component id test");
		free(fixture);
		return;
	}
	if (run_sds_dump(fixture, target_dir, "../outside/pwned.xml") == 0)
		fail("traversing component-ref id must be rejected");
	if (!dir_is_empty(outside_dir))
		fail("component-ref id traversal wrote outside target");
	cleanup_workdirs(root_template, outside_dir, target_dir);
	free(fixture);
}

static void test_nested_xml(const char *srcdir)
{
	char root_template[] = "/tmp/openscap_path_traversal_nested_XXXXXX";
	char *outside_dir = NULL, *target_dir = NULL;
	char *fixture = fixture_path(srcdir, "nested_xml.ds.xml");
	if (fixture == NULL || setup_workdirs(root_template, &outside_dir, &target_dir) != 0) {
		fail("could not set up nested XML test");
		free(fixture);
		return;
	}
	char *one = path_join(target_dir, "one/shared.xml");
	char *two = path_join(target_dir, "two/shared.xml");
	if (run_sds_dump(fixture, target_dir, XCCDF_ID) != 0)
		fail("safe nested XML names must be accepted");
	if (!file_contains(one, "id=\"one\"") || !file_contains(two, "id=\"two\""))
		fail("distinct nested XML components must retain their directories and content");
	free(one);
	free(two);
	cleanup_workdirs(root_template, outside_dir, target_dir);
	free(fixture);
}

static void test_xml_destination_symlink(const char *srcdir)
{
	char root_template[] = "/tmp/openscap_path_traversal_link_XXXXXX";
	char *outside_dir = NULL, *target_dir = NULL;
	char *fixture = fixture_path(srcdir, "nested_xml.ds.xml");
	if (fixture == NULL || setup_workdirs(root_template, &outside_dir, &target_dir) != 0) {
		fail("could not set up XML symlink test");
		free(fixture);
		return;
	}
	char *victim = path_join(outside_dir, "victim.xml");
	char *link = path_join(target_dir, XCCDF_ID);
	if (!write_marker(victim) || symlink(victim, link) != 0) {
		fail("could not create XML destination symlink");
	} else {
		struct stat st;
		if (run_sds_dump(fixture, target_dir, XCCDF_ID) != 0)
			fail("safe XML output should replace a destination symlink");
		if (!file_contains(victim, "untouched") || lstat(link, &st) != 0 || !S_ISREG(st.st_mode))
			fail("XML output followed destination symlink");
	}
	free(victim);
	free(link);
	cleanup_workdirs(root_template, outside_dir, target_dir);
	free(fixture);
}

static void test_xml_destination_hardlink(const char *srcdir)
{
	char root_template[] = "/tmp/openscap_path_traversal_hardlink_XXXXXX";
	char *outside_dir = NULL, *target_dir = NULL;
	char *fixture = fixture_path(srcdir, "nested_xml.ds.xml");
	if (fixture == NULL || setup_workdirs(root_template, &outside_dir, &target_dir) != 0) {
		fail("could not set up XML hardlink test");
		free(fixture);
		return;
	}
	char *victim = path_join(outside_dir, "victim.xml");
	char *destination = path_join(target_dir, XCCDF_ID);
	if (!write_marker(victim) || destination == NULL ||
			link(victim, destination) != 0) {
		fail("could not create XML destination hardlink");
	} else {
		if (run_sds_dump(fixture, target_dir, XCCDF_ID) != 0)
			fail("safe XML output should replace a destination hardlink");
		if (!file_contains(victim, "untouched") || !file_contains(destination, "Benchmark"))
			fail("XML output overwrote the hardlink target");
	}
	free(victim);
	free(destination);
	cleanup_workdirs(root_template, outside_dir, target_dir);
	free(fixture);
}

static void test_sce_parent_symlink(const char *srcdir)
{
	char root_template[] = "/tmp/openscap_path_traversal_sce_link_XXXXXX";
	char *outside_dir = NULL, *target_dir = NULL;
	char *fixture = fixture_path(srcdir, "legitimate_subdir_sce.ds.xml");
	if (fixture == NULL || setup_workdirs(root_template, &outside_dir, &target_dir) != 0) {
		fail("could not set up SCE symlink test");
		free(fixture);
		return;
	}
	char *link = path_join(target_dir, "services");
	if (symlink(outside_dir, link) != 0)
		fail("could not create SCE parent symlink");
	else {
		if (run_sds_dump(fixture, target_dir, XCCDF_ID) == 0)
			fail("SCE parent symlink must be rejected");
		if (!dir_is_empty(outside_dir))
			fail("SCE output followed parent symlink");
	}
	free(link);
	cleanup_workdirs(root_template, outside_dir, target_dir);
	free(fixture);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Invalid arguments, usage: %s FIXTURE_DIR\n", argv[0]);
		return 2;
	}

	test_path_has_upward_component();
	test_path_is_null();
	test_xml_catalog_traversal(argv[1]);
	test_sce_catalog_traversal(argv[1]);
	test_sce_dotdot_rejected(argv[1]);
	test_legitimate_subdir_preserved(argv[1]);
	test_component_id_traversal(argv[1]);
	test_nested_xml(argv[1]);
	test_xml_destination_symlink(argv[1]);
	test_xml_destination_hardlink(argv[1]);
	test_sce_parent_symlink(argv[1]);

	if (failures != 0) {
		fprintf(stderr, "%d test(s) failed\n", failures);
		return 1;
	}

	return 0;
}
