/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "oscap_text.h"
#include "item.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include "common/_error.h"
#include "common/debug_priv.h"
#include "common/assume.h"
#include "common/elements.h"
#include "source/public/oscap_source.h"
#include "source/oscap_source_priv.h"

#include "CPE/cpedict_priv.h"
#include "CPE/cpelang_priv.h"
#include "CPE/cpe_ctx_priv.h"

#define XCCDF_SUPPORTED "1.2"

static struct oscap_htable *xccdf_benchmark_find_target_htable(const struct xccdf_benchmark *, xccdf_type_t);
static xmlNode *xccdf_plain_text_to_dom(const struct xccdf_plain_text *ptext, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info);

struct xccdf_backref {
	struct xccdf_item **ptr;	// pointer to a pointer that is supposed to be pointing to an item with id 'id'
	xccdf_type_t type;	// expected item type
	char *id;		// id
};

struct xccdf_benchmark *xccdf_benchmark_import(const char *file)
{
	struct oscap_source *source = oscap_source_new_from_file(file);
	struct xccdf_benchmark *benchmark = xccdf_benchmark_import_source(source);
	oscap_source_free(source);
	return benchmark;
}

struct xccdf_benchmark *xccdf_benchmark_import_source(struct oscap_source *source)
{
	xmlTextReader *reader = oscap_source_get_xmlTextReader(source);

	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) ;
	struct xccdf_benchmark *benchmark = xccdf_benchmark_new();
	const bool parse_result = xccdf_benchmark_parse(XITEM(benchmark), reader);
	xmlFreeTextReader(reader);

	if (!parse_result) { // parsing fatal error
		oscap_seterr(OSCAP_EFAMILY_XML, "Failed to import XCCDF content from '%s'.", oscap_source_readable_origin(source));
		xccdf_benchmark_free(benchmark);
		return NULL;
	}

	// This is sadly the only place where we can pass origin file information
	// to the CPE1 embedded dictionary (if any). It is necessary to figure out
	// proper paths to OVAL files referenced from CPE1 dictionaries.

	// FIXME: Refactor and move this somewhere else
	struct cpe_dict_model* embedded_dict = xccdf_benchmark_get_cpe_list(benchmark);
	if (embedded_dict != NULL) {
		cpe_dict_model_set_origin_file(embedded_dict, oscap_source_readable_origin(source));
	}

	// same situation with embedded CPE2 lang model
	// FIXME: Refactor and move this somewhere else
	struct cpe_lang_model* embedded_lang_model = xccdf_benchmark_get_cpe_lang_model(benchmark);
	if (embedded_lang_model != NULL) {
		cpe_lang_model_set_origin_file(embedded_lang_model, oscap_source_readable_origin(source));
	}
	return benchmark;
}

struct xccdf_benchmark *xccdf_benchmark_new(void)
{
	struct xccdf_item *bench = xccdf_item_new(XCCDF_BENCHMARK, NULL);
	bench->sub.benchmark.schema_version = NULL;
    // lists
	bench->sub.benchmark.rear_matter  = oscap_list_new();
	bench->sub.benchmark.front_matter = oscap_list_new();
	bench->sub.benchmark.notices = oscap_list_new();
	bench->sub.benchmark.models = oscap_list_new();
	bench->sub.benchmark.content = oscap_list_new();
	bench->sub.benchmark.values = oscap_list_new();
	bench->sub.benchmark.plain_texts = oscap_list_new();
	bench->sub.benchmark.cpe_list = NULL;
	bench->sub.benchmark.cpe_lang_model = NULL;
	bench->sub.benchmark.profiles = oscap_list_new();
	bench->sub.benchmark.results = oscap_list_new();
    // hash tables
	bench->sub.benchmark.items_dict = oscap_htable_new();
	bench->sub.benchmark.profiles_dict = oscap_htable_new();
	bench->sub.benchmark.results_dict = oscap_htable_new();
	bench->sub.benchmark.clusters_dict = oscap_htable_new();

	// add the implied default scoring model
	struct xccdf_model *default_model = xccdf_model_new();
	xccdf_model_set_system(default_model, "urn:xccdf:scoring:default");
	assume_ex(xccdf_benchmark_add_model(XBENCHMARK(bench), default_model), XBENCHMARK(bench));

	return XBENCHMARK(bench);
}

struct xccdf_benchmark *xccdf_benchmark_clone(const struct xccdf_benchmark *old_benchmark)
{
	struct xccdf_item *new_benchmark = oscap_calloc(1, sizeof(struct xccdf_item) + sizeof(struct xccdf_benchmark_item));
	struct xccdf_item *old = XITEM(old_benchmark);
    xccdf_item_base_clone(&new_benchmark->item, &old->item);
	new_benchmark->type = old->type;
	//second argument is a pointer to the benchmark being created which will be the parent of all of its sub elements.
    xccdf_benchmark_item_clone(new_benchmark, old_benchmark);
	return XBENCHMARK(new_benchmark);
}

