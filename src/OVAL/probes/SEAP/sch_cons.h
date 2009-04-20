#pragma once
#ifndef SCH_CONS_H
#define SCH_CONS_H

typedef struct {
        int ifd;
        int ofd;
} sch_consdata_t;

int sch_cons_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags);
int sch_cons_openfd (SEAP_desc_t *desc, int fd, uint32_t flags);
int sch_cons_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags);
ssize_t sch_cons_recv (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
ssize_t sch_cons_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
int sch_cons_close (SEAP_desc_t *desc, uint32_t flags);

#endif /* SCH_CONS_H */
