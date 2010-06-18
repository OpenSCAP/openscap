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
#include "elements.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include "../common/elements.h"
#include <math.h>
#include <string.h>

static struct xccdf_value_instance *xccdf_value_instance_new(xccdf_value_type_t type);

struct xccdf_item *xccdf_value_new_internal(struct xccdf_item *parent, xccdf_value_type_t type)
{
	struct xccdf_item *val = xccdf_item_new(XCCDF_VALUE, parent);
	val->sub.value.type = type;
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
	struct xccdf_item *new_value = oscap_calloc(1, sizeof(struct xccdf_item) + sizeof(struct xccdf_value_item));
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

static union xccdf_value_unit xccdf_value_get(const char *str, xccdf_value_type_t type)
{
	union xccdf_value_unit val;
	memset(&val, 0, sizeof(val));
	if (str == NULL)
		return val;

	switch (type) {
	case XCCDF_TYPE_STRING:
		if (!val.s)
			val.s = strdup(str);
		break;
	case XCCDF_TYPE_NUMBER:
		val.n = strtof(str, NULL);
		break;
	case XCCDF_TYPE_BOOLEAN:
		val.b = oscap_string_to_enum(OSCAP_BOOL_MAP, str);
		break;
	default:
		assert(false);
	}
	return val;
}

char *  xccdf_value_instance_get_value(const struct xccdf_value_instance * val)
{
        char * selected = NULL;
        if (val == NULL)
            return NULL;
        switch (val->type) {
            case XCCDF_TYPE_BOOLEAN:
                    selected = malloc(sizeof(char));
                    sprintf(selected, "%d", val->value.b);
                    break;
            case XCCDF_TYPE_NUMBER:
                    // TODO: compatibility issue: what precision should be here ?
                    selected = malloc(5*sizeof(char));
                    sprintf(selected, "%.5f", val->value.n);
                    break;
            case XCCDF_TYPE_STRING:
                    selected = oscap_strdup(val->value.s);
                    break;
        }

        return selected;
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

	while (oscap_to_start_element(reader, depth)) {
		xccdf_element_t el = xccdf_element_get(reader);
		const char *selector = xccdf_attribute_get(reader, XCCDFA_SELECTOR);
		if (selector == NULL) selector = "";
		struct xccdf_value_instance *val = xccdf_value_get_instance_by_selector(XVALUE(value), selector);
		if (val == NULL) {
			val = xccdf_value_instance_new(type);
			xccdf_value_instance_set_selector(val, selector);
			assert(val != NULL);
			oscap_list_add(value->sub.value.instances, val);
		}

		switch (el) {
		case XCCDFE_SOURCE:
			oscap_list_add(value->sub.value.sources, oscap_element_string_copy(reader));
			break;
		case XCCDFE_VALUE_VAL:
			val->value = xccdf_value_get(oscap_element_string_get(reader), type);
			val->flags.value_given = true;
			break;
		case XCCDFE_DEFAULT:
			val->defval = xccdf_value_get(oscap_element_string_get(reader), type);
			val->flags.defval_given = true;
			break;
		case XCCDFE_MATCH:
			if (type == XCCDF_TYPE_STRING && !val->limits.s.match)
				val->limits.s.match = oscap_element_string_copy(reader);
			break;
		case XCCDFE_LOWER_BOUND:
			if (type == XCCDF_TYPE_NUMBER)
				val->limits.n.lower_bound = xccdf_value_get(oscap_element_string_get(reader), type).n;
			break;
		case XCCDFE_UPPER_BOUND:
			if (type == XCCDF_TYPE_NUMBER)
				val->limits.n.upper_bound = xccdf_value_get(oscap_element_string_get(reader), type).n;
			break;
		case XCCDFE_CHOICES:
			val->flags.must_match = xccdf_attribute_get_bool(reader, XCCDFA_MUSTMATCH);
			val->flags.must_match_given = true;
			while (oscap_to_start_element(reader, depth + 1)) {
				if (xccdf_element_get(reader) == XCCDFE_CHOICE) {
					union xccdf_value_unit *unit = oscap_calloc(1, sizeof(union xccdf_value_unit));
					*unit = xccdf_value_get(oscap_element_string_get(reader), type);
					oscap_list_add(val->choices, unit);
				}
				xmlTextReaderRead(reader);
			}
		default:
			xccdf_item_process_element(value, reader);
		}
		xmlTextReaderRead(reader);
	}

	return value;
}

static void xccdf_unit_node(struct oscap_list *list, xccdf_value_type_t type, union xccdf_value_unit u, bool given,
						xmlNs *ns, const char *elname, const char *selector)
{
	if (!given) return;
	xmlNode *node = xmlNewNode(ns, BAD_CAST elname);
	if (selector) xmlNewProp(node, BAD_CAST "selector", BAD_CAST selector);

	switch (type) {
	case XCCDF_TYPE_STRING: xmlNodeSetContent(node, BAD_CAST u.s); break;
	case XCCDF_TYPE_NUMBER: /* TODO */ break;
	case XCCDF_TYPE_BOOLEAN: /* TODO */ break;
	default: assert(false);
	}

	oscap_list_add(list, node);
}

void xccdf_value_to_dom(struct xccdf_value *value, xmlNode *value_node, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);

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
		xccdf_unit_node(value_nodes, inst->type, inst->value, inst->flags.value_given, ns_xccdf, "value", inst->selector);
		xccdf_unit_node(defval_nodes, inst->type, inst->defval, inst->flags.defval_given, ns_xccdf, "default", inst->selector);
		xccdf_unit_node(lower_bound_nodes, inst->type, (union xccdf_value_unit) inst->limits.n.lower_bound,
			inst->type == XCCDF_TYPE_NUMBER && inst->limits.n.lower_bound != NAN, ns_xccdf, "lower_bound", inst->selector);
		xccdf_unit_node(upper_bound_nodes, inst->type, (union xccdf_value_unit) inst->limits.n.upper_bound,
			inst->type == XCCDF_TYPE_NUMBER && inst->limits.n.upper_bound != NAN, ns_xccdf, "upper_bound", inst->selector);
		xccdf_unit_node(match_nodes, inst->type, (union xccdf_value_unit) inst->limits.s.match,
			inst->type == XCCDF_TYPE_STRING && inst->limits.s.match != NULL, ns_xccdf, "match", inst->selector);
		// TODO choices
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
		xmlNewChild(value_node, ns_xccdf, BAD_CAST "source", BAD_CAST source);
	}
	oscap_string_iterator_free(sources);
}


