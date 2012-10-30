/**
 * @file   _oval_probe_handler.h
 * @brief  OVAL probe handler private header
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup PROBEHANDLERS
 * @{
 */
/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
#ifndef _OVAL_PROBE_HANDLER
#define _OVAL_PROBE_HANDLER

#include <stdint.h>
#include "public/oval_probe_handler.h"

struct oval_ph {
        oval_subtype_t        type;
        oval_probe_handler_t *func;
        void                 *uptr;
};

struct oval_phtbl {
        struct oval_ph **ph;
        uint32_t         sz;
};

oval_phtbl_t *oval_phtbl_new(void);
void oval_phtbl_free(oval_phtbl_t *phtbl);
oval_ph_t *oval_probe_handler_get(oval_phtbl_t *phtbl, oval_subtype_t type);
int oval_probe_handler_set(oval_phtbl_t *phtbl, oval_subtype_t type, oval_probe_handler_t *handler, void *uptr);

/// @}
#endif /* _OVAL_PROBE_HANDLER */
