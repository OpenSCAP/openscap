/**
 * @file   networkfirewall_probe.c
 * @brief  networkfirewall probe
 * @author "Gwendal Didot" <gwendal.didot@soprasteria.com>
 * @author "Antoine Orhant"  <antoine.orhant@soprasteria.com>
 *
 */

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
 *      "Antoine Orhant <antoine.orhant@soprasteria.com>" 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>

// OpenSCAP libraries
#include <probe/probe.h>
#include <debug_priv.h>
#include "../OVAL/probes/probe/entcmp.h"

// probe include
#include "networkfirewall_probe.h"

const int RULESET_BUF = 256;

int get_packet_direction(SEXP_t *pck_dir)
{
	int result = 0;
	SEXP_t *INCOMING_SEXP = SEXP_string_newf("INCOMING");
	SEXP_t *OUTGOING_SEXP = SEXP_string_newf("OUTGOING");
	SEXP_t *FORWARDING_SEXP = SEXP_string_newf("FORWARDING");

	// Get the expected packet direction the rules to collect should use
	if (probe_entobj_cmp(pck_dir, INCOMING_SEXP) == OVAL_RESULT_TRUE) {
		result |= INCOMING;
	}
	if (probe_entobj_cmp(pck_dir, OUTGOING_SEXP) == OVAL_RESULT_TRUE) {
		result |= OUTGOING;
	}
	if (probe_entobj_cmp(pck_dir, FORWARDING_SEXP) == OVAL_RESULT_TRUE) {
		result |= FORWARDING;
	}
	SEXP_free(INCOMING_SEXP);
	SEXP_free(OUTGOING_SEXP);
	SEXP_free(FORWARDING_SEXP);
	return result;
}

enum packet_direction convert_hook_to_packet_dir(const char *hook)
{
	if (strncmp(hook, "input", 5) == 0) {
		return INCOMING;
	} else if (strncmp(hook, "output", 6) == 0) {
		return OUTGOING;
	} else if (strncmp(hook, "forward", 7) == 0) {
		return FORWARDING;
	} else {
		return UNSUPPORTED;
	}
}

struct oscap_list *gather_chain(json_t *ruleset)
{
	struct oscap_list *chain_list = oscap_list_new();
	json_t *nftable_child;
	size_t index;

	json_array_foreach(ruleset, index, nftable_child)
	{
		json_t *chain = json_object_get(nftable_child, "chain");
		if (chain != NULL) {
			json_incref(chain);
			oscap_list_push(chain_list, chain);
		}
	}
	return chain_list;
}

struct json_t *get_ruleset(probe_ctx *ctx)
{
	struct oscap_string *ruleset = oscap_string_new();
	FILE *cmd_ptr;
	char ruleset_buf[RULESET_BUF];
	// Call to the nftable CLI to get the current ruleset on the system.
	// The option -j return the result as a JSON and the option -nn convert protocol value to their
	// numerical port value (http -> 80, https -> 443)
	cmd_ptr = popen("nft -j -nn list ruleset", "r");

	if (cmd_ptr == NULL) {
		int errnum = errno;
		dE("Failed to request the kernel for ruleset : %s",
		   strerror(errnum));
		SEXP_t *item =
			probe_item_create(OVAL_UNIX_NETWORKFIREWALL, NULL);

		probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);
		probe_item_add_msg(
			item, OVAL_MESSAGE_LEVEL_ERROR,
			"Failed to request the kernel for ruleset : %s.",
			strerror(errnum));
		probe_item_collect(ctx, item);
		return NULL;
	}

	while (fgets(ruleset_buf, sizeof(ruleset_buf), cmd_ptr) != NULL) {
		oscap_string_append_string(ruleset, ruleset_buf);
	}
	pclose(cmd_ptr);

	json_error_t error;
	struct json_t *ruleset_json = json_loads(oscap_string_get_cstr(ruleset),
						 JSON_DECODE_ANY, &error);

	oscap_string_free(ruleset);

	return ruleset_json;
}

