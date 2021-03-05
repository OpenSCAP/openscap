/**
 * @file   networkfirewall_probe.c
 * @brief  networkfirewall probe
 * @author "Gwendal Didot" <gwendal.didot@soprasteria.com>
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

int get_packet_direction(SEXP_t *pck_dir) {
    int result = 0;
    SEXP_t* INCOMING_SEXP = SEXP_string_newf("INCOMING");
    SEXP_t * OUTGOING_SEXP = SEXP_string_newf("OUTGOING");
    SEXP_t * FORWARDING_SEXP = SEXP_string_newf("FORWARDING");

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
    return result;
}

enum packet_direction convert_hook_to_packet_dir(const char *hook) {
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


struct oscap_list* gather_chain(json_t * ruleset) {
    struct oscap_list* chain_list = oscap_list_new();
    json_t* nftable_child;
    size_t index;
    
    json_array_foreach(ruleset, index, nftable_child) {
        json_t *chain = json_object_get(nftable_child, "chain");
        if (chain != NULL) {
            oscap_list_push(chain_list, chain);
        }
    }
    return chain_list;
}

struct json_t * get_ruleset() {
    struct oscap_string * ruleset = oscap_string_new();
    FILE* cmd_ptr;
    char ruleset_buf[RULESET_BUF];
    // Call to the nftable CLI to get the current ruleset on the system.
    // The option -j return the result as a JSON and the option -nn convert protocol value to their
    // numerical port value (http -> 80, https -> 443)
    cmd_ptr = popen("nft -j -nn list ruleset", "r");

    if (cmd_ptr == NULL) {
        dE("Erreur de récupération du ruleset");
        return 0;
    }

    while (fgets(ruleset_buf, sizeof(ruleset_buf), cmd_ptr) != NULL) {
        oscap_string_append_string(ruleset, ruleset_buf);
    }
    pclose(cmd_ptr);
    
    json_error_t error;
    struct json_t * ruleset_json = json_loads(oscap_string_get_cstr(ruleset), JSON_DECODE_ANY, &error);

    return ruleset_json;
}

void filter_chain(struct ruleset* ruleset, struct oscap_list* chain_list, int filter) {
    struct oscap_iterator *chain_list_it = oscap_iterator_new(chain_list);
    while (oscap_iterator_has_more(chain_list_it))
    {
        const struct json_object *chain = oscap_iterator_next(chain_list_it);
        struct json_t *hook = json_object_get(chain, "hook");
        struct json_t *chain_name = json_object_get(chain, "name");
        enum packet_direction chain_pck_dir = convert_hook_to_packet_dir(json_string_value(hook));
        if (chain_pck_dir == UNSUPPORTED) {
            // If the hook is not a supported packet_direction, the chain is skipped
            continue;
        }
        if (filter & chain_pck_dir) {
            oscap_stringlist_add_string(ruleset->chain_list, json_string_value(chain_name));
        }
    }
}

bool _rule_chain_cmp(const char* rule, const char * chain) {
    // The "forward" value is the longest value supported by the probe
    return strncmp(rule, chain, 7) == 0;
}

struct rule* rule_new() {
    struct rule* rule = malloc(sizeof(struct rule));
    return rule;
}

const char* get_iifname(struct rule* rule) {
    struct json_t * expr = json_object_get(rule->rule_json, "expr");
    json_t* expr_child;
    size_t index;
    const char* interface_name = NULL;
    const char* meta_value = NULL;

    json_array_foreach(expr, index, expr_child) {
        dD("%s", json_dumps(expr_child, 0));
        if (json_unpack(expr_child, "{s:{s:{s:s}, s:s}}", "match", "left", "meta", &meta_value, "right", &interface_name) == 0) {
            if (strncmp(meta_value, "iifname", 7) != 0) {
                continue;
            }
            if (strstr(interface_name, "*") != NULL) {
                continue;
            }
            if (interface_name[0] == '@') {
                continue;
            }
            return interface_name;
        }
    }
    return "";
}

struct ruleset *ruleset_new(int filter) {
    struct ruleset *ruleset = malloc(sizeof(struct ruleset));
    
    ruleset->rules = oscap_list_new();
    ruleset->chain_list = oscap_stringlist_new();

    struct json_t* ruleset_json = get_ruleset();
    ruleset->ruleset = ruleset_json;

    //Gather the chain in the ruleset based on the packet_direction entity
    json_t *nftables;
    nftables = json_object_get(ruleset_json, "nftables");
    struct oscap_list *chain_list = gather_chain(nftables);

    filter_chain(ruleset, chain_list, filter);
    
    // Gather the rules in the ruleset base on the filtered chain the rules belong to
    json_t* nftables_child;
    size_t index;
    
    json_array_foreach(nftables, index, nftables_child) {
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
        if (oscap_list_contains((struct oscap_list *)ruleset->chain_list, (void *)json_string_value(rule_chain_target), (oscap_cmp_func) _rule_chain_cmp)) {
            struct rule *rule = rule_new();
            rule->rule_json = rule_json;
            oscap_list_push(ruleset->rules, rule);
        }
    }
    return ruleset;
}

int networkfirewall_probe_offline_mode_supported(void) {
        return PROBE_OFFLINE_CHROOT;
}

int networkfirewall_probe_main(probe_ctx *ctx, void *arg) {
    SEXP_t *probe_in = probe_ctx_getobject(ctx);
    SEXP_t *packet_direction = probe_obj_getent(probe_in, "packet_direction", 1);
    SEXP_t *input_interface = probe_obj_getent(probe_in, "input_interface", 1);
    SEXP_t *input_interface_nil = probe_ent_getval(input_interface);
    bool isInInterfaceNil = input_interface_nil == NULL;
    struct ruleset *ruleset = ruleset_new(get_packet_direction(packet_direction));
    struct oscap_iterator *ruleset_rule_it = oscap_iterator_new(ruleset->rules);

    while (oscap_iterator_has_more(ruleset_rule_it)) {
        dD("New rule");
        struct rule *current_rule = oscap_iterator_next(ruleset_rule_it);
        if (!isInInterfaceNil) {
            const char *iifname = get_iifname(current_rule);
            if (iifname[0] != '\0') {
                dD("%s", iifname);
            }
            dD("End new rule");
        }
    }
    return 0;
}