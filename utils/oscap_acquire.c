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

	printf("Downloading: %s ... ", url);
	fflush(stdout);
	output_filename = malloc(PATH_MAX * sizeof(char));
	snprintf(output_filename, PATH_MAX, "%s/%s", temp_dir, TEMP_URL_TEMPLATE);
	output_fd = mkstemp(output_filename);
	if (output_fd == -1) {
		printf("error\n");
		fprintf(stderr, "%s", strerror(errno));
		free(output_filename);
		return NULL;
	}

	fp = fdopen(output_fd, "w");
	if (fp == NULL) {
		printf("error\n");
		fprintf(stderr, "%s", strerror(errno));
		remove(output_filename);
		close(output_fd);
		free(output_filename);
		return NULL;
	}

	curl = curl_easy_init();
	if (curl == NULL) {
		printf("error\n");
		fprintf(stderr, "Failed to initialize libcurl.");
		remove(output_filename);
		fclose(fp);
		free(output_filename);
		return NULL;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);
	if (res != 0) {
		printf("error\n");
		fprintf(stderr, "%s", curl_easy_strerror(res));
		remove(output_filename);
		free(output_filename);
		output_filename = NULL;
	}
	else
		printf("0k\n");
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
