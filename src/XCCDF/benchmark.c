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
	struct xccdf_item** ptr; // pointer to a pointer that is supposed to be pointing to an item with id 'id'
    enum xccdf_type type;    // expected item type
	char* id;                // id
};

bool xccdf_benchmark_add_ref(struct xccdf_item* benchmark, struct xccdf_item** ptr, const char* id, enum xccdf_type type)
{
    assert(benchmark != NULL);
    if (ptr == NULL || id == NULL)
        return false;
    struct xccdf_backref* idref = calloc(1, sizeof(struct xccdf_backref));
    idref->ptr = ptr;
    idref->id = strdup(id);
    idref->type = type;
    xccdf_list_add(benchmark->sub.bench.idrefs, idref);
    return true;
}

bool xccdf_benchmark_resolve_refs(struct xccdf_item* bench)
{
	assert(bench->type == XCCDF_BENCHMARK);
    bool ret = true;
    struct xccdf_list_item* refitem = bench->sub.bench.idrefs->first;
    for (refitem = bench->sub.bench.idrefs->first; refitem != NULL; refitem = refitem->next) {
        struct xccdf_backref* ref = refitem->data;
        struct xccdf_item* item;
		
		if (ref->type) item = xccdf_htable_get(bench->sub.bench.dict, ref->id);
		else item = xccdf_htable_get(bench->sub.bench.auxdict, ref->id);

        if (item == NULL || (ref->type != 0 && (item->type & ref->type) == 0)) {
            ret = false;
            continue;
        }
        *ref->ptr = item;
    }
    /// @todo detect dependency loops
    return ret;
}

struct xccdf_benchmark* xccdf_benchmark_new_from_file(const char* filename)
{
	xmlTextReaderPtr reader = xmlReaderForFile(filename, NULL, 0);
    if (!reader) return NULL;
	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != 1);
	struct xccdf_item* benchmark = xccdf_benchmark_new_empty();
	xccdf_benchmark_parse(benchmark, reader);
	xmlFreeTextReader(reader);
    return XBENCHMARK(benchmark);
}

struct xccdf_item* xccdf_benchmark_new_empty(void)
{
	struct xccdf_item* bench = xccdf_item_new(XCCDF_BENCHMARK, NULL, NULL);
	bench->sub.bench.notices = xccdf_list_new();
	bench->sub.bench.models = xccdf_list_new();
	bench->sub.bench.idrefs = xccdf_list_new();
	bench->sub.bench.content = xccdf_list_new();
	bench->sub.bench.values = xccdf_list_new();
    bench->sub.bench.plain_texts = xccdf_htable_new();
	bench->sub.bench.profiles = xccdf_list_new();
    bench->sub.bench.dict = xccdf_htable_new();
    bench->sub.bench.auxdict = xccdf_htable_new();
	return bench;
}

bool xccdf_benchmark_parse(struct xccdf_item* benchmark, xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_BENCHMARK);
	assert(benchmark != NULL);
	if (benchmark->type != XCCDF_BENCHMARK)
		return false;
	
	if (!xccdf_item_process_attributes(benchmark, reader)) {
		xccdf_benchmark_delete(XBENCHMARK(benchmark));
		return false;
	}
	benchmark->sub.bench.style = xccdf_attribute_copy(reader, XCCDFA_STYLE);
	benchmark->sub.bench.style_href = xccdf_attribute_copy(reader, XCCDFA_STYLE_HREF);
	if (xccdf_attribute_has(reader, XCCDFA_RESOLVED))
		benchmark->item.flags.resolved = xccdf_attribute_get_bool(reader, XCCDFA_RESOLVED);

	int depth = xccdf_element_depth(reader) + 1;

	while (xccdf_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
			case XCCDFE_NOTICE: {
                const char* id = xccdf_attribute_get(reader, XCCDFA_ID);
				char* data = xccdf_get_xml(reader);
                if (data && id)
                    xccdf_list_add(benchmark->sub.bench.notices, xccdf_notice_new(id, data));
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
                xccdf_list_add(benchmark->item.platforms, xccdf_attribute_copy(reader, XCCDFA_IDREF));
                break;
            case XCCDFE_MODEL:
                xccdf_list_add(benchmark->sub.bench.models, xccdf_model_new_xml(reader));
                break;
            case XCCDFE_PLAIN_TEXT: {
                const char* id = xccdf_attribute_get(reader, XCCDFA_ID);
                char* data = xccdf_element_string_copy(reader);
                if (!id || !data || !xccdf_htable_add(benchmark->sub.bench.plain_texts, id, data)) free(data);
                break;
            }
            case XCCDFE_PROFILE:
                xccdf_list_add(benchmark->sub.bench.profiles, xccdf_profile_new_parse(reader, benchmark));
                break;
			case XCCDFE_GROUP: case XCCDFE_RULE:
				xccdf_content_parse(reader, benchmark);
				break;
			case XCCDFE_VALUE:
				xccdf_list_add(benchmark->sub.bench.values, xccdf_value_new_parse(reader, benchmark));
				break;
			default: xccdf_item_process_element(benchmark, reader);
		}
		xmlTextReaderRead(reader);
	}

    xccdf_benchmark_resolve_refs(benchmark);

	return true;
}

