/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
 * Copyright (C) 2010 Tresys Technology, LLC
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
 *      Josh Adams <jadams@tresys.com>
 */

#include "item.h"
#include "elements.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include <assert.h>
#include <string.h>

bool xccdf_content_parse(xmlTextReaderPtr reader, struct xccdf_item *parent)
{
	assert(parent != NULL);

	struct oscap_list *list = NULL;
	struct xccdf_item *item = NULL;

	switch (parent->type) {
	case XCCDF_BENCHMARK:
		list = parent->sub.benchmark.content;
		break;
	case XCCDF_GROUP:
		list = parent->sub.group.content;
		break;
	default:
		assert(false);
	}

	assert(list != NULL);

	switch (xccdf_element_get(reader)) {
	case XCCDFE_RULE:
		item = xccdf_rule_parse(reader, parent);
		break;
	case XCCDFE_GROUP:
		item = xccdf_group_parse(reader, parent);
		break;
	default:
		assert(false);
	}

	if (item != NULL) {
		oscap_list_add(list, item);
		return true;
	}

	return false;
}

static void xccdf_deps_get(struct xccdf_item *item, struct oscap_list **conflicts, struct oscap_list **requires)
{
	switch (item->type) {
	case XCCDF_RULE:
		if (conflicts)
			*conflicts = item->sub.rule.conflicts;
		if (requires)
			*requires = item->sub.rule.requires;
		break;
	case XCCDF_GROUP:
		if (conflicts)
			*conflicts = item->sub.group.conflicts;
		if (requires)
			*requires = item->sub.group.requires;
		break;
	default:
		assert(false);
	}
}

static bool xccdf_item_parse_deps(xmlTextReaderPtr reader, struct xccdf_item *item)
{
	struct oscap_list *conflicts = NULL;
	struct oscap_list *requires = NULL;
	xccdf_deps_get(item, &conflicts, &requires);

	switch (xccdf_element_get(reader)) {
	case XCCDFE_REQUIRES:{
			struct oscap_list *reqs = oscap_list_new();
			char *ids = xccdf_attribute_copy(reader, XCCDFA_IDREF), *idsstr = ids, *id;

			while ((id = oscap_strsep(&ids, " ")) != NULL) {
				if (strcmp(id, "") == 0) continue;
				oscap_list_add(reqs, oscap_strdup(id));
			}
			if (reqs->itemcount == 0) {
				oscap_list_free(reqs, NULL);
				return false;
			}

			oscap_list_add(requires, reqs);
			oscap_free(idsstr);
			break;
		}
	case XCCDFE_CONFLICTS:
		oscap_list_add(conflicts, xccdf_attribute_copy(reader, XCCDFA_IDREF));
		break;
	default:
		assert(false);
	}

	return true;
}

static void xccdf_items_print_id_list(struct oscap_list *items, const char *sep)
{
	struct oscap_list_item *it;
	if (sep == NULL)
		sep = ", ";
	for (it = items->first; it; it = it->next) {
		if (it != items->first)
			printf("%s", sep);
		printf("%s", (const char *) it->data);
	}
}

static void xccdf_item_dump_deps(struct xccdf_item *item, int depth)
{
	struct oscap_list *conflicts, *requires;
	xccdf_deps_get(item, &conflicts, &requires);

	if (requires->itemcount > 0) {
		struct oscap_list_item *it;
		xccdf_print_depth(depth);
		printf("requires: ");
		for (it = requires->first; it; it = it->next) {
			struct oscap_list *nlist = it->data;
			if (it != requires->first)
				printf(" & ");
			if (nlist->itemcount == 0)
				continue;
			if (nlist->itemcount > 1)
				printf("(");
			xccdf_items_print_id_list(nlist, " | ");
			if (nlist->itemcount > 1)
				printf(")");
		}
		printf("\n");
	}

	if (conflicts->itemcount > 0) {
		xccdf_print_depth(depth);
		printf("conflicts: ");
		xccdf_items_print_id_list(conflicts, " | ");
		printf("\n");
	}
}

struct xccdf_item *xccdf_group_new_internal(struct xccdf_item *parent)
{
	struct xccdf_item *group = xccdf_item_new(XCCDF_GROUP, parent);
	group->sub.group.content = oscap_list_new();
	group->sub.group.requires = oscap_list_new();
	group->sub.group.conflicts = oscap_list_new();
	group->sub.group.values = oscap_list_new();
	return group;
}

