/**
 * @file cpelang.c
 * \brief Interface to Common Platform Enumeration (CPE) Language
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
 *      Maros Barabas <mbarabas@redhat.com>
 */

#include <libxml/tree.h>
#include <string.h>
#include "cpelang_priv.h"
#include "../common/util.h"
#include "../common/list.h"


struct cpe_lang_model * cpe_lang_model_import(const char *fname) {

    if (fname == NULL) return NULL;

    return cpe_lang_parse(fname);
}

/*
 * Add new platform entry to @a platformspec
 * @note @a platformspec will take over memory management of @a platform
 * @param platformspec list of platforms being extended
 * @param platform platform to add to the list
 * @return true on success
 */
static bool cpe_language_match_expr(struct cpe_name ** cpe, size_t n,
				    const struct cpe_testexpr * expr)
{
	struct cpe_testexpr *cur;
	bool ret;

	switch (cpe_testexpr_get_oper(expr) & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
		ret = true;
		for (cur = cpe_testexpr_get_meta_expr(expr); cpe_testexpr_get_oper(cur); cur=cpe_testexpr_get_meta_expr(cur)) {
			if (!cpe_language_match_expr(cpe, n, cur)) {
				ret = false;
				break;
			}
		}
		break;
	case CPE_LANG_OPER_OR:
		ret = false;
		for (cur = cpe_testexpr_get_meta_expr(expr); cpe_testexpr_get_oper(cur); cur=cpe_testexpr_get_meta_expr(cur)) {
			if (cpe_language_match_expr(cpe, n, cur)) {
				ret = true;
				break;
			}
		}
		break;
	case CPE_LANG_OPER_MATCH:
		ret = cpe_name_match_cpes(cpe_testexpr_get_meta_cpe(expr), n, cpe);
		break;
	default:
		return false;
	}

	return (cpe_testexpr_get_oper(expr) & CPE_LANG_OPER_NOT ? !ret : ret);
}

bool cpe_platform_match_cpe(struct cpe_name ** cpe, size_t n,
			    const struct cpe_platform * platform)
{
        //TODO has an error when compiling
	//return cpe_language_match_expr(cpe, n, &(cpe_platform_get_expr(platform)));
        return true;
}

/*
bool cpe_language_match_str(const char* cpe, const struct cpe_platform* platform)
{
	struct cpe_name* cpe_;
	bool ret;

	cpe_ = cpe_new(cpe);
	if (cpe_ == NULL) return false;
	ret = cpe_language_match_cpe(cpe_, platform);
	cpe_free(cpe_);
	return ret;
}
*/
