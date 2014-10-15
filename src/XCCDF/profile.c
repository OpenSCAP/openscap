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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include "item.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include "common/debug_priv.h"

struct xccdf_setvalue *xccdf_setvalue_new(void)
{
	return oscap_calloc(1, sizeof(struct xccdf_setvalue));
}

struct xccdf_setvalue *xccdf_setvalue_clone(const struct xccdf_setvalue * old_value)
{
	struct xccdf_setvalue * clone = oscap_calloc(1, sizeof(struct xccdf_setvalue));
	clone->item = oscap_strdup(old_value->item);
	clone->value = oscap_strdup(old_value->value);
	return clone;
}

struct xccdf_setvalue *xccdf_setvalue_new_parse(xmlTextReaderPtr reader)
{
	const char *id = xccdf_attribute_get(reader, XCCDFA_IDREF);
	if (id == NULL)
		return NULL;
	struct xccdf_setvalue *sv = oscap_calloc(1, sizeof(struct xccdf_setvalue));
	sv->item = oscap_strdup(id);
	sv->value = oscap_element_string_copy(reader);
	return sv;
}

xmlNode *xccdf_setvalue_to_dom(struct xccdf_setvalue *setvalue, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info)
{
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);

	xmlNode *setvalue_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "set-value",  BAD_CAST setvalue->value);

	if (setvalue->item)
		xmlNewProp(setvalue_node, BAD_CAST "idref", BAD_CAST setvalue->item);

	return setvalue_node;
}

void xccdf_setvalue_free(struct xccdf_setvalue *sv)
{
	if (sv) {
		oscap_free(sv->item);
		oscap_free(sv->value);
		oscap_free(sv);
	}
}

struct xccdf_refine_value *xccdf_refine_value_new(void)
{
	struct xccdf_refine_value *foo = oscap_calloc(1, sizeof(struct xccdf_refine_value));
	foo->remarks = oscap_list_new();
	return foo;
}

struct xccdf_refine_value * xccdf_refine_value_clone(const struct xccdf_refine_value * value)
{
	struct xccdf_refine_value *clone = oscap_calloc(1, sizeof(struct xccdf_refine_value));
	clone->item = oscap_strdup(value->item);
	clone->selector = oscap_strdup(value->selector);
	clone->oper = value->oper;
	clone->remarks = oscap_list_clone(value->remarks, (oscap_clone_func) oscap_text_clone);
	return clone;
}

struct xccdf_refine_rule *xccdf_refine_rule_new(void)
{
	struct xccdf_refine_rule *foo = oscap_calloc(1, sizeof(struct xccdf_refine_rule));
	foo->remarks = oscap_list_new();
	return foo;
}

struct xccdf_refine_rule * xccdf_refine_rule_clone(const struct xccdf_refine_rule * rule)
{
	struct xccdf_refine_rule * clone = oscap_calloc(1, sizeof(struct xccdf_refine_rule));
	clone->item = oscap_strdup(rule->item);
	clone->selector = oscap_strdup(rule->selector);
	clone->role = rule->role;
	clone->severity = rule->severity;
	clone->weight = rule->weight;
	clone->remarks = oscap_list_clone(rule->remarks, (oscap_clone_func) oscap_text_clone);
	return clone;
}	

bool xccdf_refine_rule_weight_defined(const struct xccdf_refine_rule *rule)
{
	return !isnan(rule->weight);
}

struct xccdf_select *xccdf_select_new(void)
{
	struct xccdf_select *foo = oscap_calloc(1, sizeof(struct xccdf_select));
	foo->remarks = oscap_list_new();
	return foo;
}

struct xccdf_select *xccdf_select_clone(const struct xccdf_select * sel)
{
	struct xccdf_select *clone = oscap_calloc(1, sizeof(struct xccdf_select));
	clone->item     = oscap_strdup(sel->item);
	clone->remarks  = oscap_list_clone(sel->remarks, (oscap_clone_func) oscap_text_clone);
	clone->selected = sel->selected;
	return clone;
}

void xccdf_refine_value_free(struct xccdf_refine_value *rv)
{
	if (rv) {
		oscap_free(rv->item);
		oscap_list_free(rv->remarks, (oscap_destruct_func) oscap_text_free);
		oscap_free(rv->selector);
		oscap_free(rv);
	}
}

