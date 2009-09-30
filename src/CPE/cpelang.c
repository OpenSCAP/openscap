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
 */

#include <libxml/tree.h>
#include <string.h>
#include "cpelang.h"
#include "../common/util.h"
#include "../common/list.h"

struct cpe_lang_expr {
	cpe_lang_oper_t oper;	// operator
	union {
		struct cpe_lang_expr *expr;	// array of subexpressions for operators
		struct cpe_name *cpe;	// CPE for match operation
	} meta;			// operation metadata
};

struct cpe_platformspec {
        char *ns_href;
        char *ns_prefix;
	struct oscap_list* items;   // list of items
	struct oscap_htable* item;  // item by ID
};

struct cpe_platform {
	char *id;                   // platform ID
	char *title;                // human-readable platform description
	char *remark;               // remark
	struct cpe_lang_expr expr;	// expression for match evaluation
};

/*
 * New platform specification from XML node
 * @param root XML node to be processed
 * @return new platform specification list
 * @retval NULL on failure
 */
struct cpe_platformspec *cpe_platformspec_new_xml(xmlDocPtr doc, xmlNodePtr root);

/*
 * New platform form XML node
 * @param node XML node to be processed
 * @return new platform specification
 * @retval NULL on failure
 */
struct cpe_platform *cpe_platform_new_xml(xmlNodePtr node);

/*
 * Create new CPE language boolean expression from XML node
 * @param ret pointer to resulting expression
 * @param node XML node to be processed
 * @return true on success
 */
bool cpe_langexpr_new(struct cpe_lang_expr * ret, xmlNodePtr node);

/*
 * Add new platform entry to @a platformspec
 * @note @a platformspec will take over memory management of @a platform
 * @param platformspec list of platforms being extended
 * @param platform platform to add to the list
 * @return true on success
 */
bool cpe_platformspec_add(struct cpe_platformspec * platformspec,
			  struct cpe_platform * platform);

/*
 * Delete single CPE paltform specification
 * @param platform platform to be deleted
 */
void cpe_platform_free(struct cpe_platform * platform);

/*
 * Delete CPE language boolean expression
 * @param expr expression to be deleted
 */
void cpe_langexpr_free(struct cpe_lang_expr * expr);

struct cpe_platformspec *cpe_platformspec_new(const char *fname)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	struct cpe_platformspec *ret;

	if ((doc = xmlParseFile(fname)) == NULL)
		return NULL;

	if ((root = xmlDocGetRootElement(doc)) == NULL)
		return NULL;

	ret = cpe_platformspec_new_xml(doc, root);

	xmlFreeDoc(doc);

	return ret;
}

const size_t CPE_PLATFORMSPEC_EMPTY_INIT_ALLOC = 8;

struct cpe_platformspec *cpe_platformspec_new_empty()
{
	struct cpe_platformspec *res;

	res = oscap_alloc(sizeof(struct cpe_platformspec));
	if (res == NULL)
		return NULL;

	res->items = oscap_list_new();
	res->item = oscap_htable_new();

	return res;
}

struct cpe_platformspec *cpe_platformspec_new_xml(xmlDocPtr doc, xmlNodePtr root)
{
	xmlNodePtr cur;
        xmlNsPtr xmlns;
	struct cpe_platformspec *res;
	struct cpe_platform *plat;

	if (xmlStrcmp(root->name, BAD_CAST "platform-specification") != 0)
		return NULL;

        xmlns = xmlSearchNs(doc, root, BAD_CAST "cpe");
        if (xmlns == NULL)
                return NULL;

	res = cpe_platformspec_new_empty();
	if (res == NULL)
		return NULL;

        // fill namespace href
	res->ns_href = malloc(strlen((xmlns->href)+1));
        if (res->ns_href == NULL)
                return NULL;
        strcpy(res->ns_href, (char *) xmlns->href);

        // fill namespace prefix
	res->ns_prefix = malloc(strlen(xmlns->prefix)+1);
        if (res->ns_prefix == NULL)
                return NULL;
        strcpy(res->ns_prefix, (char *) xmlns->prefix);
        
	for (cur = root->xmlChildrenNode; cur != NULL; cur = cur->next) {
		if (!(plat = cpe_platform_new_xml(cur)))
			continue;
		if (!(cpe_platformspec_add(res, plat))) {
			cpe_platform_free(plat);
			cpe_platformspec_free(res);
			return NULL;
		}
	}

	return res;
}

bool cpe_platformspec_add(struct cpe_platformspec * platformspec,
			  struct cpe_platform * platform)
{
	if (platformspec == NULL || platform == NULL) return false;
	oscap_htable_add(platformspec->item, platform->id, platform);
	return oscap_list_add(platformspec->items, platform);
}

