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
#include "public/cpelang.h"

#include "../common/util.h"
#include "../common/list.h"

#define CPE_LANG_SUPPORTED "2.2"

struct cpe_lang_model *cpe_lang_model_import(const struct oscap_import_source *source)
{

	__attribute__nonnull__(source);

	if (oscap_import_source_get_name(source) == NULL)
		return NULL;

	struct cpe_lang_model *lang;

	lang = cpe_lang_model_parse_xml(source);

	return lang;
}

void cpe_lang_model_export(const struct cpe_lang_model *spec, struct oscap_export_target *target)
{

	__attribute__nonnull__(spec);
	__attribute__nonnull__(target);

	if (oscap_export_target_get_name(target) == NULL)
		return;

	cpe_lang_model_export_xml(spec, target);
}

/*
 * Add new platform entry to @a platformspec
 * @note @a platformspec will take over memory management of @a platform
 * @param platformspec list of platforms being extended
 * @param platform platform to add to the list
 * @return true on success
 */
static bool cpe_language_match_expr(struct cpe_name **cpe, size_t n, const struct cpe_testexpr *expr)
{
	__attribute__nonnull__(cpe);
	__attribute__nonnull__(expr);

	bool ret;

	switch (cpe_testexpr_get_oper(expr) & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
		ret = true;
		OSCAP_FOREACH(cpe_testexpr, cur, cpe_testexpr_get_meta_expr(expr),
			if (!cpe_language_match_expr(cpe, n, cur)) {
				ret = false;
				break;
			}
		)
		break;
	case CPE_LANG_OPER_OR:
		ret = false;
		OSCAP_FOREACH(cpe_testexpr, cur, cpe_testexpr_get_meta_expr(expr),
			if (cpe_language_match_expr(cpe, n, cur)) {
				ret = true;
				break;
			}
		)
		break;
	case CPE_LANG_OPER_MATCH:
		ret = cpe_name_match_cpes(cpe_testexpr_get_meta_cpe(expr), n, cpe);
		break;
	default:
		assert(false);
	}

	return (cpe_testexpr_get_oper(expr) & CPE_LANG_OPER_NOT ? !ret : ret);
}

bool cpe_platform_match_cpe(struct cpe_name ** cpe, size_t n, const struct cpe_platform * platform)
{
	return cpe_language_match_expr(cpe, n, cpe_platform_get_expr(platform));
}

const char * cpe_lang_model_supported(void)
{
        return CPE_LANG_SUPPORTED;
}

/*
bool cpe_language_match_str(const char* cpe, const struct cpe_platform* platform)
{

        __attribute__nonnull__(cpe);
        __attribute__nonnull__(platform);

	struct cpe_name* cpe_;
	bool ret;

	cpe_ = cpe_new(cpe);
	if (cpe_ == NULL) return false;
	ret = cpe_language_match_cpe(cpe_, platform);
	cpe_free(cpe_);
	return ret;
}
*/
