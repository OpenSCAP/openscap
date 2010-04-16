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
    struct oscap_list *warnings;

	char *version;
	char *version_update;
	time_t version_time;

	struct xccdf_item *parent;
	char *extends;
	struct oscap_list *statuses;
	struct oscap_list *references;
	struct oscap_list *platforms;
	struct xccdf_flags flags;
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

/* This structure is used for multiple-count attributes for Item (0-n) */
struct xccdf_value_val { 
	union xccdf_value_unit value;               /* Value::value   */
	union xccdf_value_unit defval;              /* Value::default */
	struct oscap_list *choices;                 /* Value::choices */
	bool must_match;                            /* */
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
	xccdf_value_type_t type;                    /* Value::type */
	xccdf_interface_hint_t interface_hint;      /* Value::interfaceHint */
	xccdf_operator_t oper;                      /* Value::operator */
	char *selector;

	struct xccdf_value_val *value;
	struct oscap_htable *values; // TODO: to list

	struct oscap_list *sources;                 /* Value::source */
};

struct xccdf_result_item {
	time_t start_time;
	time_t end_time;
	char *test_system;
	char *benchmark_uri;
	char *profile;

	struct oscap_list *identities;
	struct oscap_list *targets;
	struct oscap_list *organizations;
	struct oscap_list *remarks;
	struct oscap_list *target_addresses;
	struct oscap_list *target_facts;
	struct oscap_list *setvalues;
	struct oscap_list *rule_results;
	struct oscap_list *scores;
};

struct xccdf_profile_item {
	char *note_tag;
	struct oscap_list *selects;
	struct oscap_list *setvalues;
	struct oscap_list *refine_values;
	struct oscap_list *refine_rules;
};

struct xccdf_benchmark_item {

	struct oscap_htable *dict;
	struct oscap_list *notices;
	struct oscap_list *plain_texts;

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
		struct xccdf_benchmark_item benchmark;
		struct xccdf_rule_item rule;
		struct xccdf_group_item group;
		struct xccdf_value_item value;
		struct xccdf_result_item result;
	} sub;
};

struct xccdf_warning {
    struct oscap_text *text;
    xccdf_warning_category_t category;
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
	struct oscap_htable *params; // TODO: to list + accessors
};

struct xccdf_select {
	char *item;
	bool selected;
	struct oscap_list *remarks;
};

struct xccdf_refine_rule {
	char *item;
	char *selector;
	xccdf_role_t role;
	xccdf_level_t severity;
	xccdf_numeric weight;
	struct oscap_list *remarks;
};

struct xccdf_refine_value {
	char *item;
	char *selector;
	xccdf_operator_t oper;
	struct oscap_list *remarks;
};

struct xccdf_setvalue {
	char *item;
	char *value;
};

struct xccdf_ident {
	char *id;
	char *system;
};

struct xccdf_check {
	xccdf_bool_operator_t oper;
	struct oscap_list *children;
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
	char *value;
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
	char *fixref;
	char *content;
};

struct xccdf_reference {
    bool override;
    char *href;
    char *content;
    char *lang;
};

struct xccdf_rule_result {
	char *idref;
	xccdf_role_t role;
	time_t time;
	float weight;
	xccdf_level_t severity;
	xccdf_test_result_type_t result;
	char *version;

	struct oscap_list *overrides;
	struct oscap_list *idents;
	struct oscap_list *messages;
	struct oscap_list *instances;
	struct oscap_list *fixes;
	struct oscap_list *checks;
};

struct xccdf_identity {
	struct {
		bool authenticated : 1;
		bool privileged    : 1;
	} sub;
	char *name;
};

struct xccdf_score {
	xccdf_numeric maximum;
	xccdf_numeric score;
	char *system;
};

struct xccdf_override {
	time_t time;
	char *authority;
	xccdf_test_result_type_t old_result;
	xccdf_test_result_type_t new_result;
	struct oscap_text *remark;
};

struct xccdf_message {
	xccdf_message_severity_t severity;
	char *content;
};

struct xccdf_target_fact {
	xccdf_value_type_t type;
	char *name;
	char *value;
};

struct xccdf_instance {
    char *context;
    char *parent_context;
    char *content;
};

struct xccdf_plain_text {
    char *id;
    char *text;
};

extern const struct oscap_string_map XCCDF_LEVEL_MAP[];
extern const struct oscap_string_map XCCDF_ROLE_MAP[];
extern const struct oscap_string_map XCCDF_OPERATOR_MAP[];
extern const struct oscap_string_map XCCDF_STRATEGY_MAP[];
extern const struct oscap_string_map XCCDF_FACT_TYPE_MAP[];
extern const struct oscap_string_map XCCDF_RESULT_MAP[];

