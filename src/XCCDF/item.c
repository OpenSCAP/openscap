/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
 * Copyright (C) 2010 Tresys Technology, LLC
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
 * 	Josh Adams <jadams@tresys.com>
 */

#include "item.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include "../common/util.h"
#include <string.h>
#include <time.h>
#include <math.h>

const struct oscap_string_map XCCDF_OPERATOR_MAP[] = {
	{XCCDF_OPERATOR_EQUALS, "equals"},
	{XCCDF_OPERATOR_NOT_EQUAL, "not equal"},
	{XCCDF_OPERATOR_GREATER, "greater than"},
	{XCCDF_OPERATOR_GREATER_EQUAL, "greater than or equal"},
	{XCCDF_OPERATOR_LESS, "less than"},
	{XCCDF_OPERATOR_LESS_EQUAL, "less than or equal"},
	{XCCDF_OPERATOR_PATTERN_MATCH, "pattern match"},
	{0, NULL}
};

const struct oscap_string_map XCCDF_LEVEL_MAP[] = {
	{XCCDF_UNKNOWN, "unknown"},
	{XCCDF_INFO, "info"},
	{XCCDF_LOW, "low"},
	{XCCDF_MEDIUM, "medium"},
	{XCCDF_HIGH, "high"},
	{0, NULL}
};

static const struct oscap_string_map XCCDF_STATUS_MAP[] = {
	{XCCDF_STATUS_ACCEPTED, "accepted"},
	{XCCDF_STATUS_DEPRECATED, "deprecated"},
	{XCCDF_STATUS_DRAFT, "draft"},
	{XCCDF_STATUS_INCOMPLETE, "incomplete"},
	{XCCDF_STATUS_INTERIM, "interim"},
	{XCCDF_STATUS_NOT_SPECIFIED, NULL}
};


struct xccdf_item *xccdf_item_new(xccdf_type_t type, struct xccdf_item *parent)
{
	struct xccdf_item *item;
	size_t size = sizeof(*item) - sizeof(item->sub);

	switch (type) {
	case XCCDF_BENCHMARK:
		size += sizeof(struct xccdf_benchmark_item);
		break;
	case XCCDF_RULE:
		size += sizeof(struct xccdf_rule_item);
		break;
	case XCCDF_GROUP:
		size += sizeof(struct xccdf_group_item);
		break;
	case XCCDF_VALUE:
		size += sizeof(struct xccdf_value_item);
		break;
	case XCCDF_RESULT:
		size += sizeof(struct xccdf_result_item);
		break;
	default:
		size += sizeof(item->sub);
		break;
	}

	item = oscap_calloc(1, size);
	item->type = type;
	item->item.title = oscap_list_new();
	item->item.description = oscap_list_new();
	item->item.question = oscap_list_new();
	item->item.rationale = oscap_list_new();
	item->item.statuses = oscap_list_new();
	item->item.platforms = oscap_list_new();
	item->item.warnings = oscap_list_new();
	item->item.references = oscap_list_new();
	item->item.weight = 1.0;
	item->item.flags.selected = true;
	item->item.parent = parent;
	return item;
}

struct xccdf_item *xccdf_item_clone(const struct xccdf_item *old_item)
{
	struct xccdf_item *new_item = oscap_calloc(1, sizeof(struct xccdf_item));

	new_item->item = *(xccdf_item_base_clone(&(old_item->item)));
	new_item->type = old_item->type;

	switch (new_item->type) {
	case XCCDF_BENCHMARK:
		 new_item->sub.benchmark = *(xccdf_benchmark_item_clone(&(old_item->sub.benchmark), new_item));
		break;
	case XCCDF_RULE:
		new_item->sub.rule = *(xccdf_rule_item_clone(&(old_item->sub.rule)));
		break;
	case XCCDF_GROUP:
		new_item->sub.group = *(xccdf_group_item_clone(&(old_item->sub.group), new_item));
		break;
	case XCCDF_VALUE:
		new_item->sub.value = *(xccdf_value_item_clone(&(old_item->sub.value)));
		break;
	case XCCDF_RESULT:
		new_item->sub.result = *(xccdf_result_item_clone(&(old_item->sub.result)));
		break;
	case XCCDF_PROFILE:
		new_item->sub.profile = *(xccdf_profile_item_clone(&(old_item->sub.profile)));
		break;
	default:
		//dont initialize the sub item.
		break;
	}
	
	return new_item;
}

/* Performs a deep copy of xccdf_item_base and returns a pointer to that copy */
struct xccdf_item_base *xccdf_item_base_clone(const struct xccdf_item_base *old_base)
{
	struct xccdf_item_base *new_base = oscap_calloc(1, sizeof(struct xccdf_item_base));
	new_base->id = oscap_strdup(old_base->id);
	new_base->cluster_id = oscap_strdup(old_base->cluster_id);
	new_base->version = oscap_strdup(old_base->version);
	new_base->version_update = oscap_strdup(old_base->version_update);
	new_base->extends = oscap_strdup(old_base->extends);

	new_base->version_time = old_base->version_time;
	new_base->weight = old_base->weight;
	new_base->parent = NULL;

	new_base->title = oscap_list_clone(old_base->title, (oscap_clone_func) oscap_text_clone);
	new_base->description = oscap_list_clone(old_base->description, (oscap_clone_func) oscap_text_clone);
	new_base->question = oscap_list_clone(old_base->question, (oscap_clone_func) oscap_text_clone);
	new_base->rationale = oscap_list_clone(old_base->rationale, (oscap_clone_func) oscap_text_clone);

	new_base->warnings = oscap_list_clone(old_base->warnings, (oscap_clone_func) xccdf_warning_clone);
	new_base->statuses = oscap_list_clone(old_base->statuses, (oscap_clone_func) xccdf_status_clone);
	new_base->references = oscap_list_clone(old_base->references, (oscap_clone_func) xccdf_reference_clone);
	new_base->platforms = oscap_list_clone(old_base->platforms, (oscap_clone_func) oscap_strdup);

	/* Handling flags */
	/*
	new_base->flags.selected = old_base->flags.selected;
	new_base->flags.hidden = old_base->flags.hidden;
	new_base->flags.resolved = old_base->flags.resolved;
	new_base->flags.abstract = old_base->flags.abstract;
	new_base->flags.prohibit_changes = old_base->flags.prohibit_changes;
	new_base->flags.interactive = old_base->flags.interactive;
	new_base->flags.multiple = old_base->flags.multiple;
	*/
	new_base->flags = old_base->flags;
	new_base->defined_flags = old_base->defined_flags;

	return new_base;
}



/* Performs a deep copy of xccdf_reference and returns a pointer to that copy */
struct xccdf_reference *xccdf_reference_clone(const struct xccdf_reference *old_reference)
{
	struct xccdf_reference *new_reference = oscap_calloc(1, sizeof(struct xccdf_reference));
	//new_reference->override = old_reference->override;
	new_reference->href = oscap_strdup(old_reference->href);
	new_reference->text = oscap_text_clone(old_reference->text);
	//new_reference->content = oscap_strdup(old_reference->content);
	//new_reference->lang = oscap_strdup(old_reference->lang);
	return new_reference;
}

