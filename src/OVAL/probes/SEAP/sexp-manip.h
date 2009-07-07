#pragma once
#ifndef SEXP_MANIP_H
#define SEXP_MANIP_H

#include <assert.h>
#ifndef _A
#define _A(x) assert(x)
#endif

#include <stdarg.h>
#include <stdint.h>
#include "sexp.h"

/* 
 * NUM_STORE(type, nsrc, voidp)
 *  stores the number at nsrc to voidp directly
 *  if sizeof(void *) is less than or equal to
 *  sizeof(type). Otherwise sizeof(type) bytes
 *  of memory is allocated and the number is
 *  stored there. The pointer to this memory
 *  block is stored in voidp.
 */
#define NUM_STORE(type, nsrc, voidp) do {                            \
                if (sizeof (type) <= VOIDPTR_SIZE) {                 \
                        *((type *)(&(voidp))) = (type)(nsrc);        \
                } else {                                             \
                        (voidp) = xmalloc (sizeof (type));           \
                        *((type *)(voidp)) = (type)(nsrc);           \
                }                                                    \
        } while (0)

/*
 * NUM(type, voidp)
 *  returns the number of type type stored
 *  at or in voidp.
 */
#define NUM(type, voidp) (sizeof (type) <= VOIDPTR_SIZE ? (*((type *)(&(voidp)))) : *((type *)(voidp)))

/* Functions for manipulating with numbers */
SEXP_t *SEXP_number_new (const void *, NUM_type_t);
int     SEXP_numberp (const SEXP_t *sexp);
size_t  SEXP_number_size (const SEXP_t *sexp);
int     SEXP_number_get (const SEXP_t *sexp, void *ptr, NUM_type_t type);
int     SEXP_number_cmp (const SEXP_t *a, const SEXP_t *b);

/*
 *   d - signed
 *   u - unsigned
 *   h - short
 *   l - long
 *   f - float
 *
 *   int8 -> hhd
 *  uint8 -> hhu
 *  int32 ->  d
 * uint32 ->  u
 *  int16 -> hd
 * uint16 -> hu
 *  int64 -> lld
 * uint64 -> llu
 */

SEXP_t *SEXP_number_newd (int n);
SEXP_t *SEXP_number_newhd (short int n);
SEXP_t *SEXP_number_newhhd (char n);
SEXP_t *SEXP_number_newld (long int n);
SEXP_t *SEXP_number_newlld (long long int n);

SEXP_t *SEXP_number_newu (unsigned int n);
SEXP_t *SEXP_number_newhu (unsigned short int n);
SEXP_t *SEXP_number_newhhu (unsigned char n);
SEXP_t *SEXP_number_newlu (unsigned long int n);
SEXP_t *SEXP_number_newllu (unsigned long long int n);

SEXP_t *SEXP_number_newf (double n);
SEXP_t *SEXP_number_newlf (long double n);

int           SEXP_number_getd (const SEXP_t *sexp);
short int     SEXP_number_gethd (const SEXP_t *sexp);
char          SEXP_number_gethhd (const SEXP_t *sexp);
long int      SEXP_number_getld (const SEXP_t *sexp);
long long int SEXP_number_getlld (const SEXP_t *sexp);

unsigned int       SEXP_number_getu (const SEXP_t *sexp);
unsigned short int SEXP_number_gethu (const SEXP_t *sexp);
unsigned char      SEXP_number_gethhu (const SEXP_t *sexp);
unsigned long int  SEXP_number_getlu (const SEXP_t *sexp);
unsigned long long SEXP_number_getllu (const SEXP_t *sexp);

double      SEXP_number_getf (const SEXP_t *sexp);
long double SEXP_number_getlf (const SEXP_t *sexp);

/*
#define SEXP_number_new(num)
#define SEXP_number_get(sexp)
*/

/* Functions for manipulating with strings */
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

/* Functions for manipulating with lists */
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

