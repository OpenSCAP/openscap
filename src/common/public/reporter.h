/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 *       Lukas Kuklinek <lkuklinek@redhat.com>
 */

/**
 * @file
 * Reporters interface
 */

#ifndef OSCAP_REPORTER_H_
#define OSCAP_REPORTER_H_

#include <stdbool.h>
#include <stdarg.h>

/**
 * @addtogroup COMMON
 * @{
 * @addtogroup Reporters
 * @{
 * Reporting mechanism.
 *
 * Can be used for e.g. returning individual errors during a SCAP document validation,
 * reporting progress of some processing etc.
 *
 * Aim of reporters is to separate message content from their final representation
 * and provide reasonable modularity. You can have reporters that print messages,
 * log messages to a file, update a GUI and more.
 *
 * @par Reporter types
 *
 * Reporter types are represented by @ref oscap_reporter_type structure.
 *
 * Simple printing messages to the standard is provided by the library.
 * There are also special reporters that forward messaged to more than one other
 * reporter (@ref OSCAP_REPORTER_MULTI) or filter messages based on message code and family
 * and forward them to one or other reporter (see oscap_reporter_new_filter()).
 *
 * oscap_reporter_type_new() can be used to define your own reporter type.
 * It consists of three callbacks: init, report, and destroy.
 *
 * @par Handling messages
 *
 * Messages (oscap_reporter_message) are organized in families to avoid code clashes.
 * Message code distiguishes different types of messages. In addition to that, messages
 * cointain human-readable string description of the event.
 *
 * It is also possible to attach up to three machine-readable items to the message.
 * These are domain-specific for given family or even code and shall
 * be well-documented so they can be handeled properly.
 *
 * One function that makes use of reporters is oscap_validate_xml(). It takes XML file name,
 * schema file name and a reporter as a last parameter.
 * To print encountered defects to standard output, you would use it as follows:
 *
 * @code
 * struct oscap_reporter *reporter = oscap_reporter_new(&OSCAP_REPORTER_STDOUT, NULL);
 * oscap_validate_xml(xmlfilename, schemafilename, reporter);
 * oscap_reporter_free(reporter);
 * @endcode
 *
 * With special reporters you can for example write all the messages to a file,
 * validation-related messages to standard output and other messages
 * (e.g. xinclude substitution failure, schema errors) to standard error, creating
 * a hierarchy of reporters. Note you release only the top-level reporter.
 * It is assumed that my_*_reporter_type are previously created by the user.
 *
 * @code
 * struct oscap_reporter *reporter = oscap_reporter_new(&OSCAP_REPORTER_MULTI, NULL);
 * oscap_reporter_multi_add_reporter(reporter, oscap_reporter_new(my_file_reporter_type, "validation.log"));
 * oscap_reporter_multi_add_reporter(reporter,
 *     oscap_reporter_new_filter(
 *         oscap_reporter_new(&OSCAP_REPORTER_STDOUT, NULL),
 *         oscap_reporter_new(my_stderr_reporter_type, NULL),
 *         OSCAP_REPORTER_FAMILY_XML, XML_SCHEMAV_NOROOT, XML_SCHEMAV_MISC
 *     )
 * );
 * oscap_validate_xml(xmlfilename, schemafilename, reporter);
 * oscap_reporter_free(reporter);
 * @endcode
 *
 * @dot
 * digraph {
 *   graph [rankdir=LR]
 *   node [shape=box,fontsize=10]
 *   multi [label="Multireporter"]
 *   file [label="File reporter"]
 *   filter [label="Filter reporter"]
 *   stdout [label="Standard output reporter"]
 *   stderr [label="Standard error reporter"]
 *   multi -> file
 *   multi -> filter
 *   filter -> stdout
 *   filter -> stderr
 * }
 * @enddot
 *
 * @par Sending messages
 *
 * In order to send a message from a function, you have to create the message (@ref oscap_reporter_message) and send it.
 * To send a message use oscap_reporter_report(). To forward a message from one reporter to another
 * (used in the special reporters) use oscap_reporter_dispatch().
 *
 * There are also shortcut functions, like oscap_reporter_report_fmt(), that allows you to create and sen a message in one step.
 */