struct xccdf_group *xccdf_group_new(void)
{
    return XGROUP(xccdf_group_new_internal(NULL));
}

struct xccdf_item *xccdf_group_parse(xmlTextReaderPtr reader, struct xccdf_item *parent)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_GROUP);

	struct xccdf_item *group = xccdf_group_new_internal(parent);

	if (!xccdf_item_process_attributes(group, reader)) {
		xccdf_group_free(group);
		return NULL;
	}

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_REQUIRES:
		case XCCDFE_CONFLICTS:
			xccdf_item_parse_deps(reader, group);
			break;
		case XCCDFE_GROUP:
		case XCCDFE_RULE:
			xccdf_content_parse(reader, group);
			break;
		case XCCDFE_VALUE:
			oscap_list_add(group->sub.group.values, xccdf_value_parse(reader, group));
			break;
		default:
			xccdf_item_process_element(group, reader);
		}
		xmlTextReaderRead(reader);
	}

	return group;
}

void xccdf_group_dump(struct xccdf_item *group, int depth)
{
	xccdf_print_depth(depth);
	printf("Group : %s\n", (group ? group->item.id : "(NULL)"));
	if (group) {
		xccdf_item_print(group, depth + 1);
		xccdf_print_depth(depth + 1);
		printf("values");
		oscap_list_dump(group->sub.group.values, (oscap_dump_func) xccdf_value_dump, depth + 2);
		xccdf_print_depth(depth + 1);
		printf("content");
		oscap_list_dump(group->sub.group.content, (oscap_dump_func) xccdf_item_dump, depth + 2);
	}
}

static void xccdf_free_strlist(struct oscap_list *list)
{
	if (list) oscap_list_free(list, oscap_free);
}

void xccdf_group_free(struct xccdf_item *group)
{
	if (group) {
		oscap_list_free(group->sub.group.content, (oscap_destruct_func) xccdf_item_free);
		oscap_list_free(group->sub.group.values, (oscap_destruct_func) xccdf_value_free);
		oscap_list_free(group->sub.group.requires, (oscap_destruct_func) xccdf_free_strlist);
		oscap_list_free(group->sub.group.conflicts, oscap_free);
		xccdf_item_release(group);
	}
}

struct xccdf_item *xccdf_rule_new_internal(struct xccdf_item *parent)
{
	struct xccdf_item *rule = xccdf_item_new(XCCDF_RULE, parent);
	rule->sub.rule.role = XCCDF_ROLE_FULL;
	rule->sub.rule.severity = XCCDF_UNKNOWN;
	rule->sub.rule.idents = oscap_list_new();
	rule->sub.rule.checks = oscap_list_new();
	rule->sub.rule.requires = oscap_list_new();
	rule->sub.rule.conflicts = oscap_list_new();
	rule->sub.rule.profile_notes = oscap_list_new();
	rule->sub.rule.fixes = oscap_list_new();
	rule->sub.rule.fixtexts = oscap_list_new();
	return rule;
}

struct xccdf_rule *xccdf_rule_new(void)
{
    return XRULE(xccdf_rule_new_internal(NULL));
}

struct xccdf_item *xccdf_rule_parse(xmlTextReaderPtr reader, struct xccdf_item *parent)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_RULE);

	struct xccdf_item *rule = xccdf_rule_new_internal(parent);

	if (!xccdf_item_process_attributes(rule, reader)) {
		xccdf_rule_free(rule);
		return NULL;
	}
	if (xccdf_attribute_has(reader, XCCDFA_ROLE))
		rule->sub.rule.role = oscap_string_to_enum(XCCDF_ROLE_MAP, xccdf_attribute_get(reader, XCCDFA_ROLE));
	if (xccdf_attribute_has(reader, XCCDFA_SEVERITY))
		rule->sub.rule.severity =
		    oscap_string_to_enum(XCCDF_LEVEL_MAP, xccdf_attribute_get(reader, XCCDFA_SEVERITY));

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_REQUIRES:
		case XCCDFE_CONFLICTS:
			xccdf_item_parse_deps(reader, rule);
			break;
		case XCCDFE_PROFILE_NOTE:{
				const char *tag = xccdf_attribute_get(reader, XCCDFA_TAG);
				if (tag == NULL)
					break;
				struct xccdf_profile_note *note = xccdf_profile_note_new();
				note->reftag = strdup(tag);
				note->text = oscap_text_new_parse(XCCDF_TEXT_PROFNOTE, reader);
				oscap_list_add(rule->sub.rule.profile_notes, note);
				break;
			}
		case XCCDFE_CHECK:{
				struct xccdf_check *check = xccdf_check_parse(reader);
				if (check == NULL)
					break;
				if (check->selector == NULL || strcmp(check->selector, "") == 0)
					rule->sub.rule.check = check;
				oscap_list_add(rule->sub.rule.checks, check);
				break;
			}
		case XCCDFE_FIX:
			oscap_list_add(rule->sub.rule.fixes, xccdf_fix_parse(reader));
			break;
		case XCCDFE_FIXTEXT:
			oscap_list_add(rule->sub.rule.fixtexts, xccdf_fixtext_parse(reader));
			break;
		case XCCDFE_IDENT:
			oscap_list_add(rule->sub.rule.idents, xccdf_ident_parse(reader));
			break;
		default:
			xccdf_item_process_element(rule, reader);
		}
		xmlTextReaderRead(reader);
	}

	return rule;
}

