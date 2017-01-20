/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
 * Author:
 *     Šimon Lukašík
 */
#ifndef OSCAP_SOURCE_H
#define OSCAP_SOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oscap.h"

/**
 * The oscap_source is low-level structure. The oscap_source shall hold
 * information regarding SCAP file/node needed for further processing by
 * OpenSCAP library.
 *
 * The oscap_source may refer to a DataStream, a component of datastream,
 * an XCCDF or OVAL file, or any other SCAP file. In rare cases it can
 * also refer to a non-XML file, even binary data!
 *
 * The oscap_source is primarily focused to solve problems regarding parsing
 * and serialization of SCAP files. The structure shall not know much about
 * relation to other sources.
 *
 * For handling relations between oscap_sources users are adviced to use
 * ds_sds_session. For example, if the oscap_source originated from DataStream
 * file there may be other oscap_sources that represent the components of
 * DataStream. These oscap_sources would be managed by ds_sds_session.
 *
 * The oscap_source keeps track of its origin (i.e. XCCDF file may originate
 * by openning an XCCDF file, extracting XCCDF section from DataStream or
 * by evaluation.
 *
 * The oscap_source is lazy structure, it only does its job when it has to.
 * The structure caches data whenever it is feasible.
 */
struct oscap_source;

/**
 * Create new oscap_source from filename. In the future this should be
 * the only function in the OpenSCAP library which will take in a path
 * to filename.
 * @param filepath Path to the file on a disk
 * @returns newly created oscap_source structure
 */
struct oscap_source *oscap_source_new_from_file(const char *filepath);

/**
 * Create new oscap_source from raw memory. The memory can contain \0 bytes
 * and they are not considered NULL-terminations! Always pass the correct
 * size. This constructor is meant as a last resort when no other constructor
 * will work for your use case. If at all possible you should use the more
 * specialized constructors.
 * oscap_source will always allocate its own memory buffer and copy the data.
 * You are responsible for freeing the original passed buffer.
 * @param buffer Memory buffer with raw data
 * @param size size of the memory buffer
 * @param filepath Suggested filename for the file or NULL
 * @returns newly created oscap_source_structure
 */
struct oscap_source *oscap_source_new_from_memory(const char *buffer, size_t size, const char *filepath);

/**
 * Clone oscap_source structure.
 * @param old Resource to clone
 * @returns A clone of the given oscap_source.
 */
struct oscap_source *oscap_source_clone(struct oscap_source *old);

/**
 * Dispose oscap_source structure.
 * @param source Resource to dispose
 */
void oscap_source_free(struct oscap_source *source);

/**
 * Get filepath of the given resource
 * @memberof oscap_source
 * @param source
 * @returns filepath of the original source or NULL
 */
const char *oscap_source_get_filepath(struct oscap_source *source);

/**
 * Get SCAP document type of the given resource
 * @memberof oscap_source
 * @param source
 * @returns determined document type
 *
 * @note
 * This function returns OSCAP_DOCUMENT_UNKNOWN to signal an error. Not being
 * able to determine a valid documnent type is treated as an error.
 */
oscap_document_type_t oscap_source_get_scap_type(struct oscap_source *source);

/**
 * Get the version of the schema for the particular document type
 * @memberof oscap_source
 * @param source The oscap_source to get the schema version from.
 * @returns the schema version
 */
const char *oscap_source_get_schema_version(struct oscap_source *source);

/**
 * Validate the SCAP document against particular XML schema definition.
 * @memberof oscap_source
 * @param source The oscap_source to validate
 * @note The held resource has to be XML for this function to work.
 * @returns 0 on pass; 1 on fail, and -1 on internal error
 */
int oscap_source_validate(struct oscap_source *source, xml_reporter reporter, void *user);

/**
 * Validate the SCAP document against schematron assertions
 * @memberof oscap_source
 * @param source The oscap_source to validate
 * @param outfile path to out file containing errors, NULL for stdout
 * @note The held resource has to be XML for this function to work.
 * @returns 0 on pass; 1 on fail, and -1 on internal error
 */
int oscap_source_validate_schematron(struct oscap_source *source, const char *outfile);

/**
 * Returns human readable description of oscap_source origin
 * @memberof oscap_source
 * @param source The oscap_source to get readable source from.
 * @returns human readable description
 */
const char *oscap_source_readable_origin(const struct oscap_source *source);

/**
 * Store the resource represented by oscap_source to the file.
 * @memberof oscap_source
 * @param source The oscap_source to save
 * @param filename The filename or NULL, the previously supplied name will
 * be used if filename is NULL.
 * @returns 0 on success, 1 or -1 to indicate error
 */
int oscap_source_save_as(struct oscap_source *source, const char *filename);

/**
 * Retrieve contents refered to by oscap_source as raw memory.
 * The memory is always copied. If the origin of oscap_source is raw memory,
 * this function will simply duplicate it and the operation is relatively cheap.
 * If however the origin is xmlDoc or an XML file this function has to serialize
 * it and then copy the results to given buffer. Keep in mind that this may be
 * performance intensive.
 * You are responsible for freeing the buffer.
 * @param buffer Will be filled with a pointer to a newly allocated buffer
 * @param size Will be filled with size of the buffer
 * @returns 0 on success, 1 otherwise
 */
int oscap_source_get_raw_memory(struct oscap_source *source, char **buffer, size_t *size);

#endif