/* Performs a deep copy of xccdf_status and returns a pointer to that copy */
struct xccdf_status *xccdf_status_clone(const struct xccdf_status *old_status)
{
	struct xccdf_status *new_status = oscap_calloc(1, sizeof(struct xccdf_status));
	new_status->status = old_status->status;
	new_status->date = old_status->date;
	return new_status;
}


/* Performs a deep copy of xccdf_warning and returns a pointer to that copy */
struct xccdf_warning *xccdf_warning_clone(const struct xccdf_warning *old_warning)
{
	struct xccdf_warning *new_warning = oscap_calloc(1, sizeof(struct xccdf_warning));
	new_warning->text = oscap_text_clone(old_warning->text);
	new_warning->category = old_warning->category;
	return new_warning;
}


void xccdf_item_release(struct xccdf_item *item)
{
	if (item) {
		oscap_list_free(item->item.statuses, (oscap_destruct_func) xccdf_status_free);
		oscap_list_free(item->item.platforms, oscap_free);
		oscap_list_free(item->item.title, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(item->item.description, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(item->item.rationale, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(item->item.question, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(item->item.warnings, (oscap_destruct_func) xccdf_warning_free);
		oscap_list_free(item->item.references, (oscap_destruct_func) xccdf_reference_free);
		oscap_free(item->item.id);
		oscap_free(item->item.cluster_id);
		oscap_free(item->item.version_update);
		oscap_free(item->item.version);
		oscap_free(item->item.extends);
		oscap_free(item);
	}
}

void xccdf_item_free(struct xccdf_item *item)
{
	if (item == NULL)
		return;
	switch (item->type) {
	case XCCDF_BENCHMARK:
		xccdf_benchmark_free(XBENCHMARK(item));
		break;
	case XCCDF_GROUP:
		xccdf_group_free(item);
		break;
	case XCCDF_RULE:
		xccdf_rule_free(item);
		break;
	case XCCDF_VALUE:
		xccdf_value_free(item);
		break;
	case XCCDF_RESULT:
		xccdf_result_free(XRESULT(item));
		break;
	default:
		assert((fprintf(stderr, "Deletion of item of type no. %u is not yet supported.", item->type), false));
	}
}

void xccdf_item_dump(struct xccdf_item *item, int depth)
{
	if (item == NULL)
		return;
	switch (item->type) {
	case XCCDF_BENCHMARK:
		xccdf_benchmark_dump(XBENCHMARK(item));
		break;
	case XCCDF_GROUP:
		xccdf_group_dump(item, depth);
		break;
	case XCCDF_RULE:
		xccdf_rule_dump(item, depth);
		break;
	default:
		xccdf_print_depth(depth);
		fprintf(stderr, "I cannot yet dump an item of type no. %u.", item->type);
	}
}

void xccdf_item_print(struct xccdf_item *item, int depth)
{
	if (item) {
		if (item->item.parent) {
			xccdf_print_depth(depth);
			printf("parent  : %s\n", item->item.parent->item.id);
		}
		if (item->item.extends) {
			xccdf_print_depth(depth);
			printf("extends : %s\n", item->item.extends);
		}
		if (item->type == XCCDF_BENCHMARK) {
			xccdf_print_depth(depth);
			printf("resolved: %d\n", item->item.flags.resolved);
		}
		if (item->type & XCCDF_CONTENT) {
			xccdf_print_depth(depth);
			printf("selected: %d\n", item->item.flags.selected);
		}
		if (item->item.version) {
			xccdf_print_depth(depth);
			printf("version : %s\n", item->item.version);
		}
		xccdf_print_depth(depth);
		printf("title   : ");
        xccdf_print_textlist(oscap_iterator_new(item->item.title), depth + 1, 70, "...");
		xccdf_print_depth(depth);
		printf("desc    : ");
        xccdf_print_textlist(oscap_iterator_new(item->item.description), depth + 1, 70, "...");
		xccdf_print_depth(depth);
		printf("platforms ");
		oscap_list_dump(item->item.platforms, xccdf_cstring_dump, depth + 1);
		xccdf_print_depth(depth);
		printf("status (cur = %d)", xccdf_item_get_current_status(item));
		oscap_list_dump(item->item.statuses, xccdf_status_dump, depth + 1);
	}
}

xmlNode *xccdf_item_to_dom(struct xccdf_item *item, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	xmlNode *item_node = NULL;
	if (parent == NULL)
		item_node = xmlNewNode(NULL, BAD_CAST "Item");
	else
		item_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "Item", NULL);

	/* Handle generic item attributes */
	const char *id = xccdf_item_get_id(item);
	xmlNewProp(item_node, BAD_CAST "id", BAD_CAST id);

	const char *cluster_id = xccdf_item_get_cluster_id(item);
	if (cluster_id)
		xmlNewProp(item_node, BAD_CAST "cluster-id", BAD_CAST cluster_id);

	if (xccdf_item_get_hidden(item))
		xmlNewProp(item_node, BAD_CAST "hidden", BAD_CAST "True");

	if (xccdf_item_get_prohibit_changes(item))
		xmlNewProp(item_node, BAD_CAST "prohibitChanges", BAD_CAST "True");

	if (xccdf_item_get_abstract(item))
		xmlNewProp(item_node, BAD_CAST "abstract", BAD_CAST "True");

	/* Handle generic item child nodes */
        struct oscap_text_iterator *titles = xccdf_item_get_title(item);
        while (oscap_text_iterator_has_more(titles)) {
                struct oscap_text *title = oscap_text_iterator_next(titles);
                xmlNode * child = xmlNewChild(item_node, ns_xccdf, BAD_CAST "title", BAD_CAST oscap_text_get_text(title));
                if (oscap_text_get_lang(title) != NULL) xmlNewProp(child, BAD_CAST "xml:lang", BAD_CAST oscap_text_get_lang(title));
        }
        oscap_text_iterator_free(titles);

        struct oscap_text_iterator *descriptions = xccdf_item_get_description(item);
        while (oscap_text_iterator_has_more(descriptions)) {
                struct oscap_text *description = oscap_text_iterator_next(descriptions);
                xmlNode * child = xmlNewChild(item_node, ns_xccdf, BAD_CAST "description", BAD_CAST oscap_text_get_text(description));
                if (oscap_text_get_lang(description) != NULL) xmlNewProp(child, BAD_CAST "xml:lang", BAD_CAST oscap_text_get_lang(description));
        }
        oscap_text_iterator_free(descriptions);

	const char *version= xccdf_item_get_version(item);
	if (version)
		xmlNewChild(item_node, ns_xccdf, BAD_CAST "version", BAD_CAST version);

	struct xccdf_status_iterator *statuses = xccdf_item_get_statuses(item);
	while (xccdf_status_iterator_has_more(statuses)) {
		struct xccdf_status *status = xccdf_status_iterator_next(statuses);
		xccdf_status_to_dom(status, doc, item_node);
	}
	xccdf_status_iterator_free(statuses);

        struct oscap_text_iterator *questions = xccdf_item_get_question(item);
        while (oscap_text_iterator_has_more(questions)) {
                struct oscap_text *question = oscap_text_iterator_next(questions);
                xmlNode * child = xmlNewChild(item_node, ns_xccdf, BAD_CAST "question", BAD_CAST oscap_text_get_text(question));
                if (oscap_text_get_lang(question) != NULL) xmlNewProp(child, BAD_CAST "xml:lang", BAD_CAST oscap_text_get_lang(question));
        }
        oscap_text_iterator_free(questions);

	struct xccdf_reference_iterator *references = xccdf_item_get_references(item);
	while (xccdf_reference_iterator_has_more(references)) {
		struct xccdf_reference *ref = xccdf_reference_iterator_next(references);
		xccdf_reference_to_dom(ref, doc, item_node);
	}

	/* Handle type specific attributes and children */
	switch (xccdf_item_get_type(item)) {
		case XCCDF_RULE:
			xmlNodeSetName(item_node,BAD_CAST "Rule");
			xccdf_rule_to_dom(XRULE(item), item_node, doc, parent);
			break;
		case XCCDF_BENCHMARK:
			xmlNodeSetName(item_node,BAD_CAST "Benchmark");
			break;
		case XCCDF_PROFILE:
			xmlNodeSetName(item_node,BAD_CAST "Profile");
			xccdf_profile_to_dom(XPROFILE(item), item_node, doc, parent);
			break;
		case XCCDF_RESULT:
			xmlNodeSetName(item_node,BAD_CAST "TestResult");
			xccdf_result_to_dom(XRESULT(item), item_node, doc, parent);
			break;
		case XCCDF_GROUP:
			xmlNodeSetName(item_node,BAD_CAST "Group");
			xccdf_group_to_dom(XGROUP(item), item_node, doc, parent);
			break;
		case XCCDF_VALUE:
			xmlNodeSetName(item_node,BAD_CAST "Value");
			xccdf_value_to_dom(XVALUE(item), item_node, doc, parent);
			break;
		case XCCDF_CONTENT:
			xmlNodeSetName(item_node,BAD_CAST "Content");
			break;
		case XCCDF_OBJECT:
			xmlNodeSetName(item_node,BAD_CAST "Object");
			break;
		default:
			return item_node;
	}

	return item_node;
}

xmlNode *xccdf_reference_to_dom(struct xccdf_reference *ref, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	xmlNode *reference_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "reference", BAD_CAST "");

	const char *lang = oscap_text_get_lang(xccdf_reference_get_text(ref));
	if (lang != NULL) xmlNewProp(reference_node, BAD_CAST "xml:lang", BAD_CAST lang);

	const char *href = xccdf_reference_get_href(ref);
	xmlNewProp(reference_node, BAD_CAST "href", BAD_CAST href);

        /* TODO: Dublin Core Elements /XML spec p. 69/ */

	return reference_node;
}

xmlNode *xccdf_profile_note_to_dom(struct xccdf_profile_note *note, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	xmlNode *note_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "profile-note", NULL);

	// This is in the XCCDF Spec, but not implemented in OpenSCAP
	//const char *lang = xccdf_profile_note_get_lang(note);
	//xmlNewProp(note_node, BAD_CAST "xml:lang", BAD_CAST lang);

	struct oscap_text *text = xccdf_profile_note_get_text(note);
	xmlNewChild(note_node, ns_xccdf, BAD_CAST "sub", BAD_CAST oscap_text_get_text(text));

	const char *reftag = xccdf_profile_note_get_reftag(note);
	xmlNewChild(note_node, ns_xccdf, BAD_CAST "tag", BAD_CAST reftag);

	return note_node;
}

xmlNode *xccdf_status_to_dom(struct xccdf_status *status, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);

	xmlNode *status_node = NULL;
	xccdf_status_type_t type = xccdf_status_get_status(status);
	if (type != XCCDF_STATUS_NOT_SPECIFIED)
		status_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "status",
							BAD_CAST XCCDF_STATUS_MAP[type - 1].string);

	time_t date_time = xccdf_status_get_date(status);
	struct tm *date = localtime(&date_time);
	char date_str[] = "YYYY-DD-MM";
	snprintf(date_str, sizeof(date_str), "%d-%d-%d", date->tm_year + 1900, date->tm_mon + 1, date->tm_mday);
	xmlNewProp(status_node, BAD_CAST "date", BAD_CAST date_str);

	return status_node;
}

