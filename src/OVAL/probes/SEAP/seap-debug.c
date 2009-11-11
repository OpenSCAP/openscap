#ifndef NDEBUG
# include <stdio.h>
# include <stdarg.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/file.h>
# include <unistd.h>
# include <time.h>
# include "public/seap-debug.h"

#  if defined(SEAP_THREAD_SAFE)
#   include <pthread.h>
static pthread_mutex_t __debuglog_mutex = PTHREAD_MUTEX_INITIALIZER;
#  endif
static FILE *__debuglog_fp = NULL;

void __seap_debuglog (const char *file, const char *fn, size_t line, const char *fmt, ...)
{
        va_list ap;

#if defined(SEAP_THREAD_SAFE)        
        pthread_mutex_lock (&__debuglog_mutex);
#endif
        if (__debuglog_fp == NULL) {
                char  *logfile;
                char  *st;
                time_t ut;
                
                logfile = getenv (SEAP_DEBUG_FILE_ENV);
                
                if (logfile == NULL)
                        logfile = SEAP_DEBUG_FILE;
                
                __debuglog_fp = fopen (logfile, "a");
                
                if (__debuglog_fp == NULL)
                        return;
                
                setbuf (__debuglog_fp, NULL);
                
                ut = time (NULL);
                st = ctime (&ut);
                
                fprintf (__debuglog_fp, "=============== LOG: %.24s ===============\n", st);
        }
        
        if (flock (fileno (__debuglog_fp), LOCK_EX) == -1)
                return;
        
#if defined(SEAP_THREAD_SAFE)        
        fprintf (__debuglog_fp, "(%u:%u) [%s: %zu: %s] ", (unsigned int)getpid (), (unsigned int)pthread_self(), file, line, fn);
#else
        fprintf (__debuglog_fp, "(%u) [%s: %zu: %s] ", (unsigned int)getpid (), file, line, fn);
#endif
        va_start (ap, fmt);
        vfprintf (__debuglog_fp, fmt, ap);
        va_end (ap);
        
        if (flock (fileno (__debuglog_fp), LOCK_UN) == -1)
                abort ();
        
#if defined(SEAP_THREAD_SAFE)  
        pthread_mutex_unlock (&__debuglog_mutex);
#endif
        return;
}
#endif
