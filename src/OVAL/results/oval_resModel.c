/**
 * @file oval_resModel.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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
 *      "Peter Vrabec" <pvrabec@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <time.h>

#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "adt/oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#include "oval_probe_impl.h"
#include "results/oval_results_impl.h"
#include "oval_directives_impl.h"

#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/elements.h"
#include "oscap_source.h"
#include "source/oscap_source_priv.h"

struct oval_results_model {
	struct oval_generator *generator;
	struct oval_directives_model *directives_model;
	struct oval_definition_model *definition_model;
	struct oval_collection *systems;
	struct oval_probe_session *probe_session;
	bool   export_sys_chars;
};

struct oval_results_model *oval_results_model_new(struct oval_definition_model *definition_model,
						  struct oval_syschar_model **syschar_models)
{
	return oval_results_model_new_with_probe_session(definition_model, syschar_models, NULL);
}

struct oval_results_model *oval_results_model_new_with_probe_session(struct oval_definition_model *definition_model,
						  struct oval_syschar_model **syschar_models,
						  struct oval_probe_session *probe_session)
{
	struct oval_results_model *model = (struct oval_results_model *) oscap_alloc(sizeof(struct oval_results_model));
	if (model == NULL)
		return NULL;

	struct oval_generator *generator = oval_definition_model_get_generator(definition_model);
	model->generator = oval_generator_clone(generator);
	oval_generator_update_timestamp(model->generator);

	model->systems = oval_collection_new();
	model->definition_model = definition_model;
	if (syschar_models) {
		struct oval_syschar_model *syschar_model;
		for (syschar_model = *syschar_models; syschar_model; syschar_model = *(++syschar_models)) {
			oval_result_system_new(model, syschar_model);
		}
	}
	model->directives_model = oval_directives_model_new();
	model->probe_session = probe_session;
	model->export_sys_chars = true;
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
	/* ToDo: Directives Model clone */

	return new_resmodel;
}

void oval_results_model_set_export_system_characteristics(struct oval_results_model *model, bool export)
{
	model->export_sys_chars = export;
}

bool oval_results_model_get_export_system_characteristics(struct oval_results_model *model)
{
	return model->export_sys_chars;
}

