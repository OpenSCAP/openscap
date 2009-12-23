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

#include <string.h>
#include "item.h"

struct xccdf_backref {
	struct xccdf_item **ptr;	// pointer to a pointer that is supposed to be pointing to an item with id 'id'
	xccdf_type_t type;	// expected item type
	char *id;		// id
};

bool xccdf_benchmark_add_ref(struct xccdf_item *benchmark, struct xccdf_item **ptr, const char *id, xccdf_type_t type)
{
	assert(benchmark != NULL);
	if (ptr == NULL || id == NULL)
		return false;
	struct xccdf_backref *idref = oscap_calloc(1, sizeof(struct xccdf_backref));
	idref->ptr = ptr;
	idref->id = strdup(id);
	idref->type = type;
	oscap_list_add(benchmark->sub.bench.idrefs, idref);
	return true;
}

static bool xccdf_benchmark_resolve_refs(struct xccdf_item *bench)
{
	assert(bench->type == XCCDF_BENCHMARK);
	bool ret = true;
	struct oscap_list_item *refitem = bench->sub.bench.idrefs->first;
	for (refitem = bench->sub.bench.idrefs->first; refitem != NULL; refitem = refitem->next) {
		struct xccdf_backref *ref = refitem->data;
		struct xccdf_item *item;

		if (ref->type)
			item = oscap_htable_get(bench->sub.bench.dict, ref->id);
		else
			item = oscap_htable_get(bench->sub.bench.auxdict, ref->id);

		if (item == NULL || (ref->type != 0 && (item->type & ref->type) == 0)) {
			ret = false;
			continue;
		}
		*ref->ptr = item;
	}
	/// @todo detect dependency loops
	return ret;
}

struct xccdf_benchmark *xccdf_benchmark_parse_xml(const char *filename)
{
	xmlTextReaderPtr reader = xmlReaderForFile(filename, NULL, 0);
	if (!reader)
		return NULL;
	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != 1) ;
	struct xccdf_item *benchmark = xccdf_benchmark_new();
	xccdf_benchmark_parse(benchmark, reader);
	xmlFreeTextReader(reader);
	return XBENCHMARK(benchmark);
}

struct xccdf_item *xccdf_benchmark_new(void)
{
	struct xccdf_item *bench = xccdf_item_new(XCCDF_BENCHMARK, NULL, NULL);
	bench->sub.bench.notices = oscap_list_new();
	bench->sub.bench.models = oscap_list_new();
	bench->sub.bench.idrefs = oscap_list_new();
	bench->sub.bench.content = oscap_list_new();
	bench->sub.bench.values = oscap_list_new();
	bench->sub.bench.plain_texts = oscap_htable_new();
	bench->sub.bench.profiles = oscap_list_new();
	bench->sub.bench.dict = oscap_htable_new();
	bench->sub.bench.auxdict = oscap_htable_new();
	return bench;
}

bool xccdf_benchmark_parse(struct xccdf_item * benchmark, xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_BENCHMARK);
	assert(benchmark != NULL);
	if (benchmark->type != XCCDF_BENCHMARK)
		return false;

	if (!xccdf_item_process_attributes(benchmark, reader)) {
		xccdf_benchmark_free(XBENCHMARK(benchmark));
		return false;
	}
	benchmark->sub.bench.style = xccdf_attribute_copy(reader, XCCDFA_STYLE);
	benchmark->sub.bench.style_href = xccdf_attribute_copy(reader, XCCDFA_STYLE_HREF);
	if (xccdf_attribute_has(reader, XCCDFA_RESOLVED))
		benchmark->item.flags.resolved = xccdf_attribute_get_bool(reader, XCCDFA_RESOLVED);

	int depth = xccdf_element_depth(reader) + 1;

	while (xccdf_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_NOTICE:{
				const char *id = xccdf_attribute_get(reader, XCCDFA_ID);
				char *data = xccdf_get_xml(reader);
				if (data && id)
					oscap_list_add(benchmark->sub.bench.notices, xccdf_notice_new(id, data));
				break;
			}
		case XCCDFE_FRONT_MATTER:
			if (!benchmark->sub.bench.front_matter)
				benchmark->sub.bench.front_matter = xccdf_get_xml(reader);
			break;
		case XCCDFE_REAR_MATTER:
			if (!benchmark->sub.bench.rear_matter)
				benchmark->sub.bench.rear_matter = xccdf_get_xml(reader);
			break;
		case XCCDFE_METADATA:
			if (!benchmark->sub.bench.metadata)
				benchmark->sub.bench.metadata = xccdf_get_xml(reader);
			break;
		case XCCDFE_PLATFORM:
			oscap_list_add(benchmark->item.platforms, xccdf_attribute_copy(reader, XCCDFA_IDREF));
			break;
		case XCCDFE_MODEL:
			oscap_list_add(benchmark->sub.bench.models, xccdf_model_new_xml(reader));
			break;
		case XCCDFE_PLAIN_TEXT:{
				const char *id = xccdf_attribute_get(reader, XCCDFA_ID);
				char *data = xccdf_element_string_copy(reader);
				if (!id || !data || !oscap_htable_add(benchmark->sub.bench.plain_texts, id, data))
					oscap_free(data);
				break;
			}
		case XCCDFE_PROFILE:
			oscap_list_add(benchmark->sub.bench.profiles, xccdf_profile_parse(reader, benchmark));
			break;
		case XCCDFE_GROUP:
		case XCCDFE_RULE:
			xccdf_content_parse(reader, benchmark);
			break;
		case XCCDFE_VALUE:
			oscap_list_add(benchmark->sub.bench.values, xccdf_value_parse(reader, benchmark));
			break;
		default:
			xccdf_item_process_element(benchmark, reader);
		}
		xmlTextReaderRead(reader);
	}

	xccdf_benchmark_resolve_refs(benchmark);

	return true;
}

