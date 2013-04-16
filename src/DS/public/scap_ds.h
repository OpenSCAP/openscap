/**
 * @addtogroup DS
 * @{
 *
 * @file scap_ds.h
 * Open-scap Data Stream interface.
 * @author Martin Preisler <mpreisle@redhat.com>
 */

/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifndef OPENSCAP_DS_H
#define OPENSCAP_DS_H

#include "oscap.h"

/**
 * @brief takes given source data stream and decomposes it into separate files
 *
 * This function bases the output on an XCCDF embedded in the "checklists"
 * element in the datastream. Most of the times this is what everybody wants.
 *
 * If you want to decompose just an OVAL file or some other custom behavior,
 * see ds_sds_decompose_custom.
 *
 * @param input_file
 *     File containing a datastream collection we want to decompose parts from
 *
 * @param id
 *     ID of a datastream we want to use from the given datastream
 *     collection. The first encountered datastream is used if id is NULL.
 *
 * @param xccdf_id
 *     ID of the component containing the XCCDF we want to process.
 *     The first encountered XCCDF component is used if id is NULL.
 *
 * @param target_dir
 *     Directory where the resulting files will be stored, names of the files
 *     are deduced using component-refs inside the datastream.
 *
 * @param target_filename
 *     Base name of the target XCCDF file, if NULL is given the filename will
 *     be deduced from the contents of the datastream.
 *
 * @returns
 * 	    0 if no errors were encountered
 * 	   -1 in case of errors
 */
int ds_sds_decompose(const char* input_file, const char* id, const char* xccdf_id,
		const char* target_dir, const char* target_filename);

/**
 * @brief same as ds_sds_decompose but works with other components than just XCCDFs
 *
 * @param container component reference container such as "checklists", "checks", ...
 *
 * @param component_id
 * 		id of the component you want to start the export from. If NULL, all
 * 		components refs inside given container will be exported.
 *
 * @param target_filename
 *     Base name of the target file, if NULL is given the filename will
 *     be deduced from the contents of the datastream.
 *
 * @see ds_sds_decompose
 */
int ds_sds_decompose_custom(const char* input_file, const char* id, const char* target_dir,
		const char* container_name, const char* component_id, const char* target_filename);

/**
 * @brief takes given xccdf file and constructs a source datastream
 *
 * @param xccdf_file
 *      Path to the XCCDF file that should be included in the datastream
 *      (including its dependencies).
 *
 * @param target_datastream
 *      ID of the datastream that should contain the XCCDF file. It will be
 *      the only datastream in the resulting data-stream-collection.
 *
 * @returns
 * 	    0 if no errors were encountered
 * 	   -1 in case of errors
 */
int ds_sds_compose_from_xccdf(const char* xccdf_file, const char* target_datastream);

/**
 * @brief append a new given component to the existing source datastream
 *
 * @param target_datastream
 * 	Path of the existing source datastream into which the new component
 * 	shall be added.
 *
 * @param datastream_id
 * 	ID of the datastream into which the component shall be added.
 * 	Null value indicates the very first datastream in the collection.
 *
 * @param new_component
 * 	Path to the new component file (XCCDF, OVAL, or CPE Dictionary).
 *
 * @returns 0 in case of success
 */
int ds_sds_compose_add_component(const char *target_datastream, const char *datastream_id, const char *new_component, bool extended);

/**
 * @brief takes given source data stream and XCCDF result file and makes a result data stream
 *
 * @param sds_file
 *      Path to the source data stream file that was used to generate the result XCCDF
 *
 * @param xccdf_result_file
 *      Contains xccdf:TestResult(s) and the embedded Benchmark (optionally).
 *      The embedded Benchmark (source data) will not be included in the result
 *      data stream, we will instead bundle the source data stream.
 *
 * @param oval_result_files
 *      NULL terminated list of paths to OVAL result files that should be bundled
 *      in the result data stream.
 *
 * @param target_file
 *      Path to the file where the result data stream will be stored
 *
 * @returns
 * 	    0 if no errors were encountered
 * 	   -1 in case of errors
 */
