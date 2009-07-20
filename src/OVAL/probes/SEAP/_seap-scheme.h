#pragma once
#ifndef _SEAP_SCHEME_H
#define _SEAP_SCHEME_H

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "_sexp-types.h"
#include "_seap-types.h"
#include "seap-descriptor.h"

typedef struct {
        const char *schstr;
        int     (*sch_connect)  (SEAP_desc_t *, const char *, uint32_t);
        int     (*sch_openfd)   (SEAP_desc_t *, int, uint32_t);
        int     (*sch_openfd2)  (SEAP_desc_t *, int, int, uint32_t);
        ssize_t (*sch_recv)     (SEAP_desc_t *, void *, size_t, uint32_t);
        ssize_t (*sch_send)     (SEAP_desc_t *, void *, size_t, uint32_t);
        int     (*sch_close)    (SEAP_desc_t *, uint32_t);
        ssize_t (*sch_sendsexp) (SEAP_desc_t *, SEXP_t *, uint32_t);
} SEAP_schemefn_t;

extern const SEAP_schemefn_t __schtbl[];

#define SCH_CONNECT(idx, ...) __schtbl[idx].sch_connect (__VA_ARGS__)
#define SCH_OPENFD(idx, ...) __schtbl[idx].sch_openfd (__VA_ARGS__)
#define SCH_OPENFD2(idx, ...) __schtbl[idx].sch_openfd2 (__VA_ARGS__)
#define SCH_RECV(idx, ...) __schtbl[idx].sch_recv (__VA_ARGS__)
#define SCH_SEND(idx, ...) __schtbl[idx].sch_send (__VA_ARGS__)
#define SCH_CLOSE(idx, ...) __schtbl[idx].sch_close (__VA_ARGS__)
#define SCH_SENDSEXP(idx, ...) __schtbl[idx].sch_sendsexp (__VA_ARGS__)

SEAP_scheme_t SEAP_scheme_search (const SEAP_schemefn_t fntable[], const char *sch, size_t schlen);

/* console */
#include "sch_cons.h"
#define SCH_CONS    0

/* dummy */
#include "sch_dummy.h"
#define SCH_DUMMY   1

/* pipe */
#include "sch_pipe.h"
#define SCH_PIPE    3

/* generic */
#include "sch_generic.h"
#define SCH_GENERIC 2

#define SCH_NONE    255

#endif /* _SEAP_SCHEME_H */