void cpe_platformspec_free(struct cpe_platformspec * platformspec)
{
	if (platformspec) {
		oscap_htable_free(platformspec->item, NULL);
		oscap_list_free(platformspec->items, (oscap_destruct_func)cpe_platform_free);
		oscap_free(platformspec);
	}
}

struct cpe_platform *cpe_platform_new_xml(xmlNodePtr node)
{
	struct cpe_platform *ret;

	if (xmlStrcmp(node->name, BAD_CAST "platform") != 0)
		return NULL;

	ret = oscap_alloc(sizeof(struct cpe_platform));
	if (ret == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cpe_platform));

	if ((ret->id = (char *)xmlGetProp(node, BAD_CAST "id")) == NULL) {
		cpe_platform_free(ret);
		return NULL;
	}

	for (node = node->xmlChildrenNode; node != NULL; node = node->next) {
		if (ret->title == NULL
		    && xmlStrcmp(node->name, BAD_CAST "title") == 0)
			ret->title = (char *)xmlNodeGetContent(node);
		else if (ret->remark == NULL
			 && xmlStrcmp(node->name, BAD_CAST "remark") == 0)
			ret->remark = (char *)xmlNodeGetContent(node);
		else if (!ret->expr.oper
			 && xmlStrcmp(node->name, BAD_CAST "logical-test") == 0)
			cpe_langexpr_new(&(ret->expr), node);
	}

	return ret;
}

bool cpe_language_match_expr(struct cpe_name ** cpe, size_t n,
			     const struct cpe_lang_expr * expr)
{
	struct cpe_lang_expr *cur;
	bool ret;

	switch (expr->oper & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
		ret = true;
		for (cur = expr->meta.expr; cur->oper; ++cur) {
			if (!cpe_language_match_expr(cpe, n, cur)) {
				ret = false;
				break;
			}
		}
		break;
	case CPE_LANG_OPER_OR:
		ret = false;
		for (cur = expr->meta.expr; cur->oper; ++cur) {
			if (cpe_language_match_expr(cpe, n, cur)) {
				ret = true;
				break;
			}
		}
		break;
	case CPE_LANG_OPER_MATCH:
		ret = cpe_name_match_cpes(expr->meta.cpe, n, cpe);
		break;
	default:
		return false;
	}

	return (expr->oper & CPE_LANG_OPER_NOT ? !ret : ret);
}