void filter_chain(struct ruleset *ruleset, struct oscap_list *chain_list,
		  int filter)
{
	struct oscap_iterator *chain_list_it = oscap_iterator_new(chain_list);
	while (oscap_iterator_has_more(chain_list_it)) {
		const struct json_object *chain =
			oscap_iterator_next(chain_list_it);
		struct json_t *hook = json_object_get(chain, "hook");
		struct json_t *chain_name = json_object_get(chain, "name");
		if (hook == NULL) {
			continue;
		}
		enum packet_direction chain_pck_dir =
			convert_hook_to_packet_dir(json_string_value(hook));
		if (chain_pck_dir == UNSUPPORTED) {
			// If the hook is not a supported packet_direction, the chain is skipped
			continue;
		}
		if (filter & chain_pck_dir) {
			oscap_stringlist_add_string(
				ruleset->chain_list,
				json_string_value(chain_name));
		}
	}
	oscap_iterator_free(chain_list_it);
}

bool _rule_chain_cmp(const char *rule, const char *chain)
{
	// The "forward" value is the longest value supported by the probe
	return strncmp(rule, chain, 7) == 0;
}

struct rule *rule_new()
{
	struct rule *rule = malloc(sizeof(struct rule));
	return rule;
}

void rule_free(struct rule *rule)
{
	free(rule);
}

const char *get_iifname(struct rule *rule)
{
	struct json_t *expr = json_object_get(rule->rule_json, "expr");
	json_t *expr_child;
	size_t index;
	const char *interface_name = NULL;
	const char *meta_value = NULL;
	const char *op_value = NULL;

	json_array_foreach(expr, index, expr_child)
	{
		if (json_unpack(expr_child, "{s:{s:{s:s}, s:s,s?s}}", "match",
				"left", "meta", &meta_value, "right",
				&interface_name, "op", &op_value) == 0) {
			if (strncmp(meta_value, "iifname", 7) != 0) {
				continue;
			}
			if (strstr(interface_name, "*") != NULL) {
				continue;
			}
			if (interface_name[0] == '@') {
				continue;
			}
			if (op_value != NULL) {
				continue;
			}
			return interface_name;
		}
	}
	return "";
}

const char *get_oifname(struct rule *rule)
{
	struct json_t *expr = json_object_get(rule->rule_json, "expr");
	json_t *expr_child;
	size_t index;
	const char *interface_name = NULL;
	const char *meta_value = NULL;
	const char *op_value = NULL;

	json_array_foreach(expr, index, expr_child)
	{
		dD("%s", json_dumps(expr_child, 0));
		if (json_unpack(expr_child, "{s:{s:{s:s}, s:s,s?s}}", "match",
				"left", "meta", &meta_value, "right",
				&interface_name, "op", &op_value) == 0) {
			if (strncmp(meta_value, "oifname", 7) != 0) {
				continue;
			}
			if (strstr(interface_name, "*") != NULL) {
				continue;
			}
			if (interface_name[0] == '@') {
				continue;
			}
			if (op_value != NULL) {
				continue;
			}
			return interface_name;
		}
	}
	return "";
}

const char *get_protocol(struct rule *rule)
{
	struct json_t *expr = json_object_get(rule->rule_json, "expr");
	json_t *expr_child;
	size_t index;
	const char *protocol_name = NULL;
	const char *field_name = NULL;
	const char *ip_field_name = NULL;
	const char *str_field_value = NULL;
	const char *key_value = NULL;
	struct json_t *field_value;
	const char *op_value = NULL;

	json_array_foreach(expr, index, expr_child)
	{
		// ip protocol [tcp,udp,sctp]
		if (json_unpack(expr_child, "{s:{s:{s:{s:s, s:s}}, s:s, s?:s}}",
				"match", "left", "payload", "name",
				&ip_field_name, "field", &field_name, "right",
				&str_field_value, "op", &op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(ip_field_name, "ip", 2) == 0 &&
			    strncmp(field_name, "protocol", 8) == 0) {
				if (strncmp(str_field_value, "tcp", 3) == 0 ||
				    strncmp(str_field_value, "udp", 3) == 0 ||
				    strncmp(str_field_value, "sctp", 4) == 0) {
					return str_field_value;
				}
			}
		}
		// ct [original/reply] protocol [tcp,udp,sctp]
		if (json_unpack(expr_child, "{s:{s:{s:{s:s}}, s:s, s?:s}}",
				"match", "left", "ct", "key", &key_value,
				"right", &protocol_name, "op",
				&op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(key_value, "protocol", 8) == 0) {
				if (strncmp(protocol_name, "tcp", 3) == 0 ||
				    strncmp(protocol_name, "udp", 3) == 0 ||
				    strncmp(protocol_name, "sctp", 4) == 0) {
					return protocol_name;
				}
			}
		}
		// [tcp, udp, sctp] xxx
		if (json_unpack(expr_child, "{s:{s:{s:{s:s, s:s}}, s:o, s?:s}}",
				"match", "left", "payload", "name",
				&protocol_name, "field", &field_name, "right",
				&field_value, "op", &op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(protocol_name, "tcp", 3) == 0 ||
			    strncmp(protocol_name, "udp", 3) == 0 ||
			    strncmp(protocol_name, "sctp", 4) == 0) {
				return protocol_name;
			}
		}
	}
	return "";
}

