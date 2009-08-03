#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <config.h>
#include "generic/common.h"
#include "public/sm_alloc.h"
#include "_seap-types.h"
#include "_sexp-types.h"
#include "_seap-scheme.h"
#include "sch_cons.h"
#include "seap-descriptor.h"

#define DATA(ptr) ((sch_consdata_t *)((ptr)->scheme_data))

int sch_cons_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags)
{
        (void)uri;
        (void)flags;

        desc->scheme_data = sm_talloc (sch_consdata_t);
        DATA(desc)->ifd   = fileno (stdin);
        DATA(desc)->ofd   = fileno (stdout);
        
        return (0);
}

int sch_cons_openfd (SEAP_desc_t *desc, int fd, uint32_t flags)
{
        desc->scheme_data = sm_talloc (sch_consdata_t);

        if (flags & SEAP_DESC_FDIN)
                DATA(desc)->ifd = fd;
        if (flags & SEAP_DESC_FDOUT)
                DATA(desc)->ofd = fd;
        
        return (0);
}

int sch_cons_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags)
{
        desc->scheme_data = sm_talloc (sch_consdata_t);
        DATA(desc)->ifd   = ifd;
        DATA(desc)->ofd   = ofd;
        return (0);
}

ssize_t sch_cons_recv (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags)
{
        return read (DATA(desc)->ifd, buf, len);
}

ssize_t sch_cons_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags)
{
        return write (DATA(desc)->ofd, buf, len);
}

ssize_t sch_cons_sendsexp (SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags)
{
        errno = EOPNOTSUPP;
        return (-1);
}

int sch_cons_close (SEAP_desc_t *desc, uint32_t flags)
{
        sm_free (desc->scheme_data);
        return (0);
}

int sch_cons_select (SEAP_desc_t *desc, int ev, uint16_t timeout, uint32_t flags)
{
        return (-1);
}
