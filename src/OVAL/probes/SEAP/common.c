#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <config.h>
#include "xmalloc.h"
#include "common.h"

#ifndef _A
#define _A(x) assert(x)
#endif

void *xmemdup (const void *src, size_t len)
{
        void *new;
        
        _A(src != NULL);

        new = xmalloc (len);
        memcpy (new, src, len);
        
        return (new);
}

uint32_t xnumdigits (size_t size)
{
        uint32_t i = 0;

        do {
                ++i;
                size /= 10;
        } while (size > 0);

        return (i);
}

int xstrncoll (const char *a, size_t alen,
               const char *b, size_t blen)
{
        int cmp;

        _A(a != NULL);
        _A(b != NULL);
        
        if (alen > blen) {
                cmp = strncmp (a, b, blen);
                return (cmp == 0 ?  1 : cmp);
        } else if (blen > alen) {
                cmp = strncmp (a, b, alen);
                return (cmp == 0 ? -1 : cmp);
        } else {
                return strncmp (a, b, alen);
        }
}

void xsrandom (unsigned long seed)
{
        srandom (seed);
}

long xrandom (void)
{
        return random ();
}

#ifndef NDEBUG
# include <stdio.h>
# include <stdarg.h>
#  if defined(THREAD_SAFE)
#   include <pthread.h>
pthread_mutex_t __debuglog_mutex = PTHREAD_MUTEX_INITIALIZER;
#  endif
static FILE *__debuglog_fp = NULL;

void __debuglog (const char *fn, size_t line, const char *fmt, ...)
{
        va_list ap;
        char *nfmt;
        char linestr[9];
        size_t nfmt_len;

#if defined(THREAD_SAFE)        
        pthread_mutex_lock (&__debuglog_mutex);
#endif
        if (__debuglog_fp == NULL) {
                __debuglog_fp = fopen ("seap_debug.log", "a");
                setbuf (__debuglog_fp, NULL);
        }
        
        snprintf (linestr, sizeof linestr, "%zu", line);
        nfmt_len = strlen (fn) + strlen(linestr) + strlen (fmt);
        nfmt = malloc (sizeof (char) * (nfmt_len + 3 + 1));
        
        if (nfmt != NULL) {
                snprintf (nfmt, nfmt_len, "%s:%s: %s", linestr, fn, fmt);
                
                va_start (ap, fmt);
                vfprintf (__debuglog_fp, fmt, ap);
                va_end (ap);
                
                free (nfmt);
        }
#if defined(THREAD_SAFE)  
        pthread_mutex_unlock (&__debuglog_mutex);
#endif
        return;
}
#endif
