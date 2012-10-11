
/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 */

#pragma once
#ifndef _OSCAP_ERROR_H
#define _OSCAP_ERROR_H

#include <errno.h>
#include <libxml/xmlerror.h>
#include "public/oscap_error.h"

#define oscap_assert_errno(cond, desc) \
	{ if (!(cond)) { if ((errno)) oscap_seterr(OSCAP_EFAMILY_GLIBC, desc); \
                         else oscap_seterr(OSCAP_EFAMILY_OSCAP, desc); } }

#define oscap_setxmlerr(error) __oscap_setxmlerr (__FILE__, __LINE__, __PRETTY_FUNCTION__, error)

void __oscap_setxmlerr(const char *file, uint32_t line, const char *func, xmlErrorPtr error);

struct oscap_err_t {
	oscap_errfamily_t family;
	char *desc;
	const char *func;
	const char *file;
	uint32_t line;
};

/**
 * __oscap_seterr() wrapper function
 */
#define oscap_seterr(family, ...) __oscap_seterr (__FILE__, __LINE__, __PRETTY_FUNCTION__, family, __VA_ARGS__)


/**
 * Set an error
 */
void __oscap_seterr(const char *file, uint32_t line, const char *func, oscap_errfamily_t family, ...);

#endif				/* _OSCAP_ERROR_H */
