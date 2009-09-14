#pragma once
#ifndef PROBE_API_H
#define PROBE_API_H

#include <seap.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "api/oval_definitions.h"

/*
 * items
 */

SEXP_t *probe_item_build (const char *fmt, ...);
SEXP_t *probe_item_creat (const char *name, SEXP_t *attrs, ...);
SEXP_t *probe_item_new   (const char *name, SEXP_t *attrs);

SEXP_t *probe_item_attr_add ();
SEXP_t *probe_item_ent_add ();

/*
 * objects
 */

SEXP_t *probe_obj_build (const char *fmt, ...);
SEXP_t *probe_obj_creat (const char *name, SEXP_t *attrs, ...);
SEXP_t *probe_obj_new   (const char *name, SEXP_t *attrs);

SEXP_t *probe_obj_getent (SEXP_t *obj, const char *name, uint32_t n);
int     probe_obj_getentval (SEXP_t *obj, const char *name, uint32_t n, SEXP_t **res);

SEXP_t *probe_obj_getattrval (SEXP_t *obj, const char *name);
bool    probe_obj_attrexists (SEXP_t *obj, const char *name);

int probe_obj_setstatus (SEXP_t *obj, int status);
int probe_obj_setentstatus (SEXP_t *obj, const char *name, uint32_t n, int status);

/*
 * entities
 */

int     probe_ent_getval (SEXP_t *ent, SEXP_t **res);
SEXP_t *probe_ent_getattrval (SEXP_t *ent, const char *name);
bool    probe_ent_attrexists (SEXP_t *ent, const char *name);

typedef int oval_datatype_t;

oval_datatype_t probe_ent_setdatatype (SEXP_t *ent);
oval_datatype_t probe_ent_getdatatype (SEXP_t *ent);

int  probe_ent_setmask (SEXP_t *ent, bool mask);
bool probe_ent_getmask (SEXP_t *ent);

int probe_ent_setstatus (SEXP_t *ent, int status);
int probe_ent_getstatus (SEXP_t *ent);

char *probe_ent_getname   (const SEXP_t *ent);
char *probe_ent_getname_r (const SEXP_t *ent, char *buffer, size_t buflen);

#define PROBE_EINVAL     1 /* Invalid type/value/format */
#define PROBE_ENOELM     2 /* Missing element */
#define PROBE_ENOVAL     3 /* Missing value */
#define PROBE_ENOATTR    4 /* Missing attribute */
#define PROBE_EINIT      5 /* Initialization failed */
#define PROBE_ENOMEM     6 /* No memory */
#define PROBE_EOPNOTSUPP 7 /* Not supported */
#define PROBE_ERANGE     8 /* Out of range */
#define PROBE_EDOM       9 /* Out of domain */
#define PROBE_EFAULT    10 /* Memory fault/NULL value */
#define PROBE_EACCES    11 /* Operation not perimitted */
#define PROBE_EUNKNOWN 255 /* Unknown/Unexpected error */

#define OVAL_STATUS_ERROR        1
#define OVAL_STATUS_EXISTS       2
#define OVAL_STATUS_DOESNOTEXIST 3
#define OVAL_STATUS_NOTCOLLECTED 4

#define PROBECMD_STE_FETCH 1
#define PROBECMD_OBJ_EVAL  2

#endif /* PROBE_API_H */
