/*
 * Copyright 2022 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Kate Hsuan <hpa@redhat.com>
 */

#ifndef OPENSCAP_FWUPDSECATTR_PROBE_H
#define OPENSCAP_FWUPDSECATTR_PROBE_H

#include <sys/queue.h>

#include "probe-api.h"

struct secattr_cache {
	char *name;
	uint32_t hsi_result;
	LIST_ENTRY(secattr_cache) entries;
};

typedef enum {
	FWUPD_SECURITY_ATTR_RESULT_UNKNOWN,
	FWUPD_SECURITY_ATTR_RESULT_ENABLED,
	FWUPD_SECURITY_ATTR_RESULT_NOT_ENABLED,
	FWUPD_SECURITY_ATTR_RESULT_VALID,
	FWUPD_SECURITY_ATTR_RESULT_NOT_VALID,
	FWUPD_SECURITY_ATTR_RESULT_LOCKED,
	FWUPD_SECURITY_ATTR_RESULT_NOT_LOCKED,
	FWUPD_SECURITY_ATTR_RESULT_ENCRYPTED,
	FWUPD_SECURITY_ATTR_RESULT_NOT_ENCRYPTED,
	FWUPD_SECURITY_ATTR_RESULT_TAINTED,
	FWUPD_SECURITY_ATTR_RESULT_NOT_TAINTED,
	FWUPD_SECURITY_ATTR_RESULT_FOUND,
	FWUPD_SECURITY_ATTR_RESULT_NOT_FOUND,
	FWUPD_SECURITY_ATTR_RESULT_SUPPORTED,
	FWUPD_SECURITY_ATTR_RESULT_NOT_SUPPORTED,
	/*< private >*/
	FWUPD_SECURITY_ATTR_RESULT_LAST
} FwupdSecurityAttrResult;

LIST_HEAD(cachehed, secattr_cache);

int fwupdsecattr_probe_offline_mode_supported(void);
int fwupdsecattr_probe_main(probe_ctx *ctx, void *arg);

#endif /* OPENSCAP_FWUPD_SECURITY_ATTR_PROBE_H */
