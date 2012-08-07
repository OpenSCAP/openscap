/**
 * @addtogroup DS
 * @{
 *
 * @file ds.h
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
 * @brief checks whether given file contains a source data stream
 *
 * @param xccdf_file
 * 	   Path to the file we want checked
 *
 * @returns
 * 	   true if given file is *likely* a source data stream
 *
 * @par
 * This check is only "preliminary", it is designed to be reasonably fast
 * and won't do validation! It looks at the root element only.
 */
bool ds_is_sds(const char* xccdf_file);

/**
 * @brief takes given source data stream and decomposes it into separate files
 *
 * @param input_file
 *     File containing a datastream collection we want to decompose parts from
 *
 * @param id
 *     ID of a datastream we want to use from the given datastream
 *     collection. The first encountered datastream is used if id is NULL.
 *
 * @param target_dir
 *     Directory where the resulting files will be stored, names of the files
 *     are deduced using component-refs inside the datastream.
 *
 * @param xccdf_filename
 *     Base name of the target XCCDF file, if NULL is given the filename will
 *     be deduced from the contents of the datastream.
 */
void ds_sds_decompose(const char* input_file, const char* id,
        const char* target_dir, const char* xccdf_filename);

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
 */
void ds_sds_compose_from_xccdf(const char* xccdf_file, const char* target_datastream);

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
 */
void ds_rds_create(const char* sds_file, const char* xccdf_result_file,
        const char** oval_result_files, const char* target_file);

/************************************************************
 ** @} End of DS group */

#endif

