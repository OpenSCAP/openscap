#ifndef NDEBUG
# include <stdio.h>
# include <stdarg.h>
# include <sys/types.h>
# include <unistd.h>
#  if defined(SEAP_THREAD_SAFE)
#   include <pthread.h>
pthread_mutex_t __debuglog_mutex = PTHREAD_MUTEX_INITIALIZER;
#  endif
static FILE *__debuglog_fp = NULL;

void __seap_debuglog (const char *fn, size_t line, const char *fmt, ...)
{
        va_list ap;

#if defined(SEAP_THREAD_SAFE)        
        pthread_mutex_lock (&__debuglog_mutex);
#endif
        if (__debuglog_fp == NULL) {
                __debuglog_fp = fopen ("seap_debug.log", "a");
                setbuf (__debuglog_fp, NULL);
        }
        
        fprintf (__debuglog_fp, "(%u) [%zu: %s] ", getpid (), line, fn);
        va_start (ap, fmt);
        vfprintf (__debuglog_fp, fmt, ap);
        va_end (ap);
                
#if defined(SEAP_THREAD_SAFE)  
        pthread_mutex_unlock (&__debuglog_mutex);
#endif
        return;
}
#endif
