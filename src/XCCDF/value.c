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
#include <string.h>

#include "item.h"
#include "elements.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include "common/elements.h"
#include "common/debug_priv.h"
#include "oscap_helpers.h"

static struct xccdf_value_instance *xccdf_value_instance_new(xccdf_value_type_t type);
static struct xccdf_value_instance *_xccdf_value_get_instance_by_selector_internal(const struct xccdf_value *value, const char *selector);

struct xccdf_item *xccdf_value_new_internal(struct xccdf_item *parent, xccdf_value_type_t type)
{
	struct xccdf_item *val = xccdf_item_new(XCCDF_VALUE, parent);
	val->sub.value.type = type;
	val->sub.value.interface_hint = 0;
	val->sub.value.oper = 0;

	val->sub.value.instances = oscap_list_new();
	val->sub.value.sources = oscap_list_new();

	return val;
}

struct xccdf_value *xccdf_value_new(xccdf_value_type_t type)
{
    return XVALUE(xccdf_value_new_internal(NULL, type));
}

struct xccdf_value * xccdf_value_clone(const struct xccdf_value * value)
{
	struct xccdf_item *new_value = calloc(1, sizeof(struct xccdf_item) + sizeof(struct xccdf_value_item));
	struct xccdf_item *old = XITEM(value);
    xccdf_item_base_clone(&new_value->item, &old->item);
	new_value->type = old->type;
    xccdf_value_item_clone(&new_value->sub.value, &XITEM(value)->sub.value);
	return XVALUE(new_value);
}

static const struct oscap_string_map XCCDF_VALUE_TYPE_MAP[] = {
	{XCCDF_TYPE_NUMBER, "number"},
	{XCCDF_TYPE_STRING, "string"},
	{XCCDF_TYPE_BOOLEAN, "boolean"},
	{XCCDF_TYPE_STRING, NULL}
};

static const struct oscap_string_map XCCDF_IFACE_HINT_MAP[] = {
	{XCCDF_IFACE_HINT_CHOICE, "choice"},
	{XCCDF_IFACE_HINT_TEXTLINE, "textline"},
	{XCCDF_IFACE_HINT_TEXT, "text"},
	{XCCDF_IFACE_HINT_DATE, "date"},
	{XCCDF_IFACE_HINT_DATETIME, "datetime"},
	{XCCDF_IFACE_HINT_NONE, NULL}
};

static struct xccdf_value_instance *
_xccdf_value_find_or_create_instance(struct xccdf_value *value, const char *selector, xccdf_value_type_t type)
{
	struct xccdf_value_instance *val = _xccdf_value_get_instance_by_selector_internal(value, selector);
	if (val == NULL) {
		val = xccdf_value_instance_new(type);
		assert(val != NULL);
		xccdf_value_instance_set_selector(val, selector);
		oscap_list_add(XITEM(value)->sub.value.instances, val);
	}
	else
		assert(type == xccdf_value_instance_get_type(val));
	return val;
}

struct xccdf_item *xccdf_value_parse(xmlTextReaderPtr reader, struct xccdf_item *parent)
{
	if (xccdf_element_get(reader) != XCCDFE_VALUE)
		return NULL;
	xccdf_value_type_t type = oscap_string_to_enum(XCCDF_VALUE_TYPE_MAP, xccdf_attribute_get(reader, XCCDFA_TYPE));
	struct xccdf_item *value = xccdf_value_new_internal(parent, type);

	value->sub.value.oper = oscap_string_to_enum(XCCDF_OPERATOR_MAP, xccdf_attribute_get(reader, XCCDFA_OPERATOR));
	value->sub.value.interface_hint =
	    oscap_string_to_enum(XCCDF_IFACE_HINT_MAP, xccdf_attribute_get(reader, XCCDFA_INTERFACEHINT));
	if (!xccdf_item_process_attributes(value, reader)) {
		xccdf_value_free(value);
		return NULL;
	}

	int depth = oscap_element_depth(reader) + 1;

