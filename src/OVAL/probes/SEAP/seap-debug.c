#ifndef NDEBUG
# include <stdio.h>
# include <stdarg.h>
# include <sys/types.h>
# include <sys/file.h>
# include <unistd.h>
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
                __debuglog_fp = fopen ("seap_debug.log", "a");
                
                if (__debuglog_fp == NULL)
                        return;
                
                setbuf (__debuglog_fp, NULL);
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
