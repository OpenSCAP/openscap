
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

#pragma once
#ifndef PQUEUE_H
#define PQUEUE_H
#include "../../../../common/util.h"

OSCAP_HIDDEN_START;

typedef struct pqueue pqueue_t;

#define PQUEUE_EFAIL  1
#define PQUEUE_EFULL  2
#define PQUEUE_EEMPTY 3

pqueue_t *pqueue_new  (size_t max);
void      pqueue_free (pqueue_t *q);

int pqueue_add (pqueue_t *q, void *ptr);

int pqueue_add_first (pqueue_t *q, void *ptr);
int pqueue_add_last  (pqueue_t *q, void *ptr);

void *pqueue_first (pqueue_t *q);
void *pqueue_last  (pqueue_t *q);

void *pqueue_pick (pqueue_t *q, int (*pickp) (void *ptr));
void *pqueue_pick_first (pqueue_t *q, int (*pickp) (void *ptr));
void *pqueue_pick_last  (pqueue_t *q, int (*pickp) (void *ptr));

size_t pqueue_count (pqueue_t *q);
int    pqueue_notempty (pqueue_t *q);

OSCAP_HIDDEN_END;

#endif /* PQUEUE_H */
