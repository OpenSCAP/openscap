#pragma once
#ifndef PROBE_H
#define PROBE_H

typedef struct {
        char *name;
        char *value;
} OVALobj_attr_t;

#include <stdarg.h>
#include "oval_definitions_impl.h"

SEXP_t *SEXP_OVALobj_create (const char *obj_name, OVALobj_attr_t obj_attrs[], ...);

#define SEXP_OVALobj_getelement_value SEXP_OVALobj_getelm_val
SEXP_t *SEXP_OVALobj_getelm_val (SEXP_t *obj, const char *name);

SEXP_t *oval_object_to_sexp (const char *typestr, struct oval_object *object);

#endif /* PROBE_H */
