#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include "../public/sm_alloc.h"
#include "generic/pqueue.h"

#define PQUEUE_SHED_LIMIT 32

struct pqueue {
        size_t count;
        size_t count_max;
        void  *queue;
        size_t i_first;
        pthread_mutex_t mutex;
        uint16_t exp;
};

#define PQUEUE_LOCK(q)   pthread_mutex_lock (&((q)->mutex))
#define PQUEUE_UNLOCK(q) pthread_mutex_unlock (&((q)->mutex))
#define PQ_PARRAY(b,i)   ((void **)((((void *)(b)) + sizeof (pqueue_hdr_t))))[i]

typedef struct {
        uint16_t s; /* s + 1 == size */
        uint16_t e;
        void    *n;
} pqueue_hdr_t;

pqueue_t *pqueue_new  (size_t max)
{
        pthread_mutexattr_t attrs;
        pqueue_t *q;
        
        _A(max > 0);
        
        q = sm_talloc (pqueue_t);
        q->count_max = max;
        q->count = 0;
        q->queue = NULL;
        q->i_first = 0;
        q->exp     = 0;

        pthread_mutexattr_init (&attrs);
        pthread_mutexattr_settype (&attrs, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init (&(q->mutex), &attrs);
        pthread_mutexattr_destroy (&attrs);
        
        return (q);
}

void pqueue_free (pqueue_t *q)
{
        pqueue_hdr_t *c_hdr, *n_hdr;

        _A(q != NULL);
        
        PQUEUE_LOCK(q); /* FIXME */
        c_hdr = (pqueue_hdr_t *)(q->queue);
        
        while (c_hdr != NULL) {
                n_hdr = (pqueue_hdr_t *)(c_hdr->n);
                sm_free (c_hdr);
                c_hdr = n_hdr;
        }
        
        PQUEUE_UNLOCK(q);
        
        pthread_mutex_destroy (&(q->mutex));
        sm_free (q);
        
        return;
}

int pqueue_add (pqueue_t *q, void *ptr)
{
        _A(q   != NULL);
        _A(ptr != NULL);
        return (pqueue_add_last (q, ptr));
}

int pqueue_add_first (pqueue_t *q, void *ptr)
{
        _A(q != NULL);
        _A(ptr != NULL);

        PQUEUE_LOCK(q);

        PQUEUE_UNLOCK(q);

        return (0);
}

int pqueue_add_last (pqueue_t *q, void *ptr)
{
        pqueue_hdr_t *c_hdr, *n_hdr, **w_ptr;

        _A(q != NULL);
        _A(ptr != NULL);
        
        PQUEUE_LOCK(q);
        
        c_hdr = (pqueue_hdr_t  *)(q->queue);
        w_ptr = (pqueue_hdr_t **)((void *)&(q->queue));

        while (c_hdr != NULL) {
                n_hdr = (pqueue_hdr_t *)(c_hdr->n);
                
                if (n_hdr == NULL) {
                        if (c_hdr->s < (size_t)(2 << c_hdr->e)) {
                                goto pq_add;
                        } else {
                                w_ptr = (pqueue_hdr_t **)((void *)&(c_hdr->n));
                                goto pq_new;
                        }
                } 
                
                c_hdr = n_hdr;
        }
pq_new:
        (*w_ptr) = c_hdr = sm_alloc (sizeof (pqueue_hdr_t) + (sizeof (void *) * (2 << q->exp)));
        c_hdr->e = q->exp;
        c_hdr->n = NULL;
        c_hdr->s = 0;
        
        ++(q->exp);
pq_add:
        PQ_PARRAY(c_hdr, c_hdr->s++) = ptr;
        ++(q->count);
        
        PQUEUE_UNLOCK(q);
        
        return (0);
}

void *pqueue_first (pqueue_t *q)
{
        void *ret;
        pqueue_hdr_t *c_hdr;
        
        _A(q != NULL);
        
        PQUEUE_LOCK(q);
        c_hdr = (pqueue_hdr_t *)(q->queue);
        
        if (c_hdr == NULL) {
                PQUEUE_UNLOCK(q);
                return (NULL);
        }
        
        ret = PQ_PARRAY(c_hdr, q->i_first++);
        --(q->count);
        
        _A(q->i_first <= c_hdr->s);
        
        if (q->i_first >= (size_t)(2 << c_hdr->s)) {
                q->queue = c_hdr->n;
                --(q->exp);
                q->i_first = 0;
                sm_free (c_hdr);                
        }
        
        PQUEUE_UNLOCK(q);
        return (ret);
}

void *pqueue_last  (pqueue_t *q)
{
        _A(q != NULL);
        return (NULL);
}

void *pqueue_pick (pqueue_t *q, int (*pickp) (void *ptr))
{
        _A(q     != NULL);
        _A(pickp != NULL);
        return (pqueue_pick_first (q, pickp));
}

static void pqueue_shed_block (pqueue_hdr_t *bh, size_t i, size_t c)
{
        while  (PQ_PARRAY(bh, i + c) == NULL && (i + c) < bh->s) c++;
        memcpy (PQ_PARRAY(bh, i), PQ_PARRAY(bh, i + c), sizeof (void *) * c);
}

void *pqueue_pick_first (pqueue_t *q, int (*pickp) (void *ptr))
{
        pqueue_hdr_t *c_hdr, **w_ptr;
        void *ptr;
        size_t i, null_cnt;
        
        _A(q     != NULL);
        _A(pickp != NULL);
        
        PQUEUE_LOCK(q);
        c_hdr = (pqueue_hdr_t *)(q->queue);
        w_ptr = (pqueue_hdr_t **)((void *)&(q->queue));
        
        while (c_hdr != NULL) {
                for (i = 0, null_cnt = 0; i < c_hdr->s; ++i) {
                        ptr = PQ_PARRAY(c_hdr, i);
                        
                        if (ptr != NULL) {
                                null_cnt = 0;

                                if (pickp (ptr)) {
                                        PQ_PARRAY(c_hdr, i) = NULL;
                                        --(q->count);
                                        PQUEUE_UNLOCK(q);
                                        
                                        return (NULL);
                                }
                        } else {
                                if (++null_cnt >= PQUEUE_SHED_LIMIT) {
                                        pqueue_shed_block (c_hdr, i - null_cnt + 1, null_cnt);
                                        null_cnt = 0;
                                }
                        }
                }
                
                if (c_hdr->s == null_cnt) {
                        if (c_hdr == q->queue)
                                q->i_first = 0;
                        
                        (*w_ptr) = c_hdr->n;
                        sm_free (c_hdr);
                        c_hdr = (*w_ptr);
                } else {
                        w_ptr = (pqueue_hdr_t **)((void *)&(c_hdr->n));
                        c_hdr = c_hdr->n;
                }
        }
        
        PQUEUE_UNLOCK(q);
        
        return (NULL);
}

void *pqueue_pick_last (pqueue_t *q, int (*pickp) (void *ptr))
{
        _A(q     != NULL);
        _A(pickp != NULL);
        
        return (NULL);
}

size_t pqueue_count (pqueue_t *q)
{
        size_t c;

        _A(q != NULL);

        PQUEUE_LOCK(q);
        c = q->count;
        PQUEUE_UNLOCK(q);
        
        return (c);
}

int pqueue_notempty (pqueue_t *q)
{
        int b;
        
        _A(q != NULL);

        PQUEUE_LOCK(q);
        b = (q->count > 0);
        PQUEUE_UNLOCK(q);
        
        return (b);
}