void xccdf_rule_dump(struct xccdf_item *rule, int depth)
{
	xccdf_print_depth(depth);
	printf("Rule : %s\n", (rule ? rule->item.id : "(NULL)"));
	if (rule) {
		xccdf_item_print(rule, depth + 1);
		xccdf_item_dump_deps(rule, depth + 1);
		xccdf_print_depth(depth + 1);
		printf("idents");
		oscap_list_dump(rule->sub.rule.idents, (oscap_dump_func) xccdf_ident_dump, depth + 2);
		xccdf_print_depth(depth + 1);
		printf("checks");
		oscap_list_dump(rule->sub.rule.checks, (oscap_dump_func) xccdf_check_dump, depth + 2);
	}
}

void xccdf_rule_free(struct xccdf_item *rule)
{
	if (rule) {
		oscap_list_free(rule->sub.rule.idents, (oscap_destruct_func) xccdf_ident_free);
		oscap_list_free(rule->sub.rule.checks, (oscap_destruct_func) xccdf_check_free);
		oscap_list_free(rule->sub.rule.profile_notes, (oscap_destruct_func) xccdf_profile_note_free);
		oscap_list_free(rule->sub.rule.fixes, (oscap_destruct_func) xccdf_fix_free);
		oscap_list_free(rule->sub.rule.fixtexts, (oscap_destruct_func) xccdf_fixtext_free);
		oscap_list_free(rule->sub.rule.requires, (oscap_destruct_func) xccdf_free_strlist);
		oscap_list_free(rule->sub.rule.conflicts, oscap_free);
		xccdf_item_release(rule);
	}
}

struct xccdf_ident *xccdf_ident_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_ident));
}

struct xccdf_ident *xccdf_ident_new_fill(const char *id, const char *sys)
{
	struct xccdf_ident *ident = xccdf_ident_new();
	ident->id = strdup(id);
	ident->system = strdup(sys);
	return ident;
}

struct xccdf_ident *xccdf_ident_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_IDENT);
	const char *sys = xccdf_attribute_get(reader, XCCDFA_SYSTEM);
	const char *id = oscap_element_string_get(reader);
	return xccdf_ident_new_fill(id, sys);
}

void xccdf_ident_dump(struct xccdf_ident *ident, int depth)
{
	xccdf_print_depth(depth);
	printf("ident : %s => %s\n", ident->system, ident->id);
}

void xccdf_ident_free(struct xccdf_ident *ident)
{
	if (ident) {
		oscap_free(ident->id);
		oscap_free(ident->system);
		oscap_free(ident);
	}
}

struct xccdf_profile_note *xccdf_profile_note_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_profile_note));
}

void xccdf_profile_note_free(struct xccdf_profile_note *note)
{
	if (note) {
		oscap_free(note->reftag);
		oscap_text_free(note->text);
		oscap_free(note);
	}
}

XCCDF_GENERIC_GETTER(const char *, ident, id) XCCDF_GENERIC_GETTER(const char *, ident, system)

struct xccdf_check *xccdf_check_new(void)
{
	struct xccdf_check *check = oscap_calloc(1, sizeof(struct xccdf_check));
	check->content_refs = oscap_list_new();
	check->imports = oscap_list_new();
	check->exports = oscap_list_new();
	check->children = oscap_list_new();
	return check;
}

static const struct oscap_string_map XCCDF_BOOLOP_MAP[] = {
	{XCCDF_OPERATOR_AND, "and"}, {XCCDF_OPERATOR_AND, "AND"},
	{XCCDF_OPERATOR_OR, "or"}, {XCCDF_OPERATOR_OR, "OR"},
	{0, NULL}
};

