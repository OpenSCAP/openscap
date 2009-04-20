#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <config.h>
#include "seap.h"
#include "sch_cons.h"

#define DATA(ptr) ((sch_consdata_t *)((ptr)->scheme_data))

int sch_cons_connect (SEAP_desc_t *desc, const char *uri, uint32_t flags)
{
        (void)uri;
        (void)flags;

        desc->scheme_data = xmalloc (sizeof (sch_consdata_t));
        
        DATA(desc)->ifd = fileno (stdin);
        DATA(desc)->ofd = fileno (stdout);
        
        return (0);
}

int sch_cons_openfd (SEAP_desc_t *desc, int fd, uint32_t flags)
{
        desc->scheme_data = xmalloc (sizeof (sch_consdata_t));

        if (flags & DESC_FDIN)
                DATA(desc)->ifd = fd;
        if (flags & DESC_FDOUT)
                DATA(desc)->ofd = fd;
        
        return (0);
}

int sch_cons_openfd2 (SEAP_desc_t *desc, int ifd, int ofd, uint32_t flags)
{
        desc->scheme_data = xmalloc (sizeof (sch_consdata_t));

        DATA(desc)->ifd = ifd;
        DATA(desc)->ofd = ofd;
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

int sch_cons_close (SEAP_desc_t *desc, uint32_t flags)
{
        xfree (&(desc->scheme_data));
        return (0);
}
