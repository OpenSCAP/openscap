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

void __seap_debuglog (const char *file, const char *fn, size_t line, const char *fmt, ...)
{
        va_list ap;

#if defined(SEAP_THREAD_SAFE)        
        pthread_mutex_lock (&__debuglog_mutex);
#endif
        if (__debuglog_fp == NULL) {
                __debuglog_fp = fopen ("seap_debug.log", "a");
                setbuf (__debuglog_fp, NULL);
        }

#if defined(SEAP_THREAD_SAFE)        
        fprintf (__debuglog_fp, "(%u:%u) [%s: %zu: %s] ", (unsigned int)getpid (), (unsigned int)pthread_self(), file, line, fn);
#else
        fprintf (__debuglog_fp, "(%u) [%s: %zu: %s] ", (unsigned int)getpid (), file, line, fn);
#endif
        va_start (ap, fmt);
        vfprintf (__debuglog_fp, fmt, ap);
        va_end (ap);
        
#if defined(SEAP_THREAD_SAFE)  
        pthread_mutex_unlock (&__debuglog_mutex);
#endif
        return;
}
#endif