xmlNode *xccdf_fixtext_to_dom(struct xccdf_fixtext *fixtext, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	xmlNode *fixtext_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "fixtext", NULL);

	// This is in the XCCDF Spec, but not implemented in OpenSCAP
	//const char *lang = xccdf_fixtext_get_lang(note);
	//xmlNewProp(note_node, BAD_CAST "xml:lang", BAD_CAST lang);
	//if (xccdf_fixtext_get_override(note))
	//	xmlNewProp(fixtext_node, BAD_CAST "override", BAD_CAST "True");
	
	if (xccdf_fixtext_get_reboot(fixtext))
		xmlNewProp(fixtext_node, BAD_CAST "reboot", BAD_CAST "True");

	const char *fixref = xccdf_fixtext_get_fixref(fixtext);
	xmlNewProp(fixtext_node, BAD_CAST "fixref", BAD_CAST fixref);

	xccdf_level_t complexity = xccdf_fixtext_get_complexity(fixtext);
        if (complexity != 0)
	        xmlNewProp(fixtext_node, BAD_CAST "complexity", BAD_CAST XCCDF_LEVEL_MAP[complexity].string);

	xccdf_level_t disruption = xccdf_fixtext_get_disruption(fixtext);
        if (disruption != 0)
	        xmlNewProp(fixtext_node, BAD_CAST "disruption", BAD_CAST XCCDF_LEVEL_MAP[disruption].string);

	xccdf_strategy_t strategy = xccdf_fixtext_get_strategy(fixtext);
        if (strategy != 0)
	        xmlNewProp(fixtext_node, BAD_CAST "strategy", BAD_CAST XCCDF_STRATEGY_MAP[strategy].string);

	//const char *content = fixtext->content;
	//xmlNewChild(fixtext_node, ns_xccdf, BAD_CAST "sub", BAD_CAST content);

	return fixtext_node;
}

xmlNode *xccdf_fix_to_dom(struct xccdf_fix *fix, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	const char *content = xccdf_fix_get_content(fix);
	xmlNode *fix_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "fix", BAD_CAST content);

	const char *id = xccdf_fix_get_id(fix);
	if (id != NULL) xmlNewProp(fix_node, BAD_CAST "id", BAD_CAST id);

	const char *sys = xccdf_fix_get_system(fix);
	if (sys != NULL) xmlNewProp(fix_node, BAD_CAST "system", BAD_CAST sys);

	if (xccdf_fix_get_reboot(fix))
		xmlNewProp(fix_node, BAD_CAST "reboot", BAD_CAST "True");

	xccdf_level_t complexity = xccdf_fix_get_complexity(fix);
        if (complexity != 0)
	    xmlNewProp(fix_node, BAD_CAST "complexity", BAD_CAST XCCDF_LEVEL_MAP[complexity-1].string);

	xccdf_level_t disruption = xccdf_fix_get_disruption(fix);
        if (disruption != 0)
	        xmlNewProp(fix_node, BAD_CAST "disruption", BAD_CAST XCCDF_LEVEL_MAP[disruption-1].string);

	xccdf_strategy_t strategy = xccdf_fix_get_strategy(fix);
        if (strategy != 0)
	        xmlNewProp(fix_node, BAD_CAST "strategy", BAD_CAST XCCDF_STRATEGY_MAP[strategy-1].string);

        // Sub element is used to store XCCDF value substitutions, not a content
	//xmlNewChild(fix_node, ns_xccdf, BAD_CAST "sub", BAD_CAST content);

	// This is in the XCCDF Spec, but not implemented in OpenSCAP
	//const char *instance = xccdf_fix_get_instance(fix);
	//xmlNewChild(fix_node, ns_xccdf, BAD_CAST "instance", BAD_CAST instance);
	
	return fix_node;
}

