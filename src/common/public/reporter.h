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

/**
 * @addtogroup COMMON
 * @{
 * @addtogroup Reporters
 * @{
 */

/// Reporter message family type
typedef unsigned int oscap_reporter_family_t;
/// Reporter message code type
typedef unsigned int oscap_reporter_code_t;
/// Maximum code
extern const oscap_reporter_code_t OSCAP_REPORTER_CODE_MAX;
/// Macro to match whole code range
#define OSCAP_REPORTER_ALL 0, OSCAP_REPORTER_CODE_MAX

/**
 * @struct oscap_reporter_message
 * Represents a message to be reported
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

/// function porototype for reporter initialisation
typedef bool(*oscap_reporter_init_func)(void **user);
/// function porototype for reporting a message
typedef void(*oscap_reporter_report_func)(const struct oscap_reporter_message *msg, void *user);
/// function porototype for destroying a reporter
typedef void(*oscap_reporter_destroy_func)(void *user);

/// @memberof oscap_reporter_message
struct oscap_reporter_message *oscap_reporter_message_new(void);
/// @memberof oscap_reporter_message
struct oscap_reporter_message *oscap_reporter_message_new_fill(oscap_reporter_family_t family, oscap_reporter_code_t code, const char *string);
/// @memberof oscap_reporter_message
struct oscap_reporter_message *oscap_reporter_message_clone(const struct oscap_reporter_message *msg);
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
/// @memberof oscap_reporter_message
oscap_reporter_family_t oscap_reporter_message_get_family(const struct oscap_reporter_message *item);
/// @memberof oscap_reporter_message
oscap_reporter_code_t oscap_reporter_message_get_code(const struct oscap_reporter_message *item);
/// @memberof oscap_reporter_message
const char *oscap_reporter_message_get_string(const struct oscap_reporter_message *item);


/// @memberof oscap_reporter_type
struct oscap_reporter_type *oscap_reporter_type_new_full(
                oscap_reporter_init_func init,
                oscap_reporter_report_func report,
                oscap_reporter_destroy_func destroy);
/// @memberof oscap_reporter_type
void oscap_reporter_type_free(struct oscap_reporter_type *reptype);


/// @memberof oscap_reporter
struct oscap_reporter *oscap_reporter_new(const struct oscap_reporter_type *type, void *user);
/// @memberof oscap_reporter
void oscap_reporter_free(struct oscap_reporter *reporter);
/// @memberof oscap_reporter
void oscap_reporter_dispatch(struct oscap_reporter *reporter, const struct oscap_reporter_message *msg);
/// @memberof oscap_reporter
void oscap_reporter_report(struct oscap_reporter *reporter, struct oscap_reporter_message *msg);
/// @memberof oscap_reporter
const char *oscap_reporter_get_userdata(const struct oscap_reporter *item);
/// @memberof oscap_reporter
bool oscap_reporter_set_userdata(struct oscap_reporter *obj, const char *newval);

/// @memberof oscap_reporter
void oscap_reporter_multi_add_reporter(struct oscap_reporter *multi, struct oscap_reporter *reporter);

/**
 * @name Standard reporters
 * @{
 */

/// Standard output reporter
extern const struct oscap_reporter_type OSCAP_REPORTER_STDOUT;
/// Reporter to multiple other reporters
extern const struct oscap_reporter_type OSCAP_REPORTER_MULTI;
/**
 * Create a new filtering reporter.
 * Messages will be forwarded to the child reporter if they match given family and code range.
 * @relates oscap_reporter
 */
struct oscap_reporter *oscap_reporter_new_filter(struct oscap_reporter *child,
            oscap_reporter_family_t family, oscap_reporter_code_t min_code, oscap_reporter_code_t max_code);

/** @} */

#endif // OSCAP_REPORTER_H_
