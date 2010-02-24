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

#ifndef XCCDF_ITEM_
#define XCCDF_ITEM_

#include "public/xccdf.h"
#include <assert.h>
#include "elements.h"
#include "../common/list.h"
#include "../common/util.h"
#include "../common/text_priv.h"

OSCAP_HIDDEN_START;

struct xccdf_flags {
	unsigned selected:1;
	unsigned hidden:1;
	unsigned resolved:1;
	unsigned abstract:1;
	unsigned prohibit_changes:1;
	unsigned interactive:1;
	unsigned multiple:1;
};

struct xccdf_item;
struct xccdf_check;

struct xccdf_item_base {
	char *id;
	char *cluster_id;
	float weight;

	struct oscap_list *title;
	struct oscap_list *description;
	struct oscap_list *question;
	struct oscap_list *rationale;

	char *version;
	char *version_update;
	time_t version_time;

	struct xccdf_item *extends;
	struct xccdf_item *parent;
	struct oscap_list *statuses;
	struct oscap_list *references;
	struct oscap_list *platforms;
	struct xccdf_flags flags;
	struct xccdf_item *benchmark;
};

struct xccdf_rule_item {
	char *impact_metric;
	xccdf_role_t role;
	xccdf_level_t severity;
	struct xccdf_check *check;

	struct oscap_list *requires;
	struct oscap_list *conflicts;

	struct oscap_list *profile_notes;
	struct oscap_list *idents;
	struct oscap_list *checks;
	struct oscap_list *fixes;
	struct oscap_list *fixtexts;
};

struct xccdf_group_item {
	struct oscap_list *requires;
	struct oscap_list *conflicts;

	struct oscap_list *values;
	struct oscap_list *content;
};

union xccdf_value_unit {
	xccdf_numeric n;
	char *s;
	bool b;
};

struct xccdf_value_val {
	union xccdf_value_unit value;
	union xccdf_value_unit defval;
	struct oscap_list *choices;
	bool must_match;
	union {
		struct {
			xccdf_numeric lower_bound;
			xccdf_numeric upper_bound;
		} n;
		struct {
			char *match;
		} s;
	} limits;
};

struct xccdf_value_item {
	xccdf_value_type_t type;
	xccdf_interface_hint_t interface_hint;
	xccdf_operator_t oper;
	char *selector;

	struct xccdf_value_val *value;
	struct oscap_htable *values;

	struct oscap_list *sources;
};

// @todo complete
struct xccdf_result_item {
	struct oscap_list *status;
	time_t start_time;
	time_t end_time;
	char *test_system;
	char *remark;
	char *organization;
	char *benchmark_uri;

	struct xccdf_item *profile;
	struct oscap_list *identities;
	struct oscap_list *targets;
	struct oscap_list *target_addresses;
	struct oscap_list *target_facts;
	struct oscap_list *set_values;
	struct oscap_list *rule_results;
	struct oscap_list *scores;
};

struct xccdf_profile_item {
	char *note_tag;
	struct oscap_list *selects;
	struct oscap_list *set_values;
	struct oscap_list *refine_values;
	struct oscap_list *refine_rules;
};

struct xccdf_benchmark_item {

	struct oscap_htable *dict;
	struct oscap_htable *auxdict;
	struct oscap_list *idrefs;
	struct oscap_list *notices;
	struct oscap_htable *plain_texts;

	char *style;
	char *style_href;
	char *metadata;

	struct oscap_list *front_matter;
	struct oscap_list *rear_matter;

	struct oscap_list *models;
	struct oscap_list *profiles;
	struct oscap_list *values;
	struct oscap_list *content;
	struct oscap_list *results;
};

struct xccdf_item {
	xccdf_type_t type;
	struct xccdf_item_base item;
	union {
		struct xccdf_profile_item profile;
		struct xccdf_benchmark_item bench;
		struct xccdf_rule_item rule;
		struct xccdf_group_item group;
		struct xccdf_value_item value;
		struct xccdf_result_item result;
	} sub;
};

struct xccdf_notice {
	char *id;
	struct oscap_text *text;
};

struct xccdf_status {
	xccdf_status_type_t status;
	time_t date;
};

struct xccdf_model {
	char *system;
	struct oscap_htable *params;
};

struct xccdf_selected {
	struct xccdf_item *item;
	bool selected;
};

struct xccdf_refine_rule {
	struct xccdf_item *item;
	char *remark;
	char *selector;
	xccdf_role_t role;
	xccdf_level_t severity;
	float weight;
};

struct xccdf_refine_value {
	struct xccdf_item *item;
	char *remark;
	char *selector;
	xccdf_operator_t oper;
};

struct xccdf_set_value {
	struct xccdf_item *item;
	char *value;
};

struct xccdf_ident {
	char *id;
	char *system;
};

