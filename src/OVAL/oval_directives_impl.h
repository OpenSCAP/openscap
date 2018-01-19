/**
 * @file oval_directives_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *      "Peter Vrabec" <pvrabec@redhat.com>
 */

#ifndef OVAL_DIRECTIVES_IMPL_H
#define OVAL_DIRECTIVES_IMPL_H

#include "public/oval_directives.h"
#include "oval_agent_api_impl.h"
#include "oval_definitions_impl.h"
#include "oval_parser_impl.h"
#include "common/util.h"


int oval_result_directives_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, void *);
int oval_result_directives_to_dom(struct oval_result_directives *, xmlDoc *, xmlNode *);
xmlNode *oval_directives_model_to_dom(struct oval_directives_model *, xmlDocPtr, xmlNode *);


#define OVAL_DIRECTIVES_IMPL_H_
#endif				/* OVAL_DIRECTIVES_IMPL_H_ */