bool xccdf_benchmark_parse(struct xccdf_item * benchmark, xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_BENCHMARK);
	assert(benchmark != NULL);
	if (benchmark->type != XCCDF_BENCHMARK)
		return false;

	xccdf_benchmark_set_schema_version(XBENCHMARK(benchmark), xccdf_detect_version_parser(reader));

	if (!xccdf_item_process_attributes(benchmark, reader)) {
		return false;
	}
	benchmark->sub.benchmark.style = xccdf_attribute_copy(reader, XCCDFA_STYLE);
	benchmark->sub.benchmark.style_href = xccdf_attribute_copy(reader, XCCDFA_STYLE_HREF);
    benchmark->sub.benchmark.lang = (char *) xmlTextReaderXmlLang(reader);
	if (xccdf_attribute_has(reader, XCCDFA_RESOLVED))
		benchmark->item.flags.resolved = xccdf_attribute_get_bool(reader, XCCDFA_RESOLVED);

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		struct xccdf_model *parsed_model;

		switch (xccdf_element_get(reader)) {
		case XCCDFE_NOTICE:
				oscap_list_add(benchmark->sub.benchmark.notices, xccdf_notice_new_parse(reader));
				break;
		case XCCDFE_FRONT_MATTER:
				oscap_list_add(benchmark->sub.benchmark.front_matter, oscap_text_new_parse(XCCDF_TEXT_HTMLSUB, reader));
			break;
		case XCCDFE_REAR_MATTER:
				oscap_list_add(benchmark->sub.benchmark.rear_matter, oscap_text_new_parse(XCCDF_TEXT_HTMLSUB, reader));
			break;
		case XCCDFE_PLATFORM:
			oscap_list_add(benchmark->item.platforms, xccdf_attribute_copy(reader, XCCDFA_IDREF));
			break;
		case XCCDFE_MODEL:
			parsed_model = xccdf_model_new_xml(reader);

			// we won't add the implied default scoring model, it is already in the benchmark
			if (strcmp(xccdf_model_get_system(parsed_model), "urn:xccdf:scoring:default") != 0)
				assume_ex(xccdf_benchmark_add_model(XBENCHMARK(benchmark), parsed_model), false);
			else
				xccdf_model_free(parsed_model);

			break;
		case XCCDFE_PLAIN_TEXT:{
				const char *id = xccdf_attribute_get(reader, XCCDFA_ID);
				char *data = (char *)xmlTextReaderReadInnerXml(reader);
				if (id)
					oscap_list_add(benchmark->sub.benchmark.plain_texts,
							xccdf_plain_text_new_fill(id,
							data == NULL ? "" : data));
				xmlFree(data);
				break;
			}
		case XCCDFE_CPE_LIST:{
			struct cpe_parser_ctx *ctx = cpe_parser_ctx_from_reader(reader);
			xccdf_benchmark_set_cpe_list(XBENCHMARK(benchmark), cpe_dict_model_parse(ctx));
			cpe_parser_ctx_free(ctx);
			break;
			}
		case XCCDFE_CPE2_PLATFORMSPEC:
			xccdf_benchmark_set_cpe_lang_model(XBENCHMARK(benchmark), cpe_lang_model_parse(reader));
			break;
		case XCCDFE_PROFILE:
			oscap_list_add(benchmark->sub.benchmark.profiles, xccdf_profile_parse(reader, benchmark));
			break;
		case XCCDFE_GROUP:
		case XCCDFE_RULE:
			xccdf_content_parse(reader, benchmark);
			break;
		case XCCDFE_VALUE:
			oscap_list_add(benchmark->sub.benchmark.values, xccdf_value_parse(reader, benchmark));
			break;
		case XCCDFE_TESTRESULT:
			assume_ex(xccdf_benchmark_add_result(XBENCHMARK(benchmark), xccdf_result_new_parse(reader)), false);
			break;
		default:
			if (!xccdf_item_process_element(benchmark, reader))
				dW("Encountered an unknown element '%s' while parsing XCCDF benchmark.",
				   xmlTextReaderConstLocalName(reader));
		}
		xmlTextReaderRead(reader);
	}

	return true;
}

struct oscap_source *xccdf_benchmark_export_source(struct xccdf_benchmark *benchmark, const char *filename)
{
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	xccdf_benchmark_to_dom(benchmark, doc, NULL, NULL);
	return oscap_source_new_from_xmlDoc(doc, filename);
}

int xccdf_benchmark_export(struct xccdf_benchmark *benchmark, const char *file)
{
	__attribute__nonnull__(file);

	LIBXML_TEST_VERSION;

	struct oscap_source *source = xccdf_benchmark_export_source(benchmark, file);
	if (source == NULL) {
		return -1;
	}
	int ret = oscap_source_save_as(source, NULL);
	oscap_source_free(source);;
	return ret;
}

