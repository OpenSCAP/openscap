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


/**
 * @file
 * General OpenScap functions and types.
 * @author Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifndef OSCAP_H_
#define OSCAP_H_
#include <stdbool.h>


/**
 * @struct oscap_iterator
 * XCCDF iterator.
 *
 * Any iterator name takes a form of <tt>struct OBJECT_iterator</tt>, where @c OBJECT
 * is a name of a concrete datatype the iterator iterates over.
 */

/**
 * Iterate over an array, given an iterator.
 * Execute @a code for each array member stored in @a val.
 */
#define OSCAP_FOREACH_GENERIC(itype, vtype, val, init_val, code) \
    {                                                            \
        struct itype##_iterator *val##_iter = (init_val);        \
        vtype val;                                               \
        while (itype##_iterator_has_more(val##_iter)) {          \
            val = itype##_iterator_next(val##_iter);             \
            code                                                 \
        }                                                        \
        itype##_iterator_free(val##_iter);                       \
    }

/**
 * Iterate over an array, given an iterator.
 * @param type type of array elements (w/o the struct keyword)
 * @param val name of an variable the member will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @param code code to be executed for each element the iterator hits
 * @see OSCAP_FOREACH_GENERIC
 */
#define OSCAP_FOREACH(type, val, init_val, code) \
        OSCAP_FOREACH_GENERIC(type, struct type *, val, init_val, code)

/**
 * Iterate over an array of strings, given an iterator.
 * @param val name of an variable the string will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @param code code to be executed for each string the iterator hits
 * @see OSCAP_FOREACH_GENERIC
 */
#define OSCAP_FOREACH_STR(val, init_val, code) \
        OSCAP_FOREACH_GENERIC(oscap_string, const char *, val, init_val, code)

/* XML Metadata. Here should be every general attribute
 * that can be present in every xml element such as xml:lang
 * or xml namespace.
 * */
struct xml_metadata {
        char *namespace;    ///< XMLNS (namespace) prefix
        char *URI;          ///< XMLNS (namespace) URI
        char *lang;         ///< XML lang
};


struct xml_metadata * xml_metadata_new(void);

const char * xml_metadata_get_namespace(const struct xml_metadata * xml);
const char * xml_metadata_get_lang(const struct xml_metadata * xml);
const char * xml_metadata_get_URI(const struct xml_metadata * xml);

bool xml_metadata_set_namespace(struct xml_metadata * xml, const char *new_namespace);
bool xml_metadata_set_lang(struct xml_metadata * xml, const char *new_lang);
bool xml_metadata_set_URI(struct xml_metadata * xml, const char *new_uri);

void xml_metadata_free(struct xml_metadata * xml);

/** @struct xml_metadata_iterator
 * Iterator over XML metadata.
 * @see oscap_iterator
 * @relates xml_metadata
 */
struct xml_metadata_iterator;
/**
 * @relates xml_metadata_iterator
 */
struct xml_metadata * xml_metadata_iterator_next(struct xml_metadata_iterator * it);
/**
 * @relates xml_metadata_iterator
 */
bool xml_metadata_iterator_has_more(struct xml_metadata_iterator * it);
/**
 * @relates xml_metadata_iterator
 */
void xml_metadata_iterator_free(struct xml_metadata_iterator * it);

/**
 * @relates xml_metadata_iterator
 */
void xml_metadata_iterator_remove(struct xml_metadata_iterator * it);


/** @struct oscap_string_iterator
 * String iterator.
 * @see oscap_iterator
 */
struct oscap_string_iterator;
/// @relates oscap_string_iterator
const char* oscap_string_iterator_next(struct oscap_string_iterator* it);
/// @relates oscap_string_iterator
bool oscap_string_iterator_has_more(struct oscap_string_iterator* it);
/// @relates oscap_string_iterator
void oscap_string_iterator_free(struct oscap_string_iterator* it);
/// @relates oscap_string_iterator
void oscap_string_iterator_remove(struct oscap_string_iterator* it);


/**
 * @struct oscap_title
 * Title of an OpenScap element.
 */
struct oscap_title;

/**
 * Get oscap title content.
 * @relates oscap_title
 */
const char *oscap_title_get_content(const struct oscap_title *title);

/**
 * Get oscap title language.
 * @relates oscap_title
 */
const char *oscap_title_get_language(const struct oscap_title *title);

/**
 * Set oscap title content.
 * @relates oscap_title
 */
bool oscap_title_set_content(struct oscap_title *title, const char *new_content);

/**
 * @struct oscap_title_iterator
 * Iterator over an array of oscap_title elements.
 * @see oscap_iterator
 */
struct oscap_title_iterator;
/// @relates oscap_title_iterator
struct oscap_title* oscap_title_iterator_next(struct oscap_title_iterator* it);
/// @relates oscap_title_iterator
void oscap_title_iterator_free(struct oscap_title_iterator* it);
/// @relates oscap_title_iterator
bool oscap_title_iterator_has_more(struct oscap_title_iterator* it);
/// @relates oscap_title
void oscap_title_iterator_remove(struct oscap_title_iterator* it);

/**
 * Release library internal caches.
 *
 * This function should be called once you finish working with
 * any of the libraries included in OpenScap framework.
 * It frees internally allocated memory, e.g. cache of the XML parser.
 */
void oscap_cleanup(void);

/**
 */
struct oscap_export_target;
/**
 */
struct oscap_import_source;

typedef enum {
	OSCAP_STREAM_UNKNOWN = 0,
	OSCAP_STREAM_FILE = 1,
	OSCAP_STREAM_URL = 2,
} oscap_stream_type_t;

/**
 */
oscap_stream_type_t oscap_import_source_get_type(const struct oscap_import_source *item);

/**
 */
const char * oscap_import_source_get_name(const struct oscap_import_source *item);

/**
 */
oscap_stream_type_t oscap_export_target_get_type(const struct oscap_export_target *item);
/**
 * @relates oscap_export_target
 */
const char * oscap_export_target_get_name(const struct oscap_export_target *item);
/**
 * @relates oscap_export_target
 */
const char * oscap_export_target_get_encoding(const struct oscap_export_target *item);
/**
 * @relates oscap_export_target
 */
int oscap_export_target_get_indent(const struct oscap_export_target *item);
/**
 * @relates oscap_export_target
 */
const char * oscap_export_target_get_indent_string(const struct oscap_export_target *item);
/**
 * TODO
 */
struct oscap_import_source * oscap_import_source_new_file(const char * filename, const char *encoding);
/**
 * TODO
 */
struct oscap_import_source * oscap_import_source_new_URL(const char * url, const char *encoding);
/**
 * TODO
 */
void oscap_import_source_free(struct oscap_import_source * target);
/**
 * TODO
 */
struct oscap_export_target * oscap_export_target_new_file(const char * filename, const char * encoding);
/**
 * TODO
 */
struct oscap_export_target * oscap_export_target_new_URL(const char * url, const char * encoding);
/**
 * TODO
 */
void oscap_export_target_free(struct oscap_export_target * target);

#endif

