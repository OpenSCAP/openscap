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
SEXP_t *SEXP_OVALobj_elm_del (SEXP_t *obj, const char *name, uint32_t nth);

SEXP_t *SEXP_OVALobj_elmattr_add (SEXP_t *obj, const char *elm_name, uint32_t nth, const char *attr_name, SEXP_t *value);
SEXP_t *SEXP_OVALobj_elmattr_del (SEXP_t *obj, const char *elm_name, uint32_t nth, const char *attr_name);

SEXP_t *SEXP_OVALelm_attr_add (SEXP_t *elm, const char *name, SEXP_t *value);
SEXP_t *SEXP_OVALelm_attr_del (SEXP_t *elm, const char *name);

int SEXP_OVALobj_setstatus (SEXP_t *obj, int status);
int SEXP_OVALobj_setelmstatus (SEXP_t *obj, const char *name, uint32_t nth, int status);
int SEXP_OVALelm_setstatus (SEXP_t *elm, int status);

int     SEXP_OVALobj_validate (SEXP_t *obj);
SEXP_t *SEXP_OVALobj_getelm (SEXP_t *obj, const char *name, uint32_t nth);
SEXP_t *SEXP_OVALobj_getelmval (SEXP_t *obj, const char *name, uint32_t nth);
int     SEXP_OVALobj_hasattr (SEXP_t *obj, const char *name);

SEXP_t *SEXP_OVALelm_getval (SEXP_t *elm);
SEXP_t *SEXP_OVALelm_getattrval (SEXP_t *elm, const char *name);
int     SEXP_OVALelm_hasattr (SEXP_t *elm, const char *name);

#define PROBE_EINVAL  1
#define PROBE_ENOELM  2
#define PROBE_ENOVAL  3
#define PROBE_ENOATTR 4
#define PROBE_EINIT   5
#define PROBE_ENOMEM  6
#define PROBE_EOPNOTSUPP 7
#define PROBE_ERANGE 8
#define PROBE_EDOM   9
#define PROBE_EFAULT 10
#define PROBE_EACCES 11
#define PROBE_EUNKNOWN 255

#define OVAL_STATUS_ERROR        1
#define OVAL_STATUS_EXISTS       2
#define OVAL_STATUS_DOESNOTEXIST 3
#define OVAL_STATUS_NOTCOLLECTED 4

#endif /* PROBE_H */
