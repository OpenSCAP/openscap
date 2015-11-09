
/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

# include <stdio.h>
# include <stdarg.h>
# include <string.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/file.h>
# include <unistd.h>
# include <time.h>
# include <errno.h>

# include <sexp.h>
# include <sexp-output.h>

# include "debug_priv.h"

#include <oscap_debug.h>
#include "_error.h"

#ifndef PATH_SEPARATOR
# define PATH_SEPARATOR '/'
#endif

static const struct oscap_string_map OSCAP_VERBOSITY_LEVELS[] = {
    {DBG_E, "ERROR"},
    {DBG_W, "WARNING"},
    {DBG_I, "INFO"},
    {DBG_D, "DEVEL"},
    {DBG_UNKNOWN, NULL}
};

#  if defined(OSCAP_THREAD_SAFE)
#   include <pthread.h>
static pthread_mutex_t __debuglog_mutex = PTHREAD_MUTEX_INITIALIZER;
#  endif
static FILE *__debuglog_fp = NULL;
static oscap_verbosity_levels __debuglog_level = DBG_UNKNOWN;
static int __debuglog_pstrip = -1;

#if defined(OSCAP_THREAD_SAFE)
# define __LOCK_FP    do { if (pthread_mutex_lock   (&__debuglog_mutex) != 0) abort(); } while(0)
# define __UNLOCK_FP  do { if (pthread_mutex_unlock (&__debuglog_mutex) != 0) abort(); } while(0)
#else
# define __LOCK_FP   while(0)
# define __UNLOCK_FP while(0)
#endif

#define THREAD_NAME_LEN 16

static void __oscap_debuglog_close(void)
{
        fclose(__debuglog_fp);
}

oscap_verbosity_levels oscap_verbosity_level_from_cstr(const char *level_name)
{
	return oscap_string_to_enum(OSCAP_VERBOSITY_LEVELS, level_name);
}

bool oscap_set_verbose(const char *verbosity_level, const char *filename, bool is_probe)
{
	if (verbosity_level == NULL || filename == NULL) {
		return true;
	}
	__debuglog_level = oscap_verbosity_level_from_cstr(verbosity_level);
	if (__debuglog_level == DBG_UNKNOWN) {
		return false;
	}
	if (is_probe) {
		__debuglog_fp = fopen(filename, "a");
	} else {
		setenv("OSCAP_PROBE_VERBOSITY_LEVEL", verbosity_level, 1);
		setenv("OSCAP_PROBE_VERBOSE_LOG_FILE", filename, 1);
		__debuglog_fp = fopen(filename, "w");
	}
	if (__debuglog_fp == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Failed to open file %s.", filename);
		return false;
	}
	setbuf(__debuglog_fp, NULL);
	atexit(&__oscap_debuglog_close);
	return true;
}


static const char *__oscap_path_rstrip(const char *path, int num)
{
	register size_t len;

	len = strlen(path);

	for (len = strlen(path); len > 0; --len) {
		if (path[len - 1] == PATH_SEPARATOR)
			--num;
		if (num == 0)
			return (path + len);
	}

	return (path);
}


static void debug_message_start(int level, const char *file, const char *fn, size_t line)
{
	char  l;
	const char *f;

	__LOCK_FP;

	if (__debuglog_pstrip == -1) {
		char *pstrip;

		pstrip = getenv(OSCAP_DEBUG_PATHSTRIP_ENV);

		if (pstrip == NULL)
			__debuglog_pstrip = 0;
		else
			__debuglog_pstrip = atol(pstrip);

	}
	if (__debuglog_pstrip != 0)
		f = __oscap_path_rstrip(file, __debuglog_pstrip);
	else
		f = file;

#if defined(__SVR4) && defined (__sun)
	if (lockf(fileno(__debuglog_fp), F_LOCK, 0L) == -1) {
#else
	if (flock(fileno(__debuglog_fp), LOCK_EX) == -1) {
#endif
		__UNLOCK_FP;
		return;
	}

	switch (level) {
	case DBG_E:
		l = 'E';
		break;
	case DBG_W:
		l = 'W';
		break;
	case DBG_I:
		l = 'I';
		break;
	case DBG_D:
		l = 'D';
		break;
	default:
		l = '0';
	}
#if defined(OSCAP_THREAD_SAFE)
	char thread_name[THREAD_NAME_LEN];
	pthread_t thread = pthread_self();
	pthread_getname_np(thread, thread_name, THREAD_NAME_LEN);
	/* XXX: non-portable usage of pthread_t */
	fprintf(__debuglog_fp, "(%s(%ld):%s(%llx)) [%c:%s:%zu:%s] ",
		program_invocation_short_name, (long) getpid(), thread_name,
		(unsigned long long) thread, l, f, line, fn);
#else
	fprintf(__debuglog_fp, "(%ld) [%c:%s:%zu:%s] ", (long) getpid(),
		l, f, line, fn);
#endif
}

static void debug_message_end()
{
#if defined(__SVR4) && defined (__sun)
	if (lockf(fileno(__debuglog_fp), F_ULOCK, 0L) == -1) {
#else
	if (flock(fileno(__debuglog_fp), LOCK_UN) == -1) {
#endif
		/* __UNLOCK_FP; */
		abort();
	}

	__UNLOCK_FP;
	return;
}

void __oscap_dlprintf(int level, const char *file, const char *fn, size_t line, const char *fmt, ...)
{
	va_list ap;

	if (__debuglog_fp == NULL) {
		return;
	}
	if (__debuglog_level < level) {
		return;
	}
	va_start(ap, fmt);
	debug_message_start(level, file, fn, line);
	vfprintf(__debuglog_fp, fmt, ap);
	debug_message_end();
	va_end(ap);
}

void __oscap_debuglog_object (const char *file, const char *fn, size_t line, int objtype, void *obj)
{
	if (__debuglog_fp == NULL) {
		return;
	}
	if (__debuglog_level < DBG_I) {
		return;
	}
	debug_message_start(DBG_I, file, fn, line);
	switch (objtype) {
	case OSCAP_DEBUGOBJ_SEXP:
		SEXP_fprintfa(__debuglog_fp, (SEXP_t *)obj);
		break;
	default:
		fprintf(__debuglog_fp, "Attempt to dump a not supported object.\n");
	}
	debug_message_end();
}

