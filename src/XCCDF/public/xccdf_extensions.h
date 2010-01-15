/*
 * xccdf_extensions.h
 *
 *  Created on: Jan 7, 2010
 *      Author: david.niemoller
 */

#ifndef XCCDF_EXTENSIONS_H_
#define XCCDF_EXTENSIONS_H_

#include <xccdf.h>
#include <wchar.h>

/*--------------------*\
|   Benchmark methods  |
\*--------------------*/

/**
 * Create a new benchmark from a benchmark XML file.
 * @relates xccdf_benchmark
 * @param id the benchmark identifier.
 * @return Pointer to the new benchmark.
 */
struct xccdf_benchmark *xccdf_benchmark_create(const char *id);

/**
 * Create a group and append it to the benchmark.
 * @param id - the identifier of the appended value.
 * @return the handle of the new group.
 */
struct xccdf_group *xccdf_benchmark_append_new_group(const struct xccdf_benchmark *, const char *id);

/**
 * Create a value and append it to the benchmark.
 * @param id - the identifier of the appended value.
 * @return the handle of the new value.
 */
struct xccdf_value *xccdf_benchmark_append_new_value(const struct xccdf_benchmark *, const char *id, xccdf_value_type_t type);

/**
 * Create a rule and append it to the benchmark.
 * @param id - the identifier of the appended rule.
 * @return the handle of the new rule.
 */
struct xccdf_rule *xccdf_benchmark_append_new_rule(const struct xccdf_benchmark *, const char *id);



void xccdf_item_set_title(const struct xccdf_item* item, const char *title);
void xccdf_benchmark_set_title(const struct xccdf_benchmark* item, const char *title);
void xccdf_profile_set_title(const struct xccdf_profile* item, const char *title);
void xccdf_rule_set_title(const struct xccdf_rule* item, const char *title);
void xccdf_value_set_title(const struct xccdf_value* item, const char *title);
void xccdf_group_set_title(const struct xccdf_group* item, const char *title);
void xccdf_item_set_description(const struct xccdf_item* item, const char *description);
void xccdf_benchmark_set_description(const struct xccdf_benchmark* item, const char *description);
void xccdf_profile_set_description(const struct xccdf_profile* item, const char *description);
void xccdf_rule_set_description(const struct xccdf_rule* item, const char *description);
void xccdf_value_set_description(const struct xccdf_value* item, const char *description);
void xccdf_group_set_description(const struct xccdf_group* item, const char *description);
void xccdf_item_set_question(const struct xccdf_item* item, const char *question);
void xccdf_benchmark_set_question(const struct xccdf_benchmark* item, const char *question);
void xccdf_profile_set_question(const struct xccdf_profile* item, const char *question);
void xccdf_rule_set_question(const struct xccdf_rule* item, const char *question);
void xccdf_value_set_question(const struct xccdf_value* item, const char *question);
void xccdf_group_set_question(const struct xccdf_group* item, const char *question);
void xccdf_item_set_rationale(const struct xccdf_item* item, const char *rationale);
void xccdf_benchmark_set_rationale(const struct xccdf_benchmark* item, const char *rationale);
void xccdf_profile_set_rationale(const struct xccdf_profile* item, const char *rationale);
void xccdf_rule_set_rationale(const struct xccdf_rule* item, const char *rationale);
void xccdf_value_set_rationale(const struct xccdf_value* item, const char *rationale);
void xccdf_group_set_rationale(const struct xccdf_group* item, const char *rationale);
void xccdf_item_set_cluster_id(const struct xccdf_item* item, const char *cluster_id);
void xccdf_benchmark_set_cluster_id(const struct xccdf_benchmark* item, const char *cluster_id);
void xccdf_profile_set_cluster_id(const struct xccdf_profile* item, const char *cluster_id);
void xccdf_rule_set_cluster_id(const struct xccdf_rule* item, const char *cluster_id);
void xccdf_value_set_cluster_id(const struct xccdf_value* item, const char *cluster_id);
void xccdf_group_set_cluster_id(const struct xccdf_group* item, const char *cluster_id);
void xccdf_item_set_version(const struct xccdf_item* item, const char *version);
void xccdf_benchmark_set_version(const struct xccdf_benchmark* item, const char *version);
void xccdf_profile_set_version(const struct xccdf_profile* item, const char *version);
void xccdf_rule_set_version(const struct xccdf_rule* item, const char *version);
void xccdf_value_set_version(const struct xccdf_value* item, const char *version);
void xccdf_group_set_version(const struct xccdf_group* item, const char *version);
void xccdf_item_set_version_update(const struct xccdf_item* item, const char *version_update);
void xccdf_benchmark_set_version_update(const struct xccdf_benchmark* item, const char *version_update);
void xccdf_profile_set_version_update(const struct xccdf_profile* item, const char *version_update);
void xccdf_rule_set_version_update(const struct xccdf_rule* item, const char *version_update);
void xccdf_value_set_version_update(const struct xccdf_value* item, const char *version_update);
void xccdf_group_set_version_update(const struct xccdf_group* item, const char *version_update);

void xccdf_item_set_weight(const struct xccdf_item* item, float weight);
void xccdf_benchmark_set_weight(const struct xccdf_benchmark* item, float weight);
void xccdf_profile_set_weight(const struct xccdf_profile* item, float weight);
void xccdf_rule_set_weight(const struct xccdf_rule* item, float weight);
void xccdf_value_set_weight(const struct xccdf_value* item, float weight);
void xccdf_group_set_weight(const struct xccdf_group* item, float weight);
void xccdf_item_set_version_time(const struct xccdf_item* item, time_t version_time);
void xccdf_benchmark_set_version_time(const struct xccdf_benchmark* item, time_t version_time);
void xccdf_profile_set_version_time(const struct xccdf_profile* item, time_t version_time);
void xccdf_rule_set_version_time(const struct xccdf_rule* item, time_t version_time);
void xccdf_value_set_version_time(const struct xccdf_value* item, time_t version_time);
void xccdf_group_set_version_time(const struct xccdf_group* item, time_t version_time);
void xccdf_item_set_extends(const struct xccdf_item* item, struct xccdf_item * extends);
void xccdf_benchmark_set_extends(const struct xccdf_benchmark* item, struct xccdf_item * extends);
void xccdf_profile_set_extends(const struct xccdf_profile* item, struct xccdf_item * extends);
void xccdf_rule_set_extends(const struct xccdf_rule* item, struct xccdf_item * extends);
void xccdf_value_set_extends(const struct xccdf_value* item, struct xccdf_item * extends);
void xccdf_group_set_extends(const struct xccdf_group* item, struct xccdf_item * extends);
void xccdf_item_set_parent(const struct xccdf_item* item, struct xccdf_item * parent);
void xccdf_benchmark_set_parent(const struct xccdf_benchmark* item, struct xccdf_item * parent);
void xccdf_profile_set_parent(const struct xccdf_profile* item, struct xccdf_item * parent);
void xccdf_rule_set_parent(const struct xccdf_rule* item, struct xccdf_item * parent);
void xccdf_value_set_parent(const struct xccdf_value* item, struct xccdf_item * parent);
void xccdf_group_set_parent(const struct xccdf_group* item, struct xccdf_item * parent);

#endif /* XCCDF_EXTENSIONS_H_ */
