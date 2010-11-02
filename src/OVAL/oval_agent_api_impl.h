/**
 * @file oval_agent_api_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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

#include "public/oval_agent_api.h"
#include "oval_definitions_impl.h"
#include "../common/util.h"

OSCAP_HIDDEN_START;

#define OVAL_ENUMERATION_INVALID (-1)

#define OVAL_DEFAULT_VERSION "5.5"

#define OVAL_COMMON_NAMESPACE      BAD_CAST "http://oval.mitre.org/XMLSchema/oval-common-5"
#define OVAL_SYSCHAR_NAMESPACE     BAD_CAST "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5"
#define OVAL_RESULTS_NAMESPACE     BAD_CAST "http://oval.mitre.org/XMLSchema/oval-results-5"
#define OVAL_DEFINITIONS_NAMESPACE BAD_CAST "http://oval.mitre.org/XMLSchema/oval-definitions-5"
#define OVAL_DIGSIG_NAMESPACE      BAD_CAST "http://www.w3.org/2000/09/xmldsig#"
#define OVAL_VARIABLES_NAMESPACE   BAD_CAST "http://oval.mitre.org/XMLSchema/oval-variables-5"

/*#define OVAL_DEF_SCHEMA_LOCATION_DEF_PX    "http://oval.mitre.org/XMLSchema/oval-definitions-5 http://oval.mitre.org/language/download/schema/version"
#define OVAL_DEF_SCHEMA_LOCATION_DEF_SX    "/ovaldefinition/complete/oval-definitions-schema.xsd"
#define OVAL_DEF_SCHEMA_LOCATION_IND_PX    "http://oval.mitre.org/XMLSchema/oval-definitions-5#independent http://oval.mitre.org/language/download/schema/version"
#define OVAL_DEF_SCHEMA_LOCATION_IND_SX    "/ovaldefinition/complete/independent-definitions-schema.xsd"
#define OVAL_DEF_SCHEMA_LOCATION_UNX_PX    "http://oval.mitre.org/XMLSchema/oval-definitions-5#unix http://oval.mitre.org/language/download/schema/version"
#define OVAL_DEF_SCHEMA_LOCATION_UNX_SX    "/ovaldefinition/complete/unix-definitions-schema.xsd"
#define OVAL_DEF_SCHEMA_LOCATION_LNX_PX    "http://oval.mitre.org/XMLSchema/oval-definitions-5#linux http://oval.mitre.org/language/download/schema/version"
#define OVAL_DEF_SCHEMA_LOCATION_LNX_SX    "/ovaldefinition/complete/linux-definitions-schema.xsd"
#define OVAL_DEF_SCHEMA_LOCATION_CMN_PX    "http://oval.mitre.org/XMLSchema/oval-common-5 http://oval.mitre.org/language/download/schema/version"
#define OVAL_DEF_SCHEMA_LOCATION_CMN_SX    "/ovaldefinition/complete/oval-common-schema.xsd"
*/

#define OVAL_DEF_SCHEMA_LOCATION "http://oval.mitre.org/XMLSchema/oval-definitions-5 http://oval.mitre.org/language/download/schema/version5.5/ovaldefinition/complete/oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent http://oval.mitre.org/language/download/schema/version5.5/ovaldefinition/complete/independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#unix http://oval.mitre.org/language/download/schema/version5.5/ovaldefinition/complete/unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux http://oval.mitre.org/language/download/schema/version5.5/ovaldefinition/complete/linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 http://oval.mitre.org/language/download/schema/version5.5/ovaldefinition/complete/linux-common-schema.xsd"

#define OVAL_SYS_SCHEMA_LOCATION "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5 http://oval.mitre.org/language/version5.5/ovalsc/complete/oval-system-characteristics-schema.xsd http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent http://oval.mitre.org/language/version5.5/ovalsc/complete/independent-system-characteristics-schema.xsd http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix http://oval.mitre.org/language/version5.5/ovalsc/complete/unix-system-characteristics-schema.xsd http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux http://oval.mitre.org/language/version5.5/ovalsc/complete/linux-system-characteristics-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 http://oval.mitre.org/language/version5.5/ovalsc/complete/oval-common-schema.xsd"

#define OVAL_RES_SCHEMA_LOCATION "http://oval.mitre.org/XMLSchema/oval-results-5 http://oval.mitre.org/language/version5.5/ovalresults/complete/oval-results-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 http://oval.mitre.org/language/version5.5/ovalresults/complete/oval-common-schema.xsd"

#define OVAL_XMLNS_XSI              BAD_CAST "http://www.w3.org/2001/XMLSchema-instance"

const char * oval_definition_model_get_schema(struct oval_definition_model * model);
void oval_definition_model_set_schema(struct oval_definition_model *model, const char *version);

void oval_syschar_model_set_schema(struct oval_syschar_model *model, const char * schema);
const char * oval_syschar_model_get_schema(struct oval_syschar_model * model);

struct oval_definition *oval_definition_get_new(struct oval_definition_model *, const char *);

typedef bool oval_definitions_resolver(struct oval_definition *, void *);

xmlNode *oval_definitions_to_dom(struct oval_definition_model *definition_model, xmlDocPtr doc, xmlNode * parent);

struct oval_test *oval_test_get_new(struct oval_definition_model *, const char *);
struct oval_object *oval_object_get_new(struct oval_definition_model *, const char *);
struct oval_state *oval_state_get_new(struct oval_definition_model *, const char *);
struct oval_variable *oval_variable_get_new(struct oval_definition_model *, const char *, oval_variable_type_t type);

struct oval_syschar *oval_syschar_get_new(struct oval_syschar_model *, struct oval_object *);
struct oval_syschar_item *oval_syschar_item_get_new(struct oval_syschar_model *, const char *);
struct oval_sysitem *oval_sysitem_get_new(struct oval_syschar_model *model, const char *id);

typedef bool oval_syschar_resolver(struct oval_syschar *, void *);

xmlNode *oval_syschar_model_to_dom(struct oval_syschar_model *, xmlDocPtr, xmlNode *, oval_syschar_resolver, void *);
void oval_syschar_model_reset(struct oval_syschar_model *model);

OSCAP_HIDDEN_END;

#endif				/* OVAL_AGENT_API_IMPL_H_ */