xmlNode *xccdf_ident_to_dom(struct xccdf_ident *ident, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	const char *id = xccdf_ident_get_id(ident);
	xmlNode *ident_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "ident", BAD_CAST id);

	const char *sys = xccdf_ident_get_system(ident);
	xmlNewProp(ident_node, BAD_CAST "system", BAD_CAST sys);

	return ident_node;
}

xmlNode *xccdf_check_to_dom(struct xccdf_check *check, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	xmlNode *check_node = NULL;
	if (xccdf_check_get_complex(check))
		check_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "complex-check", NULL);
	else
		check_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "check", NULL);

	const char *id = xccdf_check_get_id(check);
	if (id)
		xmlNewProp(check_node, BAD_CAST "id", BAD_CAST id);

	const char *sys = xccdf_check_get_system(check);
	xmlNewProp(check_node, BAD_CAST "system", BAD_CAST sys);

	const char *selector = xccdf_check_get_selector(check);
	if (selector)
		xmlNewProp(check_node, BAD_CAST "selector", BAD_CAST selector);

	/* Handle complex checks */
	struct xccdf_check_iterator *checks = xccdf_check_get_children(check);
	while (xccdf_check_iterator_has_more(checks)) {
		struct xccdf_check *new_check = xccdf_check_iterator_next(checks);
		xccdf_check_to_dom(new_check, doc, check_node);
	}
	xccdf_check_iterator_free(checks);

	struct xccdf_check_import_iterator *imports = xccdf_check_get_imports(check);
	while (xccdf_check_import_iterator_has_more(imports)) {
		struct xccdf_check_import *import = xccdf_check_import_iterator_next(imports);
		const char *name = xccdf_check_import_get_name(import);
		const char *content = xccdf_check_import_get_content(import);
		xmlNode *import_node = xmlNewChild(check_node, ns_xccdf, BAD_CAST "check-import", BAD_CAST content);
		xmlNewProp(import_node, BAD_CAST "import-name", BAD_CAST name);
	}
	xccdf_check_import_iterator_free(imports);

	struct xccdf_check_export_iterator *exports = xccdf_check_get_exports(check);
	while (xccdf_check_export_iterator_has_more(exports)) {
		struct xccdf_check_export *export = xccdf_check_export_iterator_next(exports);
		// This function seems like it should be in OpenSCAP, but isn't according to the docs
		//const char *name = xccdf_check_export_get_name(export);
		const char *name = export->name;
		const char *value= xccdf_check_export_get_value(export);
		xmlNode *export_node = xmlNewChild(check_node, ns_xccdf, BAD_CAST "check-export", NULL);
		xmlNewProp(export_node, BAD_CAST "export-name", BAD_CAST name);
		xmlNewProp(export_node, BAD_CAST "value-id", BAD_CAST value);
	}
	xccdf_check_export_iterator_free(exports);

	const char *content = xccdf_check_get_content(check);
	if (content)
		xmlNewChild(check_node, ns_xccdf, BAD_CAST "check-content", BAD_CAST content);

	struct xccdf_check_content_ref_iterator *refs = xccdf_check_get_content_refs(check);
	while (xccdf_check_content_ref_iterator_has_more(refs)) {
		struct xccdf_check_content_ref *ref = xccdf_check_content_ref_iterator_next(refs);
		xmlNode *ref_node = xmlNewChild(check_node, ns_xccdf, BAD_CAST "check-content-ref", NULL);

		const char *name = xccdf_check_content_ref_get_name(ref);
		xmlNewProp(ref_node, BAD_CAST "name", BAD_CAST name);

		const char *href = xccdf_check_content_ref_get_href(ref);
		xmlNewProp(ref_node, BAD_CAST "href", BAD_CAST href);
	}
	xccdf_check_content_ref_iterator_free(refs);

	return check_node;
}

#define XCCDF_ITEM_PROCESS_FLAG(reader,flag,attr) \
	if (xccdf_attribute_has((reader), (attr))) { \
		item->item.flags.flag = xccdf_attribute_get_bool((reader), (attr)); \
		item->item.defined_flags.flag = true; }

bool xccdf_item_process_attributes(struct xccdf_item *item, xmlTextReaderPtr reader)
{
	item->item.id = xccdf_attribute_copy(reader, XCCDFA_ID);

	XCCDF_ITEM_PROCESS_FLAG(reader, resolved, XCCDFA_RESOLVED);
	XCCDF_ITEM_PROCESS_FLAG(reader, hidden, XCCDFA_HIDDEN);
	XCCDF_ITEM_PROCESS_FLAG(reader, selected, XCCDFA_SELECTED);
	XCCDF_ITEM_PROCESS_FLAG(reader, prohibit_changes, XCCDFA_PROHIBITCHANGES);
	XCCDF_ITEM_PROCESS_FLAG(reader, multiple, XCCDFA_MULTIPLE);
	XCCDF_ITEM_PROCESS_FLAG(reader, abstract, XCCDFA_ABSTRACT);
	XCCDF_ITEM_PROCESS_FLAG(reader, interactive, XCCDFA_INTERACTIVE);

	if (xccdf_attribute_has(reader, XCCDFA_WEIGHT)) {
		item->item.weight = xccdf_attribute_get_float(reader, XCCDFA_WEIGHT);
		item->item.defined_flags.weight = true;
	}
	if (xccdf_attribute_has(reader, XCCDFA_EXTENDS))
		item->item.extends = xccdf_attribute_copy(reader, XCCDFA_EXTENDS);
	item->item.cluster_id = xccdf_attribute_copy(reader, XCCDFA_CLUSTER_ID);

	if (item->item.id) {
		struct xccdf_item *bench = XITEM(xccdf_item_get_benchmark_internal(item));
		if (bench != NULL && bench != item)
			oscap_htable_add(bench->sub.benchmark.dict, item->item.id, item);
	}
	return item->item.id != NULL;
}

