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
#ifndef SEAP_COMMAND_BACKENDS
#define SEAP_COMMAND_BACKENDS

#include "../../../common/util.h"


#define SEAP_COMMAND_BACKENDS_MAXCAPACITY 769

int  SEAP_cmdtbl_backendS_add (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_ins (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
int  SEAP_cmdtbl_backendS_del (SEAP_cmdtbl_t *t, SEAP_cmdrec_t *r);
SEAP_cmdrec_t *SEAP_cmdtbl_backendS_get (SEAP_cmdtbl_t *t, SEAP_cmdcode_t c);
int  SEAP_cmdtbl_backendS_cmp (SEAP_cmdrec_t *a, SEAP_cmdrec_t *b);
void SEAP_cmdtbl_backendS_free (SEAP_cmdtbl_t *t);
int  SEAP_cmdtbl_backendS_apply (SEAP_cmdtbl_t *t, int (*func) (SEAP_cmdrec_t *r, void *), void *arg);


#endif /* SEAP_COMMAND_BACKENDS */
