/**
 * @file oval_sysModel.c
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


typedef struct oval_syschar_model {
	struct oval_sysinfo *sysinfo;
	struct oval_definition_model *definition_model;
	struct oval_string_map *syschar_map;
	struct oval_string_map *sysdata_map;
	struct oval_string_map *variable_binding_map;
	bool is_locked;
        char *schema;
} oval_syschar_model_t;


/* failed   - NULL
 * success  - oval_syschar_model
 * */
struct oval_syschar_model *oval_syschar_model_new(struct oval_definition_model *definition_model)
{
	oval_syschar_model_t *newmodel = (oval_syschar_model_t *) oscap_alloc(sizeof(oval_syschar_model_t));
	if (newmodel == NULL)
		return NULL;

	newmodel->sysinfo = NULL;
	newmodel->definition_model = definition_model;
	newmodel->syschar_map = oval_string_map_new();
	newmodel->sysdata_map = oval_string_map_new();
	newmodel->variable_binding_map = oval_string_map_new();
	newmodel->is_locked = false;
        newmodel->schema = oscap_strdup(OVAL_SYS_SCHEMA_LOCATION);

	/* check possible allocation problems */
	if ((newmodel->syschar_map == NULL) ||
	    (newmodel->sysdata_map == NULL) || (newmodel->variable_binding_map == NULL)) {
		oval_syschar_model_free(newmodel);
		return NULL;
	}

	return newmodel;
}

void oval_syschar_model_lock(struct oval_syschar_model *syschar_model)
{
	__attribute__nonnull__(syschar_model);

	if (syschar_model && oval_syschar_model_is_valid(syschar_model))
		syschar_model->is_locked = true;
}

bool oval_syschar_model_is_locked(struct oval_syschar_model *syschar_model)
{
	__attribute__nonnull__(syschar_model);

	return syschar_model->is_locked;
}

bool oval_syschar_model_is_valid(struct oval_syschar_model * syschar_model)
{
	bool is_valid = true;
	struct oval_syschar_iterator *syschars_itr;
	struct oval_sysinfo *sysinfo;
	struct oval_definition_model *definition_model;

	if (syschar_model == NULL) {
                oscap_dlprintf(DBG_W, "Argument is not valid: NULL.\n");
		return false;
        }

	/* validate sysinfo */
	sysinfo = oval_syschar_model_get_sysinfo(syschar_model);
	if (oval_sysinfo_is_valid(sysinfo) != true)
		return false;

	/* validate definition_model */
	definition_model = oval_syschar_model_get_definition_model(syschar_model);
	if (oval_definition_model_is_valid(definition_model) != true)
		return false;

	/* validate syschars */
	syschars_itr = oval_syschar_model_get_syschars(syschar_model);
	while (oval_syschar_iterator_has_more(syschars_itr)) {
		struct oval_syschar *syschar;
		syschar = oval_syschar_iterator_next(syschars_itr);
		if (oval_syschar_is_valid(syschar) != true) {
			is_valid = false;
			break;
		}
	}
	oval_syschar_iterator_free(syschars_itr);
	if (is_valid != true)
		return false;

	/* validate variable_bindings */
	// ToDo

	return true;
}

static void _oval_syschar_model_clone_variable_binding(struct oval_variable_binding *old_binding,
						       struct oval_syschar_model *new_model)
{

	struct oval_definition_model *new_defmodel = oval_syschar_model_get_definition_model(new_model);
	struct oval_variable_binding *new_binding = oval_variable_binding_clone(old_binding, new_defmodel);
	oval_syschar_model_add_variable_binding(new_model, new_binding);
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

struct oval_syschar_model *oval_syschar_model_clone(struct oval_syschar_model *old_model)
{

	__attribute__nonnull__(old_model);

	struct oval_syschar_model *new_model = oval_syschar_model_new(old_model->definition_model);

	_oval_syschar_model_clone(old_model->syschar_map, new_model,
				  (_oval_clone_func) oval_syschar_clone);
	_oval_syschar_model_clone(old_model->sysdata_map, new_model,
				  (_oval_clone_func) oval_sysdata_clone);
	_oval_syschar_model_clone(old_model->variable_binding_map, new_model,
				  (_oval_clone_func) _oval_syschar_model_clone_variable_binding);

	struct oval_sysinfo *old_sysinfo = oval_syschar_model_get_sysinfo(old_model);
	struct oval_sysinfo *new_sysinfo = oval_sysinfo_clone(new_model, old_sysinfo);
	oval_syschar_model_set_sysinfo(new_model, new_sysinfo);
        new_model->schema = oscap_strdup(old_model->schema);