/// Reporter message family
typedef enum oscap_reporter_family {
	OSCAP_REPORTER_FAMILY_XML = 1,
	OSCAP_REPORTER_FAMILY_USER_START = 1024
} oscap_reporter_family_t;

/// Reporter message code type
typedef unsigned int oscap_reporter_code_t;

/// Maximum code
extern const oscap_reporter_code_t OSCAP_REPORTER_CODE_MAX;

/// Macro to match whole code range
#define OSCAP_REPORTER_ALL 0, OSCAP_REPORTER_CODE_MAX

/**
 * @struct oscap_reporter_message
 * Represents a message to be reported.
 */
struct oscap_reporter_message;

/**
 * @struct oscap_reporter_type
 * Reporter type.
 */
struct oscap_reporter_type;

/**
 * @struct oscap_reporter
 * Reporter instance
 */
struct oscap_reporter;


/**
 * Function porototype for reporter initialisation.
 *
 * The @a user argument is a pointer to the pointer user passed to the oscap_reporter_new() call.
 * You can modify it, if you want/need to.
 *
 * The function is upposed to return true on success and false on failure.
 * In case of failure oscap_reporter_new() will return NULL.
 *
 * @param user pointer to user-supplied data
 * @return false on failure, true on success
 */
typedef bool(*oscap_reporter_init_func)(void **user);

/**
 * Function porototype for reporting a message.
 * This callback gets called for every reported message.
 * @param msg Message to be processed by the reporter.
 * @param user User supplied data.
 */
typedef void(*oscap_reporter_report_func)(const struct oscap_reporter_message *msg, void *user);
/**
 * Function porototype for destroying a reporter.
 * It is intended to be used to release allocated resources and do necessary cleanup.
 * @param user User supplied data.
 */
typedef void(*oscap_reporter_destroy_func)(void *user);

