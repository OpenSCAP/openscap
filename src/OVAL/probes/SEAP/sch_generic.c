#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <config.h>
#include "sexp-types.h"
#include "sexp-output.h"
#include "seap-types.h"
#include "sch_generic.h"

#define DATA(ptr) ((sch_genericdata_t *)(ptr))

int sch_generic_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags)
{
        (void)uri;
        (void)flags;
        return (-1);
}

int sch_generic_openfd (SEAP_desc_t *desc, int fd, uint32_t flags)
{
        desc->scheme_data = xmalloc (sizeof (sch_genericdata_t));

        if (flags & DESC_FDIN)
                DATA(desc->scheme_data)->ifd = fd;
        if (flags & DESC_FDOUT)
                DATA(desc->scheme_data)->ofd = fd;
        
        return (0);
}

int sch_generic_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags)
{
        desc->scheme_data = xmalloc (sizeof (sch_genericdata_t));
        
        DATA(desc->scheme_data)->ifd = ifd;
        DATA(desc->scheme_data)->ofd = ofd;
        return (0);
}

ssize_t sch_generic_recv (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags)
{
        return read (DATA(desc->scheme_data)->ifd, buf, len);
}

ssize_t sch_generic_send (SEAP_desc_t *desc, void *buf, size_t len, uint32_t flags)
{
        return write (DATA(desc->scheme_data)->ofd, buf, len);
}

ssize_t sch_generic_sendsexp (SEAP_desc_t *desc, SEXP_t *sexp, uint32_t flags)
{
        return SEXP_st_dprintc (DATA(desc->scheme_data)->ofd, sexp, &(desc->ostate));
}

int sch_generic_close (SEAP_desc_t *desc, uint32_t flags)
{
        return (0);
}
