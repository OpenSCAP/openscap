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
#include <libgen.h>

#include <ftw.h>

#include "oscap_acquire.h"
#include "common/_error.h"

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
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Could not create temp directory " TEMP_DIR_TEMPLATE ". %s", strerror(errno));
		return NULL;
	}
	return temp_dir;
}

static int
__unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	int rv = remove(fpath);

	if (rv)
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Could not remove %s. %s", fpath, strerror(errno));

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

int
oscap_acquire_temp_file(const char *dir, const char *template, char **filename)
{
	mode_t old_mode;
	int fd;

	if (dir == NULL || template == NULL || filename == NULL)
		return -1;

	*filename = malloc(PATH_MAX * sizeof(char));
	snprintf(*filename, PATH_MAX, "%s/%s", dir, template);

	old_mode = umask(077); /* Override unusual umask. Ensure 0700 permissions. */
	fd = mkstemp(*filename);
	(void) umask(old_mode);
	if (fd < 1) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "mkstemp for %s failed: %s", *filename, strerror(errno));
		free(*filename);
		*filename = NULL;
	}
	return fd;
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

	output_fd = oscap_acquire_temp_file(temp_dir, TEMP_URL_TEMPLATE, &output_filename);
	if (output_fd == -1) {
		return NULL;
	}

	fp = fdopen(output_fd, "w");
	if (fp == NULL) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "fdopen failed, %s", strerror(errno));
		if (remove(output_filename))
			oscap_seterr(OSCAP_EFAMILY_GLIBC, "fdopen failed. Failed to remove temp file %s. %s",
				output_filename, strerror(errno));
		close(output_fd);
		free(output_filename);
		return NULL;
	}

	curl = curl_easy_init();
	if (curl == NULL) {
		oscap_seterr(OSCAP_EFAMILY_NET, "Failed to initialize libcurl.");

		if (remove(output_filename))
			oscap_seterr(OSCAP_EFAMILY_GLIBC, "Failed to initialize libcurl. Failed to remove temp file %s. %s",
				output_filename, strerror(errno));
		fclose(fp);
		free(output_filename);
		return NULL;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);
	if (res != 0) {
		oscap_seterr(OSCAP_EFAMILY_NET, "Download failed: %s", curl_easy_strerror(res));
		if (remove(output_filename))
			oscap_seterr(OSCAP_EFAMILY_GLIBC, "Download failed: %s. Failed to remove temp file %s. %s",
				curl_easy_strerror(res), output_filename, strerror(errno));
		free(output_filename);
		output_filename = NULL;
	}
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
		oscap_seterr(OSCAP_EFAMILY_NET, "Failed to initialize libcurl.");
		return NULL;
	}

	curl_filename = curl_easy_escape(curl , url , 0);
	if (curl_filename == NULL) {
		oscap_seterr(OSCAP_EFAMILY_NET, "Failed to escape the given url %s", url);
		return NULL;
	}
	filename = strdup(curl_filename);
	curl_free(curl_filename);
	curl_easy_cleanup(curl);
	return filename;
}

char *
oscap_acquire_pipe_to_string(int fd)
{
	char* pipe_buffer = malloc(sizeof(char) * 128);
	size_t pipe_buffer_size = 128;
	size_t read_bytes = 0;

	char readbuf;
	// FIXME: Read by larger chunks in the future
	while (read(fd, &readbuf, 1) > 0) {
		// & is a special case, we have to "escape" it manually
		// (all else will eventually get handled by libxml)
		if (readbuf == '&')
			read_bytes += 5; // we have to write "&amp;" instead of just &
		else
			read_bytes += 1;

		// + 1 because we want to add \0 at the end
		if (read_bytes + 1 > pipe_buffer_size) {
			// we simply double the buffer when we blow it
			pipe_buffer_size *= 2;
			pipe_buffer = realloc(pipe_buffer, sizeof(char) * pipe_buffer_size);
		}

		// write the escaped "&amp;" to the stdout buffer
		if (readbuf == '&') {
			pipe_buffer[read_bytes - 5] = '&';
			pipe_buffer[read_bytes - 4] = 'a';
			pipe_buffer[read_bytes - 3] = 'm';
			pipe_buffer[read_bytes - 2] = 'p';
			pipe_buffer[read_bytes - 1] = ';';
		} else {
			// index from 0 onwards, first byte ends up on index 0
			pipe_buffer[read_bytes - 1] = readbuf;
		}
	}
	pipe_buffer[read_bytes] = '\0';

	close(fd);
	return pipe_buffer;
}

char *oscap_acquire_guess_realpath(const char *filepath)
{
	char *rpath = realpath(filepath, NULL);
	if (rpath == NULL) {
		// file does not exists, let's try to guess realpath
		// this is not 100% correct, but it is good enough
		char *copy = strdup(filepath);
		char *real_dir = realpath(dirname(copy), NULL);
		if (real_dir == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Cannot guess realpath for %s, directory: %s does not exists!", filepath, real_dir);
			free(copy);
			return NULL;
		}
		rpath = oscap_sprintf("%s/%s", real_dir, basename((char *)filepath));
		free(real_dir);
		free(copy);
	}
	return rpath;
}
