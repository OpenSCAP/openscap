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

static oscap_destruct_func xccdf_value_val_get_destructor(xccdf_value_type_t type);
static struct xccdf_value_val *xccdf_value_val_new(xccdf_value_type_t type);

struct xccdf_item *xccdf_value_new_internal(struct xccdf_item *parent, xccdf_value_type_t type)
{
	struct xccdf_item *val = xccdf_item_new(XCCDF_VALUE, parent);
	val->sub.value.type = type;
	val->sub.value.values = oscap_htable_new();
	oscap_htable_add(val->sub.value.values, "", xccdf_value_val_new(type));
	val->sub.value.value = oscap_htable_get(val->sub.value.values, "");
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
	new_value->item = *(xccdf_item_base_clone(&(old->item)));
	new_value->type = old->type;
	new_value->sub.value = *(xccdf_value_item_clone(&(old->sub.value)));
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
		if (selector == NULL)
			selector = "";
		struct xccdf_value_val *val = oscap_htable_get(value->sub.value.values, selector);
		if (val == NULL) {
			val = xccdf_value_val_new(type);
			assert(val != NULL);
			oscap_htable_add(value->sub.value.values, selector, val);
		}

		switch (el) {
		case XCCDFE_SOURCE:
			oscap_list_add(value->sub.value.sources, oscap_element_string_copy(reader));
			break;
		case XCCDFE_VALUE_VAL:
			val->value = xccdf_value_get(oscap_element_string_get(reader), type);
			break;
		case XCCDFE_DEFAULT:
			val->defval = xccdf_value_get(oscap_element_string_get(reader), type);
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
			val->must_match = xccdf_attribute_get_bool(reader, XCCDFA_MUSTMATCH);
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

void xccdf_value_to_dom(struct xccdf_value *value, xmlNode *value_node, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);

	/* Handle Attributes */
	const char *extends = xccdf_value_get_extends(value);
	if (extends)
		xmlNewProp(value_node, BAD_CAST "extends", BAD_CAST extends);

	xccdf_operator_t operator = xccdf_value_get_oper(value);
	xmlNewProp(value_node, BAD_CAST "operator", BAD_CAST XCCDF_OPERATOR_MAP[operator - 1].string);

	xccdf_value_type_t type = xccdf_value_get_type(value);
	xmlNewProp(value_node, BAD_CAST "type", BAD_CAST XCCDF_VALUE_TYPE_MAP[type - 1].string);

	if (xccdf_value_get_interactive(value))
		xmlNewProp(value_node, BAD_CAST "interactive", BAD_CAST "True");

	xccdf_interface_hint_t hint = xccdf_value_get_interface_hint(value);
	xmlNewProp(value_node, BAD_CAST "interfaceHint", BAD_CAST XCCDF_IFACE_HINT_MAP[hint - 1].string);

	/* Handle Child Nodes */
	const char *val_str = xccdf_value_get_value_string(value);
	xmlNode *val_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "value", BAD_CAST val_str);
	const char *selector = xccdf_value_get_selector(value);
	if (selector)
		xmlNewProp(val_node, BAD_CAST "Selector", BAD_CAST selector);

	const char *defval_str = xccdf_value_get_defval_string(value);
	xmlNode *defval_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "default", BAD_CAST defval_str);
	const char *defval_selector = xccdf_value_get_selector(value);
	if (defval_selector)
		xmlNewProp(defval_node, BAD_CAST "Selector", BAD_CAST defval_selector);

	const char *match = xccdf_value_get_match(value);
	xmlNode *match_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "match", BAD_CAST match);
	const char *match_selector = xccdf_value_get_selector(value);
	if (match_selector)
		xmlNewProp(match_node, BAD_CAST "Selector", BAD_CAST match_selector);

	xccdf_numeric lower_val = xccdf_value_get_lower_bound(value);
	char lower_str[10];
	*lower_str = '\0';
	snprintf(lower_str, sizeof(lower_str), "%f", lower_val);
	xmlNode *lower_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "lower-bound", BAD_CAST lower_str);
	const char *lower_selector = xccdf_value_get_selector(value);
	if (lower_selector)
		xmlNewProp(lower_node, BAD_CAST "Selector", BAD_CAST lower_selector);

	xccdf_numeric upper_val = xccdf_value_get_upper_bound(value);
	char upper_str[10];
	*upper_str = '\0';
	snprintf(upper_str, sizeof(upper_str), "%f", upper_val);
	xmlNode *upper_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "upper-bound", BAD_CAST upper_str);
	const char *upper_selector = xccdf_value_get_selector(value);
	if (upper_selector)
		xmlNewProp(upper_node, BAD_CAST "Selector", BAD_CAST upper_selector);

	// No support for choices in OpenSCAP
	
	struct oscap_string_iterator *sources = xccdf_value_get_sources(value);
	while (oscap_string_iterator_has_more(sources)) {
		const char *source = oscap_string_iterator_next(sources);
		xmlNewChild(value_node, ns_xccdf, BAD_CAST "source", BAD_CAST source);
	}
	oscap_string_iterator_free(sources);
}


