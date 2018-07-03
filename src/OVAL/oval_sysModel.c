/**
 * @file oval_sysModel.c
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
 *      Šimon Lukašík
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
#include "adt/oval_smc_impl.h"
#include "adt/oval_smc_iterator_impl.h"
#include "oval_system_characteristics_impl.h"
#if defined(OVAL_PROBES_ENABLED)
# include "oval_probe_impl.h"
#endif
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/elements.h"
#include "oscap_source.h"
#include "source/oscap_source_priv.h"


typedef struct oval_syschar_model {
	struct oval_generator *generator;
	struct oval_sysinfo *sysinfo;
	struct oval_definition_model *definition_model;
	struct oval_smc *syschar_map;				///< Represents objects within <collected_objects> element
	struct oval_string_map *sysitem_map;			///< Represents items within <system_data> element
        char *schema;
} oval_syschar_model_t;						///< Represents <oval_system_characteristics> element


/* failed   - NULL
 * success  - oval_syschar_model
 * */
struct oval_syschar_model *oval_syschar_model_new(struct oval_definition_model *definition_model)
{
	oval_syschar_model_t *newmodel = (oval_syschar_model_t *) malloc(sizeof(oval_syschar_model_t));
	if (newmodel == NULL)
		return NULL;

	struct oval_generator *generator = oval_definition_model_get_generator(definition_model);
	newmodel->generator = oval_generator_clone(generator);
	oval_generator_update_timestamp(newmodel->generator);

	newmodel->sysinfo = NULL;
	newmodel->definition_model = definition_model;
	newmodel->syschar_map = oval_smc_new();
	newmodel->sysitem_map = oval_string_map_new();
        newmodel->schema = oscap_strdup(OVAL_SYS_SCHEMA_LOCATION);

	/* check possible allocation problems */
	if ((newmodel->syschar_map == NULL) || (newmodel->sysitem_map == NULL) ) {
		oval_syschar_model_free(newmodel);
		return NULL;
	}

	return newmodel;
}

typedef void (*_oval_clone_func) (void *, struct oval_syschar_model *);

static void _oval_syschar_model_clone(struct oval_string_map *oldmap,
				      struct oval_syschar_model *newmodel, _oval_clone_func cloner)
{
	struct oval_string_iterator *keys = (struct oval_string_iterator *)oval_string_map_keys(oldmap);

	while (oval_string_iterator_has_more(keys)) {
		char *key = oval_string_iterator_next(keys);
		void *olditem = oval_string_map_get_value(oldmap, key);
		(*cloner) (newmodel, olditem);
	}
	oval_string_iterator_free(keys);
}

static void _oval_syschar_model_clone_helper(struct oval_smc *oldmap, struct oval_syschar_model *newmodel, _oval_clone_func cloner)
{
	struct oval_smc_iterator *it = oval_smc_iterator_new(oldmap);
	while (oval_smc_iterator_has_more(it)) {
		void *olditem = oval_smc_iterator_next(it);
		(*cloner) (newmodel, olditem);
	}
	oval_smc_iterator_free(it);
}

struct oval_syschar_model *oval_syschar_model_clone(struct oval_syschar_model *old_model)
{

	__attribute__nonnull__(old_model);

	struct oval_syschar_model *new_model = oval_syschar_model_new(old_model->definition_model);

	_oval_syschar_model_clone_helper(old_model->syschar_map, new_model, (_oval_clone_func) oval_syschar_clone);
	_oval_syschar_model_clone(old_model->sysitem_map, new_model,
				  (_oval_clone_func) oval_sysitem_clone);

	struct oval_sysinfo *old_sysinfo = oval_syschar_model_get_sysinfo(old_model);
	struct oval_sysinfo *new_sysinfo = oval_sysinfo_clone(new_model, old_sysinfo);
	oval_syschar_model_set_sysinfo(new_model, new_sysinfo);
	oval_sysinfo_free(new_sysinfo);
        new_model->schema = oscap_strdup(old_model->schema);

	return new_model;
}

void oval_syschar_model_free(struct oval_syschar_model *model)
{
	if (model != NULL) {
		oval_sysinfo_free(model->sysinfo);
		oval_smc_free(model->syschar_map, (oscap_destruct_func) oval_syschar_free);
		if (model->sysitem_map)
			oval_string_map_free(model->sysitem_map, (oscap_destruct_func) oval_sysitem_free);
		free(model->schema);
		oval_generator_free(model->generator);
		free(model);
	}
}

void oval_syschar_model_reset(struct oval_syschar_model *model) 
{
        if (model->syschar_map)
                oval_smc_free(model->syschar_map, (oscap_destruct_func) oval_syschar_free);
        if (model->sysitem_map)
                oval_string_map_free(model->sysitem_map, (oscap_destruct_func) oval_sysitem_free);
        model->syschar_map = oval_smc_new();
        model->sysitem_map = oval_string_map_new();
}

