/*
 * Copyright 2016 Red Hat Inc., Durham, North Carolina.
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

#include "rpm-helper.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_RPM46
int rpmErrorCb (rpmlogRec rec, rpmlogCallbackData data)
{
	dE("RPM: %s", rpmlogRecMessage(rec));
	return RPMLOG_DEFAULT;
}
#else
void rpmErrorCb (rpmlogRec rec, rpmlogCallbackData data)
{
	dE("RPM: %s", rpmlogRecMessage(rec));
}
#endif

void rpmLibsPreload()
{
	// Don't load rpmrc files. The are useless for us,
	// because we only need to preload libraries
	const char* rcfiles = "";
	rpmReadConfigFiles(rcfiles, NULL);
}