static void xccdf_backref_free(struct xccdf_backref *idref)
{
	if (idref) {
		oscap_free(idref->id);
		oscap_free(idref);
	}
}

void xccdf_benchmark_dump(struct xccdf_benchmark *benchmark)
{
	struct xccdf_item *bench = XITEM(benchmark);
	printf("Benchmark : %s\n", (bench ? bench->item.id : "(NULL)"));
	if (bench) {
		xccdf_item_print(bench, 1);
		printf("  front m.: ");
		xccdf_print_max(xccdf_benchmark_get_front_matter(benchmark), 64, "...");
		printf("\n");
		printf("  rear m. : ");
		xccdf_print_max(xccdf_benchmark_get_rear_matter(benchmark), 64, "...");
		printf("\n");
		printf("  profiles ");
		oscap_list_dump(bench->sub.bench.profiles, (oscap_dump_func) xccdf_profile_dump, 2);
		printf("  values");
		oscap_list_dump(bench->sub.bench.values, (oscap_dump_func) xccdf_value_dump, 2);
		printf("  content");
		oscap_list_dump(bench->sub.bench.content, (oscap_dump_func) xccdf_item_dump, 2);
	}
}

void xccdf_benchmark_free(struct xccdf_benchmark *benchmark)
{
	if (benchmark) {
		struct xccdf_item *bench = XITEM(benchmark);
		oscap_free(bench->sub.bench.style);
		oscap_free(bench->sub.bench.style_href);
		oscap_free(bench->sub.bench.front_matter);
		oscap_free(bench->sub.bench.rear_matter);
		oscap_free(bench->sub.bench.metadata);
		oscap_list_free(bench->sub.bench.notices, (oscap_destruct_func) xccdf_notice_free);
		oscap_list_free(bench->sub.bench.models, (oscap_destruct_func) xccdf_model_free);
		oscap_list_free(bench->sub.bench.idrefs, (oscap_destruct_func) xccdf_backref_free);
		oscap_list_free(bench->sub.bench.content, (oscap_destruct_func) xccdf_item_free);
		oscap_list_free(bench->sub.bench.values, (oscap_destruct_func) xccdf_value_free);
		oscap_htable_free(bench->sub.bench.plain_texts, oscap_free);
		oscap_htable_free(bench->sub.bench.dict, NULL);
		oscap_htable_free(bench->sub.bench.auxdict, NULL);
		oscap_list_free(bench->sub.bench.profiles, (oscap_destruct_func) xccdf_profile_free);
		xccdf_item_release(bench);
	}
}

XCCDF_BENCHMARK_GETTER(const char *, front_matter)
XCCDF_BENCHMARK_GETTER(const char *, rear_matter)
XCCDF_BENCHMARK_GETTER(const char *, metadata)
XCCDF_BENCHMARK_GETTER(const char *, style)
XCCDF_BENCHMARK_GETTER(const char *, style_href)
XCCDF_BENCHMARK_IGETTER(notice, notices)
XCCDF_BENCHMARK_IGETTER(model, models)
XCCDF_BENCHMARK_IGETTER(profile, profiles)
XCCDF_BENCHMARK_IGETTER(item, content)
XCCDF_ITERATOR_GEN_S(notice)
XCCDF_ITERATOR_GEN_S(model)
XCCDF_ITERATOR_GEN_S(profile)
XCCDF_HTABLE_GETTER(const char *, benchmark, plain_text, sub.bench.plain_texts)
XCCDF_HTABLE_GETTER(struct xccdf_item *, benchmark, item, sub.bench.dict) XCCDF_STATUS_CURRENT(benchmark)

struct xccdf_notice *xccdf_notice_new(const char *id, char *text)
{
	struct xccdf_notice *notice = oscap_calloc(1, sizeof(struct xccdf_notice));
	notice->id = strdup(id);
	notice->text = text;
	return notice;
}

void xccdf_notice_dump(struct xccdf_notice *notice, int depth)
{
	xccdf_print_depth(depth);
	printf("%.20s: ", xccdf_notice_get_id(notice));
	xccdf_print_max(xccdf_notice_get_text(notice), 50, "...");
	printf("\n");
}

void xccdf_notice_free(struct xccdf_notice *notice)
{
	if (notice) {
		oscap_free(notice->id);
		oscap_free(notice->text);
		oscap_free(notice);
	}
}

XCCDF_GENERIC_GETTER(const char *, notice, id) XCCDF_GENERIC_GETTER(const char *, notice, text)

void xccdf_cleanup(void)
{
	xmlCleanupParser();
}