void xccdf_refine_rule_free(struct xccdf_refine_rule *rr)
{
	if (rr) {
		oscap_free(rr->item);
		oscap_list_free(rr->remarks, (oscap_destruct_func) oscap_text_free);
		oscap_free(rr->selector);
		oscap_free(rr);
	}
}

void xccdf_select_free(struct xccdf_select *sel)
{
	if (sel) {
		oscap_free(sel->item);
		oscap_list_free(sel->remarks, (oscap_destruct_func) oscap_text_free);
		oscap_free(sel);
	}
}

static void xccdf_select_dump(struct xccdf_select *sel, int depth)
{
	xccdf_print_depth(depth);
	printf("sel %c= %s\n", (sel->selected ? '+' : '-'), (sel->item ? sel->item : "(unknown)"));
	// oscap_text_dump(); // TODO
}

static void xccdf_refine_value_dump(struct xccdf_refine_value *rv, int depth)
{
	xccdf_print_depth(depth);
	printf("%s: selector='%s', operator='%s'\n", 
			rv->item, rv->selector, oscap_enum_to_string(XCCDF_OPERATOR_MAP, rv->oper));
	// oscap_text_dump(); // TODO
}

void xccdf_setvalue_dump(struct xccdf_setvalue *sv, int depth)
{
	xccdf_print_depth(depth); printf("%s: %s\n", sv->item, sv->value);
}


struct xccdf_item *xccdf_profile_new_internal(struct xccdf_item *bench)
{
	if (bench)
		assert(bench->type == XCCDF_BENCHMARK);
	struct xccdf_item *prof = xccdf_item_new(XCCDF_PROFILE, bench);
	prof->sub.profile.selects = oscap_list_new();
	prof->sub.profile.setvalues = oscap_list_new();
	prof->sub.profile.refine_values = oscap_list_new();
	prof->sub.profile.refine_rules = oscap_list_new();
	prof->sub.profile.tailoring = false;
	return prof;
}

struct xccdf_profile *xccdf_profile_new(void)
{
    return XPROFILE(xccdf_profile_new_internal(NULL));
}

struct xccdf_profile *xccdf_profile_clone (const struct xccdf_profile *old_profile)
{
	struct xccdf_item *new_profile = oscap_calloc(1, sizeof(struct xccdf_item) + sizeof(struct xccdf_profile_item));
	struct xccdf_item *old = XITEM(old_profile);
    xccdf_item_base_clone(&new_profile->item, &(old->item));
	new_profile->type = old->type;
    xccdf_profile_item_clone(&new_profile->sub.profile, &old->sub.profile);
	return XPROFILE(new_profile);
}


const struct oscap_string_map XCCDF_ROLE_MAP[] = {
	{XCCDF_ROLE_FULL, "full"},
	{XCCDF_ROLE_UNSCORED, "unscored"},
	{XCCDF_ROLE_UNCHECKED, "unchecked"},
	{0, NULL}
};

static void xccdf_parse_remarks(xmlTextReaderPtr reader, struct oscap_list* list, int depth)
{
	while (oscap_to_start_element(reader, depth))
		if (xccdf_element_get(reader) == XCCDFE_REMARK)
			oscap_list_add(list, oscap_text_new_parse(XCCDF_TEXT_PLAIN, reader));
}

