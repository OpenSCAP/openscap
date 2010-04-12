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
#include <math.h>


// constants
static const xccdf_numeric XCCDF_SCORE_MAX_DAFAULT = 100.0f;
static const char *XCCDF_INSTANCE_DEFAULT_CONTEXT = "undefined";
const size_t XCCDF_NUMERIC_SIZE = 32;
const char *XCCDF_NUMERIC_FORMAT = "%f";

// prototypes
void xccdf_rule_result_free(struct xccdf_rule_result *rr);
void xccdf_identity_free(struct xccdf_identity *identity);
void xccdf_score_free(struct xccdf_score *score);
void xccdf_target_fact_free(struct xccdf_target_fact *fact);
void xccdf_message_free(struct xccdf_message *msg);
void xccdf_instance_free(struct xccdf_instance *inst);
void xccdf_override_free(struct xccdf_override *oride);

struct xccdf_result *xccdf_result_new(void)
{
	struct xccdf_item *result = xccdf_item_new(XCCDF_RESULT, NULL);
	oscap_create_lists(&result->sub.result.identities, &result->sub.result.targets,
		&result->sub.result.remarks, &result->sub.result.target_addresses,
		&result->sub.result.target_facts, &result->sub.result.setvalues, &result->sub.result.organizations,
		&result->sub.result.rule_results, &result->sub.result.scores, NULL);
	return XRESULT(result);
}

static inline void xccdf_result_free_impl(struct xccdf_item *result)
{
	if (result != NULL) {
		oscap_free(result->sub.result.test_system);
		oscap_free(result->sub.result.benchmark_uri);
		oscap_free(result->sub.result.profile);

		oscap_list_free(result->sub.result.identities, (oscap_destruct_func) xccdf_identity_free);
		oscap_list_free(result->sub.result.target_facts, (oscap_destruct_func) xccdf_target_fact_free);
		oscap_list_free(result->sub.result.scores, (oscap_destruct_func) xccdf_score_free);
		oscap_list_free(result->sub.result.targets, oscap_free);
		oscap_list_free(result->sub.result.remarks, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(result->sub.result.target_addresses, oscap_free);
		oscap_list_free(result->sub.result.setvalues, (oscap_destruct_func) xccdf_setvalue_free);
		oscap_list_free(result->sub.result.rule_results, (oscap_destruct_func) xccdf_rule_result_free);
		oscap_list_free(result->sub.result.organizations, oscap_free);

		xccdf_item_release(result);
	}
}
XCCDF_FREE_GEN(result)

XCCDF_ACCESSOR_STRING(result, test_system)
XCCDF_ACCESSOR_STRING(result, benchmark_uri)
XCCDF_ACCESSOR_STRING(result, profile)
XCCDF_LISTMANIP(result, identity, identities)
XCCDF_LISTMANIP_STRING(result, target, targets)
XCCDF_LISTMANIP_STRING(result, target_address, target_addresses)
XCCDF_LISTMANIP_STRING(result, organization, organizations)
XCCDF_LISTMANIP_TEXT(result, remark, remarks)
XCCDF_LISTMANIP(result, target_fact, target_facts)
XCCDF_LISTMANIP(result, setvalue, setvalues)
XCCDF_LISTMANIP(result, rule_result, rule_results)
XCCDF_LISTMANIP(result, score, scores)
OSCAP_ITERATOR_GEN(xccdf_result)


struct xccdf_rule_result *xccdf_rule_result_new(void)
{
	struct xccdf_rule_result *rr = oscap_calloc(1, sizeof(struct xccdf_rule_result));
	oscap_create_lists(&rr->overrides, &rr->idents, &rr->messages,
		&rr->instances, &rr->fixes, &rr->checks, NULL);
	return rr;
}

void xccdf_rule_result_free(struct xccdf_rule_result *rr)
{
	if (rr != NULL) {
		oscap_free(rr->idref);
		oscap_free(rr->version);

		oscap_list_free(rr->overrides, (oscap_destruct_func) xccdf_override_free);
		oscap_list_free(rr->idents, (oscap_destruct_func) xccdf_ident_free);
		oscap_list_free(rr->messages, (oscap_destruct_func) xccdf_message_free);
		oscap_list_free(rr->instances, (oscap_destruct_func) xccdf_instance_free);
		oscap_list_free(rr->fixes, (oscap_destruct_func) xccdf_fix_free);
		oscap_list_free(rr->checks, (oscap_destruct_func) xccdf_check_free);

		oscap_free(rr);
	}
}

OSCAP_ACCESSOR_SIMPLE(time_t, xccdf_rule_result, time)
OSCAP_ACCESSOR_SIMPLE(xccdf_role_t, xccdf_rule_result, role)
OSCAP_ACCESSOR_SIMPLE(float, xccdf_rule_result, weight)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_rule_result, severity)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_rule_result, result)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, version)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, idref)
OSCAP_IGETINS(xccdf_ident, xccdf_rule_result, idents, ident)
OSCAP_IGETINS(xccdf_fix, xccdf_rule_result, fixes, fix)
OSCAP_IGETTER(xccdf_check, xccdf_rule_result, checks)
OSCAP_IGETINS_GEN(xccdf_override, xccdf_rule_result, overrides, override)
OSCAP_IGETINS_GEN(xccdf_message, xccdf_rule_result, messages, message)
OSCAP_IGETINS_GEN(xccdf_instance, xccdf_rule_result, instances, instance)