struct xccdf_check *xccdf_check_parse(xmlTextReaderPtr reader)
{
	xccdf_element_t el = xccdf_element_get(reader);
	if (el != XCCDFE_CHECK && el != XCCDFE_COMPLEX_CHECK)
		return NULL;
	struct xccdf_check *check = xccdf_check_new();

	check->id = xccdf_attribute_copy(reader, XCCDFA_ID);
	check->system = xccdf_attribute_copy(reader, XCCDFA_SYSTEM);
	check->selector = xccdf_attribute_copy(reader, XCCDFA_SELECTOR);
	check->oper = oscap_string_to_enum(XCCDF_BOOLOP_MAP, xccdf_attribute_get(reader, XCCDFA_OPERATOR));
	if (xccdf_attribute_get_bool(reader, XCCDFA_NEGATE))
		check->oper |= XCCDF_OPERATOR_NOT;

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_CHECK:
		case XCCDFE_COMPLEX_CHECK:
			if (check->oper == 0)
				break;
			oscap_list_add(check->children, xccdf_check_parse(reader));
			break;
		case XCCDFE_CHECK_CONTENT_REF:{
				const char *href = xccdf_attribute_get(reader, XCCDFA_HREF);
				if (href == NULL)
					break;
				struct xccdf_check_content_ref *ref = xccdf_check_content_ref_new();
				ref->name = xccdf_attribute_copy(reader, XCCDFA_NAME);
				ref->href = strdup(href);
				oscap_list_add(check->content_refs, ref);
				break;
			}
		case XCCDFE_CHECK_CONTENT:
			if (check->content == NULL)
				check->content = oscap_element_string_copy(reader);
			break;
		case XCCDFE_CHECK_IMPORT:{
				const char *name = xccdf_attribute_get(reader, XCCDFA_IMPORT_NAME);
				if (name == NULL)
					break;
				struct xccdf_check_import *imp = xccdf_check_import_new();
				imp->name = strdup(name);
				imp->content = oscap_element_string_copy(reader);
				oscap_list_add(check->imports, imp);
				break;
			}
		case XCCDFE_CHECK_EXPORT:{
				const char *name = xccdf_attribute_get(reader, XCCDFA_EXPORT_NAME);
				if (name == NULL)
					break;
				struct xccdf_check_export *exp = xccdf_check_export_new();
				exp->name = strdup(name);
				exp->value = xccdf_attribute_copy(reader, XCCDFA_VALUE_ID);
				oscap_list_add(check->exports, exp);
				break;
			}
		default:
			break;
		}
		xmlTextReaderRead(reader);
	}

	return check;
}

void xccdf_check_dump(struct xccdf_check *check, int depth)
{
	if (check->id) {
		xccdf_print_depth(depth);
		printf("id      : %s\n", check->id);
	}
	if (check->system) {
		xccdf_print_depth(depth);
		printf("system  : %s\n", check->system);
	}
	if (check->selector) {
		xccdf_print_depth(depth);
		printf("selector: %s\n", check->selector);
	}
	if (check->content) {
		xccdf_print_depth(depth);
		printf("content : %s\n", check->content);
	}
	xccdf_print_depth(depth);
	printf("content-refs");
	oscap_list_dump(check->content_refs, (oscap_dump_func) xccdf_check_content_ref_dump, depth + 1);
}

bool xccdf_check_get_complex(const struct xccdf_check *check)
{
	return check->oper ? true : false;
}

void xccdf_check_free(struct xccdf_check *check)
{
	if (check) {
		oscap_list_free(check->content_refs, (oscap_destruct_func) xccdf_check_content_ref_free);
		oscap_list_free(check->imports, (oscap_destruct_func) xccdf_check_import_free);
		oscap_list_free(check->exports, (oscap_destruct_func) xccdf_check_export_free);
		oscap_list_free(check->children, (oscap_destruct_func) xccdf_check_free);
		oscap_free(check->id);
		oscap_free(check->system);
		oscap_free(check->selector);
		oscap_free(check->content);
		oscap_free(check);
	}
}

void xccdf_check_content_ref_dump(struct xccdf_check_content_ref *ref, int depth)
{
	xccdf_print_depth(depth);
	printf("%s (%s)\n", ref->href, ref->name);
}

struct xccdf_check_content_ref *xccdf_check_content_ref_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_check_content_ref));
}