struct xccdf_item *xccdf_profile_parse(xmlTextReaderPtr reader, struct xccdf_item *bench)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_PROFILE);
	struct xccdf_item *prof = xccdf_profile_new_internal(bench);

	if (!xccdf_item_process_attributes(prof, reader)) {
		xccdf_profile_free(prof);
		return NULL;
	}

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_SELECT:{
				struct xccdf_select *sel = xccdf_select_new();
				sel->selected = xccdf_attribute_get_bool(reader, XCCDFA_SELECTED);
				sel->item = xccdf_attribute_copy(reader, XCCDFA_IDREF);
				xccdf_parse_remarks(reader, sel->remarks, depth + 1);
				oscap_list_add(prof->sub.profile.selects, sel);
				break;
			}
		case XCCDFE_REFINE_RULE:{
				const char *id = xccdf_attribute_get(reader, XCCDFA_IDREF);
				if (id == NULL)
					break;
				struct xccdf_refine_rule *rr = xccdf_refine_rule_new();
				rr->item = oscap_strdup(id);
				rr->selector = xccdf_attribute_copy(reader, XCCDFA_SELECTOR);
				rr->weight = xccdf_attribute_get_float(reader, XCCDFA_WEIGHT);
				if (xccdf_attribute_has(reader, XCCDFA_ROLE))
					rr->role =
					    oscap_string_to_enum(XCCDF_ROLE_MAP,
								 xccdf_attribute_get(reader, XCCDFA_ROLE));
				if (xccdf_attribute_has(reader, XCCDFA_SEVERITY))
					rr->severity =
					    oscap_string_to_enum(XCCDF_LEVEL_MAP,
								 xccdf_attribute_get(reader, XCCDFA_SEVERITY));
				xccdf_parse_remarks(reader, rr->remarks, depth + 1);
				oscap_list_add(prof->sub.profile.refine_rules, rr);
				break;
			}
		case XCCDFE_REFINE_VALUE:{
				const char *id = xccdf_attribute_get(reader, XCCDFA_IDREF);
				if (id == NULL)
					break;
				struct xccdf_refine_value *rv = xccdf_refine_value_new();
				rv->item = oscap_strdup(id);
				rv->selector = xccdf_attribute_copy(reader, XCCDFA_SELECTOR);
				if (xccdf_attribute_has(reader, XCCDFA_OPERATOR))
					rv->oper =
					    oscap_string_to_enum(XCCDF_OPERATOR_MAP,
								 xccdf_attribute_get(reader, XCCDFA_OPERATOR));
				xccdf_parse_remarks(reader, rv->remarks, depth + 1);
				oscap_list_add(prof->sub.profile.refine_values, rv);
				break;
			}
		case XCCDFE_SET_VALUE:{
				oscap_list_add(prof->sub.profile.setvalues, xccdf_setvalue_new_parse(reader));
				break;
			}
		default:
			if (!xccdf_item_process_element(prof, reader))
				dW("Encountered an unknown element '%s' while parsing XCCDF profile.",
				   xmlTextReaderConstLocalName(reader));
		}
		xmlTextReaderRead(reader);
	}

	return prof;
}

