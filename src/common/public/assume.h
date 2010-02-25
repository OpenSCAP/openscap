#pragma once
#ifndef ASSUME_H
#define ASSUME_H

#include <stdio.h>
#include <stdlib.h>

/*
 * Note: the term "terminate" used in the following text means a call to abort()
 *       in the case the code was compiled without NDEBUG defined. The retval
 *       argument is ignored in that case. In the other case (NDEBUG is defined)
 *       terminate means that `return (retval)' is used instead of aborting the
 *       program. This is because we are in a library and taking down a program
 *       from inside a library function isn't safe.
 *         Unless ASSUME_VERBOSE is defined during compilation time, assume won't
 *       emit any messages to stderr. In debugging mode (i.e. when NDEBUG is
 *       undefined) the error message is always written to stderr.
 *
 * Usage:
 *
 * 1. assume(expr, retval)
 *
 *    Check whether expr is true, terminate if not.
 *
 * 2. assume(expr, retval, f_branch)
 *    
 *    Check whether expr is true, execute f_branch if not and terminate. The break
 *    statement can be used to skip the termination.
 *   
 * 3. assume(expr, retval, f_branch, t_branch)
 *
 *    Check whether expr is true, execute f_branch if not and terminate. If expr
 *    is true, then execute t_branch.
 *    
 */

#define __LB(l, ...) l
#define __RB(l, ...) __VA_ARGS__

#include <sys/cdefs.h>
#define __XC(a, b) __CONCAT(a, __CONCAT(b, __CONCAT(atcg /* random string */, b)))

#define __emitmsg_fp stderr

/*
 * == Implementation note #1 ==
 * We use ftrylockfile here because it's better to drop the message than to
 * cause a deadlock.
 */
#define __atomic_emitmsg(...)                           \
        do {                                            \
                if (ftrylockfile(__emitmsg_fp) == 0) {  \
                        fprintf (stderr, __VA_ARGS__);  \
                        funlockfile(__emitmsg_fp);      \
                }                                       \
        } while (0)

#ifndef NDEBUG
# define __terminate(retval) abort()
# define __emitmsg(...) __atomic_emitmsg (__VA_ARGS__)
#else
# define __terminate(retval) return(retval)
# ifdef ASSUME_VERBOSE
#  define __emitmsg(...) __atomic_emitmsg (__VA_ARGS__)
# else
#  define __emitmsg(...) while(0)
# endif /* ASSUME_VERBOSE */
#endif /* NDEBUG */

#define __assume(expr, exprstr, retval, ...)                            \
        do {                                                            \
                int __XC(__cont, __LINE__) = 1;                         \
                if (!(expr)) {                                          \
                        __emitmsg ("%s:%d (%s): Assumption `%s' not fulfilled!\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, exprstr); \
                        do {__LB(__VA_ARGS__)} while((__XC(__cont, __LINE__) = 0)); \
                        if (__XC(__cont, __LINE__) == 0) __terminate(retval); \
                } else {                                                \
                        do {__RB(__VA_ARGS__)} while(0);                \
                }                                                       \
        } while (0)

#if defined(__GNUC__)
# define assume(expr, retval, ...) __assume(__builtin_expect(expr, 1), #expr, retval, __VA_ARGS__)
#else
# define assume(expr, retval, ...) __assume(expr, #expr, retval, __VA_ARGS__)
#endif

/**
 * assume_r (runtime) is present even if NDEBUG is defined
 */
#define assume_r(...) assume(__VA_ARGS__)

/**
 * assume_d (debug) is present if NDEBUG is not defined (i.e. in debugging mode)
 */
#ifndef NDEBUG
# define assume_d(...) assume(__VA_ARGS__)
#else
# define assume_d(...) while(0)
#endif

#endif /* ASSUME_H */
