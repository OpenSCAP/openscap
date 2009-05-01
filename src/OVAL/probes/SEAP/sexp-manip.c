#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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
        
        return (sexp);
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

int SEXP_strcmp (SEXP_t *sexp, const char *str)
{
        _A(str != NULL);
        
        if (sexp != NULL) {
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
        return (SEXP_TYPE(sexp) == ATOM_STRING);
}

char *SEXP_string_cstr (SEXP_t *sexp)
{
        _A(sexp != NULL);
        char *str;
        size_t  i;

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

        sexp = xmalloc (sizeof (SEXP_t));
        SEXP_SETTYPE(sexp, ATOM_LIST);
        SEXP_SETFLAG(sexp, SEXP_FLAGFREE);
        sexp->atom.list.memb  = xmalloc (sizeof (SEXP_t) * LIST_INIT_SIZE);
        sexp->atom.list.count = 0;
        sexp->atom.list.size  = LIST_INIT_SIZE;
        
        return (sexp);
}

SEXP_t *SEXP_list_init (SEXP_t *sexp)
{
        _A(sexp != NULL);

        SEXP_SETTYPE(sexp, ATOM_LIST);
        LIST_init (&(sexp->atom.list));
        
        return (sexp);
}

SEXP_t *SEXP_list_free (SEXP_t *sexp)
{
        /* FIXME */
        return (NULL);
}

SEXP_t *LIST_add (LIST_t *list, SEXP_t *sexp)
{
        _A(list != NULL);
        _A(list->count <= list->size);
        _A(list->size > 0);
        
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
        if (SEXP_listp (list)) {
                return LIST_add (&(list->atom.list), sexp);
        } else {
                return (NULL);
        }
}

SEXP_t *SEXP_list_first (SEXP_t *sexp)
{
        SEXP_t *empty;

        _A(sexp != NULL);
        if (SEXP_TYPE(sexp) == ATOM_LIST) {
                if (sexp->atom.list.count > 0) {
                        return (&(SEXP(sexp->atom.list.memb)[0]));
                } else {
                        empty = SEXP_new ();
                        SEXP_SETTYPE(sexp, ATOM_EMPTY);
                        return (empty);
                }
        } else {
                return (NULL);
        }
}

SEXP_t *SEXP_list_last (SEXP_t *sexp)
{
        SEXP_t *empty;
        
        _A(sexp != NULL);
        if (SEXP_TYPE(sexp) == ATOM_LIST) {
                if (sexp->atom.list.count > 0) {
                        return (&(SEXP(sexp->atom.list.memb)[sexp->atom.list.count - 1]));
                } else {
                        empty = SEXP_new ();
                        SEXP_SETTYPE(empty, ATOM_EMPTY); /* FIXME: this is redundant */
                        return (empty);
                }
        } else {
                return (NULL);
        }       
}

int SEXP_listp (SEXP_t *sexp)
{
        return (sexp != NULL ? (SEXP_TYPE(sexp) == ATOM_LIST) : 0);
}

SEXP_t *SEXP_list_pop (SEXP_t **sexp)
{
        SEXP_t *sexp_ret;

        _A(sexp != NULL);
        _A(*sexp != NULL);
        
        if (SEXP_TYPE(*sexp) == ATOM_LIST) {
                if ((*sexp)->atom.list.count > 0) {
                        sexp_ret = SEXP_copyobj(&(SEXP((*sexp)->atom.list.memb)[0]));
                        
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
        
        if (SEXP_listp (sexp) && n > 0) {
                if (sexp->atom.list.count <= n)
                        return &(SEXP(sexp->atom.list.memb)[n - 1]);
        }
        
        return (NULL);
}

SEXP_t *SEXP_list_nth_copy (SEXP_t *sexp, uint32_t n)
{
        _A(sexp != NULL);
        _A(n > 0);
        
        if (SEXP_listp (sexp) && n > 0) {
                if (sexp->atom.list.count <= n)
                        return SEXP_copy(&(SEXP(sexp->atom.list.memb)[n - 1]));
        }
        
        return (NULL);
}

SEXP_t *SEXP_new  (void)
{
        SEXP_t *sexp;

        sexp = xmalloc (sizeof (SEXP_t));
        sexp->flags = 0;
        SEXP_SETTYPE(sexp, ATOM_EMPTY);
        SEXP_SETFLAG(sexp, SEXP_FLAGFREE);
        sexp->handler = NULL;
        
        return (sexp);
}

void SEXP_init (SEXP_t  *sexp)
{
        _A(sexp != NULL);
        
        sexp->flags = 0;
        SEXP_SETTYPE(sexp, ATOM_EMPTY);
        sexp->handler = NULL;
        return;
}

void SEXP_free (SEXP_t **sexpp)
{
        _A(sexpp  != NULL);
        /* _A(*sexpp != NULL); */
        
#warning "FIXME: SEXP_free not implemented" /* FIXME */
        
        if (*sexpp == NULL) {
                _D("WARNING: NULL free\n");
                return;
        }
        
        switch (SEXP_TYPE(*sexpp)) {
        case ATOM_LIST:
        case ATOM_NUMBER:
        case ATOM_STRING:
        case ATOM_UNFIN:
        case ATOM_EMPTY:
                break;
        default:
                abort ();
        }

        if (SEXP_FREE(*sexpp))
                xfree ((void **)(sexpp));
        
        return;
}

SEXP_t *SEXP_copy (SEXP_t *sexp)
{
        SEXP_t *copy;
        
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

SEXP_t *SEXP_copyobj (SEXP_t *sexp)
{
        SEXP_t *copy;
        
        copy = SEXP_new ();
        copy->flags   = sexp->flags;
        copy->handler = sexp->handler;
        memcpy (&(copy->atom), &(sexp->atom), sizeof sexp->atom);
        
        return (copy);
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

const char *SEAP_sexp_strtype (const SEXP_t *sexp)
{
        _A(SEXP_TYPE(sexp) >= 0);
        return (SEXP_TYPE(sexp) < 0x06 ? __sexp_strtype[SEXP_TYPE(sexp)] : "unknown");
}