bool xccdf_item_process_element(struct xccdf_item * item, xmlTextReaderPtr reader)
{
	xccdf_element_t el = xccdf_element_get(reader);

	switch (el) {
	case XCCDFE_TITLE:
        oscap_list_add(item->item.title, oscap_text_new_parse(XCCDF_TEXT_PLAIN, reader));
		return true;
	case XCCDFE_DESCRIPTION:
        oscap_list_add(item->item.description, oscap_text_new_parse(XCCDF_TEXT_HTMLSUB, reader));
		return true;
	case XCCDFE_WARNING:
        oscap_list_add(item->item.warnings, xccdf_warning_new_parse(reader));
		return true;
	case XCCDFE_REFERENCE:
        oscap_list_add(item->item.references, xccdf_reference_new_parse(reader));
		return true;
	case XCCDFE_STATUS:{
        const char *date = xccdf_attribute_get(reader, XCCDFA_DATE);
        char *str = oscap_element_string_copy(reader);
        struct xccdf_status *status = xccdf_status_new_fill(str, date);
        oscap_free(str);
        if (status) {
            oscap_list_add(item->item.statuses, status);
            return true;
        }
        break;
    }
	case XCCDFE_VERSION:
		item->item.version_time = oscap_get_datetime(xccdf_attribute_get(reader, XCCDFA_TIME));
		item->item.version_update = xccdf_attribute_copy(reader, XCCDFA_UPDATE);
		item->item.version = oscap_element_string_copy(reader);
		break;
	case XCCDFE_RATIONALE:
        oscap_list_add(item->item.rationale, oscap_text_new_parse(XCCDF_TEXT_HTMLSUB, reader));
		return true;
	case XCCDFE_QUESTION:
        oscap_list_add(item->item.question, oscap_text_new_parse(XCCDF_TEXT_PLAIN, reader));
		return true;
	default:
		break;
	}
	return false;
}

inline struct xccdf_item* xccdf_item_get_benchmark_internal(struct xccdf_item* item)
{
	if (item == NULL) return NULL;
	while (xccdf_item_get_parent(item) != NULL)
		item = xccdf_item_get_parent(item);
	return (xccdf_item_get_type(item) == XCCDF_BENCHMARK ? item : NULL);
}

#define XCCDF_BENCHGETTER(TYPE) \
	struct xccdf_benchmark* xccdf_##TYPE##_get_benchmark(const struct xccdf_##TYPE* item) \
	{ return XBENCHMARK(xccdf_item_get_benchmark_internal(XITEM(item))); }
XCCDF_BENCHGETTER(item)  XCCDF_BENCHGETTER(profile) XCCDF_BENCHGETTER(rule)
XCCDF_BENCHGETTER(group) XCCDF_BENCHGETTER(value)   XCCDF_BENCHGETTER(result)
#undef XCCDF_BENCHGETTER

static void *xccdf_item_convert(struct xccdf_item *item, xccdf_type_t type)
{
	return ((item != NULL && (item->type & type)) ? item : NULL);
}

#define XCCDF_ITEM_CONVERT(T1,T2) struct xccdf_##T1* xccdf_item_to_##T1(struct xccdf_item* item) { return xccdf_item_convert(item, XCCDF_##T2); }
XCCDF_ITEM_CONVERT(benchmark, BENCHMARK)
XCCDF_ITEM_CONVERT(profile, PROFILE)
XCCDF_ITEM_CONVERT(rule, RULE)
XCCDF_ITEM_CONVERT(group, GROUP)
XCCDF_ITEM_CONVERT(value, VALUE)
XCCDF_ITEM_CONVERT(result, RESULT)
#undef XCCDF_ITEM_CONVERT

#define XCCDF_ITEM_UPCAST(T) struct xccdf_item *xccdf_##T##_to_item(struct xccdf_##T *item) { return XITEM(item); }
XCCDF_ITEM_UPCAST(benchmark) XCCDF_ITEM_UPCAST(profile) XCCDF_ITEM_UPCAST(rule)
XCCDF_ITEM_UPCAST(group) XCCDF_ITEM_UPCAST(value) XCCDF_ITEM_UPCAST(result)
#undef XCCDF_ITEM_UPCAST

XCCDF_ABSTRACT_GETTER(xccdf_type_t, item, type, type)
XCCDF_ITEM_GETTER(const char *, id)

XCCDF_ITEM_TIGETTER(question);
XCCDF_ITEM_TIGETTER(rationale);
XCCDF_ITEM_TIGETTER(title);
XCCDF_ITEM_TIGETTER(description);
XCCDF_ITEM_ADDER(struct oscap_text *, question, question)
XCCDF_ITEM_ADDER(struct oscap_text *, title, title)
XCCDF_ITEM_ADDER(struct oscap_text *, description, description)
XCCDF_ITEM_ADDER(struct oscap_text *, rationale, rationale)

XCCDF_ITEM_GETTER(const char *, version)
XCCDF_ITEM_GETTER(const char *, cluster_id)
XCCDF_ITEM_GETTER(const char *, version_update) XCCDF_ITEM_GETTER(time_t, version_time) XCCDF_ITEM_GETTER(float, weight)
XCCDF_ITEM_GETTER(struct xccdf_item *, parent)
XCCDF_ITEM_GETTER(const char *, extends)
XCCDF_FLAG_GETTER(resolved)
XCCDF_FLAG_GETTER(hidden)
XCCDF_FLAG_GETTER(selected)
XCCDF_FLAG_GETTER(multiple)
XCCDF_FLAG_GETTER(prohibit_changes)
XCCDF_FLAG_GETTER(abstract)
XCCDF_FLAG_GETTER(interactive)
XCCDF_ITEM_SIGETTER(platforms)
XCCDF_ITEM_ADDER_STRING(platform, platforms)
XCCDF_ITEM_IGETTER(reference, references)
XCCDF_ITEM_IGETTER(warning, warnings)
XCCDF_ITEM_IGETTER(status, statuses)
XCCDF_ITEM_ADDER(struct xccdf_reference *, reference, references)
XCCDF_ITEM_ADDER(struct xccdf_warning *, warning, warnings)
XCCDF_ITEM_ADDER(struct xccdf_status *, status, statuses)
XCCDF_ITERATOR_GEN_S(item) XCCDF_ITERATOR_GEN_S(status) XCCDF_ITERATOR_GEN_S(reference)
OSCAP_ITERATOR_GEN_T(struct xccdf_warning *, xccdf_warning)

XCCDF_ITEM_SETTER_SIMPLE(xccdf_numeric, weight)
XCCDF_ITEM_SETTER_SIMPLE(time_t, version_time)
XCCDF_ITEM_SETTER_STRING(version)
XCCDF_ITEM_SETTER_STRING(version_update)
XCCDF_ITEM_SETTER_STRING(extends)
XCCDF_ITEM_SETTER_STRING(cluster_id)

#define XCCDF_SETTER_ID(T) bool xccdf_##T##_set_id(struct xccdf_##T *item, const char *newval) \
                { return xccdf_benchmark_rename_item(XITEM(item), newval); }
XCCDF_SETTER_ID(item) XCCDF_SETTER_ID(benchmark) XCCDF_SETTER_ID(profile)
XCCDF_SETTER_ID(rule) XCCDF_SETTER_ID(group) XCCDF_SETTER_ID(value) XCCDF_SETTER_ID(result)
#undef XCCDF_SETTER_ID

struct xccdf_item_iterator *xccdf_item_get_content(const struct xccdf_item *item)
{
	if (item == NULL) return NULL;
	switch (xccdf_item_get_type(item)) {
		case XCCDF_GROUP:     return xccdf_group_get_content(XGROUP(item));
		case XCCDF_BENCHMARK: return xccdf_benchmark_get_content(XBENCHMARK(item));
		default: return NULL;
	}
}

