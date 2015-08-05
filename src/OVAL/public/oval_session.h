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
 * Costructor of an \ref oval_session. It attempts to recognize a type of the
 * filename which has to be a source datastream or OVAL Definitions.
 *
 * @memberof oval_session
 * @param filename a path to an OVAL or source datastream file
 *
 * @returns a newly created \ref oval_session
 * @retval NULL in case of an error (use \ref oscap_err_desc or \ref
 * oscap_err_get_full_error to get more details)
 */
struct oval_session *oval_session_new(const char *filename);

/**
 * Set OVAL Variables.
 *
 * If a filename has been already set then it will be overwritten. If you pass
 * NULL as filename argument then the currently set variables will be freed.
 * Validation of the format will be perforemed when the \ref oval_session_load
 * is called.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param filename a path to an OVAL Variables file
 */
void oval_session_set_variables(struct oval_session *session, const char *filename);

/**
 * Set OVAL Directives
 *
 * If a filename has been already set then it will be overwritten. If you pass
 * NULL as filename argument then the currently set variables will be freed.
 * Validation of the format will be perforemed when the \ref oval_session_load
 * is called.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param filename a path to an OVAL Directives file
 */
void oval_session_set_directives(struct oval_session *session, const char *filename);

/**
 * Set XSD validation level.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param validate false value indicates to skip any XSD validation
 * @param full_validation true value indicates that every possible step will be validated by XSD
 */
void oval_session_set_validation(struct oval_session *session, bool validate, bool full_validation);

/**
 * Set ID of a specific OVAL Definition in an source datastream.
 *
 * If the ID has been already set then it will be overwritten. In case where
 * there are two OVALs in one datastream use \ref oval_session_set_component_id.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param id an id of a definition
 */
void oval_session_set_datastream_id(struct oval_session *session, const char *id);

/**
 * Set ID of a particular OVAL component if there are two OVALs in one
 * source datastream. If the ID has been already set then it will be
 * overwritten.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param id an id of a definition
 */
void oval_session_set_component_id(struct oval_session *session, const char *id);

/**
 * Set a name of the file that the the OVAL Results will be written into. If the
 * file's name has been already set for the current session then it will be
 * overwritten.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param filename a path to a new file
 */
void oval_session_set_results_export(struct oval_session *session, const char *filename);

/**
 * Set a name of the file that the the OVAL Results, converted to HTML format,
 * will be written into. If the file's name has been already set for the current
 * session then it will be overwritten.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param filename a path to a new file
 */
void oval_session_set_report_export(struct oval_session *session, const char *filename);

/**
 * Set XML validation reporter.
 *
 * This function is called when there is something invalid in input/output XML
 * file.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param fn pointer to XML reporter function
 */
void oval_session_set_xml_reporter(struct oval_session *session, xml_reporter fn);

/**
 * Load OVAL Definitions and bind OVAL Variables to it if provided. Validation
 * if performed automatically if you've set it with \ref
 * oval_session_set_validation. if the validation failed then the function will
 * print information about what line in what file isn't valid and why not.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 *
 * @retval 0 on success
 * @retval 1 on an internal error (use \ref oscap_err_desc or \ref
 * oscap_err_get_full_error to get more details)
 */
int oval_session_load(struct oval_session *session);

/**
 * Evaluate a specific OVAL Definition. The result of the evaluation will be
 * written into the result argument. You can convert it into a string
 * representation with \ref oval_result_get_text.
 *
 * @memberof oval_session
 * @param session an \ref oval_session
 * @param probe_root FIXME:
 * @param id id of an OVAL Definition
 * @param result variable to write the result into
 *
 * @retval 0 on success
 * @retval 1 on an internal error (use \ref oscap_err_desc or \ref
 * oscap_err_get_full_error to get more details)
 */
int oval_session_evaluate_id(struct oval_session *session, char *probe_root, const char *id, oval_result_t *result);

/**
 * Destructor of an \ref oval_session.
 * @memberof oval_session
 * @param session an \ref oval_session to destroy
 */
void oval_session_free(struct oval_session *session);

#endif
