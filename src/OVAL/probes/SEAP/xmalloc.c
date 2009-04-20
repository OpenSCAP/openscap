#include <stdlib.h>
#include <unistd.h>
#include <config.h>
#include "seap.h"
#include "xmalloc.h"

#if defined(USE_XMALLOC)
void *xmalloc (size_t size)
{
        void *ptr;
  
        _A(size < (1<<16));
        
        ptr = malloc (size);
  
        if (NULL == ptr) {
                _exit (ERR_MEMORY_FATAL);
        }
  
        return ptr;
}

void *xrealloc (void *oldptr, size_t newsize)
{
        void *newptr;

        /* _A(oldptr != NULL); */
  
        newptr = realloc (oldptr, newsize);
        if (newptr == NULL && newsize > 0) {
                _exit (ERR_MEMORY_FATAL);
        }
  
        return newptr;
}

void xfree (void **ptr)
{
        return;
        //void **dptr = (void **)(ptr);
        _A(ptr  != NULL);
        _A(*ptr != NULL);
        
        free (*ptr);
        *ptr = NULL;
}
#endif