struct xccdf_status *xccdf_status_new_fill(const char *status, const char *date)
{
	struct xccdf_status *ret;
	if (!status)
		return NULL;
	ret = oscap_calloc(1, sizeof(struct xccdf_status));
	if ((ret->status = oscap_string_to_enum(XCCDF_STATUS_MAP, status)) == XCCDF_STATUS_NOT_SPECIFIED) {
		oscap_free(ret);
		return NULL;
	}
	ret->date = oscap_get_date(date);
	return ret;
}

struct xccdf_status *xccdf_status_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_status));
}

void xccdf_status_dump(struct xccdf_status *status, int depth)
{
	xccdf_print_depth(depth);
	time_t date = xccdf_status_get_date(status);
	printf("%-10s (%24.24s)\n", oscap_enum_to_string(XCCDF_STATUS_MAP, xccdf_status_get_status(status)),
	       (date ? ctime(&date) : "   date not specified   "));
}

void xccdf_status_free(struct xccdf_status *status)
{
	oscap_free(status);
}

OSCAP_ACCESSOR_SIMPLE(time_t, xccdf_status, date)
OSCAP_ACCESSOR_SIMPLE(xccdf_status_type_t, xccdf_status, status)

xccdf_status_type_t xccdf_item_get_current_status(const struct xccdf_item *item)
{
	time_t maxtime = 0;
	xccdf_status_type_t maxtype = XCCDF_STATUS_NOT_SPECIFIED;
	const struct oscap_list_item *li = item->item.statuses->first;
	struct xccdf_status *status;
	while (li) {
		status = li->data;
		if (status->date == 0 || status->date >= maxtime) {
			maxtime = status->date;
			maxtype = status->status;
		}
		li = li->next;
	}
	return maxtype;
}

struct xccdf_model *xccdf_model_clone(const struct xccdf_model *old_model)
{
	struct xccdf_model *new_model = oscap_calloc(1, sizeof(struct xccdf_model));
	new_model->system = oscap_strdup(old_model->system);

	//params maps char * to char * so we will need to oscap_strdup the items.
	new_model->params = oscap_htable_clone(old_model->params, (oscap_clone_func) oscap_strdup);
	new_model->params = NULL;
	return new_model;
}

struct xccdf_model *xccdf_model_new(void)
{
    struct xccdf_model *model = oscap_calloc(1, sizeof(struct xccdf_model));
    model->params = oscap_htable_new();
    return model;
}

struct xccdf_model *xccdf_model_new_xml(xmlTextReaderPtr reader)
{
	xccdf_element_t el = xccdf_element_get(reader);
	int depth = oscap_element_depth(reader) + 1;
	struct xccdf_model *model;

	if (el != XCCDFE_MODEL)
		return NULL;

	model = xccdf_model_new();
	model->system = xccdf_attribute_copy(reader, XCCDFA_SYSTEM);

	while (oscap_to_start_element(reader, depth)) {
		if (xccdf_element_get(reader) == XCCDFE_PARAM) {
			const char *name = xccdf_attribute_get(reader, XCCDFA_NAME);
			char *value = oscap_element_string_copy(reader);
			if (!name || !value || !oscap_htable_add(model->params, name, value))
				oscap_free(value);
		}
	}

	return model;
}

OSCAP_ACCESSOR_STRING(xccdf_model, system)

static const struct oscap_string_map XCCDF_WARNING_MAP[] = {
	{ XCCDF_WARNING_GENERAL, "general" },
	{ XCCDF_WARNING_FUNCTIONALITY, "functionality" },
	{ XCCDF_WARNING_PERFORMANCE, "performance" },
	{ XCCDF_WARNING_HARDWARE, "hardware" },
	{ XCCDF_WARNING_LEGAL, "legal" },
	{ XCCDF_WARNING_REGULATORY, "regulatory" },
	{ XCCDF_WARNING_MANAGEMENT, "management" },
	{ XCCDF_WARNING_AUDIT, "audit" },
	{ XCCDF_WARNING_DEPENDENCY, "dependency" },
	{ XCCDF_WARNING_GENERAL, NULL }
};

void xccdf_model_free(struct xccdf_model *model)
{
	if (model) {
		oscap_free(model->system);
		oscap_htable_free(model->params, oscap_free);
		oscap_free(model);
	}
}

void xccdf_cstring_dump(const char *data, int depth)
{
	xccdf_print_depth(depth);
	printf("%s\n", data);
}

struct xccdf_warning *xccdf_warning_new(void)
{
    struct xccdf_warning *w = oscap_calloc(1, sizeof(struct xccdf_warning));
    w->category = XCCDF_WARNING_GENERAL;
    return w;
}

struct xccdf_warning *xccdf_warning_new_parse(xmlTextReaderPtr reader)
{
    struct xccdf_warning *w = xccdf_warning_new();
    w->category = oscap_string_to_enum(XCCDF_WARNING_MAP, xccdf_attribute_get(reader, XCCDFA_CATEGORY));
    w->text = oscap_text_new_parse(XCCDF_TEXT_HTMLSUB, reader);
    return w;
}

void xccdf_warning_free(struct xccdf_warning * w)
{
    if (w != NULL) {
        oscap_text_free(w->text);
        oscap_free(w);
    }
}

OSCAP_ACCESSOR_SIMPLE(xccdf_warning_category_t, xccdf_warning, category)
OSCAP_ACCESSOR_TEXT(xccdf_warning, text)

struct xccdf_reference *xccdf_reference_new(void)
{
    struct xccdf_reference *ref = oscap_calloc(1, sizeof(struct xccdf_reference));
    return ref;
}

struct xccdf_reference *xccdf_reference_new_parse(xmlTextReaderPtr reader)
{
    struct xccdf_reference *ref = xccdf_reference_new();
    //if (xccdf_attribute_has(reader, XCCDFA_OVERRIDE))
    //        ref->override = oscap_string_to_enum(OSCAP_BOOL_MAP, xccdf_attribute_get(reader, XCCDFA_OVERRIDE));
    ref->href = xccdf_attribute_copy(reader, XCCDFA_HREF);
	ref->text = oscap_text_new_parse(OSCAP_TEXT_TRAITS_HTML, reader);
    //ref->content = oscap_element_string_copy(reader);
    // TODO Dublin Core
    return ref;
}

void xccdf_reference_free(struct xccdf_reference *ref)
{
    if (ref != NULL) {
        //oscap_free(ref->lang);
        oscap_free(ref->href);
		oscap_text_free(ref->text);
        //oscap_free(ref->content);
        oscap_free(ref);
    }
}

//clones the specific types of items
struct xccdf_profile_item * xccdf_profile_item_clone(const struct xccdf_profile_item * item)
{
	struct xccdf_profile_item * clone = oscap_calloc(1, sizeof(struct xccdf_profile_item));
	clone->note_tag = oscap_strdup(item->note_tag);
	clone->selects = 	oscap_list_clone(item->selects, (oscap_clone_func) xccdf_select_clone);
	clone->setvalues = oscap_list_clone(item->setvalues, (oscap_clone_func) xccdf_setvalue_clone);
	clone->refine_values = oscap_list_clone(item->refine_values, (oscap_clone_func) xccdf_refine_value_clone);
	clone->refine_rules = oscap_list_clone(item->refine_rules, (oscap_clone_func) xccdf_refine_rule_clone);
	return clone;
}

