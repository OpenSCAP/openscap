/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef SCH_CONS_H
#define SCH_CONS_H

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "seap-descriptor.h"
#include "../../../common/util.h"


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


#endif /* SCH_CONS_H */
