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
 * Reporters private interface
 */

#ifndef OSCAP_REPORTER_PRIV_H_
#define OSCAP_REPORTER_PRIV_H_

#include "public/reporter.h"
#include "util.h"
#include <libxml/xmlerror.h>

OSCAP_HIDDEN_START;

/**
 * @addtogroup COMMON
 * @{
 * @addtogroup Reporters
 * @{
 */

/// Reporter cast
#define XREPORTER(r) ((struct oscap_reporter*) r)

/// @memberof oscap_reporter_message
void oscap_reporter_message_free(struct oscap_reporter_message *msg);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_family(struct oscap_reporter_message *obj, oscap_reporter_family_t newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_code(struct oscap_reporter_message *obj, oscap_reporter_code_t newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_string(struct oscap_reporter_message *obj, const char *newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_user1str(struct oscap_reporter_message *msg, const char *newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_user1num(struct oscap_reporter_message *msg, int newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_user1ptr(struct oscap_reporter_message *msg, void *newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_user2str(struct oscap_reporter_message *msg, const char *newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_user2num(struct oscap_reporter_message *msg, int newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_user2ptr(struct oscap_reporter_message *msg, void *newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_user3str(struct oscap_reporter_message *msg, const char *newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_user3num(struct oscap_reporter_message *msg, int newval);
/// @memberof oscap_reporter_message
bool oscap_reporter_message_set_user3ptr(struct oscap_reporter_message *msg, void *newval);

/**
 * Feed a XML error to the reporter.
 * @memberof oscap_reporter
 */
void oscap_reporter_report_xml(struct oscap_reporter *reporter, xmlErrorPtr error);

/**
 * Report standard C error (errno based) to a reporter
 * @memberof oscap_reporter
 */
void oscap_reporter_report_libc(struct oscap_reporter *reporter);

/** @} */
/** @} */

OSCAP_HIDDEN_END;

#endif // OSCAP_REPORTER_PRIV_H_