#define OSCAP_XML_XSI BAD_CAST "http://www.w3.org/XML/1998/namespace"
xmlNode *xccdf_benchmark_to_dom(struct xccdf_benchmark *benchmark, xmlDocPtr doc,
				xmlNode *parent, void *user_args)
{
	const struct xccdf_version_info *version_info = xccdf_benchmark_get_schema_version(benchmark);

	xmlNodePtr root_node = xccdf_item_to_dom(XITEM(benchmark), doc, parent, version_info);
	if (parent == NULL) {
		xmlDocSetRootElement(doc, root_node);
	}

	// FIXME!
	//xmlNewProp(root_node, BAD_CAST "xsi:schemaLocation", BAD_CAST XCCDF_SCHEMA_LOCATION);

	lookup_xsi_ns(doc);

	/* Handle attributes */
	if (xccdf_benchmark_get_resolved(benchmark))
		xmlNewProp(root_node, BAD_CAST "resolved", BAD_CAST "1");
	else
		xmlNewProp(root_node, BAD_CAST "resolved", BAD_CAST "0");

    const char *xmllang = xccdf_benchmark_get_lang(benchmark);
	if (xmllang) {
		xmlNs *ns_xml = xmlSearchNsByHref(doc, root_node, OSCAP_XML_XSI);
		if (ns_xml == NULL) {
			ns_xml = xmlNewNs(root_node, OSCAP_XML_XSI, BAD_CAST "xml");
		}
		xmlNewNsProp(root_node, ns_xml, BAD_CAST "lang", BAD_CAST xmllang);
	}

	const char *style = xccdf_benchmark_get_style(benchmark);
	if (style)
		xmlNewProp(root_node, BAD_CAST "style", BAD_CAST style);

	const char *style_href = xccdf_benchmark_get_style_href(benchmark);
	if (style_href)
		xmlNewProp(root_node, BAD_CAST "style-href", BAD_CAST style_href);

	// Export plain-text elements
	struct xccdf_plain_text_iterator *plain_text_it = xccdf_benchmark_get_plain_texts(benchmark);
	while (xccdf_plain_text_iterator_has_more(plain_text_it)) {
		struct xccdf_plain_text *plain_text = xccdf_plain_text_iterator_next(plain_text_it);
		xccdf_plain_text_to_dom(plain_text, doc, root_node, version_info);
	}
	xccdf_plain_text_iterator_free(plain_text_it);

	/* Handle children */
	if (xccdf_benchmark_get_cpe_list(benchmark)) {
		// CPE API can only export via xmlTextWriter, we export via DOM
		// this is used to bridge both methods
		xmlTextWriterPtr writer = xmlNewTextWriterTree(doc, root_node, 0);
		cpe_dict_export(xccdf_benchmark_get_cpe_list(benchmark), writer);
		xmlFreeTextWriter(writer);
	}
	if (xccdf_benchmark_get_cpe_lang_model(benchmark)) {
		// CPE API can only export via xmlTextWriter, we export via DOM
		// this is used to bridge both methods
		xmlTextWriterPtr writer = xmlNewTextWriterTree(doc, root_node, 0);
		cpe_lang_export(xccdf_benchmark_get_cpe_lang_model(benchmark), writer);
		xmlFreeTextWriter(writer);
	}

	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, root_node, version_info);

	struct oscap_string_iterator *platforms = xccdf_benchmark_get_platforms(benchmark);
	while (oscap_string_iterator_has_more(platforms)) {
		xmlNode *platform_node = xmlNewTextChild(root_node, ns_xccdf, BAD_CAST "platform", NULL);

		const char *idref = oscap_string_iterator_next(platforms);
		if (idref)
			xmlNewProp(platform_node, BAD_CAST "idref", BAD_CAST idref);
	}
	oscap_string_iterator_free(platforms);

	const char *version = xccdf_benchmark_get_version(benchmark);
	if (version) {
		xmlNodePtr version_node = xmlNewTextChild(root_node, ns_xccdf, BAD_CAST "version", BAD_CAST version);

		const char *version_time = xccdf_benchmark_get_version_time(benchmark);
		if (version_time)
			xmlNewProp(version_node, BAD_CAST "time", BAD_CAST version_time);

		const char *version_update = xccdf_benchmark_get_version_update(benchmark);
		if (version_update)
			xmlNewProp(version_node, BAD_CAST "update", BAD_CAST version_update);
	}

	struct oscap_string_iterator* metadata = xccdf_item_get_metadata(XITEM(benchmark));
	while (oscap_string_iterator_has_more(metadata))
	{
		const char* meta = oscap_string_iterator_next(metadata);
		xmlNode *m = oscap_xmlstr_to_dom(root_node, "metadata", meta);
		xmlSetNs(m, ns_xccdf);
	}
	oscap_string_iterator_free(metadata);

	OSCAP_FOR(xccdf_model, model, xccdf_benchmark_get_models(benchmark)) {
		xmlNode *model_node = xmlNewTextChild(root_node, ns_xccdf, BAD_CAST "model", NULL);
		xmlNewProp(model_node, BAD_CAST "system", BAD_CAST xccdf_model_get_system(model));
	}

	struct xccdf_profile_iterator *profiles = xccdf_benchmark_get_profiles(benchmark);
	while (xccdf_profile_iterator_has_more(profiles)) {
		struct xccdf_profile *profile = xccdf_profile_iterator_next(profiles);
		xccdf_item_to_dom(XITEM(profile), doc, root_node, version_info);
	}
	xccdf_profile_iterator_free(profiles);

	struct xccdf_value_iterator *values = xccdf_benchmark_get_values(benchmark);
	while (xccdf_value_iterator_has_more(values)) {
		struct xccdf_value *value = xccdf_value_iterator_next(values);
		xccdf_item_to_dom(XITEM(value), doc, root_node, version_info);
	}
	xccdf_value_iterator_free(values);

	struct xccdf_item_iterator *items = xccdf_benchmark_get_content(benchmark);
	while (xccdf_item_iterator_has_more(items)) {
		struct xccdf_item *item = xccdf_item_iterator_next(items);
		if (XBENCHMARK(xccdf_item_get_parent(item)) == benchmark)
			xccdf_item_to_dom(item, doc, root_node, version_info);
	}
	xccdf_item_iterator_free(items);

	struct xccdf_result_iterator *results = xccdf_benchmark_get_results(benchmark);
	while (xccdf_result_iterator_has_more(results)) {
		struct xccdf_result *result = xccdf_result_iterator_next(results);
		xccdf_item_to_dom(XITEM(result), doc, root_node, version_info);
	}
	xccdf_result_iterator_free(results);

	return root_node;
}

