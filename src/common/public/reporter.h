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
 * Reporeter is a callback function that processes messages (see oscap_reporter).
 *
 * Aim of reporters is to separate message content from their final representation
 * and provide reasonable modularity. You can have reporters that print messages,
 * log messages to a file, update a GUI and more.
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
 * schema file name and a reporter as a parameter.
 * To print encountered defects to standard output, you would use it as follows:
 *
 * @par Sending messages
 *
 * In order to send a message from a function, you have to create the message (@ref oscap_reporter_message) and send it.
 * To send a message use oscap_reporter_report(). To forward a message from one reporter to another
 * (used in the special reporters) use oscap_reporter_dispatch().
 *
 * There are also shortcut functions, like oscap_reporter_report_fmt(), that allows you to create and send a message in one step.
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

/**
 * @struct oscap_reporter_message
 * Represents a message to be reported.
 */
struct oscap_reporter_message;

/**
 * Reporter prototype
 */
typedef void (*oscap_reporter)(const struct oscap_reporter_message *msg, void *arg);


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
 * Forward a message to a reporter.
 * @param msg message to be sent / forwarded
 * @memberof oscap_reporter
 */
void oscap_reporter_dispatch(oscap_reporter reporter, const struct oscap_reporter_message *msg, void *arg);

/**
 * Do report.
 * This function does nothing if @a reporter is null. The message gets freed afterwards.
 * @param reporter Reporter to use.
 * @param msg Message to send. Will be freed after processing by the reporter.
 * @memberof oscap_reporter
 */
void oscap_reporter_report(oscap_reporter reporter, struct oscap_reporter_message *msg, void *arg);
/// @memberof oscap_reporter
void oscap_reporter_report_fmt(oscap_reporter reporter, void *arg, oscap_reporter_family_t family, oscap_reporter_code_t code, const char *fmt, ...);

/// File descriptor reporter
void oscap_reporter_fd(const struct oscap_reporter_message *msg, void *arg);

#endif // OSCAP_REPORTER_H_
