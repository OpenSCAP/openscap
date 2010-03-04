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
#include "../common/public/error.h"

#define XCCDF_SUPPORTED "1.1.4"

struct xccdf_backref {
	struct xccdf_item **ptr;	// pointer to a pointer that is supposed to be pointing to an item with id 'id'
	xccdf_type_t type;	// expected item type
	char *id;		// id
};

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
	struct xccdf_item *bench = xccdf_item_new(XCCDF_BENCHMARK, NULL);
    // lists
	bench->sub.bench.rear_matter  = oscap_list_new();
	bench->sub.bench.front_matter = oscap_list_new();
	bench->sub.bench.notices = oscap_list_new();
	bench->sub.bench.models = oscap_list_new();
	bench->sub.bench.content = oscap_list_new();
	bench->sub.bench.values = oscap_list_new();
	bench->sub.bench.plain_texts = oscap_htable_new();
	bench->sub.bench.profiles = oscap_list_new();
    // hash tables
	bench->sub.bench.dict = oscap_htable_new();
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

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_NOTICE:
				oscap_list_add(benchmark->sub.bench.notices, xccdf_notice_new_parse(reader));
				break;
		case XCCDFE_FRONT_MATTER:
				oscap_list_add(benchmark->sub.bench.front_matter, oscap_text_new_parse(XCCDF_TEXT_HTMLSUB, reader));
			break;
		case XCCDFE_REAR_MATTER:
				oscap_list_add(benchmark->sub.bench.rear_matter, oscap_text_new_parse(XCCDF_TEXT_HTMLSUB, reader));
			break;
		case XCCDFE_METADATA:
			if (!benchmark->sub.bench.metadata)
				benchmark->sub.bench.metadata = oscap_get_xml(reader);
			break;
		case XCCDFE_PLATFORM:
			oscap_list_add(benchmark->item.platforms, xccdf_attribute_copy(reader, XCCDFA_IDREF));
			break;
		case XCCDFE_MODEL:
			oscap_list_add(benchmark->sub.bench.models, xccdf_model_new_xml(reader));
			break;
		case XCCDFE_PLAIN_TEXT:{
				const char *id = xccdf_attribute_get(reader, XCCDFA_ID);
				char *data = oscap_element_string_copy(reader);
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

	return true;
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
		oscap_list_dump(bench->sub.bench.profiles, xccdf_profile_dump, 2);
		printf("  values");
		oscap_list_dump(bench->sub.bench.values, xccdf_value_dump, 2);
		printf("  content");
		oscap_list_dump(bench->sub.bench.content, xccdf_item_dump, 2);
	}
}

void xccdf_benchmark_free(struct xccdf_benchmark *benchmark)
{
	if (benchmark) {
		struct xccdf_item *bench = XITEM(benchmark);
		oscap_free(bench->sub.bench.style);
		oscap_free(bench->sub.bench.style_href);
		oscap_free(bench->sub.bench.metadata);
		oscap_list_free(bench->sub.bench.front_matter, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(bench->sub.bench.rear_matter, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(bench->sub.bench.notices, (oscap_destruct_func) xccdf_notice_free);
		oscap_list_free(bench->sub.bench.models, (oscap_destruct_func) xccdf_model_free);
		oscap_list_free(bench->sub.bench.content, (oscap_destruct_func) xccdf_item_free);
		oscap_list_free(bench->sub.bench.values, (oscap_destruct_func) xccdf_value_free);
		oscap_htable_free(bench->sub.bench.plain_texts, oscap_free);
		oscap_htable_free(bench->sub.bench.dict, NULL);
		oscap_list_free(bench->sub.bench.profiles, (oscap_destruct_func) xccdf_profile_free);
		xccdf_item_release(bench);
	}
}

XCCDF_TEXT_IGETTER(benchmark, front_matter, sub.bench.front_matter)
XCCDF_TEXT_IGETTER(benchmark, rear_matter, sub.bench.rear_matter)
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
XCCDF_HTABLE_GETTER(struct xccdf_item *, benchmark, item, sub.bench.dict) 
XCCDF_STATUS_CURRENT(benchmark)

struct xccdf_notice *xccdf_notice_new(void)
{
    struct xccdf_notice *notice = oscap_calloc(1, sizeof(struct xccdf_notice));
    notice->text = oscap_text_new_full(XCCDF_TEXT_NOTICE, NULL, NULL);
    return NULL;
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

XCCDF_GENERIC_GETTER(const char *, notice, id)
XCCDF_GENERIC_GETTER(struct oscap_text *, notice, text)

void xccdf_cleanup(void)
{
	xmlCleanupParser();
}

const char * xccdf_benchmark_supported(void)
{
    return XCCDF_SUPPORTED;
}

struct xccdf_benchmark *xccdf_benchmark_create(const char *id) 
{
    //TODO: not implemented
    oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_ENOTIMPL, "This feature is not implemented");
    return NULL;
}
struct xccdf_group *xccdf_benchmark_append_new_group(const struct xccdf_benchmark *benchmark, const char *id)
{
    //TODO: not implemented
    oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_ENOTIMPL, "This feature is not implemented");
    return NULL;
}
struct xccdf_value *xccdf_benchmark_append_new_value(const struct xccdf_benchmark *benchmark, const char *id, xccdf_value_type_t type)
{
    //TODO: not implemented
    oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_ENOTIMPL, "This feature is not implemented");
    return NULL;
}
struct xccdf_rule *xccdf_benchmark_append_new_rule(const struct xccdf_benchmark *benchmark, const char *id)
{
    //TODO: not implemented
    oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_ENOTIMPL, "This feature is not implemented");
    return NULL;
}