struct xccdf_check {
	xccdf_bool_operator_t oper;
	struct oscap_list *children;
	struct xccdf_item *parent;
	char *id;
	char *system;
	char *selector;
	char *content;
	struct oscap_list *imports;
	struct oscap_list *exports;
	struct oscap_list *content_refs;
};

struct xccdf_check_content_ref {
	char *href;
	char *name;
};

struct xccdf_check_import {
	char *name;
	char *content;
};

struct xccdf_check_export {
	char *name;
	struct xccdf_item *value;
};

struct xccdf_profile_note {
	char *reftag;
	struct oscap_text *text;
};

struct xccdf_fix {
	bool reboot;
	xccdf_strategy_t strategy;
	xccdf_level_t disruption;
	xccdf_level_t complexity;
	char *id;
	char *content;
	char *system;
	char *platform;
};

struct xccdf_fixtext {
	bool reboot;
	xccdf_strategy_t strategy;
	xccdf_level_t disruption;
	xccdf_level_t complexity;
	struct xccdf_fix *fixref;
	char *content;
};

extern const struct oscap_string_map XCCDF_LEVEL_MAP[];
extern const struct oscap_string_map XCCDF_ROLE_MAP[];
extern const struct oscap_string_map XCCDF_OPERATOR_MAP[];
extern const struct oscap_string_map XCCDF_STRATEGY_MAP[];

struct xccdf_item *xccdf_item_new(xccdf_type_t type, struct xccdf_item *bench, struct xccdf_item *parent);
void xccdf_item_release(struct xccdf_item *item);
void xccdf_item_print(struct xccdf_item *item, int depth);
void xccdf_item_dump(struct xccdf_item *item, int depth);
void xccdf_item_free(struct xccdf_item *item);

struct xccdf_item *xccdf_benchmark_new(void);
bool xccdf_benchmark_parse(struct xccdf_item *benchmark, xmlTextReaderPtr reader);
bool xccdf_benchmark_add_ref(struct xccdf_item *benchmark, struct xccdf_item **ptr, const char *id, xccdf_type_t type);
void xccdf_benchmark_dump(struct xccdf_benchmark *benchmark);

struct xccdf_item *xccdf_profile_new(struct xccdf_item *bench);
struct xccdf_item *xccdf_profile_parse(xmlTextReaderPtr reader, struct xccdf_item *bench);
void xccdf_profile_dump(struct xccdf_item *prof, int depth);
void xccdf_profile_free(struct xccdf_item *prof);

bool xccdf_item_process_attributes(struct xccdf_item *item, xmlTextReaderPtr reader);
bool xccdf_item_process_element(struct xccdf_item *item, xmlTextReaderPtr reader);

bool xccdf_content_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
struct xccdf_item *xccdf_group_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
struct xccdf_item *xccdf_group_new(struct xccdf_item *parent);
void xccdf_group_dump(struct xccdf_item *group, int depth);
void xccdf_group_free(struct xccdf_item *group);

struct xccdf_item *xccdf_rule_new(struct xccdf_item *parent);
struct xccdf_item *xccdf_rule_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
void xccdf_rule_dump(struct xccdf_item *rule, int depth);
void xccdf_rule_free(struct xccdf_item *rule);

struct xccdf_item *xccdf_value_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
struct xccdf_item *xccdf_value_new(struct xccdf_item *parent, xccdf_value_type_t type);
void xccdf_value_dump(struct xccdf_item *value, int depth);
void xccdf_value_free(struct xccdf_item *val);

//struct xccdf_notice *xccdf_notice_from_string(const char *id, char *string);
//struct xccdf_notice *xccdf_notice_from_text(const char *id, struct oscap_text *string);
struct xccdf_notice *xccdf_notice_new(void);
struct xccdf_notice *xccdf_notice_new_parse(xmlTextReaderPtr reader);
void xccdf_notice_dump(struct xccdf_notice *notice, int depth);
void xccdf_notice_free(struct xccdf_notice *notice);

struct xccdf_status *xccdf_status_new(const char *status, const char *date);
void xccdf_status_dump(struct xccdf_status *status, int depth);
void xccdf_status_free(struct xccdf_status *status);

struct xccdf_model *xccdf_model_new_xml(xmlTextReaderPtr reader);
void xccdf_model_free(struct xccdf_model *model);

void xccdf_cstring_dump(const char *data, int depth);

struct xccdf_ident *xccdf_ident_new(const char *id, const char *system);
void xccdf_ident_free(struct xccdf_ident *ident);

