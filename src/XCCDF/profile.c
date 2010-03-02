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

#include "item.h"

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

struct xccdf_refine_rule *xccdf_refine_rule_new(void)
{
	struct xccdf_refine_rule *foo = oscap_calloc(1, sizeof(struct xccdf_refine_rule));
	foo->remarks = oscap_list_new();
	return foo;
}

struct xccdf_select *xccdf_select_new(void)
{
	struct xccdf_select *foo = oscap_calloc(1, sizeof(struct xccdf_select));
	foo->remarks = oscap_list_new();
	return foo;
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

static void xccdf_setvalue_dump(struct xccdf_setvalue *sv, int depth)
{
	xccdf_print_depth(depth); printf("%s: %s", sv->item, sv->value);
}


struct xccdf_item *xccdf_profile_new(struct xccdf_item *bench)
{
	if (bench)
		assert(bench->type == XCCDF_BENCHMARK);
	struct xccdf_item *prof = xccdf_item_new(XCCDF_PROFILE, bench, bench);
	prof->sub.profile.selects = oscap_list_new();
	prof->sub.profile.setvalues = oscap_list_new();
	prof->sub.profile.refine_values = oscap_list_new();
	prof->sub.profile.refine_rules = oscap_list_new();
	return prof;
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
			oscap_list_add(list, oscap_text_new_parse(OSCAP_TEXT_TRAITS_PLAIN, reader));
}

struct xccdf_item *xccdf_profile_parse(xmlTextReaderPtr reader, struct xccdf_item *bench)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_PROFILE);
	struct xccdf_item *prof = xccdf_profile_new(bench);

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
				const char *id = xccdf_attribute_get(reader, XCCDFA_IDREF);
				if (id == NULL)
					break;
				struct xccdf_setvalue *sv = oscap_calloc(1, sizeof(struct xccdf_setvalue));
				sv->item = oscap_strdup(id);
				sv->value = oscap_element_string_copy(reader);
				oscap_list_add(prof->sub.profile.setvalues, sv);
				break;
			}
		default:
			xccdf_item_process_element(prof, reader);
		}
		xmlTextReaderRead(reader);
	}

	return prof;
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
XCCDF_PROFILE_GETTER(const char *, note_tag)
XCCDF_PROFILE_IGETTER(select, selects)
XCCDF_PROFILE_IGETTER(setvalue, setvalues)
XCCDF_PROFILE_IGETTER(refine_value, refine_values)
XCCDF_PROFILE_IGETTER(refine_rule, refine_rules)
XCCDF_ITERATOR_GEN_S(profile_note)
XCCDF_ITERATOR_GEN_S(refine_value)
XCCDF_ITERATOR_GEN_S(refine_rule) XCCDF_ITERATOR_GEN_S(setvalue) XCCDF_ITERATOR_GEN_S(select)
OSCAP_GETTER(const char *, xccdf_select, item)
OSCAP_GETTER(bool, xccdf_select, selected)
OSCAP_IGETTER(oscap_text, xccdf_select, remarks)
OSCAP_GETTER(const char *, xccdf_refine_rule, item)
OSCAP_GETTER(const char *, xccdf_refine_rule, selector)
OSCAP_GETTER(xccdf_role_t, xccdf_refine_rule, role)
OSCAP_GETTER(xccdf_level_t, xccdf_refine_rule, severity)
OSCAP_IGETTER(oscap_text, xccdf_refine_rule, remarks)
OSCAP_GETTER(const char *, xccdf_refine_value, item)
OSCAP_GETTER(const char *, xccdf_refine_value, selector)
OSCAP_GETTER(xccdf_operator_t, xccdf_refine_value, oper)
OSCAP_IGETTER(oscap_text, xccdf_refine_value, remarks)
OSCAP_GETTER(const char *, xccdf_setvalue, item)
OSCAP_GETTER(const char *, xccdf_setvalue, value)
