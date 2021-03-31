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


struct json_t *get_ruleset(probe_ctx *ctx);

/**
 * Constructor for a ruleset object
 * This methode allocate an oscap_list
 */
struct ruleset* ruleset_new(probe_ctx *ctx,int filter);

void ruleset_free(struct ruleset* ruleset);

/**
 * Constructor for a rule object
 */
struct rule* rule_new();

void rule_free(struct rule *rule);

/**
 * Method that try to get the iifname value in the rule
 * @return The iifname value
 */
const char* get_iifname(struct rule* rule);

enum addr_type {
    IPV4 = 0,
    IPV6 = 1,
    NULL_IP = 3
};

struct addr_info {
    const char *addr;
    enum addr_type type;
};

/**
 * Method that try to get the source address value in the rule
 * @return The iifname value
 */
struct addr_info get_saddr(struct rule* rule);

/**
 * Method that try to get the destination address value in the rule
 * @return The iifname value
 */
struct addr_info get_daddr(struct rule* rule);

/**
 * Method that try to get the oifname value in the rule
 * @return The oifname value
 */
const char* get_oifname(struct rule* rule);

/**
 * Method that try to get the source address value in the rule
 * @return The iifname value
 */
const char *get_saddr(struct rule* rule);

/**
 * Method that return the protocol value in the rule
 * @return The protocol value 
 */
const char *get_protocol(struct rule* rule);

/**
 * Method that return the source port in the rule
 * @return The source value 
 */
const int get_sport(struct rule* rule);

/**
 * Method that return the destination port in the rule
 * @return The destination value 
 */
const int get_dport(struct rule* rule);

/**
 * Method that return the rule filtering action
 */
const char *get_filtering_action(struct rule *rule);

/**
 * Method that return a int corresponding to combinaison of the bitflag describe by the enum packet_direction
 */
int get_packet_direction(SEXP_t *pck_dir);

/**
 * Methode that return all the chain in the ruleset
 * This method allocate an oscap_list
 */
struct oscap_list* gather_chain(json_t * ruleset);

void filter_chain(struct ruleset* ruleset, struct oscap_list* chain_list, int filter);

/**
 * Convert a string to an instance of the packet_direction enum
 */
enum packet_direction convert_hook_to_packet_dir(const char *hook);

int networkfirewall_probe_offline_mode_supported(void);
int networkfirewall_probe_main(probe_ctx *ctx, void *arg);
bool _rule_chain_cmp(const char* rule, const char * chain);

#endif /* OPENSCAP_NETWORKFIREWALL_PROBE_H */