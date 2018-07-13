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
 */

#ifndef OPENSCAP_OSCAP_PLATFORMS_H
#define OPENSCAP_OSCAP_PLATFORMS_H

#undef OS_FREEBSD
#undef OS_LINUX
#undef OS_SOLARIS
#undef OS_SUNOS
#undef OS_WINDOWS
#undef OS_AIX
#undef OS_APPLE
#undef OS_OSX

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
# define OS_FREEBSD
#elif defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
# define OS_LINUX
#elif defined(sun) || defined(__sun)
# if defined(__SVR4) || defined(__svr4__)
#  define OS_SOLARIS
# else
#  define OS_SUNOS
# endif
#elif defined(_WIN32) || defined(_WIN64)
# define OS_WINDOWS
#elif defined(_AIX)
# define OS_AIX
#elif defined(__APPLE__)
# define OS_APPLE
# if defined(Macintosh) || defined(macintosh) || defined(__APPLE__) && defined(__MACH__)
#  define OS_OSX
# endif
#else
# error "Sorry, your OS isn't supported."
#endif

#endif
