#ifndef NDEBUG
# include <stdio.h>
# include <stdarg.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/file.h>
# include <unistd.h>
# include <time.h>
# include "public/debug.h"

#  if defined(OSCAP_THREAD_SAFE)
#   include <pthread.h>
static pthread_mutex_t __debuglog_mutex = PTHREAD_MUTEX_INITIALIZER;
#  endif
static FILE *__debuglog_fp = NULL;
int __debuglog_level = -1;

#if defined(OSCAP_THREAD_SAFE)
# define __LOCK_FP    do { if (pthread_mutex_lock   (&__debuglog_mutex) != 0) abort(); } while(0)
# define __UNLOCK_FP  do { if (pthread_mutex_unlock (&__debuglog_mutex) != 0) abort(); } while(0)
#else
# define __LOCK_FP   while(0)
# define __UNLOCK_FP while(0)
#endif

void __oscap_dprintf(const char *file, const char *fn, size_t line, const char *fmt, ...)
{
	va_list ap;

	__LOCK_FP;

        if (__debuglog_level == -1)
                __debuglog_level = atoi (getenv ("OSCAP_DEBUG_LEVEL"));
        
	if (__debuglog_fp == NULL) {
		char *logfile;
		char *st;
		time_t ut;

		logfile = getenv(OSCAP_DEBUG_FILE_ENV);

		if (logfile == NULL)
			logfile = OSCAP_DEBUG_FILE;

		__debuglog_fp = fopen(logfile, "a");

		if (__debuglog_fp == NULL) {
			__UNLOCK_FP;
			return;
		}

		setbuf(__debuglog_fp, NULL);

		ut = time(NULL);
		st = ctime(&ut);

		fprintf(__debuglog_fp, "=============== LOG: %.24s ===============\n", st);
	}

	if (flock(fileno(__debuglog_fp), LOCK_EX) == -1) {
		__UNLOCK_FP;
		return;
	}
#if defined(OSCAP_THREAD_SAFE)
	/* XXX: non-portable usage of pthread_t */
	fprintf(__debuglog_fp, "(%u:%u) [%s: %zu: %s] ", (unsigned int)getpid(), (unsigned int)pthread_self(), file,
		line, fn);
#else
	fprintf(__debuglog_fp, "(%u) [%s: %zu: %s] ", (unsigned int)getpid(), file, line, fn);
#endif
	va_start(ap, fmt);
	vfprintf(__debuglog_fp, fmt, ap);
	va_end(ap);

	if (flock(fileno(__debuglog_fp), LOCK_UN) == -1) {
		/* __UNLOCK_FP; */
		abort();
	}

	__UNLOCK_FP;
	return;
}

#endif