void xccdf_profile_to_dom(struct xccdf_profile *profile, xmlNode *profile_node, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info *version_info)
{
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);

	/* Handle attributes */
	if (xccdf_profile_get_abstract(profile))
		xmlNewProp(profile_node, BAD_CAST "abstract", BAD_CAST "true");

	const char *extends = xccdf_profile_get_extends(profile);
	if (extends)
		xmlNewProp(profile_node, BAD_CAST "extends", BAD_CAST extends);

	const char *note_tag = xccdf_profile_get_note_tag(profile);
	if (note_tag)
		xmlNewProp(profile_node, BAD_CAST "note-tag", BAD_CAST note_tag);

	/* Hanlde children */
        struct oscap_string_iterator *platforms = xccdf_profile_get_platforms(profile);
        while (oscap_string_iterator_has_more(platforms)) {
                const char *platform = oscap_string_iterator_next(platforms);
                xmlNewTextChild(profile_node, ns_xccdf, BAD_CAST "platform", BAD_CAST platform);
        }   
        oscap_string_iterator_free(platforms);

	// Internally, we allow selects with the same ID and we let them override each
	// other. This is required for inheritance to work and there is no reason to
	// artificially disallow that in a single Profile.
	//
	// However the XCCDF specification explicitly prohibits clashing selectors
	// in one Profile so we have to make sure we only export the "final" one.
	//
	// We construct a hashmap that contains just the final selectors and then
	// export everything that is in the hashmap.

	struct oscap_htable *final_selects = oscap_htable_new();
	struct xccdf_select_iterator *selects = xccdf_profile_get_selects(profile);
	while (xccdf_select_iterator_has_more(selects)) {
		const struct xccdf_select *sel = xccdf_select_iterator_next(selects);
		const char *idref = xccdf_select_get_item(sel);

		if (!idref)
			continue;

		oscap_htable_detach(final_selects, idref);
		oscap_htable_add(final_selects, idref, (void*)sel);
	}
	xccdf_select_iterator_free(selects);

	// We still have to iterate through selects in the profile to maintain
	// order in which the selects were loaded.
	selects = xccdf_profile_get_selects(profile);
	while (xccdf_select_iterator_has_more(selects)) {
		const struct xccdf_select *sel = xccdf_select_iterator_next(selects);
		const char *idref = xccdf_select_get_item(sel);

		if (idref && oscap_htable_get(final_selects, idref) != sel)
			continue;

		xmlNode *select_node = xmlNewTextChild(profile_node, ns_xccdf, BAD_CAST "select", NULL);

		if (idref)
			xmlNewProp(select_node, BAD_CAST "idref", BAD_CAST idref);

		if (xccdf_select_get_selected(sel))
			xmlNewProp(select_node, BAD_CAST "selected", BAD_CAST "true");
		else
			xmlNewProp(select_node, BAD_CAST "selected", BAD_CAST "false");

		xccdf_texts_to_dom(xccdf_select_get_remarks(sel), select_node, "remark");
	}
	xccdf_select_iterator_free(selects);
	oscap_htable_free0(final_selects);

	// More or less the same situation as with selects, let us only serialize
	// the last effective setvalue.
	struct oscap_htable *final_setvalues = oscap_htable_new();
	struct xccdf_setvalue_iterator *setvalues = xccdf_profile_get_setvalues(profile);
	while (xccdf_setvalue_iterator_has_more(setvalues)) {
		const struct xccdf_setvalue *setvalue = xccdf_setvalue_iterator_next(setvalues);
		const char *idref = xccdf_setvalue_get_item(setvalue);

		if (!idref)
			continue;

		oscap_htable_detach(final_setvalues, idref);
		oscap_htable_add(final_setvalues, idref, (void*)setvalue);
	}
	xccdf_setvalue_iterator_free(setvalues);

	setvalues = xccdf_profile_get_setvalues(profile);
	while (xccdf_setvalue_iterator_has_more(setvalues)) {
		struct xccdf_setvalue *setvalue = xccdf_setvalue_iterator_next(setvalues);
		const char *idref = xccdf_setvalue_get_item(setvalue);

		if (idref && oscap_htable_get(final_setvalues, idref) != setvalue)
			continue;

		xccdf_setvalue_to_dom(setvalue, doc, profile_node, version_info);
	}
	xccdf_setvalue_iterator_free(setvalues);
	oscap_htable_free0(final_setvalues);

	struct xccdf_refine_value_iterator *refine_values = xccdf_profile_get_refine_values(profile);
	while (xccdf_refine_value_iterator_has_more(refine_values)) {
		struct xccdf_refine_value *refine_value = xccdf_refine_value_iterator_next(refine_values);
		xmlNode *refval_node = xmlNewTextChild(profile_node, ns_xccdf, BAD_CAST "refine-value", NULL);

		const char *idref = xccdf_refine_value_get_item(refine_value);
		if (idref)
			xmlNewProp(refval_node, BAD_CAST "idref", BAD_CAST idref);

		xccdf_operator_t operator = xccdf_refine_value_get_oper(refine_value);
		if (operator != 0)
			xmlNewProp(refval_node, BAD_CAST "operator", BAD_CAST XCCDF_OPERATOR_MAP[operator - 1].string);

		const char *selector = xccdf_refine_value_get_selector(refine_value);
		if (selector)
			xmlNewProp(refval_node, BAD_CAST "selector", BAD_CAST selector);

		xccdf_texts_to_dom(xccdf_refine_value_get_remarks(refine_value), refval_node, "remark");
	}
	xccdf_refine_value_iterator_free(refine_values);

	struct xccdf_refine_rule_iterator *refine_rules = xccdf_profile_get_refine_rules(profile);
	while (xccdf_refine_rule_iterator_has_more(refine_rules)) {
		struct xccdf_refine_rule *refine_rule = xccdf_refine_rule_iterator_next(refine_rules);
		xmlNode *refrule_node = xmlNewTextChild(profile_node, ns_xccdf, BAD_CAST "refine-rule", NULL);

		const char *idref = xccdf_refine_rule_get_item(refine_rule);
		if (idref)
			xmlNewProp(refrule_node, BAD_CAST "idref", BAD_CAST idref);

		xccdf_role_t role = xccdf_refine_rule_get_role(refine_rule);
		if (role != 0)
			xmlNewProp(refrule_node, BAD_CAST "role", BAD_CAST XCCDF_ROLE_MAP[role - 1].string);

		const char *selector = xccdf_refine_rule_get_selector(refine_rule);
		if (selector)
			xmlNewProp(refrule_node, BAD_CAST "selector", BAD_CAST selector);

		xccdf_level_t severity = xccdf_refine_rule_get_severity(refine_rule);
		if (severity != XCCDF_LEVEL_NOT_DEFINED)
			xmlNewProp(refrule_node, BAD_CAST "severity", BAD_CAST XCCDF_LEVEL_MAP[severity - 1].string);

		if (xccdf_refine_rule_weight_defined(refine_rule)) {
			float weight = xccdf_refine_rule_get_weight(refine_rule);
			char *weight_str = oscap_sprintf("%f", weight);
			xmlNewProp(refrule_node, BAD_CAST "weight", BAD_CAST weight_str);
			oscap_free(weight_str);
		}
		
		xccdf_texts_to_dom(xccdf_refine_rule_get_remarks(refine_rule), refrule_node, "remark");
	}
	xccdf_refine_rule_iterator_free(refine_rules);
}