void xccdf_value_free(struct xccdf_item *val)
{
    if (val) {
        oscap_list_free(val->sub.value.instances, (oscap_destruct_func)xccdf_value_instance_free);
        oscap_list_free(val->sub.value.sources, oscap_free);
        xccdf_item_release(val);
    }
}

static void xccdf_value_unit_s_free(union xccdf_value_unit *u)
{
	oscap_free(u->s);
	oscap_free(u);
}

static oscap_destruct_func xccdf_value_unit_destructor(xccdf_value_type_t type)
{
	switch (type) {
	case XCCDF_TYPE_STRING:
		return (oscap_destruct_func) xccdf_value_unit_s_free;
	case XCCDF_TYPE_NUMBER:
	case XCCDF_TYPE_BOOLEAN:
		return free;
	}
	return NULL;
}

bool xccdf_value_set_oper(struct xccdf_item * value, xccdf_operator_t oper)
{
        __attribute__nonnull__(value);

        value->sub.value.oper = oper;
        return true;

}


static void xccdf_value_instance_n_dump(struct xccdf_value_instance *val, int depth)
{
	xccdf_print_depth(depth);
	printf("%f (default %f, from %f to %f)\n", val->value.n, val->defval.n, val->limits.n.lower_bound,
	       val->limits.n.upper_bound);
}

static void xccdf_value_instance_s_dump(struct xccdf_value_instance *val, int depth)
{
	xccdf_print_depth(depth);
	printf("'%s' (default '%s', match '%s')\n", val->value.s, val->defval.s, val->limits.s.match);
}

static void xccdf_value_instance_b_dump(struct xccdf_value_instance *val, int depth)
{
	xccdf_print_depth(depth);
	printf("%d (default %d)\n", val->value.b, val->defval.b);
}

static void xccdf_string_dump(const char *s, int depth)
{
	xccdf_print_depth(depth);
	printf("%s\n", s);
}

void xccdf_value_dump(struct xccdf_item *value, int depth)
{
	xccdf_print_depth(depth++);
	printf("Value : %s\n", (value ? value->item.id : "(NULL)"));
	if (!value)
		return;
	xccdf_item_print(value, depth);
	void (*valdump) (struct xccdf_value_instance * val, int depth) = NULL;
	xccdf_print_depth(depth);
	printf("type: ");
	switch (value->sub.value.type) {
	case XCCDF_TYPE_NUMBER:
		printf("number\n");
		valdump = xccdf_value_instance_n_dump;
		break;
	case XCCDF_TYPE_STRING:
		printf("string\n");
		valdump = xccdf_value_instance_s_dump;
		break;
	case XCCDF_TYPE_BOOLEAN:
		printf("boolean\n");
		valdump = xccdf_value_instance_b_dump;
		break;
	default:
		assert(false);
	}
	xccdf_print_depth(depth);
	printf("values");
	oscap_list_dump(value->sub.value.instances, (oscap_dump_func) valdump, depth + 1);
	if (value->sub.value.sources->itemcount != 0) {
		xccdf_print_depth(depth);
		printf("sources");
		oscap_list_dump(value->sub.value.sources, (oscap_dump_func) xccdf_string_dump, depth + 1);
	}
}

