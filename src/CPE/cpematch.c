/**
 * @file cpematch.c
 * \brief Interface to Common Product Enumeration (CPE) matching facilities.
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#include "cpematch.h"

bool cpe_name_match_dict(Cpe_t* cpe, CpeDict_t* dict)
{
	if (cpe == NULL || dict == NULL) return false;
	return cpe_name_match_cpes(cpe, dict->item_n, dict->cpes);
}

bool cpe_name_match_dict_str(const char* cpestr, CpeDict_t* dict)
{
	bool ret;
	if (cpestr == NULL) return false;
	Cpe_t* cpe = cpe_new(cpestr);
	ret = cpe_name_match_dict(cpe, dict);
	cpe_delete(cpe);
	return ret;
}

