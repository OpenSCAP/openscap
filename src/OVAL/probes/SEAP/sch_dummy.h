#pragma once
#ifndef SCH_DUMMY_H
#define SCH_DUMMY_H

#include "../../../common/util.h"

OSCAP_HIDDEN_START;

int sch_dummy_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags);
int sch_dummy_openfd (SEAP_desc_t *desc, int fd, uint32_t flags);
int sch_dummy_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags);
ssize_t sch_dummy_recv (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
ssize_t sch_dummy_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
ssize_t sch_dummy_sendsexp (SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags);
int sch_dummy_close (SEAP_desc_t *desc, uint32_t flags);
int sch_dummy_select (SEAP_desc_t *desc, int ev, uint16_t timeout, uint32_t flags);

OSCAP_HIDDEN_END;

#endif /* SCH_DUMMY_H */
