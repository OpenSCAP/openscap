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
#include <math.h>
#include <string.h>

static oscap_destruct_func xccdf_value_val_get_destructor(xccdf_value_type_t type);
static struct xccdf_value_val *xccdf_value_val_new(xccdf_value_type_t type);

static struct xccdf_item *xccdf_value_new(struct xccdf_item *parent, xccdf_value_type_t type)
{
	struct xccdf_item *val = xccdf_item_new(XCCDF_VALUE, parent->item.benchmark, parent);
	val->sub.value.type = type;
	val->sub.value.values = oscap_htable_new();
	oscap_htable_add(val->sub.value.values, "", xccdf_value_val_new(type));
	val->sub.value.value = oscap_htable_get(val->sub.value.values, "");
	val->sub.value.selector = strdup("");
	val->sub.value.sources = oscap_list_new();
	return val;
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
		val.b = oscap_string_to_enum(XCCDF_BOOL_MAP, str);
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
	struct xccdf_item *value = xccdf_value_new(parent, type);

	value->sub.value.oper = oscap_string_to_enum(XCCDF_OPERATOR_MAP, xccdf_attribute_get(reader, XCCDFA_OPERATOR));
	value->sub.value.interface_hint =
	    oscap_string_to_enum(XCCDF_IFACE_HINT_MAP, xccdf_attribute_get(reader, XCCDFA_INTERFACEHINT));
	if (!xccdf_item_process_attributes(value, reader)) {
		xccdf_value_free(value);
		return NULL;
	}

	int depth = xccdf_element_depth(reader) + 1;

	while (xccdf_to_start_element(reader, depth)) {
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
			oscap_list_add(value->sub.value.sources, xccdf_element_string_copy(reader));
			break;
		case XCCDFE_VALUE_VAL:
			val->value = xccdf_value_get(xccdf_element_string_get(reader), type);
			break;
		case XCCDFE_DEFAULT:
			val->defval = xccdf_value_get(xccdf_element_string_get(reader), type);
			break;
		case XCCDFE_MATCH:
			if (type == XCCDF_TYPE_STRING && !val->limits.s.match)
				val->limits.s.match = xccdf_element_string_copy(reader);
			break;
		case XCCDFE_LOWER_BOUND:
			if (type == XCCDF_TYPE_NUMBER)
				val->limits.n.lower_bound = xccdf_value_get(xccdf_element_string_get(reader), type).n;
			break;
		case XCCDFE_UPPER_BOUND:
			if (type == XCCDF_TYPE_NUMBER)
				val->limits.n.upper_bound = xccdf_value_get(xccdf_element_string_get(reader), type).n;
			break;
		case XCCDFE_CHOICES:
			val->must_match = xccdf_attribute_get_bool(reader, XCCDFA_MUSTMATCH);
			while (xccdf_to_start_element(reader, depth + 1)) {
				if (xccdf_element_get(reader) == XCCDFE_CHOICE) {
					union xccdf_value_unit *unit = oscap_calloc(1, sizeof(union xccdf_value_unit));
					*unit = xccdf_value_get(xccdf_element_string_get(reader), type);
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
XCCDF_VALUE_GETTER_I(struct xccdf_value *, extends)
XCCDF_VALUE_GETTER(xccdf_value_type_t, type)
XCCDF_VALUE_GETTER(xccdf_interface_hint_t, interface_hint)
XCCDF_VALUE_GETTER(xccdf_operator_t, oper) XCCDF_VALUE_GETTER(const char *, selector) XCCDF_SIGETTER(value, sources)
