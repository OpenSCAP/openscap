/**
 * @file oval_agent_api_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#ifndef OVAL_AGENT_API_IMPL_H_
#define OVAL_AGENT_API_IMPL_H_

#include "api/oval_agent_api.h"
#include "oval_definitions_impl.h"
#include "../common/util.h"

OSCAP_HIDDEN_START;


#define OVAL_COMMON_NAMESPACE      BAD_CAST "http://oval.mitre.org/XMLSchema/oval-common-5"
#define OVAL_SYSCHAR_NAMESPACE     BAD_CAST "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5"
#define OVAL_RESULTS_NAMESPACE     BAD_CAST "http://oval.mitre.org/XMLSchema/oval-results-5"
#define OVAL_DEFINITIONS_NAMESPACE BAD_CAST "http://oval.mitre.org/XMLSchema/oval-definitions-5"
#define OVAL_DIGSIG_NAMESPACE      BAD_CAST "http://www.w3.org/2000/09/xmldsig#"

struct oval_definition *get_oval_definition_new
	(struct oval_definition_model *, char *);

typedef bool oval_definitions_resolver(struct oval_definition *, void *);

xmlNode *oval_definitions_to_dom
	(struct oval_definition_model *definition_model, xmlDocPtr doc, xmlNode *parent,
	 oval_definitions_resolver resolver, void *user_arg);

struct oval_test *get_oval_test_new(struct oval_definition_model *, char *);
struct oval_object *get_oval_object_new(struct oval_definition_model *, char *);
struct oval_state *get_oval_state_new(struct oval_definition_model *, char *);
struct oval_variable *get_oval_variable_new(struct oval_definition_model *, char *, oval_variable_type_t type);

struct oval_definition *oval_definition_clone(struct oval_definition *, struct oval_definition_model *);
struct oval_test       *oval_test_clone      (struct oval_test       *, struct oval_definition_model *);
struct oval_object     *oval_object_clone    (struct oval_object     *, struct oval_definition_model *);
struct oval_state      *oval_state_clone     (struct oval_state      *, struct oval_definition_model *);
struct oval_variable   *oval_variable_clone  (struct oval_variable   *, struct oval_definition_model *);

struct oval_syschar *get_oval_syschar_new(struct oval_syschar_model *, struct oval_object *);
struct oval_syschar_item *get_oval_syschar_item_new(struct oval_syschar_model *, char *);

typedef bool oval_syschar_resolver(struct oval_syschar *, void *);

xmlNode *oval_syschar_model_to_dom
	(struct oval_syschar_model *, xmlDocPtr, xmlNode *,
			oval_syschar_resolver, void *);

struct oval_sysdata *get_oval_sysdata_new(struct oval_syschar_model *, char *);
void oval_syschar_model_add_syschar(struct oval_syschar_model *model, struct oval_syschar *syschar);
void oval_syschar_model_set_sysinfo(struct oval_syschar_model *model, struct oval_sysinfo *sysinfo);

typedef struct oval_export_target {
	char *filename;
	char *encoding;
} oval_export_target_t;


typedef struct oval_import_source {
	char *import_source_filename;
} oval_import_source_t;

OSCAP_HIDDEN_END;

#endif				/* OVAL_AGENT_API_IMPL_H_ */
