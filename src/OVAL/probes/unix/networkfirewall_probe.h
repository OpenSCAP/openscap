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
#ifndef OPENSCAP_NETWORKFIREWALL_PROBE_H
#define OPENSCAP_NETWORKFIREWALL_PROBE_H

#include "probe-api.h"
#include <oscap_string.h>
#include <jansson.h>
#include <list.h>

struct ruleset {
	// JSON representation of the current system ruleset
	struct json_t *ruleset;
	// List of the chains name that correspond to the packet_direction entity
	struct oscap_stringlist *chain_list;
	// List of the rules filtered by the packet direction
	struct oscap_list *rules;
};

// Direction of the packet in the system network stack
enum packet_direction {
	INCOMING = 1,
	OUTGOING = 2,
	FORWARDING = 4,
	UNSUPPORTED = 8
};
struct rule {
	// JSON representation of the rule
	struct json_t *rule_json;
};

/**
 * Method use to get the current ruleset of nftable
 * @param ctx Probe context
 * @return A JSON object representing the ruleset
 */
struct json_t *get_ruleset(probe_ctx *ctx);

/**
 * Constructor for a ruleset object
 * This methode allocate an oscap_list
 * @param ctx The probe context
 * @param filter filter use to indicate in which chain the rules should be collected, this filter correspond to the packet_direction entity
 * @return A pointer to a new instance of a ruleset
 */
struct ruleset *ruleset_new(probe_ctx *ctx, int filter);

/**
 * Destructor of the ruleset struct
 * @param ruleset Ruleset struct to free
 */
void ruleset_free(struct ruleset *ruleset);

/**
 * Constructor for a rule object
 * @return A pointer to a new instance of a rule
 */
struct rule *rule_new();

/**
 * Destructor of the rule struct
 * @param rule rule struct to free 
 */
void rule_free(struct rule *rule);

/**
 * Method that try to get the iifname value in the rule
 * @param rule a rule of the ruleset
 * @return The iifname value
 */
const char *get_iifname(struct rule *rule);

/**
 * Method that try to get the oifname value in the rule
 * @param rule a rule of the ruleset
 * @return The oifname value
 */
const char *get_oifname(struct rule *rule);

// Type of addresses use in rules
enum addr_type { IPV4 = 0, IPV6 = 1, NULL_IP = 3 };

// Struct use to provide information about adress use by a rule
struct addr_info {
	// string representation of an ipv4/ipv6 adress
	const char *addr;
	// type of the adress
	enum addr_type type;
	// bool use to know if the addr member is managed or not
	bool managed;
};

/**
 * Method that try to get the source address value in the rule
 * @param rule a rule of the ruleset
 * @return A struct representing the source address
 */
struct addr_info get_saddr(struct rule *rule);

/**
 * Method that try to get the destination address value in the rule
 * @param rule a rule of the ruleset
 * @return A struct representing the destination address
 */
struct addr_info get_daddr(struct rule *rule);

/**
 * Method that return the protocol value in the rule
 * @param rule a rule of the ruleset 
 * @return The protocol value 
 */
const char *get_protocol(struct rule *rule);

/**
 * Method that return the source port in the rule
 * @param rule a rule of the ruleset 
 * @return The source port value 
 */
int get_sport(struct rule *rule);

/**
 * Method that return the destination port in the rule
 * @param rule a rule of the ruleset 
 * @return The destination port value 
 */
int get_dport(struct rule *rule);

/**
 * Method that return the rule filtering action
 * @param rule a rule of the ruleset
 * @return The filtering action of the rule
 */
const char *get_filtering_action(struct rule *rule);

/**
 * Method that return a int corresponding to combination of the bitflag describe by the enum packet_direction
 * @param pck_dir packet_direction entity
 * @return A bitflag of the possible packet_direction values
 */
int get_packet_direction(SEXP_t *pck_dir);

/**
 * Method that return all the chain in the ruleset
 * This method allocate an oscap_list
 * @param ruleset the current nftable ruleset
 * @return an list of the different chain in the ruleset
 */
struct oscap_list *gather_chain(json_t *ruleset);

/**
 * A method use to filter the ruleset chains by the packet_direction value
 * @param ruleset the current nftable ruleset
 * @param chain_list list of the ruleset chains
 * @param filter bitflag combination of the possible packet_direction values 
 */
void filter_chain(struct ruleset *ruleset, struct oscap_list *chain_list,
		  int filter);

/**
 * Convert a string to an instance of the packet_direction enum
 * @param hook a chain hook value
 * @return a value of the packet_direction enum
 */
enum packet_direction convert_hook_to_packet_dir(const char *hook);

int networkfirewall_probe_offline_mode_supported(void);
int networkfirewall_probe_main(probe_ctx *ctx, void *arg);
// Internal method to do comparison
bool _rule_chain_cmp(const char *rule, const char *chain);

#endif /* OPENSCAP_NETWORKFIREWALL_PROBE_H */