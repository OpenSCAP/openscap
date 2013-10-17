/* Substitute for <net/if.h>.
   Copyright (C) 2007-2014 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef _@GUARD_PREFIX@_NET_IF_H

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

#if @HAVE_NET_IF_H@

/* On some platforms, <net/if.h> assumes prior inclusion of
   <sys/socket.h>.  */
# include <sys/socket.h>

/* The include_next requires a split double-inclusion guard.  */
# @INCLUDE_NEXT@ @NEXT_NET_IF_H@

#endif

#ifndef _@GUARD_PREFIX@_NET_IF_H
#define _@GUARD_PREFIX@_NET_IF_H

#if !@HAVE_NET_IF_H@

/* A platform that lacks <net/if.h>.  */

struct if_nameindex
{
  unsigned if_index;
  char *if_name;
};

# define IF_NAMESIZE 1 /* Without if_* functions, this supports only "". */

#endif

#endif /* _@GUARD_PREFIX@_NET_IF_H */
#endif /* _@GUARD_PREFIX@_NET_IF_H */
