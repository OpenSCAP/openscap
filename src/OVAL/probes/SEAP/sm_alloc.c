#include <stdlib.h>
#include <errno.h>
#include "public/sm_alloc.h"

#if defined(NDEBUG)
/*
 * Normal
 */

void *__sm_alloc (size_t s)
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

void *__sm_calloc (size_t n, size_t s)
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

void *__sm_realloc (void *p, size_t s)
{
        void *m;

        m = realloc (p, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL && s > 0)
                exit (ENOMEM);
#endif
        return (m);
}

void *__sm_reallocf (void *p, size_t s)
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

void __sm_free (void *p)
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
        if (m == NULL)
                exit (ENOMEM);
#endif
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
        if (m == NULL)
                exit (ENOMEM);
#endif
        return (m);
}

void *__sm_realloc_dbg (void *p, size_t s, const char *f, size_t l)
{
        void *m;
        m = realloc (p, s);
#if defined(SEAP_MALLOC_EXIT)
        if (m == NULL && s > 0)
                exit (ENOMEM);
#endif
        return (m);
}

void *__sm_reallocf_dbg (void *p, size_t s, const char *f, size_t l)
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

void __sm_free_dbg (void **p, const char *f, size_t l)
{
#if defined(SEAP_MALLOC_STRICT)
        _A(p != NULL);
#endif
        free (*p);
#if defined(SEAP_MALLOC_RESET)
        *p = NULL;
#endif
        return;
}
#endif
