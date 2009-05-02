#pragma once
#ifndef SEXP_MANIP_H
#define SEXP_MANIP_H

#include <assert.h>
#ifndef _A
#define _A(x) assert(x)
#endif

#include <stdint.h>
#include "sexp.h"

#define SEXP(ptr)      ((SEXP_t *)(ptr))
#define SEXP_TYPE(ptr) ((SEXP(ptr)->flags) & SEXP_TYPEMASK)

/*
 *  SEXP_FREE returns the value of SEXP_FLAGFREE bit.
 */
#define SEXP_FREE(ptr) (((SEXP(ptr)->flags) & SEXP_FLAGFREE) == SEXP_FLAGFREE)

static inline void SEXP_SETTYPE(SEXP_t *sexp, ATOM_type_t type)
{
        sexp->flags = (sexp->flags & SEXP_FLAGMASK) | (type & SEXP_TYPEMASK);
        return;
}

static inline void SEXP_SETFLAG(SEXP_t *sexp, ATOM_flags_t flag)
{
        sexp->flags |= flag & SEXP_FLAGMASK;
        return;
}

#define VOIDPTR_SIZE (sizeof (void *))

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
int     SEXP_numberp (SEXP_t *sexp);
size_t  SEXP_number_size (SEXP_t *sexp);
void    SEXP_number_get (SEXP_t *sexp, void *ptr, NUM_type_t type);

/* Functions for manipulating with strings */
SEXP_t *SEXP_string_new (const void *, size_t);
int     SEXP_strcmp (SEXP_t *sexp, const char *str);
int     SEXP_strncmp (SEXP_t *sexp, const char *str, size_t n);
int     SEXP_strncoll (SEXP_t *sexp, const char *str, size_t n);
int     SEXP_stringp (SEXP_t *sexp);
char   *SEXP_string_cstr (SEXP_t *sexp);

/* Functions for manipulating with lists */
SEXP_t *SEXP_list_new (void);
SEXP_t *SEXP_list_init (SEXP_t *);
SEXP_t *SEXP_list_free (SEXP_t *);
SEXP_t *SEXP_list_add (SEXP_t *, SEXP_t *);
SEXP_t *SEXP_list_first (SEXP_t *);
SEXP_t *SEXP_list_last (SEXP_t *);
int     SEXP_listp (SEXP_t *);
SEXP_t *SEXP_list_pop (SEXP_t **);
SEXP_t *SEXP_list_nth (SEXP_t *, uint32_t);
SEXP_t *SEXP_list_nth_copy (SEXP_t *sexp, uint32_t n);

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
void    SEXP_free (SEXP_t **sexpp);
SEXP_t *SEXP_copy (SEXP_t *sexp);
SEXP_t *SEXP_copyobj (SEXP_t *sexp);
size_t  SEXP_length (SEXP_t *sexp);

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
