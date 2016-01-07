/**
 * @file oval_directives.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 * @cond
 */

/*
 * Copyright 2011--2013 Red Hat Inc., Durham, North Carolina.
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
 *      "Peter Vrabec" <pvrabec@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "oval_directives_impl.h"
#include "adt/oval_collection_impl.h"
#include "oval_parser_impl.h"

#include "common/assume.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/elements.h"

#define NUMBER_OF_RESULTS 6
#define NUMBER_OF_CLASSES 5


struct oval_result_directives {
	struct {
		bool reported;
		oval_result_directive_content_t content;
	} directive[NUMBER_OF_RESULTS];
	bool inc_definitions;
};

struct oval_directives_model {
	struct oval_generator *generator;
	struct oval_result_directives * def_directives;
	struct oval_result_directives * class_directives[NUMBER_OF_CLASSES];
};


static int oval_directives_model_parse(xmlTextReaderPtr, struct oval_parser_context *);

struct oval_directives_model *oval_directives_model_new(void) {

	struct oval_directives_model * new_model = (struct oval_directives_model *) 
		oscap_alloc(sizeof(struct oval_directives_model));

	new_model->generator = oval_generator_new();
	new_model->def_directives = oval_result_directives_new();

	/* no class directives */
	for(int i=0; i<NUMBER_OF_CLASSES; i++)
		new_model->class_directives[i] = NULL;
	
        return new_model;
}

void oval_directives_model_free(struct oval_directives_model *model) {
	__attribute__nonnull__(model);

	for(int i=0; i<NUMBER_OF_CLASSES; i++) {
		if (model->class_directives[i]) {
			oscap_free(model->class_directives[i]);
			model->class_directives[i]=NULL;
		}
	}

	oscap_free(model->def_directives);
	model->def_directives=NULL;

	oval_generator_free(model->generator);
	model->generator=NULL;

	oscap_free(model);
}

int oval_directives_model_import(struct oval_directives_model * model, char *file) {

	int ret=0;
	char *tagname = NULL;
	char *namespace = NULL;

	/* open file */
        xmlTextReader *reader = xmlNewTextReaderFilename(file);
        if (reader == NULL) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "%s '%s'", strerror(errno), file);
		ret = -1;
		goto cleanup;
        }

        /* setup context */
        struct oval_parser_context context;
        context.reader = reader;
        context.directives_model = model;
        context.user_data = NULL;
        xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, &context);
        /* jump into oval_system_characteristics */
        xmlTextReaderRead(reader);

        /* make sure this is a right schema and tag */
        tagname = (char *)xmlTextReaderLocalName(reader);
        namespace = (char *)xmlTextReaderNamespaceUri(reader);
        int is_ovaldir = strcmp((const char *)OVAL_DIRECTIVES_NAMESPACE, namespace) == 0;
        /* start parsing */
        if (is_ovaldir && (strcmp(tagname, OVAL_ROOT_ELM_DIRECTIVES) == 0)) {
                ret = oval_directives_model_parse(reader, &context);
        } else {
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "Missing \"oval_directives\" element");
                ret = -1;
        }

cleanup:
        oscap_free(tagname);
        oscap_free(namespace);
        xmlFreeTextReader(reader);

	return ret;
}

int oval_directives_model_export(struct oval_directives_model *model, const char *file) {
	 __attribute__nonnull__(model);

	xmlDocPtr doc;

        doc = xmlNewDoc(BAD_CAST "1.0");
        if (doc == NULL) {
                oscap_setxmlerr(xmlGetLastError());
                return -1;
        }

        oval_directives_model_to_dom(model, doc, NULL);
	return oscap_xml_save_filename(file, doc);
}



struct oval_generator *oval_directives_model_get_generator(struct oval_directives_model *model)
{
        return model->generator;
}

struct oval_result_directives *oval_directives_model_get_defdirs(struct oval_directives_model *model)
{
        return model->def_directives;
}

