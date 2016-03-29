/**
 * @file oval_results_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 *      Šimon Lukašík
 */

#ifndef OVAL_RESULTS_IMPL_H_
#define OVAL_RESULTS_IMPL_H_

#include "oval_results.h"

#include "OVAL/oval_definitions_impl.h"
#include "OVAL/oval_system_characteristics_impl.h"
#include "OVAL/adt/oval_smc_impl.h"

#include "common/util.h"
#include "source/oscap_source_priv.h"

OSCAP_HIDDEN_START;

int oval_result_system_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, void *);
xmlNode *oval_result_system_to_dom(struct oval_result_system *, struct oval_results_model *, struct oval_directives_model *, xmlDocPtr, xmlNode *);

struct oval_result_test *oval_result_system_get_new_test(struct oval_result_system *, struct oval_test *, int variable_instance);

int oval_result_definition_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, void *);
struct oval_result_definition *make_result_definition_from_oval_definition(struct oval_result_system *,
									   struct oval_definition *,
									   int variable_instance);
xmlNode *oval_result_definition_to_dom(struct oval_result_definition *, oval_result_directive_content_t, xmlDocPtr, xmlNode *);
int oval_result_definition_get_variable_instance_hint(const struct oval_result_definition *definition);
void oval_result_definition_set_variable_instance_hint(struct oval_result_definition *definition, int new_hint_value);

struct oval_result_test *make_result_test_from_oval_test(struct oval_result_system *system, struct oval_test *oval_test, int variable_instance);

int oval_result_test_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, void *);
xmlNode *oval_result_test_to_dom(struct oval_result_test *, xmlDocPtr, xmlNode *);


int oval_result_item_parse_tag(xmlTextReaderPtr, struct oval_parser_context *, struct oval_result_system *, oscap_consumer_func, void *);
xmlNode *oval_result_item_to_dom(struct oval_result_item *, xmlDocPtr, xmlNode *);

struct oval_result_criteria_node *make_result_criteria_node_from_oval_criteria_node(struct oval_result_system *, struct oval_criteria_node *, int variable_instance);

int oval_result_criteria_node_parse(xmlTextReaderPtr, struct oval_parser_context *, struct oval_result_system *, oscap_consumer_func, void *);
oval_result_t oval_result_criteria_node_negate(struct oval_result_criteria_node *node, oval_result_t result);
xmlNode *oval_result_criteria_node_to_dom(struct oval_result_criteria_node *, xmlDocPtr, xmlNode *);

oval_result_t oval_result_parse(xmlTextReaderPtr, char *, oval_result_t);

struct oval_result_definition *oval_result_system_get_new_definition(struct oval_result_system *,
								     struct oval_definition *,
								     int variable_instance);
struct oval_result_test *oval_result_system_get_test(struct oval_result_system *, char *);

struct oresults {
	int true_cnt;
	int false_cnt;
	int unknown_cnt;
	int error_cnt;
	int noteval_cnt;
	int notappl_cnt;
};

int ores_add_res(struct oresults *ores, oval_result_t res);
void ores_clear(struct oresults *ores);
oval_result_t ores_get_result_bychk(struct oresults *ores, oval_check_t check);
oval_result_t ores_get_result_byopr(struct oresults *ores, oval_operator_t op);

struct oval_results_model *oval_results_model_new_with_probe_session(struct oval_definition_model *definition_model, struct oval_syschar_model **syschar_models, struct oval_probe_session *probe_session);
struct oval_probe_session *oval_results_model_get_probe_session(struct oval_results_model *model);
void oval_results_model_add_system(struct oval_results_model *, struct oval_result_system *);

struct oval_result_definition_iterator *oval_result_definition_iterator_new(struct oval_smc *mapping);
struct oval_result_test_iterator *oval_result_test_iterator_new(struct oval_smc *mapping);

const char *oval_result_test_get_id(const struct oval_result_test *test);


struct oval_result_definition *oval_result_system_prepare_definition(struct oval_result_system *sys, const char *id);

OSCAP_HIDDEN_END;

#endif				/* OVAL_RESULTS_IMPL_H_ */