void xccdf_value_free(struct xccdf_item *val)
{
	oscap_htable_free(val->sub.value.values, xccdf_value_val_get_destructor(val->sub.value.type));
	oscap_list_free(val->sub.value.sources, oscap_free);
	xccdf_item_release(val);
}

static struct xccdf_value_val *xccdf_value_val_new(xccdf_value_type_t type)
{
	struct xccdf_value_val *v = oscap_calloc(1, sizeof(struct xccdf_value_val));

	switch (type) {
	case XCCDF_TYPE_NUMBER:
		v->value.n = v->defval.n = NAN;	//TODO: REPLACE WITH ANSI
		v->limits.n.lower_bound = -INFINITY;	//TODO: REPLACE WITH ANSI
		v->limits.n.upper_bound = INFINITY;	//TODO: REPLACE WITH ANSI
		break;
	case XCCDF_TYPE_STRING:
	case XCCDF_TYPE_BOOLEAN:
		break;
	default:
		oscap_free(v);
		return NULL;
	}

	v->choices = oscap_list_new();

	return v;
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

static void xccdf_value_val_free_0(struct xccdf_value_val *v, xccdf_value_type_t type)
{
	oscap_list_free(v->choices, xccdf_value_unit_destructor(type));
	switch (type) {
	case XCCDF_TYPE_STRING:
		oscap_free(v->limits.s.match);
		oscap_free(v->defval.s);
		oscap_free(v->value.s);
		break;
	default:
		break;
	}
	oscap_free(v);
}

static void xccdf_value_val_free_b(struct xccdf_value_val *v)
{
	xccdf_value_val_free_0(v, XCCDF_TYPE_BOOLEAN);
}

static void xccdf_value_val_free_n(struct xccdf_value_val *v)
{
	xccdf_value_val_free_0(v, XCCDF_TYPE_NUMBER);
}

static void xccdf_value_val_free_s(struct xccdf_value_val *v)
{
	xccdf_value_val_free_0(v, XCCDF_TYPE_STRING);
}

static oscap_destruct_func xccdf_value_val_get_destructor(xccdf_value_type_t type)
{
	switch (type) {
	case XCCDF_TYPE_NUMBER:
		return (oscap_destruct_func) xccdf_value_val_free_n;
	case XCCDF_TYPE_BOOLEAN:
		return (oscap_destruct_func) xccdf_value_val_free_b;
	case XCCDF_TYPE_STRING:
		return (oscap_destruct_func) xccdf_value_val_free_s;
	}
	return NULL;
}

bool xccdf_value_get_set_selector(struct xccdf_item * value, const char *selector)
{
	oscap_free(value->sub.value.selector);
	if (!selector)
		selector = "";
	value->sub.value.selector = strdup(selector);
	value->sub.value.value = oscap_htable_get(value->sub.value.values, selector);
	return value->sub.value.value != NULL;
}

bool xccdf_value_set_oper(struct xccdf_item * value, xccdf_operator_t oper)
{
        __attribute__nonnull__(value);

        value->sub.value.oper = oper;
        return true;

}

const char *xccdf_value_get_defval_string(const struct xccdf_value *value)
{
	if (XITEM(value)->sub.value.type != XCCDF_TYPE_STRING || XITEM(value)->sub.value.value == NULL)
		return NULL;
	return XITEM(value)->sub.value.value->defval.s;
}

xccdf_numeric xccdf_value_get_defval_number(const struct xccdf_value * value)
{
	if (XITEM(value)->sub.value.type != XCCDF_TYPE_NUMBER || XITEM(value)->sub.value.value == NULL)
		return NAN;	//TODO: REPLACE WITH ANSI
	return XITEM(value)->sub.value.value->defval.n;
}

bool xccdf_value_get_defval_boolean(const struct xccdf_value * value)
{
	if (XITEM(value)->sub.value.value == NULL)
		return false;
	switch (XITEM(value)->sub.value.type) {
	case XCCDF_TYPE_BOOLEAN:
		return XITEM(value)->sub.value.value->defval.b;
	case XCCDF_TYPE_NUMBER:
		return XITEM(value)->sub.value.value->defval.n != 0.0;
	case XCCDF_TYPE_STRING:
		return XITEM(value)->sub.value.value->defval.s != NULL;
	}
	return false;
}

/*
char *  xccdf_value_get_selected_value(const struct xccdf_value * value)
{
        const char * selector = xccdf_value_get_selector(value);
        char * selected = NULL;
        if (selector == NULL) // default value
                selector = "";
        struct xccdf_value_val *val = oscap_htable_get(XITEM(value)->sub.value.values, selector);
        if (val == NULL)
            return NULL;
        switch (XITEM(value)->sub.value.type) {
            case XCCDF_TYPE_BOOLEAN:
                    selected = malloc(sizeof(char));
                    sprintf(selected, "%b", val->value.b);
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
*/

const char *xccdf_value_get_value_string(const struct xccdf_value *value)
{
	if (XITEM(value)->sub.value.type != XCCDF_TYPE_STRING || XITEM(value)->sub.value.value == NULL)
		return NULL;
	return XITEM(value)->sub.value.value->value.s;
}

xccdf_numeric xccdf_value_get_value_number(const struct xccdf_value * value)
{
	if (XITEM(value)->sub.value.type != XCCDF_TYPE_NUMBER || XITEM(value)->sub.value.value == NULL)
		return NAN;	//TODO: REPLACE WITH ANSI
	return XITEM(value)->sub.value.value->value.n;
}

bool xccdf_value_get_value_boolean(const struct xccdf_value * value)
{
	if (XITEM(value)->sub.value.value == NULL)
		return false;
	switch (XITEM(value)->sub.value.type) {
	case XCCDF_TYPE_BOOLEAN:
		return XITEM(value)->sub.value.value->value.b;
	case XCCDF_TYPE_NUMBER:
		return XITEM(value)->sub.value.value->value.n != 0.0;
	case XCCDF_TYPE_STRING:
		return XITEM(value)->sub.value.value->value.s != NULL;
	}
	return false;
}

xccdf_numeric xccdf_value_get_lower_bound(const struct xccdf_value * value)
{
	if (XITEM(value)->sub.value.type != XCCDF_TYPE_NUMBER || XITEM(value)->sub.value.value == NULL)
		return NAN;	//TODO: REPLACE WITH ANSI
	return XITEM(value)->sub.value.value->limits.n.lower_bound;
}

xccdf_numeric xccdf_value_get_upper_bound(const struct xccdf_value * value)
{
	if (XITEM(value)->sub.value.type != XCCDF_TYPE_NUMBER || XITEM(value)->sub.value.value == NULL)
		return NAN;	//TODO: REPLACE WITH ANSI
	return XITEM(value)->sub.value.value->limits.n.upper_bound;
}

const char *xccdf_value_get_match(const struct xccdf_value *value)
{
	if (XITEM(value)->sub.value.type != XCCDF_TYPE_STRING || XITEM(value)->sub.value.value == NULL)
		return NULL;
	return XITEM(value)->sub.value.value->limits.s.match;
}

bool xccdf_value_get_must_match(const struct xccdf_value * value)
{
	if (XITEM(value)->sub.value.value == NULL)
		return false;
	return XITEM(value)->sub.value.value->must_match;
}

static void xccdf_value_val_n_dump(struct xccdf_value_val *val, int depth)
{
	xccdf_print_depth(depth);
	printf("%f (default %f, from %f to %f)\n", val->value.n, val->defval.n, val->limits.n.lower_bound,
	       val->limits.n.upper_bound);
}

static void xccdf_value_val_s_dump(struct xccdf_value_val *val, int depth)
{
	xccdf_print_depth(depth);
	printf("'%s' (default '%s', match '%s')\n", val->value.s, val->defval.s, val->limits.s.match);
}

static void xccdf_value_val_b_dump(struct xccdf_value_val *val, int depth)
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
	void (*valdump) (struct xccdf_value_val * val, int depth) = NULL;
	xccdf_print_depth(depth);
	printf("type: ");
	switch (value->sub.value.type) {
	case XCCDF_TYPE_NUMBER:
		printf("number\n");
		valdump = xccdf_value_val_n_dump;
		break;
	case XCCDF_TYPE_STRING:
		printf("string\n");
		valdump = xccdf_value_val_s_dump;
		break;
	case XCCDF_TYPE_BOOLEAN:
		printf("boolean\n");
		valdump = xccdf_value_val_b_dump;
		break;
	default:
		assert(false);
	}
	xccdf_print_depth(depth);
	printf("values");
	oscap_htable_dump(value->sub.value.values, (oscap_dump_func) valdump, depth + 1);
	if (value->sub.value.sources->itemcount != 0) {
		xccdf_print_depth(depth);
		printf("sources");
		oscap_list_dump(value->sub.value.sources, (oscap_dump_func) xccdf_string_dump, depth + 1);
	}
}

XCCDF_STATUS_CURRENT(value)
XCCDF_VALUE_GETTER(xccdf_value_type_t, type)
XCCDF_VALUE_GETTER(xccdf_interface_hint_t, interface_hint)
XCCDF_VALUE_GETTER(xccdf_operator_t, oper) XCCDF_VALUE_GETTER(const char *, selector) XCCDF_SIGETTER(value, sources)
XCCDF_ITERATOR_GEN_S(value)
