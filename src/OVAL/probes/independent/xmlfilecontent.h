/*
 * Copyright 2018 Red Hat Inc., Durham, North Carolina.
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
 *      Jan Černý <jcerny@redhat.com>
 */

#ifndef OPENSCAP_XMLFILECONTENT_H
#define OPENSCAP_XMLFILECONTENT_H

#include "probe-api.h"

int xmlfilecontent_probe_offline_mode_supported(void);
void *xmlfilecontent_probe_init(void);
int xmlfilecontent_probe_main(probe_ctx *ctx, void *arg);
void xmlfilecontent_probe_fini(void *arg);

#endif /* OPENSCAP_XMLFILECONTENT_H */
