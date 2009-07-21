#pragma once
#ifndef OSCAP_ALLOC_H
#define OSCAP_ALLOC_H

#include <stdlib.h>

#if defined(NDEBUG)
void *__oscap_alloc    (size_t s);
void *__oscap_calloc   (size_t n, size_t s);
void *__oscap_realloc  (void  *p, size_t s);
void *__oscap_reallocf (void  *p, size_t s);
void  __oscap_free     (void  *p);

# define oscap_alloc(s)       __oscap_alloc (s)
# define oscap_calloc(n, s)   __oscap_calloc (n, s);
# define oscap_realloc(p, s)  __oscap_realloc ((void *)(p), s)
# define oscap_reallocf(p, s) __oscap_reallocf((void *)(p), s)
# define oscap_free(p)        __oscap_free ((void *)(p))
#else
void *__oscap_alloc_dbg    (size_t s,           const char *f, size_t l);
void *__oscap_calloc_dbg   (size_t n, size_t s, const char *f, size_t l);
void *__oscap_realloc_dbg  (void  *p, size_t s, const char *f, size_t l);
void *__oscap_reallocf_dbg (void  *p, size_t s, const char *f, size_t l);
void  __oscap_free_dbg     (void **p,           const char *f, size_t l);

# define oscap_alloc(s)       __oscap_alloc_dbg (s, __PRETTY_FUNCTION__, __LINE__)
# define oscap_calloc(n, s)   __oscap_calloc_dbg (n, s, __PRETTY_FUNCTION__, __LINE__)
# define oscap_realloc(p, s)  __oscap_realloc_dbg ((void *)(p), s, __PRETTY_FUNCTION__, __LINE__)
# define oscap_reallocf(p, s) __oscap_reallocf_dbg ((void *)(p), s, __PRETTY_FUNCTION__, __LINE__)
# define oscap_free(p)        __oscap_free_dbg ((void **)&(p), __PRETTY_FUNCTION__, __LINE__)
#endif

#define  oscap_talloc(T) ((T *) oscap_alloc(sizeof(T)))
#define  oscap_valloc(v) ((typeof(v) *) oscap_alloc(sizeof v))

#include <assert.h>
#ifndef _A
# define _A(x) assert(x)
#endif /* _A */
#endif /* OSCAP_ALLOC_H */
