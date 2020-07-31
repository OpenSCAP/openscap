/**
 * @addtogroup DS
 * @{
 *
 * @file scap_ds.h
 * Open-scap Data Stream interface.
 * @author Martin Preisler <mpreisle@redhat.com>
 */

/*
 * Copyright 2012--2014 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifndef OPENSCAP_DS_H
#define OPENSCAP_DS_H

#include "oscap.h"
#include "oscap_export.h"

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
OSCAP_API int ds_sds_compose_from_xccdf(const char* xccdf_file, const char* target_datastream);

/**
 * @brief takes given xccdf file and constructs a source datastream of given version
 *
 * @param xccdf_file
 *      Path to the XCCDF file that should be included in the datastream
 *      (including its dependencies).
 *
 * @param target_datastream
 *      ID of the datastream that should contain the XCCDF file. It will be
 *      the only datastream in the resulting data-stream-collection.
 *
 * @param version
 *      OCAP version
 *
 * @returns
 * 	    0 if no errors were encountered
 * 	   -1 in case of errors
 */
OSCAP_API int ds_sds_compose_from_xccdf_version(const char* xccdf_file, const char* target_datastream, oscap_document_version_t version);

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
OSCAP_API int ds_sds_compose_add_component(const char *target_datastream, const char *datastream_id, const char *new_component, bool extended);

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
OSCAP_API int ds_rds_create(const char* sds_file, const char* xccdf_result_file,
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
OSCAP_API struct ds_stream_index* ds_stream_index_new(void);
/// @memberof ds_stream_index
OSCAP_API void ds_stream_index_free(struct ds_stream_index* s);

/**
 * @brief Gets ID of the <data-stream> element the index represents.
 *
 * @memberof ds_stream_index
 */
OSCAP_API const char* ds_stream_index_get_id(struct ds_stream_index* s);

/**
 * @brief Timestamp of creation OR modification of the <data-stream> element the index represents.
 *
 * @memberof ds_stream_index
 */
OSCAP_API const char* ds_stream_index_get_timestamp(struct ds_stream_index* s);

/**
 * @brief scap-version of the the <data-stream> element the index represents.
 *
 * @memberof ds_stream_index
 */
OSCAP_API const char* ds_stream_index_get_version(struct ds_stream_index* s);

/**
 * @brief Retrieves iterator over all components inside the <checks> element.
 *
 * @memberof ds_stream_index
 */
OSCAP_API struct oscap_string_iterator* ds_stream_index_get_checks(struct ds_stream_index* s);

/**
 * @brief Retrieves iterator over all components inside the <checklists> element.
 *
 * @memberof ds_stream_index
 */
OSCAP_API struct oscap_string_iterator* ds_stream_index_get_checklists(struct ds_stream_index* s);

/**
 * @brief Retrieves iterator over all components inside the <dictionaries> element.
 *
 * @memberof ds_stream_index
 */
OSCAP_API struct oscap_string_iterator* ds_stream_index_get_dictionaries(struct ds_stream_index* s);

/**
 * @brief Retrieves iterator over all components inside the <extended-components> element.
 *
 * @memberof ds_stream_index
 */
OSCAP_API struct oscap_string_iterator* ds_stream_index_get_extended_components(struct ds_stream_index* s);

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
OSCAP_API struct ds_sds_index* ds_sds_index_new(void);
/// @memberof ds_sds_index
OSCAP_API void ds_sds_index_free(struct ds_sds_index* s);

/**
 * @brief retrieves a stream index by data-stream ID
 *
 * @memberof ds_sds_index
 */
OSCAP_API struct ds_stream_index* ds_sds_index_get_stream(struct ds_sds_index* s, const char* stream_id);

/**
 * @brief retrieves all streams indexed inside this structure
 *
 * @memberof ds_sds_index
 */
OSCAP_API struct ds_stream_index_iterator* ds_sds_index_get_streams(struct ds_sds_index* s);

/**
 * @brief chooses datastream and checklist id combination given the IDs
 *
 * @param datastream_id has to point to an allocated char*, selected datastream ID will be filled into it
 * @param component_id has to point to an allocated char*, selected xccdf ID will be filled into it
 *
 * datastream_id and component_id must not point to the same memory! However,
 * the pointers pointed to may be NULL (which means any ID will do).
 *
 * component_id is actually a component-ref ID, the reason is that we need the component-ref
 * to know which other components are in the catalog and thus needed when splitting.
 */
OSCAP_API int ds_sds_index_select_checklist(struct ds_sds_index* s,
		const char** datastream_id, const char** component_id);

/**
 * @brief chooses datastream and checklist component ref ID combination, given benchmark ID
 *
 * @param benchmark_id Which XCCDF Benchmark ID are we looking for?
 * Parameters are similar to ds_sds_index_select_checklist, except for benchmark_id.
 * @see ds_sds_index_select_checklist
 *
 * This function looks through to datastream index to find the first component-ref (top-down)
 * which satisfies the following condition:
 * "it does point to a component which contains XCCDF Benchmark with id
 * exactly the same as given benchmark_id".
 *
 * Please note that datastream_id AND component_ref_id are pointers to pointers and they
 * will be overwritten if 0 is returned. The values are never used, unlike with
 * ds_sds_index_select_checklist.
 */
OSCAP_API int ds_sds_index_select_checklist_by_benchmark_id(struct ds_sds_index* s,
		const char *benchmark_id, const char **datastream_id, const char **component_ref_id);

/** 
 * @struct ds_stream_index_iterator
 * @see oscap_iterator
 */
struct ds_stream_index_iterator;

/// @memberof ds_stream_index_iterator
OSCAP_API struct ds_stream_index *ds_stream_index_iterator_next(struct ds_stream_index_iterator *it);
/// @memberof ds_stream_index_iterator
OSCAP_API bool ds_stream_index_iterator_has_more(struct ds_stream_index_iterator *it);
/// @memberof ds_stream_index_iterator
OSCAP_API void ds_stream_index_iterator_free(struct ds_stream_index_iterator *it);

/**
 * @struct rds_report_request_index
 */
struct rds_report_request_index;

OSCAP_API struct rds_report_request_index* rds_report_request_index_new(void);
OSCAP_API void rds_report_request_index_free(struct rds_report_request_index* s);
OSCAP_API const char* rds_report_request_index_get_id(struct rds_report_request_index* s);

/**
 * @struct rds_asset_index
 */
struct rds_asset_index;

/**
 * @struct rds_report_index
 */
struct rds_report_index;

OSCAP_API struct rds_asset_index* rds_asset_index_new(void);
OSCAP_API void rds_asset_index_free(struct rds_asset_index* s);
OSCAP_API const char* rds_asset_index_get_id(struct rds_asset_index* s);
OSCAP_API void rds_asset_index_add_report_ref(struct rds_asset_index* s, struct rds_report_index* report);
OSCAP_API struct rds_report_index_iterator* rds_asset_index_get_reports(struct rds_asset_index* s);

OSCAP_API struct rds_report_index* rds_report_index_new(void);
OSCAP_API void rds_report_index_free(struct rds_report_index* s);
OSCAP_API const char* rds_report_index_get_id(struct rds_report_index* s);
OSCAP_API void rds_report_index_set_request(struct rds_report_index* s, struct rds_report_request_index *request);
OSCAP_API struct rds_report_request_index *rds_report_index_get_request(struct rds_report_index* s);

/**
 * @struct rds_report_request_index_iterator
 * @see oscap_iterator
 */
struct rds_report_request_index_iterator;

/// @memberof rds_report_request_index_iterator
OSCAP_API struct rds_report_request_index *rds_report_request_index_iterator_next(struct rds_report_request_index_iterator *it);
/// @memberof rds_report_request_index_iterator
OSCAP_API bool rds_report_request_index_iterator_has_more(struct rds_report_request_index_iterator *it);
/// @memberof rds_report_request_index_iterator
OSCAP_API void rds_report_request_index_iterator_free(struct rds_report_request_index_iterator *it);

/**
 * @struct rds_asset_index_iterator
 * @see oscap_iterator
 */
struct rds_asset_index_iterator;

/// @memberof rds_asset_index_iterator
OSCAP_API struct rds_asset_index *rds_asset_index_iterator_next(struct rds_asset_index_iterator *it);
/// @memberof rds_asset_index_iterator
OSCAP_API bool rds_asset_index_iterator_has_more(struct rds_asset_index_iterator *it);
/// @memberof rds_asset_index_iterator
OSCAP_API void rds_asset_index_iterator_free(struct rds_asset_index_iterator *it);

/**
 * @struct rds_report_index_iterator
 * @see oscap_iterator
 */
struct rds_report_index_iterator;

/// @memberof rds_report_index_iterator
OSCAP_API struct rds_report_index *rds_report_index_iterator_next(struct rds_report_index_iterator *it);
/// @memberof rds_report_index_iterator
OSCAP_API bool rds_report_index_iterator_has_more(struct rds_report_index_iterator *it);
/// @memberof rds_report_index_iterator
OSCAP_API void rds_report_index_iterator_free(struct rds_report_index_iterator *it);

/**
 * @struct rds_index
 *
 * Represents <asset-report-collection> element - the root element of each
 * result datastream in Asset Reporting Format = ARF.
 *
 * Indexes assets and report-requests.
 */
struct rds_index;

/// @memberof rds_index
OSCAP_API struct rds_index* rds_index_new(void);
/// @memberof rds_index
OSCAP_API void rds_index_free(struct rds_index *s);

/// @memberof rds_index
OSCAP_API struct rds_report_request_index_iterator *rds_index_get_report_requests(struct rds_index *s);
/// @memberof rds_index
OSCAP_API struct rds_asset_index_iterator *rds_index_get_assets(struct rds_index *s);
/// @memberof rds_index
OSCAP_API struct rds_report_index_iterator *rds_index_get_reports(struct rds_index *s);

/// @memberof rds_index
OSCAP_API struct rds_report_request_index *rds_index_get_report_request(struct rds_index *rds, const char *id);
/// @memberof rds_index
OSCAP_API struct rds_asset_index *rds_index_get_asset(struct rds_index *rds, const char *id);
/// @memberof rds_index
OSCAP_API struct rds_report_index *rds_index_get_report(struct rds_index *rds, const char *id);
/// @memberof rds_index
OSCAP_API int rds_index_select_report(struct rds_index *s, const char **report_id);

/************************************************************/
/** @} End of DS group */

#endif
