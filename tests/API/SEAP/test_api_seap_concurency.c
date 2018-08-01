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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sexp.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include "_sexp-manip.h"

#ifndef TEST_THREAD_COUNT_MAX
#define TEST_THREAD_COUNT_MAX 8
#endif

#ifndef TEST_REF_COUNT
#define TEST_REF_COUNT 32767
#endif

void *worker_thread (void *arg);

#define __PRINT_TIME(ident, t0, t1, tz, code)                           \
        do {                                                            \
                if (gettimeofday (&(t0), &(tz)) != 0) abort();          \
                code                                                    \
                if (gettimeofday (&(t1), &(tz)) != 0) abort();          \
                                                                        \
                flockfile (stdout);                                     \
                fprintf (stdout, "[%u][%s] t= %.3g\n", (unsigned int)(pthread_self ()), ident, \
                         ((double)t1.tv_sec + ((double)t1.tv_usec / 1000000.0)) - \
                         ((double)t0.tv_sec + ((double)t0.tv_usec / 1000000.0))); \
                funlockfile(stdout);                                    \
        } while (0)

void *worker_thread (void *arg)
{
        register int i;
        struct timezone tz;
        struct timeval t0, t1;
        
        tz.tz_minuteswest = 0;
        tz.tz_dsttime     = 0;

        SEXP_t *ref0 = (SEXP_t *)arg;
        SEXP_t *ref1 = SEXP_string_newf ("I'm a non-shared S-expression!\n");

        SEXP_t *set0[TEST_REF_COUNT];
        SEXP_t *set1[TEST_REF_COUNT];

        /******************************************/
        __PRINT_TIME("T1", t0, t1, tz,
                     for (i = 0; i < TEST_REF_COUNT; ++i) {
                             set0[i] = SEXP_ref (ref0);
                             set1[i] = SEXP_ref (ref1);
                     }
                     for (i = 0; i < TEST_REF_COUNT; ++i) {
                             SEXP_free (set0[i]);
                             SEXP_free (set1[i]);
                     }
                );
        /******************************************/
        __PRINT_TIME("T2", t0, t1, tz,
                     for (i = 0; i < TEST_REF_COUNT; ++i) {
                             set1[i] = SEXP_ref (ref1);
                             set0[i] = SEXP_ref (ref0);
                     }
                     for (i = 0; i < TEST_REF_COUNT; ++i) {
                             SEXP_free (set0[i]);
                             SEXP_free (set1[i]);
                     }
                );
        /******************************************/
        __PRINT_TIME("T3", t0, t1, tz,
                     for (i = 0; i < TEST_REF_COUNT; ++i) {
                             set0[i] = SEXP_ref (ref0);
                     }
                     for (i = 0; i < TEST_REF_COUNT; ++i) {
                             SEXP_free (set0[i]);
                     }
                );
        /******************************************/
        __PRINT_TIME("T4", t0, t1, tz,
                     for (i = 0; i < TEST_REF_COUNT; ++i) {
                             set1[i] = SEXP_ref (ref1);
                     }
                     for (i = 0; i < TEST_REF_COUNT; ++i) {
                             SEXP_free (set1[i]);
                     }
                );
        /******************************************/
        SEXP_free (ref1);

        return (ref0);
}

int main (void)
{
        pthread_t th[TEST_THREAD_COUNT_MAX];
        SEXP_t *s0; 
        register int i, th_cnt;
        
        s0 = SEXP_string_newf ("I'm a shared S-expression!\n");
        
        for (th_cnt = 2; th_cnt <= TEST_THREAD_COUNT_MAX; ++th_cnt) {
                SEXP_t *s0_ref;

                fprintf (stdout, "----- th_cnt= %u -----\n", th_cnt);
                
                for (i = 0; i < th_cnt; ++i) {
                        s0_ref = SEXP_ref (s0);
                        
                        if (pthread_create (&th[i], NULL, worker_thread, (void *)s0_ref) != 0) {
                                abort ();
                        }
                }
                
                for (i = 0; i < th_cnt; ++i) {
                        if (pthread_join (th[i], (void **)&s0_ref) != 0) {
                                abort ();
                        }
                        
                        SEXP_free (s0_ref);
                }
        }

        if (SEXP_refs (s0) != 1) abort ();
        SEXP_free (s0);

        return (0);
}