void xccdf_benchmark_dump(struct xccdf_benchmark *benchmark)
{
	struct xccdf_item *bench = XITEM(benchmark);
	printf("Benchmark : %s\n", (bench ? bench->item.id : "(NULL)"));
	if (bench) {
		xccdf_item_print(bench, 1);
		printf("  front m.");
		xccdf_print_textlist(xccdf_benchmark_get_front_matter(benchmark), 2, 80, "...");
		printf("  rear m.");
		xccdf_print_textlist(xccdf_benchmark_get_rear_matter(benchmark), 2, 80, "...");
		printf("  profiles");
		oscap_list_dump(bench->sub.benchmark.profiles, xccdf_profile_dump, 2);
		printf("  values");
		oscap_list_dump(bench->sub.benchmark.values, xccdf_value_dump, 2);
		printf("  content");
		oscap_list_dump(bench->sub.benchmark.content, xccdf_item_dump, 2);
		printf("  results");
		oscap_list_dump(bench->sub.benchmark.results, (oscap_dump_func) xccdf_result_dump, 2);
	}
}

void xccdf_benchmark_free(struct xccdf_benchmark *benchmark)
{
	if (benchmark) {
		struct xccdf_item *bench = XITEM(benchmark);
		oscap_free(bench->sub.benchmark.style);
		oscap_free(bench->sub.benchmark.style_href);
		oscap_free(bench->sub.benchmark.lang);
		oscap_list_free(bench->sub.benchmark.front_matter, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(bench->sub.benchmark.rear_matter, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(bench->sub.benchmark.notices, (oscap_destruct_func) xccdf_notice_free);
		oscap_list_free(bench->sub.benchmark.models, (oscap_destruct_func) xccdf_model_free);
		oscap_list_free(bench->sub.benchmark.content, (oscap_destruct_func) xccdf_item_free);
		oscap_list_free(bench->sub.benchmark.values, (oscap_destruct_func) xccdf_value_free);
		oscap_list_free(bench->sub.benchmark.results, (oscap_destruct_func) xccdf_result_free);
		oscap_list_free(bench->sub.benchmark.plain_texts, (oscap_destruct_func) xccdf_plain_text_free);
		cpe_dict_model_free(bench->sub.benchmark.cpe_list);
		cpe_lang_model_free(bench->sub.benchmark.cpe_lang_model);
		oscap_list_free(bench->sub.benchmark.profiles, (oscap_destruct_func) xccdf_profile_free);
		oscap_htable_free0(bench->sub.benchmark.items_dict);
		oscap_htable_free0(bench->sub.benchmark.profiles_dict);
		oscap_htable_free0(bench->sub.benchmark.results_dict);
		oscap_htable_free(bench->sub.benchmark.clusters_dict, (oscap_destruct_func) oscap_htable_free0);
		xccdf_item_release(bench);
	}
}

XCCDF_ACCESSOR_SIMPLE(benchmark, const struct xccdf_version_info*, schema_version);
XCCDF_ACCESSOR_STRING(benchmark, style)
XCCDF_ACCESSOR_STRING(benchmark, style_href)
XCCDF_ACCESSOR_STRING(benchmark, lang)
XCCDF_LISTMANIP_TEXT(benchmark, front_matter, front_matter)
XCCDF_LISTMANIP_TEXT(benchmark, rear_matter, rear_matter)
XCCDF_LISTMANIP(benchmark, notice, notices)
XCCDF_LISTMANIP(benchmark, model, models)
XCCDF_BENCHMARK_IGETTER(item, content)
XCCDF_BENCHMARK_IGETTER(result, results)
XCCDF_BENCHMARK_IGETTER(value, values)
XCCDF_BENCHMARK_IGETTER(profile, profiles)
XCCDF_ITERATOR_GEN_S(notice)
XCCDF_ITERATOR_GEN_S(model)
XCCDF_ITERATOR_GEN_S(profile)
XCCDF_LISTMANIP(benchmark, plain_text, plain_texts)
XCCDF_HTABLE_GETTER(struct xccdf_item *, benchmark, item, sub.benchmark.items_dict)
XCCDF_STATUS_CURRENT(benchmark)
OSCAP_ITERATOR_GEN(xccdf_plain_text)
OSCAP_ITERATOR_REMOVE_F(xccdf_plain_text)

XCCDF_ITEM_ADDER_REG(benchmark, rule, content)
XCCDF_ITEM_ADDER_REG(benchmark, group, content)
XCCDF_ITEM_ADDER_REG(benchmark, value, values)
XCCDF_ITEM_ADDER_REG(benchmark, profile, profiles)

bool xccdf_benchmark_add_result(struct xccdf_benchmark *benchmark, struct xccdf_result *item)
{
	const char *id = xccdf_result_get_id(item);
	if (id != NULL) {
		// Resolve possible conflicts of the IDs in the list of TestResults.
		struct xccdf_item *found = xccdf_benchmark_get_member(benchmark, XCCDF_RESULT, id);
	        if (found != NULL) {
			if (found == XITEM(item))
				return true;
			// Here is conflict. Generate a new id.
			char *new_id = xccdf_benchmark_gen_id(benchmark, XCCDF_RESULT, id);
			xccdf_result_set_id(item, new_id);
			oscap_free(new_id);
		}
	}
	return xccdf_add_item(
		((struct xccdf_item*)benchmark)->sub.benchmark.results,
		((struct xccdf_item*)benchmark),
		((struct xccdf_item*)item),
		"result" "-");
}

struct xccdf_profile *
xccdf_benchmark_get_profile_by_id(struct xccdf_benchmark *benchmark, const char *profile_id)
{
	struct xccdf_profile_iterator *profit = xccdf_benchmark_get_profiles(benchmark);
	while (xccdf_profile_iterator_has_more(profit)) {
		struct xccdf_profile *profile = xccdf_profile_iterator_next(profit);
		if (profile == NULL) {
			assert(profile != NULL);
			continue;
		}
		if (oscap_streq(xccdf_profile_get_id(profile), profile_id)) {
			xccdf_profile_iterator_free(profit);
			return profile;
		}
	}
	xccdf_profile_iterator_free(profit);
	return NULL;
}

struct xccdf_result *xccdf_benchmark_get_result_by_id(struct xccdf_benchmark *benchmark, const char *testresult_id)
{
	struct xccdf_result *result = NULL;
	if (testresult_id == NULL) {
		/* Take the latest TestResult by default. It may turn out to be
		 * a good idea to not change that, since the SCAP-Workbench project
		 * is assuming thissemantics. */
		struct xccdf_result_iterator * results_it = xccdf_benchmark_get_results(benchmark);
		while (xccdf_result_iterator_has_more(results_it))
			result = xccdf_result_iterator_next(results_it);
		xccdf_result_iterator_free(results_it);
	} else {
		result = XRESULT(xccdf_benchmark_get_member(benchmark, XCCDF_RESULT, testresult_id));
	}
	return result;
}

struct xccdf_result *xccdf_benchmark_get_result_by_id_suffix(struct xccdf_benchmark *benchmark, const char *testresult_suffix)
{
	struct xccdf_result *init_result = xccdf_benchmark_get_result_by_id(benchmark, testresult_suffix);
	if (init_result != NULL)
		return init_result;

	const char *final_result_id = NULL;
	struct xccdf_result_iterator *result_iterator = xccdf_benchmark_get_results(benchmark);

	while (xccdf_result_iterator_has_more(result_iterator)) {
		struct xccdf_result *result = xccdf_result_iterator_next(result_iterator);
		const char *result_full_id = xccdf_result_get_id(result);

		if (oscap_str_endswith(result_full_id, testresult_suffix)) {
			if (final_result_id != NULL) {
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Multiple matches found:\n%s\n%s\n",
					final_result_id, result_full_id);
				break;
			} else {
				final_result_id = result_full_id;
			}
		}
	}
	xccdf_result_iterator_free(result_iterator);

	return xccdf_benchmark_get_result_by_id(benchmark, final_result_id);
}

bool xccdf_benchmark_add_content(struct xccdf_benchmark *bench, struct xccdf_item *item)
{
	if (item == NULL) return false;
	switch (xccdf_item_get_type(item)) {
		case XCCDF_RULE:  return xccdf_benchmark_add_rule(bench, XRULE(item));
		case XCCDF_GROUP: return xccdf_benchmark_add_group(bench, XGROUP(item));
		case XCCDF_VALUE: return xccdf_benchmark_add_value(bench, XVALUE(item));
		default: return false;
	}
}


const char *xccdf_benchmark_get_plain_text(const struct xccdf_benchmark *bench, const char *id)
{
    assert(bench != NULL);

    OSCAP_FOR(xccdf_plain_text, cur, xccdf_benchmark_get_plain_texts(bench)) {
        if (oscap_streq(cur->id, id)) {
            xccdf_plain_text_iterator_free(cur_iter);
            return cur->text;
        }
    }
    return NULL;
}

bool xccdf_benchmark_set_cpe_list(struct xccdf_benchmark *benchmark, struct cpe_dict_model* cpe_list)
{
	struct xccdf_item *bench = XITEM(benchmark);

	assert(bench != NULL);

	if (bench->sub.benchmark.cpe_list)
		cpe_dict_model_free(bench->sub.benchmark.cpe_list);

	bench->sub.benchmark.cpe_list = cpe_list;

	return true;
}

struct cpe_dict_model *xccdf_benchmark_get_cpe_list(const struct xccdf_benchmark *benchmark)
{
	const struct xccdf_item *bench = XITEM(benchmark);

	assert(bench != NULL);

	return bench->sub.benchmark.cpe_list;
}

bool xccdf_benchmark_set_cpe_lang_model(struct xccdf_benchmark *benchmark, struct cpe_lang_model* cpe_lang_model)
{
	struct xccdf_item *bench = XITEM(benchmark);

	assert(bench != NULL);

	if (bench->sub.benchmark.cpe_lang_model)
		cpe_lang_model_free(bench->sub.benchmark.cpe_lang_model);

	bench->sub.benchmark.cpe_lang_model = cpe_lang_model;

	return true;
}

struct cpe_lang_model *xccdf_benchmark_get_cpe_lang_model(const struct xccdf_benchmark *benchmark)
{
	const struct xccdf_item *bench = XITEM(benchmark);

	assert(bench != NULL);

	return bench->sub.benchmark.cpe_lang_model;
}

struct xccdf_notice *xccdf_notice_new(void)
{
    struct xccdf_notice *notice = oscap_calloc(1, sizeof(struct xccdf_notice));
    notice->text = oscap_text_new_full(XCCDF_TEXT_NOTICE, NULL, NULL);
    return notice;
}

struct xccdf_notice *xccdf_notice_clone(const struct xccdf_notice * notice)
{
	 struct xccdf_notice *new_notice = oscap_calloc(1, sizeof(struct xccdf_notice));
	 new_notice->id = oscap_strdup(notice->id);
    new_notice->text = oscap_text_clone(notice->text);
    return new_notice;
}

struct xccdf_notice *xccdf_notice_new_parse(xmlTextReaderPtr reader)
{
    struct xccdf_notice *notice = oscap_calloc(1, sizeof(struct xccdf_notice));
    notice->id = xccdf_attribute_copy(reader, XCCDFA_ID);
    notice->text = oscap_text_new_parse(XCCDF_TEXT_NOTICE, reader);
    return notice;
}

void xccdf_notice_dump(struct xccdf_notice *notice, int depth)
{
	xccdf_print_depth(depth);
	printf("%.20s: ", xccdf_notice_get_id(notice));
	xccdf_print_max_text(xccdf_notice_get_text(notice), 50, "...");
	printf("\n");
}

void xccdf_notice_free(struct xccdf_notice *notice)
{
	if (notice) {
		oscap_free(notice->id);
		oscap_text_free(notice->text);
		oscap_free(notice);
	}
}

OSCAP_ACCESSOR_STRING(xccdf_notice, id)
OSCAP_ACCESSOR_TEXT(xccdf_notice, text)

OSCAP_DEPRECATED(
void xccdf_cleanup(void)
{
	xmlCleanupParser();
}
)

const char * xccdf_benchmark_supported(void)
{
    return XCCDF_SUPPORTED;
}

const struct xccdf_version_info *xccdf_benchmark_supported_schema_version(void)
{
	return xccdf_version_info_find(xccdf_benchmark_supported());
}

struct xccdf_group *xccdf_benchmark_append_new_group(struct xccdf_benchmark *benchmark, const char *id)
{
	if (benchmark == NULL) return NULL;
	struct xccdf_group *group = xccdf_group_new();
	xccdf_group_set_id(group, id);
	assume_ex(xccdf_benchmark_add_group(benchmark, group), group);
    return group;
}
struct xccdf_value *xccdf_benchmark_append_new_value(struct xccdf_benchmark *benchmark, const char *id, xccdf_value_type_t type)
{
	if (benchmark == NULL) return NULL;
	struct xccdf_value *value = xccdf_value_new(type);
	xccdf_value_set_id(value, id);
	assume_ex(xccdf_benchmark_add_value(benchmark, value), value);
    return value;
}
struct xccdf_rule *xccdf_benchmark_append_new_rule(struct xccdf_benchmark *benchmark, const char *id)
{
	if (benchmark == NULL) return NULL;
	struct xccdf_rule *rule = xccdf_rule_new();
	xccdf_rule_set_id(rule, id);
	assume_ex(xccdf_benchmark_add_rule(benchmark, rule), value);
    return rule;
}

char *xccdf_benchmark_gen_id(struct xccdf_benchmark *benchmark, xccdf_type_t type, const char *prefix)
{
	assert(prefix != NULL);

	const char *fmt = "%s%03d";
	char foo[2];
	int length = snprintf(foo, 1, fmt, prefix, 0);
	if (length < 0)
		return NULL;
	length++;
	char *buff = (char *) oscap_calloc(length, sizeof(char));
	int ret;
	int i = 0;

	do {
		ret = snprintf(buff, length, fmt, prefix, ++i);
		if (ret < 0 || ret > length) {
			oscap_free(buff);
			return NULL;
		}
	} while (xccdf_benchmark_get_member(benchmark, type, buff) != NULL);

	return buff;
}

bool xccdf_add_item(struct oscap_list *list, struct xccdf_item *parent, struct xccdf_item *item, const char *prefix)
{
	assert(list != NULL);
	assert(item != NULL);

	if (parent == NULL)
		return false;

	struct xccdf_benchmark *bench = xccdf_item_get_benchmark(parent);

	if (bench != NULL) {
		if (xccdf_item_get_id(item) == NULL)
			item->item.id = xccdf_benchmark_gen_id(bench, xccdf_item_get_type(item), prefix);

		if (xccdf_benchmark_register_item(bench, item)) {
			item->item.parent = parent;
			return oscap_list_add(list, item);
		}
		else
			assert(false);
	}
	else return true;

	return false;
}

struct xccdf_item *
xccdf_benchmark_get_member(const struct xccdf_benchmark *benchmark, xccdf_type_t type, const char *key)
{
	return (struct xccdf_item *)oscap_htable_get(xccdf_benchmark_find_target_htable(benchmark, type), key);
}

static inline bool
_register_item_to_cluster(struct xccdf_benchmark *benchmark, struct xccdf_item *item)
{
	if (!oscap_streq(xccdf_item_get_cluster_id(item), "")) {
		struct oscap_htable *cluster = oscap_htable_get(XITEM(benchmark)->sub.benchmark.clusters_dict, xccdf_item_get_cluster_id(item));
		if (cluster == NULL) {
			cluster = oscap_htable_new();
			if (cluster == NULL || !oscap_htable_add(XITEM(benchmark)->sub.benchmark.clusters_dict, xccdf_item_get_cluster_id(item), cluster)) {
				oscap_htable_free0(cluster);
				return false;
			}
		}
		return oscap_htable_add(cluster, xccdf_item_get_id(item), item);
	}
	return true;
}

static inline bool
_unregister_item_from_cluster(struct xccdf_benchmark *benchmark, struct xccdf_item *item)
{
	if (!oscap_streq(xccdf_item_get_cluster_id(item), "")) {
		struct oscap_htable *cluster = oscap_htable_get(XITEM(benchmark)->sub.benchmark.clusters_dict, xccdf_item_get_cluster_id(item));
		return cluster != NULL && oscap_htable_detach(cluster, xccdf_item_get_cluster_id(item)) != NULL;
	}
	return true;
}

struct oscap_htable_iterator *
xccdf_benchmark_get_cluster_items(struct xccdf_benchmark *benchmark, const char *cluster_id)
{
	/* Get iterator through all items with given cluster-id */
	struct oscap_htable *cluster = oscap_htable_get(XITEM(benchmark)->sub.benchmark.clusters_dict, cluster_id);
	return (cluster == NULL) ? NULL : oscap_htable_iterator_new(cluster);
}

bool xccdf_benchmark_register_item(struct xccdf_benchmark *benchmark, struct xccdf_item *item)
{
	if (benchmark == NULL || item == NULL || xccdf_item_get_id(item) == NULL)
		return false;

	if (xccdf_item_get_type(item) == XCCDF_PROFILE) {
		struct xccdf_profile *profile = XPROFILE(item);

		// If the profile is a tailoring profile (== comes from Tailoring element),
		// we cannot register it to the Benchmark!
		if (xccdf_profile_get_tailoring(profile))
			return false;
	}

	const char *id = xccdf_item_get_id(item);
	struct xccdf_item *found = xccdf_benchmark_get_member(benchmark, xccdf_item_get_type(item), id);
	if (found != NULL) return found == item; // already registered

    if (item->type == XCCDF_GROUP) {
        OSCAP_FOR(xccdf_item, cnt, xccdf_group_get_content(XGROUP(item)))
            xccdf_benchmark_register_item(benchmark, cnt);
        OSCAP_FOR(xccdf_value, val, xccdf_group_get_values(XGROUP(item)))
            xccdf_benchmark_register_item(benchmark, XITEM(val));
    }

	return oscap_htable_add(xccdf_benchmark_find_target_htable(benchmark, xccdf_item_get_type(item)), xccdf_item_get_id(item), item) &&
		_register_item_to_cluster(benchmark, item);
}

bool xccdf_benchmark_unregister_item(struct xccdf_item *item)
{
	if (item == NULL) return false;

	struct xccdf_benchmark *bench = xccdf_item_get_benchmark(item);
	if (bench == NULL) return false;

	if (xccdf_item_get_type(item) == XCCDF_PROFILE) {
		struct xccdf_profile *profile = XPROFILE(item);

		// If the profile is a tailoring profile (== comes from Tailoring element),
		// we cannot unregister it from the Benchmark as it was never there!
		if (xccdf_profile_get_tailoring(profile))
			return false;
	}

	assert(xccdf_benchmark_get_member(bench, xccdf_item_get_type(item), xccdf_item_get_id(item)) == item);

	return oscap_htable_detach(xccdf_benchmark_find_target_htable(bench, xccdf_item_get_type(item)), xccdf_item_get_id(item)) != NULL &&
		_unregister_item_from_cluster(bench, item);
}

bool xccdf_benchmark_rename_item(struct xccdf_item *item, const char *newid)
{
	if (item == NULL)
		return false;

	struct xccdf_item *bench = XITEM(xccdf_item_get_benchmark(item));

	if (bench != NULL) {
		if (newid != NULL && xccdf_benchmark_get_member(XBENCHMARK(bench), xccdf_item_get_type(item), newid) != NULL)
			return false; // ID already assigned

		if (xccdf_item_get_id(item) != NULL)
			xccdf_benchmark_unregister_item(item);

		if (newid != NULL) {
			oscap_htable_add(xccdf_benchmark_find_target_htable(xccdf_item_get_benchmark(item), xccdf_item_get_type(item)), newid, item);
			_register_item_to_cluster(xccdf_item_get_benchmark(item), item);
		}
	}

	oscap_free(item->item.id);
	item->item.id = oscap_strdup(newid);

	return true;
}

/**
 * Find appropriate hashtable based on the type (xccdf type) of the item.
 * Note that IDs of xccdf:Profile and xccdf:Item are not guaranteed to not
 * overlap -> thus the need for separate hashtables.
 */
struct oscap_htable *
xccdf_benchmark_find_target_htable(const struct xccdf_benchmark *benchmark, xccdf_type_t type)
{
	assert(type & (XCCDF_ITEM | XCCDF_PROFILE | XCCDF_RESULT));
	if (type == XCCDF_PROFILE)
		return XITEM(benchmark)->sub.benchmark.profiles_dict;
	if (type == XCCDF_RESULT)
		return XITEM(benchmark)->sub.benchmark.results_dict;
	return XITEM(benchmark)->sub.benchmark.items_dict;
}


struct xccdf_plain_text *xccdf_plain_text_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_plain_text));
}

