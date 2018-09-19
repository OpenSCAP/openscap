/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 * 	Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <time.h>
#include <math.h>
#include <pcre.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/xpathInternals.h>

#include "item.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include "common/util.h"

/* According to `man 3 pcreapi`, the number passed in ovecsize should always
 * be a multiple of three.
 */
#define OVECTOR_LEN 30

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
	{XCCDF_LEVEL_NOT_DEFINED, NULL}
};

const struct oscap_string_map XCCDF_BOOLOP_MAP[] = {
	{XCCDF_OPERATOR_AND, "AND"},
	{XCCDF_OPERATOR_OR, "OR"},
	{XCCDF_OPERATOR_AND, "and"},
	{XCCDF_OPERATOR_OR, "or"},
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

static const struct oscap_string_map XCCDF_WARNING_MAP[] = {
        {XCCDF_WARNING_GENERAL, "general"},
        {XCCDF_WARNING_FUNCTIONALITY, "functionality"},
        {XCCDF_WARNING_PERFORMANCE, "performance"},
        {XCCDF_WARNING_HARDWARE, "hardware"},
        {XCCDF_WARNING_LEGAL, "legal"},
        {XCCDF_WARNING_REGULATORY, "regulatory"},
        {XCCDF_WARNING_MANAGEMENT, "management"},
        {XCCDF_WARNING_AUDIT, "audit"},
        {XCCDF_WARNING_DEPENDENCY, "dependency"},
	{XCCDF_WARNING_NOT_SPECIFIED, NULL}
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

	item = calloc(1, size);
	item->type = type;
	item->item.title = oscap_list_new();
	item->item.description = oscap_list_new();
	item->item.question = oscap_list_new();
	item->item.rationale = oscap_list_new();
	item->item.statuses = oscap_list_new();
	item->item.dc_statuses = oscap_list_new();
	item->item.platforms = oscap_list_new();
	item->item.warnings = oscap_list_new();
	item->item.references = oscap_list_new();
	item->item.weight = 1.0;
	item->item.flags.selected = true;
	item->item.parent = parent;
	item->item.metadata = (struct oscap_list*)oscap_stringlist_new();

	return item;
}

struct xccdf_item *xccdf_item_clone(const struct xccdf_item *old_item)
{
	struct xccdf_item *new_item = calloc(1, sizeof(struct xccdf_item));

    xccdf_item_base_clone(&new_item->item, &(old_item->item));
	new_item->type = old_item->type;

	switch (new_item->type) {
	case XCCDF_BENCHMARK:
        xccdf_benchmark_item_clone(new_item, XBENCHMARK(old_item));
		break;
	case XCCDF_RULE:
	    xccdf_rule_item_clone(&new_item->sub.rule, &old_item->sub.rule);
		break;
	case XCCDF_GROUP:
	    xccdf_group_item_clone(new_item, &old_item->sub.group);
		break;
	case XCCDF_VALUE:
	    xccdf_value_item_clone(&new_item->sub.value, &old_item->sub.value);
		break;
	case XCCDF_RESULT:
	    xccdf_result_item_clone(&new_item->sub.result, &old_item->sub.result);
		break;
	case XCCDF_PROFILE:
	    xccdf_profile_item_clone(&new_item->sub.profile, &old_item->sub.profile);
		break;
	default:
		//dont initialize the sub item.
		break;
	}
	
	return new_item;
}

/* Performs a deep copy of xccdf_item_base and returns a pointer to that copy */
void xccdf_item_base_clone(struct xccdf_item_base *new_base, const struct xccdf_item_base *old_base)
{
	new_base->id = oscap_strdup(old_base->id);
	new_base->cluster_id = oscap_strdup(old_base->cluster_id);
	new_base->extends = oscap_strdup(old_base->extends);

	new_base->version = oscap_strdup(old_base->version);
	new_base->version_update = oscap_strdup(old_base->version_update);
	new_base->version_time = oscap_strdup(old_base->version_time);


	new_base->weight = old_base->weight;
	new_base->parent = NULL;

	new_base->title = oscap_list_clone(old_base->title, (oscap_clone_func) oscap_text_clone);
	new_base->description = oscap_list_clone(old_base->description, (oscap_clone_func) oscap_text_clone);
	new_base->question = oscap_list_clone(old_base->question, (oscap_clone_func) oscap_text_clone);
	new_base->rationale = oscap_list_clone(old_base->rationale, (oscap_clone_func) oscap_text_clone);

	new_base->warnings = oscap_list_clone(old_base->warnings, (oscap_clone_func) xccdf_warning_clone);
	new_base->statuses = oscap_list_clone(old_base->statuses, (oscap_clone_func) xccdf_status_clone);
	new_base->dc_statuses = oscap_list_clone(old_base->dc_statuses, (oscap_clone_func) oscap_reference_clone);
	new_base->references = oscap_list_clone(old_base->references, (oscap_clone_func) oscap_reference_clone);
	new_base->platforms = oscap_list_clone(old_base->platforms, (oscap_clone_func) oscap_strdup);

	/* Handling flags */
	new_base->flags = old_base->flags;
	new_base->defined_flags = old_base->defined_flags;

	new_base->metadata = (struct oscap_list*)oscap_stringlist_clone((struct oscap_stringlist*)(old_base->metadata));
}

/* Performs a deep copy of xccdf_status and returns a pointer to that copy */
struct xccdf_status *xccdf_status_clone(const struct xccdf_status *old_status)
{
	struct xccdf_status *new_status = calloc(1, sizeof(struct xccdf_status));
	new_status->status = old_status->status;
	new_status->date = old_status->date;
	return new_status;
}

/* Performs a deep copy of xccdf_warning and returns a pointer to that copy */
struct xccdf_warning *xccdf_warning_clone(const struct xccdf_warning *old_warning)
{
	struct xccdf_warning *new_warning = calloc(1, sizeof(struct xccdf_warning));
	new_warning->text = oscap_text_clone(old_warning->text);
	new_warning->category = old_warning->category;
	return new_warning;
}


void xccdf_item_release(struct xccdf_item *item)
{
	if (item) {
		oscap_list_free(item->item.statuses, (oscap_destruct_func) xccdf_status_free);
		oscap_list_free(item->item.dc_statuses, (oscap_destruct_func) oscap_reference_free);
		oscap_list_free(item->item.platforms, free);
		oscap_list_free(item->item.title, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(item->item.description, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(item->item.rationale, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(item->item.question, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(item->item.warnings, (oscap_destruct_func) xccdf_warning_free);
		oscap_list_free(item->item.references, (oscap_destruct_func) oscap_reference_free);
		if (item->type != XCCDF_BENCHMARK) xccdf_benchmark_unregister_item(item);
		free(item->item.id);
		free(item->item.cluster_id);
		free(item->item.version_time);
		free(item->item.version_update);
		free(item->item.version);
		free(item->item.extends);
		oscap_stringlist_free((struct oscap_stringlist*)(item->item.metadata));

		free(item);
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
		printf("status (cur = ");
		const struct xccdf_status *status = xccdf_item_get_current_status(item);
		if (status != NULL)
			printf("%d)", xccdf_status_get_status(status));
		else
			printf("(NULL))");
		oscap_list_dump(item->item.statuses, xccdf_status_dump, depth + 1);
	}
}

void xccdf_texts_to_dom(struct oscap_text_iterator *texts, xmlNode *parent, const char *elname)
{
	OSCAP_FOR(oscap_text, text, texts)
		oscap_text_to_dom(text, parent, elname);
}

xmlNode *xccdf_item_to_dom(struct xccdf_item *item, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info *version_info)
{
	/*
	We have 2 special cases here, either we have a parent node or we don't.
	In case we have a parent node we can look for XCCDF namespace or create
	it there. But if we don't we have no node to create the namespace in!
	In this case we create a node with no namespace, then create the namespace
	in it and only then we set the node to the new namespace. This avoids
	undefined / undocumented behavior that we relied on previously.
	*/

	xmlNs *ns_xccdf = NULL;
	xmlNode *item_node = NULL;
	if (parent == NULL) {
		item_node = xmlNewNode(NULL, BAD_CAST "Item");
		// this creates the namespace and item_node carries it
		ns_xccdf = lookup_xccdf_ns(doc, item_node, version_info);
		xmlSetNs(item_node, ns_xccdf);
	}
	else {
		ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);
		item_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "Item", NULL);
	}

	/* Handle generic item attributes */
	const char *id = xccdf_item_get_id(item);
	xmlNewProp(item_node, BAD_CAST "id", BAD_CAST id);

	const char *cluster_id = xccdf_item_get_cluster_id(item);
	if (cluster_id)
		xmlNewProp(item_node, BAD_CAST "cluster-id", BAD_CAST cluster_id);

	if (item->item.defined_flags.hidden) {
		if (xccdf_item_get_hidden(item))
			xmlNewProp(item_node, BAD_CAST "hidden", BAD_CAST "true");
		else
			xmlNewProp(item_node, BAD_CAST "hidden", BAD_CAST "false");
	}

	if (xccdf_item_get_prohibit_changes(item))
		xmlNewProp(item_node, BAD_CAST "prohibitChanges", BAD_CAST "true");

	if (xccdf_item_get_abstract(item))
		xmlNewProp(item_node, BAD_CAST "abstract", BAD_CAST "true");

	struct xccdf_status_iterator *statuses = xccdf_item_get_statuses(item);
	while (xccdf_status_iterator_has_more(statuses)) {
		struct xccdf_status *status = xccdf_status_iterator_next(statuses);
		xccdf_status_to_dom(status, doc, item_node, version_info);
	}
	xccdf_status_iterator_free(statuses);

	struct oscap_reference_iterator *dc_statuses = xccdf_item_get_dc_statuses(item);
	while (oscap_reference_iterator_has_more(dc_statuses)) {
		struct oscap_reference *ref = oscap_reference_iterator_next(dc_statuses);
		oscap_reference_to_dom(ref, item_node, doc, "dc-status");
	}
	oscap_reference_iterator_free(dc_statuses);

	/* version and attributes */
	const char *version = xccdf_item_get_version(item);
	if ((xccdf_item_get_type(item) != XCCDF_BENCHMARK && xccdf_item_get_type(item) != XCCDF_RESULT) && version) {
		xmlNode* version_node = xmlNewTextChild(item_node, ns_xccdf, BAD_CAST "version", BAD_CAST version);

		const char *version_update = xccdf_item_get_version_update(item);
		if (version_update)
			xmlNewProp(version_node, BAD_CAST "update", BAD_CAST version_update);

		const char *version_time = xccdf_item_get_version_time(item);
		if (version_time)
			xmlNewProp(version_node, BAD_CAST "time", BAD_CAST version_time);
	}

	/* Handle generic item child nodes */
	xccdf_texts_to_dom(xccdf_item_get_title(item), item_node, "title");
	xccdf_texts_to_dom(xccdf_item_get_description(item), item_node, "description");

	/* Handle some type specific children */
	if (xccdf_item_get_type(item)==XCCDF_BENCHMARK) {
		struct xccdf_benchmark *benchmark = XBENCHMARK(item);

		struct xccdf_notice_iterator *notices = xccdf_benchmark_get_notices(benchmark);
		while (xccdf_notice_iterator_has_more(notices)) {
			struct xccdf_notice *notice = xccdf_notice_iterator_next(notices);
			xmlNode *notice_node = oscap_text_to_dom(xccdf_notice_get_text(notice), item_node, "notice");

			const char * notice_id = xccdf_notice_get_id(notice);
			if (notice_id)
				xmlNewProp(notice_node, BAD_CAST "id", BAD_CAST notice_id);
		}
		xccdf_notice_iterator_free(notices);

		xccdf_texts_to_dom(xccdf_benchmark_get_front_matter(benchmark), item_node, "front-matter");
		xccdf_texts_to_dom(xccdf_benchmark_get_rear_matter(benchmark), item_node, "rear-matter");
	}

	/* Handle generic item child nodes */
	struct xccdf_warning_iterator *warnings = xccdf_item_get_warnings(item);
	while (xccdf_warning_iterator_has_more(warnings)) {
		struct xccdf_warning *warning = xccdf_warning_iterator_next(warnings);
		xccdf_warning_to_dom(warning, doc, item_node);
	}
	xccdf_warning_iterator_free(warnings);

	xccdf_texts_to_dom(xccdf_item_get_question(item), item_node, "question");

	struct oscap_reference_iterator *references = xccdf_item_get_references(item);
	while (oscap_reference_iterator_has_more(references)) {
		struct oscap_reference *ref = oscap_reference_iterator_next(references);
		oscap_reference_to_dom(ref, item_node, doc, "reference");
	}
    oscap_reference_iterator_free(references);

	if (xccdf_item_get_type(item)!=XCCDF_BENCHMARK) {
		struct oscap_string_iterator* metadata = xccdf_item_get_metadata(item);
		while (oscap_string_iterator_has_more(metadata))
		{
			const char* meta = oscap_string_iterator_next(metadata);
			xmlNode *m = oscap_xmlstr_to_dom(item_node, "metadata", meta);
			xmlSetNs(m, ns_xccdf);
		}
		oscap_string_iterator_free(metadata);
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
			xccdf_profile_to_dom(XPROFILE(item), item_node, doc, parent, version_info);
			break;
		case XCCDF_RESULT:
			xmlNodeSetName(item_node,BAD_CAST "TestResult");
			if (parent) xccdf_result_to_dom(XRESULT(item), item_node, doc, parent, false);
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

xmlNode *xccdf_profile_note_to_dom(struct xccdf_profile_note *note, xmlDoc *doc, xmlNode *parent)
{
	xmlNode *note_node = oscap_text_to_dom(xccdf_profile_note_get_text(note), parent, "profile-note");
	xmlNewProp(note_node, BAD_CAST "tag", BAD_CAST xccdf_profile_note_get_reftag(note));

	return note_node;
}

xmlNode *xccdf_warning_to_dom(struct xccdf_warning *warning, xmlDoc *doc, xmlNode *parent)
{
	xmlNode *warning_node = NULL;
	xccdf_warning_category_t category = xccdf_warning_get_category(warning);

	warning_node = oscap_text_to_dom(xccdf_warning_get_text(warning), parent, "warning");
	if (category != XCCDF_WARNING_NOT_SPECIFIED)
	    xmlNewProp(warning_node, BAD_CAST "category", BAD_CAST XCCDF_WARNING_MAP[category - 1].string);

	return warning_node;
}

xmlNode *xccdf_status_to_dom(struct xccdf_status *status, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info)
{
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);

	xmlNode *status_node = NULL;
	xccdf_status_type_t type = xccdf_status_get_status(status);
	if (type != XCCDF_STATUS_NOT_SPECIFIED)
		status_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "status",
							BAD_CAST XCCDF_STATUS_MAP[type - 1].string);

	time_t date_time = xccdf_status_get_date(status);
	if (date_time) {
		struct tm *date = localtime(&date_time);
		char date_str[] = "YYYY-DD-MM";
		snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", date->tm_year + 1900, date->tm_mon + 1, date->tm_mday);
		xmlNewProp(status_node, BAD_CAST "date", BAD_CAST date_str);
	}

	return status_node;
}

xmlNode *xccdf_fixtext_to_dom(struct xccdf_fixtext *fixtext, xmlDoc *doc, xmlNode *parent)
{
	xmlNode *fixtext_node = oscap_text_to_dom(xccdf_fixtext_get_text(fixtext), parent, "fixtext");
	
	if (xccdf_fixtext_get_reboot(fixtext))
		xmlNewProp(fixtext_node, BAD_CAST "reboot", BAD_CAST "true");

	const char *fixref = xccdf_fixtext_get_fixref(fixtext);
    if (fixref)
	    xmlNewProp(fixtext_node, BAD_CAST "fixref", BAD_CAST fixref);

	xccdf_level_t complexity = xccdf_fixtext_get_complexity(fixtext);
        if (complexity != XCCDF_LEVEL_NOT_DEFINED)
	        xmlNewProp(fixtext_node, BAD_CAST "complexity", BAD_CAST XCCDF_LEVEL_MAP[complexity-1].string);

	xccdf_level_t disruption = xccdf_fixtext_get_disruption(fixtext);
        if (disruption != XCCDF_LEVEL_NOT_DEFINED)
	        xmlNewProp(fixtext_node, BAD_CAST "disruption", BAD_CAST XCCDF_LEVEL_MAP[disruption-1].string);

	xccdf_strategy_t strategy = xccdf_fixtext_get_strategy(fixtext);
        if (strategy != XCCDF_STRATEGY_UNKNOWN)
	        xmlNewProp(fixtext_node, BAD_CAST "strategy", BAD_CAST XCCDF_STRATEGY_MAP[strategy].string);

	return fixtext_node;
}

xmlNode *xccdf_fix_to_dom(struct xccdf_fix *fix, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info)
{
	xmlNode *fix_node = oscap_xmlstr_to_dom(parent, "fix", xccdf_fix_get_content(fix));
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);
	xmlSetNs(fix_node, ns_xccdf);

	const char *id = xccdf_fix_get_id(fix);
	if (id != NULL) xmlNewProp(fix_node, BAD_CAST "id", BAD_CAST id);

	const char *sys = xccdf_fix_get_system(fix);
	if (sys != NULL) xmlNewProp(fix_node, BAD_CAST "system", BAD_CAST sys);

	const char *platform = xccdf_fix_get_platform(fix);
	if (platform != NULL) xmlNewProp(fix_node, BAD_CAST "platform", BAD_CAST platform);

	if (xccdf_fix_get_reboot(fix))
		xmlNewProp(fix_node, BAD_CAST "reboot", BAD_CAST "true");

	xccdf_level_t complexity = xccdf_fix_get_complexity(fix);
        if (complexity != XCCDF_LEVEL_NOT_DEFINED)
	    xmlNewProp(fix_node, BAD_CAST "complexity", BAD_CAST XCCDF_LEVEL_MAP[complexity-1].string);

	xccdf_level_t disruption = xccdf_fix_get_disruption(fix);
        if (disruption != XCCDF_LEVEL_NOT_DEFINED)
	        xmlNewProp(fix_node, BAD_CAST "disruption", BAD_CAST XCCDF_LEVEL_MAP[disruption-1].string);

	xccdf_strategy_t strategy = xccdf_fix_get_strategy(fix);
        if (strategy != XCCDF_STRATEGY_UNKNOWN)
	        xmlNewProp(fix_node, BAD_CAST "strategy", BAD_CAST XCCDF_STRATEGY_MAP[strategy-1].string);

        // Sub element is used to store XCCDF value substitutions, not a content
	//xmlNewTextChild(fix_node, ns_xccdf, BAD_CAST "sub", BAD_CAST content);

	// This is in the XCCDF Spec, but not implemented in OpenSCAP
	//const char *instance = xccdf_fix_get_instance(fix);
	//xmlNewTextChild(fix_node, ns_xccdf, BAD_CAST "instance", BAD_CAST instance);

	return fix_node;
}

xmlNode *xccdf_ident_to_dom(struct xccdf_ident *ident, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info)
{
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);

	const char *id = xccdf_ident_get_id(ident);
	xmlNode *ident_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "ident", BAD_CAST id);

	const char *sys = xccdf_ident_get_system(ident);
	xmlNewProp(ident_node, BAD_CAST "system", BAD_CAST sys);

	return ident_node;
}

