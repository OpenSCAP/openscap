/**
 * @file oval_resModel.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
 */


#include <string.h>
#include <time.h>
#include "config.h"
#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#include "oval_probe_impl.h"
#include "oval_results_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/reporter_priv.h"

struct oval_results_model {
	struct oval_definition_model *definition_model;
	struct oval_collection *systems;
	bool is_locked;
        char *schema;
};

struct oval_results_model *oval_results_model_new(struct oval_definition_model *definition_model,
						  struct oval_syschar_model **syschar_models)
{
	struct oval_results_model *model = (struct oval_results_model *) oscap_alloc(sizeof(struct oval_results_model));
	if (model == NULL)
		return NULL;

	model->systems = oval_collection_new();
	model->definition_model = definition_model;
	model->is_locked = false;
        model->schema = oscap_strdup(OVAL_RES_SCHEMA_LOCATION);
	if (syschar_models) {
		struct oval_syschar_model *syschar_model;
		for (syschar_model = *syschar_models; syschar_model; syschar_model = *(++syschar_models)) {
			oval_result_system_new(model, syschar_model);
		}
	}
	return model;
}

void oval_results_model_lock(struct oval_results_model *results_model)
{
	__attribute__nonnull__(results_model);

	if (results_model && oval_results_model_is_valid(results_model))
		results_model->is_locked = true;
}

bool oval_results_model_is_locked(struct oval_results_model *results_model)
{
	__attribute__nonnull__(results_model);

	return results_model->is_locked;
}

bool oval_results_model_is_valid(struct oval_results_model * results_model)
{
	bool is_valid = true;
	struct oval_result_system_iterator *systems_itr;
	struct oval_definition_model *definition_model;

	if (results_model == NULL) {
                oscap_dlprintf(DBG_W, "Argument is not valid: NULL.\n");
		return false;
        }

	/* validate definition_model */
	definition_model = oval_results_model_get_definition_model(results_model);
	if (oval_definition_model_is_valid(definition_model) != true)
		return false;

	/* validate systems */
	systems_itr = oval_results_model_get_systems(results_model);
	while (oval_result_system_iterator_has_more(systems_itr)) {
		struct oval_result_system *rslt_system;

		rslt_system = oval_result_system_iterator_next(systems_itr);
		if (oval_result_system_is_valid(rslt_system) != true) {
			is_valid = false;
			break;
		}
	}
	oval_result_system_iterator_free(systems_itr);
	if (is_valid != true)
		return false;

	return true;
}

struct oval_results_model *oval_results_model_clone(struct oval_results_model *old_resmodel)
{
	struct oval_definition_model *old_defmodel = oval_results_model_get_definition_model(old_resmodel);
	struct oval_results_model *new_resmodel = oval_results_model_new(old_defmodel, NULL);
	struct oval_result_system_iterator *old_systems = oval_results_model_get_systems(old_resmodel);

	while (oval_result_system_iterator_has_more(old_systems)) {
		struct oval_result_system *old_system = oval_result_system_iterator_next(old_systems);
		oval_result_system_clone(new_resmodel, old_system);
	}
	oval_result_system_iterator_free(old_systems);
        new_resmodel->schema = strdup(old_resmodel->schema);

	return new_resmodel;
}

void oval_results_model_free(struct oval_results_model *model)
{
	__attribute__nonnull__(model);

        if (model->schema)
                oscap_free(model->schema);
	oval_collection_free_items(model->systems, (oscap_destruct_func) oval_result_system_free);
	model->definition_model = NULL;
	model->systems = NULL;
        model->schema = NULL;
	oscap_free(model);
}

struct oval_definition_model *oval_results_model_get_definition_model(struct oval_results_model *model) {
	__attribute__nonnull__(model);

	return model->definition_model;
}

struct oval_result_system_iterator *oval_results_model_get_systems(struct oval_results_model *model)
{
	__attribute__nonnull__(model);

	return (struct oval_result_system_iterator *)
	    oval_collection_iterator(model->systems);
}

void oval_results_model_add_system(struct oval_results_model *model, struct oval_result_system *sys)
{
	if (model && !oval_results_model_is_locked(model)) {
		if (sys)
			oval_collection_add(model->systems, sys);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

struct oval_result_directives *oval_results_model_import(struct oval_results_model *model, const char *file)
{
	__attribute__nonnull__(model);

	xmlDoc *doc = xmlParseFile(file);
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	xmlTextReader *reader = xmlNewTextReaderFilename(file);
	if (reader == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	if (xmlTextReaderRead(reader) < 1) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}
	struct oval_result_directives *directives = ovalres_parser_parse(model, reader, NULL);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);

	return directives;
}



int oval_results_model_eval(struct oval_results_model *res_model)
{
	struct oval_result_system_iterator *systems_itr;

	systems_itr = oval_results_model_get_systems(res_model);

	while (oval_result_system_iterator_has_more(systems_itr)) {
		struct oval_result_system *sys;

		sys = oval_result_system_iterator_next(systems_itr);
		if( oval_result_system_eval(sys) != 0 ) { 	/* evaluate each result_system */
			oval_result_system_iterator_free(systems_itr);
			return -1;
		}
	}
	oval_result_system_iterator_free(systems_itr);
	return 0;
}

static xmlNode *oval_results_to_dom(struct oval_results_model *results_model,
				    struct oval_result_directives *directives, xmlDocPtr doc, xmlNode * parent)
{
	xmlNode *root_node;
	if (parent) {
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_results", NULL);
	} else {
		root_node = xmlNewNode(NULL, BAD_CAST "oval_results");
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNewProp(root_node, BAD_CAST "xsi:schemaLocation", BAD_CAST results_model->schema);

	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_xsi = xmlNewNs(root_node, OVAL_XMLNS_XSI, BAD_CAST "xsi");
	xmlNs *ns_results = xmlNewNs(root_node, OVAL_RESULTS_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_xsi);
	xmlSetNs(root_node, ns_results);

	xmlNode *tag_generator = xmlNewChild(root_node, ns_results, BAD_CAST "generator", NULL);

	/* Report generator & directices */
	_generator_to_dom(doc, tag_generator);
	oval_result_directives_to_dom(directives, doc, root_node);

	/* Report definitions */
	struct oval_definition_model *definition_model = oval_results_model_get_definition_model(results_model);
	oval_definitions_to_dom(definition_model, doc, root_node);

	xmlNode *results_node = xmlNewChild(root_node, ns_results, BAD_CAST "results", NULL);
	struct oval_result_system_iterator *systems = oval_results_model_get_systems(results_model);
	while (oval_result_system_iterator_has_more(systems)) {
		struct oval_result_system *sys = oval_result_system_iterator_next(systems);
		oval_result_system_to_dom(sys, results_model, directives, doc, results_node);
	}
	oval_result_system_iterator_free(systems);

	return root_node;
}

int oval_results_model_export(struct oval_results_model *results_model,  struct oval_result_directives *directives,
                              const char *file)
{
	__attribute__nonnull__(results_model);

	int xmlCode = 0;

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_results_to_dom(results_model, directives, doc, NULL);
	xmlCode = xmlSaveFormatFileEnc(file, doc, "UTF-8", 1);
	if (xmlCode <= 0) {
		oscap_setxmlerr(xmlGetLastError());
		oscap_dlprintf(DBG_W, "No bytes exported: xmlCode: %d.\n", xmlCode);
	}

	xmlFreeDoc(doc);

	return ((xmlCode >= 1) ? 1 : -1);
}