void xccdf_check_content_ref_free(struct xccdf_check_content_ref *ref)
{
	if (ref) {
		oscap_free(ref->name);
		oscap_free(ref->href);
		oscap_free(ref);
	}
}

struct xccdf_check_import *xccdf_check_import_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_check_import));
}

void xccdf_check_import_free(struct xccdf_check_import *item)
{
	if (item) {
		oscap_free(item->name);
		oscap_free(item->content);
		oscap_free(item);
	}
}

struct xccdf_check_export *xccdf_check_export_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_check_export));
}

void xccdf_check_export_free(struct xccdf_check_export *item)
{
	if (item) {
		oscap_free(item->name);
		oscap_free(item->value);
		oscap_free(item);
	}
}

const struct oscap_string_map XCCDF_STRATEGY_MAP[] = {
	{XCCDF_STRATEGY_CONFIGURE, "configure"},
	{XCCDF_STRATEGY_COMBINATION, "combination"},
	{XCCDF_STRATEGY_DISABLE, "disable"},
	{XCCDF_STRATEGY_ENABLE, "enable"},
	{XCCDF_STRATEGY_PATCH, "patch"},
	{XCCDF_STRATEGY_POLICY, "policy"},
	{XCCDF_STRATEGY_RESTRICT, "restrict"},
	{XCCDF_STRATEGY_UPDATE, "update"},
	{XCCDF_STRATEGY_UNKNOWN, NULL}
};

#define XCCDF_FIXCOMMON_PARSE(reader, fix) do { \
	fix->reboot     = xccdf_attribute_get_bool(reader, XCCDFA_REBOOT); \
	fix->strategy   = oscap_string_to_enum(XCCDF_STRATEGY_MAP, xccdf_attribute_get(reader, XCCDFA_STRATEGY)); \
	fix->disruption = oscap_string_to_enum(XCCDF_LEVEL_MAP, xccdf_attribute_get(reader, XCCDFA_DISRUPTION)); \
	fix->complexity = oscap_string_to_enum(XCCDF_LEVEL_MAP, xccdf_attribute_get(reader, XCCDFA_COMPLEXITY)); \
	fix->content    = oscap_element_string_copy(reader); \
	} while (false)

struct xccdf_fix *xccdf_fix_new(void)
{
        return oscap_calloc(1, sizeof(struct xccdf_fix));
}

struct xccdf_fix *xccdf_fix_parse(xmlTextReaderPtr reader)
{
	struct xccdf_fix *fix = xccdf_fix_new();
	fix->id = xccdf_attribute_copy(reader, XCCDFA_ID);
	fix->system = xccdf_attribute_copy(reader, XCCDFA_SYSTEM);
	fix->platform = xccdf_attribute_copy(reader, XCCDFA_PLATFORM);
	XCCDF_FIXCOMMON_PARSE(reader, fix);
	return fix;
}

struct xccdf_fixtext *xccdf_fixtext_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_fixtext));
}

struct xccdf_fixtext *xccdf_fixtext_parse(xmlTextReaderPtr reader)
{
	struct xccdf_fixtext *fix = xccdf_fixtext_new();
	fix->fixref = xccdf_attribute_copy(reader, XCCDFA_FIXREF);
	XCCDF_FIXCOMMON_PARSE(reader, fix);
	return fix;
}

void xccdf_fixtext_free(struct xccdf_fixtext *item)
{
	if (item) {
		oscap_free(item->content);
		oscap_free(item->fixref);
		oscap_free(item);
	}
}

void xccdf_fix_free(struct xccdf_fix *item)
{
	if (item) {
		oscap_free(item->id);
		oscap_free(item->system);
		oscap_free(item->platform);
		oscap_free(item->content);
		oscap_free(item);
	}
}

struct oscap_string_iterator *xccdf_item_get_conflicts(const struct xccdf_item* item)
{
	if (item == NULL) return NULL;
	if (item->type == XCCDF_RULE)  return xccdf_rule_get_conflicts ( XRULE(item));
	if (item->type == XCCDF_GROUP) return xccdf_group_get_conflicts(XGROUP(item));
	return NULL;
}
struct oscap_string_iterator *xccdf_rule_get_conflicts(const struct xccdf_rule* rule)
{
	return oscap_iterator_new(XITEM(rule)->sub.rule.conflicts);
}
struct oscap_string_iterator *xccdf_group_get_conflicts(const struct xccdf_group* group)
{
	return oscap_iterator_new(XITEM(group)->sub.group.conflicts);
}