void xccdf_backref_delete(struct xccdf_backref* idref)
{
    if (idref) {
        free(idref->id);
        free(idref);
    }
}

void xccdf_benchmark_dump(struct xccdf_benchmark* benchmark)
{
	struct xccdf_item* bench = XITEM(benchmark);
	printf("Benchmark : %s\n", (bench ? bench->item.id : "(NULL)"));
	if (bench) {
        xccdf_item_print(bench, 1);
        printf("  front m.: "); xccdf_print_max(xccdf_benchmark_front_matter(benchmark), 64, "..."); printf("\n");
        printf("  rear m. : "); xccdf_print_max(xccdf_benchmark_rear_matter(benchmark), 64, "..."); printf("\n");
        printf("  profiles "); xccdf_list_dump(bench->sub.bench.profiles, (xccdf_dump_func)xccdf_profile_dump, 2);
		printf("  values"); xccdf_list_dump(bench->sub.bench.values, (xccdf_dump_func)xccdf_value_dump, 2);
		printf("  content"); xccdf_list_dump(bench->sub.bench.content, (xccdf_dump_func)xccdf_item_dump, 2);
	}
}

void xccdf_benchmark_delete(struct xccdf_benchmark* benchmark)
{
	if (benchmark) {
		struct xccdf_item* bench = XITEM(benchmark);
		free(bench->sub.bench.style);
		free(bench->sub.bench.style_href);
		free(bench->sub.bench.front_matter);
		free(bench->sub.bench.rear_matter);
		free(bench->sub.bench.metadata);
		xccdf_list_delete(bench->sub.bench.notices, (xccdf_destruct_func)xccdf_notice_delete);
		xccdf_list_delete(bench->sub.bench.models, (xccdf_destruct_func)xccdf_model_delete);
		xccdf_list_delete(bench->sub.bench.idrefs, (xccdf_destruct_func)xccdf_backref_delete);
		xccdf_list_delete(bench->sub.bench.content, (xccdf_destruct_func)xccdf_item_delete);
		xccdf_list_delete(bench->sub.bench.values, (xccdf_destruct_func)xccdf_value_delete);
        xccdf_htable_delete(bench->sub.bench.plain_texts, free);
        xccdf_htable_delete(bench->sub.bench.dict, NULL);
		xccdf_htable_delete(bench->sub.bench.auxdict, NULL);
		xccdf_list_delete(bench->sub.bench.profiles, (xccdf_destruct_func)xccdf_profile_delete);
		xccdf_item_release(bench);
	}
}

XCCDF_BENCHMARK_GETTER(const char*, front_matter)
XCCDF_BENCHMARK_GETTER(const char*, rear_matter)
XCCDF_BENCHMARK_GETTER(const char*, metadata)
XCCDF_BENCHMARK_IGETTER(notice, notices)
XCCDF_BENCHMARK_IGETTER(model, models)
XCCDF_BENCHMARK_IGETTER(profile, profiles)
XCCDF_BENCHMARK_IGETTER(item, content)
XCCDF_ITERATOR_GEN_S(notice)
XCCDF_ITERATOR_GEN_S(model)
XCCDF_ITERATOR_GEN_S(profile)
XCCDF_HTABLE_GETTER(const char*,benchmark,plain_text,sub.bench.plain_texts)
XCCDF_HTABLE_GETTER(struct xccdf_item*,benchmark,item,sub.bench.dict)
XCCDF_STATUS_CURRENT(benchmark)

struct xccdf_notice* xccdf_notice_new(const char* id, char* text)
{
    struct xccdf_notice* notice = calloc(1, sizeof(struct xccdf_notice));
    notice->id = strdup(id);
    notice->text = text;
    return notice;
}

void xccdf_notice_dump(struct xccdf_notice* notice, int depth)
{
    xccdf_print_depth(depth);
    printf("%.20s: ", xccdf_notice_id(notice));
    xccdf_print_max(xccdf_notice_text(notice), 50, "...");
    printf("\n");
}

void xccdf_notice_delete(struct xccdf_notice* notice)
{
    if (notice) {
        free(notice->id);
        free(notice->text);
        free(notice);
    }
}

XCCDF_GENERIC_GETTER(const char*, notice, id)
XCCDF_GENERIC_GETTER(const char*, notice, text)

void xccdf_cleanup(void)
{
	xmlCleanupParser();
}

