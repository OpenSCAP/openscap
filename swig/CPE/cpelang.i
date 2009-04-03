/*
 * Common Product Enumeration
 * (http://nvd.nist.gov/cpe.cfm)
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
 *      Maros Barabas   <mbarabas@redhat.com>
 *
 */

%module cpelang
%{
 #include "../../src/CPE/cpelang.h"
%}


cpe_platform_spec_t *cpe_platformspec_new(const char *fname);
bool cpe_platformspec_add(cpe_platform_spec_t * platformspec, cpe_platform_t * platform);
void cpe_platformspec_delete(cpe_platform_spec_t * platformspec);
bool cpe_language_match_cpe(cpe_t ** cpe, size_t n, const cpe_platform_t * platform);
void cpe_platform_delete(cpe_platform_t * platform);
void cpe_langexpr_delete(cpe_lang_expr_t * expr);