struct xccdf_benchmark_item * xccdf_benchmark_item_clone(const struct xccdf_benchmark_item * item, struct xccdf_item *parent)
{
	struct xccdf_benchmark_item * clone = oscap_calloc(1, sizeof(struct xccdf_benchmark_item));
	clone->dict = oscap_htable_clone(item->dict, (oscap_clone_func) xccdf_item_clone);	
	clone->notices = oscap_list_clone(item->notices, (oscap_clone_func) xccdf_notice_clone);
	clone->plain_texts = oscap_list_clone(item->plain_texts, (oscap_clone_func) xccdf_plain_text_clone);
	
	clone->style = oscap_strdup(item->style);
	clone->style_href = oscap_strdup(item->style_href);
	
	clone->rear_matter  = oscap_list_clone(item->rear_matter, (oscap_clone_func) oscap_text_clone);
	clone->front_matter = oscap_list_clone(item->front_matter, (oscap_clone_func) oscap_text_clone);
	
	clone->models = oscap_list_clone(item->models, (oscap_clone_func) xccdf_model_clone);
	clone->content = oscap_list_clone(item->content, (oscap_clone_func) xccdf_item_clone);
	xccdf_reparent_list(clone->content, parent);
	clone->values = oscap_list_clone(item->values, (oscap_clone_func) xccdf_value_clone);
	xccdf_reparent_list(clone->values, parent);
	clone->profiles = oscap_list_clone(item->profiles, (oscap_clone_func) xccdf_profile_clone);
	xccdf_reparent_list(clone->profiles, parent);
	clone->results = oscap_list_clone(item->results, (oscap_clone_func) xccdf_result_clone);
	xccdf_reparent_list(clone->results, parent);
	return clone;
}

struct xccdf_rule_item * xccdf_rule_item_clone(const struct xccdf_rule_item * item)
{
	struct xccdf_rule_item * clone = oscap_calloc(1, sizeof(struct xccdf_rule_item));
	clone->role = item->role;
	clone->severity = item->severity;
	clone->idents = oscap_list_clone(item->idents, (oscap_clone_func) xccdf_ident_clone );
	clone->checks = oscap_list_clone(item->checks, (oscap_clone_func) xccdf_check_clone );
	//requires is an oscap_list of char * so we should be able to use oscap_strdup for cloning
	clone->requires = oscap_list_clone(item->requires, (oscap_clone_func) oscap_strdup);
	//conflicts is also an oscap_list of char *
	clone->conflicts = oscap_list_clone(item->conflicts, (oscap_clone_func) oscap_strdup);
	clone->profile_notes = oscap_list_clone(item->profile_notes, (oscap_clone_func) xccdf_profile_note_clone);
	clone->fixes = oscap_list_clone(item->fixes, (oscap_clone_func) xccdf_fix_clone);
	clone->fixtexts = oscap_list_clone(item->fixtexts, (oscap_clone_func) xccdf_fixtext_clone);
	return clone;
}

struct xccdf_group_item * xccdf_group_item_clone(const struct xccdf_group_item * item, struct xccdf_item *parent)
{
	struct xccdf_group_item * clone = oscap_calloc(1, sizeof(struct xccdf_group_item));;
	//requires is an oscap_list of char * so we should be able to use oscap_strdup for cloning
	clone->requires = oscap_list_clone(item->requires, (oscap_clone_func) oscap_strdup);
	//conflicts is also an oscap_list of char *
	clone->conflicts = oscap_list_clone(item->conflicts, (oscap_clone_func) oscap_strdup);
	clone->values = oscap_list_clone(item->values, (oscap_clone_func) xccdf_value_clone);
	xccdf_reparent_list(clone->values, parent);
	clone->content = oscap_list_clone(item->content, (oscap_clone_func) xccdf_item_clone);
	xccdf_reparent_list(clone->content, parent);
	return clone;
}

union xccdf_value_unit xccdf_value_unit_clone_str(const union xccdf_value_unit unit)
{
	union xccdf_value_unit val;
	val.s = oscap_strdup(unit.s);
	return val;
}

union xccdf_value_unit xccdf_value_unit_clone_numeric(const union xccdf_value_unit unit)
{
	union xccdf_value_unit val;
	val.n = unit.n;
	return val;
}

union xccdf_value_unit xccdf_value_unit_clone_bool(const union xccdf_value_unit unit)
{
	union xccdf_value_unit val;
	val.b = unit.b;
	return val;
}

struct xccdf_value_val * xccdf_value_val_clone_str(const struct xccdf_value_val * val)
{
	return xccdf_value_val_clone(val, XCCDF_TYPE_STRING);
}

struct xccdf_value_val * xccdf_value_val_clone_numeric(const struct xccdf_value_val * val)
{
	return xccdf_value_val_clone(val, XCCDF_TYPE_NUMBER);
}

struct xccdf_value_val * xccdf_value_val_clone_bool(const struct xccdf_value_val * val)
{
	return xccdf_value_val_clone(val, XCCDF_TYPE_BOOLEAN);
}

struct xccdf_value_val * xccdf_value_val_clone(const struct xccdf_value_val * val, xccdf_value_type_t type)
{
	struct xccdf_value_val * clone = oscap_calloc(1, sizeof(struct xccdf_value_val));
	
	switch (type) {
	case XCCDF_TYPE_STRING:
		clone->value.s = oscap_strdup(val->value.s);
		clone->defval.s = oscap_strdup(val->defval.s);
		clone->choices = oscap_list_clone(val->choices, (oscap_clone_func) xccdf_value_unit_clone_str);
		clone->limits.s.match = oscap_strdup(val->limits.s.match);
		break;
	case XCCDF_TYPE_NUMBER:
		clone->value.n = val->value.n;
		clone->defval.n = val->defval.n;
		clone->choices = oscap_list_clone(val->choices, (oscap_clone_func) xccdf_value_unit_clone_numeric);
		clone->limits.n.lower_bound = val->limits.n.lower_bound;
		clone->limits.n.upper_bound = val->limits.n.upper_bound;
		break;
	case XCCDF_TYPE_BOOLEAN:
		clone->value.b = val->value.b;
		clone->defval.b = val->defval.b;
		clone->choices = oscap_list_clone(val->choices, (oscap_clone_func) xccdf_value_unit_clone_bool);
		break;
	default:
		assert(false);
	}
	
	clone->must_match = val->must_match;
	return clone;
}

struct xccdf_value_item * xccdf_value_item_clone(const struct xccdf_value_item * item)
{
	struct xccdf_value_item * clone = oscap_calloc(1, sizeof(struct xccdf_value_item));
	//these three are values which can be directly assigned.
	clone->type = item->type;
	clone->interface_hint = item->interface_hint;
	clone->oper = item->oper;
	
