#pragma once
#ifndef PQUEUE_H
#define PQUEUE_H

typedef struct pqueue pqueue_t;

pqueue_t *pqueue_new  (size_t max);
void      pqueue_free (pqueue_t *q);

int   pqueue_add   (pqueue_t *q, void *ptr);
void *pqueue_first (pqueue_t *q);
void *pqueue_last  (pqueue_t *q);

#endif /* PQUEUE_H */
