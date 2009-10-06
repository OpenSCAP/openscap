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