extern const struct oscap_text_traits XCCDF_TEXT_PLAIN;
extern const struct oscap_text_traits XCCDF_TEXT_HTML;
extern const struct oscap_text_traits XCCDF_TEXT_PLAINSUB;
extern const struct oscap_text_traits XCCDF_TEXT_HTMLSUB;
extern const struct oscap_text_traits XCCDF_TEXT_NOTICE;
extern const struct oscap_text_traits XCCDF_TEXT_PROFNOTE;

extern const size_t XCCDF_NUMERIC_SIZE;
extern const char *XCCDF_NUMERIC_FORMAT;

struct xccdf_item *xccdf_item_new(xccdf_type_t type, struct xccdf_item *parent);
void xccdf_item_release(struct xccdf_item *item);
void xccdf_item_print(struct xccdf_item *item, int depth);
void xccdf_item_dump(struct xccdf_item *item, int depth);
struct xccdf_item* xccdf_item_get_benchmark_internal(struct xccdf_item* item);

bool xccdf_benchmark_parse(struct xccdf_item *benchmark, xmlTextReaderPtr reader);
void xccdf_benchmark_dump(struct xccdf_benchmark *benchmark);
bool xccdf_benchmark_register_item(struct xccdf_benchmark *benchmark, struct xccdf_item *item);
bool xccdf_benchmark_unregister_item(struct xccdf_item *item);
bool xccdf_benchmark_rename_item(struct xccdf_item *item, const char *newid);
char *xccdf_benchmark_gen_id(struct xccdf_benchmark *benchmark, const char *prefix);
bool xccdf_add_item(struct oscap_list *list, struct xccdf_item *parent, struct xccdf_item *item, const char *prefix);

struct xccdf_item *xccdf_profile_new_internal(struct xccdf_item *bench);
struct xccdf_item *xccdf_profile_parse(xmlTextReaderPtr reader, struct xccdf_item *bench);
void xccdf_profile_dump(struct xccdf_item *prof, int depth);

bool xccdf_item_process_attributes(struct xccdf_item *item, xmlTextReaderPtr reader);
bool xccdf_item_process_element(struct xccdf_item *item, xmlTextReaderPtr reader);

bool xccdf_content_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
struct xccdf_item *xccdf_group_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
struct xccdf_item *xccdf_group_new_internal(struct xccdf_item *parent);
void xccdf_group_dump(struct xccdf_item *group, int depth);

struct xccdf_item *xccdf_rule_new_internal(struct xccdf_item *parent);
struct xccdf_item *xccdf_rule_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
void xccdf_rule_dump(struct xccdf_item *rule, int depth);

struct xccdf_item *xccdf_value_parse(xmlTextReaderPtr reader, struct xccdf_item *parent);
struct xccdf_item *xccdf_value_new_internal(struct xccdf_item *parent, xccdf_value_type_t type);
void xccdf_value_dump(struct xccdf_item *value, int depth);

struct xccdf_notice *xccdf_notice_new_parse(xmlTextReaderPtr reader);
void xccdf_notice_dump(struct xccdf_notice *notice, int depth);

void xccdf_status_dump(struct xccdf_status *status, int depth);

struct xccdf_model *xccdf_model_new_xml(xmlTextReaderPtr reader);

void xccdf_cstring_dump(const char *data, int depth);
void xccdf_result_dump(struct xccdf_result *res, int depth);
struct xccdf_result *xccdf_result_new_parse(xmlTextReaderPtr reader);


struct xccdf_check *xccdf_check_parse(xmlTextReaderPtr reader);
void xccdf_check_dump(struct xccdf_check *check, int depth);
void xccdf_check_content_ref_dump(struct xccdf_check_content_ref *ref, int depth);
struct xccdf_ident *xccdf_ident_parse(xmlTextReaderPtr reader);
void xccdf_ident_dump(struct xccdf_ident *ident, int depth);
struct xccdf_fix *xccdf_fix_parse(xmlTextReaderPtr reader);
struct xccdf_fixtext *xccdf_fixtext_parse(xmlTextReaderPtr reader);

struct xccdf_setvalue *xccdf_setvalue_new_parse(xmlTextReaderPtr reader);
void xccdf_setvalue_dump(struct xccdf_setvalue *sv, int depth);

struct xccdf_warning *xccdf_warning_new_parse(xmlTextReaderPtr reader);
struct xccdf_reference *xccdf_reference_new_parse(xmlTextReaderPtr reader);

#include "unused.h"

OSCAP_HIDDEN_END;

#endif