struct xccdf_identity *xccdf_identity_new(void)
{
	return oscap_calloc(1, sizeof(struct xccdf_identity));
}

void xccdf_identity_free(struct xccdf_identity *identity)
{
	if (identity != NULL) {
		oscap_free(identity->name);
		oscap_free(identity);
	}
}

OSCAP_ACCESSOR_EXP(bool, xccdf_identity, authenticated, sub.authenticated)
OSCAP_ACCESSOR_EXP(bool, xccdf_identity, privileged, sub.privileged)
OSCAP_ACCESSOR_STRING(xccdf_identity, name)

struct xccdf_score *xccdf_score_new(void)
{
	struct xccdf_score *score = oscap_calloc(1, sizeof(struct xccdf_score));
	score->score = NAN;
	score->maximum = XCCDF_SCORE_MAX_DAFAULT;
	return score;
}

void xccdf_score_free(struct xccdf_score *score)
{
	if (score != NULL) {
		oscap_free(score->system);
		oscap_free(score);
	}
}

OSCAP_ACCESSOR_SIMPLE(xccdf_numeric, xccdf_score, maximum)
OSCAP_ACCESSOR_SIMPLE(xccdf_numeric, xccdf_score, score)
OSCAP_ACCESSOR_STRING(xccdf_score, system)

struct xccdf_override *xccdf_override_new(void)
{
	return oscap_calloc(1, sizeof(struct xccdf_override));
}

void xccdf_override_free(struct xccdf_override *oride)
{
	if (oride != NULL) {
		oscap_free(oride->authority);
		oscap_text_free(oride->remark);
		oscap_free(oride);
	}
}

OSCAP_ACCESSOR_SIMPLE(time_t, xccdf_override, time)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_override, new_result)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_override, old_result)
OSCAP_ACCESSOR_STRING(xccdf_override, authority)
OSCAP_ACCESSOR_TEXT(xccdf_override, remark)

struct xccdf_message *xccdf_message_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_message));
}

void xccdf_message_free(struct xccdf_message *msg)
{
    if (msg != NULL) {
        oscap_free(msg->content);
        oscap_free(msg);
    }
}

OSCAP_ACCESSOR_SIMPLE(xccdf_message_severity_t, xccdf_message, severity)
OSCAP_ACCESSOR_STRING(xccdf_message, content)

struct xccdf_target_fact* xccdf_target_fact_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_target_fact));
}

void xccdf_target_fact_free(struct xccdf_target_fact *fact)
{
    if (fact != NULL) {
        oscap_free(fact->name);
        oscap_free(fact->value);
        oscap_free(fact);
    }
}

static inline bool xccdf_target_fact_set_value(struct xccdf_target_fact *fact, xccdf_value_type_t type, const char *str)
{
    assert(fact != NULL);
    oscap_free(fact->value);
    fact->value = oscap_strdup(str);
    fact->type  = type;
    return true;
}

bool xccdf_target_fact_set_string(struct xccdf_target_fact *fact, const char *str)
{
    return xccdf_target_fact_set_value(fact, XCCDF_TYPE_STRING, str);
}

bool xccdf_target_fact_set_number(struct xccdf_target_fact *fact, xccdf_numeric val)
{
    char buff[XCCDF_NUMERIC_SIZE];
    sprintf(buff, XCCDF_NUMERIC_FORMAT, val);
    return xccdf_target_fact_set_value(fact, XCCDF_TYPE_NUMBER, buff);
}

bool xccdf_target_fact_set_boolean(struct xccdf_target_fact *fact, bool val)
{
    return xccdf_target_fact_set_value(fact, XCCDF_TYPE_BOOLEAN, oscap_enum_to_string(OSCAP_BOOL_MAP, val));
}