	//the rest need deep copy
	clone->selector = oscap_strdup(item->selector);
	clone->value = xccdf_value_val_clone(item->value, item->type);
	//this assumes the values list is a list of xccdf_value_val objects of each type and that the
	//lists do not have mixed types.
	switch(clone->type)
	{
		case XCCDF_TYPE_STRING:
			clone->values = oscap_htable_clone(item->values, (oscap_clone_func) xccdf_value_val_clone_str);
			break;
		case XCCDF_TYPE_NUMBER:
			clone->values = oscap_htable_clone(item->values, (oscap_clone_func) xccdf_value_val_clone_numeric);
			break;
		case XCCDF_TYPE_BOOLEAN:
			clone->values = oscap_htable_clone(item->values, (oscap_clone_func) xccdf_value_val_clone_bool);
			break;
	}
	clone->sources = oscap_list_clone(item->sources, (oscap_clone_func) oscap_strdup);
	return clone;
}

struct xccdf_identity * xccdf_identity_clone(const struct xccdf_identity * identity)
{
	struct xccdf_identity * clone = oscap_calloc(1, sizeof(struct xccdf_identity));
	clone->sub.authenticated = identity->sub.authenticated;
	clone->sub.privileged = identity->sub.privileged;
	clone->name = oscap_strdup(identity->name);
	return clone;
}

struct xccdf_target_fact * xccdf_target_fact_clone(const struct xccdf_target_fact * tf)
{
	struct xccdf_target_fact * clone = oscap_calloc(1, sizeof(struct xccdf_target_fact));
	clone->type = tf->type;
	clone->name = oscap_strdup(tf->name);
	clone->value = oscap_strdup(tf->value);
	return clone;
}

struct xccdf_override * xccdf_override_clone(const struct xccdf_override * override)
{
	struct xccdf_override * clone = oscap_calloc(1, sizeof(struct xccdf_override));
	clone->time = override->time;
	clone->authority = oscap_strdup(clone->authority);
	clone->old_result = override->old_result;
	clone->new_result = override->new_result;
	clone->remark = oscap_text_clone(override->remark);
	return clone;	
}

struct xccdf_message * xccdf_message_clone(const struct xccdf_message * message)
{
	struct xccdf_message * clone = oscap_calloc(1, sizeof(struct xccdf_message));
	clone->content = oscap_strdup(message->content);
	clone->severity = message->severity;
	return clone;
}

struct xccdf_instance * xccdf_instance_clone(const struct xccdf_instance * instance)
{
    struct xccdf_instance * clone = oscap_calloc(1, sizeof(struct xccdf_instance));
    clone->context = oscap_strdup(instance->context);
    clone->parent_context = oscap_strdup(instance->parent_context);
    clone->context = oscap_strdup(instance->content);
    return clone;
}

struct xccdf_rule_result * xccdf_rule_result_clone(const struct xccdf_rule_result * result)
{
	struct xccdf_rule_result * clone = oscap_calloc(1, sizeof(struct xccdf_rule_result));
	clone->idref = oscap_strdup(result->idref);
	clone->role = result->role;
	clone->time = result->time;
	clone->weight = result->weight;
	clone->severity = result->severity;
	clone->result = result->result;
	clone->version = oscap_strdup(result->version);
	clone->overrides = oscap_list_clone(result->overrides, (oscap_clone_func) xccdf_override_clone);
	clone->idents = oscap_list_clone(result->idents, (oscap_clone_func) xccdf_ident_clone);
	clone->messages = oscap_list_clone(result->messages, (oscap_clone_func) xccdf_message_clone);
	clone->instances = oscap_list_clone(result->instances, (oscap_clone_func) xccdf_instance_clone);
	clone->fixes = oscap_list_clone(result->fixes, (oscap_clone_func) xccdf_fix_clone);
	clone->checks = oscap_list_clone(result->checks, (oscap_clone_func) xccdf_check_clone);
	return clone;
}

struct xccdf_score * xccdf_score_clone(const struct xccdf_score * score)
{
	struct xccdf_score * clone = oscap_calloc(1, sizeof(struct xccdf_score));
	clone->maximum = score->maximum;
	clone->score = score->score;
	clone->system = oscap_strdup(score->system);
	return clone;
}

struct xccdf_result_item * xccdf_result_item_clone(const struct xccdf_result_item * item)
{
	struct xccdf_result_item * clone = oscap_calloc(1, sizeof(struct xccdf_result_item));
	clone->start_time = item->start_time;
	clone->end_time = item->end_time;
	clone->test_system = oscap_strdup(item->test_system);
	clone->benchmark_uri = oscap_strdup(item->benchmark_uri);
	clone->profile = oscap_strdup(item->profile);

	clone->identities = oscap_list_clone(item->identities, (oscap_clone_func) xccdf_identity_clone);
	clone->targets = oscap_list_clone(item->targets, (oscap_clone_func) oscap_strdup);
	clone->organizations = oscap_list_clone(item->organizations, (oscap_clone_func) oscap_strdup);
	clone->remarks = oscap_list_clone(item->remarks, (oscap_clone_func) oscap_text_clone);
	clone->target_addresses = oscap_list_clone(item->target_addresses, (oscap_clone_func) oscap_strdup);
	clone->target_facts = oscap_list_clone(item->identities, (oscap_clone_func) xccdf_target_fact_clone);
	clone->setvalues = oscap_list_clone(item->identities, (oscap_clone_func) xccdf_setvalue_clone);
	clone->rule_results = oscap_list_clone(item->identities, (oscap_clone_func) xccdf_rule_result_clone);
	clone->scores = oscap_list_clone(item->identities, (oscap_clone_func) xccdf_score_clone);
	
	return clone;
}

void xccdf_reparent_list(struct oscap_list * item_list, struct xccdf_item * parent)
{
	struct oscap_iterator *it = oscap_iterator_new(item_list);
	while(oscap_iterator_has_more(it))
	{
		struct xccdf_item * item = oscap_iterator_next(it);
		xccdf_reparent_item(item, parent);
	}
}

void xccdf_reparent_item(struct xccdf_item * item, struct xccdf_item * parent)
{
	if(item != NULL)
	{
		item->item.parent = parent;
	}
}

//OSCAP_ACCESSOR_STRING(xccdf_reference, lang)
OSCAP_ACCESSOR_STRING(xccdf_reference, href)
OSCAP_ACCESSOR_TEXT(xccdf_reference, text)
//OSCAP_ACCESSOR_STRING(xccdf_reference, content)
//OSCAP_ACCESSOR_SIMPLE(bool,        xccdf_reference, override)

const struct oscap_text_traits XCCDF_TEXT_PLAIN    = { .can_override = true };
const struct oscap_text_traits XCCDF_TEXT_HTML     = { .html = true, .can_override = true };
const struct oscap_text_traits XCCDF_TEXT_PLAINSUB = { .can_override = true, .can_substitute = true };
const struct oscap_text_traits XCCDF_TEXT_HTMLSUB  = { .html = true, .can_override = true, .can_substitute = true };
const struct oscap_text_traits XCCDF_TEXT_NOTICE   = { .html = true };
const struct oscap_text_traits XCCDF_TEXT_PROFNOTE = { .html = true, .can_substitute = true };

