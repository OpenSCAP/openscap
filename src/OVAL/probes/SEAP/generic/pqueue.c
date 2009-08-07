#include <stddef.h>
#include <stdint.h>
#include "../public/sm_alloc.h"
#include "generic/pqueue.h"

struct pqueue {
        size_t count;
        size_t count_max;
        void  *queue;
        size_t fst;
        size_t lst;
};

typedef struct {
        uint32_t e;
        void    *n;
} pqueue_hdr_t;

pqueue_t *pqueue_new  (size_t max)
{
        pqueue_t *q;
        
        _A(max > 0);
        
        q = sm_talloc (pqueue_t);
        q->count_max = max;
        q->count = 0;
        q->queue = NULL;
        q->fst   = 0;
        q->lst   = 0;

        return (q);
}

void pqueue_free (pqueue_t *q)
{
        pqueue_hdr_t *c_hdr, *n_hdr;

        _A(q != NULL);
        
        c_hdr = (pqueue_hdr_t *)q->queue;
        
        while (c_hdr != NULL) {
                n_hdr = (pqueue_hdr_t *)(c_hdr->n);
                sm_free (c_hdr);
                c_hdr = n_hdr;
        }
        
        sm_free (q);
        return;
}

int pqueue_add (pqueue_t *q, void *ptr)
{
        return (-1);
}

void *pqueue_first (pqueue_t *q)
{
        return (NULL);
}

void *pqueue_last  (pqueue_t *q)
{
        return (NULL);
}
