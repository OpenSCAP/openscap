#pragma once
#ifndef SCH_GENERIC_H
#define SCH_GENERIC_H

typedef struct {
        int ifd;
        int ofd;
} sch_genericdata_t;

int sch_generic_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags);
int sch_generic_openfd (SEAP_desc_t *desc, int fd, uint32_t flags);
int sch_generic_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags);
ssize_t sch_generic_recv (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
ssize_t sch_generic_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
ssize_t sch_generic_sendsexp (SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags);
int sch_generic_close (SEAP_desc_t *desc, uint32_t flags);

#endif /* SCH_GENERIC_H */
