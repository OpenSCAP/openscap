/**
 * @file debug_priv.h
 * @brief oscap debug helpers private header
 */
/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *       Daniel Kopecek <dkopecek@redhat.com>
 *       Peter Vrabec <pvrabec@redhat.com>
 */

#pragma once
#ifndef OSCAP_DEBUG_PRIV_H_
#define OSCAP_DEBUG_PRIV_H_

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include "util.h"
#include "public/oscap_debug.h"


#define OSCAP_DEBUGOBJ_SEXP 1

#ifndef _A
#define _A(x) assert(x)
#endif


# define __dlprintf_wrapper(l, ...) __oscap_dlprintf (l, __FILE__, __PRETTY_FUNCTION__, __LINE__, 0, __VA_ARGS__)

/**
 * Convenience macro for calling __oscap_dlprintf. Only the fmt & it's arguments
 * need to be specified. The __FILE__, __PRETTY_FUNCTION__ and __LINE__ macros
 * are used for the first three arguments.
 */
# define oscap_dlprintf(l, ...) __dlprintf_wrapper (l, __VA_ARGS__)

void __oscap_debuglog_object (const char *file, const char *fn, size_t line, int objtype, void *obj);

# define dO(type, obj) __oscap_debuglog_object(__FILE__, __PRETTY_FUNCTION__, __LINE__, type, obj)


#define dI(...) oscap_dlprintf(DBG_I, __VA_ARGS__)
#define dW(...) oscap_dlprintf(DBG_W, __VA_ARGS__)
#define dE(...) oscap_dlprintf(DBG_E, __VA_ARGS__)
#define dD(...) oscap_dlprintf(DBG_D, __VA_ARGS__)

#define dIndent(indent_change) __oscap_dlprintf(DBG_I, __FILE__, __PRETTY_FUNCTION__, __LINE__, indent_change, NULL)


#endif
