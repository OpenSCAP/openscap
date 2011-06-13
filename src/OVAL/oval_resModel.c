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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <time.h>

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
	struct oval_generator *generator;
	struct oval_definition_model *definition_model;
	struct oval_collection *systems;
        char *schema;
};

struct oval_results_model *oval_results_model_new(struct oval_definition_model *definition_model,
						  struct oval_syschar_model **syschar_models)
{
	struct oval_results_model *model = (struct oval_results_model *) oscap_alloc(sizeof(struct oval_results_model));
	if (model == NULL)
		return NULL;

	model->generator = oval_generator_new();
	model->systems = oval_collection_new();
	model->definition_model = definition_model;
        model->schema = oscap_strdup(OVAL_RES_SCHEMA_LOCATION);
	if (syschar_models) {
		struct oval_syschar_model *syschar_model;
		for (syschar_model = *syschar_models; syschar_model; syschar_model = *(++syschar_models)) {
			oval_result_system_new(model, syschar_model);
		}
	}
	return model;
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
	model->systems = NULL;
	model->definition_model = NULL;
        model->schema = NULL;

	oval_generator_free(model->generator);

	oscap_free(model);
}

struct oval_generator *oval_results_model_get_generator(struct oval_results_model *model)
{
	return model->generator;
}

void oval_results_model_set_generator(struct oval_results_model *model, struct oval_generator *generator)
{
	oval_generator_free(model->generator);
	model->generator = generator;
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
	__attribute__nonnull__(model);
	if (sys)
		oval_collection_add(model->systems, sys);
}

struct oval_result_directives *oval_results_model_import(struct oval_results_model *model, const char *file)
{
	__attribute__nonnull__(model);

	struct oval_result_directives *directives = NULL;
	xmlTextReader *reader = xmlNewTextReaderFilename(file);
	if (reader == NULL) {
                if(errno)
                        oscap_seterr(OSCAP_EFAMILY_GLIBC, errno, strerror(errno));
                oscap_dlprintf(DBG_E, "Unable to open file.\n");
                return NULL;
	}

	/* setup context */
	struct oval_parser_context context;
	context.reader = reader;
	context.results_model = model;
	context.definition_model = oval_results_model_get_definition_model(model);
	context.user_data = NULL;
	oscap_setxmlerr(xmlGetLastError());
	xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, &context);
	/* jump into oval_definitions */
	xmlTextReaderRead(reader);
	/* make sure these are results */
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int is_ovalres = strcmp((const char *)OVAL_RESULTS_NAMESPACE, namespace) == 0;
	/* star parsing */
	if (is_ovalres && (strcmp(tagname, "oval_results") == 0)) {
		if (oval_results_model_parse(reader, &context, &directives) == -1 ) {
			oval_result_directives_free(directives);
			directives = NULL;
		}
	} else {
                oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_EXMLELEM, "Missing \"oval_results\" element");
                dE("Unprocessed tag: <%s:%s>.\n", namespace, tagname);
	}

        oscap_free(tagname);
        oscap_free(namespace);
	xmlFreeTextReader(reader);

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
		root_node = xmlNewTextChild(parent, NULL, BAD_CAST "oval_results", NULL);
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

	/* Report generator & directices */
	oval_generator_to_dom(results_model->generator, doc, root_node);
	oval_result_directives_to_dom(directives, doc, root_node);

	/* Report definitions */
	struct oval_definition_model *definition_model = oval_results_model_get_definition_model(results_model);
	oval_definitions_to_dom(definition_model, doc, root_node);

	xmlNode *results_node = xmlNewTextChild(root_node, ns_results, BAD_CAST "results", NULL);
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

