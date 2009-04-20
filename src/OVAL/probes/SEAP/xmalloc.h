#if defined(USE_XMALLOC)

#define ERR_MEMORY_FATAL 32

void *xmalloc  (size_t);
void *xrealloc (void *, size_t);
void  xfree    (void **);
#else
# if !defined(_STDLIB_H_)
#  include <stdlib.h>
# endif
# define xmalloc(size)             malloc(size)
# define xrealloc(oldptr, newsize) realloc(oldptr, newsize)
# define xfree(pptr)               free(*(pptr))
#endif /* USE_XMALLOC */
