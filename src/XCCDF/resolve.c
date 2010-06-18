/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
#include "helpers.h"
#include "../common/tsort.h"
#include <math.h>

typedef void (*xccdf_textresolve_func)(void *child, void *parent);

static void xccdf_resolve_item(struct xccdf_item *item);

static struct oscap_list *xccdf_benchmark_resolve_dependencies(void *itemptr, void *userdata)
{
	struct xccdf_item *item = XITEM(itemptr);
	struct oscap_list *ret = oscap_list_new();
	struct xccdf_value_iterator *val_it = NULL;

	const char *extends = xccdf_item_get_extends(item);
	if (extends) oscap_list_add(ret, xccdf_benchmark_get_item(xccdf_item_get_benchmark(item), extends));

	switch (xccdf_item_get_type(item)) {
		case XCCDF_BENCHMARK: {
			OSCAP_FOR(xccdf_profile, profile, xccdf_benchmark_get_profiles(xccdf_item_to_benchmark(item)))
				oscap_list_add(ret, profile);
			val_it = xccdf_benchmark_get_values(xccdf_item_to_benchmark(item));
			break;
		}
		case XCCDF_GROUP:
			val_it = xccdf_group_get_values(xccdf_item_to_group(item));
		default: break; /* no-op */
	}

	OSCAP_FOR(xccdf_item, child, xccdf_item_get_content(item))
		oscap_list_add(ret, child);

	OSCAP_FOR(xccdf_value, val, val_it)
		oscap_list_add(ret, val);

	return ret;
}

bool xccdf_benchmark_resolve(struct xccdf_benchmark *benchmark)
{
	struct oscap_list *resolve_order = NULL, *root_nodes = oscap_list_new();
	oscap_list_add(root_nodes, benchmark);
	bool ret = false;
	
	if (oscap_tsort(root_nodes, &resolve_order, xccdf_benchmark_resolve_dependencies, NULL, NULL)) {
		OSCAP_FOR(xccdf_item, item, oscap_iterator_new(resolve_order))
			xccdf_resolve_item(item);
		ret = true;
	}

	oscap_list_free(root_nodes, NULL);
	oscap_list_free(resolve_order, NULL);
	return ret;
}

// prototypes
static void xccdf_resolve_textlist(struct oscap_list *child_list, struct oscap_list *parent_list, xccdf_textresolve_func more);
static void xccdf_resolve_appendlist(struct oscap_list **child_list, struct oscap_list *parent_list, oscap_cmp_func item_compare, oscap_clone_func cloner, bool prepend);
static void xccdf_resolve_value_instance(struct xccdf_value_instance *child, struct xccdf_value_instance *parent);
static void xccdf_resolve_profile(struct xccdf_item *child, struct xccdf_item *parent);
static void xccdf_resolve_group(struct xccdf_item *child, struct xccdf_item *parent);
static void xccdf_resolve_rule(struct xccdf_item *child, struct xccdf_item *parent);
static void xccdf_resolve_value(struct xccdf_item *child, struct xccdf_item *parent);

static void xccdf_resolve_warning(void *w1, void *w2) {
	if (xccdf_warning_get_category(w1) == 0)
		xccdf_warning_set_category(w1, xccdf_warning_get_category(w2));
}
static void xccdf_resolve_reference(void *r1, void *r2) {
	if (xccdf_reference_get_href(r1) == NULL)
		xccdf_reference_set_href(r1, xccdf_reference_get_href(r2));
}

#define XCCDF_RESOLVE_FLAG(ITEM,PARENT,FLAGNAME) do { \
	if (!ITEM->item.defined_flags.FLAGNAME) ITEM->item.flags.FLAGNAME = PARENT->item.flags.FLAGNAME; \
	} while (false) 