struct oval_result_directives *oval_directives_model_get_classdir(struct oval_directives_model *model, oval_definition_class_t classdir)
{
	/* enum -> index */
	int classind = --classdir;

	if (classind < NUMBER_OF_CLASSES)
		return model->class_directives[classind];

	return NULL;
}

struct oval_result_directives *oval_directives_model_get_new_classdir(struct oval_directives_model *model, oval_definition_class_t classdir)
{
	int classind = --classdir;

	if (classind < NUMBER_OF_CLASSES) {
		if (model->class_directives[classind] == NULL)
			model->class_directives[classind] = oval_result_directives_new();

		return model->class_directives[classind];
	}

	return NULL;
}


struct oval_result_directives *oval_result_directives_new(void)
{
	struct oval_result_directives *directives = (struct oval_result_directives *)
	    oscap_alloc(sizeof(struct oval_result_directives));
	if (directives == NULL)
		return NULL;

	/* full report by default */
	oval_result_directives_set_reported(directives,
					    OVAL_RESULT_TRUE	| OVAL_RESULT_FALSE | 
					    OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED | 
					    OVAL_RESULT_ERROR	| OVAL_RESULT_NOT_APPLICABLE, 
					    true);

	oval_result_directives_set_content(directives,
					   OVAL_RESULT_TRUE | OVAL_RESULT_FALSE |
					   OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED |
					   OVAL_RESULT_NOT_APPLICABLE | OVAL_RESULT_ERROR,
					   OVAL_DIRECTIVE_CONTENT_FULL);
	/* default value */
	directives->inc_definitions=true;

	return directives;
}

void oval_result_directives_free(struct oval_result_directives *directives)
{
	oscap_free(directives);
}

bool oval_result_directives_get_reported(struct oval_result_directives *directives, oval_result_t type) {
	__attribute__nonnull__(directives);

	int i=-1;
	while (type) {
		i = i + 1;
		type = type >> 1;
	}

        assume_r(i >= 0 && i < NUMBER_OF_RESULTS,
                 /* return */ false);

	return directives->directive[i].reported;
}

oval_result_directive_content_t oval_result_directives_get_content
    (struct oval_result_directives * directives, oval_result_t type) {
	__attribute__nonnull__(directives);

	int i=-1;
	while (type) {
		i = i + 1;
		type = type >> 1;
	}

        assume_r(i >= 0 && i < NUMBER_OF_RESULTS,
                 /* return */ OVAL_DIRECTIVE_CONTENT_UNKNOWN);

	return directives->directive[i].content;
}

bool oval_result_directives_get_included(struct oval_result_directives *directives) {
	__attribute__nonnull__(directives);

	return directives->inc_definitions;
}

void oval_result_directives_set_included(struct oval_result_directives *directives, bool val) {
	__attribute__nonnull__(directives);

	directives->inc_definitions = val;
}

void oval_result_directives_set_reported(struct oval_result_directives *directives, int flag, bool val) {
	__attribute__nonnull__(directives);

	int i=0;
	while (flag>>i) {
		if( flag & (1 << i) )
			directives->directive[i].reported = val;
		i = i + 1;
	}
}

void oval_result_directives_set_content(struct oval_result_directives *directives, int flag, oval_result_directive_content_t content) 
{
	__attribute__nonnull__(directives);
	int i=0;
	while (flag>>i) {
		if( flag & (1 << i) )
			directives->directive[i].content = content;
		i = i + 1;
	}
}


