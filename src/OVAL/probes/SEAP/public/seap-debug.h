/**
 * @file   seap-debug.h
 * @brief  SEAP debug helpers public header
 * @author Daniel Kopecek <dkopecek@redhat.com>
 */
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
#ifndef SEAP_DEBUG_H
#define SEAP_DEBUG_H

#include <assert.h>
#include "common/debug_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _A
#define _A(x) assert(x)
#endif

#define _D(...) dI(__VA_ARGS__)
#define _W(...) dW(__VA_ARGS__)
#define _I(...) dI(__VA_ARGS__)
#define _F(...) dE(__VA_ARGS__)
#define _SE(sexp) dO(OSCAP_DEBUGOBJ_SEXP, sexp)
#define _LOGCALL_ while(0)

#ifdef __cplusplus
}
#endif

#endif /* SEAP_DEBUG_H */