/// @memberof oscap_reporter_message
struct oscap_reporter_message *oscap_reporter_message_new(void);
/// @memberof oscap_reporter_message
struct oscap_reporter_message *oscap_reporter_message_new_fill(oscap_reporter_family_t family, oscap_reporter_code_t code, const char *string);
/// @memberof oscap_reporter_message
struct oscap_reporter_message *oscap_reporter_message_new_arg(oscap_reporter_family_t family, oscap_reporter_code_t code, const char *fmt, va_list ap);
/// @memberof oscap_reporter_message
struct oscap_reporter_message *oscap_reporter_message_new_fmt(oscap_reporter_family_t family, oscap_reporter_code_t code, const char *fmt, ...);
/// @memberof oscap_reporter_message
struct oscap_reporter_message *oscap_reporter_message_clone(const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
oscap_reporter_family_t oscap_reporter_message_get_family(const struct oscap_reporter_message *item);
/// @memberof oscap_reporter_message
oscap_reporter_code_t oscap_reporter_message_get_code(const struct oscap_reporter_message *item);
/// @memberof oscap_reporter_message
const char *oscap_reporter_message_get_string(const struct oscap_reporter_message *item);

/// @memberof oscap_reporter_message
const char *oscap_reporter_message_get_user1str(const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
int oscap_reporter_message_get_user1num(const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
void *oscap_reporter_message_get_user1ptr(const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
const char *oscap_reporter_message_get_user2str(const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
int oscap_reporter_message_get_user2num(const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
void *oscap_reporter_message_get_user2ptr(const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
const char *oscap_reporter_message_get_user3str(const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
int oscap_reporter_message_get_user3num(const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
void *oscap_reporter_message_get_user3ptr(const struct oscap_reporter_message *msg);


/**
 * Create a new reporter type.
 * @param init function called upon initialisation with a pointer to user supplied data
 * @param report reporting function, that is called for each reported message
 * @param destroy reporter cleanup function
 * @memberof oscap_reporter_type
 * @see oscap_reporter_init_func oscap_reporter_report_func oscap_reporter_destroy_func
 */
struct oscap_reporter_type *oscap_reporter_type_new(
                oscap_reporter_init_func init,
                oscap_reporter_report_func report,
                oscap_reporter_destroy_func destroy);

/// @memberof oscap_reporter_type
void oscap_reporter_type_free(struct oscap_reporter_type *reptype);


/**
 * Create a new reporter of given type.
 * @param type reporter handler set
 * @param user reporter-specific user-supplied data
 * @memberof oscap_reporter
 */
struct oscap_reporter *oscap_reporter_new(const struct oscap_reporter_type *type, void *user);
/// @memberof oscap_reporter
void oscap_reporter_free(struct oscap_reporter *reporter);

/**
 * Forward a message to a reporter.
 * @param msg message to be sent / forwarded
 * @memberof oscap_reporter
 */
void oscap_reporter_dispatch(struct oscap_reporter *reporter, const struct oscap_reporter_message *msg);

/**
 * Do report.
 * This function does nothing if @a reporter is null. The message gets freed afterwards.
 * @param reporter Reporter to use.
 * @param msg Message to send. Will be freed after processing by the reporter.
 * @memberof oscap_reporter
 */
void oscap_reporter_report(struct oscap_reporter *reporter, struct oscap_reporter_message *msg);
/// @memberof oscap_reporter
void oscap_reporter_report_fmt(struct oscap_reporter *reporter, oscap_reporter_family_t family, oscap_reporter_code_t code, const char *fmt, ...);

/// @memberof oscap_reporter
const char *oscap_reporter_get_userdata(const struct oscap_reporter *item);
/// @memberof oscap_reporter
bool oscap_reporter_set_userdata(struct oscap_reporter *obj, const char *newval);

/**
 * @name Standard reporters
 * @{
 */

/**
 * Standard output reporter.
 * This reporter simply prints messages to the standard output.
 * If user data is supplied, it shall be a string that will be printed as a message prefix.
 * @relates oscap_reporter
 */
extern const struct oscap_reporter_type OSCAP_REPORTER_STDOUT;

/**
 * Reporter to multiple other reporters.
 * Use oscap_reporter_multi_add_reporter() to register reporters.
 * Any message handled by this reporter will be forwarded to all the registered subreporters.
 * The subreporters will be destroyed with the multireporter.
 * No user data allowed.
 * @relates oscap_reporter
 * @see oscap_reporter_multi_add_reporter
 */
extern const struct oscap_reporter_type OSCAP_REPORTER_MULTI;

/**
 * Add a subreporter to given multireporter.
 * It is users responsibility to ensure that reporter type of the first param is OSCAP_REPORTER_MULTI.
 * @relates oscap_reporter
 * @see OSCAP_REPORTER_MULTI
 */
void oscap_reporter_multi_add_reporter(struct oscap_reporter *multi, struct oscap_reporter *reporter);

/**
 * Create a new filtering reporter.
 * Messages will be forwarded to the pos_child reporter if they match given family and code range
 * and to neg_child if they do not. Either of these reporters can be NULL.
 * The child reporters will be freed with filter.
 * If you want to match all error codes, you can use OSCAP_REPORTER_ALL instead of two last params.
 * @param pos_child Reporter to forward messages that pass filter criteria.
 * @param neg_child Reporter to forward messages that do NOT pass filter criteria.
 * @param family Error family to let pass through the filter.
 * @param min_code Minimum error code number to match (inclusive).
 * @param max_code Maximum error code number to match (inclusive).
 * @relates oscap_reporter
 */
struct oscap_reporter *oscap_reporter_new_filter(struct oscap_reporter *pos_child, struct oscap_reporter *neg_child,
            oscap_reporter_family_t family, oscap_reporter_code_t min_code, oscap_reporter_code_t max_code);

/** @} */

#endif // OSCAP_REPORTER_H_
