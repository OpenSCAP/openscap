/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
 */

#include <stdio.h> // for P_tmpdir macro
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>

#include <curl/curl.h>
#include <curl/easy.h>

#include <ftw.h>

#include "oscap_acquire.h"

#ifndef P_tmpdir
#define P_tmpdir "/tmp"
#endif

#define TEMP_DIR_TEMPLATE P_tmpdir "/oscap.XXXXXX"
#define TEMP_URL_TEMPLATE "downloaded.XXXXXX"

char *
oscap_acquire_temp_dir()
{
	char *temp_dir = strdup(TEMP_DIR_TEMPLATE);
	if (mkdtemp(temp_dir) == NULL) {
		free(temp_dir);
		fprintf(stderr, "Could not create temp directory " TEMP_DIR_TEMPLATE ". %s\n", strerror(errno));
		return NULL;
	}
	return temp_dir;
}

static int
__unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	int rv = remove(fpath);

	if (rv)
		perror(fpath);

	return rv;
}

void
oscap_acquire_cleanup_dir(char **dir_path)
{
	if (*dir_path != NULL)
	{
		nftw(*dir_path, __unlink_cb, 64, FTW_DEPTH | FTW_PHYS | FTW_MOUNT);
		free(*dir_path);
		*dir_path = NULL;
	}
}

char *
oscap_acquire_url_download(const char *temp_dir, const char *url)
{
	/* SADLY, we create a tempfile which we use later.
	 * Much greater solution would be to use unliked
	 * file descriptors, but the library interface is
	 * not yet prepared for that. */
	char *output_filename = NULL;
	int output_fd;
	FILE *fp;
	CURL *curl;
	CURLcode res;
	mode_t old_mode;

	printf("Downloading: %s ... ", url);
	fflush(stdout);
	output_filename = malloc(PATH_MAX * sizeof(char));
	snprintf(output_filename, PATH_MAX, "%s/%s", temp_dir, TEMP_URL_TEMPLATE);

	old_mode = umask(077);  /* Override unusual umask. Ensure 0700 permissions. */
	output_fd = mkstemp(output_filename);
	(void) umask(old_mode);
	if (output_fd == -1) {
		printf("error\n");
		fprintf(stderr, "%s\n", strerror(errno));
		free(output_filename);
		return NULL;
	}

	fp = fdopen(output_fd, "w");
	if (fp == NULL) {
		printf("error\n");
		fprintf(stderr, "%s\n", strerror(errno));
		if (remove(output_filename))
			fprintf(stderr, "Failed to remove temp file %s. %s\n", output_filename, strerror(errno));
		close(output_fd);
		free(output_filename);
		return NULL;
	}

	curl = curl_easy_init();
	if (curl == NULL) {
		printf("error\n");
		fprintf(stderr, "Failed to initialize libcurl.\n");
		if (remove(output_filename))
			fprintf(stderr, "Failed to remove temp file %s. %s\n", output_filename, strerror(errno));
		fclose(fp);
		free(output_filename);
		return NULL;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);
	if (res != 0) {
		printf("error\n");
		fprintf(stderr, "%s\n", curl_easy_strerror(res));
		if (remove(output_filename))
			fprintf(stderr, "Failed to remove temp file %s. %s\n", output_filename, strerror(errno));
		free(output_filename);
		output_filename = NULL;
	}
	else
		printf("Ok\n");
	curl_easy_cleanup(curl);
	fclose(fp);
	return output_filename;
}

bool
oscap_acquire_url_is_supported(const char *url)
{
	return !strncmp(url, "http://", strlen("http://"));
}

char *
oscap_acquire_url_to_filename(const char *url)
{
	/* RFC 3986: 2.1. Percent-Encoding */
	char *curl_filename = NULL;
	char *filename = NULL;
	CURL *curl;

	curl = curl_easy_init();
	if (curl == NULL) {
		fprintf(stderr, "Failed to initialize libcurl.\n");
		return NULL;
	}

	curl_filename = curl_easy_escape(curl , url , 0);
	if (curl_filename == NULL) {
		fprintf(stderr, "Failed to escape the given url %s\n", url);
		return NULL;
	}
	filename = strdup(curl_filename);
	curl_free(curl_filename);
	curl_easy_cleanup(curl);
	return filename;
}
