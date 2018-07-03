/**
 * @file cpelang.c
 * \brief Interface to Common Platform Enumeration (CPE) Language
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/tree.h>
#include <string.h>

#include "public/cpe_lang.h"
#include "cpelang_priv.h"
#include "common/util.h"
#include "common/list.h"
#include "source/public/oscap_source.h"

#define CPE_LANG_SUPPORTED "2.2"

void cpe_lang_model_export(const struct cpe_lang_model *spec, const char *file)
{

	__attribute__nonnull__(spec);
	__attribute__nonnull__(file);

	if (file == NULL)
		return;

	cpe_lang_model_export_xml(spec, file);
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

	bool ret = false;

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

char *cpe_lang_model_detect_version_priv(xmlTextReader *reader)
{
	// FIXME: There is no detection logic in there, since there is only
	//        one version of CPE language so we just return that.
	return oscap_strdup("2.3");
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

static bool cpe_testexpr_evaluate(const struct cpe_testexpr* expr, cpe_check_fn check_cb, cpe_dict_fn dict_cb, void* usr)
{
	bool ret = false;

	switch (cpe_testexpr_get_oper(expr) & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
		ret = true;
		OSCAP_FOREACH(cpe_testexpr, cur, cpe_testexpr_get_meta_expr(expr),
			if (!cpe_testexpr_evaluate(cur, check_cb, dict_cb, usr)) {
				ret = false;
				break;
			}
		)
		break;
	case CPE_LANG_OPER_OR:
		OSCAP_FOREACH(cpe_testexpr, cur, cpe_testexpr_get_meta_expr(expr),
			if (cpe_testexpr_evaluate(cur, check_cb, dict_cb, usr)) {
				ret = true;
				break;
			}
		)
		break;
	case CPE_LANG_OPER_MATCH:
		ret = dict_cb(cpe_testexpr_get_meta_cpe(expr), usr);
		break;
	case CPE_LANG_OPER_CHECK:
		ret = check_cb(cpe_testexpr_get_meta_check_system(expr), cpe_testexpr_get_meta_check_href(expr), cpe_testexpr_get_meta_check_id(expr), usr);
		break;
	default:
		assert(false);
	}

	return (cpe_testexpr_get_oper(expr) & CPE_LANG_OPER_NOT) ? !ret : ret;
}

bool cpe_platform_applicable_lang_model(const char* platform, struct cpe_lang_model *lang_model, cpe_check_fn check_cb, cpe_dict_fn dict_fn, void* usr)
{
	struct cpe_platform* plat = cpe_lang_model_get_item(lang_model, platform);

	if (plat == NULL) // can't find any matching platform implies not applicable
		return false;

	const struct cpe_testexpr* expr = cpe_platform_get_expr(plat);
	return cpe_testexpr_evaluate(expr, check_cb, dict_fn, usr);
}
