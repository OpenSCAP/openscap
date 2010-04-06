/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

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

#if defined(SEAP_THREAD_SAFE)
# define __LOCK_FP    do { if (pthread_mutex_lock   (&__debuglog_mutex) != 0) abort(); } while(0)
# define __UNLOCK_FP  do { if (pthread_mutex_unlock (&__debuglog_mutex) != 0) abort(); } while(0)
#else
# define __LOCK_FP   while(0)
# define __UNLOCK_FP while(0)
#endif

void __seap_debuglog (const char *file, const char *fn, size_t line, const char *fmt, ...)
{
        va_list ap;

        if (getenv ("SEAP_DEBUGLOG_DISABLE") != NULL)
                return;
        
        __LOCK_FP;
        
        if (__debuglog_fp == NULL) {
                char  *logfile;
                char  *st;
                time_t ut;
                
                logfile = getenv (SEAP_DEBUG_FILE_ENV);
                
                if (logfile == NULL)
                        logfile = SEAP_DEBUG_FILE;
                
                __debuglog_fp = fopen (logfile, "a");
                
                if (__debuglog_fp == NULL) {
                        __UNLOCK_FP;
                        return;
                }
                
                setbuf (__debuglog_fp, NULL);
                
                ut = time (NULL);
                st = ctime (&ut);
                
                fprintf (__debuglog_fp, "=============== LOG: %.24s ===============\n", st);
        }
        
        if (flock (fileno (__debuglog_fp), LOCK_EX | LOCK_NB) == -1) {
                __UNLOCK_FP;
                return;
        }
        
#if defined(SEAP_THREAD_SAFE)        
        /* XXX: non-portable usage of pthread_t */
        fprintf (__debuglog_fp, "(%u:%u) [%s: %zu: %s] ", (unsigned int)getpid (), (unsigned int)pthread_self(), file, line, fn);
#else
        fprintf (__debuglog_fp, "(%u) [%s: %zu: %s] ", (unsigned int)getpid (), file, line, fn);
#endif
        va_start (ap, fmt);
        vfprintf (__debuglog_fp, fmt, ap);
        va_end (ap);
        
        if (flock (fileno (__debuglog_fp), LOCK_UN | LOCK_NB) == -1) {
                /* __UNLOCK_FP; */
                abort ();
        }
        
        __UNLOCK_FP;
        return;
}
#endif