struct addr_info get_saddr(struct rule *rule)
{
	struct json_t *expr = json_object_get(rule->rule_json, "expr");
	json_t *expr_child;
	size_t index;
	char *ip_buf = malloc(512 * sizeof(char));
	const char *payload_name = NULL;
	const char *payload_field = NULL;
	const char *field_value = NULL;
	const char *op_value = NULL;
	int prefix_len = -1;

	json_array_foreach(expr, index, expr_child)
	{
		// [ip,ip6] saddr xxx
		if (json_unpack(expr_child, "{s:{s:{s:{s:s, s:s}}, s:s, s?:s}}",
				"match", "left", "payload", "name",
				&payload_name, "field", &payload_field, "right",
				&field_value, "op", &op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			// ip and ip6 are cover by the first strncmp
			if (strncmp(payload_name, "ip", 2) == 0 &&
			    strncmp(payload_field, "saddr", 5) == 0) {
				if (field_value[0] == '@') {
					continue;
				}
				if (strncmp(payload_name, "ip6", 3) != 0) {
					struct addr_info ip_addr = {
						.addr = field_value,
						.type = IPV4,
						.managed = true
					};
					free(ip_buf);
					return ip_addr;
				} else {
					struct addr_info ip_addr = {
						.addr = field_value,
						.type = IPV6,
						.managed = true
					};
					free(ip_buf);
					return ip_addr;
				}
			}
		}
		// [ip,ip6] saddr xxx/yy
		if (json_unpack(expr_child,
				"{s:{s:{s:{s:s, s:s}}, s:{s:{s:s, s:i}}, s?:s}}",
				"match", "left", "payload", "name",
				&payload_name, "field", &payload_field, "right",
				"prefix", "addr", &field_value, "len",
				&prefix_len, "op", &op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(payload_name, "ip", 2) == 0 &&
			    strncmp(payload_field, "saddr", 5) == 0) {
				if (field_value[0] == '@') {
					continue;
				}

				snprintf(ip_buf, 512, "%s/%d", field_value,
					 prefix_len);
				if (strncmp(payload_name, "ip6", 3) != 0) {
					struct addr_info ip_addr = {
						.addr = ip_buf,
						.type = IPV4,
						.managed = false
					};
					return ip_addr;
				} else {
					struct addr_info ip_addr = {
						.addr = ip_buf,
						.type = IPV6,
						.managed = false
					};
					return ip_addr;
				}
			}
		}

		const char *key = NULL;
		const char *dir = NULL;
		const char *family = NULL;
		const char *key_value = NULL;
		// ct [original,reply] ip saddr xxx
		if (json_unpack(expr_child,
				"{s:{s:{s:{s:s, s:s, s:s}}, s:s, s?:s}}",
				"match", "left", "ct", "key", &key, "dir", &dir,
				"family", &family, "right", &key_value, "op",
				&op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(family, "ip", 2) == 0 &&
			    strncmp(key, "saddr", 5) == 0) {
				if (key_value[0] == '@') {
					continue;
				}
				struct addr_info ip_addr = { .addr = key_value,
							     .type = IPV4,
							     .managed = true };
				free(ip_buf);
				return ip_addr;
			}
		}
		// ct [original,reply] ip saddr xxx/yy
		if (json_unpack(
			    expr_child,
			    "{s:{s:{s:{s:s, s:s, s:s}}, s:{s:{s:s, s:i}}, s?:s}}",
			    "match", "left", "ct", "key", &key, "dir", &dir,
			    "family", &family, "right", "prefix", "addr",
			    &key_value, "len", &prefix_len, "op",
			    &op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(family, "ip", 2) == 0 &&
			    strncmp(key, "saddr", 5) == 0) {
				if (key_value[0] == '@') {
					continue;
				}
				snprintf(ip_buf, 512, "%s/%d", key_value,
					 prefix_len);
				struct addr_info ip_addr = { .addr = ip_buf,
							     .type = IPV4,
							     .managed = false };
				return ip_addr;
			}
		}
	}

	free(ip_buf);

	struct addr_info default_addr = { .addr = "",
					  .type = NULL_IP,
					  .managed = false };

	return default_addr;
}

struct addr_info get_daddr(struct rule *rule)
{
	struct json_t *expr = json_object_get(rule->rule_json, "expr");
	json_t *expr_child;
	size_t index;
	char *ip_buf = malloc(512 * sizeof(char));
	const char *payload_name = NULL;
	const char *payload_field = NULL;
	const char *field_value = NULL;
	const char *op_value = NULL;
	int prefix_len = -1;