struct oscap_stringlist_iterator *xccdf_item_get_requires(const struct xccdf_item* item)
{
	if (item == NULL) return NULL;
	if (item->type == XCCDF_RULE)  return xccdf_rule_get_requires ( XRULE(item));
	if (item->type == XCCDF_GROUP) return xccdf_group_get_requires(XGROUP(item));
	return NULL;
}
struct oscap_stringlist_iterator *xccdf_rule_get_requires(const struct xccdf_rule* rule)
{
	return oscap_iterator_new(XITEM(rule)->sub.rule.requires);
}
struct oscap_stringlist_iterator *xccdf_group_get_requires(const struct xccdf_group* group)
{
	return oscap_iterator_new(XITEM(group)->sub.group.requires);
}

void xccdf_rule_to_dom(struct xccdf_rule *rule, xmlNode *rule_node, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);

	/* Handle Attributes */
	const char *extends = xccdf_rule_get_extends(rule);
	if (extends)
		xmlNewProp(rule_node, BAD_CAST "extends", BAD_CAST extends);

	if (xccdf_rule_get_multiple(rule))
		xmlNewProp(rule_node, BAD_CAST "multiple", BAD_CAST "True");

	if (xccdf_rule_get_selected(rule))
		xmlNewProp(rule_node, BAD_CAST "selected", BAD_CAST "True");

	float weight = xccdf_rule_get_weight(rule);
	char weight_str[10];
	sprintf(weight_str, "%f", weight);
	xmlNewProp(rule_node, BAD_CAST "weight", BAD_CAST weight_str);

	xccdf_role_t role = xccdf_rule_get_role(rule);
	xmlNewProp(rule_node, BAD_CAST "role", BAD_CAST XCCDF_ROLE_MAP[role - 1].string);

	xccdf_level_t severity = xccdf_rule_get_severity(rule);
	xmlNewProp(rule_node, BAD_CAST "severity", BAD_CAST XCCDF_LEVEL_MAP[severity].string);

	/* Handle Child Nodes */
	struct oscap_text_iterator *rationales = xccdf_rule_get_rationale(rule);
	while (oscap_text_iterator_has_more(rationales)) {
		struct oscap_text *rationale = oscap_text_iterator_next(rationales);
		xmlNode * child = xmlNewChild(rule_node, ns_xccdf, BAD_CAST "rationale", BAD_CAST oscap_text_get_text(rationale));
                if (oscap_text_get_lang(rationale) != NULL) xmlNewProp(child, BAD_CAST "xml:lang", BAD_CAST oscap_text_get_lang(rationale));
	}
	oscap_text_iterator_free(rationales);

	struct oscap_string_iterator *platforms = xccdf_rule_get_platforms(rule);
	while (oscap_string_iterator_has_more(platforms)) {
		const char *platform = oscap_string_iterator_next(platforms);
		xmlNewChild(rule_node, ns_xccdf, BAD_CAST "platform", BAD_CAST platform);
	}
	oscap_string_iterator_free(platforms);

	struct oscap_stringlist_iterator *lists = xccdf_rule_get_requires(rule);
	while (oscap_stringlist_iterator_has_more(lists)) {
		struct oscap_stringlist *list = oscap_stringlist_iterator_next(lists);
		struct oscap_string_iterator *strings = oscap_stringlist_get_strings(list);
		while (oscap_string_iterator_has_more(strings)) {
			const char *requires = oscap_string_iterator_next(strings);
			xmlNewChild(rule_node, ns_xccdf, BAD_CAST "requires", BAD_CAST requires);
		}
		oscap_string_iterator_free(strings);
	}
	oscap_stringlist_iterator_free(lists);

	struct oscap_string_iterator *conflicts = xccdf_rule_get_conflicts(rule);
	while (oscap_string_iterator_has_more(conflicts)) {
		const char *conflict = oscap_string_iterator_next(conflicts);
		xmlNewChild(rule_node, ns_xccdf, BAD_CAST "conflicts", BAD_CAST conflict);
	}
	oscap_string_iterator_free(conflicts);

	struct xccdf_ident_iterator *idents = xccdf_rule_get_idents(rule);
	while (xccdf_ident_iterator_has_more(idents)) {
		struct xccdf_ident *ident = xccdf_ident_iterator_next(idents);
		xccdf_ident_to_dom(ident, doc, rule_node);
	}
	xccdf_ident_iterator_free(idents);

	struct xccdf_profile_note_iterator *notes = xccdf_rule_get_profile_notes(rule);
	while (xccdf_profile_note_iterator_has_more(notes)) {
		struct xccdf_profile_note *note = xccdf_profile_note_iterator_next(notes);
		xccdf_profile_note_to_dom(note, doc, rule_node);
	}
	xccdf_profile_note_iterator_free(notes);

	struct xccdf_fixtext_iterator *fixtexts = xccdf_rule_get_fixtexts(rule);
	while (xccdf_fixtext_iterator_has_more(fixtexts)) {
		struct xccdf_fixtext *fixtext = xccdf_fixtext_iterator_next(fixtexts);
		xccdf_fixtext_to_dom(fixtext, doc, rule_node);
	}
	xccdf_fixtext_iterator_free(fixtexts);

	struct xccdf_fix_iterator *fixes = xccdf_rule_get_fixes(rule);
	while (xccdf_fix_iterator_has_more(fixes)) {
		struct xccdf_fix *fix = xccdf_fix_iterator_next(fixes);
		xccdf_fix_to_dom(fix, doc, rule_node);
	}
	xccdf_fix_iterator_free(fixes);

	struct xccdf_check_iterator *checks = xccdf_rule_get_checks(rule);
	while (xccdf_check_iterator_has_more(checks)) {
		struct xccdf_check *check = xccdf_check_iterator_next(checks);
		xccdf_check_to_dom(check, doc, rule_node);
	}
	xccdf_check_iterator_free(checks);
}

