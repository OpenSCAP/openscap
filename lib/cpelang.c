/**
 * @file cpelang.c
 * \brief Interface to Common Product Enumeration (CPE) Language
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

#include <libxml/tree.h>
#include <string.h>
#include "libcpelang.h"

/*** Private ***/

/**
 * New platform specification from XML node
 * @param root XML node to be processed
 * @return new platform specification list
 * @retval NULL on failure
 */
CpePlatformSpec_t* cpe_platformspec_new_xml(xmlNodePtr root);

/**
 * New platform form XML node
 * @param node XML node to be processed
 * @return new platform specification
 * @retval NULL on failure
 */
CpePlatform_t* cpe_platform_new_xml(xmlNodePtr node);

/**
 * Create new CPE language boolean expression from XML node
 * @param ret pointer to resulting expression
 * @param node XML node to be processed
 * @return true on success
 */
bool cpe_langexpr_new(CpeLangExpr_t* ret, xmlNodePtr node);

/*** Public ***/

CpePlatformSpec_t* cpe_platformspec_new(const char* fname)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	CpePlatformSpec_t* ret;

	if ((doc = xmlParseFile(fname)) == NULL)
		return NULL;

	if ((root = xmlDocGetRootElement(doc)) == NULL)
		return NULL;

	ret = cpe_platformspec_new_xml(root);

	xmlFreeDoc(doc);

	return ret;
}

const size_t CPE_PLATFORMSPEC_EMPTY_INIT_ALLOC = 8;

CpePlatformSpec_t* cpe_platformspec_new_empty()
{
	CpePlatformSpec_t* res;

	res = malloc(sizeof(CpePlatformSpec_t));
	if (res == NULL) return NULL;
	
	res->alloc_      = CPE_PLATFORMSPEC_EMPTY_INIT_ALLOC;
	res->platforms   = malloc(res->alloc_ * sizeof(CpePlatform_t*));
	res->platforms_n = 0;

	if (res->platforms == NULL) {
		free(res);
		return NULL;
	}

	return res;
}

CpePlatformSpec_t* cpe_platformspec_new_xml(xmlNodePtr root)
{
	xmlNodePtr cur;
	CpePlatformSpec_t* res;
	CpePlatform_t* plat;
	int idx;

	if (xmlStrcmp(root->name, BAD_CAST "platform-specification") != 0)
		return NULL;

	res = cpe_platformspec_new_empty();
	if (res == NULL) return NULL;

	for (cur = root->xmlChildrenNode; cur != NULL; cur = cur->next) {
		if (!(plat = cpe_platform_new_xml(cur))) continue;
		if (!(cpe_platformspec_add(res, plat))) {
			cpe_platform_delete(plat);
			cpe_platformspec_delete(res);
			return NULL;
		}
	}

	return res;
}

bool cpe_platformspec_add(CpePlatformSpec_t* platformspec, CpePlatform_t* platform)
{
	CpePlatform_t** old;

	if (platformspec == NULL || platform == NULL)
		return NULL;

	if (platformspec->alloc_ < platformspec->platforms_n + 1) {
		if (platformspec->alloc_ > 0) platformspec->alloc_ *= 2;
		else platformspec->alloc_ = CPE_PLATFORMSPEC_EMPTY_INIT_ALLOC;

		old = platformspec->platforms;
		if (!(platformspec->platforms = realloc(old, platformspec->alloc_ * sizeof(CpePlatform_t*)))) {
			platformspec->platforms = old;
			return false;
		}
	}
	
	platformspec->platforms[platformspec->platforms_n++] = platform;
	return true;
}

void cpe_platformspec_delete(CpePlatformSpec_t* platformspec)
{
	int i;
	if (platformspec != NULL) {
		for (i = 0; i < platformspec->platforms_n; ++i)
			cpe_platform_delete(platformspec->platforms[i]);
		free(platformspec->platforms);
	}
	free(platformspec);
}


CpePlatform_t* cpe_platform_new_xml(xmlNodePtr node)
{
	CpePlatform_t* ret;
	char* data;

	if (xmlStrcmp(node->name, BAD_CAST "platform") != 0)
		return NULL;

	ret = malloc(sizeof(CpePlatform_t));
	if (ret == NULL) return NULL;
	memset(ret, 0, sizeof(CpePlatform_t));
	
	if ((ret->id = xmlGetProp(node, BAD_CAST "id")) == NULL) {
		cpe_platform_delete(ret);
		return NULL;
	}

	for (node = node->xmlChildrenNode; node != NULL; node = node->next) {
		if (ret->title == NULL && xmlStrcmp(node->name, BAD_CAST "title") == 0)
			ret->title = xmlNodeGetContent(node);
		else if (ret->remark == NULL && xmlStrcmp(node->name, BAD_CAST "remark") == 0)
			ret->remark = xmlNodeGetContent(node);
		else if (!ret->expr.oper && xmlStrcmp(node->name, BAD_CAST "logical-test") == 0)
			cpe_langexpr_new(&(ret->expr), node);
	}

	return ret;
}

