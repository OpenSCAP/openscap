/**
 * @file   environmentvariable58.c
 * @brief  environmentvariable58 probe
 * @author "Petr Lautrbach" <plautrba@redhat.com>
 *
 *  This probe is able to process a environmentvariable58_object as defined in OVAL 5.8.
 *
 */

/*
 * Copyright 2009-2011 Red Hat Inc., Durham, North Carolina.
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
 *   Petr Lautrbach <plautrba@redhat.com>
 */

/*
 * environmentvariable58 probe:
 *
 * pid
 * name
 * value
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "alloc.h"

#define BUFFER_SIZE 256

extern char **environ;

static int read_environment(SEXP_t *pid_ent, SEXP_t *name_ent, probe_ctx *ctx)
{
	int err = 1, pid, empty = 1;
	size_t env_name_size;
	SEXP_t *env_name, *env_value, *item;
	char *buffer = NULL, env_file[256];
	int fd;
	unsigned int buffer_size = 0, read_size = 0;

	PROBE_ENT_I32VAL(pid_ent, pid, return -1;);

	if (pid == 0)
		pid = getpid();

	sprintf(env_file, "/proc/%d/environ", pid);

	if ((fd = open(env_file, O_RDONLY)) == -1) {
		return err;
	}

	if ((buffer = oscap_realloc(NULL, BUFFER_SIZE)) == NULL) {
		close(fd);
		return err;
	}

	buffer_size = BUFFER_SIZE;

	if (read_size = read(fd, buffer, buffer_size - 1))
		empty = 0;

	buffer[buffer_size - 1] = 0;

	while (! empty) {
		/* we dont have whole var=val string */
		while (strlen(buffer) >= read_size) {
			int s;
			if (read_size + 1 >= buffer_size) {
				buffer_size += BUFFER_SIZE;
				buffer = oscap_realloc(buffer, buffer_size);
			}
			s = read(fd, buffer + read_size, buffer_size - read_size - 1);
			if (s == 0) {
				empty = 1;
				break;
			}
			read_size += s;
			buffer[buffer_size - 1] = 0;
		}

		while (strlen(buffer) < read_size && read_size > 0) {
			env_name_size =  strchr(buffer, '=') - buffer;
			env_name = SEXP_string_new(buffer, env_name_size);
			env_value = SEXP_string_newf("%s", buffer + env_name_size + 1);
			if (probe_entobj_cmp(name_ent, env_name) == OVAL_RESULT_TRUE) {
				item = probe_item_create(
					OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE58, NULL,
					"pid", OVAL_DATATYPE_INTEGER, pid,
					"name",  OVAL_DATATYPE_SEXP, env_name,
					"value", OVAL_DATATYPE_SEXP, env_value,
				      NULL);
				probe_item_collect(ctx, item);
				err = 0;
			}
			SEXP_free(env_name);
			SEXP_free(env_value);

			read_size -= strlen(buffer);
			memmove(buffer, buffer + strlen(buffer) + 1, read_size--);
		}
	}
	oscap_free(buffer);
	close(fd);
	return err;
}

int probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *name, *pid;

	probe_in  = probe_ctx_getobject(ctx);
	name = probe_obj_getent(probe_in, "name", 1);

	if (name == NULL) {
		return PROBE_ENOVAL;
	}

	pid = probe_obj_getent(probe_in, "pid", 1);

	read_environment(pid, name, ctx);
	SEXP_free(name);
	SEXP_free(pid);

	return 0;
}