void xccdf_group_to_dom(struct xccdf_group *group, xmlNode *group_node, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);

	/* Handle Attributes */
	const char *extends = xccdf_group_get_extends(group);
	if (extends)
		xmlNewProp(group_node, BAD_CAST "extends", BAD_CAST extends);

	if (xccdf_group_get_selected(group))
		xmlNewProp(group_node, BAD_CAST "selected", BAD_CAST "True");

	float weight = xccdf_group_get_weight(group);
	char weight_str[10];
	sprintf(weight_str, "%f", weight);
	xmlNewProp(group_node, BAD_CAST "weight", BAD_CAST weight_str);

	/* Handle Child Nodes */
	struct oscap_text_iterator *rationales = xccdf_group_get_rationale(group);
	while (oscap_text_iterator_has_more(rationales)) {
		struct oscap_text *rationale = oscap_text_iterator_next(rationales);
		xmlNode * child = xmlNewChild(group_node, ns_xccdf, BAD_CAST "rationale", BAD_CAST oscap_text_get_text(rationale));
                if (oscap_text_get_lang(rationale) != NULL) xmlNewProp(child, BAD_CAST "xml:lang", BAD_CAST oscap_text_get_lang(rationale));
	}
	oscap_text_iterator_free(rationales);

	struct oscap_string_iterator *platforms = xccdf_group_get_platforms(group);
	while (oscap_string_iterator_has_more(platforms)) {
		const char *platform = oscap_string_iterator_next(platforms);
		xmlNewChild(group_node, ns_xccdf, BAD_CAST "platform", BAD_CAST platform);
	}
	oscap_string_iterator_free(platforms);

	struct oscap_stringlist_iterator *lists = xccdf_group_get_requires(group);
	while (oscap_stringlist_iterator_has_more(lists)) {
		struct oscap_stringlist *list = oscap_stringlist_iterator_next(lists);
		struct oscap_string_iterator *strings = oscap_stringlist_get_strings(list);
		while (oscap_string_iterator_has_more(strings)) {
			const char *requires = oscap_string_iterator_next(strings);
			xmlNewChild(group_node, ns_xccdf, BAD_CAST "requires", BAD_CAST requires);
		}
		oscap_string_iterator_free(strings);
	}
	oscap_stringlist_iterator_free(lists);

	struct oscap_string_iterator *conflicts = xccdf_group_get_conflicts(group);
	while (oscap_string_iterator_has_more(conflicts)) {
		const char *conflict = oscap_string_iterator_next(conflicts);
		xmlNewChild(group_node, ns_xccdf, BAD_CAST "conflicts", BAD_CAST conflict);
	}
	oscap_string_iterator_free(conflicts);

	struct xccdf_item_iterator *items = xccdf_group_get_content(group);
	while (xccdf_item_iterator_has_more(items)) {
		struct xccdf_item *item = xccdf_item_iterator_next(items);
		if (XGROUP(xccdf_item_get_parent(item)) == group) {
			xccdf_item_to_dom(item, doc, group_node);
		}
	}
	xccdf_item_iterator_free(items);
}