OSCAP_GETTER(xccdf_value_type_t, xccdf_target_fact, type)
OSCAP_GETTER(const char *, xccdf_target_fact, value)
OSCAP_ACCESSOR_STRING(xccdf_target_fact, name)

struct xccdf_instance *xccdf_instance_new(void)
{
    struct xccdf_instance *inst = oscap_calloc(1, sizeof(struct xccdf_instance));
    inst->context = oscap_strdup(XCCDF_INSTANCE_DEFAULT_CONTEXT);
    return inst;
}

void xccdf_instance_free(struct xccdf_instance *inst)
{
    if (inst != NULL) {
        oscap_free(inst->context);
        oscap_free(inst->parent_context);
        oscap_free(inst->content);
        oscap_free(inst);
    }
}

OSCAP_ACCESSOR_STRING(xccdf_instance, context)
OSCAP_ACCESSOR_STRING(xccdf_instance, parent_context)
OSCAP_ACCESSOR_STRING(xccdf_instance, content)

/* ------ string maps ------ */

const struct oscap_string_map XCCDF_FACT_TYPE_MAP[] = {
	{ XCCDF_TYPE_BOOLEAN, "boolean" },
	{ XCCDF_TYPE_STRING,  "string"  },
	{ XCCDF_TYPE_NUMBER,  "number"  },
	{ XCCDF_TYPE_BOOLEAN, NULL      }
};

const struct oscap_string_map XCCDF_RESULT_MAP[] = {
	{ XCCDF_RESULT_PASS,           "pass"          },
	{ XCCDF_RESULT_FAIL,           "fail"          },
	{ XCCDF_RESULT_ERROR,          "error"         },
	{ XCCDF_RESULT_UNKNOWN,        "unknown"       },
	{ XCCDF_RESULT_NOT_APPLICABLE, "notapplicable" },
	{ XCCDF_RESULT_NOT_CHECKED,    "notchecked"    },
	{ XCCDF_RESULT_NOT_SELECTED,   "notselected"   },
	{ XCCDF_RESULT_INFORMATIONAL,  "informational" },
	{ XCCDF_RESULT_FIXED,          "fixed"         },
	{ 0, NULL }
};

/* --------- import -------- */

static struct xccdf_identity *xccdf_identity_new_parse(xmlTextReaderPtr reader);
static struct xccdf_target_fact *xccdf_target_fact_new_parse(xmlTextReaderPtr reader);
static struct xccdf_score *xccdf_score_new_parse(xmlTextReaderPtr reader);
static struct xccdf_rule_result *xccdf_rule_result_new_parse(xmlTextReaderPtr reader);
static struct xccdf_override *xccdf_override_new_parse(xmlTextReaderPtr reader);
static struct xccdf_message *xccdf_message_new_parse(xmlTextReaderPtr reader);
static struct xccdf_instance *xccdf_instance_new_parse(xmlTextReaderPtr reader);

struct xccdf_result *xccdf_result_new_parse(xmlTextReaderPtr reader)
{
	assert(reader != NULL);
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_TESTRESULT);

	struct xccdf_item *res = XITEM(xccdf_result_new());

	if (!xccdf_item_process_attributes(res, reader)) goto fail;

	if (xccdf_attribute_has(reader, XCCDFA_END_TIME))
		res->sub.result.start_time = oscap_get_datetime(xccdf_attribute_get(reader, XCCDFA_END_TIME));
	else goto fail;
	res->sub.result.start_time = oscap_get_datetime(xccdf_attribute_get(reader, XCCDFA_START_TIME));
	res->item.version = xccdf_attribute_copy(reader, XCCDFA_VERSION);
	res->sub.result.test_system = xccdf_attribute_copy(reader, XCCDFA_TEST_SYSTEM);

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_ORGANIZATION:
			oscap_list_add(res->sub.result.organizations, oscap_element_string_copy(reader));
			break;
		case XCCDFE_IDENTITY:
			oscap_list_add(res->sub.result.identities, xccdf_identity_new_parse(reader));
			break;
		case XCCDFE_RESULT_PROFILE:
			if (res->sub.result.profile == NULL)
				res->sub.result.profile = oscap_element_string_copy(reader);
			break;
		case XCCDFE_TARGET:
			oscap_list_add(res->sub.result.targets, oscap_element_string_copy(reader));
			break;
		case XCCDFE_TARGET_ADDRESS:
			oscap_list_add(res->sub.result.target_addresses, oscap_element_string_copy(reader));
			break;
		case XCCDFE_TARGET_FACTS:
			while (oscap_to_start_element(reader, depth + 1)) {
				if (xccdf_element_get(reader) == XCCDFE_FACT)
					oscap_list_add(res->sub.result.target_facts, xccdf_target_fact_new_parse(reader));
				xmlTextReaderRead(reader);
			}
			break;
		case XCCDFE_SET_VALUE:
			oscap_list_add(res->sub.result.setvalues, xccdf_setvalue_new_parse(reader));
			break;
		case XCCDFE_RULE_RESULT:
			oscap_list_add(res->sub.result.rule_results, xccdf_rule_result_new_parse(reader));
			break;
		case XCCDFE_SCORE:
			oscap_list_add(res->sub.result.scores, xccdf_score_new_parse(reader));
			break;
		case XCCDFE_RESULT_BENCHMARK:
			if (res->sub.result.benchmark_uri == NULL)
				res->sub.result.benchmark_uri = xccdf_attribute_copy(reader, XCCDFA_HREF);
			break;
		default: xccdf_item_process_element(res, reader);
		}
		xmlTextReaderRead(reader);
	}

	return XRESULT(res);

