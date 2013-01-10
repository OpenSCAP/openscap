/**
 * @addtogroup XCCDF
 * @{
 * @addtogroup Session
 * @{
 *
 * @file xccdf_session.h
 * High Level API for OpenSCAP XCCDF operations.
 * @author Simon Lukasik <slukasik@redhat.com>i
 */

/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 */

#ifndef XCCDF_SESSION_H_
#define XCCDF_SESSION_H_

/**
 * @struct xccdf_session
 * A structure encapsuling the context of XCCDF operations.
 */
struct xccdf_session {
	// Note this has been made public only for refactoring purposes.
	// It will be removed later on.
	const char *filename;			///< File name of SCAP (SDS or XCCDF) file for this session.
	char *temp_dir;					///< Temp directory used for decomposed component files.
	struct {
		char *file;				///< Path to XCCDF File (shall differ from the filename for sds).
		struct xccdf_policy_model *policy_model;///< Active policy model.
		char *doc_version;			///< Version of parsed XCCDF file
	} xccdf;
	struct {
		struct ds_sds_index *sds_idx;		///< Index of Source DataStream (only applicable for sds).
		char *user_datastream_id;		///< Datastream id requested by user (only applicable for sds).
		char *user_component_id;		///< Component id requested by user (only applicable for sds).
		char *datastream_id;			///< Datastream id used (only applicable for sds).
		char *component_id;			///< Component id used (only applicable for sds).
	} ds;
	char *user_cpe;					///< Path to CPE dictionary required by user
	oscap_document_type_t doc_type;		///< Document type of the session file (see filename member) used.
	bool validate;				///< False value indicates to skip any XSD validation.
	bool full_validation;			///< True value indicates that every possible step will be validated by XSD.
};

/**
 * Costructor of xccdf_session. It attempts to recognize type of the filename.
 * @memberof xccdf_session
 * @param filename path to XCCDF or DS file.
 * @returns newly created \ref xccdf_session.
 * @retval NULL is returned in case of error. Details might be found through \ref oscap_err_desc()
 */
struct xccdf_session *xccdf_session_new(const char *filename);

/**
 * Destructor of xccdf_session.
 * @memberof xccdf_session
 * @param session to destroy.
 */
void xccdf_session_free(struct xccdf_session *session);

/**
 * Query if the session is based on Source DataStream.
 * @memberof xccdf_session
 * @returns true if the session is based on Source Datastream
 */
bool xccdf_session_is_sds(const struct xccdf_session *session);

/**
 * Set XSD validation level.
 * @memberof xccdf_session
 * @param validate False value indicates to skip any XSD validation.
 * @param full_validation True value indicates that every possible step will be validated by XSD.
 */
void xccdf_session_set_validation(struct xccdf_session *session, bool validate, bool full_validation);

/**
 * Set requested datastream_id for this session. This datastream_id is later
 * passed down to @ref ds_sds_index_select_checklist to determine target component.
 * This function is applicable only for sessions based on a DataStream.
 * @memberof xccdf_session
 * @param datastream_id requested datastream_id for this session.
 */
void xccdf_session_set_datastream_id(struct xccdf_session *session, const char *datastream_id);

/**
 * Set requested component_id for this session. This component_id is later
 * pased down to @ref ds_sds_index_select_checklist to determine target component.
 * This function is applicable only for sessions based on a DataStream.
 * @memberof xccdf_session
 * @param component_id requested component_id for this session.
 */
void xccdf_session_set_component_id(struct xccdf_session *session, const char *component_id);

/**
 * Set path to custom CPE dictionary for the session. This function is applicable
 * only before session loads. It has no effect if run afterwards.
 * @memberof xccdf_session
 * @param user_cpe File path to user defined cpe dictionary.
 */
void xccdf_session_set_user_cpe(struct xccdf_session *session, const char *user_cpe);

/**
 * Load and parse XCCDF file. If the file upon which is based this session is
 * Source DataStream use functions @ref xccdf_session_set_datastream_id and
 * @ref xccdf_session_set_component_id to select particular component within
 * that DataStream to parse. This function is reentrant meaning that it allows
 * user to change i.e. component_id and load_xccdf again in the very same session.
 * However in such case, previous xccdf structures will be deallocated from session
 * and pointers to it become invalid.
 * @memberof xccdf_session
 * @returns zero on success
 */
int xccdf_session_load_xccdf(struct xccdf_session *session);

/**
 * Load and parse CPE dictionaries. Function xccdf_session_set_user_cpe
 * might be called before this to set custom CPE dictionary.
 * @memberof xccdf_session
 * @returns zero on success
 */
int xccdf_session_load_cpe(struct xccdf_session *session);

/**
 * Get policy_model of the session. The @ref xccdf_session_load_xccdf shall be run
 * before this to parse XCCDF file to the policy_model.
 * @memberof xccdf_session
 * @returns XCCDF Policy Model or NULL in case of failure.
 */
struct xccdf_policy_model *xccdf_session_get_policy_model(const struct xccdf_session *session);

/// @}
/// @}
#endif
