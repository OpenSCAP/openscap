#include <stdlib.h>
#include <errno.h>
#include "public/alloc.h"

#if defined(NDEBUG)
/*
 * Normal
 */

void *__oscap_alloc (size_t s)
{
        void *m;
#if defined(OSCAP_ALLOC_STRICT)
        _A(s > 0);
#endif
        m = malloc (s);
#if defined(OSCAP_ALLOC_EXIT)
        if (m == NULL)
                exit (ENOMEM);
#endif
        return (m);
}

void *__oscap_calloc (size_t n, size_t s)
{
        void *m;
#if defined(OSCAP_ALLOC_STRICT)
        _A(n > 0);
        _A(s > 0);
#endif
        m = calloc (n, s);
#if defined(OSCAP_ALLOC_EXIT)
        if (m == NULL)
                exit (ENOMEM);
#endif
        return (m);
}

void *__oscap_realloc (void *p, size_t s)
{
        void *m;

        m = realloc (p, s);
#if defined(OSCAP_ALLOC_EXIT)
        if (m == NULL && s > 0)
                exit (ENOMEM);
#endif
        return (m);
}

void *__oscap_reallocf (void *p, size_t s)
{
        void *m;

        m = realloc (p, s);
        if (m == NULL && s > 0) {
                oscap_free (p);
#if defined(OSCAP_ALLOC_EXIT)
                exit (ENOMEM);
#endif
        }
        return (m);
}

void __oscap_free (void *p)
{
#if defined(OSCAP_ALLOC_STRICT)
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

void *__oscap_alloc_dbg (size_t s, const char *func, size_t line)
{
        void *m;
#if defined(OSCAP_ALLOC_STRICT)
        _A(s > 0);
#endif
        m = malloc (s);
#if defined(OSCAP_ALLOC_EXIT)
        if (m == NULL)
                exit (ENOMEM);
#endif
        return (m);
}

void *__oscap_calloc_dbg (size_t n, size_t s, const char *f, size_t l)
{
        void *m;
#if defined(OSCAP_ALLOC_STRICT)
        _A(n > 0);
        _A(s > 0);
#endif
        m = calloc (n, s);
#if defined(OSCAP_ALLOC_EXIT)
        if (m == NULL)
                exit (ENOMEM);
#endif
        return (m);
}

void *__oscap_realloc_dbg (void *p, size_t s, const char *f, size_t l)
{
        void *m;
        m = realloc (p, s);
#if defined(OSCAP_ALLOC_EXIT)
        if (m == NULL && s > 0)
                exit (ENOMEM);
#endif
        return (m);
}

void *__oscap_reallocf_dbg (void *p, size_t s, const char *f, size_t l)
{
        void *m;
        m = realloc (p, s);
        if (m == NULL && s > 0) {
                oscap_free (p);
#if defined(OSCAP_ALLOC_EXIT)
                exit (ENOMEM);
#endif
        }
        return (m);
}

void __oscap_free_dbg (void **p, const char *f, size_t l)
{
        _A(p != NULL);
#if defined(OSCAP_ALLOC_STRICT)
        _A(*p != NULL);
#endif
        if (*p != NULL) {
                free (*p);
#if defined(OSCAP_ALLOC_RESET)
                *p = NULL;
#endif
        }
        return;
}
#endif