	json_array_foreach(expr, index, expr_child)
	{
		// [ip,ip6] daddr xxx
		if (json_unpack(expr_child, "{s:{s:{s:{s:s, s:s}}, s:s, s?:s}}",
				"match", "left", "payload", "name",
				&payload_name, "field", &payload_field, "right",
				&field_value, "op", &op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			// ip and ip6 are cover by the first strncmp
			if (strncmp(payload_name, "ip", 2) == 0 &&
			    strncmp(payload_field, "daddr", 5) == 0) {
				if (field_value[0] == '@') {
					continue;
				}
				if (strncmp(payload_name, "ip6", 3) != 0) {
					struct addr_info ip_addr = {
						.addr = field_value,
						.type = IPV4,
						.managed = true
					};
					free(ip_buf);
					return ip_addr;
				} else {
					struct addr_info ip_addr = {
						.addr = field_value,
						.type = IPV6,
						.managed = true
					};
					free(ip_buf);
					return ip_addr;
				}
			}
		}
		// [ip,ip6] daddr xxx/yy
		if (json_unpack(expr_child,
				"{s:{s:{s:{s:s, s:s}}, s:{s:{s:s, s:i}}, s?:s}}",
				"match", "left", "payload", "name",
				&payload_name, "field", &payload_field, "right",
				"prefix", "addr", &field_value, "len",
				&prefix_len, "op", &op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(payload_name, "ip", 2) == 0 &&
			    strncmp(payload_field, "daddr", 5) == 0) {
				if (field_value[0] == '@') {
					continue;
				}
				snprintf(ip_buf, 512, "%s/%d", field_value,
					 prefix_len);
				if (strncmp(payload_name, "ip6", 3) != 0) {
					struct addr_info ip_addr = {
						.addr = ip_buf,
						.type = IPV4,
						.managed = false
					};
					return ip_addr;
				} else {
					struct addr_info ip_addr = {
						.addr = ip_buf,
						.type = IPV6,
						.managed = false
					};
					return ip_addr;
				}
			}
		}

		const char *key = NULL;
		const char *dir = NULL;
		const char *family = NULL;
		const char *key_value = NULL;
		// ct [original,reply] ip daddr xxx
		if (json_unpack(expr_child,
				"{s:{s:{s:{s:s, s:s, s:s}}, s:s, s?:s}}",
				"match", "left", "ct", "key", &key, "dir", &dir,
				"family", &family, "right", &key_value, "op",
				&op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(family, "ip", 2) == 0 &&
			    strncmp(key, "daddr", 5) == 0) {
				if (key_value[0] == '@') {
					continue;
				}
				struct addr_info ip_addr = { .addr = key_value,
							     .type = IPV4,
							     .managed = true };
				free(ip_buf);
				return ip_addr;
			}
		}
		// ct [original,reply] ip daddr xxx/yy
		if (json_unpack(
			    expr_child,
			    "{s:{s:{s:{s:s, s:s, s:s}}, s:{s:{s:s, s:i}}, s?:s}}",
			    "match", "left", "ct", "key", &key, "dir", &dir,
			    "family", &family, "right", "prefix", "addr",
			    &key_value, "len", &prefix_len, "op",
			    &op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(family, "ip", 2) == 0 &&
			    strncmp(key, "daddr", 5) == 0) {
				if (key_value[0] == '@') {
					continue;
				}
				snprintf(ip_buf, 512, "%s/%d", key_value,
					 prefix_len);
				struct addr_info ip_addr = { .addr = ip_buf,
							     .type = IPV4,
							     .managed = false };
				return ip_addr;
			}
		}
	}

	free(ip_buf);

	struct addr_info default_addr = { .addr = "",
					  .type = NULL_IP,
					  .managed = false };

	return default_addr;
}

int get_sport(struct rule *rule)
{
	struct json_t *expr = json_object_get(rule->rule_json, "expr");
	json_t *expr_child;
	size_t index;

	const char *field_name = NULL;
	const char *ip_field_name = NULL;
	int int_field_value = -1;
	const char *op_value = NULL;
	const char *key_value = NULL;
	const char *dir_value = NULL;

	json_array_foreach(expr, index, expr_child)
	{
		if (json_unpack(expr_child, "{s:{s:{s:{s:s, s:s}}, s:i, s?:s}}",
				"match", "left", "payload", "name",
				&ip_field_name, "field", &field_name, "right",
				&int_field_value, "op", &op_value) == 0) {
			if (strncmp(ip_field_name, "tcp", 3) == 0 ||
			    strncmp(ip_field_name, "udp", 3) == 0 ||
			    strncmp(ip_field_name, "sctp", 4) == 0) {
				if (op_value != NULL) {
					continue;
				}
				if (strncmp(field_name, "sport", 5) == 0) {
					return int_field_value;
				}
			}
		}

		if (json_unpack(expr_child, "{s:{s:{s:{s:s,s:s}}, s:i, s?:s}}",
				"match", "left", "ct", "key", &key_value, "dir",
				&dir_value, "right", &int_field_value, "op",
				&op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(key_value, "proto-src", 8) == 0) {
				return int_field_value;
			}
		}
	}
	return -1;
}

int get_dport(struct rule *rule)
{
	struct json_t *expr = json_object_get(rule->rule_json, "expr");
	json_t *expr_child;
	size_t index;

	const char *field_name = NULL;
	const char *ip_field_name = NULL;
	int int_field_value = -1;
	const char *op_value = NULL;
	const char *key_value = NULL;
	const char *dir_value = NULL;

	json_array_foreach(expr, index, expr_child)
	{
		if (json_unpack(expr_child, "{s:{s:{s:{s:s, s:s}}, s:i, s?:s}}",
				"match", "left", "payload", "name",
				&ip_field_name, "field", &field_name, "right",
				&int_field_value, "op", &op_value) == 0) {
			if (strncmp(ip_field_name, "tcp", 3) == 0 ||
			    strncmp(ip_field_name, "udp", 3) == 0 ||
			    strncmp(ip_field_name, "sctp", 4) == 0) {
				if (op_value != NULL) {
					continue;
				}
				if (strncmp(field_name, "dport", 5) == 0) {
					return int_field_value;
				}
			}
		}

		if (json_unpack(expr_child, "{s:{s:{s:{s:s,s:s}}, s:i, s?:s}}",
				"match", "left", "ct", "key", &key_value, "dir",
				&dir_value, "right", &int_field_value, "op",
				&op_value) == 0) {
			if (op_value != NULL) {
				continue;
			}
			if (strncmp(key_value, "proto-dst", 8) == 0) {
				return int_field_value;
			}
		}
	}
	return -1;
}

const char *get_filtering_action(struct rule *rule)
{
	struct json_t *expr = json_object_get(rule->rule_json, "expr");
	json_t *expr_child;
	size_t index;

	json_array_foreach(expr, index, expr_child)
	{
		if (json_unpack(expr_child, "{s:n}", "accept") == 0) {
			return "ALLOW";
		}
		if (json_unpack(expr_child, "{s:n}", "drop") == 0) {
			return "DENY";
		}
	}
	return "";
}

struct ruleset *ruleset_new(probe_ctx *ctx, int filter)
{
	struct ruleset *ruleset = malloc(sizeof(struct ruleset));