void xccdf_profile_dump(struct xccdf_item *prof, int depth)
{
	xccdf_print_depth(depth);
	printf("Profile : %s\n", (prof ? prof->item.id : "(NULL)"));
	if (prof == NULL)
		return;
	xccdf_item_print(prof, depth + 1);
	xccdf_print_depth(depth + 1); printf("selects ");
	oscap_list_dump(prof->sub.profile.selects, (oscap_dump_func) xccdf_select_dump, depth + 2);
	xccdf_print_depth(depth + 1); printf("refine values ");
	oscap_list_dump(prof->sub.profile.refine_values, (oscap_dump_func) xccdf_refine_value_dump, depth + 2);
	xccdf_print_depth(depth + 1); printf("set values ");
	oscap_list_dump(prof->sub.profile.setvalues, (oscap_dump_func) xccdf_setvalue_dump, depth + 2);
}

void xccdf_profile_free(struct xccdf_item *prof)
{
	if (prof) {
		oscap_list_free(prof->sub.profile.selects, (oscap_destruct_func) xccdf_select_free);
		oscap_list_free(prof->sub.profile.setvalues, (oscap_destruct_func) xccdf_setvalue_free);
		oscap_list_free(prof->sub.profile.refine_values, (oscap_destruct_func) xccdf_refine_value_free);
		oscap_list_free(prof->sub.profile.refine_rules, (oscap_destruct_func) xccdf_refine_rule_free);
		xccdf_item_release(prof);
	}
}

XCCDF_STATUS_CURRENT(profile)
XCCDF_ACCESSOR_STRING(profile, note_tag)
XCCDF_ACCESSOR_SIMPLE(profile, bool, tailoring)
XCCDF_LISTMANIP(profile, select, selects)
XCCDF_LISTMANIP(profile, setvalue, setvalues)
XCCDF_LISTMANIP(profile, refine_value, refine_values)
XCCDF_LISTMANIP(profile, refine_rule, refine_rules)
XCCDF_ITERATOR_GEN_S(profile_note)
XCCDF_ITERATOR_GEN_S(refine_value)
XCCDF_ITERATOR_GEN_S(refine_rule) XCCDF_ITERATOR_GEN_S(setvalue) XCCDF_ITERATOR_GEN_S(select)
OSCAP_ACCESSOR_STRING(xccdf_select, item)
OSCAP_ACCESSOR_SIMPLE(bool, xccdf_select, selected)
OSCAP_IGETINS(oscap_text, xccdf_select, remarks, remark)
OSCAP_ACCESSOR_STRING(xccdf_refine_rule, item)
OSCAP_ACCESSOR_STRING(xccdf_refine_rule, selector)
OSCAP_ACCESSOR_SIMPLE(xccdf_role_t, xccdf_refine_rule, role)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_refine_rule, severity)
OSCAP_ACCESSOR_SIMPLE(xccdf_numeric, xccdf_refine_rule, weight)
OSCAP_IGETINS(oscap_text, xccdf_refine_rule, remarks, remark)
OSCAP_ACCESSOR_STRING(xccdf_refine_value, item)
OSCAP_ACCESSOR_STRING(xccdf_refine_value, selector)
OSCAP_ACCESSOR_SIMPLE(xccdf_operator_t, xccdf_refine_value, oper)
OSCAP_IGETINS(oscap_text, xccdf_refine_value, remarks, remark)
OSCAP_ACCESSOR_STRING(xccdf_setvalue, item)
OSCAP_ACCESSOR_STRING(xccdf_setvalue, value)
