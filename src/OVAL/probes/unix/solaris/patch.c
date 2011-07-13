/**
 * @file patch.c
 * @brief patch probe
 * @author "Ryan E Haggerty" <rhaggerty@tresys.com
 *
 * 2011/05/06 rhaggerty@tresys.com
 *	This probe allows patches installed on the system to be checked.
 */

/*
 * Copyright (c) 2011 Tresys Technology LLC, Columbia, Maryland, USA
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
*/

#include "probe-api.h"


int probe_main(probe_ctx *ctx, void *probe_arg)
{
        return(PROBE_EOPNOTSUPP);
}