	return new_model;
}

void oval_syschar_model_free(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	if (model->sysinfo)
		oval_sysinfo_free(model->sysinfo);
	if (model->syschar_map)
		oval_string_map_free(model->syschar_map, (oscap_destruct_func) oval_syschar_free);
	if (model->sysdata_map)
		oval_string_map_free(model->sysdata_map, (oscap_destruct_func) oval_sysdata_free);
	if (model->variable_binding_map)
		oval_string_map_free(model->variable_binding_map, (oscap_destruct_func) oval_variable_binding_free);
        if (model->schema)
                oscap_free(model->schema);

	model->sysinfo = NULL;
	model->definition_model = NULL;
	model->syschar_map = NULL;
	model->sysdata_map = NULL;
	model->variable_binding_map = NULL;
        model->schema = NULL;
	oscap_free(model);
}

void oval_syschar_model_reset(struct oval_syschar_model *model) 
{
        if (model->syschar_map)
                oval_string_map_free(model->syschar_map, (oscap_destruct_func) oval_syschar_free);
        if (model->sysdata_map)
                oval_string_map_free(model->sysdata_map, (oscap_destruct_func) oval_sysdata_free);
        if (model->variable_binding_map)
                oval_string_map_free(model->variable_binding_map, (oscap_destruct_func) oval_variable_binding_free);
        model->syschar_map = oval_string_map_new();
        model->sysdata_map = oval_string_map_new();
        model->variable_binding_map = oval_string_map_new();
}

struct oval_definition_model *oval_syschar_model_get_definition_model(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	return model->definition_model;
}


struct oval_syschar_iterator *oval_syschar_model_get_syschars(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	struct oval_syschar_iterator *iterator =
	    (struct oval_syschar_iterator *)oval_string_map_values(model->syschar_map);
	return iterator;
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
	if (model && !oval_syschar_model_is_locked(model)) {
		model->sysinfo = oval_sysinfo_clone(model, sysinfo);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_syschar_model_set_schema(struct oval_syschar_model *model, const char * schema)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		model->schema = oscap_strdup(schema);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}


void oval_syschar_model_add_syschar(struct oval_syschar_model *model, struct oval_syschar *syschar)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		struct oval_object *object = oval_syschar_get_object(syschar);
		if (object != NULL) {
			char *id = oval_object_get_id(object);
			oval_string_map_put(model->syschar_map, id, syschar);
		}
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_syschar_model_add_variable_binding(struct oval_syschar_model *model, struct oval_variable_binding *binding)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		struct oval_variable *variable = oval_variable_binding_get_variable(binding);
		char *varid = oval_variable_get_id(variable);
		oval_string_map_put(model->variable_binding_map, varid, binding);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}



void oval_syschar_model_add_sysdata(struct oval_syschar_model *model, struct oval_sysdata *sysdata)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		char *id = oval_sysdata_get_id(sysdata);
		if (id != NULL) {
			oval_string_map_put(model->sysdata_map, id, sysdata);
		}
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}


int oval_syschar_model_import(struct oval_syschar_model *model, const char *file)
{
	__attribute__nonnull__(model);

	int ret;

	xmlDoc *doc = xmlParseFile(file);
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	xmlTextReader *reader = xmlNewTextReaderFilename(file);
	if (reader == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	xmlTextReaderRead(reader);
	ret = ovalsys_parser_parse(model, reader, NULL);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);
	return ret;
}


void oval_syschar_model_bind_variable_model(struct oval_syschar_model *sysmodel, struct oval_variable_model *varmodel)
{
	__attribute__nonnull__(sysmodel);

	oval_definition_model_bind_variable_model(sysmodel->definition_model, varmodel);
}

struct oval_syschar *oval_syschar_model_get_syschar(struct oval_syschar_model *model, const char *object_id)
{
	__attribute__nonnull__(model);

	return (struct oval_syschar *)oval_string_map_get_value(model->syschar_map, object_id);
}

struct oval_sysdata *oval_syschar_model_get_sysdata(struct oval_syschar_model *model, const char *id)
{
	__attribute__nonnull__(model);

	return (struct oval_sysdata *)oval_string_map_get_value(model->sysdata_map, id);
}


struct oval_syschar *oval_syschar_get_new(struct oval_syschar_model *model, struct oval_object *object)
{
	char *object_id = oval_object_get_id(object);
	struct oval_syschar *syschar = oval_syschar_model_get_syschar(model, object_id);
	if (syschar == NULL) {
		syschar = oval_syschar_new(model, object);
		oval_syschar_model_add_syschar(model, syschar);
	}
	return syschar;
}

