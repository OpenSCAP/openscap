#pragma once
#ifndef SCH_CONS_H
#define SCH_CONS_H

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "seap-descriptor.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

typedef struct {
        int ifd;
        int ofd;
} sch_consdata_t;

int sch_cons_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags);
int sch_cons_openfd (SEAP_desc_t *desc, int fd, uint32_t flags);
int sch_cons_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags);
ssize_t sch_cons_recv (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
ssize_t sch_cons_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
ssize_t sch_cons_sendsexp (SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags);
int sch_cons_close (SEAP_desc_t *desc, uint32_t flags);
int sch_cons_select (SEAP_desc_t *desc, int ev, uint16_t timeout, uint32_t flags);

OSCAP_HIDDEN_END;

#endif /* SCH_CONS_H */