void oval_results_model_free(struct oval_results_model *model)
{
	__attribute__nonnull__(model);

	oval_collection_free_items(model->systems, (oscap_destruct_func) oval_result_system_free);
	model->systems = NULL;
	model->definition_model = NULL;

	oval_generator_free(model->generator);
	model->generator=NULL;

	oval_directives_model_free(model->directives_model);
	model->directives_model=NULL;

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

struct oval_directives_model *oval_results_model_get_defdirectives_model(struct oval_results_model *model) {
	__attribute__nonnull__(model);

	return oval_directives_model_get_defdirs(model->directives_model);
}

void oval_results_model_set_defdirectives_model(struct oval_results_model *model, struct oval_result_directives *dir_model) {
	__attribute__nonnull__(model);

	oval_directives_model_set_defdirs(model->directives_model, dir_model);
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

struct oval_probe_session *oval_results_model_get_probe_session(struct oval_results_model *model)
{
	__attribute__nonnull__(model);
	return model->probe_session;
}

void oval_results_model_add_system(struct oval_results_model *model, struct oval_result_system *sys)
{
	__attribute__nonnull__(model);
	if (sys)
		oval_collection_add(model->systems, sys);
}

int oval_results_model_import_source(struct oval_results_model *model, struct oscap_source *source)
{
	__attribute__nonnull__(model);
	__attribute__nonnull__(source);

	int ret = 0;
	char *tagname = NULL;
	char *namespace = NULL;

	/* setup context */
	struct oval_parser_context context;
	context.reader = oscap_source_get_xmlTextReader(source);
	if (context.reader == NULL) {
		return -1;
	}
	context.results_model = model;
	context.definition_model = oval_results_model_get_definition_model(model);
	context.user_data = NULL;
	oscap_setxmlerr(xmlGetLastError());
	/* jump into document */
	xmlTextReaderRead(context.reader);
	/* make sure these are results */
	tagname = (char *)xmlTextReaderLocalName(context.reader);
	namespace = (char *)xmlTextReaderNamespaceUri(context.reader);
	int is_ovalres = strcmp((const char *)OVAL_RESULTS_NAMESPACE, namespace) == 0;
	/* star parsing */
	if (is_ovalres && (strcmp(tagname, OVAL_ROOT_ELM_RESULTS) == 0)) {
		ret = oval_results_model_parse(context.reader, &context);
	} else {
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "Missing \"oval_results\" element");
		ret = -1;
	}

        oscap_free(tagname);
        oscap_free(namespace);
	xmlFreeTextReader(context.reader);
	return ret;
}

int oval_results_model_import(struct oval_results_model *model, const char *file)
{
	__attribute__nonnull__(model);

	int ret = 0;

	struct oscap_source *source = oscap_source_new_from_file(file);
	ret = oval_results_model_import_source(model, source);
	oscap_source_free(source);

	return ret;
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
				    struct oval_directives_model *directives_model, 
				    xmlDocPtr doc, xmlNode * parent)
{
	xmlNode *root_node;
	struct oval_result_directives * dirs;
	struct oval_directives_model * dirs_model;

	if (parent) {
		root_node = xmlNewTextChild(parent, NULL, BAD_CAST OVAL_ROOT_ELM_RESULTS, NULL);
	} else {
		root_node = xmlNewNode(NULL, BAD_CAST OVAL_ROOT_ELM_RESULTS);
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNewNsProp(root_node, lookup_xsi_ns(doc), BAD_CAST "schemaLocation", BAD_CAST OVAL_RES_SCHEMA_LOCATION);

	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_results = xmlNewNs(root_node, OVAL_RESULTS_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_results);

	/* Report generator */
	oval_generator_to_dom(results_model->generator, doc, root_node);

	/* Report default directives and class directives from internal or external
	 * directives model(if provided) */
	dirs_model = (directives_model) ? directives_model : results_model->directives_model;
	oval_directives_model_to_dom(dirs_model, doc, root_node);

	dirs = oval_directives_model_get_defdirs(dirs_model);

	/* Report definitions */
	if(oval_result_directives_get_included(dirs)) {
		struct oval_definition_model *definition_model = oval_results_model_get_definition_model(results_model);
		oval_definition_model_to_dom(definition_model, doc, root_node);
	}

	xmlNode *results_node = xmlNewTextChild(root_node, ns_results, BAD_CAST "results", NULL);
	struct oval_result_system_iterator *systems = oval_results_model_get_systems(results_model);
	while (oval_result_system_iterator_has_more(systems)) {
		struct oval_result_system *sys = oval_result_system_iterator_next(systems);
		oval_result_system_to_dom(sys, results_model, dirs_model, doc, results_node);
	}
	oval_result_system_iterator_free(systems);

	return root_node;
}

struct oscap_source *oval_results_model_export_source(struct oval_results_model *results_model, struct oval_directives_model *directives_model, const char *name)
{
	__attribute__nonnull__(results_model);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	oval_results_to_dom(results_model, directives_model, doc, NULL);
	return oscap_source_new_from_xmlDoc(doc, name);
}

int oval_results_model_export(struct oval_results_model *results_model,
			      struct oval_directives_model *directives_model,
			      const char *file)
{
	struct oscap_source *source = oval_results_model_export_source(results_model, directives_model, file);
	if (source == NULL) {
		return -1;
	}
	int ret = oscap_source_save_as(source, NULL);
	oscap_source_free(source);
	return ret;
}

int oval_results_model_parse(xmlTextReaderPtr reader, struct oval_parser_context *context) {
        int depth = xmlTextReaderDepth(reader);
        int ret = 0;

        xmlTextReaderRead(reader);
        while ((xmlTextReaderDepth(reader) > depth) && (ret != -1 )) {
                if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        char *tagname = (char *)xmlTextReaderLocalName(reader);
                        char *namespace = (char *)xmlTextReaderNamespaceUri(reader);

                        int is_ovalres = strcmp((const char *)OVAL_RESULTS_NAMESPACE, namespace) == 0;
                        int is_ovaldef = (is_ovalres) ? false : (strcmp((const char *)OVAL_DEFINITIONS_NAMESPACE, namespace) == 0);
                        if (is_ovalres && (strcmp(tagname, "generator") == 0)) {
                                struct oval_generator *gen;
                                gen = oval_results_model_get_generator(context->results_model);
                                ret = oval_parser_parse_tag(reader, context, &oval_generator_parse_tag, gen);
                        } else if (is_ovalres && (strcmp(tagname, "directives") == 0)) {
				struct oval_result_directives *def_dirs;
				bool val;
				def_dirs = oval_directives_model_get_defdirs(context->results_model->directives_model);
				val = oval_parser_boolean_attribute(reader, "include_source_definitions", 1);
				oval_result_directives_set_included(def_dirs, val);
				ret = oval_parser_parse_tag(reader, context, &oval_result_directives_parse_tag, def_dirs);
                        } else if (is_ovalres && (strcmp(tagname, "class_directives") == 0)) {
                                struct oval_result_directives *class_dir;
                                oval_definition_class_t class_type;
                                char *class_str = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "class");
                                class_type = oval_definition_class_enum(class_str);
                                class_dir = oval_directives_model_get_new_classdir(context->results_model->directives_model, class_type);
                                ret = oval_parser_parse_tag(reader, context, &oval_result_directives_parse_tag, class_dir);
                                oscap_free(class_str);
                        } else if (is_ovaldef && (strcmp(tagname, OVAL_ROOT_ELM_DEFINITIONS) == 0)) {
                                ret = oval_definition_model_parse(reader, context);
                        } else if (is_ovalres && (strcmp(tagname, "results") == 0)) {
                                ret = oval_parser_parse_tag(reader, context, &oval_result_system_parse_tag , NULL);
                        } else {
                                dW("Unprocessed tag: <%s:%s>.", namespace, tagname);
                                oval_parser_skip_tag(reader, context);
                        }

                        oscap_free(tagname);
                        oscap_free(namespace);
                } else {
                        if (xmlTextReaderRead(reader) != 1) {
                                ret = -1;
                                break;
                        }
                }
        }

        return ret;
}


