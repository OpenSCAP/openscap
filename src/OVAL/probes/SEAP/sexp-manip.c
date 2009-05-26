#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <config.h>
#include "common.h"
#include "xmalloc.h"
#include "sexp-manip.h"

#ifndef _A
#define _A(x) assert(x)
#endif

SEXP_t *SEXP_number_new (const void *num, NUM_type_t type)
{
        SEXP_t *sexp;

        _A(num != NULL);

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_NUMBER);
   
        switch (type) {
        case NUM_INT8:
                NUM_STORE(int8_t, (*(int8_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_UINT8:
                NUM_STORE(uint8_t, (*(uint8_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_INT16:
                NUM_STORE(int16_t, (*(int16_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_UINT16:
                NUM_STORE(uint16_t, (*(uint16_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_INT32:
                NUM_STORE(int32_t, (*(int32_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_UINT32:
                NUM_STORE(uint32_t, (*(uint32_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_INT64:
                NUM_STORE(int64_t, (*(int64_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_UINT64:
                NUM_STORE(uint64_t, (*(uint64_t *)(num)), sexp->atom.number.nptr);
                break;
        case NUM_DOUBLE:
                NUM_STORE(double, (*(double *)(num)), sexp->atom.number.nptr);
                break;
        default:
                _D("Unsupported number type: %d\n", type);
                abort ();                
        }
   
        sexp->atom.number.type = type;
     
        return (sexp);
}

int SEXP_numberp (SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                return (SEXP_TYPE(sexp) == ATOM_NUMBER);
        }
        return (0);
}

size_t SEXP_number_size (SEXP_t *sexp)
{
        SEXP_VALIDATE(sexp);
        
        if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                switch (sexp->atom.number.type) {
                case NUM_INT8:
                        return sizeof (int8_t);
                case NUM_UINT8:
                        return sizeof (uint8_t);
                case NUM_INT16:
                        return sizeof (int16_t);
                case NUM_UINT16:
                        return sizeof (uint16_t);
                case NUM_INT32:
                        return sizeof (int32_t);
                case NUM_UINT32:
                        return sizeof (uint32_t);
                case NUM_INT64:
                        return sizeof (int64_t);
                case NUM_UINT64:
                        return sizeof (uint64_t);
                case NUM_DOUBLE:
                        return sizeof (double);
                default:
                        _D("Unsupported number type: %d\n", sexp->atom.number.type);
                        abort ();
                }
        }
        return (0);
}

int SEXP_number_get (SEXP_t *sexp, void *ptr, NUM_type_t type)
{
        _A(ptr != NULL);
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_NUMBER) {
                        if (type >= sexp->atom.number.type) {
                                switch (sexp->atom.number.type) {
                                case NUM_INT8:
                                        switch (type) {
                                        case NUM_INT8:
                                                *((int8_t *)ptr)   = (int8_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT8:
                                                *((uint8_t *)ptr)  = (uint8_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT16:
                                                *((int16_t *)ptr)  = (int16_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT16:
                                                *((uint16_t *)ptr) = (uint16_t)NUM(int8_t, sexp->atom.number.nptr);                                        
                                                break;
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(int8_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_UINT8:
                                        switch (type) {
                                        case NUM_INT8:
                                                *((int8_t *)ptr)   = (int8_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT8:
                                                *((uint8_t *)ptr)  = (uint8_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT16:
                                                *((int16_t *)ptr)  = (int16_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT16:
                                                *((uint16_t *)ptr) = (uint16_t)NUM(uint8_t, sexp->atom.number.nptr);                                        
                                                break;
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint8_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_INT16:
                                        switch (type) {
                                        case NUM_INT16:
                                                *((int16_t *)ptr)  = (int16_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT16:
                                                *((uint16_t *)ptr) = (uint16_t)NUM(int16_t, sexp->atom.number.nptr);                                        
                                                break;
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(int16_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_UINT16:
                                        switch (type) {
                                        case NUM_INT16:
                                                *((int16_t *)ptr)  = (int16_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT16:
                                                *((uint16_t *)ptr) = (uint16_t)NUM(uint16_t, sexp->atom.number.nptr);                                        
                                                break;
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint16_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_INT32:
                                        switch (type) {
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(int32_t, sexp->atom.number.nptr);
                                                break;
                                        default:
                                                abort ();
                                        }
                                        return (0);
                                case NUM_UINT32:
                                        switch (type) {
                                        case NUM_INT32:
                                                *((int32_t *)ptr)  = (int32_t)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT32:
                                                *((uint32_t *)ptr) = (uint32_t)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint32_t, sexp->atom.number.nptr);
                                                break;
                                        }
                                        return (0);
                                case NUM_INT64:
                                        switch (type) {
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(int64_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(int64_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(int64_t, sexp->atom.number.nptr);
                                                break;
                                        }
                                        return (0);
                                case NUM_UINT64:
                                        switch (type) {
                                        case NUM_INT64:
                                                *((int64_t *)ptr)  = (int64_t)NUM(uint64_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_UINT64:
                                                *((uint64_t *)ptr) = (uint64_t)NUM(uint64_t, sexp->atom.number.nptr);
                                                break;
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint64_t, sexp->atom.number.nptr);
                                                break;
                                        }
                                        return (0);
                                case NUM_DOUBLE:
                                        switch (type) {
                                        case NUM_DOUBLE:
                                                *((double *)ptr)   = (double)NUM(uint64_t, sexp->atom.number.nptr);
                                                break;
                                        }
                                        return (0);
                                }
                                return (0);
                        }
                } else {
                        /* not a number */
                        errno = EINVAL;
                        return (1);
                }
        } else {
                /* NULL */
                errno = EINVAL;
                return (1);
        }
        
        return (0);
}

SEXP_t *SEXP_string_new (const void *str, size_t len)
{
        SEXP_t *sexp;

        _A(str != NULL);
        /* _A(len > 0); */
        
        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_STRING);
        sexp->atom.string.str = xmemdup (str, len);
        sexp->atom.string.len = len;
        
        return (sexp);
}

SEXP_t *SEXP_string_newf (const char *fmt, ...) {
        va_list ap;
        char *str = NULL;
        SEXP_t *sexp = NULL;
        
        va_start (ap, fmt);
        if (vasprintf (&str, fmt, ap) != -1) {
                sexp = SEXP_new ();
                SEXP_SETTYPE(sexp, ATOM_STRING);
                sexp->atom.string.str = str;
                sexp->atom.string.len = strlen (str);
        }
        va_end (ap);
        
        return (sexp);
}

int SEXP_strcmp (SEXP_t *sexp, const char *str)
{
        _A(str != NULL);
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);

                if (SEXP_TYPE(sexp) == ATOM_STRING) {
                        return strncmp (sexp->atom.string.str, str, sexp->atom.string.len);
                } else {
                        return (1);
                }
        } else {
                return (-1);
        }
}

int SEXP_strncmp (SEXP_t *sexp, const char *str, size_t n)
{
        _A(str != NULL);
        _A(n > 0);
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_STRING) {
                        return (n < sexp->atom.string.len ?
                                strncmp (sexp->atom.string.str, str, n):
                                strncmp (sexp->atom.string.str, str, sexp->atom.string.len));
                } else {
                        return (1);
                }
        } else {
                return (-1);
        }
}

int SEXP_strncoll (SEXP_t *sexp, const char *str, size_t n)
{
        _A(str != NULL);
        _A(n > 0);
        
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                
                if (SEXP_TYPE(sexp) == ATOM_STRING) {
                        return xstrncoll (sexp->atom.string.str,
                                          sexp->atom.string.len,
                                          str, n);
                } else {
                        return (1);
                }
        } else {
                /* NULL < str */
                return (-1);
        }
}

int SEXP_stringp (SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                return (SEXP_TYPE(sexp) == ATOM_STRING);
        }
        return (0);
}

char *SEXP_string_cstr (SEXP_t *sexp)
{
        char *str;
        size_t  i;

        SEXP_VALIDATE(sexp);

        if (SEXP_TYPE(sexp) == ATOM_STRING) {
                str = xmalloc (sizeof (char) * (sexp->atom.string.len + 1));
                
                for (i = 0; (i < sexp->atom.string.len &&
                             isprint(sexp->atom.string.str[i])); ++i)
                {
                        str[i] = sexp->atom.string.str[i];
                }
                
                str[i] = '\0';

                if (i < sexp->atom.string.len)
                        return xrealloc (str, sizeof (char) * (i + 1));
                else
                        return (str);
        } else {
                return (NULL);
        }
}

SEXP_t *SEXP_list_new (void)
{
        SEXP_t *sexp;

        sexp = SEXP_new ();
        SEXP_SETTYPE(sexp, ATOM_LIST);
        sexp->atom.list.memb  = xmalloc (sizeof (SEXP_t) * LIST_INIT_SIZE);
        sexp->atom.list.count = 0;
        sexp->atom.list.size  = LIST_INIT_SIZE;
        
        return (sexp);
}

SEXP_t *SEXP_list_init (SEXP_t *sexp)
{
        _A(sexp != NULL);

        SEXP_VALIDATE(sexp);
        SEXP_SETTYPE(sexp, ATOM_LIST);
        LIST_init (&(sexp->atom.list));
        
        return (sexp);
}

SEXP_t *SEXP_list_free (SEXP_t *sexp)
{
        SEXP_free (sexp);
        return (NULL);
}

SEXP_t *LIST_add (LIST_t *list, SEXP_t *sexp)
{
        _A(list != NULL);
        _A(list->count <= list->size);
        _A(list->size > 0);

        SEXP_VALIDATE(sexp);
        
        if (list->count >= list->size) {
                list->size += LIST_GROW_ADD;
                list->memb  = xrealloc (list->memb, sizeof (SEXP_t) * list->size);
        }
        
        memcpy (SEXP(list->memb) + list->count, sexp, sizeof (SEXP_t));
        
        ++(list->count);
        
        if (SEXP_FREE(sexp)) {
                xfree ((void **)(&sexp));
        }
        
        return (SEXP(list->memb) + list->count - 1);
}

SEXP_t *SEXP_list_add (SEXP_t *list, SEXP_t *sexp)
{
        SEXP_VALIDATE(sexp);
        
        if (SEXP_listp (list)) {
                return LIST_add (&(list->atom.list), sexp);
        } else {
                return (NULL);
        }
}

SEXP_t *SEXP_list_first (SEXP_t *sexp)
{
        SEXP_VALIDATE(sexp);
        
        if (SEXP_TYPE(sexp) == ATOM_LIST) {
                if (sexp->atom.list.count > 0) {
                        return (&(SEXP(sexp->atom.list.memb)[0]));
                } else {
                        return SEXP_new ();
                }
        } else {
                return (NULL);
        }
}

SEXP_t *SEXP_list_last (SEXP_t *sexp)
{
        SEXP_VALIDATE(sexp);
        
        if (SEXP_TYPE(sexp) == ATOM_LIST) {
                if (sexp->atom.list.count > 0) {
                        return (&(SEXP(sexp->atom.list.memb)[sexp->atom.list.count - 1]));
                } else {
                        return SEXP_new ();
                }
        } else {
                return (NULL);
        }       
}

int SEXP_listp (SEXP_t *sexp)
{
        if (sexp != NULL) {
                SEXP_VALIDATE(sexp);
                return (SEXP_TYPE(sexp) == ATOM_LIST);
        }
        return (0);
}

SEXP_t *SEXP_list_pop (SEXP_t **sexp)
{
        SEXP_t *sexp_ret;

        _A(sexp != NULL);
        _A(*sexp != NULL);
        
        if (SEXP_TYPE(*sexp) == ATOM_LIST) {
                if ((*sexp)->atom.list.count > 0) {
                        sexp_ret = SEXP_dup(&(SEXP((*sexp)->atom.list.memb)[0]));
                        
                        if ((*sexp)->atom.list.count > 1) {
                                --(*sexp)->atom.list.count;
                                
                                memmove (SEXP((*sexp)->atom.list.memb),
                                         SEXP((*sexp)->atom.list.memb) + 1,
                                         sizeof (SEXP_t) * ((*sexp)->atom.list.count));
                                
                                (*sexp)->atom.list.memb = xrealloc ((*sexp)->atom.list.memb,
                                                                    sizeof (SEXP_t) * ((*sexp)->atom.list.count));
                        } else {
                                xfree (&((*sexp)->atom.list.memb));
                                
                                if (SEXP_FREE(*sexp))
                                        xfree ((void **)sexp);
                                
                                *sexp = NULL;
                        }
                        return (sexp_ret);
                } else {
                        if (SEXP_FREE(*sexp))
                                xfree ((void **)sexp);
                        
                        *sexp = NULL;

                        return (NULL);
                }
        } else {
                return (NULL);
        }
}

SEXP_t *SEXP_list_nth (SEXP_t *sexp, uint32_t n)
{
        _A(sexp != NULL);
        _A(n > 0);
        
        if (SEXP_TYPE(sexp) == ATOM_LIST && n > 0) {
                _D("nth: %u, %p\n", n, sexp);
                
                if (n <= sexp->atom.list.count) {
                        _D("nth: %u, %p, %s\n",
                           n, &(SEXP(sexp->atom.list.memb)[n - 1]),
                           SEXP_strtype (&(SEXP(sexp->atom.list.memb)[n - 1])));
                        
                        return &(SEXP(sexp->atom.list.memb)[n - 1]);
                }
        }
        
        return (NULL);
}

SEXP_t *SEXP_list_nth_copy (SEXP_t *sexp, uint32_t n)
{
        _A(sexp != NULL);
        _A(n > 0);
        
        if (SEXP_listp (sexp) && n > 0) {
                if (sexp->atom.list.count <= n)
                        return SEXP_deepdup(&(SEXP(sexp->atom.list.memb)[n - 1]));
        }
        
        return (NULL);
}

SEXP_t *SEXP_list_join (SEXP_t *a, SEXP_t *b)
{
        SEXP_VALIDATE(a);
        SEXP_VALIDATE(b);
        
        if (SEXP_listp (a) && SEXP_listp (b)) {
                if (a->atom.list.size < b->atom.list.count) {
                        a->atom.list.size = a->atom.list.count + b->atom.list.count;
                        a->atom.list.memb = xrealloc (a->atom.list.memb,
                                                      sizeof (SEXP_t) * (a->atom.list.count + b->atom.list.count));
                        
                }
                
                memcpy (SEXP(a->atom.list.memb) + a->atom.list.count,
                        b->atom.list.memb, sizeof (SEXP_t) * b->atom.list.count);
                
                a->atom.list.count += b->atom.list.count;
                
                return (a);
        } else {
                return (NULL);
        }
}

SEXP_t *SEXP_new  (void)
{
        SEXP_t *sexp;

        sexp = xmalloc (sizeof (SEXP_t));
        sexp->flags = 0;
        SEXP_SETTYPE(sexp, ATOM_EMPTY);
        SEXP_SETFLAG(sexp, SEXP_FLAGFREE);
        sexp->handler = NULL;
#if !defined(NDEBUG)
        sexp->__magic0 = SEXP_MAGIC0;
        sexp->__magic1 = SEXP_MAGIC1;
#endif
        return (sexp);
}

void SEXP_init (SEXP_t *sexp)
{
        _A(sexp != NULL);
        
        sexp->flags = 0;
        SEXP_SETTYPE(sexp, ATOM_EMPTY);
        sexp->handler = NULL;
#if !defined(NDEBUG)
        sexp->__magic0 = SEXP_MAGIC0;
        sexp->__magic1 = SEXP_MAGIC1;
#endif
        return;
}

void SEXP_free (SEXP_t *sexp)
{
        _A(sexp != NULL);
        
        SEXP_VALIDATE(sexp);
        
        switch (SEXP_TYPE(sexp)) {
        case ATOM_LIST:
        {
                uint32_t i;
                
                for (i = 0; i < (sexp->atom.list.count); ++i) {
                        SEXP_free (&SEXP(sexp->atom.list.memb)[i]);
                }
                
                if (sexp->atom.list.size > 0) {
                        xfree ((void **)&(sexp->atom.list.memb));
                }
                
        } break;
        case ATOM_NUMBER:
        {
                uint16_t sz;
                
                switch (sexp->atom.number.type) {
                case NUM_INT8:   sz = sizeof (int8_t);
                        break;
                case NUM_UINT8:  sz = sizeof (uint8_t);
                        break;
                case NUM_INT16:  sz = sizeof (int16_t);
                        break;
                case NUM_UINT16: sz = sizeof (uint16_t);
                        break;
                case NUM_INT32:  sz = sizeof (int32_t);
                        break;
                case NUM_UINT32: sz = sizeof (uint32_t);
                        break;
                case NUM_INT64:  sz = sizeof (int64_t);
                        break;
                case NUM_UINT64: sz = sizeof (uint64_t);
                        break;
                case NUM_DOUBLE: sz = sizeof (double);
                        break;
                case NUM_NONE:
                case NUM_FRACT: /* Not implemented */
                case NUM_BIGNUM: /* Not implemented */
                        sz = 0;
                        break;
                default:
                        abort ();
                }

                if (sz > VOIDPTR_SIZE)
                        xfree ((void **)&(sexp->atom.number.nptr));
                
        } break;
        case ATOM_STRING:
 
                if (sexp->atom.string.len > 0)
                        xfree ((void **)&(sexp->atom.string.str));
 
                break;
        case ATOM_UNFIN:
        case ATOM_EMPTY:
        case ATOM_INVAL:
                break;
        default:
                abort ();
        }

#if !defined(NDEBUG) || defined(VALIDATE_SEXP)
        memset (sexp, 0, sizeof (SEXP_t));
        sexp->__magic0 = SEXP_MAGIC0_INV;
        sexp->__magic1 = SEXP_MAGIC1_INV;
#endif

        if (SEXP_FREE(sexp))
                xfree ((void **)(sexp));
        
        return;
}

SEXP_t *SEXP_deepdup (SEXP_t *sexp)
{
        SEXP_t *copy;
        
        SEXP_VALIDATE(sexp);

        copy = SEXP_new ();
        copy->flags = sexp->flags;
        copy->handler = sexp->handler;
        
        switch (SEXP_TYPE(sexp)) {
        case ATOM_UNFIN:
        case ATOM_EMPTY:
                break;
        case ATOM_NUMBER:
                
                break;
        case ATOM_LIST:

                break;
        case ATOM_STRING:

                break;
        default:
                abort ();
        }
        
        return (copy);
}

SEXP_t *SEXP_dup (SEXP_t *sexp)
{
        SEXP_t *copy;
        
        SEXP_VALIDATE(sexp);

        copy = SEXP_new ();
        copy->flags   = sexp->flags;
        copy->handler = sexp->handler;
        memcpy (&(copy->atom), &(sexp->atom), sizeof sexp->atom);
        
        return (copy);
}

SEXP_t *SEXP_copy (SEXP_t *dst, SEXP_t *src)
{
        SEXP_VALIDATE(src);
        return memcpy (dst, src, sizeof (SEXP_t));
}

SEXP_t *SEXP_deepcopy (SEXP_t *dst, SEXP_t *src)
{
        SEXP_VALIDATE(src);
        return (NULL);
}

size_t SEXP_length (SEXP_t *sexp)
{
        switch (SEXP_TYPE(sexp)) {
        case ATOM_LIST:
                return ((size_t)(sexp->atom.list.count));
        case ATOM_STRING:
                return ((size_t)(sexp->atom.string.len));
        case ATOM_NUMBER:
        default:
                return (0);
        }
}

const char *__sexp_strtype[] = {
        "unfinished",
        "list",
        "number",
        "symbol",
        "string",
        "binary",
        "empty"
};

const char *SEXP_strtype (const SEXP_t *sexp)
{
        if (sexp != NULL) {
                _A(SEXP_TYPE(sexp) >= 0);
                return (SEXP_TYPE(sexp) < 0x06 ? __sexp_strtype[SEXP_TYPE(sexp)] : "unknown");
        } else {
                return ("(null)");
        }
}