	struct xccdf_value_instance *val;
	while (oscap_to_start_element(reader, depth)) {
		xccdf_element_t el = xccdf_element_get(reader);
		const char *selector = xccdf_attribute_get(reader, XCCDFA_SELECTOR);
		if (selector == NULL) selector = "";

		val = NULL;
		switch (el) {
		case XCCDFE_SOURCE:
			oscap_list_add(value->sub.value.sources, oscap_element_string_copy(reader));
			break;
		case XCCDFE_VALUE_VAL:
			val = _xccdf_value_find_or_create_instance(XVALUE(value), selector, type);
			val->value = oscap_element_string_copy(reader);
			val->flags.value_given = true;
			break;
		case XCCDFE_DEFAULT:
			val = _xccdf_value_find_or_create_instance(XVALUE(value), selector, type);
			val->defval = oscap_element_string_copy(reader);
			val->flags.defval_given = true;
			break;
		case XCCDFE_MATCH:
			if (type == XCCDF_TYPE_STRING) {
				val = _xccdf_value_find_or_create_instance(XVALUE(value), selector, type);
				val->match = oscap_element_string_copy(reader);
			}
			break;
		case XCCDFE_LOWER_BOUND:
			if (type == XCCDF_TYPE_NUMBER) {
				val = _xccdf_value_find_or_create_instance(XVALUE(value), selector, type);
				val->lower_bound = atof(oscap_element_string_get(reader));
			}
			break;
		case XCCDFE_UPPER_BOUND:
			if (type == XCCDF_TYPE_NUMBER) {
				val = _xccdf_value_find_or_create_instance(XVALUE(value), selector, type);
				val->upper_bound = atof(oscap_element_string_get(reader));
			}
			break;
		case XCCDFE_CHOICES:
			val = _xccdf_value_find_or_create_instance(XVALUE(value), selector, type);
			val->flags.must_match = xccdf_attribute_get_bool(reader, XCCDFA_MUSTMATCH);
			val->flags.must_match_given = true;
                        xmlTextReaderRead(reader); /* Move to the next node (subnode of <choices>)*/
			while (oscap_to_start_element(reader, depth + 1)) {
				if (xccdf_element_get(reader) == XCCDFE_CHOICE)
					oscap_list_add(val->choices, oscap_element_string_copy(reader));
				xmlTextReaderRead(reader);
			}
                        break;
		default:
			if (!xccdf_item_process_element(value, reader))
				dW("Encountered an unknown element '%s' while parsing XCCDF group.",
				   xmlTextReaderConstLocalName(reader));
			break;
		}
		xmlTextReaderRead(reader);
	}

	return value;
}

static void xccdf_unit_node(struct oscap_list *list, xccdf_value_type_t type, const char *u, bool given,
						xmlNs *ns, const char *elname, const char *selector)
{
	if (!given) return;
	xmlNode *node = xmlNewNode(ns, BAD_CAST elname);
	if ((selector) && (strlen(selector) > 0)) xmlNewProp(node, BAD_CAST "selector", BAD_CAST selector);
	xmlChar *escaped = xmlEncodeSpecialChars(NULL, BAD_CAST u);
	xmlNodeSetContent(node, escaped);
	xmlFree(escaped);
	oscap_list_add(list, node);
}

