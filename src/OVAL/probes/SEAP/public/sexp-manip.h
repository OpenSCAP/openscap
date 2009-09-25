#ifndef __STUB_PROBE
#pragma once
#ifndef SEXP_MANIP_H
#define SEXP_MANIP_H

#include <stdint.h>
#include <stdbool.h>
#include <sexp-types.h>

/*
 * number
 */

SEXP_t *SEXP_number_new (SEXP_numtype_t t, const void *n);
SEXP_t *SEXP_number_newb (bool n);
SEXP_t *SEXP_number_newi_8  (int8_t n);
SEXP_t *SEXP_number_newu_8  (uint8_t n);
SEXP_t *SEXP_number_newi_16 (int16_t n);
SEXP_t *SEXP_number_newu_16 (uint16_t n);
#define SEXP_number_newi SEXP_number_newi_32
SEXP_t *SEXP_number_newi_32 (int32_t n);
int32_t SEXP_number_geti_32 (const SEXP_t *s_exp);
#define SEXP_number_newu SEXP_number_newu_32
SEXP_t *SEXP_number_newu_32 (uint32_t n);
uint32_t SEXP_number_getu_32 (const SEXP_t *s_exp);
SEXP_t *SEXP_number_newi_64 (int64_t n);
int64_t SEXP_number_geti_64 (const SEXP_t *s_exp);
SEXP_t *SEXP_number_newu_64 (uint64_t n);
uint64_t SEXP_number_getu_64 (const SEXP_t *s_exp);
SEXP_t *SEXP_number_newf (double n);
double  SEXP_number_getf (const SEXP_t *s_exp);

int SEXP_number_get (SEXP_t *s_exp, void *dst, SEXP_numtype_t type);

uint16_t SEXP_number_getu_16 (const SEXP_t *s_exp);

void    SEXP_number_free (SEXP_t *s_exp);
bool    SEXP_numberp (const SEXP_t *s_exp);
SEXP_numtype_t SEXP_number_type (const SEXP_t *sexp);

/*
 * string
 */

SEXP_t *SEXP_string_new  (const void *string, size_t strlen);
SEXP_t *SEXP_string_newf (const char *format, ...);
void SEXP_string_free (SEXP_t *s_exp);
bool SEXP_stringp (const SEXP_t *s_exp);
size_t SEXP_string_length (const SEXP_t *s_exp);

int SEXP_strcmp (SEXP_t *s_exp, const char *str);
int SEXP_strncmp (SEXP_t *s_exp, const char *str, size_t n);
char *SEXP_string_cstr (SEXP_t *s_exp);
char *SEXP_string_cstr_r (SEXP_t *s_exp, char *buf, size_t len);
char *SEXP_string_cstrp (const SEXP_t *s_exp);

char *SEXP_string_subcstr (SEXP_t *s_exp, size_t beg, size_t len);
int SEXP_string_cmp (const SEXP_t *str_a, const SEXP_t *str_b);

/*
 * list
 */

SEXP_t *SEXP_list_new (SEXP_t *memb, ...);
void    SEXP_list_free (SEXP_t *s_exp);
bool    SEXP_listp (const SEXP_t *s_exp);
size_t  SEXP_list_length (const SEXP_t *s_exp);
SEXP_t *SEXP_list_first (const SEXP_t *list);
SEXP_t *SEXP_list_rest  (const SEXP_t *list);
SEXP_t *SEXP_list_last (const SEXP_t *list);
SEXP_t *SEXP_list_nth (const SEXP_t *list, uint32_t n);
SEXP_t *SEXP_list_add (SEXP_t *list, SEXP_t *s_exp);
SEXP_t *SEXP_list_join (const SEXP_t *list_a, const SEXP_t *list_b);
SEXP_t *SEXP_list_push (SEXP_t *list, SEXP_t *s_exp);
SEXP_t *SEXP_list_pop (SEXP_t *list);

SEXP_t *SEXP_listref_first (const SEXP_t *list);
SEXP_t *SEXP_listref_rest (const SEXP_t *list);
SEXP_t *SEXP_listref_last (const SEXP_t *list);
SEXP_t *SEXP_listref_nth (const SEXP_t *list, uint32_t n);

typedef struct SEXP_it SEXP_it_t;

#define SEXP_IT_RECURSIVE 0x01
#define SEXP_IT_HARDREF   0x02

SEXP_it_t *SEXP_listit_new (const SEXP_t *list, int flags);
SEXP_t    *SEXP_listit_next(SEXP_it_t *it);
SEXP_t    *SEXP_listit_prev (SEXP_it_t *it);
SEXP_t    *SEXP_listit_length (SEXP_it_t *it);
SEXP_t    *SEXP_listit_seek (SEXP_it_t *it, uint32_t n);
void       SEXP_listit_free (SEXP_it_t *it);

#if __STDC_VERSION__ >= 199901L
# include <sys/cdefs.h>
# define __XC(a,b) __CONCAT(a,b)

/* TODO: use alloca & softref_r here */
#define SEXP_list_foreach(var, list) for (;0;)
#define SEXP_sublist_foreach(var, list, beg, end) for (;0;)

#endif /* __STDC_VERSION__ >= 199901L */

/*
 * generic 
 */

SEXP_t *SEXP_new (void);
SEXP_t *SEXP_ref (const SEXP_t *s_exp);
SEXP_t *SEXP_softref (const SEXP_t *s_exp);
void    SEXP_free (SEXP_t *s_exp);

const char *SEXP_datatype (const SEXP_t *s_exp);
int         SEXP_datatype_set (SEXP_t *s_exp, const char *name);
SEXP_type_t SEXP_typeof (const SEXP_t *s_exp);
const char *SEXP_strtype (const SEXP_t *s_exp);

#define SEXP_VALIDATE(s) __SEXP_VALIDATE(s, __FILE__, __LINE__, __PRETTY_FUNCTION__)

static void __SEXP_VALIDATE(const SEXP_t *s_exp, const char *file, uint32_t line, const char *func)
{
        return;
}

#endif /* SEXP_MANIP_H */
#endif
