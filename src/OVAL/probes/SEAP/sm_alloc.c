#ifndef __STUB_PROBE
#include <stdlib.h>
#include <errno.h>
#include "public/sm_alloc.h"

#if defined(NDEBUG)
/*
 * Normal
 */

void *sm_alloc (size_t s)
{
        void *m;
#if defined(SEAP_MALLOC_STRICT)
        _A(s > 0);
#endif
        m = malloc (s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL)
                exit (ENOMEM);
#endif
        return (m);
}

void *sm_calloc (size_t n, size_t s)
{
        void *m;
#if defined(SEAP_MALLOC_STRICT)
        _A(n > 0);
        _A(s > 0);
#endif
        m = calloc (n, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL)
                exit (ENOMEM);
#endif
        return (m);
}

void *sm_realloc (void *p, size_t s)
{
        void *m;

        m = realloc (p, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL && s > 0)
                exit (ENOMEM);
#endif
        return (m);
}

void *sm_reallocf (void *p, size_t s)
{
        void *m;

        m = realloc (p, s);
        if (m == NULL && s > 0) {
                sm_free (p);
#if defined(SEAP_MALLOC_EXIT)
                exit (ENOMEM);
#endif
        }
        return (m);
}

int sm_memalign (void **p, size_t a, size_t s)
{
        int ret;
        
        _A(p != NULL);

        ret = posix_memalign (p, a, s);
        
#if defined(SEAP_MALLOC_EXIT)        
        if (ret != 0)
                exit (ret);
#endif
        return (ret);
}

void sm_free (void *p)
{
#if defined(SEAP_MALLOC_STRICT)
        _A(p != NULL);
#endif
        if (p != NULL)
                free (p);
        return;
}

#else
/*
 * Debug
 */

void *__sm_alloc_dbg (size_t s, const char *func, size_t line)
{
        void *m;
#if defined(SEAP_MALLOC_STRICT)
        _A(s > 0);
#endif
        m = malloc (s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL) {
                _D("FAIL: size=%zu\n", s);
                exit (ENOMEM);
        }
#endif
        _D("%s:%u, ptr=%p, size=%zu\n", func, line, m, s);
        return (m);
}

void *__sm_calloc_dbg (size_t n, size_t s, const char *f, size_t l)
{
        void *m;
#if defined(SEAP_MALLOC_STRICT)
        _A(n > 0);
        _A(s > 0);
#endif
        m = calloc (n, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL) {
                _D("FAIL: nmemb=%zu, size=%zu, total=%zu\n",
                   n, s, n * s);
                exit (ENOMEM);
        }
#endif
        _D("ptr=%p, nmemb=%zu, size=%zu, total=%zu\n",
           m, n, s, n * s);
        return (m);
}

void *__sm_realloc_dbg (void *p, size_t s, const char *f, size_t l)
{
        void *m;
        m = realloc (p, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL && s > 0) {
                _D("FAIL: old=%p, size=%zu\n", p, s);
                exit (ENOMEM);
        }
#endif
        _D("%s:%u, old=%p, new=%p, size=%zu\n", f, l, p, m, s);
        return (m);
}

void *__sm_reallocf_dbg (void *p, size_t s, const char *f, size_t l)
{
        void *m;
        m = realloc (p, s);
        if (m == NULL && s > 0) {
                _D("FAIL: old=%p, size=%zu\n", p, s);
                sm_free (p);
#if defined(SEAP_MALLOC_EXIT)
                exit (ENOMEM);
#endif
        } else {
                _D("old=%p, new=%p, size=%zu\n", p, m, s);
        }
        return (m);
}

int __sm_memalign_dbg (void **p, size_t a, size_t s, const char *f, size_t l)
{
        int ret;
        
        _A(p != NULL);

        ret = posix_memalign (p, a, s);
        
#if defined(SEAP_MALLOC_EXIT)        
        if (ret != 0) {
                _D("FAIL: p=%p, a=%zu, s=%zu\n", p, a, s);
                exit (ret);
        }
#endif
        return (ret);
}

void __sm_free_dbg (void *p, const char *f, size_t l)
{
#if defined(SEAP_MALLOC_STRICT)
        _A(p != NULL);
#endif
        _D("ptr=%p\n", p);
        
        if (p != NULL)
                free (p);
        return;
}
#endif
#endif