static int oval_directives_model_parse(xmlTextReaderPtr reader, struct oval_parser_context *context)
{
	int depth = xmlTextReaderDepth(reader);
	int ret = 0;

	xmlTextReaderRead(reader);
	while ((xmlTextReaderDepth(reader) > depth) && (ret != -1 )) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			char *tagname = (char *)xmlTextReaderLocalName(reader);
			char *namespace = (char *)xmlTextReaderNamespaceUri(reader);

			int is_ovaldir = strcmp((const char*)OVAL_DIRECTIVES_NAMESPACE, namespace) == 0;
			if (is_ovaldir && (strcmp(tagname, "generator") == 0)) {
				struct oval_generator *gen;
				gen = oval_directives_model_get_generator(context->directives_model);
				ret = oval_parser_parse_tag(reader, context, &oval_generator_parse_tag, gen);
			} else if (is_ovaldir && (strcmp(tagname, "directives") == 0)) {
				struct oval_result_directives *def_dirs;
				bool val;
                                def_dirs = oval_directives_model_get_defdirs(context->directives_model);
				val=oval_parser_boolean_attribute(reader, "include_source_definitions", 1);
				oval_result_directives_set_included(def_dirs, val);
                                ret = oval_parser_parse_tag(reader, context, &oval_result_directives_parse_tag, def_dirs);
			} else if (is_ovaldir && (strcmp(tagname, "class_directives") == 0)) {
				struct oval_result_directives *class_dir;
				oval_definition_class_t class_type;
				char *class_str = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "class");
				class_type = oval_definition_class_enum(class_str);
				class_dir = oval_directives_model_get_new_classdir(context->directives_model, class_type);
                                ret = oval_parser_parse_tag(reader, context, &oval_result_directives_parse_tag, class_dir);
				oscap_free(class_str);
			} else {
				dW("Unprocessed tag: <%s:%s>.", namespace, tagname);
				oval_parser_skip_tag(reader, context);
			}

			oscap_free(tagname);
			oscap_free(namespace);
		} else {
			if (xmlTextReaderRead(reader) != XML_READER_TYPE_ELEMENT) {
				ret = -1;
				break;
			}
		}
	}

	return ret;
}

static const struct oscap_string_map OVAL_DIRECTIVE_MAP[] = {
	{OVAL_RESULT_TRUE, "definition_true"},
	{OVAL_RESULT_FALSE, "definition_false"},
	{OVAL_RESULT_UNKNOWN, "definition_unknown"},
	{OVAL_RESULT_ERROR, "definition_error"},
	{OVAL_RESULT_NOT_EVALUATED, "definition_not_evaluated"},
	{OVAL_RESULT_NOT_APPLICABLE, "definition_not_applicable"},
	{OVAL_ENUMERATION_INVALID, NULL}
};

int oval_result_directives_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr) {
	struct oval_result_directives *directives = (struct oval_result_directives *)usr;
	int retcode = 0;

	xmlChar *name = xmlTextReaderLocalName(reader);
	oval_result_t type = oscap_string_to_enum(OVAL_DIRECTIVE_MAP, (const char *)name);

	if ( (int) type != OVAL_ENUMERATION_INVALID) {
		/*reported */
		xmlChar *boolstr = xmlTextReaderGetAttribute(reader, BAD_CAST "reported");
		bool reported = (strcmp((const char *)boolstr, "1") == 0) || (strcmp((const char *)boolstr, "true") == 0);
		oscap_free(boolstr);
		oval_result_directives_set_reported(directives, type, reported);

		/*content */
		xmlChar *contentstr = xmlTextReaderGetAttribute(reader, BAD_CAST "content");
		oval_result_directive_content_t content = OVAL_DIRECTIVE_CONTENT_UNKNOWN;
		if (contentstr) {
			if (strcmp("thin", (const char *)contentstr) == 0)
				content = OVAL_DIRECTIVE_CONTENT_THIN;
			else if (strcmp("full", (const char *)contentstr) == 0)
				content = OVAL_DIRECTIVE_CONTENT_FULL;

			if (content != OVAL_DIRECTIVE_CONTENT_UNKNOWN) {
				oval_result_directives_set_content(directives, type, content);
			} else {
				dW("Cannot resolve @content: \"%s\".", contentstr);
				retcode = 1;
			}
			oscap_free(contentstr);
		} else {
			content = OVAL_DIRECTIVE_CONTENT_FULL;
		}
	} else {
		dW("Cannot resolve <%s>.", name);
		retcode = 1;
	}
	oscap_free(name);
	return retcode;
}