static void xccdf_resolve_item(struct xccdf_item *item)
{
	assert(item != NULL);

	if (xccdf_item_get_type(item) == XCCDF_BENCHMARK)
		xccdf_benchmark_set_resolved(xccdf_item_to_benchmark(item), true);

	struct xccdf_item *parent = xccdf_benchmark_get_item(xccdf_item_get_benchmark(item), xccdf_item_get_extends(item));
	if (parent == NULL) return;
	if (xccdf_item_get_type(item) != xccdf_item_get_type(parent)) return;

	// resolve flags
	XCCDF_RESOLVE_FLAG(item, parent, selected);
	XCCDF_RESOLVE_FLAG(item, parent, hidden);
	XCCDF_RESOLVE_FLAG(item, parent, prohibit_changes);
	XCCDF_RESOLVE_FLAG(item, parent, interactive);
	XCCDF_RESOLVE_FLAG(item, parent, multiple);

	// resolve weight & version
	if (!item->item.defined_flags.weight)
		xccdf_item_set_weight(item, xccdf_item_get_weight(parent));

	if (xccdf_item_get_version(item) == NULL) {
		xccdf_item_set_version(item, xccdf_item_get_version(parent));
		xccdf_item_set_version_update(item, xccdf_item_get_version_update(parent));
		xccdf_item_set_version_time(item, xccdf_item_get_version_time(parent));
	}

	// resolve textual elements
	xccdf_resolve_textlist(item->item.title,       parent->item.title,       NULL);
	xccdf_resolve_textlist(item->item.description, parent->item.description, NULL);
	xccdf_resolve_textlist(item->item.question,    parent->item.question,    NULL);
	xccdf_resolve_textlist(item->item.rationale,   parent->item.rationale,   NULL);
	xccdf_resolve_textlist(item->item.warnings,    parent->item.warnings,    xccdf_resolve_warning);
	xccdf_resolve_textlist(item->item.references,  parent->item.references,  xccdf_resolve_reference);

	// resolve platforms
	OSCAP_FOR_STR(platform, xccdf_item_get_platforms(parent))
		xccdf_item_add_platform(item, platform);

	// resolve properties specific to particular item type
	switch (xccdf_item_get_type(item)) {
		case XCCDF_PROFILE:   xccdf_resolve_profile(item, parent); break;
		case XCCDF_GROUP:     xccdf_resolve_group(item, parent);   break;
		case XCCDF_RULE:      xccdf_resolve_rule(item, parent);    break;
		case XCCDF_VALUE:     xccdf_resolve_value(item, parent);   break;
		default: assert(false);
	}

	// item resolved -> it no longer has a parent
	xccdf_item_set_extends(item, NULL);
}

// resolve textlists
static void xccdf_resolve_textlist(struct oscap_list *child_list, struct oscap_list *parent_list, xccdf_textresolve_func more)
{
	OSCAP_FOR(oscap_text, child, oscap_iterator_new(child_list)) {
		if (oscap_text_get_overrides(child)) continue;

		OSCAP_FOR(oscap_text, parent, oscap_iterator_new(parent_list)) {
			if (oscap_streq(oscap_text_get_lang(child), oscap_text_get_lang(parent))) {
				char *text = oscap_sprintf("%s%s", oscap_text_get_text(parent), oscap_text_get_text(child));
				oscap_text_set_text(child, text);
				oscap_free(text);
				if (more) more(child, parent);
				break;
			}
		}
		oscap_text_iterator_free(parent_iter);
	}
}

static void xccdf_resolve_appendlist(struct oscap_list **child_list, struct oscap_list *parent_list,
                                                   oscap_cmp_func item_compare, oscap_clone_func cloner, bool prepend)
{
	struct oscap_iterator *parent_iter = oscap_iterator_new(parent_list);
	struct oscap_list *to_add = oscap_list_new();
	while (oscap_iterator_has_more(parent_iter)) {
		void *parent = oscap_iterator_next(parent_iter);
		struct oscap_iterator *child_iter = oscap_iterator_new(*child_list);
		bool found = false;
		while (oscap_iterator_has_more(child_iter))
			if (item_compare(parent, oscap_iterator_next(child_iter)))
				found = true;
		if (!found) oscap_list_add(to_add, cloner(parent));
		oscap_iterator_free(child_iter);
	}
	oscap_iterator_free(parent_iter);
	*child_list = (prepend ? oscap_list_destructive_join(*child_list, to_add) : oscap_list_destructive_join(to_add, *child_list));
}

static bool xccdf_select_idcmp(void *s1, void *s2) {
	return oscap_streq(((struct xccdf_select*)s1)->item, ((struct xccdf_select*)s2)->item);
}
static bool xccdf_setvalue_idcmp(void *s1, void *s2) {
	return oscap_streq(((struct xccdf_setvalue*)s1)->item, ((struct xccdf_setvalue*)s2)->item);
}
static bool xccdf_refine_rule_idcmp(void *s1, void *s2) {
	return oscap_streq(((struct xccdf_refine_rule*)s1)->item, ((struct xccdf_refine_rule*)s2)->item);
}
static bool xccdf_refine_value_idcmp(void *s1, void *s2) {
	return oscap_streq(((struct xccdf_refine_value*)s1)->item, ((struct xccdf_refine_value*)s2)->item);
}

