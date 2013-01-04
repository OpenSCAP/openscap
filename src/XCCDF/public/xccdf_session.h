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
	oscap_document_type_t doc_type;		///< Document type of the session file (see filename member) used.
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

/// @}
/// @}
#endif
