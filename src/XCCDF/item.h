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

#include <assert.h>
#include "xccdf.h"
#include "elements.h"
#include "list.h"

struct xccdf_flags {
	unsigned selected         : 1;
	unsigned hidden           : 1;
	unsigned resolved         : 1;
	unsigned abstract         : 1;
	unsigned prohibit_changes : 1;
	unsigned interactive      : 1;
	unsigned multiple         : 1;
};

struct xccdf_item;
struct xccdf_check;

struct xccdf_item_base {
	char* id;
	char* title;
	char* description;
	char* question;
	char* rationale;
	char* cluster_id;
	float weight;

    char* version;
    char* version_update;
    time_t version_time;

	struct xccdf_item* extends;
	struct xccdf_item* parent;
	struct xccdf_list* statuses;
	struct xccdf_list* references;
	struct xccdf_list* platforms;
	struct xccdf_flags flags;
	struct xccdf_item* benchmark;
};

struct xccdf_rule_item {
	char* impact_metric;
	enum xccdf_role role;
	enum xccdf_level severity;
	struct xccdf_check* check;

	struct xccdf_list* requires;
	struct xccdf_list* conflicts;

	struct xccdf_list* profile_notes;
	struct xccdf_list* idents;
	struct xccdf_list* checks;
	struct xccdf_list* fixes;
	struct xccdf_list* fixtexts;
};

struct xccdf_group_item {
	struct xccdf_list* requires;
	struct xccdf_list* conflicts;

	struct xccdf_list* values;
	struct xccdf_list* content;
};

// @todo review
struct xccdf_value_item {
	enum xccdf_value_type type;
	enum xccdf_interface_hint interface_hint;
	enum xccdf_operator oper;
	char* selector;

	struct xccdf_list* values;
	struct xccdf_list* default_vals;
	struct xccdf_list* matches;
	struct xccdf_list* lower_bounds;
	struct xccdf_list* upper_bounds;
	struct xccdf_list* choices;
	struct xccdf_list* sources;
};

// @todo complete
struct xccdf_result_item {
	char* note_tag;
	struct xccdf_list* status;
};

struct xccdf_profile_item {
    char* note_tag;
    struct xccdf_list* selects;
    struct xccdf_list* set_values;
    struct xccdf_list* refine_values;
    struct xccdf_list* refine_rules;
};

struct xccdf_benchmark_item {
	
	struct xccdf_htable* dict;
	struct xccdf_htable* auxdict;
	struct xccdf_list* idrefs;
	struct xccdf_list* notices;
    struct xccdf_htable* plain_texts;

	char* style;
	char* style_href;
	char* front_matter;
	char* rear_matter;
	char* metadata;

    struct xccdf_list* models;
	struct xccdf_list* profiles;
	struct xccdf_list* values;
	struct xccdf_list* content;
	struct xccdf_list* results;
};


struct xccdf_item {
	enum xccdf_type type;
	struct xccdf_item_base item;
	union {
        struct xccdf_profile_item   profile;
		struct xccdf_benchmark_item bench;
		struct xccdf_rule_item      rule;
		struct xccdf_group_item     group;
		struct xccdf_value_item     value;
		struct xccdf_result_item    result;
	} sub;
};

struct xccdf_notice {
	char* id;
	char* text;
};

struct xccdf_status {
    enum xccdf_status_type status;
    time_t date;
};

struct xccdf_model {
    char* system;
    struct xccdf_htable* params;
};

struct xccdf_selected {
    struct xccdf_item* item;
    bool selected;
};

struct xccdf_refine_rule {
	struct xccdf_item* item;
	char* remark;
	char* selector;
	enum xccdf_role role;
	enum xccdf_level severity;
	float weight;
};

struct xccdf_refine_value {
	struct xccdf_item* item;
	char* remark;
	char* selector;
	enum xccdf_operator oper;
};

struct xccdf_set_value {
	struct xccdf_item* item;
	char* value;
};

struct xccdf_ident {
	char* id;
	char* system;
};

struct xccdf_check {
	enum xccdf_bool_operator oper;
	struct xccdf_list* children;
	struct xccdf_item* parent;
	char* id;
	char* system;
	char* selector;
	char* content;
	struct xccdf_list* imports;
	struct xccdf_list* exports;
	struct xccdf_list* content_refs;
};