static bool xccdf_value_has_selector(void *inst, void *sel)
{
	return inst != NULL && oscap_streq(((struct xccdf_value_instance *)inst)->selector, sel);
}

struct xccdf_value_instance *xccdf_value_get_instance_by_selector(const struct xccdf_value *value, const char *selector)
{
	return oscap_list_find(XITEM(value)->sub.value.instances, (void*)selector, xccdf_value_has_selector);
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


struct xccdf_value_instance *xccdf_value_instance_new(xccdf_value_type_t type)
{
	struct xccdf_value_instance *inst = oscap_calloc(1, sizeof(struct xccdf_value_instance));

	switch (type) {
	case XCCDF_TYPE_NUMBER:
		inst->value.n = inst->defval.n = NAN;	//TODO: REPLACE WITH ANSI
		inst->limits.n.lower_bound = NAN;	//TODO: REPLACE WITH ANSI
		inst->limits.n.upper_bound = NAN;	//TODO: REPLACE WITH ANSI
		break;
	case XCCDF_TYPE_STRING:
	case XCCDF_TYPE_BOOLEAN:
		break;
	default:
		oscap_free(inst);
		return NULL;
	}

	inst->type = type;
	inst->choices = oscap_list_new();
	return inst;
}

void xccdf_value_instance_free(struct xccdf_value_instance *inst)
{
	if (inst != NULL) {
		oscap_list_free(inst->choices, xccdf_value_unit_destructor(inst->type));
		oscap_free(inst->selector);
		if (inst->type == XCCDF_TYPE_STRING) {
			oscap_free(inst->limits.s.match);
			oscap_free(inst->value.s);
			oscap_free(inst->defval.s);
		}
		oscap_free(inst);
	}
}

struct xccdf_value_instance *xccdf_value_new_instance(struct xccdf_value *val)
{
	if (val == NULL) return NULL;
	return xccdf_value_instance_new(XITEM(val)->sub.value.type);
}

#pragma GCC diagnostic ignored "-Wunused-value"
#define XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(WHAT, EXPR, TYPE, TYPENAME, TYPECONST, CLONER, DELETER) \
	TYPE xccdf_value_instance_get_##WHAT##TYPENAME(const struct xccdf_value_instance *inst) { \
		if (!inst || inst->type != TYPECONST) return 0; return inst->EXPR; } \
	bool xccdf_value_instance_set_##WHAT##TYPENAME(struct xccdf_value_instance *inst, TYPE newval) { \
		if (!inst || inst->type != TYPECONST) return false; DELETER(inst->EXPR); inst->EXPR = CLONER(newval); return true; }

OSCAP_ACCESSOR_STRING(xccdf_value_instance, selector)
OSCAP_GETTER(xccdf_value_type_t, xccdf_value_instance, type)
OSCAP_ACCESSOR_EXP(bool, xccdf_value_instance, must_match, flags.must_match)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(value, value.b, bool, _boolean, XCCDF_TYPE_BOOLEAN,,)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(value, value.n, xccdf_numeric, _number, XCCDF_TYPE_NUMBER,,)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(value, value.s, const char *, _string, XCCDF_TYPE_STRING, oscap_strdup, oscap_free)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(defval, defval.b, bool, _boolean, XCCDF_TYPE_BOOLEAN,,)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(defval, defval.n, xccdf_numeric, _number, XCCDF_TYPE_NUMBER,,)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(defval, defval.s, const char *, _string, XCCDF_TYPE_STRING, oscap_strdup, oscap_free)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(lower_bound, limits.n.lower_bound, xccdf_numeric,, XCCDF_TYPE_NUMBER,,)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(upper_bound, limits.n.upper_bound, xccdf_numeric,, XCCDF_TYPE_NUMBER,,)
XCCDF_VALUE_INSTANCE_VALUE_ACCESSOR(match, limits.s.match, const char *,, XCCDF_TYPE_STRING, oscap_strdup, oscap_free)
