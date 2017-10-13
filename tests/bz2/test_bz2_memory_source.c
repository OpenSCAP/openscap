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
 *	Simon Lukasik <slukasik@redhat.com>
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <oscap_error.h>
#include <oscap_source.h>

static unsigned long read_file(char *filename, char **buffer)
{
	FILE *file = fopen(filename, "rb");
	assert(file != NULL);

	fseek(file, 0, SEEK_END);
	unsigned long len = ftell(file);
	fseek(file, 0, SEEK_SET);

	assert(buffer != NULL);
	*buffer = malloc(len + 1);
	assert(*buffer != NULL);

	fread(*buffer, len, 1, file);
	fclose(file);
	return len;
}


int main(int argc, char *argv[])
{
	oscap_init();
	assert(argc == 2);
	char *buffer;
	unsigned long buff_size = read_file(argv[1], &buffer);
	assert(buff_size != 0);

	struct oscap_source *src = oscap_source_new_from_memory(buffer, buff_size, "file.xml.bz2");
	printf("SCAP TYPE: %s\n", oscap_document_type_to_string(oscap_source_get_scap_type(src)));
	assert(oscap_source_validate(src, NULL, NULL) == 0);
	oscap_source_free(src);
	free(buffer);

	if (oscap_err()) {
		char *err = oscap_err_get_full_error();
		fprintf(stderr, "%s", err);
		assert(strlen(err)==0);
		free(err);
	}

	oscap_cleanup();
	return 0;
}