xmlNode *xccdf_check_to_dom(struct xccdf_check *check, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info)
{
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);

	xmlNode *check_node = NULL;
	if (xccdf_check_get_complex(check))
		check_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "complex-check", NULL);
	else {
		check_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "check", NULL);
		const char *sys = xccdf_check_get_system(check);
		xmlNewProp(check_node, BAD_CAST "system", BAD_CAST sys);
	}


	const char *id = xccdf_check_get_id(check);
	if (id)
		xmlNewProp(check_node, BAD_CAST "id", BAD_CAST id);

	const char *selector = xccdf_check_get_selector(check);
	if (selector)
		xmlNewProp(check_node, BAD_CAST "selector", BAD_CAST selector);
	if (!xccdf_check_get_complex(check) && (check->flags.def_multicheck || xccdf_check_get_multicheck(check))) {
		xmlNewProp(check_node, BAD_CAST "multi-check",
			BAD_CAST (xccdf_check_get_multicheck(check) ? "true" : "false"));
	}
	if (check->flags.def_negate || xccdf_check_get_negate(check))
		xmlNewProp(check_node, BAD_CAST "negate",
			BAD_CAST (xccdf_check_get_negate(check) ? "true" : "false"));
	if (xccdf_check_get_complex(check))
		xmlNewProp(check_node, BAD_CAST "operator",
			BAD_CAST oscap_enum_to_string(XCCDF_BOOLOP_MAP, xccdf_check_get_oper(check)));

	/* Handle complex checks */
	struct xccdf_check_iterator *checks = xccdf_check_get_children(check);
	while (xccdf_check_iterator_has_more(checks)) {
		struct xccdf_check *new_check = xccdf_check_iterator_next(checks);
		xccdf_check_to_dom(new_check, doc, check_node, version_info);
	}
	xccdf_check_iterator_free(checks);

	struct xccdf_check_import_iterator *imports = xccdf_check_get_imports(check);
	while (xccdf_check_import_iterator_has_more(imports)) {
		struct xccdf_check_import *import = xccdf_check_import_iterator_next(imports);
		const char *name = xccdf_check_import_get_name(import);
		const char *xpath = xccdf_check_import_get_xpath(import);
		const char *content = xccdf_check_import_get_content(import);
		xmlNode *import_node = xmlNewChild(check_node, ns_xccdf, BAD_CAST "check-import", BAD_CAST content);
		xmlNewProp(import_node, BAD_CAST "import-name", BAD_CAST name);
		if (xpath)
			xmlNewProp(import_node, BAD_CAST "import-xpath", BAD_CAST xpath);
	}
	xccdf_check_import_iterator_free(imports);

	struct xccdf_check_export_iterator *exports = xccdf_check_get_exports(check);
	while (xccdf_check_export_iterator_has_more(exports)) {
		struct xccdf_check_export *export = xccdf_check_export_iterator_next(exports);
		const char *name = xccdf_check_export_get_name(export);
		const char *value= xccdf_check_export_get_value(export);
		xmlNode *export_node = xmlNewTextChild(check_node, ns_xccdf, BAD_CAST "check-export", NULL);
		xmlNewProp(export_node, BAD_CAST "export-name", BAD_CAST name);
		xmlNewProp(export_node, BAD_CAST "value-id", BAD_CAST value);
	}
	xccdf_check_export_iterator_free(exports);

	struct xccdf_check_content_ref_iterator *refs = xccdf_check_get_content_refs(check);
	while (xccdf_check_content_ref_iterator_has_more(refs)) {
		struct xccdf_check_content_ref *ref = xccdf_check_content_ref_iterator_next(refs);
		xmlNode *ref_node = xmlNewTextChild(check_node, ns_xccdf, BAD_CAST "check-content-ref", NULL);

		const char *name = xccdf_check_content_ref_get_name(ref);
		if (name != NULL)
			xmlNewProp(ref_node, BAD_CAST "name", BAD_CAST name);

		const char *href = xccdf_check_content_ref_get_href(ref);
		xmlNewProp(ref_node, BAD_CAST "href", BAD_CAST href);
	}
	xccdf_check_content_ref_iterator_free(refs);

	const char *content = xccdf_check_get_content(check);
	if (content) {
		oscap_xmlstr_to_dom(check_node, "check-content", content);
	}

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
			xccdf_benchmark_register_item(xccdf_item_get_benchmark(item), item);
	}
	return item->item.id != NULL;
}

