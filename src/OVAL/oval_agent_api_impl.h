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


#define OVAL_COMMON_NAMESPACE  BAD_CAST "http://oval.mitre.org/XMLSchema/oval-common-5"
#define OVAL_SYSCHAR_NAMESPACE BAD_CAST "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5"
#define OVAL_DIGSIG_NAMESPACE  BAD_CAST "http://www.w3.org/2000/09/xmldsig#"


void add_oval_definition(struct oval_object_model *, struct oval_definition *);
void add_oval_test(struct oval_object_model *, struct oval_test *);
void add_oval_object(struct oval_object_model *, struct oval_object *);
void add_oval_state(struct oval_object_model *, struct oval_state *);
void add_oval_variable(struct oval_object_model *, struct oval_variable *);
void set_oval_sysinfo(struct oval_object_model *, struct oval_sysinfo *);

struct oval_definition *get_oval_definition_new(struct oval_object_model *,
						char *);
struct oval_test *get_oval_test_new(struct oval_object_model *, char *);
struct oval_object *get_oval_object_new(struct oval_object_model *, char *);
struct oval_state *get_oval_state_new(struct oval_object_model *, char *);
struct oval_variable *get_oval_variable_new(struct oval_object_model *, char *, oval_variable_type_enum type);
struct oval_syschar *get_oval_syschar_new(struct oval_syschar_model *, struct oval_object *);

struct oval_sysdata *get_oval_sysdata_new(struct oval_syschar_model *, char *);

char *malloc_string(const char *);

#endif				/* OVAL_AGENT_API_IMPL_H_ */