struct oval_generator *oval_syschar_model_get_generator(struct oval_syschar_model *model)
{
	return model->generator;
}

void oval_syschar_model_set_generator(struct oval_syschar_model *model, struct oval_generator *generator)
{
	oval_generator_free(model->generator);
	model->generator = generator;
}

struct oval_definition_model *oval_syschar_model_get_definition_model(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	return model->definition_model;
}

struct oval_syschar_iterator *oval_syschar_model_get_syschars(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	return oval_syschar_iterator_new(model->syschar_map);
}

struct oval_sysinfo *oval_syschar_model_get_sysinfo(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	return model->sysinfo;
}

const char * oval_syschar_model_get_schema(struct oval_syschar_model * model)
{
        __attribute__nonnull__(model);

        return model->schema;
}

void oval_syschar_model_set_sysinfo(struct oval_syschar_model *model, struct oval_sysinfo *sysinfo)
{
	__attribute__nonnull__(model);
	if (model->sysinfo)
		oval_sysinfo_free(model->sysinfo);
	model->sysinfo = oval_sysinfo_clone(model, sysinfo);
}

void oval_syschar_model_set_schema(struct oval_syschar_model *model, const char * schema)
{
	__attribute__nonnull__(model);
	model->schema = oscap_strdup(schema);
}


void oval_syschar_model_add_syschar(struct oval_syschar_model *model, struct oval_syschar *syschar)
{
	__attribute__nonnull__(model);
	const char *id = oval_syschar_get_id(syschar);
	if (id != NULL)
		oval_smc_put_last(model->syschar_map, id, syschar);
}

void oval_syschar_model_add_sysitem(struct oval_syschar_model *model, struct oval_sysitem *sysitem)
{
	__attribute__nonnull__(model);
	char *id = oval_sysitem_get_id(sysitem);
	if (id != NULL) {
		oval_string_map_put(model->sysitem_map, id, sysitem);
	}
}

int oval_syschar_model_import_source(struct oval_syschar_model *model, struct oscap_source *source)
{
	int ret = 0;
	/* setup context */
        struct oval_parser_context context;
        context.reader = oscap_source_get_xmlTextReader(source);
	if (context.reader == NULL) {
		return -1;
	}
        context.definition_model = oval_syschar_model_get_definition_model(model);
        context.syschar_model = model;
        context.user_data = NULL;

	/* jump into oval_system_characteristics */
	xmlTextReaderRead(context.reader);
	/* make sure this is syschar */
	char *tagname = (char *)xmlTextReaderLocalName(context.reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(context.reader);
	int is_ovalsys = strcmp((const char *)OVAL_SYSCHAR_NAMESPACE, namespace) == 0;
	/* start parsing */
	if (is_ovalsys && (strcmp(tagname, OVAL_ROOT_ELM_SYSCHARS) == 0)) {
		ret = oval_syschar_model_parse(context.reader, &context);
	} else {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Missing \"oval_system_characteristics\" element");
		dE("Unprocessed tag: <%s:%s>.", namespace, tagname);
		ret = -1;
	}

	free(tagname);
	free(namespace);
	xmlFreeTextReader(context.reader);
	return ret;
}

int oval_syschar_model_bind_variable_model(struct oval_syschar_model *sysmodel, struct oval_variable_model *varmodel)
{
	__attribute__nonnull__(sysmodel);

	return oval_definition_model_bind_variable_model(sysmodel->definition_model, varmodel);
}

struct oval_syschar *oval_syschar_model_get_syschar(struct oval_syschar_model *model, const char *object_id)
{
	__attribute__nonnull__(model);

	return (struct oval_syschar *)oval_smc_get_last(model->syschar_map, object_id);
}

struct oval_sysitem *oval_syschar_model_get_sysitem(struct oval_syschar_model *model, const char *id)
{
	__attribute__nonnull__(model);

	return (struct oval_sysitem *)oval_string_map_get_value(model->sysitem_map, id);
}


struct oval_syschar *oval_syschar_model_get_new_syschar(struct oval_syschar_model *model, struct oval_object *object)
{
	char *object_id = oval_object_get_id(object);
	struct oval_syschar *syschar = oval_syschar_model_get_syschar(model, object_id);
	if (syschar == NULL) {
		syschar = oval_syschar_new(model, object);
	}
	return syschar;
}

struct oval_sysitem *oval_syschar_model_get_new_sysitem(struct oval_syschar_model *model, const char *id)
{
	struct oval_sysitem *sysitem = oval_syschar_model_get_sysitem(model, id);
	if (sysitem == NULL) {
		sysitem = oval_sysitem_new(model, id);
	}
	return sysitem;
}