void xccdf_item_add_applicable_platform(struct xccdf_item *item, xmlTextReaderPtr reader)
{
	char *platform_idref = xccdf_attribute_copy(reader, XCCDFA_IDREF);

	/* Official Windows 7 CPE according to National Vulnerability Database
	 * CPE Dictionary as of 2018-08-29 is 'cpe:/o:microsoft:windows_7'.
	 * However, content exported from Microsoft Security Compliance Manager
	 * as of version 4.0.0.1 in CAB archive using 'Export in SCAP 1.0' is
	 * 'cpe:/o:microsoft:windows7'. If this pattern is matched, we will add
	 * an underscore to workaround the situation that this XCCDF benchmark is
	 * not applicable.
	 */
	const char *pcreerror = NULL;
	int erroffset = 0;
	pcre *regex = pcre_compile("^(cpe:/o:microsoft:windows)(7.*)", 0, &pcreerror, &erroffset, NULL);
	int ovector[OVECTOR_LEN];
	int rc = pcre_exec(regex, NULL, platform_idref, strlen(platform_idref), 0, 0, ovector, OVECTOR_LEN);
	/* 1 pattern + 2 groups = 3 */
	if (rc == 3) {
		size_t match_len = ovector[1] - ovector[0];
		/* match_len + 1 underscore + 1 zero byte */
		char *alternate_platform_idref = malloc(match_len + 1 + 1);
		const int first_group_start = ovector[2];
		const int first_group_end = ovector[3];
		size_t first_group_len = first_group_end - first_group_start;
		const int second_group_start = ovector[4];
		const int second_group_end = ovector[5];
		size_t second_group_len = second_group_end - second_group_start;
		char *aptr = alternate_platform_idref;
		strncpy(aptr, platform_idref + first_group_start, first_group_len);
		aptr += first_group_len;
		*aptr = '_';
		aptr++;
		strncpy(aptr, platform_idref + second_group_start, second_group_len);
		aptr += second_group_len;
		*aptr = '\0';
		oscap_list_add(item->item.platforms, alternate_platform_idref);
	}

	oscap_list_add(item->item.platforms, platform_idref);
}

