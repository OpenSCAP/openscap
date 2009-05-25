#pragma once
#ifndef PROBE_H
#define PROBE_H

#include <stdarg.h>
#include "api/oval_definitions.h"

SEXP_t *SEXP_OVALattr_create (const char *name, ...);
SEXP_t *SEXP_OVALelm_create (const char *name, ...);
SEXP_t *SEXP_OVALobj_create (const char *obj_name, SEXP_t *obj_attrs, ...);

SEXP_t *SEXP_OVALobj_attr_add (SEXP_t *obj, const char *name, SEXP_t *value);
SEXP_t *SEXP_OVALobj_attr_del (SEXP_t *obj, const char *name);

SEXP_t *SEXP_OVALobj_elm_add (SEXP_t *obj, const char *name, SEXP_t *attrs, SEXP_t *value);
SEXP_t *SEXP_OVALobj_elm_del (SEXP_t *obj, const char *name);

SEXP_t *SEXP_OVALobj_elmattr_add (SEXP_t *obj, const char *elm_name, const char *attr_name, SEXP_t *value);
SEXP_t *SEXP_OVALobj_elmattr_del (SEXP_t *obj, const char *elm_name, const char *attr_name);

SEXP_t *SEXP_OVALelm_attr_add (SEXP_t *elm, const char *name, SEXP_t *value);
SEXP_t *SEXP_OVALelm_attr_del (SEXP_t *elm, const char *name);

#define SEXP_OVALobj_getelement_value SEXP_OVALobj_getelm_val
char *SEXP_OVALobj_getelm_val (SEXP_t *obj, const char *name);

int     SEXP_OVALobj_validate (SEXP_t *obj);
SEXP_t *SEXP_OVALobj_getelm (SEXP_t *obj, const char *name);
SEXP_t *SEXP_OVALobj_getelmval (SEXP_t *obj, const char *name);
SEXP_t *SEXP_OVALelm_getval (SEXP_t *elm);
SEXP_t *SEXP_OVALelm_getattrval (SEXP_t *elm, const char *name);
int     SEXP_OVALelm_hasattr (SEXP_t *elm, const char *name);

SEXP_t *oval_object_to_sexp (const char *typestr, struct oval_object *object);

#endif /* PROBE_H */
