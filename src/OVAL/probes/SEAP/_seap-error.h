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
#ifndef _SEAP_ERROR_H
#define _SEAP_ERROR_H

#include "public/sexp.h"
#include "_seap-message.h"
#include "../../../common/util.h"

struct SEAP_err {
	SEAP_msgid_t id;
	uint32_t code;
	uint8_t type;
	SEXP_t *data;
};

typedef struct SEAP_err SEAP_err_t;

SEAP_err_t *SEAP_error_new(void);
SEAP_err_t *SEAP_error_clone(SEAP_err_t *e);
void SEAP_error_free(SEAP_err_t *e);

#endif /* _SEAP_ERROR_H */