bool xccdf_item_process_element(struct xccdf_item * item, xmlTextReaderPtr reader)
{
	xccdf_element_t el = xccdf_element_get(reader);

	switch (el) {
	case XCCDFE_TITLE:
        oscap_list_add(item->item.title, oscap_text_new_parse(XCCDF_TEXT_PLAINSUB, reader));
		return true;
	case XCCDFE_DESCRIPTION:
        oscap_list_add(item->item.description, oscap_text_new_parse(XCCDF_TEXT_HTMLSUB, reader));
		return true;
	case XCCDFE_WARNING:
        oscap_list_add(item->item.warnings, xccdf_warning_new_parse(reader));
		return true;
	case XCCDFE_REFERENCE:
        oscap_list_add(item->item.references, oscap_reference_new_parse(reader));
		return true;
	case XCCDFE_STATUS:{
        const char *date = xccdf_attribute_get(reader, XCCDFA_DATE);
        char *str = oscap_element_string_copy(reader);
        struct xccdf_status *status = xccdf_status_new_fill(str, date);
        free(str);
        if (status) {
            oscap_list_add(item->item.statuses, status);
            return true;
        }
        break;
    }
	case XCCDFE_DC_STATUS:
        oscap_list_add(item->item.dc_statuses, oscap_reference_new_parse(reader));
		return true;
	case XCCDFE_VERSION: {
	        xmlNode *ver = xmlTextReaderExpand(reader);
		/* optional attributes */
		item->item.version_time = (char*) xmlGetProp(ver, BAD_CAST "time");
		item->item.version_update = (char*) xmlGetProp(ver, BAD_CAST "update");
		/* content */
		item->item.version = (char *) xmlNodeGetContent(ver);
		if (oscap_streq(item->item.version, "")) {
			free(item->item.version);
			item->item.version = NULL;
		}
		return true;
    }
	case XCCDFE_RATIONALE:
        oscap_list_add(item->item.rationale, oscap_text_new_parse(XCCDF_TEXT_HTMLSUB, reader));
		return true;
        case XCCDFE_PLATFORM:
		xccdf_item_add_applicable_platform(item, reader);
                return true;
	case XCCDFE_QUESTION:
        oscap_list_add(item->item.question, oscap_text_new_parse(XCCDF_TEXT_PLAIN, reader));
		return true;
	case XCCDFE_METADATA: {
		char* xml = oscap_get_xml(reader);
		xccdf_item_add_metadata(item, xml);
		free(xml);
		return true;
	}
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

const struct xccdf_version_info* xccdf_item_get_schema_version(struct xccdf_item* item)
{
	struct xccdf_benchmark* top_benchmark = xccdf_item_get_benchmark(item);
	if (top_benchmark == NULL) {
		if (xccdf_item_get_type(item) == XCCDF_RESULT) {
			// TestResult is special item, it may not have parent benchmark
			return xccdf_result_get_schema_version(XRESULT(item));
		} else {
			return NULL;
		}
	}
	return xccdf_benchmark_get_schema_version(top_benchmark);
}

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
XCCDF_ITEM_GETTER(const char *, version_update)
XCCDF_ITEM_GETTER(const char *, version_time)
XCCDF_ITEM_GETTER(float, weight)
XCCDF_ITEM_GETTER(struct xccdf_item *, parent)
XCCDF_ITEM_GETTER(const char *, extends)
XCCDF_FLAG_GETTER(resolved)
XCCDF_FLAG_GETTER(hidden)
XCCDF_FLAG_GETTER(selected)
XCCDF_ITEM_SIGETTER(metadata);
XCCDF_ITEM_ADDER_ONE(item, metadata, metadata, const char*, oscap_strdup);
XCCDF_ITEM_ADDER_ONE(benchmark, metadata, metadata, const char*, oscap_strdup);
XCCDF_ITEM_ADDER_ONE(profile, metadata, metadata, const char*, oscap_strdup);
XCCDF_ITEM_ADDER_ONE(group, metadata, metadata, const char*, oscap_strdup);
XCCDF_ITEM_ADDER_ONE(rule, metadata, metadata, const char*, oscap_strdup);
XCCDF_ITEM_ADDER_ONE(value, metadata, metadata, const char*, oscap_strdup);
XCCDF_ITEM_ADDER_ONE(result, metadata, metadata, const char*, oscap_strdup);
XCCDF_FLAG_GETTER(multiple)
XCCDF_FLAG_GETTER(prohibit_changes)
XCCDF_FLAG_GETTER(abstract)
XCCDF_FLAG_GETTER(interactive)
XCCDF_ITEM_SIGETTER(platforms)
XCCDF_ITEM_ADDER_STRING(platform, platforms)
XCCDF_ITEM_IGETTER(warning, warnings)
XCCDF_ITEM_IGETTER(status, statuses)
XCCDF_ITEM_ADDER(struct oscap_reference *, reference, references)
XCCDF_ITEM_ADDER(struct xccdf_warning *, warning, warnings)
XCCDF_ITEM_ADDER(struct xccdf_status *, status, statuses)
XCCDF_ITEM_ADDER(struct oscap_reference *, dc_status, dc_statuses)
XCCDF_ITERATOR_GEN_S(item) XCCDF_ITERATOR_GEN_S(status)
OSCAP_ITERATOR_GEN(xccdf_warning)
OSCAP_ITERATOR_REMOVE_F(xccdf_warning)

XCCDF_ITEM_SETTER_SIMPLE(xccdf_numeric, weight)
XCCDF_ITEM_SETTER_STRING(version_time)
XCCDF_ITEM_SETTER_STRING(version)
XCCDF_ITEM_SETTER_STRING(version_update)
XCCDF_ITEM_SETTER_STRING(extends)
XCCDF_ITEM_SETTER_STRING(cluster_id)

#define XCCDF_SETTER_ID(T) bool xccdf_##T##_set_id(struct xccdf_##T *item, const char *newval) \
                { return xccdf_benchmark_rename_item(XITEM(item), newval); }
XCCDF_SETTER_ID(item) XCCDF_SETTER_ID(benchmark) XCCDF_SETTER_ID(profile)
XCCDF_SETTER_ID(rule) XCCDF_SETTER_ID(group) XCCDF_SETTER_ID(value) XCCDF_SETTER_ID(result)
#undef XCCDF_SETTER_ID

struct oscap_reference_iterator *xccdf_item_get_references(const struct xccdf_item *item) { return oscap_iterator_new(item->item.references); }
struct oscap_reference_iterator *xccdf_item_get_dc_statuses(const struct xccdf_item *item) { return oscap_iterator_new(item->item.dc_statuses); }
struct oscap_reference_iterator *xccdf_benchmark_get_references(const struct xccdf_benchmark *item) { return oscap_iterator_new(XITEM(item)->item.references); }
struct oscap_reference_iterator *xccdf_benchmark_get_dc_statuses(const struct xccdf_benchmark *item) { return oscap_iterator_new(XITEM(item)->item.dc_statuses); }
struct oscap_reference_iterator *xccdf_value_get_references(const struct xccdf_value *item) { return oscap_iterator_new(XITEM(item)->item.references); }
struct oscap_reference_iterator *xccdf_value_get_dc_statuses(const struct xccdf_value *item) { return oscap_iterator_new(XITEM(item)->item.dc_statuses); }
struct oscap_reference_iterator *xccdf_group_get_references(const struct xccdf_group *item) { return oscap_iterator_new(XITEM(item)->item.references); }
struct oscap_reference_iterator *xccdf_group_get_dc_statuses(const struct xccdf_group *item) { return oscap_iterator_new(XITEM(item)->item.dc_statuses); }
struct oscap_reference_iterator *xccdf_rule_get_references(const struct xccdf_rule *item) { return oscap_iterator_new(XITEM(item)->item.references); }
struct oscap_reference_iterator *xccdf_rule_get_dc_statuses(const struct xccdf_rule *item) { return oscap_iterator_new(XITEM(item)->item.dc_statuses); }
struct oscap_reference_iterator *xccdf_profile_get_references(const struct xccdf_profile *item) { return oscap_iterator_new(XITEM(item)->item.references); }
struct oscap_reference_iterator *xccdf_profile_get_dc_statuses(const struct xccdf_profile *item) { return oscap_iterator_new(XITEM(item)->item.dc_statuses); }

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
	ret = calloc(1, sizeof(struct xccdf_status));
	if ((ret->status = oscap_string_to_enum(XCCDF_STATUS_MAP, status)) == XCCDF_STATUS_NOT_SPECIFIED) {
		free(ret);
		return NULL;
	}
	ret->date = oscap_get_date(date);
	return ret;
}

struct xccdf_status *xccdf_status_new(void)
{
    return calloc(1, sizeof(struct xccdf_status));
}

const char *xccdf_status_type_to_text(xccdf_status_type_t id)
{
	return oscap_enum_to_string(XCCDF_STATUS_MAP, id);
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
	free(status);
}

OSCAP_ACCESSOR_SIMPLE(time_t, xccdf_status, date)
OSCAP_ACCESSOR_SIMPLE(xccdf_status_type_t, xccdf_status, status)

struct xccdf_status * xccdf_item_get_current_status(const struct xccdf_item *item)
{
	time_t maxtime = 0;
	struct xccdf_status *max_status = NULL;
	struct xccdf_status *status;
	const struct oscap_list_item *li = item->item.statuses->first;
	while (li) {
		status = li->data;
		if (status->date == 0 || status->date >= maxtime) {
			maxtime = status->date;
			max_status = status;
		}
		li = li->next;
	}
	return max_status;
}

struct xccdf_model *xccdf_model_clone(const struct xccdf_model *old_model)
{
	struct xccdf_model *new_model = calloc(1, sizeof(struct xccdf_model));
	new_model->system = oscap_strdup(old_model->system);

	//params maps char * to char * so we will need to oscap_strdup the items.
	new_model->params = oscap_htable_clone(old_model->params, (oscap_clone_func) oscap_strdup);
	//new_model->params = NULL;
	return new_model;
}

struct xccdf_model *xccdf_model_new(void)
{
    struct xccdf_model *model = calloc(1, sizeof(struct xccdf_model));
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
				free(value);
		}
	}

	return model;
}