#define SEXP_list_foreach(var, list)                                    \
        for (register uint32_t ___i_ = 1;                               \
             ((var) = SEXP_list_nth ((list), ___i_)) != NULL;           \
             ++___i_)

#define SEXP_sublist_foreach(var, list, beg, end)                       \
        for (register uint32_t ___i_ = (beg);                           \
             ___i_ <= (uint32_t)(end) && ((var) = SEXP_list_nth ((list), ___i_)) != NULL; \
             ++___i_)

void SEXP_list_cb (SEXP_t *list, void (*fn) (SEXP_t *, void *), void *ptr);

/* Internal functions, don't use! */
SEXP_t *LIST_add (LIST_t *list, SEXP_t *sexp);

/* DON'T TOUCH THIS */
#define LIST_GROW_LOW_TRESH 5
#define LIST_GROW_LOW 3
#define LIST_GROW 1.2
#define LIST_GROW_ADD 16
#define LIST_INIT_SIZE 1

#include "xmalloc.h"

static inline LIST_t *LIST_init (LIST_t *list)
{
        _A(list != NULL);

        list->memb  = xmalloc (sizeof (SEXP_t) * LIST_INIT_SIZE);
        list->count = 0;
        list->size  = LIST_INIT_SIZE;
        
        return (list);
}

static inline LIST_t *LIST_new (void)
{
        LIST_t *list;

        list = xmalloc (sizeof (LIST_t));
        LIST_init (list);
        
        return (list);
}

/* Functions for manipulating with S-exp objects */
SEXP_t *SEXP_new  (void);
void    SEXP_init (SEXP_t *sexp);
void    SEXP_free (SEXP_t *sexp);

/* shallow copy */
SEXP_t *SEXP_dup (const SEXP_t *sexp);

/* deep copy */
SEXP_t *SEXP_deepdup (const SEXP_t *sexp);

/* shallow copy */
SEXP_t *SEXP_copy (SEXP_t *dst, const SEXP_t *src);

/* deep copy */
SEXP_t *SEXP_deepcopy (SEXP_t *dst, const SEXP_t *src);

size_t  SEXP_length (const SEXP_t *sexp);
int     SEXP_cmp (const SEXP_t *a, const SEXP_t *b);
int     SEXP_cmpobj (const SEXP_t *a, const SEXP_t *b);

/*
 *  Returns atomic type of the given S-exp as a string.
 */
const char *SEXP_strtype (const SEXP_t *sexp);

/*
 *  INTERNAL/EXPERIMENTAL stuff
 */

SEXP_t *SEXP_READV_int  (SEXP_t *sexp, int   *ptr);
SEXP_t *SEXP_READP_int  (SEXP_t *sexp, int  **ptr);
SEXP_t *SEXP_READV_char (SEXP_t *sexp, char  *ptr);
SEXP_t *SEXP_READP_char (SEXP_t *sexp, char **ptr);

#if defined(SEAP_TYPES_FILE)
/* Create type definitions */
# define STRUCT(name) struct name
# define VAR(type, name) type   name
# define PTR(type, name) type * name
#  include SEAP_TYPES_FILE
# undef  STRUCT
# undef  VAR
# undef  PTR

/* Create type functions */ 
# define STRUCT(name)                                                   \
        static inline void SEXPtoTYPE_struct_##name (SEXP_t *sexp, struct name * ptr)

# define VAR(type, name) sexp = SEXP_READV_##type (sexp, &(ptr->name))
# define PTR(type, name) sexp = SEXP_READP_##type (sexp, &(ptr->name))

#  include SEAP_TYPES_FILE
# undef  STRUCT
# undef  VAR
# undef  PTR

#define struct2sexp(type, ptr, sexp) SEXPtoTYPE_struct_##type (sexp, ptr)
#define sexp2struct(type, sexp, ptr) TYPEtoSEXP_struct_##type (ptr, sexp)

#endif /* SEAP_TYPES_FILE */

#endif /* SEAP_MANIP_H */