void xccdf_value_to_dom(struct xccdf_value *value, xmlNode *value_node, xmlDoc *doc, xmlNode *parent)
{
	const struct xccdf_version_info* version_info = xccdf_item_get_schema_version(XITEM(value));
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);

	/* Handle Attributes */
	const char *extends = xccdf_value_get_extends(value);
	if (extends)
		xmlNewProp(value_node, BAD_CAST "extends", BAD_CAST extends);

	xccdf_operator_t operator = xccdf_value_get_oper(value);
	if (operator != 0)
		xmlNewProp(value_node, BAD_CAST "operator", BAD_CAST XCCDF_OPERATOR_MAP[operator - 1].string);

	xccdf_value_type_t type = xccdf_value_get_type(value);
	if (type != 0)
		xmlNewProp(value_node, BAD_CAST "type", BAD_CAST XCCDF_VALUE_TYPE_MAP[type - 1].string);

	if (xccdf_value_get_interactive(value))
		xmlNewProp(value_node, BAD_CAST "interactive", BAD_CAST "true");

	xccdf_interface_hint_t hint = xccdf_value_get_interface_hint(value);
	if (hint != XCCDF_IFACE_HINT_NONE)
		xmlNewProp(value_node, BAD_CAST "interfaceHint", BAD_CAST XCCDF_IFACE_HINT_MAP[hint - 1].string);

	struct oscap_list *value_nodes       = oscap_list_new();
	struct oscap_list *defval_nodes      = oscap_list_new();
	struct oscap_list *choices_nodes     = oscap_list_new();
	struct oscap_list *lower_bound_nodes = oscap_list_new();
	struct oscap_list *upper_bound_nodes = oscap_list_new();
	struct oscap_list *match_nodes       = oscap_list_new();

	OSCAP_FOR(xccdf_value_instance, inst, xccdf_value_get_instances(value)) {
        char buff[16]; buff[15] = '\0';
		xccdf_unit_node(value_nodes, inst->type, inst->value, inst->flags.value_given, ns_xccdf, "value", inst->selector);
		xccdf_unit_node(defval_nodes, inst->type, inst->defval, inst->flags.defval_given, ns_xccdf, "default", inst->selector);
        snprintf(buff, 15, "%f", inst->lower_bound);
		xccdf_unit_node(lower_bound_nodes, inst->type, buff,
			inst->type == XCCDF_TYPE_NUMBER && !isnan(inst->lower_bound) && buff[0] != 'n', ns_xccdf, "lower-bound", inst->selector);
        snprintf(buff, 15, "%f", inst->upper_bound);
		xccdf_unit_node(upper_bound_nodes, inst->type, buff,
			inst->type == XCCDF_TYPE_NUMBER && !isnan(inst->upper_bound) && buff[0] != 'n', ns_xccdf, "upper-bound", inst->selector);
		xccdf_unit_node(match_nodes, inst->type, inst->match,
			inst->type == XCCDF_TYPE_STRING && inst->match != NULL, ns_xccdf, "match", inst->selector);
        if (inst->choices->first) {
            xmlNode *choices = xmlNewNode(ns_xccdf, BAD_CAST "choices");
            if ((inst->flags.must_match_given) && (inst->flags.must_match))
                xmlNewProp(choices, BAD_CAST "mustMatch", BAD_CAST oscap_enum_to_string(OSCAP_BOOL_MAP, inst->flags.must_match));
            if ((inst->selector)&&(strlen(inst->selector) > 0)) xmlNewProp(choices, BAD_CAST "selector", BAD_CAST inst->selector);
            OSCAP_FOR_STR(ch, xccdf_value_instance_get_choices(inst))
                xmlNewTextChild(choices, ns_xccdf, BAD_CAST "choice", BAD_CAST ch);
            oscap_list_add(choices_nodes, choices);
        }
	}

	struct oscap_list *all_nodes = oscap_list_destructive_join(oscap_list_destructive_join(value_nodes, defval_nodes),
			oscap_list_destructive_join(oscap_list_destructive_join(match_nodes, lower_bound_nodes),
			                            oscap_list_destructive_join(upper_bound_nodes, choices_nodes)));

	struct oscap_iterator *it = oscap_iterator_new(all_nodes);
	while (oscap_iterator_has_more(it)) xmlAddChild(value_node, oscap_iterator_next(it));
	oscap_iterator_free(it);
	oscap_list_free(all_nodes, NULL);
	
	struct oscap_string_iterator *sources = xccdf_value_get_sources(value);
	while (oscap_string_iterator_has_more(sources)) {
		const char *source = oscap_string_iterator_next(sources);
		xmlNewTextChild(value_node, ns_xccdf, BAD_CAST "source", BAD_CAST source);
	}
	oscap_string_iterator_free(sources);
}


