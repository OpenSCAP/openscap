/*
 * Copyright 2017 Red Hat Inc., Durham, North Carolina.
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
 *    Jan Černý <jcerny@redhat.com>
 */

#ifndef OSCAP_API_H
#define OSCAP_API_H

/**
 * OSCAP_API macro
 *
 * OSCAP_API is a macro that marks every public function in OpenSCAP API.
 * Functions marked with OSCAP_API will be exported to the shared library
 * interface.
 */
#ifndef OSCAP_API
#  if defined(_WIN32) && defined(_MSC_VER)
#    ifdef OSCAP_BUILD_SHARED /* build DLL */
#      define OSCAP_API __declspec(dllexport)
#    else /* use DLL */
#      define OSCAP_API __declspec(dllimport)
#    endif
#  else
#    define OSCAP_API
#  endif
#endif

#endif
