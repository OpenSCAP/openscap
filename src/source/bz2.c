/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *     Šimon Lukašík
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <stdlib.h>
#include <string.h>
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

#include "bz2_priv.h"
#include "common/_error.h"

#ifdef BZIP2_FOUND

#include <bzlib.h>

struct bz2_file {
	FILE *f;
	BZFILE *file;
	bool eof;
};

static struct bz2_file *bz2_fd_open(int fd)
{
	struct bz2_file *b = NULL;
	FILE* f;
	int bzerror;

	f = fdopen (fd, "r" );
	if (f) {
		b = malloc(sizeof(struct bz2_file));
		b->f = f;
		b->file = BZ2_bzReadOpen(&bzerror, f, 0, 0, NULL, 0);
		b->eof = false;
		if (bzerror != BZ_OK) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not build BZ2FILE: %s",
					BZ2_bzerror(b->file, &bzerror));
			BZ2_bzReadClose(&bzerror, b->file);
			free(b);
			b = NULL;
		}
	}
	return b;
}

//xmlInputReadCallback
static int bz2_file_read(struct bz2_file *bzfile, char *buffer, int len)
{
	int bzerror;
	if (bzfile->eof) {
		// If we run bzRead on closed file we will get SEQUENCE_ERROR
		return 0;
	}
	int size = BZ2_bzRead(&bzerror, (bzfile)->file, buffer, len);
	if (bzerror == BZ_STREAM_END) {
		bzfile->eof = true;
	}
	if (bzerror == BZ_OK || bzerror == BZ_STREAM_END)
		return size;
	else {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not read from bZ2FILE: %s",
				BZ2_bzerror(bzfile->file, &bzerror));
		return -1;
	}
}

// xmlInputCloseCallback
static int bz2_file_close(void *bzfile)
{
	int bzerror;
	BZ2_bzReadClose(&bzerror, ((struct bz2_file *)bzfile)->file);
	fclose(((struct bz2_file *)bzfile)->f);
	free(bzfile);
	return bzerror == BZ_OK ? 0 : -1;
}

xmlDoc *bz2_fd_read_doc(int fd)
{
	struct bz2_file *bzfile = bz2_fd_open(fd);
	if (bzfile == NULL) {
		return NULL;
	}
	return xmlReadIO((xmlInputReadCallback) bz2_file_read, bz2_file_close, bzfile, "url", NULL, XML_PARSE_PEDANTIC);
}

struct bz2_mem {
	bz_stream *stream;
	bool eof;
};

static void bz2_mem_free(struct bz2_mem *bzmem)
{
	free(bzmem->stream);
	free(bzmem);
}

static struct bz2_mem *bz2_mem_open(const char *buffer, size_t size)
{
	struct bz2_mem *b = calloc(1, sizeof(struct bz2_mem));
	b->stream = calloc(1, sizeof(bz_stream));
	// next_in should point at the compressed data
	b->stream->next_in = (char *) buffer;
	// and avail_in should indicate how many bytes the library may read
	b->stream->avail_in = size;
	int bzerror = BZ2_bzDecompressInit(b->stream, 0, 0);
	if (bzerror != BZ_OK) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not build bz_stream from memory buffer: BZ2_bzDecompressInit returns %d", bzerror);
		bz2_mem_free(b);
		return NULL;
	}
	return b;
}

// xmlInputReadCallback
static int bz2_mem_read(struct bz2_mem *bzmem, char *buffer, int len)
{
	if (len < 1) {
		// ensure that at least one byte of output space is available at each BZ2_bzDecompress call.
		return 0;
	}
	if (bzmem->eof) {
		// If we run BZ2_bzDecompress on processed buffer we will get -1 (SEQUENCE_ERROR)
		return 0;
	}
	// next_out should point to a buffer in which the uncompressed output is to be placed
	bzmem->stream->next_out = buffer;
	// with avail_out indicating how much output space is available.
	bzmem->stream->avail_out = len;
	int bzerror = BZ2_bzDecompress(bzmem->stream);
	if (bzerror == BZ_STREAM_END) {
		bzmem->eof = true;
	}
	if (bzerror == BZ_OK || bzerror == BZ_STREAM_END)
		return (len - bzmem->stream->avail_out);
	else {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not read from bz_stream: BZ2_bzDecompress returns %d", bzerror);
		return -1;
	}
}

// xmlInputCloseCallback
static int bz2_mem_close(void *bzmem) {
	int bzerror = BZ2_bzDecompressEnd(((struct bz2_mem *)bzmem)->stream);
	bz2_mem_free(bzmem);
	if (bzerror != BZ_OK) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not close bz_stream: BZ2_bzDecompressEnd returns %d", bzerror);
	}
	return bzerror == BZ_OK ? 0 : -1;
}

xmlDoc *bz2_mem_read_doc(const char *buffer, size_t size)
{
	struct bz2_mem *bzmem = bz2_mem_open(buffer, size);
	if (bzmem == NULL) {
		return NULL;
	}
	return xmlReadIO((xmlInputReadCallback) bz2_mem_read, bz2_mem_close, bzmem, "url", NULL, XML_PARSE_PEDANTIC);
}

#endif

static const char magic_number[] = {'B','Z'};

bool bz2_memory_is_bzip(const char* memory, const size_t size){
	if (size < 2){
		return false; // Cannot read magic number
	}

	// compare memory header with reference magic_number of bz2
	return ((memory[0] == magic_number[0]) && (memory[1] == magic_number[1]));
}

bool bz2_fd_is_bzip(int fd)
{
	int fd_dup = dup(fd);
	if (fd_dup == -1) {
		return false;
	}
	FILE* file = fdopen(fd_dup, "r");
	bool is_bzip;
	if (file == NULL) {
		return false; // cannot open/determine file type
	} else {
		// Compare magic number with file header. Type casting to integer solve EOF (-1) returned by fgetc()
		is_bzip = (fgetc(file) == (int)magic_number[0]) && (fgetc(file) == (int)magic_number[1]);
	}

	fclose(file);
	lseek(fd, 0, SEEK_SET);
	return is_bzip;

}