struct xccdf_plain_text *xccdf_plain_text_new_fill(const char *id, const char *text)
{
    struct xccdf_plain_text *plain = xccdf_plain_text_new();
    plain->id = oscap_strdup(id);
    plain->text = oscap_strdup(text);
    return plain;
}

static xmlNode *xccdf_plain_text_to_dom(const struct xccdf_plain_text *ptext, xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info* version_info)
{
	xmlNs *ns_xccdf = lookup_xccdf_ns(doc, parent, version_info);
	xmlNode *ptext_node = xmlNewTextChild(parent, ns_xccdf, BAD_CAST "plain-text",
			BAD_CAST (ptext->text == NULL ? "" : ptext->text));
	if (ptext->id != NULL)
		xmlNewProp(ptext_node, BAD_CAST "id", BAD_CAST ptext->id);
	return ptext_node;
}

struct xccdf_plain_text * xccdf_plain_text_clone(const struct xccdf_plain_text * pt)
{
    struct xccdf_plain_text *plain = oscap_calloc(1, sizeof(struct xccdf_plain_text));
    plain->id = oscap_strdup(pt->id);
    plain->text = oscap_strdup(pt->text);
    return plain;
}

void xccdf_plain_text_free(struct xccdf_plain_text *plain)
{
    if (plain != NULL) {
        oscap_free(plain->id);
        oscap_free(plain->text);
        oscap_free(plain);
    }
}

OSCAP_ACCESSOR_STRING(xccdf_plain_text, id)
OSCAP_ACCESSOR_STRING(xccdf_plain_text, text)
