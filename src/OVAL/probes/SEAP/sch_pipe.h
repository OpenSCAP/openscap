#pragma once
#ifndef SCH_PIPE_H
#define SCH_PIPE_H

#include <sys/types.h>
#include <unistd.h>

typedef struct {
        int   pfd;
        pid_t pid;
        char *execpath;
} sch_pipedata_t;

int sch_pipe_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags);
int sch_pipe_openfd (SEAP_desc_t *desc, int fd, uint32_t flags);
int sch_pipe_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags);
ssize_t sch_pipe_recv (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
ssize_t sch_pipe_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags);
ssize_t sch_pipe_sendsexp (SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags);
int sch_pipe_close (SEAP_desc_t *desc, uint32_t flags);

#endif /* SCH_PIPE_H */
