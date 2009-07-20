#pragma once
#ifndef SEXP_MANIP_H
#define SEXP_MANIP_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <seap-debug.h>
#include <sexp-types.h>

/*
 * Functions for manipulating with numbers
 */

SEXP_t *SEXP_number_new  (const void *, NUM_type_t);

SEXP_t *SEXP_number_newd   (int n);
SEXP_t *SEXP_number_newhd  (short int n);
SEXP_t *SEXP_number_newhhd (char n);
SEXP_t *SEXP_number_newld  (long int n);
SEXP_t *SEXP_number_newlld (long long int n);
SEXP_t *SEXP_number_newu   (unsigned int n);
SEXP_t *SEXP_number_newhu  (unsigned short int n);
SEXP_t *SEXP_number_newhhu (unsigned char n);
SEXP_t *SEXP_number_newlu  (unsigned long int n);
SEXP_t *SEXP_number_newllu (unsigned long long int n);

SEXP_t *SEXP_number_newf  (double n);
SEXP_t *SEXP_number_newlf (long double n);

int     SEXP_number_get  (const SEXP_t *sexp, void *ptr, NUM_type_t type);

int           SEXP_number_getd   (const SEXP_t *sexp);
short int     SEXP_number_gethd  (const SEXP_t *sexp);
char          SEXP_number_gethhd (const SEXP_t *sexp);
long int      SEXP_number_getld  (const SEXP_t *sexp);
long long int SEXP_number_getlld (const SEXP_t *sexp);
unsigned int       SEXP_number_getu   (const SEXP_t *sexp);
unsigned short int SEXP_number_gethu  (const SEXP_t *sexp);
unsigned char      SEXP_number_gethhu (const SEXP_t *sexp);
unsigned long int  SEXP_number_getlu  (const SEXP_t *sexp);
unsigned long long SEXP_number_getllu (const SEXP_t *sexp);

double      SEXP_number_getf (const SEXP_t *sexp);
long double SEXP_number_getlf (const SEXP_t *sexp);

int     SEXP_numberp     (const SEXP_t *sexp);
size_t  SEXP_number_size (const SEXP_t *sexp);
int     SEXP_number_cmp  (const SEXP_t *a, const SEXP_t *b);

/*
 * Functions for manipulating with strings
 */

SEXP_t *SEXP_string_new (const void *, size_t);
SEXP_t *SEXP_string_newf (const char *fmt, ...);
int     SEXP_string_cmp (const SEXP_t *a, const SEXP_t *b);
int     SEXP_strcmp (const SEXP_t *sexp, const char *str);
int     SEXP_strncmp (const SEXP_t *sexp, const char *str, size_t n);
int     SEXP_strncoll (const SEXP_t *sexp, const char *str, size_t n);
int     SEXP_stringp (const SEXP_t *sexp);
char   *SEXP_string_cstr (const SEXP_t *sexp);
const char *SEXP_string_cstrp (const SEXP_t *sexp);
SEXP_t *SEXP_string_substr (const SEXP_t *sexp, size_t off, size_t len);
char   *SEXP_string_subcstr (const SEXP_t *sexp, size_t off, size_t len);
size_t  SEXP_string_length (const SEXP_t *sexp);

#if 0
typedef struct {
} SEXP_mp_t;

SEXP_mp_t *SEXP_strmpm_init (const char *str, ...);
SEXP_mp_t *SEXP_strmpm_inita (const char *str[], size_t n);
size_t    *SEXP_strmpm (SEXP_mp_t *mp, const SEXP_t *sexp);
void       SEXP_strmpm_free (SEXP_mp_t *mp);

SEXP_mp_t *SEXP_string_mpminit (SEXP_t *sexp, ...);
SEXP_mp_t *SEXP_string_mpminita (SEXP_t *sexp[], size_t n);
SEXP_mp_t *SEXP_string_mpminitl (SEXP_t *list);
size_t     SEXP_string_mpm (SEXP_mp_t *mp, const SEXP_t *sexp);
void       SEXP_string_mpmfree (SEXP_mp_t *mp);
#endif