OSCAP_ACCESSOR_STRING(xccdf_model, system)

void xccdf_model_free(struct xccdf_model *model)
{
	if (model) {
		free(model->system);
		oscap_htable_free(model->params, free);
		free(model);
	}
}

void xccdf_cstring_dump(const char *data, int depth)
{
	xccdf_print_depth(depth);
	printf("%s\n", data);
}

struct xccdf_warning *xccdf_warning_new(void)
{
    struct xccdf_warning *w = calloc(1, sizeof(struct xccdf_warning));
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
        free(w);
    }
}

OSCAP_ACCESSOR_SIMPLE(xccdf_warning_category_t, xccdf_warning, category)
OSCAP_ACCESSOR_TEXT(xccdf_warning, text)

//clones the specific types of items
void xccdf_profile_item_clone(struct xccdf_profile_item *clone, const struct xccdf_profile_item * item)
{
	clone->note_tag = oscap_strdup(item->note_tag);
	clone->selects = 	oscap_list_clone(item->selects, (oscap_clone_func) xccdf_select_clone);
	clone->setvalues = oscap_list_clone(item->setvalues, (oscap_clone_func) xccdf_setvalue_clone);
	clone->refine_values = oscap_list_clone(item->refine_values, (oscap_clone_func) xccdf_refine_value_clone);
	clone->refine_rules = oscap_list_clone(item->refine_rules, (oscap_clone_func) xccdf_refine_rule_clone);
	clone->tailoring = item->tailoring;
}

