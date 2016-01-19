/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>


#include "common/list.h"
#include "common/_error.h"
#include "common/debug_priv.h"
#include "public/xccdf_benchmark.h"
#include "XCCDF/item.h"
#include "XCCDF/result_scoring_priv.h"

/**
 * XCCDF Default score structure represents Default XCCDF Score model
 * for each rule
 */
struct xccdf_default_score {
	float score;
	float accumulator;
	float weight_score;
	int count;
};

/**
 * XCCDF Flat score structure represents Flat XCCDF Score model
 * for each rule
 */
struct xccdf_flat_score {
	float score;
	float weight;

};

static struct xccdf_default_score * xccdf_item_get_default_score(struct xccdf_item * item, struct xccdf_result * test_result)
{
	// Implements algorithm as described in NISTIR-7275-r4
	// Table 40: Default Model Algorithm Sub-Steps
	struct xccdf_default_score *score;
	struct xccdf_default_score *ch_score;
	struct xccdf_rule_result *rule_result;
	struct xccdf_item *child;

	xccdf_type_t itype = xccdf_item_get_type(item);

	switch (itype) {
	case XCCDF_RULE: {
		/* Rule */
		const char *rule_id = xccdf_rule_get_id((const struct xccdf_rule *) item);
		rule_result = xccdf_result_get_rule_result_by_id(test_result, rule_id);
		if (rule_result == NULL) {
			dE("Rule result ID(%s) not fount", rule_id);
			return NULL;
		}
		if (xccdf_rule_result_get_role(rule_result) == XCCDF_ROLE_UNSCORED) {
			return NULL;
		}

		/* Ignore these rules */
		if ((xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_SELECTED) ||
				(xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_APPLICABLE) ||
				(xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_INFORMATIONAL) ||
				(xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_CHECKED))
			return NULL;

		score = oscap_alloc(sizeof(struct xccdf_default_score));

		/* Count with this rule */
		score->count = 1;

		/* If the test result is 'pass', assign the node a score of 100, otherwise assign a score of 0 */
		if ((xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_PASS) ||
				(xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_FIXED))
			score->score = 100.0;
		else
			score->score = 0.0;

		/* Default weight */
		score->weight_score = score->score * xccdf_item_get_weight(item);
	} break;

	case XCCDF_BENCHMARK:
	case XCCDF_GROUP: {
		/* Init */
		score = oscap_alloc(sizeof(struct xccdf_default_score));
		score->count = 0;
		score->score = 0.0;
		score->accumulator = 0.0;

		/* Recurse */
		struct xccdf_item_iterator * child_it;
		if (itype == XCCDF_GROUP)
			child_it = xccdf_group_get_content((const struct xccdf_group *)item);
		else
			child_it = xccdf_benchmark_get_content((const struct xccdf_benchmark *)item);

		while (xccdf_item_iterator_has_more(child_it)) {
			child = xccdf_item_iterator_next(child_it);
			ch_score = xccdf_item_get_default_score(child, test_result);

			if (ch_score == NULL) /* we got item that can't be processed */
				continue;

			if (ch_score->count == 0) { /* we got item that has no selected items */
				oscap_free(ch_score);
				continue;
			}

			/* If child's count value is not 0, then add the child's wighted score to this node's score */
			score->score += ch_score->weight_score;
			score->count++;
			score->accumulator += xccdf_item_get_weight(child);

			oscap_free(ch_score);
		}

		/* Normalize */
		if (score->count && score->accumulator)
			score->score = score->score / score->accumulator;
		/* Default weight */
		score->weight_score = score->score * xccdf_item_get_weight(item);

		xccdf_item_iterator_free(child_it);
	} break;

	default: {
		dE("Unsupported item type: %d", itype);
		score=NULL;
	} break;

	} /* switch */
	return score;
}

static struct xccdf_flat_score * xccdf_item_get_flat_score(struct xccdf_item * item, struct xccdf_result * test_result, bool unweighted)
{
	// Implements algorithm as described in NISTIR-7275-r4
	// Table 41: Flat Model Algorithm Sub-Steps
	struct xccdf_flat_score *score;
	struct xccdf_flat_score *ch_score;
	struct xccdf_rule_result *rule_result;
	struct xccdf_item *child;

	xccdf_type_t itype = xccdf_item_get_type(item);