void xccdf_value_free(struct xccdf_item *val)
{
    if (val) {
        oscap_list_free(val->sub.value.instances, (oscap_destruct_func)xccdf_value_instance_free);
        oscap_list_free(val->sub.value.sources, free);
        xccdf_item_release(val);
    }
}

bool xccdf_value_set_oper(struct xccdf_value * value, xccdf_operator_t oper)
{
        __attribute__nonnull__(value);

        xccdf_value_to_item(value)->sub.value.oper = oper;
        return true;

}

static void xccdf_string_dump(const char *s, int depth)
{
	xccdf_print_depth(depth);
	printf("%s\n", s);
}

static void
_xccdf_value_type_dump(xccdf_value_type_t type, int depth)
{
	xccdf_print_depth(depth);
	printf("type: ");
	switch (type) {
	case XCCDF_TYPE_NUMBER: printf("number\n"); break;
	case XCCDF_TYPE_STRING: printf("string\n"); break;
	case XCCDF_TYPE_BOOLEAN: printf("boolean\n"); break;
	default:
		assert(false);
	}
}

static void
_xccdf_value_instance_dump(struct xccdf_value_instance *vi, int depth)
{
	xccdf_print_depth(depth++);
	printf("Value Instance: ");
	if (vi == NULL) {
		printf("(NULL)\n");
		return;
	}
	printf("\n");
	xccdf_print_depth(depth);
	printf("selector: '%s'\n", vi->selector == NULL ? "(NULL)" : vi->selector);
	_xccdf_value_type_dump(vi->type, depth);
	xccdf_print_depth(depth);
	printf("value: '%s'\n", vi->value == NULL ? "(NULL)" : vi->value);
	xccdf_print_depth(depth);
	printf("defval:'%s'\n", vi->defval == NULL ? "(NULL)" : vi->defval);
}

void xccdf_value_dump(struct xccdf_item *value, int depth)
{
	xccdf_print_depth(depth++);
	printf("Value : %s\n", (value ? value->item.id : "(NULL)"));
	if (!value)
		return;
	xccdf_item_print(value, depth);
	_xccdf_value_type_dump(value->sub.value.type, depth);
	xccdf_print_depth(depth);
	printf("values");
	oscap_list_dump(value->sub.value.instances, (oscap_dump_func) _xccdf_value_instance_dump, depth + 1);
	if (value->sub.value.sources->itemcount != 0) {
		xccdf_print_depth(depth);
		printf("sources");
		oscap_list_dump(value->sub.value.sources, (oscap_dump_func) xccdf_string_dump, depth + 1);
	}
}

static bool xccdf_value_has_selector(void *inst, void *sel)
{
        if (inst == NULL)
                return false;
	return oscap_streq(((struct xccdf_value_instance *)inst)->selector, sel);
}

static inline struct xccdf_value_instance *
_xccdf_value_get_instance_by_selector_internal(const struct xccdf_value *value, const char *selector)
{
	return oscap_list_find(XITEM(value)->sub.value.instances, (void*)selector, xccdf_value_has_selector);
}

struct xccdf_value_instance *xccdf_value_get_instance_by_selector(const struct xccdf_value *value, const char *selector)
{
	struct xccdf_value_instance *val = _xccdf_value_get_instance_by_selector_internal(value, selector);
	if (val == NULL && oscap_streq(selector, "")) {
		/* From NISTIR-7275r4:
		 * If there is no <xccdf:value> or <xccdf:complex-value> element with an empty
		 * or absent @selector, the first <xccdf:value> or <xccdf:complex-value>
		 * element in top-down processing of the XML SHALL be the default element. */
		struct xccdf_value_instance_iterator *instance_it = xccdf_value_get_instances(value);
		if (xccdf_value_instance_iterator_has_more(instance_it))
			val = xccdf_value_instance_iterator_next(instance_it);
		xccdf_value_instance_iterator_free(instance_it);
	}
	return val;
}

bool xccdf_value_add_instance(struct xccdf_value *value, struct xccdf_value_instance *instance)
{
	if (instance == NULL || value == NULL || xccdf_value_get_type(value) != instance->type)
		return false;
	oscap_list_add(XITEM(value)->sub.value.instances, instance);
	return true;
}

