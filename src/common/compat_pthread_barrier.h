/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
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
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifndef OSCAP_COMPAT_PTHREAD_BARRIER_H_
#define OSCAP_COMPAT_PTHREAD_BARRIER_H_

#include "oscap_platforms.h"

/*
 * This file implements a very slow and basic pthread_barrier_t
 * on systems where this implementation isn't available.
 *
 * One such commonly used system is Apple MacOS X.
 */

#include <pthread.h>

// TODO: Maybe there is a better macro to check here
#ifdef OS_APPLE

// returned to the last thread that hits the trip count
#define PTHREAD_BARRIER_SERIAL_THREAD -1
// 0 is returned to all the other threads that have been waiting

typedef int pthread_barrierattr_t;

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int trip_count;
} pthread_barrier_t;

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_wait(pthread_barrier_t *barrier);

#endif

#endif