int ds_rds_create(const char* sds_file, const char* xccdf_result_file,
        const char** oval_result_files, const char* target_file);

/**
 * @struct ds_stream_index
 *
 * Contains information about one particular "<data-stream>" element in
 * the datastream collection (also called SDS = source datastream). Is
 * contained inside ds_sds_index which indexes the entire datastream collection.
 *
 * Only contains IDs, does not contain the data of components themselves!
 * See ds_sds_decompose for that.
 *
 * Inside it are components divided into categories called "containers".
 * These are "checks", "checklists", "dictionaries" and "extended-components".
 * See the specification for more details about their meaning.
 *
 * @see ds_sds_index
 */
struct ds_stream_index;

/// @memberof ds_stream_index
struct ds_stream_index* ds_stream_index_new(void);
/// @memberof ds_stream_index
void ds_stream_index_free(struct ds_stream_index* s);

/**
 * @brief Gets ID of the <data-stream> element the index represents.
 *
 * @memberof ds_stream_index
 */
const char* ds_stream_index_get_id(struct ds_stream_index* s);

/**
 * @brief Timestamp of creation OR modification of the <data-stream> element the index represents.
 *
 * @memberof ds_stream_index
 */
const char* ds_stream_index_get_timestamp(struct ds_stream_index* s);

/**
 * @brief scap-version of the the <data-stream> element the index represents.
 *
 * @memberof ds_stream_index
 */
const char* ds_stream_index_get_version(struct ds_stream_index* s);

/**
 * @brief Retrieves iterator over all components inside the <checks> element.
 *
 * @memberof ds_stream_index
 */
struct oscap_string_iterator* ds_stream_index_get_checks(struct ds_stream_index* s);

/**
 * @brief Retrieves iterator over all components inside the <checklists> element.
 *
 * @memberof ds_stream_index
 */
struct oscap_string_iterator* ds_stream_index_get_checklists(struct ds_stream_index* s);

/**
 * @brief Retrieves iterator over all components inside the <dictionaries> element.
 *
 * @memberof ds_stream_index
 */
struct oscap_string_iterator* ds_stream_index_get_dictionaries(struct ds_stream_index* s);

/**
 * @brief Retrieves iterator over all components inside the <extended-components> element.
 *
 * @memberof ds_stream_index
 */
struct oscap_string_iterator* ds_stream_index_get_extended_components(struct ds_stream_index* s);

/**
 * @struct ds_sds_index
 *
 * Represents <data-stream-collection> element - the root element of each
 * source datastream. Its purpose is to provide IDs and other metadata.
 *
 * Contains a list of ds_stream_index structures, each representing one
 * data-stream inside the collection.
 *
 * @see ds_stream_index
 */
struct ds_sds_index;

/// @memberof ds_sds_index
struct ds_sds_index* ds_sds_index_new(void);
/// @memberof ds_sds_index
void ds_sds_index_free(struct ds_sds_index* s);

/**
 * @brief retrieves a stream index by data-stream ID
 *
 * @memberof ds_sds_index
 */
struct ds_stream_index* ds_sds_index_get_stream(struct ds_sds_index* s, const char* stream_id);

/**
 * @brief retrieves all streams indexed inside this structure
 *
 * @memberof ds_sds_index
 */
struct ds_stream_index_iterator* ds_sds_index_get_streams(struct ds_sds_index* s);

/**
 * @brief imports given source datastream and indexes it
 *
 * @memberof ds_sds_index
 */
struct ds_sds_index *ds_sds_index_import(const char* file);

/**
 * @brief chooses datastream and checklist id combination given the IDs
 *
 * @param datastream_id has to point to an allocated char*, selected datastream ID will be filled into it
 * @param component_id has to point to an allocated char*, selected xccdf ID will be filled into it
 *
 * datastream_id and component_id must not point to the same memory! However,
 * the pointers pointed to may be NULL (which means any ID will do).
 */