static void xccdf_resolve_profile(struct xccdf_item *child, struct xccdf_item *parent)
{
	if (child->sub.profile.note_tag != NULL && child->sub.profile.note_tag != NULL) {
		char *note_tag = oscap_sprintf("%s %s", xccdf_profile_get_note_tag(XPROFILE(child)), xccdf_profile_get_note_tag(XPROFILE(parent)));
		xccdf_profile_set_note_tag(XPROFILE(child), note_tag);
		oscap_free(note_tag);
	}

	xccdf_resolve_appendlist(&child->sub.profile.selects,       parent->sub.profile.selects,       xccdf_select_idcmp,       (oscap_clone_func)xccdf_select_clone, false);
	xccdf_resolve_appendlist(&child->sub.profile.setvalues,     parent->sub.profile.setvalues,     xccdf_setvalue_idcmp,     (oscap_clone_func)xccdf_setvalue_clone, false);
	xccdf_resolve_appendlist(&child->sub.profile.refine_rules,  parent->sub.profile.refine_rules,  xccdf_refine_rule_idcmp,  (oscap_clone_func)xccdf_refine_rule_clone, false);
	xccdf_resolve_appendlist(&child->sub.profile.refine_values, parent->sub.profile.refine_values, xccdf_refine_value_idcmp, (oscap_clone_func)xccdf_refine_value_clone, false);
}

static struct xccdf_item *xccdf_resolve_copy_item(struct xccdf_item *src)
{
	struct xccdf_benchmark *bench = xccdf_item_get_benchmark(src);
	const char *prefix = NULL;
	switch (xccdf_item_get_type(src)) {
		case XCCDF_RULE:  prefix = "inherited-rule-";  break;
		case XCCDF_GROUP: prefix = "inherited-group-"; break;
		case XCCDF_VALUE: prefix = "inherited-value-"; break;
		default: assert(false);
	}
	char *newid = xccdf_benchmark_gen_id(bench, prefix);
	struct xccdf_item *clone = xccdf_item_clone(src);
	xccdf_item_set_id(clone, newid);
	oscap_free(newid);
	return clone;
}

static bool xccdf_incomparable(void *i1, void *i2) { return false; }
//static void *xccdf_strlist_clone(void *l) { return oscap_list_clone(l, (oscap_clone_func)oscap_strdup); }

static void xccdf_resolve_group(struct xccdf_item *child, struct xccdf_item *parent)
{
	// TODO: resolve requires properly (how?)
	//xccdf_resolve_appendlist(&child->sub.group.requires, parent->sub.group.requires, xccdf_incomparable, xccdf_strlist_clone, false);
	xccdf_resolve_appendlist(&child->sub.group.conflicts, parent->sub.group.conflicts, (oscap_cmp_func)oscap_strcmp, (oscap_clone_func)oscap_strdup, false);
	
	OSCAP_FOR(xccdf_item, item, xccdf_group_get_content(XGROUP(parent)))
		xccdf_group_add_content(XGROUP(child), xccdf_resolve_copy_item(item));
	OSCAP_FOR(xccdf_value, val, xccdf_group_get_values(XGROUP(parent)))
		xccdf_group_add_value(XGROUP(child), xccdf_item_to_value(xccdf_resolve_copy_item(XITEM(val))));
}

static bool xccdf_ident_idcmp(void *s1, void *s2) {
	return oscap_streq(((struct xccdf_ident*)s1)->id, ((struct xccdf_ident*)s2)->id);
}
static void xccdf_resolve_profile_note(void *p1, void *p2) {
	if (xccdf_profile_note_get_reftag(p1) == NULL)
		xccdf_profile_note_set_reftag(p1, xccdf_profile_note_get_reftag(p2));
}
static void xccdf_resolve_fixtext(struct xccdf_fixtext *t1, struct xccdf_fixtext *t2) {
	if (xccdf_fixtext_get_fixref(t1) == NULL)
		xccdf_fixtext_set_fixref(t1, xccdf_fixtext_get_fixref(t2));
	if (xccdf_fixtext_get_disruption(t1) == 0)
		xccdf_fixtext_set_disruption(t1, xccdf_fixtext_get_disruption(t2));
	if (xccdf_fixtext_get_complexity(t1) == 0)
		xccdf_fixtext_set_complexity(t1, xccdf_fixtext_get_complexity(t2));
}