struct xccdf_check_content_ref {
	char* href;
	char* name;
};

struct xccdf_check_import {
	char* name;
	char* content;
};

struct xccdf_check_export {
	char* name;
	struct xccdf_item* value;
};

struct xccdf_profile_note {
	char* reftag;
	char* text;
};

struct xccdf_fix {
	bool reboot;
	enum xccdf_strategy strategy;
	enum xccdf_level disruption;
	enum xccdf_level complexity;
	char* id;
	char* content;
	char* system;
	char* platform;
};

struct xccdf_fixtext {
	bool reboot;
	enum xccdf_strategy strategy;
	enum xccdf_level disruption;
	enum xccdf_level complexity;
	struct xccdf_fix* fixref;
	char* content;
};

extern const struct xccdf_string_map XCCDF_LEVEL_MAP[];
extern const struct xccdf_string_map XCCDF_ROLE_MAP[];
extern const struct xccdf_string_map XCCDF_OPERATOR_MAP[];
extern const struct xccdf_string_map XCCDF_STRATEGY_MAP[];

struct xccdf_item* xccdf_item_new(enum xccdf_type type, struct xccdf_item* bench, struct xccdf_item* parent);
void xccdf_item_release(struct xccdf_item* item);
enum xccdf_status_type xccdf_item_status_current(const struct xccdf_item* item);
void xccdf_item_print(struct xccdf_item* item, int depth);
void xccdf_item_dump(struct xccdf_item* item, int depth);
void xccdf_item_delete(struct xccdf_item* item);

struct xccdf_item* xccdf_benchmark_new_empty(void);
bool xccdf_benchmark_parse(struct xccdf_item* benchmark, xmlTextReaderPtr reader);
bool xccdf_benchmark_add_ref(struct xccdf_item* benchmark, struct xccdf_item** ptr, const char* id, enum xccdf_type type);
void xccdf_benchmark_dump(struct xccdf_benchmark* benchmark);

struct xccdf_item* xccdf_profile_new_empty(struct xccdf_item* bench);
struct xccdf_item* xccdf_profile_new_parse(xmlTextReaderPtr reader, struct xccdf_item* bench);
void xccdf_profile_dump(struct xccdf_item* prof, int depth);
void xccdf_profile_delete(struct xccdf_item* prof);

bool xccdf_item_process_attributes(struct xccdf_item* item, xmlTextReaderPtr reader);
bool xccdf_item_process_element(struct xccdf_item* item, xmlTextReaderPtr reader);

bool xccdf_content_parse(xmlTextReaderPtr reader, struct xccdf_item* parent);
struct xccdf_item* xccdf_group_new_parse(xmlTextReaderPtr reader, struct xccdf_item* parent);
void xccdf_group_dump(struct xccdf_item* group, int depth);
void xccdf_group_delete(struct xccdf_item* group);

struct xccdf_item* xccdf_rule_new_parse(xmlTextReaderPtr reader, struct xccdf_item* parent);
void xccdf_rule_dump(struct xccdf_item* rule, int depth);
void xccdf_rule_delete(struct xccdf_item* rule);

struct xccdf_notice* xccdf_notice_new(const char* id, char* text);
void xccdf_notice_dump(struct xccdf_notice* notice, int depth);
void xccdf_notice_delete(struct xccdf_notice* notice);

struct xccdf_status* xccdf_status_new(const char* status, const char* date);
void xccdf_status_dump(struct xccdf_status* status, int depth);
void xccdf_status_delete(struct xccdf_status* status);

struct xccdf_model* xccdf_model_new_xml(xmlTextReaderPtr reader);
void xccdf_model_delete(struct xccdf_model* model);

void xccdf_cstring_dump(const char* data, int depth);

struct xccdf_ident* xccdf_ident_new(const char* id, const char* system);
void xccdf_ident_delete(struct xccdf_ident* ident);

