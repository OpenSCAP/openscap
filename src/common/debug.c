
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

#ifndef NDEBUG
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

#ifndef PATH_SEPARATOR
# define PATH_SEPARATOR '/'
#endif

#  if defined(OSCAP_THREAD_SAFE)
#   include <pthread.h>
static pthread_mutex_t __debuglog_mutex = PTHREAD_MUTEX_INITIALIZER;
#  endif
static FILE *__debuglog_fp = NULL;
int __debuglog_level  = -1;
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

static void __oscap_vdlprintf(int level, const char *file, const char *fn, size_t line, const char *fmt, va_list ap)
{
	char  l;
	const char *f;

	__LOCK_FP;

	if (__debuglog_level == -1) {
		char *env;

		env = getenv(OSCAP_DEBUG_LEVEL_ENV);
		if (env == NULL)
			__debuglog_level = DBG_I;
		else
			__debuglog_level = atoi(env);
	}
	if (__debuglog_level < level) {
		__UNLOCK_FP;
		return;
	}
	if (__debuglog_fp == NULL) {
		char *logfile, pathbuf[4096];
		char *st;
		time_t ut;

		logfile = getenv(OSCAP_DEBUG_FILE_ENV);

		if (logfile == NULL)
			logfile = OSCAP_DEBUG_FILE;

		if (snprintf(pathbuf, sizeof pathbuf, "%s.%u",
			     logfile, (unsigned int)getpid()) >= (signed int) sizeof pathbuf)
		{
                        __UNLOCK_FP;
			return;
		}

                __debuglog_fp = fopen (pathbuf, "w");

		if (__debuglog_fp == NULL) {
			__UNLOCK_FP;
			return;
		}

		setbuf(__debuglog_fp, NULL);

		ut = time(NULL);
		st = ctime(&ut);

		fprintf(__debuglog_fp, "\n=============== LOG: %.24s ===============\n", st);
                atexit(&__oscap_debuglog_close);
	}
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

	if (flock(fileno(__debuglog_fp), LOCK_EX) == -1) {
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
	vfprintf(__debuglog_fp, fmt, ap);

	if (flock(fileno(__debuglog_fp), LOCK_UN) == -1) {
		/* __UNLOCK_FP; */
		abort();
	}

	__UNLOCK_FP;
	return;
}

void __oscap_dlprintf(int level, const char *file, const char *fn, size_t line, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__oscap_vdlprintf(level, file, fn, line, fmt, ap);
	va_end(ap);
}

void __oscap_debuglog_object (const char *file, const char *fn, size_t line, int objtype, void *obj)
{
        __LOCK_FP;

        if (__debuglog_fp == NULL) {
                char  *logfile, pathbuf[4096];
                char  *st;
                time_t ut;

                logfile = getenv (OSCAP_DEBUG_FILE_ENV);

                if (logfile == NULL)
                        logfile = OSCAP_DEBUG_FILE;

		if (snprintf(pathbuf, sizeof pathbuf, "%s.%ld",
			     logfile, (long)getpid()) >= (signed int) sizeof pathbuf)
		{
                        __UNLOCK_FP;
			return;
		}

                __debuglog_fp = fopen (pathbuf, "w");

                if (__debuglog_fp == NULL) {
                        __UNLOCK_FP;
                        return;
                }

                setbuf (__debuglog_fp, NULL);

                ut = time (NULL);
                st = ctime (&ut);

                fprintf (__debuglog_fp, "=============== LOG: %.24s ===============\n", st);
                atexit(&__oscap_debuglog_close);
        }

        if (flock (fileno (__debuglog_fp), LOCK_EX | LOCK_NB) == -1) {
                __UNLOCK_FP;
                return;
        }

#if defined(SEAP_THREAD_SAFE)
        /* XXX: non-portable usage of pthread_t */
	fprintf (__debuglog_fp, "(%ld:%llx) [%s:%zu:%s]\n------ \n", (long)getpid (), (unsigned long long)pthread_self(), file, line, fn);
#else
	fprintf (__debuglog_fp, "(%ld) [%s:%zu:%s]\n------\n ", (long)getpid (), file, line, fn);
#endif

        switch (objtype) {
        case OSCAP_DEBUGOBJ_SEXP:
                SEXP_fprintfa(__debuglog_fp, (SEXP_t *)obj);
        }

        fprintf(__debuglog_fp, "\n-----------\n");

        if (flock (fileno (__debuglog_fp), LOCK_UN | LOCK_NB) == -1) {
                /* __UNLOCK_FP; */
                abort ();
        }

        __UNLOCK_FP;
        return;
}

#endif
