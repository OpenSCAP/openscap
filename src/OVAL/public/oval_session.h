/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALSESSION
 * OVAL Session
 *
 * This is a high level API for OVAL Definition content evaluation, collection
 * of OVAL System Characteristics and analysing of OVAL Definitions and
 * collected OVAL System Characteristics.
 * @{
 *
 * @file
 */

/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
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
 * 		Michal Šrubař <msrubar@redhat.com>
 */

#ifndef OVAL_SESSION_H_
#define OVAL_SESSION_H_

/**
 * @struct oval_session
 * A structure encapsulating the context of OVAL operations.
 */
struct oval_session;

/**
 * Costructor of an oval_session. It attempts to recognize a type of the filename.
 * @memberof oval_session
 * @param filename a path to an oval or source datastream file
 * @returns a newly created \ref oval_session
 * @retval NULL is returned in case of error (details might be found through \ref oscap_err_desc())
 */
struct oval_session *oval_session_new(const char *filename);

/**
 * Set OVAL Variables.
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param filename a path to an OVAL Variables file
 * @returns
 * @retval 0 on success
 * @retval 1 on an invalid \ref oval_session
 * @retval -1 on an internal error (use \ref oscap_err_desc to get more details)
 */
int oval_session_set_variables(struct oval_session *session, const char *filename);

/**
 * Destructor of an oval_session.
 * @memberof oval_session
 * @param session an \ref oval_session to destroy
 */
void oval_session_free(struct oval_session *session);

#endif