static void xccdf_resolve_rule(struct xccdf_item *child, struct xccdf_item *parent)
{
	// TODO: resolve requires properly (how?)
	//xccdf_resolve_appendlist(&child->sub.rule.requires, parent->sub.rule.requires, xccdf_incomparable, xccdf_strlist_clone);
	xccdf_resolve_appendlist(&child->sub.rule.conflicts, parent->sub.rule.conflicts, (oscap_cmp_func)oscap_strcmp, (oscap_clone_func)oscap_strdup, false);
	xccdf_resolve_appendlist(&child->sub.rule.idents, parent->sub.rule.idents, (oscap_cmp_func)xccdf_ident_idcmp, (oscap_clone_func)xccdf_ident_clone, false);
	xccdf_resolve_appendlist(&child->sub.rule.fixes, parent->sub.rule.fixes, (oscap_cmp_func)xccdf_incomparable, (oscap_clone_func)xccdf_fix_clone, false);

	if (oscap_list_get_itemcount(child->sub.rule.checks) == 0 && oscap_list_get_itemcount(parent->sub.rule.checks) > 0) {
		oscap_list_free(child->sub.rule.checks, NULL);
		child->sub.rule.checks = oscap_list_clone(parent->sub.rule.checks, (oscap_clone_func)xccdf_check_clone);
	}

	if (!child->item.defined_flags.role) child->sub.rule.role = parent->sub.rule.role;
	if (!child->item.defined_flags.severity) child->sub.rule.severity = parent->sub.rule.severity;

	xccdf_resolve_textlist(child->sub.rule.profile_notes, parent->sub.rule.profile_notes, xccdf_resolve_profile_note);
	xccdf_resolve_textlist(child->sub.rule.fixtexts, parent->sub.rule.fixtexts, (xccdf_textresolve_func)xccdf_resolve_fixtext);
}


static void xccdf_resolve_value(struct xccdf_item *child, struct xccdf_item *parent)
{
	if (xccdf_value_get_interface_hint(XVALUE(child)) == 0)
		child->sub.value.interface_hint = xccdf_value_get_interface_hint(XVALUE(parent));
	if (xccdf_value_get_oper(XVALUE(child)) == 0)
		xccdf_value_set_oper(child, xccdf_value_get_oper(XVALUE(parent)));
	
	struct xccdf_value_instance *inst_child;
	OSCAP_FOR(xccdf_value_instance, inst_parent, xccdf_value_get_instances(XVALUE(parent))) {
		inst_child = xccdf_value_get_instance_by_selector(XVALUE(child), xccdf_value_instance_get_selector(inst_parent));
		if (inst_child == NULL) xccdf_value_add_instance(XVALUE(child), xccdf_value_instance_clone(inst_parent));
		else xccdf_resolve_value_instance(inst_child, inst_parent);
	}
}

static inline void xccdf_transfer_value_unit(union xccdf_value_unit *tgt, union xccdf_value_unit src, xccdf_value_type_t type)
{
	assert(tgt != NULL);

	switch (type) {
	case XCCDF_TYPE_STRING: oscap_free(tgt->s); tgt->s = oscap_strdup(src.s); break;
	case XCCDF_TYPE_NUMBER: tgt->n = src.n; break;
	case XCCDF_TYPE_BOOLEAN: tgt->b = src.b; break;
	default: assert(false);
	}
}

static void xccdf_resolve_value_instance(struct xccdf_value_instance *child, struct xccdf_value_instance *parent)
{
	assert(child != NULL);
	assert(parent != NULL);
	assert(oscap_streq(child->selector, parent->selector));
	if (parent->type != child->type) return;
	
	if (!child->flags.value_given)
		xccdf_transfer_value_unit(&child->value, parent->value, child->type);
	if (!child->flags.defval_given)
		xccdf_transfer_value_unit(&child->defval, parent->defval, child->type);
	if (child->type == XCCDF_TYPE_STRING && child->limits.s.match == NULL)
		xccdf_value_instance_set_match(child, xccdf_value_instance_get_match(parent));
	if (child->type == XCCDF_TYPE_NUMBER) {
		if (child->limits.n.lower_bound == NAN)
			child->limits.n.lower_bound = parent->limits.n.lower_bound;
		if (child->limits.n.upper_bound == NAN)
			child->limits.n.upper_bound = parent->limits.n.upper_bound;
	}

	struct oscap_iterator *it = oscap_iterator_new(parent->choices);
	while (oscap_iterator_has_more(it)) {
		union xccdf_value_unit *unit = oscap_iterator_next(it);
		void *unitclone = oscap_alloc(sizeof(union xccdf_value_unit));
		xccdf_transfer_value_unit(unitclone, *unit, child->type);
		oscap_list_add(child->choices, unitclone);
	}
	oscap_iterator_free(it);
}
