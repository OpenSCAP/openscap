/**
 * @file   environmentvariable58_probe.c
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
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

#include "seap.h"
#include "probe-api.h"
#include "probe/entcmp.h"
#include "alloc.h"
#include "common/debug_priv.h"
#include "environmentvariable58_probe.h"

#define BUFFER_SIZE 256

extern char **environ;

static int read_environment(SEXP_t *pid_ent, SEXP_t *name_ent, probe_ctx *ctx)
{
	int err = 1, pid, fd;
	bool empty;
	size_t env_name_size;
	SEXP_t *env_name, *env_value, *item, *pid_sexp;
	DIR *d;
	struct dirent *d_entry;
	char *buffer, env_file[256], *null_char;
	ssize_t buffer_used;
	size_t buffer_size;

	d = opendir("/proc");
	if (d == NULL) {
		dE("Can't read /proc: errno=%d, %s.", errno, strerror (errno));
		return PROBE_EACCESS;
	}

	if ((buffer = realloc(NULL, BUFFER_SIZE)) == NULL) {
		dE("Can't allocate memory");
		closedir(d);
		return PROBE_EFAULT;
	}
	buffer_size = BUFFER_SIZE;

	while ((d_entry = readdir(d))) {
		if (strspn(d_entry->d_name, "0123456789") != strlen(d_entry->d_name))
			continue;

		pid = atoi(d_entry->d_name);
		pid_sexp = SEXP_number_newi_32(pid);

		if (probe_entobj_cmp(pid_ent, pid_sexp) != OVAL_RESULT_TRUE) {
			SEXP_free(pid_sexp);
			continue;
		}
		SEXP_free(pid_sexp);

		sprintf(env_file, "/proc/%d/environ", pid);

		if ((fd = open(env_file, O_RDONLY)) == -1) {
			dE("Can't open \"%s\": errno=%d, %s.", env_file, errno, strerror (errno));
			item = probe_item_create(
					OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE58, NULL,
					"pid", OVAL_DATATYPE_INTEGER, (int64_t)pid,
					NULL
			);

			probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);
			probe_item_add_msg(item, OVAL_MESSAGE_LEVEL_ERROR,
					   "Can't open \"%s\": errno=%d, %s.", env_file, errno, strerror (errno));
			probe_item_collect(ctx, item);
			continue;
		}

		empty = true;

		if ((buffer_used = read(fd, buffer, buffer_size - 1)) > 0) {
			empty = false;
		}

		while (! empty) {
			while (! (null_char = memchr(buffer, 0, buffer_used))) {
				ssize_t s;
				if ((size_t)buffer_used >= buffer_size) {
					buffer_size += BUFFER_SIZE;
					buffer = realloc(buffer, buffer_size);
					if (buffer == NULL) {
						dE("Can't allocate memory");
						exit(ENOMEM);
					}

				}
				s = read(fd, buffer + buffer_used, buffer_size - buffer_used);
				if (s <= 0) {
					empty = true;
					buffer[buffer_used++] = 0;
				}
				else {
					buffer_used += s;
				}
			}

			do {
				char *eq_char = strchr(buffer, '=');
				if (eq_char == NULL) {
					/* strange but possible:
					 * $ strings /proc/1218/environ
 					/dev/input/event0 /dev/input/event1 /dev/input/event4 /dev/input/event3
					*/
					buffer_used -= null_char + 1 - buffer;
					memmove(buffer, null_char + 1, buffer_used);
					continue;
				}

				env_name_size =  eq_char - buffer;
				env_name = SEXP_string_new(buffer, env_name_size);
				env_value = SEXP_string_newf("%s", buffer + env_name_size + 1);
				if (probe_entobj_cmp(name_ent, env_name) == OVAL_RESULT_TRUE) {
					item = probe_item_create(
						OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE58, NULL,
						"pid", OVAL_DATATYPE_INTEGER, (int64_t)pid,
						"name",  OVAL_DATATYPE_SEXP, env_name,
						"value", OVAL_DATATYPE_SEXP, env_value,
					      NULL);
					probe_item_collect(ctx, item);
					err = 0;
				}
				SEXP_free(env_name);
				SEXP_free(env_value);

				buffer_used -= null_char + 1 - buffer;
				memmove(buffer, null_char + 1, buffer_used);
			} while ((null_char = memchr(buffer, 0, buffer_used)));
		}

		close(fd);
	}
	closedir(d);
	free(buffer);
	if (err) {
		SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
				"Can't find process with requested PID.");
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		err = 0;
	}

	return err;
}

int environmentvariable58_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in, *name_ent, *pid_ent;
	int pid, err;

	probe_in  = probe_ctx_getobject(ctx);
	name_ent = probe_obj_getent(probe_in, "name", 1);

	if (name_ent == NULL) {
		return PROBE_ENOENT;
	}

	pid_ent = probe_obj_getent(probe_in, "pid", 1);
	if (pid_ent == NULL) {
		SEXP_free(name_ent);
		return PROBE_ENOENT;
	}

	PROBE_ENT_I32VAL(pid_ent, pid, pid = -1;, pid = 0;);

	if (pid == -1) {
		SEXP_free(name_ent);
		SEXP_free(pid_ent);
		return PROBE_ERANGE;
	}

	if (pid == 0) {
		/* overwrite pid value with actual pid */
		SEXP_t *nref, *nval, *new_pid_ent;

		nref = SEXP_list_first(probe_in);
		nval = SEXP_number_newu_32(getpid());
		new_pid_ent = SEXP_list_new(nref, nval, NULL);
		SEXP_free(pid_ent);
		SEXP_free(nref);
		SEXP_free(nval);
		pid_ent = new_pid_ent;
	}

	err = read_environment(pid_ent, name_ent, ctx);
	SEXP_free(name_ent);
	SEXP_free(pid_ent);

	return err;
}
