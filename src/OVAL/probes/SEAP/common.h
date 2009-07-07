#pragma once
#ifndef COMMON_H
#define COMMON_H

#define _STR(x) #x
#define  STR(x) _STR(x)

#ifndef _D
# ifndef NDEBUG
#  include <stdio.h>
#  include <errno.h>
#  include <stdlib.h>
/*
#  define _D(...) do {                                                  \
                if (getenv("SEAP_NDEBUG") == NULL) {                    \
                        int ___e = errno;                               \
                        fprintf (stderr, "%s: ", __PRETTY_FUNCTION__);  \
                        errno = ___e;                                   \
                        fprintf (stderr, __VA_ARGS__);                  \
                }                                                       \
        } while(0)
# else
*/
#include <stdarg.h>
void __debuglog (const char *, size_t, const char *, ...);
#  define _D(...) __debuglog (__PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
# else
#  define _D(...) while(0)
# endif /* NDEBUG */
#endif /* _D */

#include <stddef.h>
#include <stdint.h>

void    *xmemdup (const void *src, size_t len);
uint32_t xnumdigits (size_t size);
int      xstrncoll (const char *a, size_t alen, const char *b, size_t blen);
void     xsrandom (unsigned long seed);
long     xrandom (void);

#ifndef EDOOFUS
# define EDOOFUS 88
#endif

#endif /* COMMON_H */
