/**
 * @file oval_resultSystem.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oval_agent_api_impl.h"
#include "results/oval_results_impl.h"
#include "adt/oval_collection_impl.h"
#include "public/oval_agent_api.h"
#include "common/util.h"
#include "common/debug_priv.h"

typedef struct oval_result_definition {
	struct oval_definition *definition;
	oval_result_t result;
	struct oval_result_system *system;
	struct oval_result_criteria_node *criteria;
	struct oval_collection *messages;
	int instance;
	int variable_instance_hint;			///< A next possible variable_instance attribute
} oval_result_definition_t;

struct oval_result_definition *oval_result_definition_new(struct oval_result_system *sys, char *definition_id) {
	oval_result_definition_t *definition = (oval_result_definition_t *)
	    oscap_alloc(sizeof(oval_result_definition_t));
	if (definition == NULL)
		return NULL;

	definition->system = sys;
	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);
	struct oval_definition_model *definition_model = oval_syschar_model_get_definition_model(syschar_model);
	definition->definition = oval_definition_model_get_new_definition(definition_model, definition_id);
	definition->result = OVAL_RESULT_NOT_EVALUATED;
	definition->criteria = NULL;
	definition->messages = oval_collection_new();
	definition->variable_instance_hint = 1;
	definition->instance = 1;
	return definition;
}

struct oval_result_definition *oval_result_definition_clone
    (struct oval_result_system *new_system, struct oval_result_definition *old_definition) {
	const char *id = oval_result_definition_get_id(old_definition);
	struct oval_result_definition *new_definition = oval_result_definition_new(new_system, (char *) id);

	struct oval_result_criteria_node *old_crit = oval_result_definition_get_criteria(old_definition);
	if (old_crit) {
		struct oval_result_criteria_node *new_crit = oval_result_criteria_node_clone(new_system, old_crit);
		oval_result_definition_set_criteria(new_definition, new_crit);
	}

	struct oval_message_iterator *old_messages = oval_result_definition_get_messages(old_definition);
	while (oval_message_iterator_has_more(old_messages)) {
		struct oval_message *old_message = oval_message_iterator_next(old_messages);
		struct oval_message *new_message = oval_message_clone(old_message);
		oval_result_definition_add_message(new_definition, new_message);
	}
	oval_message_iterator_free(old_messages);

	oval_result_definition_set_result(new_definition, oval_result_definition_get_result(old_definition));
	oval_result_definition_set_instance(new_definition, oval_result_definition_get_instance(old_definition));
	oval_result_definition_set_variable_instance_hint(new_definition, oval_result_definition_get_variable_instance_hint(old_definition));
	return new_definition;
}

void oval_result_definition_free(struct oval_result_definition *definition)
{
	__attribute__nonnull__(definition);

	if (definition->criteria)
		oval_result_criteria_node_free(definition->criteria);
	oval_collection_free_items(definition->messages, (oscap_destruct_func) oval_message_free);

	definition->system = NULL;
	definition->criteria = NULL;
	definition->definition = NULL;
	definition->messages = NULL;
	definition->result = OVAL_RESULT_NOT_EVALUATED;
	definition->instance = 1;
	oscap_free(definition);
}

struct oval_result_definition *make_result_definition_from_oval_definition
    (struct oval_result_system *sys, struct oval_definition *oval_definition, int variable_instance) {
	char *defid = oval_definition_get_id(oval_definition);
	struct oval_result_definition *rslt_definition = oval_result_definition_new(sys, defid);
	oval_result_definition_set_instance(rslt_definition, variable_instance);
	struct oval_criteria_node *oval_criteria = oval_definition_get_criteria(oval_definition);
	struct oval_result_criteria_node *rslt_criteria = 
		make_result_criteria_node_from_oval_criteria_node(sys, oval_criteria, variable_instance);
	if (rslt_criteria)
		oval_result_definition_set_criteria(rslt_definition, rslt_criteria);
	return rslt_definition;
}

struct oval_definition *oval_result_definition_get_definition(const struct oval_result_definition *definition) {
	__attribute__nonnull__(definition);

	return definition->definition;
}

struct oval_result_system *oval_result_definition_get_system(const struct oval_result_definition *definition) {
	__attribute__nonnull__(definition);

	return definition->system;
}

int oval_result_definition_get_instance(const struct oval_result_definition *definition) {
	__attribute__nonnull__(definition);

	return definition->instance;
}

oval_result_t oval_result_definition_eval(struct oval_result_definition * definition)
{
	__attribute__nonnull__(definition);

	if (definition->result == OVAL_RESULT_NOT_EVALUATED) {
		struct oval_result_criteria_node *criteria = oval_result_definition_get_criteria(definition);
		if (criteria != NULL) {
			dIndent(1);
			definition->result = oval_result_criteria_node_eval(criteria);
			dIndent(-1);
		}
	}
	dI("Definition '%s' evaluated as %s.", oval_result_definition_get_id(definition), oval_result_get_text(definition->result));
	return definition->result;
}

oval_result_t oval_result_definition_get_result(const struct oval_result_definition * definition)
{
	__attribute__nonnull__(definition);

	return definition->result;
}

struct oval_message_iterator *oval_result_definition_get_messages(const struct oval_result_definition *definition) 
{
	__attribute__nonnull__(definition);

	return (struct oval_message_iterator *)
	    oval_collection_iterator(definition->messages);
}

struct oval_result_criteria_node *oval_result_definition_get_criteria(const struct oval_result_definition *definition) 
{
	__attribute__nonnull__(definition);

	return definition->criteria;
}

void oval_result_definition_set_result(struct oval_result_definition *definition, oval_result_t result) 
{
	__attribute__nonnull__(definition);
	definition->result = result;
}

void oval_result_definition_set_instance(struct oval_result_definition *definition, int instance) 
{
	__attribute__nonnull__(definition);
	definition->instance = instance;
	// When a new variable_instance is set, we usually want to reset the hint
	definition->variable_instance_hint = instance;
}

void oval_result_definition_set_criteria(struct oval_result_definition *definition, struct oval_result_criteria_node *criteria) 
{
	__attribute__nonnull__(definition);
	if (definition->criteria) {
		if (oval_result_criteria_node_get_type(criteria) == OVAL_NODETYPE_CRITERIA) {
			oval_result_criteria_node_free(definition->criteria);
		}
	}
	definition->criteria = criteria;
}

void oval_result_definition_add_message(struct oval_result_definition *definition, struct oval_message *message) {
	__attribute__nonnull__(definition);
	if (message)
		oval_collection_add(definition->messages, message);
}

static void _oval_result_definition_consume_criteria(struct oval_result_criteria_node *node, struct oval_result_definition *definition) {
	oval_result_definition_set_criteria(definition, node);
}

static void _oval_result_definition_consume_message(struct oval_message *message, struct oval_result_definition *definition) {
	oval_result_definition_add_message(definition, message);
}

static int oval_result_definition_parse(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr) {
	int return_code = 0;
	xmlChar *localName = xmlTextReaderLocalName(reader);
	struct oval_result_system  *sys = oval_result_definition_get_system((struct oval_result_definition *) usr);
	
	if (strcmp((const char *)localName, "criteria") == 0) {
		return_code = oval_result_criteria_node_parse
		    (reader, context, sys, (oscap_consumer_func) _oval_result_definition_consume_criteria, usr);
	} else if (strcmp((const char *)localName, "message") == 0) {
		return_code = oval_message_parse_tag
		    (reader, context, (oscap_consumer_func) _oval_result_definition_consume_message, usr);
	}
	oscap_free(localName);
	return return_code;
}

int oval_result_definition_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr) {

	struct oval_result_system *sys = (struct oval_result_system *) usr;
	int return_code = 0;
	struct oval_definition_model *dmod;
	struct oval_definition *ddef;
	struct oval_result_definition *definition;
	xmlChar *definition_id = xmlTextReaderGetAttribute(reader, BAD_CAST "definition_id");
	xmlChar *definition_version = xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	int resvsn = atoi((char *)definition_version);

	oval_result_t result = oval_result_parse(reader, "result", OVAL_ENUMERATION_INVALID);

	int instance = oval_parser_int_attribute(reader, "variable_instance", 1);

	dmod = context->definition_model;
	ddef = oval_definition_model_get_new_definition(dmod, (char *) definition_id);
	definition = oval_result_system_get_new_definition(sys, ddef, instance);
	if (definition == NULL)
		return -1;

	int defvsn = oval_definition_get_version(definition->definition);
	if (defvsn && resvsn != defvsn) {
		dW("Definition versions don't match: definition id: %s, ovaldef vsn: %d, resdef vsn: %d.", definition_id, defvsn, resvsn);
	}
	oval_definition_set_version(definition->definition, resvsn);
	// The following _set_instance() might be overabundant, since it should be already set
	// by oval_result_system_get_new_definition() Let's see if the assert agrees over time:
	assert(oval_result_definition_get_instance(definition) == instance);
	oval_result_definition_set_instance(definition, instance);
	

	if ((int)result != OVAL_ENUMERATION_INVALID) {
		oval_result_definition_set_result(definition, result);
	} else {
		dW("Can't resolve result attribute, definition id: %s.", definition_id);
		oval_result_definition_set_result(definition, OVAL_RESULT_UNKNOWN);
	}

	return_code = oval_parser_parse_tag(reader, context, oval_result_definition_parse, definition);

	oscap_free(definition_id);
	oscap_free(definition_version);

	return return_code;
}

xmlNode *oval_result_definition_to_dom
    (struct oval_result_definition * definition, oval_result_directive_content_t content,
     xmlDocPtr doc, xmlNode * parent) {
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *definition_node = xmlNewTextChild(parent, ns_results, BAD_CAST "definition", NULL);

	struct oval_definition *oval_definition = oval_result_definition_get_definition(definition);
	char *definition_id = oval_definition_get_id(oval_definition);
	xmlNewProp(definition_node, BAD_CAST "definition_id", BAD_CAST definition_id);

	oval_result_t result = oval_result_definition_get_result(definition);
	const char *result_att = oval_result_get_text(result);
	xmlNewProp(definition_node, BAD_CAST "result", BAD_CAST result_att);

	int version = oval_definition_get_version(oval_definition);
	char version_att[10] = "";
	snprintf(version_att, sizeof(version_att), "%d", version);
	xmlNewProp(definition_node, BAD_CAST "version", BAD_CAST version_att);

	int instance = oval_result_definition_get_instance(definition);
	if (instance != 1 ||
			oval_result_definition_get_variable_instance_hint(definition) != instance) {
		char instance_att[10] = "";
		snprintf(instance_att, sizeof(instance_att), "%d", instance);
		xmlNewProp(definition_node, BAD_CAST "variable_instance", BAD_CAST instance_att);
	}

	struct oval_message_iterator *messages = oval_result_definition_get_messages(definition);
	while (oval_message_iterator_has_more(messages)) {
		oval_message_to_dom(oval_message_iterator_next(messages), doc, definition_node);
	}
	oval_message_iterator_free(messages);

	if (content == OVAL_DIRECTIVE_CONTENT_FULL) {
		struct oval_result_criteria_node *criteria = oval_result_definition_get_criteria(definition);
		if (criteria) {
			oval_result_criteria_node_to_dom(criteria, doc, definition_node);
		}
	}

	return definition_node;
}

/**
 * Get the value of the variable_instance hint.
 * @memberof oval_result_definition
 */
int oval_result_definition_get_variable_instance_hint(const struct oval_result_definition *definition)
{
	return definition->variable_instance_hint;
}

/**
 * This sets counter on next possible variable instance. The hint is later used
 * to decide whether instanciate a new result_definition, and what variable_instance
 * attribute assign to it.
 * @memberof oval_result_definition
 */
void oval_result_definition_set_variable_instance_hint(struct oval_result_definition *definition, int new_hint_value)
{
	definition->variable_instance_hint = new_hint_value;
}

const char *oval_result_definition_get_id(const struct oval_result_definition *rslt_definition)
{
	__attribute__nonnull__(rslt_definition);
	struct oval_definition *def = oval_result_definition_get_definition(rslt_definition);
	return (def == NULL) ? NULL : oval_definition_get_id(def);
}