struct oval_sysdata *oval_sysdata_get_new(struct oval_syschar_model *model, const char *id)
{
	struct oval_sysdata *sysdata = oval_syschar_model_get_sysdata(model, id);
	if (sysdata == NULL) {
		sysdata = oval_sysdata_new(model, id);
		oval_syschar_model_add_sysdata(model, sysdata);
	}
	return sysdata;
}

xmlNode *oval_syschar_model_to_dom(struct oval_syschar_model * syschar_model, xmlDocPtr doc, xmlNode * parent, 
			           oval_syschar_resolver resolver, void *user_arg)
{

	xmlNodePtr root_node;

	if (parent) {
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_system_characteristics", NULL);
	} else {
		root_node = xmlNewNode(NULL, BAD_CAST "oval_system_characteristics");
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNewProp(root_node, BAD_CAST "xsi:schemaLocation", BAD_CAST syschar_model->schema);

	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_xsi = xmlNewNs(root_node, OVAL_XMLNS_XSI, BAD_CAST "xsi");
	xmlNs *ns_syschar = xmlNewNs(root_node, OVAL_SYSCHAR_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_xsi);
	xmlSetNs(root_node, ns_syschar);

        /* Report generator */
        if (!parent) {
		xmlNode *tag_generator = xmlNewChild(root_node, ns_syschar, BAD_CAST "generator", NULL);
                _generator_to_dom(doc, tag_generator);
        }

        /* Report sysinfo */
	oval_sysinfo_to_dom(oval_syschar_model_get_sysinfo(syschar_model), doc, root_node);

	struct oval_collection *collection = NULL;
	struct oval_syschar_iterator *syschars = oval_syschar_model_get_syschars(syschar_model);
	if (resolver) {
		collection = oval_collection_new();
		while (oval_syschar_iterator_has_more(syschars)) {
			struct oval_syschar *syschar = oval_syschar_iterator_next(syschars);
			if ((*resolver) (syschar, user_arg)) {
				oval_collection_add(collection, syschar);
			}
		}
		oval_syschar_iterator_free(syschars);
		syschars = (struct oval_syschar_iterator *)oval_collection_iterator(collection);
	}

	struct oval_string_map *sysdata_map = oval_string_map_new();
	if (oval_syschar_iterator_has_more(syschars)) {
		xmlNode *tag_objects = xmlNewChild(root_node, ns_syschar, BAD_CAST "collected_objects", NULL);

		while (oval_syschar_iterator_has_more(syschars)) {
			struct oval_syschar *syschar = oval_syschar_iterator_next(syschars);
			oval_syschar_to_dom(syschar, doc, tag_objects);
			struct oval_sysdata_iterator *sysdatas = oval_syschar_get_sysdata(syschar);
			while (oval_sysdata_iterator_has_more(sysdatas)) {
				struct oval_sysdata *sysdata = oval_sysdata_iterator_next(sysdatas);
				oval_string_map_put(sysdata_map, oval_sysdata_get_id(sysdata), sysdata);
			}
			oval_sysdata_iterator_free(sysdatas);
		}
	}
	oval_collection_free(collection);
	oval_syschar_iterator_free(syschars);

	struct oval_iterator *sysdatas = oval_string_map_values(sysdata_map);
	if (oval_collection_iterator_has_more(sysdatas)) {
		xmlNode *tag_items = xmlNewChild(root_node, ns_syschar, BAD_CAST "system_data", NULL);
		while (oval_collection_iterator_has_more(sysdatas)) {
			struct oval_sysdata *sysdata = (struct oval_sysdata *)
			    oval_collection_iterator_next(sysdatas);
			oval_sysdata_to_dom(sysdata, doc, tag_items);
		}
	}
	oval_collection_iterator_free(sysdatas);
	oval_string_map_free(sysdata_map, NULL);

	return root_node;
}

int oval_syschar_model_export(struct oval_syschar_model *model, const char *file)
{

	__attribute__nonnull__(model);

	int retcode = 0;

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_syschar_model_to_dom(model, doc, NULL, NULL, NULL);
	/*
	 * Dumping document to stdio or file
	 */
	retcode = xmlSaveFormatFileEnc(file, doc, "UTF-8", 1);

	if (retcode < 1)
		oscap_setxmlerr(xmlGetLastError());

	xmlFreeDoc(doc);
	return retcode;
}

