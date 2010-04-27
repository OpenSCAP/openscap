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
 *
 * @addtogroup COMMON
 * @{
 */

#ifndef OSCAP_H_
#define OSCAP_H_
#include <stdbool.h>
#include <wchar.h>

#include "text.h"
#include "reporter.h"

/**
 * @addtogroup ITER
 * @{
 *
 * Iterators concept.
 *
 * Any iterator name takes a form of <tt>struct OBJECT_iterator</tt>, where @c OBJECT
 * is a name of particular datatype the iterator iterates over.
 *
 * Each iterator type defines several manipulation functions, namely:
 * - @c OBJECT_iterator_has_more - returns true if there is anything left to iterate over
 * - @c OBJECT_iterator_next     - returns next item in the collection
 * - @c OBJECT_iterator_free     - destroys the iterator
 *
 * You can also use @ref OSCAP_FOREACH convience macro.
 */

/**
 * Iterate over an array, given an iterator.
 * Execute @a code for each array member stored in @a val.
 * It is NOT safe to use return or goto inside of the @a code
 * or iterator will not be freed properly.
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

/**
 * Iterate over an array, given an iterator.
 * It is generally not safe to use break, return or goto inside the loop
 * (iterator wouldn't be properly freed otherwise).
 * Two variables, named VAL and VAL_iter (substitute VAL for actual macro argument)
 * will be added to current variable scope. You can free the iterator explicitly
 * after previous unusual escape from the loop (e.g. using break).
 * @param val name of an variable the string will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @param code code to be executed for each string the iterator hits
 */
#define OSCAP_FOR_GENERIC(itype, vtype, val, init_val)                  \
    vtype val = NULL; struct itype##_iterator *val##_iter = (init_val); \
    while (itype##_iterator_has_more(val##_iter)                        \
            ? (val = itype##_iterator_next(val##_iter), true)           \
            : (itype##_iterator_free(val##_iter), val##_iter = NULL, false))

/**
 * Iterate over an array, given an iterator.
 * @param type type of array elements (w/o the struct keyword)
 * @param val name of an variable the member will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @see OSCAP_FOR_GENERIC
 */
#define OSCAP_FOR(type, val, init_val) OSCAP_FOR_GENERIC(type, struct type *, val, init_val)

/**
 * Iterate over an array of strings, given an iterator.
 * @param val name of an variable the member will be sequentially stored in
 * @param init_val initial member value (i.e. an iterator pointing to the start element)
 * @see OSCAP_FOR_GENERIC
 */
#define OSCAP_FOR_STR(val, init_val) OSCAP_FOR_GENERIC(oscap_string, const char *, val, init_val)

/** @} */

/**
 * @defgroup XMLMETA Element metadata
 * @{
 * Describes XML elemenent metadata such as namespace or language.
 */

/**
 * @struct xml_metadata
 * XML Metadata.
 *
 * Here should be every general attribute that can be present
 * in every xml element such as xml:lang or xml namespace.
 */
struct xml_metadata;

/** 
 * @struct xml_metadata_iterator
 * Iterator over XML metadata.
 * @see oscap_iterator
 */
struct xml_metadata_iterator;

/**
 * @name New functions
 * Constructors of XML metadata model structures. Free function returns new empty allocated structure.
 * If returns non NULL it need to be freed by the caller.
 * @{
 * */

/** 
 * Constructor of XML metadata
 * @memberof xml_metadata
 */
struct xml_metadata *xml_metadata_new(void);

/**
 * @}
 */

/**
 * @name Get functions
 * Functions for getting attributes from XML metadata model structure. Return value is pointer to structure's member. Do not 
 * free unless you null the pointer in the structure. Use remove function otherwise.
 * @{
 * */

/** 
 * Get namespace attribute from XML metadata
 * @memberof xml_metadata
 */
const char *xml_metadata_get_nspace(const struct xml_metadata *xml);

/** 
 * Get xml:lang attribute from XML metadata
 * @memberof xml_metadata
 */
const char *xml_metadata_get_lang(const struct xml_metadata *xml);

/** 
 * Get URI attribute from XML metadata
 * @memberof xml_metadata
 */
const char *xml_metadata_get_URI(const struct xml_metadata *xml);

/**
 * @}
 */

/**
 * @name Add and Set functions
 * Set functions assign values to members of structures except lists. For lists use add functions. 
 * Parameters of set functions are duplicated in memory and need to be freed by caller.
 * @{
 */

/** 
 * Set namespace attribute of XML metadata
 * @memberof xml_metadata
 */
bool xml_metadata_set_nspace(struct xml_metadata *xml, const char *new_namespace);

/** 
 * Set xml:lang attribute of XML metadata
 * @memberof xml_metadata
 */
bool xml_metadata_set_lang(struct xml_metadata *xml, const char *new_lang);

/** 
 * Set URI attribute of XML metadata
 * @memberof xml_metadata
 */
bool xml_metadata_set_URI(struct xml_metadata *xml, const char *new_uri);

/**
 * @}
 */

/**
 * @name Free functions
 * Destructors of XML metadata model structures. Functions free structures with all members recursively. 
 * For simple deletion of entity use remove functions.
 * @{
 */

/** 
 * Destructor of XML metadata structure
 * @memberof xml_metadata
 */
void xml_metadata_free(struct xml_metadata *xml);

/**
 * @}
 */


/**
 * @name Iterator functions
 * Functions to iterate throught lists.
 * @{
 * */

/// @memberof xml_metadata_iterator
struct xml_metadata *xml_metadata_iterator_next(struct xml_metadata_iterator *it);

/// @memberof xml_metadata_iterator
bool xml_metadata_iterator_has_more(struct xml_metadata_iterator *it);

/// @memberof xml_metadata_iterator
void xml_metadata_iterator_free(struct xml_metadata_iterator *it);

/// @memberof xml_metadata_iterator
void xml_metadata_iterator_remove(struct xml_metadata_iterator *it);

/**
 * @}
 */

/** @} */

/**
 * @addtogroup STRINGS
 * @{
 * Functions to access and manipulate textual data.
 */

/**
 * @struct oscap_string_iterator
 * String iterator.
 *
 * An iterator iterating over a collection of char*-s.
 * @see oscap_iterator
 */
struct oscap_string_iterator;
/// @memberof oscap_string_iterator
const char *oscap_string_iterator_next(struct oscap_string_iterator *it);
/// @memberof oscap_string_iterator
bool oscap_string_iterator_has_more(struct oscap_string_iterator *it);
/// @memberof oscap_string_iterator
void oscap_string_iterator_free(struct oscap_string_iterator *it);
/// @memberof oscap_string_iterator
void oscap_string_iterator_remove(struct oscap_string_iterator *it);

/**
 * @struct oscap_stringlist
 * A collection of strings.
 */
struct oscap_stringlist;
/// @memberof oscap_stringlist
struct oscap_string_iterator *oscap_stringlist_get_strings(const struct oscap_stringlist* list);
/// @memberof oscap_stringlist
bool oscap_stringlist_add_string(struct oscap_stringlist* list, const char *str);

/**
 * @struct oscap_stringlist_iterator
 * Iterator over collections of strings.
 * @see oscap_iterator
 */
struct oscap_stringlist_iterator;
/// @memberof oscap_stringlist_iterator
struct oscap_stringlist *oscap_stringlist_iterator_next(struct oscap_stringlist_iterator *it);
/// @memberof oscap_stringlist_iterator
bool oscap_stringlist_iterator_has_more(struct oscap_stringlist_iterator *it);
/// @memberof oscap_stringlist_iterator
void oscap_stringlist_iterator_free(struct oscap_stringlist_iterator *it);
/// @memberof oscap_stringlist_iterator
void oscap_stringlist_iterator_remove(struct oscap_stringlist_iterator *it);

/**
 * @struct oscap_title
 * Title of an OpenScap element.
 */
struct oscap_title;

/**
 * Get oscap title content.
 * @memberof oscap_title
 */
const char *oscap_title_get_content(const struct oscap_title *title);

/**
 * Get oscap title language.
 * @memberof oscap_title
 */
const char *oscap_title_get_language(const struct oscap_title *title);

/**
 * Set oscap title content.
 * @memberof oscap_title
 */
bool oscap_title_set_content(struct oscap_title *title, const char *new_content);

/**
 * @struct oscap_title_iterator
 * Iterator over an array of oscap_title elements.
 * @see oscap_iterator
 */
struct oscap_title_iterator;
/// @memberof oscap_title_iterator
struct oscap_title *oscap_title_iterator_next(struct oscap_title_iterator *it);
/// @memberof oscap_title_iterator
void oscap_title_iterator_free(struct oscap_title_iterator *it);
/// @memberof oscap_title_iterator
bool oscap_title_iterator_has_more(struct oscap_title_iterator *it);
/// @memberof oscap_title
void oscap_title_iterator_remove(struct oscap_title_iterator *it);

/** @} */

/**
 * Release library internal caches.
 *
 * This function should be called once you finish working with
 * any of the libraries included in OpenScap framework.
 * It frees internally allocated memory, e.g. cache of the XML parser.
 */
void oscap_cleanup(void);


/**
 * @addtogroup IMPORTEXPORT
 * @{
 * Handling input and output files plus validation according to XML schema.
 */

/**
 * @struct oscap_export_target
 */
struct oscap_export_target;
/**
 * @struct oscap_import_source
 */
struct oscap_import_source;

/**
 * Stream type
 */
typedef enum {
	OSCAP_STREAM_UNKNOWN = 0,
	OSCAP_STREAM_FILE = 1,
	OSCAP_STREAM_URL = 2,
} oscap_stream_type_t;

/**
 * Get type of this import source.
 * @memberof oscap_import_source
 */
oscap_stream_type_t oscap_import_source_get_type(const struct oscap_import_source *item);

/**
 * Get name of this import source.
 * @memberof oscap_import_source
 */
const char *oscap_import_source_get_name(const struct oscap_import_source *item);

/**
 * Get type of this export target.
 * @memberof oscap_export_target
 */
oscap_stream_type_t oscap_export_target_get_type(const struct oscap_export_target *item);

/**
 * Get name of this export target.
 * @memberof oscap_export_target
 */
const char *oscap_export_target_get_name(const struct oscap_export_target *item);

/**
 * Get encoding for this export target.
 * @memberof oscap_export_target
 */
const char *oscap_export_target_get_encoding(const struct oscap_export_target *item);

/**
 * Get indentation for this export target.
 * @memberof oscap_export_target
 */
int oscap_export_target_get_indent(const struct oscap_export_target *item);

/**
 * Get indentation string for this export target.
 * @memberof oscap_export_target
 */
const char *oscap_export_target_get_indent_string(const struct oscap_export_target *item);

/**
 * Function returns new import source structure, or NULL if an error occurred.
 * @memberof oscap_import_source
 */
struct oscap_import_source *oscap_import_source_new_file(const char *filename, const char *encoding);

/**
 * Function returns new import source structure, or NULL if an error occurred.
 * @memberof oscap_import_source
 */
struct oscap_import_source *oscap_import_source_new_URL(const char *url, const char *encoding);

/**
 * Destroy this import source structure.
 * @memberof oscap_import_source
 */
void oscap_import_source_free(struct oscap_import_source *target);

/**
 * Function returns new export target structure, or NULL if an error occurred.
 * @memberof oscap_export_target
 */
struct oscap_export_target *oscap_export_target_new_file(const char *filename, const char *encoding);

/**
 * Function returns new export target structure, or NULL if an error occurred.
 * @memberof oscap_export_target
 */
struct oscap_export_target *oscap_export_target_new_URL(const char *url, const char *encoding);

/**
 * Destroy this export target structure.
 * @memberof oscap_export_target
 */
void oscap_export_target_free(struct oscap_export_target *target);



/**
 * @struct oscap_nsinfo_entry_iterator
 * Namespace info iterator
 * @see oscap_iterator
 */
struct oscap_nsinfo_entry_iterator;
/// @memberof oscap_nsinfo_entry_iterator
bool oscap_nsinfo_entry_iterator_has_more(struct oscap_nsinfo_entry_iterator *it);
/// @memberof oscap_nsinfo_entry_iterator
struct oscap_nsinfo_entry *oscap_nsinfo_entry_iterator_next(struct oscap_nsinfo_entry_iterator *it);
/// @memberof oscap_nsinfo_entry_iterator
void oscap_nsinfo_entry_iterator_free(struct oscap_nsinfo_entry_iterator *it);

/**
 * @struct oscap_nsinfo_entry
 * Namespace information entry.
 * This structure carries namespace prefix,
 * Namespace URI and schema location.
 */
struct oscap_nsinfo_entry;
/// @memberof oscap_nsinfo_entry
struct oscap_nsinfo_entry *oscap_nsinfo_entry_new(void);
/// @memberof oscap_nsinfo_entry
struct oscap_nsinfo_entry *oscap_nsinfo_entry_new_fill(const char *nsprefix, const char *nsname);
/// @memberof oscap_nsinfo_entry
void oscap_nsinfo_entry_free(struct oscap_nsinfo_entry *entry);
/// @memberof oscap_nsinfo_entry
const char *oscap_nsinfo_entry_get_nsname(const struct oscap_nsinfo_entry *item);
/// @memberof oscap_nsinfo_entry
bool oscap_nsinfo_entry_set_nsname(struct oscap_nsinfo_entry *obj, const char *newval);
/// @memberof oscap_nsinfo_entry
const char *oscap_nsinfo_entry_get_nsprefix(const struct oscap_nsinfo_entry *item);
/// @memberof oscap_nsinfo_entry
bool oscap_nsinfo_entry_set_nsprefix(struct oscap_nsinfo_entry *obj, const char *newval);
/// @memberof oscap_nsinfo_entry
const char *oscap_nsinfo_entry_get_schema_location(const struct oscap_nsinfo_entry *item);
/// @memberof oscap_nsinfo_entry
bool oscap_nsinfo_entry_set_schema_location(struct oscap_nsinfo_entry *obj, const char *newval);

/**
 * @struct oscap_nsinfo
 * Information on namespaces for given document
 */
struct oscap_nsinfo;
/// @memberof oscap_nsinfo
struct oscap_nsinfo *oscap_nsinfo_new(void);
/// @memberof oscap_nsinfo
struct oscap_nsinfo *oscap_nsinfo_new_file(const char *fname);
/// @memberof oscap_nsinfo
void oscap_nsinfo_free(struct oscap_nsinfo *info);
/// @memberof oscap_nsinfo
struct oscap_nsinfo_entry_iterator *oscap_nsinfo_get_entries(const struct oscap_nsinfo *item);
/// @memberof oscap_nsinfo
bool oscap_nsinfo_add_entry(struct oscap_nsinfo *obj, struct oscap_nsinfo_entry *item);
/// @memberof oscap_nsinfo
struct oscap_nsinfo_entry *oscap_nsinfo_get_root_entry(const struct oscap_nsinfo *item);
/// @memberof oscap_nsinfo
bool oscap_nsinfo_set_root_entry(struct oscap_nsinfo *obj, struct oscap_nsinfo_entry *newval);
/// @memberof oscap_nsinfo
struct oscap_nsinfo_entry *oscap_nsinfo_get_entry_by_ns(struct oscap_nsinfo *info, const char *ns);

/// validate a xml file against given xml schema
bool oscap_validate_xml(const char *xmlfile, const char *schemafile, struct oscap_reporter* reporter);

/** @} */

#endif
