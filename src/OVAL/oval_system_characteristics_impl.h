/**
 * @file oval_system_characteristics_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009,2010,2011 Red Hat Inc., Durham, North Carolina.
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
 *      "Peter Vrabec" <pvrabec@redhat.com>
 */

#ifndef OVAL_SYSCHAR_IMPL
#define OVAL_SYSCHAR_IMPL

#include "public/oval_system_characteristics.h"
#include "oval_parser_impl.h"
#include "adt/oval_smc_impl.h"
#include "../common/util.h"


/* sysint */
typedef void (*oval_sysint_consumer) (struct oval_sysint *, void *);
int oval_sysint_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_sysint_consumer, void *);
void oval_sysint_to_dom(struct oval_sysint *, xmlDoc *, xmlNode *);

/* sysinfo */
void oval_sysinfo_to_dom(struct oval_sysinfo *, xmlDoc *, xmlNode *);
int oval_sysinfo_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *);

/* sysitem */
void oval_sysitem_to_dom(struct oval_sysitem *, xmlDoc *, xmlNode *);
int oval_sysitem_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, void *usr);

/* syschar */
void oval_syschar_to_dom(struct oval_syschar *, xmlDoc *, xmlNode *);
int oval_syschar_parse_tag(xmlTextReaderPtr, struct oval_parser_context *context, void *);
oval_syschar_collection_flag_t oval_syschar_flag_parse(xmlTextReaderPtr, char *, oval_syschar_collection_flag_t);
oval_syschar_status_t oval_syschar_status_parse(xmlTextReaderPtr, char *, oval_syschar_status_t);
struct oval_syschar_model *oval_syschar_get_model(struct oval_syschar *syschar);

/* sysent */
typedef void (*oval_sysent_consumer) (struct oval_sysent *, void *client);
int oval_sysent_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, oval_sysent_consumer, void *);
void oval_sysent_to_dom(struct oval_sysent *sysent, xmlDoc * doc, xmlNode * tag_parent);
void oval_sysent_to_print(struct oval_sysent *, char *, int);

/* syschar_model */
typedef bool oval_syschar_resolver(struct oval_syschar *, void *);
xmlNode *oval_syschar_model_to_dom(struct oval_syschar_model *, xmlDocPtr, xmlNode *, oval_syschar_resolver, void *, bool);
void oval_syschar_model_reset(struct oval_syschar_model *model);

struct oval_syschar *oval_syschar_model_get_new_syschar(struct oval_syschar_model *, struct oval_object *);
struct oval_sysitem *oval_syschar_model_get_new_sysitem(struct oval_syschar_model *, const char *id);
void oval_syschar_model_add_syschar(struct oval_syschar_model *model, struct oval_syschar *syschar);
void oval_syschar_model_add_sysitem(struct oval_syschar_model *model, struct oval_sysitem *sysitem);

void oval_syschar_model_set_schema(struct oval_syschar_model *model, const char * schema);
const char * oval_syschar_model_get_schema(struct oval_syschar_model * model);

struct oval_syschar_iterator *oval_syschar_iterator_new(struct oval_smc *mapping);
int oval_syschar_get_variable_instance_hint(const struct oval_syschar *syschar);
void oval_syschar_set_variable_instance_hint(struct oval_syschar *syschar, int variable_instance_hint_in);
const char *oval_syschar_get_id(const struct oval_syschar *syschar);


#endif