struct xccdf_benchmark_item * xccdf_benchmark_item_clone(struct xccdf_item *parent, const struct xccdf_benchmark * bench)
{
    struct xccdf_benchmark_item *item = &XITEM(bench)->sub.benchmark;
    struct xccdf_benchmark_item *clone = &parent->sub.benchmark;

    clone->schema_version = item->schema_version;

	clone->items_dict = oscap_htable_new();
	clone->profiles_dict = oscap_htable_new();
	clone->results_dict = oscap_htable_new();
	clone->notices = oscap_list_clone(item->notices, (oscap_clone_func) xccdf_notice_clone);
	clone->plain_texts = oscap_list_clone(item->plain_texts, (oscap_clone_func) xccdf_plain_text_clone);
	
	clone->style = oscap_strdup(item->style);
	clone->style_href = oscap_strdup(item->style_href);
	clone->lang = oscap_strdup(item->lang);
	
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

void xccdf_rule_item_clone(struct xccdf_rule_item *clone, const struct xccdf_rule_item * item)
{
	clone->role = item->role;
	clone->severity = item->severity;
	clone->idents = oscap_list_clone(item->idents, (oscap_clone_func) xccdf_ident_clone );
	clone->checks = oscap_list_clone(item->checks, (oscap_clone_func) xccdf_check_clone );
	//requires is an oscap_list of oscap_lists of char *
	clone->requires = oscap_list_clone(item->requires, (oscap_clone_func) oscap_stringlist_clone);
	//conflicts is an oscap_list of char *
	clone->conflicts = oscap_list_clone(item->conflicts, (oscap_clone_func) oscap_strdup);
	clone->profile_notes = oscap_list_clone(item->profile_notes, (oscap_clone_func) xccdf_profile_note_clone);
	clone->fixes = oscap_list_clone(item->fixes, (oscap_clone_func) xccdf_fix_clone);
	clone->fixtexts = oscap_list_clone(item->fixtexts, (oscap_clone_func) xccdf_fixtext_clone);
}

void xccdf_group_item_clone(struct xccdf_item *parent, const struct xccdf_group_item * item)
{
    struct xccdf_group_item *clone = &parent->sub.group;
	//requires is an oscap_list of oscap_lists of char *
	clone->requires = oscap_list_clone(item->requires, (oscap_clone_func) oscap_stringlist_clone);
	//conflicts is an oscap_list of char *
	clone->conflicts = oscap_list_clone(item->conflicts, (oscap_clone_func) oscap_strdup);
	clone->values = oscap_list_clone(item->values, (oscap_clone_func) xccdf_value_clone);
	xccdf_reparent_list(clone->values, parent);
	clone->content = oscap_list_clone(item->content, (oscap_clone_func) xccdf_item_clone);
	xccdf_reparent_list(clone->content, parent);
}

struct xccdf_value_instance * xccdf_value_instance_clone(const struct xccdf_value_instance * val)
{
	struct xccdf_value_instance * clone = calloc(1, sizeof(struct xccdf_value_instance));
    clone->type = val->type;
	