struct xccdf_check *xccdf_check_new(struct xccdf_item *parent);
struct xccdf_check *xccdf_check_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
void xccdf_check_dump(struct xccdf_check *check, int depth);
void xccdf_check_free(struct xccdf_check *check);
void xccdf_check_content_ref_free(struct xccdf_check_content_ref *ref);
void xccdf_check_content_ref_dump(struct xccdf_check_content_ref *ref, int depth);
struct xccdf_ident *xccdf_ident_new(const char *id, const char *system);
struct xccdf_ident *xccdf_ident_parse(xmlTextReaderPtr reader);
void xccdf_ident_dump(struct xccdf_ident *ident, int depth);
void xccdf_ident_free(struct xccdf_ident *ident);
void xccdf_profile_note_free(struct xccdf_profile_note *note);
void xccdf_check_import_free(struct xccdf_check_import *item);
void xccdf_check_export_free(struct xccdf_check_export *item);
struct xccdf_fix *xccdf_fix_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
struct xccdf_fixtext *xccdf_fixtext_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
void xccdf_fixtext_free(struct xccdf_fixtext *item);
void xccdf_fix_free(struct xccdf_fix *item);
void xccdf_set_value_free(struct xccdf_set_value *sv);

#define MACRO_BLOCK(code) do { code } while(false)
#define ASSERT_TYPE(item,t) assert((item)->type & t)
#define ASSERT_BENCHMARK(item) ASSERT_TYPE(item, XCCDF_BENCHMARK)
#define XBENCHMARK(item) ((struct xccdf_benchmark*)item)
#define XPROFILE(item) ((struct xccdf_profile*)item)
#define XGROUP(item) ((struct xccdf_group*)item)
#define XRULE(item) ((struct xccdf_rule*)item)
#define XITEM(item) ((struct xccdf_item*)item)
#define XVALUE(item) ((struct xccdf_value*)item)

#define XCCDF_STATUS_CURRENT(TYPE) \
		xccdf_status_type_t xccdf_##TYPE##_get_status_current(const struct xccdf_##TYPE* item) {\
			return xccdf_item_get_current_status(XITEM(item)); }

#define XCCDF_TEXT_IGETTER(SNAME,MNAME,MEXP) \
        struct oscap_text_iterator* xccdf_##SNAME##_get_##MNAME(const struct xccdf_##SNAME* item) \
        { return oscap_iterator_new(XITEM(item)->MEXP); }
#define XCCDF_GENERIC_GETTER(RTYPE,TNAME,MEMBER) \
        RTYPE xccdf_##TNAME##_get_##MEMBER(const struct xccdf_##TNAME* item) { return (RTYPE)((item)->MEMBER); }
#define XCCDF_GENERIC_IGETTER(ITYPE,TNAME,MNAME) \
        struct xccdf_##ITYPE##_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(item->MNAME); }
#define XCCDF_ABSTRACT_GETTER(RTYPE,TNAME,MNAME,MEMBER) \
        RTYPE xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) { return (RTYPE)(XITEM(item)->MEMBER); }
#define XCCDF_ITERATOR_GETTER(ITYPE,TNAME,MNAME,MEMBER) \
        struct xccdf_##ITYPE##_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(XITEM(item)->MEMBER); }
#define XCCDF_SITERATOR_GETTER(TNAME,MNAME,MEMBER) \
        struct oscap_string_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(XITEM(item)->MEMBER); }
#define XCCDF_TITERATOR_GETTER(TNAME,MNAME,MEMBER) \
        struct oscap_text_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(XITEM(item)->MEMBER); }
#define XCCDF_HTABLE_GETTER(RTYPE,TNAME,MNAME,MEMBER) \
		RTYPE xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item, const char* key) \
		{ return (RTYPE)oscap_htable_get(XITEM(item)->MEMBER, key); }
#define XCCDF_SIGETTER(TNAME,MNAME) \
        struct oscap_string_iterator* xccdf_##TNAME##_get_##MNAME(const struct xccdf_##TNAME* item) \
        { return oscap_iterator_new(XITEM(item)->sub.TNAME.MNAME); }

#define XCCDF_BENCHMARK_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,benchmark,MNAME,MEMBER)
#define XCCDF_BENCHMARK_GETTER_I(RTYPE,MNAME) XCCDF_BENCHMARK_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_BENCHMARK_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,benchmark,MNAME,item.MNAME)
#define XCCDF_BENCHMARK_GETTER(RTYPE,MNAME) XCCDF_BENCHMARK_GETTER_A(RTYPE,MNAME,sub.bench.MNAME)
#define XCCDF_BENCHMARK_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,benchmark,MNAME,sub.bench.MNAME)

#define XCCDF_PROFILE_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,profile,MNAME,MEMBER)
#define XCCDF_PROFILE_GETTER_I(RTYPE,MNAME) XCCDF_PROFILE_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_PROFILE_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,profile,MNAME,item.MNAME)
#define XCCDF_PROFILE_GETTER(RTYPE,MNAME) XCCDF_PROFILE_GETTER_A(RTYPE,MNAME,sub.profile.MNAME)
#define XCCDF_PROFILE_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,profile,MNAME,sub.profile.MNAME)