/*
 * Functions for manipulating with lists
 */

SEXP_t *SEXP_list_new (void);
SEXP_t *SEXP_list_init (SEXP_t *);
SEXP_t *SEXP_list_free (SEXP_t *);
SEXP_t *SEXP_list_free_nr (SEXP_t *);
SEXP_t *SEXP_list_add (SEXP_t *, SEXP_t *);
SEXP_t *SEXP_list_first (const SEXP_t *);
SEXP_t *SEXP_list_last (const SEXP_t *);
int     SEXP_listp (const SEXP_t *);
SEXP_t *SEXP_list_pop (SEXP_t **);
SEXP_t *SEXP_list_nth (const SEXP_t *, uint32_t);
SEXP_t *SEXP_list_nth_dup (const SEXP_t *sexp, uint32_t n);
SEXP_t *SEXP_list_nth_deepdup (const SEXP_t *sexp, uint32_t n);
SEXP_t *SEXP_list_join (SEXP_t *, SEXP_t *);
size_t  SEXP_list_length (const SEXP_t *sexp);
SEXP_t *SEXP_list_map (SEXP_t *list, int (*fn) (SEXP_t *, SEXP_t *));
SEXP_t *SEXP_list_map2 (SEXP_t *list, int (*fn) (SEXP_t *, SEXP_t *, void *), void *ptr);
SEXP_t *SEXP_list_sort (SEXP_t *list, int (*cmp) (const SEXP_t *, const SEXP_t *));
int     SEXP_list_cmp (const SEXP_t *a, const SEXP_t *b);

#define SEXP_REDUCE_LNEIGHBOR 1
#define SEXP_REDUCE_RNEIGHBOR 2
#define SEXP_REDUCE_RANDOM    3
#define SEXP_REDUCE_PARALLEL  4

SEXP_t *SEXP_list_reduce (SEXP_t *list, SEXP_t *(*fn) (const SEXP_t *, const SEXP_t *), int strategy);
SEXP_t *SEXP_list_reduce2 (SEXP_t *list, SEXP_t *(*fn) (const SEXP_t *, const SEXP_t *, void *), int strategy, void *ptr);

#if __STDC_VERSION__ >= 199901L
#define SEXP_list_foreach(var, list)                                    \
        for (register uint32_t ___i_ = 1;                               \
             ((var) = SEXP_list_nth ((list), ___i_)) != NULL;           \
             ++___i_)

#define SEXP_sublist_foreach(var, list, beg, end)                       \
        for (register uint32_t ___i_ = (beg);                           \
             ___i_ <= (uint32_t)(end) && ((var) = SEXP_list_nth ((list), ___i_)) != NULL; \
             ++___i_)
#endif

void SEXP_list_cb (SEXP_t *list, void (*fn) (SEXP_t *, void *), void *ptr);

/*
 * Functions for manipulating with S-exp objects
 */

SEXP_t *SEXP_new  (void);
void    SEXP_free (SEXP_t *sexp);

SEXP_t *SEXP_dup (const SEXP_t *sexp); /* shallow copy */
SEXP_t *SEXP_deepdup (const SEXP_t *sexp);
SEXP_t *SEXP_copy (SEXP_t *dst, const SEXP_t *src); /* shallow copy */
SEXP_t *SEXP_deepcopy (SEXP_t *dst, const SEXP_t *src);

size_t  SEXP_length (const SEXP_t *sexp);

int     SEXP_cmp (const SEXP_t *a, const SEXP_t *b);
int     SEXP_cmpobj (const SEXP_t *a, const SEXP_t *b);

/*
 *  Returns atomic type of the given S-exp as a string.
 */
const char *SEXP_strtype (const SEXP_t *sexp);

#endif /* SEXP_MANIP_H */