	ruleset->rules = oscap_list_new();
	ruleset->chain_list = oscap_stringlist_new();

	struct json_t *ruleset_json = get_ruleset(ctx);

	if (ruleset_json == NULL) {
		return NULL;
	}
	ruleset->ruleset = ruleset_json;

	//Gather the chain in the ruleset based on the packet_direction entity
	json_t *nftables;
	nftables = json_object_get(ruleset_json, "nftables");
	struct oscap_list *chain_list = gather_chain(nftables);

	filter_chain(ruleset, chain_list, filter);

	// Gather the rules in the ruleset base on the filtered chain the rules belong to
	json_t *nftables_child;
	size_t index;

	json_array_foreach(nftables, index, nftables_child)
	{
		json_t *rule_json = json_object_get(nftables_child, "rule");
		if (rule_json == NULL) {
			// The current element is not a rule
			continue;
		}
		json_t *rule_chain_target = json_object_get(rule_json, "chain");
		if (rule_chain_target == NULL) {
			// The rule doesn't specify the chain she is membre of, this case should not appear
			// unless the return of nft is ill-formed or changed
			dW("A rule doesn't specify a chain, maybe the nft executable is a malicious one.");
			continue;
		}
		if (oscap_list_contains(
			    (struct oscap_list *)ruleset->chain_list,
			    (void *)json_string_value(rule_chain_target),
			    (oscap_cmp_func)_rule_chain_cmp)) {
			struct rule *rule = rule_new();
			rule->rule_json = rule_json;
			oscap_list_push(ruleset->rules, rule);
		}
	}
	oscap_list_free(chain_list, json_decref);
	return ruleset;
}

void ruleset_free(struct ruleset *ruleset)
{
	oscap_list_free(ruleset->rules, (oscap_destruct_func)rule_free);
	oscap_stringlist_free(ruleset->chain_list);

	while (ruleset->ruleset->refcount > 1) {
		json_decref(ruleset->ruleset);
	}
	json_decref(ruleset->ruleset);
}

int networkfirewall_probe_offline_mode_supported(void)
{
	return PROBE_OFFLINE_CHROOT;
}

int networkfirewall_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in = probe_ctx_getobject(ctx);
	SEXP_t *packet_direction =
		probe_obj_getent(probe_in, "packet_direction", 1);
	SEXP_t *input_interface =
		probe_obj_getent(probe_in, "input_interface", 1);
	SEXP_t *input_interface_nil = probe_ent_getval(input_interface);
	SEXP_t *output_interface =
		probe_obj_getent(probe_in, "output_interface", 1);
	SEXP_t *output_interface_nil = probe_ent_getval(output_interface);
	SEXP_t *filtering_action =
		probe_obj_getent(probe_in, "filtering_action", 1);
	SEXP_t *filtering_action_value = probe_ent_getval(filtering_action);
	SEXP_t *sexp_packet_dir_val = probe_ent_getval(packet_direction);
	char *str_packet_direction = SEXP_string_cstr(sexp_packet_dir_val);
	bool isFilteringActionNil = filtering_action_value == NULL;
	char *str_filtering_action;
	if (!isFilteringActionNil) {
		str_filtering_action = SEXP_string_cstr(filtering_action_value);
	}
	bool isInInterfaceNil = input_interface_nil == NULL;
	bool isOutInterfaceNil = output_interface_nil == NULL;
	struct ruleset *ruleset =
		ruleset_new(ctx, get_packet_direction(packet_direction));
	if (ruleset != NULL) {
		struct oscap_iterator *ruleset_rule_it =
			oscap_iterator_new(ruleset->rules);

		while (oscap_iterator_has_more(ruleset_rule_it)) {
			struct rule *current_rule =
				oscap_iterator_next(ruleset_rule_it);
			const char *filtering_action_val =
				get_filtering_action(current_rule);
			if (!isFilteringActionNil) {
				if (strncmp(str_filtering_action,
					    filtering_action_val, 5) != 0) {
					// The rule does not match with the object, it get ignored
					continue;
				}
			}

			SEXP_t *new_item = probe_item_create(
				OVAL_UNIX_NETWORKFIREWALL, NULL,
				"filtering_action", OVAL_DATATYPE_STRING,
				filtering_action_val, "packet_direction",
				OVAL_DATATYPE_STRING, str_packet_direction);

			if (!isInInterfaceNil) {
				const char *iifname = get_iifname(current_rule);
				if (iifname[0] != '\0') {
					SEXP_t *sexp_iifname = SEXP_string_new(
						iifname, strlen(iifname));

					if (probe_entobj_cmp(input_interface,
							     sexp_iifname) ==
					    OVAL_RESULT_TRUE) {
						probe_item_ent_add(
							new_item,
							"input_interface", NULL,
							sexp_iifname);
					} else {
						continue;
					}
				} else {
					continue;
				}
			}
			if (!isOutInterfaceNil) {
				const char *oifname = get_oifname(current_rule);
				if (oifname[0] != '\0') {
					SEXP_t *sexp_oifname = SEXP_string_new(
						oifname, strlen(oifname));

					if (probe_entobj_cmp(output_interface,
							     sexp_oifname) ==
					    OVAL_RESULT_TRUE) {
						probe_item_ent_add(
							new_item,
							"output_interface",
							NULL, sexp_oifname);
					} else {
						continue;
					}
				} else {
					continue;
				}
			}

			const char *transport_protocol =
				get_protocol(current_rule);
			if (transport_protocol[0] != '\0') {
				SEXP_t *sexp_transport_protocol =
					SEXP_string_new(
						transport_protocol,
						strlen(transport_protocol));
				probe_item_ent_add(new_item,
						   "transport_protocol", NULL,
						   sexp_transport_protocol);
				SEXP_string_free(sexp_transport_protocol);
			}

			struct addr_info src_addr = get_saddr(current_rule);
			if (src_addr.addr[0] != '\0') {
				SEXP_t *sexp_saddr = SEXP_string_new(
					src_addr.addr, strlen(src_addr.addr));
				probe_item_ent_add(new_item,
						   "source_inet_address", NULL,
						   sexp_saddr);
				SEXP_string_free(sexp_saddr);
				SEXP_t *new_ent = probe_item_getent(
					new_item, "source_inet_address", 1);
				switch (src_addr.type) {
				case IPV4:
					probe_ent_setdatatype(
						new_ent,
						OVAL_DATATYPE_IPV4ADDR);
					break;
				case IPV6:
					probe_ent_setdatatype(
						new_ent,
						OVAL_DATATYPE_IPV6ADDR);
					break;
				default:
					break;
				}
				SEXP_free(new_ent);
				if (!src_addr.managed) {
					free((char *)src_addr.addr);
				}
			}

			struct addr_info dest_addr = get_daddr(current_rule);
			if (dest_addr.addr[0] != '\0') {
				SEXP_t *sexp_daddr = SEXP_string_new(
					dest_addr.addr, strlen(dest_addr.addr));
				probe_item_ent_add(new_item,
						   "destination_inet_address",
						   NULL, sexp_daddr);
				SEXP_string_free(sexp_daddr);
				SEXP_t *new_ent = probe_item_getent(
					new_item, "destination_inet_address",
					1);
				switch (src_addr.type) {
				case IPV4:
					probe_ent_setdatatype(
						new_ent,
						OVAL_DATATYPE_IPV4ADDR);
					break;
				case IPV6:
					probe_ent_setdatatype(
						new_ent,
						OVAL_DATATYPE_IPV6ADDR);
					break;
				default:
					break;
				}
				SEXP_free(new_ent);
				if (!dest_addr.managed) {
					free((char *)dest_addr.addr);
				}
			}

			const int src_port = get_sport(current_rule);
			if (src_port >= 0) {
				SEXP_t *sexp_sport =
					SEXP_number_newi_32(src_port);
				probe_item_ent_add(new_item, "source_port",
						   NULL, sexp_sport);
				SEXP_number_free(sexp_sport);
			}

			const int dest_port = get_dport(current_rule);
			if (dest_port >= 0) {
				SEXP_t *sexp_dport =
					SEXP_number_newi_32(dest_port);
				probe_item_ent_add(new_item, "destination_port",
						   NULL, sexp_dport);
				SEXP_number_free(sexp_dport);
			}

			probe_item_collect(ctx, new_item);
		}

		oscap_iterator_free(ruleset_rule_it);
	}
	free(str_packet_direction);

	SEXP_free(sexp_packet_dir_val);
	SEXP_free(packet_direction);
	SEXP_free(input_interface_nil);
	SEXP_free(input_interface);
	SEXP_free(output_interface_nil);
	SEXP_free(output_interface);
	SEXP_free(filtering_action_value);
	SEXP_free(filtering_action);

	ruleset_free(ruleset);
	return 0;
}