fail:
	xccdf_result_free(XRESULT(res));
	return NULL;
}

static struct xccdf_identity *xccdf_identity_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_IDENTITY);

	struct xccdf_identity *identity = xccdf_identity_new();
	identity->sub.authenticated = xccdf_attribute_get_bool(reader, XCCDFA_AUTHENTICATED);
	identity->sub.privileged    = xccdf_attribute_get_bool(reader, XCCDFA_PRIVILEDGED);
	identity->name              = oscap_element_string_copy(reader);
	return identity;
}

static struct xccdf_target_fact *xccdf_target_fact_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_FACT);

	struct xccdf_target_fact *fact = xccdf_target_fact_new();
	fact->type = oscap_string_to_enum(XCCDF_FACT_TYPE_MAP, xccdf_attribute_get(reader, XCCDFA_TYPE));
	fact->name = xccdf_attribute_copy(reader, XCCDFA_NAME);
	fact->value = oscap_element_string_copy(reader);
	return fact;
}

static struct xccdf_score *xccdf_score_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_SCORE);

	struct xccdf_score *score = xccdf_score_new();
	score->system = xccdf_attribute_copy(reader, XCCDFA_SYSTEM);
	if (xccdf_attribute_has(reader, XCCDFA_MAXIMUM))
		score->maximum = xccdf_attribute_get_float(reader, XCCDFA_MAXIMUM);
	else score->maximum = XCCDF_SCORE_MAX_DAFAULT;
	score->score = atof(oscap_element_string_get(reader));
	return score;
}

static struct xccdf_rule_result *xccdf_rule_result_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_RULE_RESULT);

	struct xccdf_rule_result *rr = xccdf_rule_result_new();

	rr->idref    = xccdf_attribute_copy(reader, XCCDFA_IDREF);
	rr->role     = oscap_string_to_enum(XCCDF_ROLE_MAP, xccdf_attribute_get(reader, XCCDFA_ROLE));
	rr->time     = oscap_get_datetime(xccdf_attribute_get(reader, XCCDFA_TIME));
	rr->version  = xccdf_attribute_copy(reader, XCCDFA_VERSION);
	rr->weight   = xccdf_attribute_get_float(reader, XCCDFA_WEIGHT);
	rr->severity = oscap_string_to_enum(XCCDF_LEVEL_MAP, xccdf_attribute_get(reader, XCCDFA_SEVERITY));

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_RESULT:
			rr->result = oscap_string_to_enum(XCCDF_RESULT_MAP, oscap_element_string_get(reader));
			break;
		case XCCDFE_OVERRIDE:
			oscap_list_add(rr->overrides, xccdf_override_new_parse(reader));
			break;
		case XCCDFE_IDENT:
			oscap_list_add(rr->idents, xccdf_ident_parse(reader));
			break;
		case XCCDFE_MESSAGE:
			oscap_list_add(rr->messages, xccdf_message_new_parse(reader));
			break;
		case XCCDFE_INSTANCE:
			oscap_list_add(rr->instances, xccdf_instance_new_parse(reader));
			break;
		case XCCDFE_FIX:
			oscap_list_add(rr->fixes, xccdf_fix_parse(reader));
			break;
		case XCCDFE_CHECK:
			oscap_list_add(rr->checks, xccdf_check_parse(reader));
			break;
		default: break;
		}
		xmlTextReaderRead(reader);
	}

	return rr;
}

