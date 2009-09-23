#pragma once
#ifndef PROBE_API_H
#define PROBE_API_H

#include <seap.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "api/oval_definitions.h"
#include "api/oval_system_characteristics.h"
#include "api/oval_results.h"

/*
 * items
 */

SEXP_t *probe_item_build (const char *fmt, ...);
SEXP_t *probe_item_creat (const char *name, SEXP_t *attrs, ...);
SEXP_t *probe_item_new   (const char *name, SEXP_t *attrs);

SEXP_t *probe_item_attr_add ();
SEXP_t *probe_item_ent_add ();

/*
 * attributes
 */

SEXP_t *probe_attr_creat (const char *name, SEXP_t *val, ...);

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
int probe_objent_setstatus (SEXP_t *obj, const char *name, uint32_t n, int status);

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

#include <probe-cache.h>

struct id_desc_t {
#ifndef HAVE_ATOMIC_FUNCTIONS
        pthread_mutex_t item_id_ctr_lock;
#endif
        int item_id_ctr;
};

SEXP_t *SEXP_OVALitem_newid(struct id_desc_t *id_desc);
void SEXP_OVALitem_resetid(struct id_desc_t *id_desc);

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

#endif /* PROBE_API_H */