XCCDF_STATUS_CURRENT(rule)
XCCDF_STATUS_CURRENT(group)
XCCDF_GROUP_IGETTER(item, content)

XCCDF_ITEM_ADDER_REG(group, rule, content)
XCCDF_ITEM_ADDER_REG(group, group, content)
XCCDF_ITEM_ADDER_REG(group, value, values)

bool xccdf_group_add_content(struct xccdf_group *rule, struct xccdf_item *item)
{
	if (item == NULL) return false;
	switch (xccdf_item_get_type(item)) {
		case XCCDF_RULE:  return xccdf_group_add_rule (rule, XRULE(item));
		case XCCDF_GROUP: return xccdf_group_add_group(rule, XGROUP(item));
		case XCCDF_VALUE: return xccdf_group_add_value(rule, XVALUE(item));
		default: return false;
	}
}

XCCDF_ACCESSOR_STRING(rule, impact_metric)
XCCDF_ACCESSOR_SIMPLE(rule, xccdf_role_t, role)
XCCDF_ACCESSOR_SIMPLE(rule, xccdf_level_t, severity)
XCCDF_RULE_GETTER(struct xccdf_check *, check)
XCCDF_SETTER_GENERIC(rule, check, struct xccdf_check *, xccdf_check_free,)
XCCDF_LISTMANIP(rule, ident, idents)
XCCDF_LISTMANIP(rule, check, checks)
XCCDF_LISTMANIP(rule, profile_note, profile_notes)
XCCDF_LISTMANIP(rule, fix, fixes)
XCCDF_LISTMANIP(rule, fixtext, fixtexts)
XCCDF_ITERATOR_GEN_S(ident)

OSCAP_ACCESSOR_STRING(xccdf_check, id)
OSCAP_ACCESSOR_STRING(xccdf_check, system)
OSCAP_ACCESSOR_STRING(xccdf_check, selector)
OSCAP_ACCESSOR_STRING(xccdf_check, content)
OSCAP_ACCESSOR_SIMPLE(xccdf_bool_operator_t, xccdf_check, oper)
OSCAP_IGETINS(xccdf_check_import, xccdf_check, imports, import)
OSCAP_IGETINS(xccdf_check_export, xccdf_check, exports, export)
OSCAP_IGETINS(xccdf_check_content_ref, xccdf_check, content_refs, content_ref)
OSCAP_IGETINS(xccdf_check, xccdf_check, children, child)
XCCDF_ITERATOR_GEN_S(check_content_ref)
XCCDF_ITERATOR_GEN_S(check_export) XCCDF_ITERATOR_GEN_S(check_import) XCCDF_ITERATOR_GEN_S(check)

OSCAP_ACCESSOR_STRING(xccdf_profile_note, reftag)    OSCAP_ACCESSOR_TEXT(xccdf_profile_note, text)
OSCAP_ACCESSOR_STRING(xccdf_check_import, name)      OSCAP_ACCESSOR_STRING(xccdf_check_import, content)
OSCAP_ACCESSOR_STRING(xccdf_check_export, name)      OSCAP_ACCESSOR_STRING(xccdf_check_export, value)
OSCAP_ACCESSOR_STRING(xccdf_check_content_ref, name) OSCAP_ACCESSOR_STRING(xccdf_check_content_ref, href)

OSCAP_ACCESSOR_SIMPLE(xccdf_strategy_t, xccdf_fixtext, strategy)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_fixtext, disruption)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_fixtext, complexity)
OSCAP_ACCESSOR_SIMPLE(bool, xccdf_fixtext, reboot)
OSCAP_ACCESSOR_STRING(xccdf_fixtext, content)
OSCAP_ACCESSOR_STRING(xccdf_fixtext, fixref) XCCDF_ITERATOR_GEN_S(fixtext)

OSCAP_ACCESSOR_SIMPLE(xccdf_strategy_t, xccdf_fix, strategy)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_fix, disruption)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_fix, complexity)
OSCAP_ACCESSOR_SIMPLE(bool, xccdf_fix, reboot)
OSCAP_ACCESSOR_STRING(xccdf_fix, content)
OSCAP_ACCESSOR_STRING(xccdf_fix, system)
OSCAP_ACCESSOR_STRING(xccdf_fix, platform)
OSCAP_ACCESSOR_STRING(xccdf_fix, id)
XCCDF_ITERATOR_GEN_S(fix)