int ds_sds_index_select_checklist(struct ds_sds_index* s,
		const char** datastream_id, const char** component_id);

/** 
 * @struct ds_stream_index_iterator
 * @see oscap_iterator
 */
struct ds_stream_index_iterator;

/// @memberof ds_stream_index_iterator
struct ds_stream_index *ds_stream_index_iterator_next(struct ds_stream_index_iterator *it);
/// @memberof ds_stream_index_iterator
bool ds_stream_index_iterator_has_more(struct ds_stream_index_iterator *it);
/// @memberof ds_stream_index_iterator
void ds_stream_index_iterator_free(struct ds_stream_index_iterator *it);

/**
 * @struct rds_report_request_index
 */
struct rds_report_request_index;

struct rds_report_request_index* rds_report_request_index_new(void);
void rds_report_request_index_free(struct rds_report_request_index* s);
const char* rds_report_request_index_get_id(struct rds_report_request_index* s);

/**
 * @struct rds_asset_index
 */
struct rds_asset_index;

struct rds_asset_index* rds_asset_index_new(void);
void rds_asset_index_free(struct rds_asset_index* s);
const char* rds_asset_index_get_id(struct rds_asset_index* s);

/**
 * @struct rds_report_index
 */
struct rds_report_index;

struct rds_report_index* rds_report_index_new(void);
void rds_report_index_free(struct rds_report_index* s);
const char* rds_report_index_get_id(struct rds_report_index* s);

/**
 * @struct rds_report_request_index_iterator
 * @see oscap_iterator
 */
struct rds_report_request_index_iterator;

/// @memberof rds_report_request_index_iterator
struct rds_report_request_index *rds_report_request_index_iterator_next(struct rds_report_request_index_iterator *it);
/// @memberof rds_report_request_index_iterator
bool rds_report_request_index_iterator_has_more(struct rds_report_request_index_iterator *it);
/// @memberof rds_report_request_index_iterator
void rds_report_request_index_iterator_free(struct rds_report_request_index_iterator *it);

/**
 * @struct rds_asset_index_iterator
 * @see oscap_iterator
 */
struct rds_asset_index_iterator;

/// @memberof rds_asset_index_iterator
struct rds_asset_index *rds_asset_index_iterator_next(struct rds_asset_index_iterator *it);
/// @memberof rds_asset_index_iterator
bool rds_asset_index_iterator_has_more(struct rds_asset_index_iterator *it);
/// @memberof rds_asset_index_iterator
void rds_asset_index_iterator_free(struct rds_asset_index_iterator *it);

/**
 * @struct rds_report_index_iterator
 * @see oscap_iterator
 */
struct rds_report_index_iterator;

/// @memberof rds_report_index_iterator
struct rds_report_index *rds_report_index_iterator_next(struct rds_report_index_iterator *it);
/// @memberof rds_report_index_iterator
bool rds_report_index_iterator_has_more(struct rds_report_index_iterator *it);
/// @memberof rds_report_index_iterator
void rds_report_index_iterator_free(struct rds_report_index_iterator *it);

/**
 * @struct rds_index
 *
 * Represents <asset-report-collection> element - the root element of each
 * result datastream in Asset Reporting Format = ARF.
 *
 * Indexes assets and report-requests.
 */
struct rds_index;

struct rds_index* rds_index_new(void);
void rds_index_free(struct rds_index* s);

struct rds_report_request_index_iterator* rds_index_get_report_requests(struct rds_index* s);
struct rds_asset_index_iterator* rds_index_get_assets(struct rds_index* s);
struct rds_report_index_iterator* rds_index_get_reports(struct rds_index* s);

struct rds_report_request_index* rds_index_get_report_request(struct rds_index* rds, const char* id);
struct rds_asset_index* rds_index_get_asset(struct rds_index* rds, const char* id);
struct rds_report_index* rds_index_get_report(struct rds_index* rds, const char* id);

struct rds_index *rds_index_import(const char* file);

/************************************************************/
/** @} End of DS group */

#endif