XCCDF_STATUS_CURRENT(value)
XCCDF_VALUE_GETTER(xccdf_value_type_t, type)
XCCDF_VALUE_GETTER(xccdf_interface_hint_t, interface_hint)
XCCDF_VALUE_GETTER(xccdf_operator_t, oper)
XCCDF_VALUE_IGETTER(value_instance, instances)
XCCDF_SIGETTER(value, sources)
XCCDF_ITERATOR_GEN_S(value)
OSCAP_ITERATOR_GEN(xccdf_value_instance)
OSCAP_ITERATOR_REMOVE_F(xccdf_value_instance)


struct xccdf_value_instance *xccdf_value_instance_new(xccdf_value_type_t type)
{
	struct xccdf_value_instance *inst = calloc(1, sizeof(struct xccdf_value_instance));
	inst->lower_bound = NAN;
	inst->upper_bound = NAN;

	inst->type = type;
	inst->choices = oscap_list_new();
	return inst;
}

void xccdf_value_instance_free(struct xccdf_value_instance *inst)
{
	if (inst != NULL) {
		oscap_list_free(inst->choices, free);
		free(inst->selector);
		free(inst->match);
		free(inst->value);
		free(inst->defval);
		free(inst);
	}
}

struct xccdf_value_instance *xccdf_value_new_instance(struct xccdf_value *val)
{
	if (val == NULL) return NULL;
	return xccdf_value_instance_new(XITEM(val)->sub.value.type);
}

#pragma GCC diagnostic ignored "-Wunused-value"
#define XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR_STR(WHAT) \
	const char *xccdf_value_instance_get_##WHAT##_string(const struct xccdf_value_instance *inst) { return inst->WHAT; } \
	bool xccdf_value_instance_set_##WHAT##_string(struct xccdf_value_instance *inst, const char *newval) { \
		free(inst->WHAT); inst->WHAT = oscap_strdup(newval); inst->flags.WHAT##_given = true; return true; }
// TODO: Parse floats correctly instead of converting long to float
#define XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR_NUM(WHAT) \
	xccdf_numeric xccdf_value_instance_get_##WHAT##_number(const struct xccdf_value_instance *inst) { return inst->WHAT == NULL ? NAN : (float)strtol(inst->WHAT, NULL, 10); } \
	bool xccdf_value_instance_set_##WHAT##_number(struct xccdf_value_instance *inst, xccdf_numeric newval) { \
		free(inst->WHAT); inst->WHAT = oscap_sprintf("%f", newval); inst->flags.WHAT##_given = true; return true; }
#define XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR_BOOL(WHAT) \
	bool xccdf_value_instance_get_##WHAT##_boolean(const struct xccdf_value_instance *inst) \
        { return oscap_string_to_enum(OSCAP_BOOL_MAP, inst->WHAT); } \
	bool xccdf_value_instance_set_##WHAT##_boolean(struct xccdf_value_instance *inst, bool newval) \
		{ free(inst->WHAT); inst->WHAT = oscap_strdup(oscap_enum_to_string(OSCAP_BOOL_MAP, newval)); inst->flags.WHAT##_given = true; return true; }

OSCAP_ACCESSOR_STRING(xccdf_value_instance, selector)
OSCAP_GETTER(xccdf_value_type_t, xccdf_value_instance, type)
OSCAP_ACCESSOR_EXP(bool, xccdf_value_instance, must_match, flags.must_match)
OSCAP_ACCESSOR_SIMPLE(xccdf_numeric, xccdf_value_instance, upper_bound)
OSCAP_ACCESSOR_SIMPLE(xccdf_numeric, xccdf_value_instance, lower_bound)
OSCAP_ACCESSOR_STRING(xccdf_value_instance, match)
OSCAP_GETTER(const char*, xccdf_value_instance, value)
OSCAP_IGETTER(oscap_string, xccdf_value_instance, choices)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR_STR(value)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR_STR(defval)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR_NUM(value)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR_NUM(defval)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR_BOOL(value)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR_BOOL(defval)
