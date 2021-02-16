/*
 * Copyright 2021 Sopra Steria.
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
 *      "Gwendal Didot <gwendal.didot@soprasteria.com>"
 */

#ifndef OPENSCAP_AUDITDLINE_PROBE_H
#define OPENSCAP_AUDITDLINE_PROBE_H

#include "probe-api.h"

/**
 * Helper struct that represent an auditd_line item
 */
struct audit_line {
	struct oscap_string *audit_line;
	struct oscap_stringlist *filter_key;
	int line_number;
};

/**
 * Constructor method for an audit_line struct
 * This method alloc an oscap_string and an oscap_stringlist 
 */
struct audit_line *audit_line_new(void);

/**
 * Destructor method for an audit_line struct
 * This method free the internal buffers of the struct
 */
void auditd_line_free(struct audit_line *audit_line);

/**
 * This method push the audit_line_part parameter in the audit_line buffer of the struct
 * @param audit_line struct to modify
 * @param audit_line_part element to add to the audit_line field of the struct
 */
void audit_line_push(struct audit_line *audit_line,
		     const char *audit_line_part);

/**
 * This method add a filter key to the filter_key field of an audit_line struct
 * @param audit_line struct to modify
 * @param filter_key filter_key element to add to the filter_key field of the struct
 */
bool audit_line_add_filter_key(struct audit_line *audit_line,
			       const char *filter_key);

/**
 *  This function prints 1 rule from the kernel reply
 */
struct audit_line *get_rule(const struct audit_rule_data *r);

int auditdline_probe_offline_mode_supported(void);
int auditdline_probe_main(probe_ctx *ctx, void *arg);

#endif /* OPENSCAP_AUDITDLINE_PROBE_H */