static struct xccdf_override *xccdf_override_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_OVERRIDE);

	struct xccdf_override *override = xccdf_override_new();

	override->time      = oscap_get_datetime(xccdf_attribute_get(reader, XCCDFA_TIME));
	override->authority = xccdf_attribute_copy(reader, XCCDFA_AUTHORITY);

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_OLD_RESULT:
			override->old_result = oscap_string_to_enum(XCCDF_RESULT_MAP, oscap_element_string_get(reader));
			break;
		case XCCDFE_NEW_RESULT:
			override->new_result = oscap_string_to_enum(XCCDF_RESULT_MAP, oscap_element_string_get(reader));
			break;
		case XCCDFE_REMARK:
			if (override->remark == NULL)
				override->remark = oscap_text_new_parse(XCCDF_TEXT_PLAIN, reader);
			break;
		default: break;
		}
		xmlTextReaderRead(reader);
	}

	return override;
}

static struct xccdf_message *xccdf_message_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_MESSAGE);

	struct xccdf_message *msg = xccdf_message_new();
	msg->severity = oscap_string_to_enum(XCCDF_LEVEL_MAP, xccdf_attribute_get(reader, XCCDFA_SEVERITY));
	msg->content  = oscap_element_string_copy(reader);
	return msg;
}

static struct xccdf_instance *xccdf_instance_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_INSTANCE);

	struct xccdf_instance *inst = xccdf_instance_new();
	if (xccdf_attribute_has(reader, XCCDFA_CONTEXT))
		xccdf_instance_set_context(inst, xccdf_attribute_get(reader, XCCDFA_CONTEXT));
	inst->parent_context = xccdf_attribute_copy(reader, XCCDFA_PARENTCONTEXT);
	inst->content        = oscap_element_string_copy(reader);
	return inst;
}

/* --------  DUMP ---------- */

static void xccdf_rule_result_dump(struct xccdf_rule_result *res, int depth)
{
	xccdf_print_depth(depth); printf("Rule result: %s\n", xccdf_rule_result_get_idref(res)); ++depth;
	xccdf_print_depth(depth); printf("version:   %s\n", xccdf_rule_result_get_version(res));
	xccdf_print_depth(depth); printf("weight:    %f\n", xccdf_rule_result_get_weight(res));
	/*
	xccdf_role_t role;
	time_t time;
	xccdf_level_t severity;
	xccdf_test_result_type_t result;
	struct oscap_list *overrides;
	struct oscap_list *idents;
	struct oscap_list *messages;
	struct oscap_list *instances;
	struct oscap_list *fixes;
	struct oscap_list *checks;
	*/
}

void xccdf_result_dump(struct xccdf_result *res, int depth)
{
	xccdf_print_depth(depth); printf("TestResult : %s\n", (res ? xccdf_result_get_id(res) : "(NULL)"));
	if (res != NULL) {
		++depth; xccdf_item_print(XITEM(res), depth);
		xccdf_print_depth(depth); printf("test system:   %s\n", xccdf_result_get_test_system(res));
		xccdf_print_depth(depth); printf("benchmark URI: %s\n", xccdf_result_get_benchmark_uri(res));
		xccdf_print_depth(depth); printf("profile ID:    %s\n", xccdf_result_get_profile(res));
		// start time, end time...
		//xccdf_print_depth(depth); printf("identities");
		//oscap_list_dump(XITEM(res)->result.identities, NULL, depth+1);
		xccdf_print_depth(depth); printf("targets");
		oscap_list_dump(XITEM(res)->sub.result.targets, xccdf_cstring_dump, depth+1);
		xccdf_print_depth(depth); printf("organizations");
		oscap_list_dump(XITEM(res)->sub.result.organizations, xccdf_cstring_dump, depth+1);
		xccdf_print_depth(depth); printf("remarks");
		xccdf_print_textlist(xccdf_result_get_remarks(res), depth+1, 80, "...");
		xccdf_print_depth(depth); printf("target addresses");
		oscap_list_dump(XITEM(res)->sub.result.target_addresses, xccdf_cstring_dump, depth+1);
		//xccdf_print_depth(depth); printf("target_facts");
		xccdf_print_depth(depth); printf("setvalues");
		oscap_list_dump(XITEM(res)->sub.result.setvalues, xccdf_setvalue_dump, depth+1);
		xccdf_print_depth(depth); printf("rule results");
		oscap_list_dump(XITEM(res)->sub.result.rule_results, (oscap_dump_func) xccdf_rule_result_dump, depth+1);
		//xccdf_print_depth(depth); printf("scores");
	}
}