xmlNode *oval_syschar_model_to_dom(struct oval_syschar_model * syschar_model, xmlDocPtr doc, xmlNode * parent, 
			           oval_syschar_resolver resolver, void *user_arg, bool export_syschar)
{

	xmlNodePtr root_node = NULL;

	if (parent) { /* result file */
		root_node = xmlNewTextChild(parent, NULL, BAD_CAST OVAL_ROOT_ELM_SYSCHARS, NULL);
	} else { /* system characteristics file, we are the root */
		root_node = xmlNewNode(NULL, BAD_CAST OVAL_ROOT_ELM_SYSCHARS);
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNewNsProp(root_node, lookup_xsi_ns(doc), BAD_CAST "schemaLocation", BAD_CAST syschar_model->schema);

	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_unix = xmlNewNs(root_node, OVAL_SYSCHAR_UNIX_NS, BAD_CAST "unix-sys");
	xmlNs *ns_ind = xmlNewNs(root_node, OVAL_SYSCHAR_IND_NS, BAD_CAST "ind-sys");
	xmlNs *ns_lin = xmlNewNs(root_node, OVAL_SYSCHAR_LIN_NS, BAD_CAST "lin-sys");
	xmlNs *ns_win = xmlNewNs(root_node, OVAL_SYSCHAR_WIN_NS, BAD_CAST "win-sys");
	xmlNs *ns_syschar = xmlNewNs(root_node, OVAL_SYSCHAR_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_unix);
	xmlSetNs(root_node, ns_ind);
	xmlSetNs(root_node, ns_lin);
	xmlSetNs(root_node, ns_win);
	xmlSetNs(root_node, ns_syschar);

        /* Always report the generator */
	oval_generator_to_dom(syschar_model->generator, doc, root_node);

        /* Report sysinfo */
	oval_sysinfo_to_dom(oval_syschar_model_get_sysinfo(syschar_model), doc, root_node);

	if (!export_syschar) {
		return root_node;
	}

	struct oval_smc *resolved_smc = NULL;
	struct oval_syschar_iterator *syschars = oval_syschar_model_get_syschars(syschar_model);
	if (resolver) {
		resolved_smc = oval_smc_new();
		while (oval_syschar_iterator_has_more(syschars)) {
			struct oval_syschar *syschar = oval_syschar_iterator_next(syschars);
			if ((*resolver) (syschar, user_arg)) {
				oval_smc_put_last(resolved_smc, oval_syschar_get_id(syschar), syschar);
			}
		}
		oval_syschar_iterator_free(syschars);
		syschars = oval_syschar_iterator_new(resolved_smc);
	}

	struct oval_string_map *sysitem_map = oval_string_map_new();
	if (oval_syschar_iterator_has_more(syschars)) {
		xmlNode *tag_objects = xmlNewTextChild(root_node, ns_syschar, BAD_CAST "collected_objects", NULL);

		while (oval_syschar_iterator_has_more(syschars)) {
			struct oval_syschar *syschar = oval_syschar_iterator_next(syschars);
			struct oval_object *object = oval_syschar_get_object(syschar);
			if (oval_syschar_get_flag(syschar) == SYSCHAR_FLAG_UNKNOWN /* Skip unneeded syschars */
			    || oval_object_get_base_obj(object)) /* Skip internal objects */
				continue;
			oval_syschar_to_dom(syschar, doc, tag_objects);
			struct oval_sysitem_iterator *sysitems = oval_syschar_get_sysitem(syschar);
			while (oval_sysitem_iterator_has_more(sysitems)) {
				struct oval_sysitem *sysitem = oval_sysitem_iterator_next(sysitems);
				oval_string_map_put(sysitem_map, oval_sysitem_get_id(sysitem), sysitem);
			}
			oval_sysitem_iterator_free(sysitems);
		}
	}
	oval_smc_free0(resolved_smc);
	oval_syschar_iterator_free(syschars);

	struct oval_iterator *sysitems = oval_string_map_values(sysitem_map);
	if (oval_collection_iterator_has_more(sysitems)) {
		xmlNode *tag_items = xmlNewTextChild(root_node, ns_syschar, BAD_CAST "system_data", NULL);
		while (oval_collection_iterator_has_more(sysitems)) {
			struct oval_sysitem *sysitem = (struct oval_sysitem *)
			    oval_collection_iterator_next(sysitems);
			oval_sysitem_to_dom(sysitem, doc, tag_items);
		}
	}
	oval_collection_iterator_free(sysitems);
	oval_string_map_free(sysitem_map, NULL);

	return root_node;
}

int oval_syschar_model_export(struct oval_syschar_model *model, const char *file)
{

	__attribute__nonnull__(model);

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_syschar_model_to_dom(model, doc, NULL, NULL, NULL, true);
	return oscap_xml_save_filename_free(file, doc);
}

