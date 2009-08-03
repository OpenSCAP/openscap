#pragma once
#ifndef COMMON_H
#define COMMON_H

#define _STR(x) #x
#define  STR(x) _STR(x)

#ifndef __XCONCAT
# include <sys/cdefs.h>
# define __XCONCAT(a, b) __CONCAT(a,b)
#endif

#include <stddef.h>
#include <stdint.h>

void    *xmemdup (const void *src, size_t len);
uint32_t xnumdigits (size_t size);
int      xstrncoll (const char *a, size_t alen, const char *b, size_t blen);
void     xsrandom (unsigned long seed);
long     xrandom (void);

#include <errno.h>

#ifndef EDOOFUS
# define EDOOFUS 88
#endif

#define protect_errno for (int __XCONCAT(__e,__LINE__) = errno, __XCONCAT(__s,__LINE__) = 1; \
                           __XCONCAT(__s,__LINE__)-- ; errno = __XCONCAT(__e, __LINE__))

#endif /* COMMON_H */