bool cpe_platform_match_cpe(struct cpe_name ** cpe, size_t n,
			    const struct cpe_platform * platform)
{
	return cpe_language_match_expr(cpe, n, &platform->expr);
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

void cpe_platform_free(struct cpe_platform * platform)
{
	if (platform) {
		xmlFree(platform->id);
		xmlFree(platform->title);
		xmlFree(platform->remark);
		cpe_langexpr_free(&platform->expr);
	}
	oscap_free(platform);
}

bool cpe_langexpr_new(struct cpe_lang_expr * ret, xmlNodePtr node)
{
	xmlChar *temp;
	xmlNodePtr cur;
	int i;
	size_t elem_cnt = 0;

	if (xmlStrcmp(node->name, BAD_CAST "fact-ref") == 0) {
		ret->oper = CPE_LANG_OPER_MATCH;
		temp = xmlGetProp(node, BAD_CAST "name");
		ret->meta.cpe = cpe_name_new((char *)temp);
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
		oscap_free(ret);
		return false;
	}
	xmlFree(temp);

	temp = xmlGetProp(node, BAD_CAST "negate");
	if (temp && xmlStrcasecmp(temp, BAD_CAST "TRUE") == 0)
		ret->oper |= CPE_LANG_OPER_NOT;
	xmlFree(temp);

	for (cur = node->xmlChildrenNode; cur != NULL; cur = cur->next)
		++elem_cnt;

	ret->meta.expr = oscap_alloc((elem_cnt + 1) * sizeof(struct cpe_lang_expr));
	if (ret->meta.expr == NULL)
		return false;

	for (i = 0, node = node->xmlChildrenNode; node != NULL;
	     node = node->next)
		if (cpe_langexpr_new(&ret->meta.expr[i], node))
			++i;
	ret->meta.expr[i].oper = CPE_LANG_OPER_HALT;

	return true;
}

void cpe_langexpr_free(struct cpe_lang_expr * expr)
{
	struct cpe_lang_expr *cur;

	if (expr == NULL)
		return;

	switch (expr->oper & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
	case CPE_LANG_OPER_OR:
		for (cur = expr->meta.expr; cur->oper; ++cur)
			cpe_langexpr_free(cur);
		oscap_free(expr->meta.expr);
		break;
	case CPE_LANG_OPER_MATCH:
		cpe_name_free(expr->meta.cpe);
		break;
	default:
		break;
	}

	expr->oper = 0;
}

/**
 * Private function which is called in dumping model function "cpe_platform_export" to recursively
 * export lang expressions.
 * @relates cpe_platform
 * @param expr lang expression structure
 * @param root_node node in xml model that will be lang expr dumped in
 * @param xmlns xml name space of root node
 */
void cpe_langexpr_export( struct cpe_lang_expr expr, xmlNodePtr root_node, xmlNsPtr xmlns) {

        if (expr.oper == CPE_LANG_OPER_HALT)
            return;

        xmlNodePtr node = NULL;
        if (expr.oper == CPE_LANG_OPER_MATCH) {
                node = xmlNewChild(root_node, xmlns, BAD_CAST "fact-ref", NULL);
                xmlNewProp(node, BAD_CAST "name", BAD_CAST cpe_name_get_uri(expr.meta.cpe));
                return;
        } else {
                node = xmlNewChild(root_node, xmlns, BAD_CAST "logical-test", NULL);
        }
        if (expr.oper == CPE_LANG_OPER_AND) {
                xmlNewProp(node, BAD_CAST "operator", BAD_CAST "AND");
                xmlNewProp(node, BAD_CAST "negate", BAD_CAST "FALSE");
        } else if (expr.oper == CPE_LANG_OPER_OR) {
                xmlNewProp(node, BAD_CAST "operator", BAD_CAST "OR");
                xmlNewProp(node, BAD_CAST "negate", BAD_CAST "FALSE");
        } else if (expr.oper == CPE_LANG_OPER_NOR) {
                xmlNewProp(node, BAD_CAST "operator", BAD_CAST "OR");
                xmlNewProp(node, BAD_CAST "negate", BAD_CAST "TRUE");
        } else if (expr.oper == CPE_LANG_OPER_NAND) {
                xmlNewProp(node, BAD_CAST "operator", BAD_CAST "AND");
                xmlNewProp(node, BAD_CAST "negate", BAD_CAST "TRUE");
        } else {
            // CPE_LANG_OPER_MATCH or raise exception ! We are dumped !
        }

        int i = 0;
        while (expr.meta.expr[i].oper != CPE_LANG_OPER_HALT){
            //printf("%s\n", expr.meta.expr[i].oper);
            cpe_langexpr_export(expr.meta.expr[i], node, xmlns);
            ++i;
        }
    
}


/**
 * Private function which is called in dumping model function "cpe_platformspec_export" to recursively
 * export platforms.
 * @relates cpe_platform
 * @param p pointer to cpe_platform structure for dumping to XML
 * @param root_node node in xml model that will be platform dumped in
 * @param xmlns xml name space of root node
 */
void cpe_platform_export( struct cpe_platform * p, xmlNodePtr root_node, xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;
        xmlNodePtr title_node = NULL;
        node = xmlNewChild(root_node, xmlns, BAD_CAST "platform", NULL); 
        if (cpe_platform_get_id(p) != NULL) 
                xmlNewProp(node, BAD_CAST "id", BAD_CAST cpe_platform_get_id(p));
        if (cpe_platform_get_title(p) != NULL)
                title_node = xmlNewChild(node, xmlns, BAD_CAST "title", BAD_CAST cpe_platform_get_title(p) ); 
        
        cpe_langexpr_export(p->expr, node, xmlns);
}


bool cpe_platformspec_export(const struct cpe_platformspec * res, const char * fname) {


        if (res == NULL)
                return false;

        xmlDocPtr   doc         = NULL;             // document pointer
        xmlNodePtr  root_node   = NULL;             // root node pointer
        xmlNsPtr    xmlns       = NULL;             // xml namespace nodea

        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "platform-specification");
        xmlns = xmlNewNs(root_node, BAD_CAST res->ns_href, BAD_CAST res->ns_prefix);
        root_node->ns = xmlns;
        xmlDocSetRootElement(doc, root_node);

        if (res->items == NULL) {
            xmlFreeDoc(doc);
            return NULL;
        }

	if (res != NULL) {
		OSCAP_FOREACH (cpe_platform, p, cpe_platformspec_get_items(res),
			// dump its contents to XML tree
                        cpe_platform_export( p, root_node, xmlns );
		)
	}

        fflush(stdout);
        xmlSaveFormatFileEnc(fname, doc, "UTF-8", 1);

        xmlFreeDoc(doc);

        return true;
}

OSCAP_GETTER(const char*, cpe_platform, id)
OSCAP_GETTER(const char*, cpe_platform, title)
OSCAP_GETTER(const char*, cpe_platform, remark)
OSCAP_IGETTER_GEN(cpe_platform, cpe_platformspec, items)
OSCAP_HGETTER_STRUCT(cpe_platform, cpe_platformspec, item)