#define XCCDF_RULE_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,rule,MNAME,MEMBER)
#define XCCDF_RULE_GETTER_I(RTYPE,MNAME) XCCDF_RULE_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_RULE_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,rule,MNAME,item.MNAME)
#define XCCDF_RULE_GETTER(RTYPE,MNAME) XCCDF_RULE_GETTER_A(RTYPE,MNAME,sub.rule.MNAME)
#define XCCDF_RULE_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,rule,MNAME,sub.rule.MNAME)

#define XCCDF_GROUP_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,group,MNAME,MEMBER)
#define XCCDF_GROUP_GETTER_I(RTYPE,MNAME) XCCDF_GROUP_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_GROUP_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,group,MNAME,item.MNAME)
#define XCCDF_GROUP_GETTER(RTYPE,MNAME) XCCDF_GROUP_GETTER_A(RTYPE,MNAME,sub.group.MNAME)
#define XCCDF_GROUP_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,group,MNAME,sub.group.MNAME)

#define XCCDF_VALUE_GETTER_A(RTYPE,MNAME,MEMBER) XCCDF_ABSTRACT_GETTER(RTYPE,value,MNAME,MEMBER)
#define XCCDF_VALUE_GETTER_I(RTYPE,MNAME) XCCDF_VALUE_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_VALUE_IGETTER_I(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,value,MNAME,item.MNAME)
#define XCCDF_VALUE_GETTER(RTYPE,MNAME) XCCDF_VALUE_GETTER_A(RTYPE,MNAME,sub.value.MNAME)
#define XCCDF_VALUE_IGETTER(ITYPE,MNAME) XCCDF_ITERATOR_GETTER(ITYPE,value,MNAME,sub.value.MNAME)

#define XCCDF_ITEM_GETTER(RTYPE,MNAME) \
		XCCDF_ABSTRACT_GETTER(RTYPE,item,MNAME,item.MNAME) \
        XCCDF_BENCHMARK_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_PROFILE_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_RULE_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_VALUE_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_GROUP_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_ITEM_IGETTER(RTYPE,MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,item,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,benchmark,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,profile,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,rule,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,value,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,group,MNAME,item.MNAME)
#define XCCDF_ITEM_SIGETTER(MNAME) \
        XCCDF_SITERATOR_GETTER(item,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(benchmark,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(profile,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(rule,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(value,MNAME,item.MNAME) \
        XCCDF_SITERATOR_GETTER(group,MNAME,item.MNAME)
#define XCCDF_ITEM_TIGETTER(MNAME) \
        XCCDF_TITERATOR_GETTER(item,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(benchmark,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(profile,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(rule,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(value,MNAME,item.MNAME) \
        XCCDF_TITERATOR_GETTER(group,MNAME,item.MNAME)
#define XCCDF_FLAG_GETTER(MNAME) \
        XCCDF_BENCHMARK_GETTER_A(bool,MNAME,item.flags.MNAME) \
        XCCDF_PROFILE_GETTER_A(bool,MNAME,item.flags.MNAME) \
        XCCDF_RULE_GETTER_A(bool,MNAME,item.flags.MNAME) \
        XCCDF_VALUE_GETTER_A(bool,MNAME,item.flags.MNAME) \
        XCCDF_GROUP_GETTER_A(bool,MNAME,item.flags.MNAME)

#define XITERATOR(x) ((struct oscap_iterator*)(x))
#define XCCDF_ITERATOR(n) struct xccdf_##n##_iterator*
#define XCCDF_ITERATOR_FWD(n) struct xccdf_##n##_iterator;
#define XCCDF_ITERATOR_HAS_MORE(n) bool xccdf_##n##_iterator_has_more(XCCDF_ITERATOR(n) it) { return oscap_iterator_has_more(XITERATOR(it)); }
#define XCCDF_ITERATOR_NEXT(t,n) t xccdf_##n##_iterator_next(XCCDF_ITERATOR(n) it) { return oscap_iterator_next(XITERATOR(it)); }
#define XCCDF_ITERATOR_FREE(n) void xccdf_##n##_iterator_free(XCCDF_ITERATOR(n) it) { oscap_iterator_free(XITERATOR(it)); }
#define XCCDF_ITERATOR_GEN_T(t,n) XCCDF_ITERATOR_FWD(n) XCCDF_ITERATOR_HAS_MORE(n) XCCDF_ITERATOR_NEXT(t,n) XCCDF_ITERATOR_FREE(n)
#define XCCDF_ITERATOR_GEN_S(n) XCCDF_ITERATOR_GEN_T(struct xccdf_##n*,n)
#define XCCDF_ITERATOR_GEN_TEXT(n) XCCDF_ITERATOR_GEN_T(struct oscap_text *,n)

OSCAP_HIDDEN_END;

#endif