bool cpe_language_match_expr(Cpe_t** cpe, size_t n, const CpeLangExpr_t* expr)
{
	CpeLangExpr_t* cur;
	bool ret;

	switch (expr->oper & CPE_LANG_OPER_MASK) {
		case CPE_LANG_OPER_AND:
			ret = true;
			for (cur = expr->meta.expr; cur->oper; ++cur) {
				if(!cpe_language_match_expr(cpe, n, cur)) {
					ret = false;
					break;
				}
			}
			break;
		case CPE_LANG_OPER_OR:
			ret = false;
			for (cur = expr->meta.expr; cur->oper; ++cur) {
				if(cpe_language_match_expr(cpe, n, cur)) {
					ret = true;
					break;
				}
			}
			break;
		case CPE_LANG_OPER_MATCH:
			ret = cpe_name_match_cpes(expr->meta.cpe, n, cpe);
			break;
		default: return false;
	}
	
	return (expr->oper & CPE_LANG_OPER_NOT ? !ret : ret);
}

bool cpe_language_match_cpe(Cpe_t** cpe, size_t n, const CpePlatform_t* platform)
{
	return cpe_language_match_expr(cpe, n, &platform->expr);
}

/*
bool cpe_language_match_str(const char* cpe, const CpePlatform_t* platform)
{
	Cpe_t* cpe_;
	bool ret;

	cpe_ = cpe_new(cpe);
	if (cpe_ == NULL) return false;
	ret = cpe_language_match_cpe(cpe_, platform);
	cpe_delete(cpe_);
	return ret;
}
*/

void cpe_platform_delete(CpePlatform_t* platform)
{
	if (platform) {
		xmlFree(platform->id);
		xmlFree(platform->title);
		xmlFree(platform->remark);
		cpe_langexpr_delete(&platform->expr);
	}
	free(platform);
}

bool cpe_langexpr_new(CpeLangExpr_t* ret, xmlNodePtr node)
{
	xmlChar* temp;
	xmlNodePtr cur;
	int i;
	size_t elem_cnt = 0;

	if (xmlStrcmp(node->name, BAD_CAST "fact-ref") == 0) {
		ret->oper = CPE_LANG_OPER_MATCH;
		temp = xmlGetProp(node, BAD_CAST "name");
		ret->meta.cpe = cpe_new(temp);
		xmlFree(temp);
		return (ret->meta.cpe ? true : false);
	}

	if (xmlStrcmp(node->name, BAD_CAST "logical-test") != 0)
		return false;

	temp = xmlGetProp(node, BAD_CAST "operator");
	if (xmlStrcasecmp(temp, BAD_CAST "AND") == 0)
		ret->oper = CPE_LANG_OPER_AND;
	else if (xmlStrcasecmp(temp, BAD_CAST "OR") == 0)
		ret->oper = CPE_LANG_OPER_OR;
	else {
		xmlFree(temp);
		free(ret);
		return false;
	}
	xmlFree(temp);

	temp = xmlGetProp(node, BAD_CAST "negate");
	if (temp && xmlStrcasecmp(temp, BAD_CAST "TRUE") == 0)
		ret->oper |= CPE_LANG_OPER_NOT;
	xmlFree(temp);

	for (cur = node->xmlChildrenNode; cur != NULL; cur = cur->next) ++elem_cnt;

	ret->meta.expr = malloc((elem_cnt + 1) * sizeof(CpeLangExpr_t));
	if (ret->meta.expr == NULL)
		return false;

	for (i = 0, node = node->xmlChildrenNode; node != NULL; node = node->next)
		if (cpe_langexpr_new(&ret->meta.expr[i], node)) ++i;
	ret->meta.expr[i].oper = CPE_LANG_OPER_HALT;

	return true;
}

void cpe_langexpr_delete(CpeLangExpr_t* expr)
{
	CpeLangExpr_t* cur;

	if (expr == NULL)
		return;	

	switch (expr->oper & CPE_LANG_OPER_MASK) {
		case CPE_LANG_OPER_AND:
		case CPE_LANG_OPER_OR:
			for (cur = expr->meta.expr; cur->oper; ++cur)
				cpe_langexpr_delete(cur);
			free(expr->meta.expr);
			break;
		case CPE_LANG_OPER_MATCH:
			cpe_delete(expr->meta.cpe);
			break;
		default: break;
	}

	expr->oper = 0;
}



