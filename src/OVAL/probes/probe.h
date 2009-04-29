#pragma once
#ifndef PROBE_H
#define PROBE_H

typedef struct {
        char *name;
        char *value;
} OVALobj_attr_t;

#include <stdarg.h>

SEXP_t *SEXP_OVALobj_create (const char *obj_name, OVALobj_attr_t obj_attrs[], ...);

#endif /* PROBE_H */
