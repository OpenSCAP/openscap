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

#include "common/util.h"

OSCAP_HIDDEN_START;

#define OVAL_ENUMERATION_INVALID (-1)

#define OVAL_SUPPORTED "5.11.1"

#define OVAL_COMMON_NAMESPACE      BAD_CAST "http://oval.mitre.org/XMLSchema/oval-common-5"
#define OVAL_DIGSIG_NAMESPACE      BAD_CAST "http://www.w3.org/2000/09/xmldsig#"

#define OVAL_SYSCHAR_NAMESPACE     BAD_CAST "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5"
#define OVAL_SYSCHAR_UNIX_NS       BAD_CAST "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix"
#define OVAL_SYSCHAR_IND_NS        BAD_CAST "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent"
#define OVAL_SYSCHAR_LIN_NS        BAD_CAST "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux"
#define OVAL_SYSCHAR_WIN_NS        BAD_CAST "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows"

#define OVAL_DEFINITIONS_NAMESPACE BAD_CAST "http://oval.mitre.org/XMLSchema/oval-definitions-5"
#define OVAL_DEFINITIONS_UNIX_NS   BAD_CAST "http://oval.mitre.org/XMLSchema/oval-definitions-5#unix"
#define OVAL_DEFINITIONS_IND_NS    BAD_CAST "http://oval.mitre.org/XMLSchema/oval-definitions-5#independent"
#define OVAL_DEFINITIONS_LIN_NS    BAD_CAST "http://oval.mitre.org/XMLSchema/oval-definitions-5#linux"
#define OVAL_DEFINITIONS_WIN_NS    BAD_CAST "http://oval.mitre.org/XMLSchema/oval-definitions-5#windows"

#define OVAL_RESULTS_NAMESPACE     BAD_CAST "http://oval.mitre.org/XMLSchema/oval-results-5"
#define OVAL_VARIABLES_NAMESPACE   BAD_CAST "http://oval.mitre.org/XMLSchema/oval-variables-5"
#define OVAL_DIRECTIVES_NAMESPACE  BAD_CAST "http://oval.mitre.org/XMLSchema/oval-directives-5"

#define OVAL_DEF_SCHEMA_LOCATION "http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd"

#define OVAL_SYS_SCHEMA_LOCATION "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5 oval-system-characteristics-schema.xsd http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux linux-system-characteristics-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd"

#define OVAL_RES_SCHEMA_LOCATION "http://oval.mitre.org/XMLSchema/oval-results-5 oval-results-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd"

#define OVAL_DIR_SCHEMA_LOCATION "http://oval.mitre.org/XMLSchema/oval-results-5 oval-results-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd http://oval.mitre.org/XMLSchema/oval-directives-5 oval-directives-schema.xsd"

#define OVAL_VAR_SCHEMA_LOCATION "http://oval.mitre.org/XMLSchema/oval-results-5 oval-results-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd http://oval.mitre.org/XMLSchema/oval-variables-5 oval-variables-schema.xsd"

OSCAP_HIDDEN_END;

#endif				/* OVAL_AGENT_API_IMPL_H_ */
