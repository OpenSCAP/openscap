#pragma once
#ifndef ASSUME_H
#define ASSUME_H

#include <stdio.h>
#include <stdlib.h>

/*
 * Usage:
 *
 * 1. assume(expr)
 *
 *    Check whether expr is true, call abort() if not (= assert)
 *
 * 2. assume(expr, f_branch)
 *    
 *    Check whether expr is true, execute f_branch if not and
 *    then call abort(). break can be used to skip the call to
 *    abort().
 *   
 * 3. assume(expr, f_branch, t_branch)
 *
 *    Check whether expr is true, execute f_branch if not and
 *    call abort(). If expr is true, then execute t_branch.
 *    
 * 
 */

#define __LB(l, ...) l
#define __RB(l, ...) __VA_ARGS__

#define __assume(expr, exprstr, ...)                                    \
        do {                                                            \
                if (!(expr)) {                                          \
                        fprintf (stderr, "%s:%d (%s): Assumption `%s' not fulfilled!\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, exprstr); \
                        do { __LB(__VA_ARGS__) } while(abort(),0);      \
                } else {                                                \
                        do { __RB(__VA_ARGS__) } while(0);              \
                }                                                       \
        } while (0)

#if defined(__GNUC__)
# define assume(expr, ...) __assume(__builtin_expect(expr, 1), #expr, __VA_ARGS__)
#else
# define assume(expr, ...) __assume(expr, #expr, __VA_ARGS__)
#endif

#endif /* ASSUME_H */