xmlNode *oval_directives_model_to_dom(struct oval_directives_model *model, xmlDocPtr doc, xmlNode * parent)
{
        xmlNode *root_node;
	xmlNode *directives_node;
	const char *class_str;
	struct oval_result_directives * dirs;



	if(parent == NULL) { /* standalone OVAL Directives Document */
		root_node = xmlNewNode(NULL, BAD_CAST OVAL_ROOT_ELM_DIRECTIVES);
		xmlDocSetRootElement(doc, root_node);

		/*schemalocation */
		xmlNewProp(root_node, BAD_CAST "xsi:schemaLocation", BAD_CAST OVAL_DIR_SCHEMA_LOCATION);

		/* NS */
		xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
		xmlNs *ns_xsi = xmlNewNs(root_node, OVAL_XMLNS_XSI, BAD_CAST "xsi");
		xmlNs *ns_results = xmlNewNs(root_node, OVAL_RESULTS_NAMESPACE, BAD_CAST "oval-res");
		xmlNs *ns_directives = xmlNewNs(root_node, OVAL_DIRECTIVES_NAMESPACE, NULL);
		xmlSetNs(root_node, ns_common);
		xmlSetNs(root_node, ns_xsi);
		xmlSetNs(root_node, ns_results);
		xmlSetNs(root_node, ns_directives);

		/* Report generator */
		oval_generator_to_dom(model->generator, doc, root_node);
	}
	else { /* Directives are part of OVAL Results Document */
		root_node = parent;
	}


	/* Report default directives */
	xmlNs *ns_search = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	directives_node = xmlNewTextChild(root_node, ns_search, BAD_CAST "directives", NULL);
	dirs = oval_directives_model_get_defdirs(model);
	if(!oval_result_directives_get_included(dirs))
		xmlNewProp(directives_node, BAD_CAST "include_source_definitions", BAD_CAST "false");
	oval_result_directives_to_dom(dirs, doc, directives_node);

	/* Report class directives */
	for(int i=OVAL_CLASS_COMPLIANCE; i<=OVAL_CLASS_VULNERABILITY; i++) {
		dirs = oval_directives_model_get_classdir(model, i);
		if (dirs) {
			directives_node = xmlNewTextChild(root_node, ns_search, BAD_CAST "class_directives", NULL);
			class_str = oval_definition_class_text(i);
			xmlNewProp(directives_node, BAD_CAST "class", BAD_CAST class_str);
			oval_result_directives_to_dom(dirs, doc, directives_node);
		}
	}

	return root_node;
}

int oval_result_directives_to_dom(struct oval_result_directives *directives, xmlDoc * doc, xmlNode * parent) {
	int retcode = 1;
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *directives_node = parent;

	const struct oscap_string_map *map;
	for (map = OVAL_DIRECTIVE_MAP; map->string; map++) {
		oval_result_t directive = (oval_result_t) map->value;
		bool reported = oval_result_directives_get_reported(directives, directive);
		oval_result_directive_content_t content = oval_result_directives_get_content(directives, directive);
		xmlNode *directive_node = xmlNewTextChild(directives_node, ns_results, BAD_CAST(map->string), NULL);
		char *val_reported = (reported) ? "true" : "false";
		char *val_content = (content == OVAL_DIRECTIVE_CONTENT_FULL) ? "full" : "thin";
		xmlNewProp(directive_node, BAD_CAST "reported", BAD_CAST val_reported);
		xmlNewProp(directive_node, BAD_CAST "content", BAD_CAST val_content);
	}
	return retcode;
}

/**
 * @endcond
 */
