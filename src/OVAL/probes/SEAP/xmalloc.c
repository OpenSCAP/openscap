#include <stdlib.h>
#include <unistd.h>
#include <config.h>
#include <assert.h>
#include "xmalloc.h"

#ifndef _A
#define _A(x) assert(x)
#endif

#if defined(USE_XMALLOC)
void *xmalloc (size_t size)
{
        void *ptr;
  
        _A(size < (1<<16));
        
        ptr = malloc (size);

#if defined(XMALLOC_EXIT)  
        if (NULL == ptr) {
                _exit (ERR_MEMORY_FATAL);
        }
#endif
        return ptr;
}

void *xcalloc (size_t num, size_t size)
{
        void *ptr;
        
        ptr = calloc (num, size);
#if defined(XMALLOC_EXIT)
        if (NULL == ptr) {
                _exit (ERR_MEMORY_FATAL);
        }
#endif
        return ptr;
}

void *xrealloc (void *oldptr, size_t newsize)
{
        void *newptr;

        /* _A(oldptr != NULL); */
  
        newptr = realloc (oldptr, newsize);
#if defined(XMALLOC_EXIT)
        if (newptr == NULL && newsize > 0) {
                _exit (ERR_MEMORY_FATAL);
        }
#endif
        return newptr;
}

void xfree (void **ptr)
{
        _A(ptr  != NULL);
        _A(*ptr != NULL);
        
        free (*ptr);
        *ptr = NULL;
}
#endif