	switch (itype) {
	case XCCDF_RULE:{
		/* Rule */
		const char *rule_id = xccdf_rule_get_id((const struct xccdf_rule *) item);
		rule_result = xccdf_result_get_rule_result_by_id(test_result, rule_id);
		if (rule_result == NULL) {
			dE("Rule result ID(%s) not fount", rule_id);
			return NULL;
		}
		if (xccdf_rule_result_get_role(rule_result) == XCCDF_ROLE_UNSCORED) {
			return NULL;
		}

		/* Ignore these rules */
		if ((xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_SELECTED) ||
				(xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_APPLICABLE) ||
				(xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_INFORMATIONAL) ||
				(xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_NOT_CHECKED))
			return NULL;

		score = oscap_alloc(sizeof(struct xccdf_flat_score));

		/* max possible score = sum of weights*/
		if (unweighted)
			score->weight = 1.0;
		else score->weight =
			xccdf_item_get_weight(item);

		/* score = sum of weights of rules that pass */
		if ((xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_PASS) ||
				(xccdf_rule_result_get_result(rule_result) == XCCDF_RESULT_FIXED)) {
			if (unweighted)
				score->score = 1.0;
			else
				score->score = xccdf_item_get_weight(item);
		} else
			score->score = 0.0;	/* fail */
	} break;
	case XCCDF_BENCHMARK:
	case XCCDF_GROUP:{
		/* Init */
		score = oscap_alloc(sizeof(struct xccdf_flat_score));
		score->score = 0;
		score->weight = 0.0;

		/* Recurse */
		struct xccdf_item_iterator * child_it;
		if (itype == XCCDF_GROUP)
			child_it = xccdf_group_get_content((const struct xccdf_group *)item);
		else
			child_it = xccdf_benchmark_get_content((const struct xccdf_benchmark *)item);

		while (xccdf_item_iterator_has_more(child_it)) {
			child = xccdf_item_iterator_next(child_it);
			ch_score = xccdf_item_get_flat_score(child, test_result, unweighted);

			if (ch_score == NULL) /* we got item that can't be processed */
				continue;

			if (ch_score->weight == 0) { /* we got item that has no selected items */
				oscap_free(ch_score);
				continue;
			}

			/* If child's count value is not 0, then add the child's wighted score to this node's score */
			score->score += ch_score->score;
			score->weight += ch_score->weight;

			oscap_free(ch_score);
		}

		xccdf_item_iterator_free(child_it);
	} break;

	default: {
		dE("Unsupported item type: %d", itype);
		score=NULL;
	} break;

	} /* switch */
	return score;
}

struct xccdf_score *xccdf_result_calculate_score(struct xccdf_result *test_result, struct xccdf_item *benchmark, const char *score_system)
{
	struct xccdf_score *score = xccdf_score_new();
	xccdf_score_set_system(score, score_system);
	if (oscap_streq(score_system, "urn:xccdf:scoring:default")) {
		struct xccdf_default_score * item_score = xccdf_item_get_default_score(benchmark, test_result);
		xccdf_score_set_score(score, item_score->score);
		oscap_free(item_score);
	} else if (oscap_streq(score_system, "urn:xccdf:scoring:flat")) {
		struct xccdf_flat_score * item_score = xccdf_item_get_flat_score(benchmark, test_result, false);
		xccdf_score_set_maximum(score, item_score->weight);
		xccdf_score_set_score(score, item_score->score);
		oscap_free(item_score);
	} else if (oscap_streq(score_system, "urn:xccdf:scoring:flat-unweighted")) {
		struct xccdf_flat_score * item_score = xccdf_item_get_flat_score(benchmark, test_result, true);
		xccdf_score_set_maximum(score, item_score->weight);
		xccdf_score_set_score(score, item_score->score);
		oscap_free(item_score);
	} else if (oscap_streq(score_system, "urn:xccdf:scoring:absolute")) {
		int absolute;
		struct xccdf_flat_score * item_score = xccdf_item_get_flat_score(benchmark, test_result, false);
		xccdf_score_set_maximum(score, item_score->weight);
		absolute = (item_score->score == item_score->weight);
		xccdf_score_set_score(score, absolute);
		oscap_free(item_score);
	} else {
		xccdf_score_free(score);
		dE("Scoring system \"%s\" is not supported.", score_system);
		return NULL;
	}
	return score;
}

int xccdf_result_recalculate_scores(struct xccdf_result *result, struct xccdf_item *benchmark)
{
	struct oscap_list *new_scores = oscap_list_new();
	struct xccdf_score_iterator *score_it = xccdf_result_get_scores(result);
	while (xccdf_score_iterator_has_more(score_it)) {
		struct xccdf_score *old = xccdf_score_iterator_next(score_it);
		struct xccdf_score *new = xccdf_result_calculate_score(result, benchmark,
			xccdf_score_get_system(old));
		if (new == NULL) {
			oscap_list_free(new_scores, (oscap_destruct_func) xccdf_score_free);
			xccdf_score_iterator_free(score_it);
			return 1;
		}
		oscap_list_add(new_scores, new);
	}
	xccdf_score_iterator_free(score_it);
	oscap_list_free(((struct xccdf_item *)result)->sub.result.scores, (oscap_destruct_func) xccdf_score_free);
        ((struct xccdf_item *)result)->sub.result.scores = new_scores;
	return 0;
}