    clone->value = oscap_strdup(val->value);
    clone->defval = oscap_strdup(val->defval);
    clone->choices = oscap_list_clone(val->choices, (oscap_clone_func) oscap_strdup);
    clone->match = oscap_strdup(val->match);
    clone->lower_bound = val->lower_bound;
    clone->upper_bound = val->upper_bound;

	clone->flags = val->flags;
	xccdf_value_instance_set_selector(clone, xccdf_value_instance_get_selector(val));
	return clone;
}

void xccdf_value_item_clone(struct xccdf_value_item *clone, const struct xccdf_value_item * item)
{
	//these three are values which can be directly assigned.
	clone->type = item->type;
	clone->interface_hint = item->interface_hint;
	clone->oper = item->oper;
	
	//the rest need deep copy
	clone->instances = oscap_list_clone(item->instances, (oscap_clone_func) xccdf_value_instance_clone);
	clone->sources = oscap_list_clone(item->sources, (oscap_clone_func) oscap_strdup);
}

struct xccdf_identity * xccdf_identity_clone(const struct xccdf_identity * identity)
{
	struct xccdf_identity * clone = calloc(1, sizeof(struct xccdf_identity));
	clone->sub.authenticated = identity->sub.authenticated;
	clone->sub.privileged = identity->sub.privileged;
	clone->name = oscap_strdup(identity->name);
	return clone;
}

struct xccdf_target_fact * xccdf_target_fact_clone(const struct xccdf_target_fact * tf)
{
	struct xccdf_target_fact * clone = calloc(1, sizeof(struct xccdf_target_fact));
	clone->type = tf->type;
	clone->name = oscap_strdup(tf->name);
	clone->value = oscap_strdup(tf->value);
	return clone;
}

struct xccdf_override * xccdf_override_clone(const struct xccdf_override * override)
{
	struct xccdf_override * clone = calloc(1, sizeof(struct xccdf_override));
	clone->time = override->time;
	clone->authority = oscap_strdup(clone->authority);
	clone->old_result = override->old_result;
	clone->new_result = override->new_result;
	clone->remark = oscap_text_clone(override->remark);
	return clone;	
}

struct xccdf_message * xccdf_message_clone(const struct xccdf_message * message)
{
	struct xccdf_message * clone = calloc(1, sizeof(struct xccdf_message));
	clone->content = oscap_strdup(message->content);
	clone->severity = message->severity;
	return clone;
}

struct xccdf_instance * xccdf_instance_clone(const struct xccdf_instance * instance)
{
    struct xccdf_instance * clone = calloc(1, sizeof(struct xccdf_instance));
    clone->context = oscap_strdup(instance->context);
    clone->parent_context = oscap_strdup(instance->parent_context);
    clone->content = oscap_strdup(instance->content);
    return clone;
}

struct xccdf_rule_result * xccdf_rule_result_clone(const struct xccdf_rule_result * result)
{
	struct xccdf_rule_result * clone = calloc(1, sizeof(struct xccdf_rule_result));
	clone->idref = oscap_strdup(result->idref);
	clone->role = result->role;
	clone->time = oscap_strdup(result->time);
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
	struct xccdf_score * clone = calloc(1, sizeof(struct xccdf_score));
	clone->maximum = score->maximum;
	clone->score = score->score;
	clone->system = oscap_strdup(score->system);
	return clone;
}

void xccdf_result_item_clone(struct xccdf_result_item *clone, const struct xccdf_result_item * item)
{
	clone->start_time = oscap_strdup(item->start_time);
	clone->end_time = oscap_strdup(item->end_time);
	clone->test_system = oscap_strdup(item->test_system);
	clone->benchmark_uri = oscap_strdup(item->benchmark_uri);
	clone->profile = oscap_strdup(item->profile);

	clone->identities = oscap_list_clone(item->identities, (oscap_clone_func) xccdf_identity_clone);
	clone->targets = oscap_list_clone(item->targets, (oscap_clone_func) oscap_strdup);
	clone->organizations = oscap_list_clone(item->organizations, (oscap_clone_func) oscap_strdup);
	clone->remarks = oscap_list_clone(item->remarks, (oscap_clone_func) oscap_text_clone);
	clone->target_addresses = oscap_list_clone(item->target_addresses, (oscap_clone_func) oscap_strdup);
	clone->target_facts = oscap_list_clone(item->target_facts, (oscap_clone_func) xccdf_target_fact_clone);
	clone->target_id_refs = oscap_list_clone(item->target_id_refs, (oscap_clone_func) xccdf_target_identifier_clone);
	clone->applicable_platforms = oscap_list_clone(item->applicable_platforms, (oscap_clone_func) oscap_strdup);
	clone->setvalues = oscap_list_clone(item->setvalues, (oscap_clone_func) xccdf_setvalue_clone);
	clone->rule_results = oscap_list_clone(item->rule_results, (oscap_clone_func) xccdf_rule_result_clone);
	clone->scores = oscap_list_clone(item->scores, (oscap_clone_func) xccdf_score_clone);
}

void xccdf_reparent_list(struct oscap_list * item_list, struct xccdf_item * parent)
{
	struct oscap_iterator *it = oscap_iterator_new(item_list);
	while(oscap_iterator_has_more(it))
	{
		struct xccdf_item * item = oscap_iterator_next(it);
		xccdf_reparent_item(item, parent);
	}
    oscap_iterator_free(it);
}

void xccdf_reparent_item(struct xccdf_item * item, struct xccdf_item * parent)
{
	if(item != NULL)
	{
		item->item.parent = parent;
		xccdf_benchmark_register_item(xccdf_item_get_benchmark(item), item);
	}
}

const struct oscap_text_traits XCCDF_TEXT_PLAIN    = { .can_override = true };
const struct oscap_text_traits XCCDF_TEXT_HTML     = { .html = true, .can_override = true };
const struct oscap_text_traits XCCDF_TEXT_PLAINSUB = { .can_override = true, .can_substitute = true };
const struct oscap_text_traits XCCDF_TEXT_HTMLSUB  = { .html = true, .can_override = true, .can_substitute = true };
const struct oscap_text_traits XCCDF_TEXT_NOTICE   = { .html = true };
const struct oscap_text_traits XCCDF_TEXT_PROFNOTE = { .html = true, .can_substitute = true };
