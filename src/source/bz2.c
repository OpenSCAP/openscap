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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *     Šimon Lukašík
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_BZ2

#include <bzlib.h>
#include <libxml/tree.h>
#include <stdlib.h>
#include <string.h>

#include "bz2_priv.h"
#include "common/_error.h"

struct bz2_file {
	FILE *f;
	BZFILE *file;
	bool eof;
};

static struct bz2_file *bz2_file_open(const char *filename)
{
	struct bz2_file *b = NULL;
	FILE* f;
	int bzerror;

	f = fopen (filename, "r" );
	if (f) {
		b = malloc(sizeof(struct bz2_file));
		b->f = f;
		b->file = BZ2_bzReadOpen(&bzerror, f, 0, 0, NULL, 0);
		if (bzerror != BZ_OK) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not build BZ2FILE from %s: %s",
					BZ2_bzerror(b->file, &bzerror));
			BZ2_bzReadClose(&bzerror, b->file);
			free(b);
			b = NULL;
		}
	}
	b->eof = false;
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
	oscap_free(bzfile);
	return bzerror == BZ_OK ? 0 : -1;
}

xmlDoc *bz2_file_read_doc(const char *filepath)
{
	struct bz2_file *bzfile = bz2_file_open(filepath);
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
	oscap_free(bzmem->stream);
	oscap_free(bzmem);
}

static struct bz2_mem *bz2_mem_open(const char *buffer, size_t size)
{
	struct bz2_mem *b = oscap_calloc(sizeof(struct bz2_mem), 1);
	b->stream = oscap_calloc(sizeof(bz_stream), 1);
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

bool bz2_file_is_bzip(const char *filepath)
{
	int offset = strlen(filepath) - strlen(".xml.bz2");
	if (offset >= 0) {
		return strcasecmp(filepath + offset, ".xml.bz2") == 0;
	}
	return false;
}
#endif