struct xccdf_check* xccdf_check_new_empty(struct xccdf_item* parent);
struct xccdf_check* xccdf_check_new_parse(xmlTextReaderPtr reader, struct xccdf_item* parent);
void xccdf_check_dump(struct xccdf_check* check, int depth);
void xccdf_check_delete(struct xccdf_check* check);
void xccdf_check_content_ref_delete(struct xccdf_check_content_ref* ref);
void xccdf_check_content_ref_dump(struct xccdf_check_content_ref* ref, int depth);
struct xccdf_ident* xccdf_ident_new(const char* id, const char* system);
struct xccdf_ident* xccdf_ident_new_parse(xmlTextReaderPtr reader);
void xccdf_ident_dump(struct xccdf_ident* ident, int depth);
void xccdf_ident_delete(struct xccdf_ident* ident);
void xccdf_profile_note_delete(struct xccdf_profile_note* note);
void xccdf_check_import_delete(struct xccdf_check_import* item);
void xccdf_check_export_delete(struct xccdf_check_export* item);
void xccdf_fixtext_delete(struct xccdf_fixtext* item);
void xccdf_fix_delete(struct xccdf_fix* item);



#define MACRO_BLOCK(code) do { code } while(false)
#define ASSERT_TYPE(item,t) assert((item)->type & t)
#define ASSERT_BENCHMARK(item) ASSERT_TYPE(item, XCCDF_BENCHMARK)
#define XBENCHMARK(item) ((struct xccdf_benchmark*)item)
#define XPROFILE(item) ((struct xccdf_profile*)item)
#define XGROUP(item) ((struct xccdf_group*)item)
#define XRULE(item) ((struct xccdf_rule*)item)
#define XITEM(item) ((struct xccdf_item*)item)

#define XCCDF_STATUS_CURRENT(TYPE) \
		enum xccdf_status_type xccdf_##TYPE##_status_current(const struct xccdf_##TYPE* item) {\
			return xccdf_item_status_current(XITEM(item)); }

#define XCCDF_GENERIC_GETTER(RTYPE,TNAME,MEMBER) \
        RTYPE xccdf_##TNAME##_##MEMBER(const struct xccdf_##TNAME* item) { return (RTYPE)((item)->MEMBER); }
#define XCCDF_GENERIC_IGETTER(ITYPE,TNAME,MNAME) \
        struct xccdf_##ITYPE##_iterator* xccdf_##TNAME##_##MNAME(const struct xccdf_##TNAME* item) \
        { return xccdf_iterator_new(item->MNAME); }
#define XCCDF_ABSTRACT_GETTER(RTYPE,TNAME,MNAME,MEMBER) \
        RTYPE xccdf_##TNAME##_##MNAME(const struct xccdf_##TNAME* item) { return (RTYPE)(XITEM(item)->MEMBER); }
#define XCCDF_ITERATOR_GETTER(ITYPE,TNAME,MNAME,MEMBER) \
        struct xccdf_##ITYPE##_iterator* xccdf_##TNAME##_##MNAME(const struct xccdf_##TNAME* item) \
        { return xccdf_iterator_new(XITEM(item)->MEMBER); }
#define XCCDF_HTABLE_GETTER(RTYPE,TNAME,MNAME,MEMBER) \
		RTYPE xccdf_##TNAME##_##MNAME(const struct xccdf_##TNAME* item, const char* key) \
		{ return (RTYPE)xccdf_htable_get(XITEM(item)->MEMBER, key); }

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

#define XCCDF_ITEM_GETTER(RTYPE,MNAME) \
		XCCDF_ABSTRACT_GETTER(RTYPE,item,MNAME,item.MNAME) \
        XCCDF_BENCHMARK_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_PROFILE_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_RULE_GETTER_A(RTYPE,MNAME,item.MNAME) \
        XCCDF_GROUP_GETTER_A(RTYPE,MNAME,item.MNAME)
#define XCCDF_ITEM_IGETTER(RTYPE,MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,item,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,benchmark,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,profile,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,rule,MNAME,item.MNAME) \
        XCCDF_ITERATOR_GETTER(RTYPE,group,MNAME,item.MNAME)
#define XCCDF_FLAG_GETTER(MNAME) \
        XCCDF_BENCHMARK_GETTER_A(bool,MNAME,item.flags.MNAME) \
        XCCDF_PROFILE_GETTER_A(bool,MNAME,item.flags.MNAME) \
        XCCDF_RULE_GETTER_A(bool,MNAME,item.flags.MNAME) \
        XCCDF_GROUP_GETTER_A(bool,MNAME,item.flags.MNAME)


#endif

