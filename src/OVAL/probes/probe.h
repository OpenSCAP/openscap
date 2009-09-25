#ifndef __STUB_PROBE
#pragma once
#ifndef PROBE_H
#define PROBE_H

#include <seap.h>
#include <stdarg.h>
#include <probe-entcmp.h>
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
int SEXP_OVALelm_getstatus (SEXP_t *elm);

int     SEXP_OVALobj_validate (SEXP_t *obj);
SEXP_t *SEXP_OVALobj_getelm (SEXP_t *obj, const char *name, uint32_t nth);
SEXP_t *SEXP_OVALobj_getelmval (SEXP_t *obj, const char *name, uint32_t nth_e, uint32_t nth_v);
SEXP_t *SEXP_OVALobj_getattrval (SEXP_t *obj, const char *name);
int     SEXP_OVALobj_hasattr (SEXP_t *obj, const char *name);

SEXP_t *SEXP_OVALelm_getval (SEXP_t *elm, uint32_t nth);
SEXP_t *SEXP_OVALelm_getattrval (SEXP_t *elm, const char *name);
int     SEXP_OVALelm_hasattr (SEXP_t *elm, const char *name);
int     SEXP_OVALelm_setdatatype(SEXP_t *elm, uint32_t nth, int type);
int     SEXP_OVALelm_getdatatype(SEXP_t *elm, uint32_t nth);
int     SEXP_OVALelm_setmask(SEXP_t *elm, bool mask);
bool    SEXP_OVALelm_getmask(SEXP_t *elm);

char       *SEXP_OVALelm_name_cstr   (const SEXP_t *elm);
char       *SEXP_OVALelm_name_cstr_r (const SEXP_t *elm, char *buf, size_t len);
const char *SEXP_OVALelm_name_cstrp  (const SEXP_t *elm);

#include <pthread.h>

struct id_desc_t {
#ifndef HAVE_ATOMIC_FUNCTIONS
        pthread_mutex_t item_id_ctr_lock;
#endif
        int item_id_ctr;
};

SEXP_t *SEXP_OVALitem_newid(struct id_desc_t *id_desc);
void SEXP_OVALitem_resetid(struct id_desc_t *id_desc);

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

#include <probe-cache.h>

typedef struct {
        /* Protocol stuff */
        SEAP_CTX_t *ctx;
        int         sd;

        /* Object cache */
        pcache_t   *pcache;
        pthread_rwlock_t pcache_lock;

        struct id_desc_t id_desc;

        /* probe main */
        void *probe_arg;
} globals_t;

#if defined(HAVE_ATOMIC_FUNCTIONS)
#define GLOBALS_INITIALIZER { NULL, -1, NULL, PTHREAD_MUTEX_INITIALIZER, {1}, NULL }
#else
#define GLOBALS_INITIALIZER { NULL, -1, NULL, PTHREAD_MUTEX_INITIALIZER, {PTHREAD_MUTEX_INITIALIZER, 1}, NULL }
#endif

extern globals_t global;

#define READER_LOCK_CACHE pthread_rwlock_rdlock (&globals.pcache_lock)
#define WRITER_LOCK_CACHE pthread_rwlock_wrlock (&globals.pcache_lock)
#define READER_UNLOCK_CACHE pthread_rwlock_unlock (&globals.pcache_lock)
#define WRITER_UNLOCK_CACHE pthread_rwlock_unlock (&globals.pcache_lock)

#define SEAP_LOCK pthread_mutex_lock (&globals.seap_lock)
#define SEAP_UNLOCK pthread_mutex_unlock (&globals.seap_lock)

#define PROBECMD_STE_FETCH 1
#define PROBECMD_OBJ_EVAL  2

#endif /* PROBE_H */
#endif
