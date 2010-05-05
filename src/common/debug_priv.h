
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 *       Peter Vrabec <pvrabec@redhat.com>
 */ 

#pragma once
#ifndef OSCAP_DEBUG_PRIV_H_
#define OSCAP_DEBUG_PRIV_H_

#include "util.h"
#include "public/debug.h"

OSCAP_HIDDEN_START;

#ifndef oscap_dprintf           
#if defined(NDEBUG)
# define oscap_dprintf(...) while(0)
#else
# include <stddef.h>
# include <stdarg.h>
void __oscap_dprintf(const char *, const char *, size_t, const char *, ...);
# define oscap_dprintf(...) __oscap_dprintf (__FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#endif                          /* NDEBUG */
#endif                          /* oscap_dprintf */


#ifndef NDEBUG
#include <stdlib.h>
extern int __debuglog_level;
# define debug(l) if ((__debuglog_level = (__debuglog_level == -1 ? atoi (getenv (OSCAP_DEBUG_LEVEL_ENV) == NULL ? "0" : getenv (OSCAP_DEBUG_LEVEL_ENV)) : __debuglog_level)) && __debuglog_level >= (l))
#else
# define debug(l) if (0)
#endif

#define oscap_dlprintf(l, ...) do { debug(l) { oscap_dprintf(__VA_ARGS__); }} while(0)

OSCAP_HIDDEN_END;

#endif

