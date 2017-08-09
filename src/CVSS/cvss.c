/*! \file cvss.c
 *  \brief Interface to Common Vulnerability Scoring System Version 2
 *
 *  See details at http://nvd.nist.gov/cvss.cfm
 *
 */

/*
 * Copyright 2008-2009 Red Hat Inc., Durham, North Carolina.
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
 *      Tomas Heinrich <theinric@redhat.com>
 *      Peter Vrabec   <pvrabec@redhat.com>
 *      Brandon Dixon  <Brandon.Dixon@g2-inc.com>
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "public/cvss_score.h"
#include "cvss_priv.h"
#include "common/elements.h"

#define CVSS_SUPPORTED "2.0"
#define NS_VULN_STR BAD_CAST "vuln"
#define NS_CVSS_STR BAD_CAST "cvss"
#define NS_VULN_URI BAD_CAST "http://scap.nist.gov/schema/vulnerability/0.4"
#define NS_CVSS_URI BAD_CAST "http://scap.nist.gov/schema/cvss-v2/0.2"

const char *cvss_model_supported(void) { return CVSS_SUPPORTED; }


static struct cvss_metrics **cvss_impact_metricsptr(struct cvss_impact* impact, enum cvss_category cat)
{
    assert(impact != NULL);

    switch (cat) {
        case CVSS_BASE:          return &impact->base_metrics;
        case CVSS_TEMPORAL:      return &impact->temporal_metrics;
        case CVSS_ENVIRONMENTAL: return &impact->environmental_metrics;
        default: assert(false); return false;
    }
}

struct cvss_impact *cvss_impact_new(void) { return calloc(1, sizeof(struct cvss_metrics)); }

struct cvss_keytab_entry {
    enum cvss_key key;     // cvss vector component
    const char *human_str; // human-readable name of the component
    const char *xml_str;   // component XML element name
};

static const struct cvss_keytab_entry CVSS_KEYTAB[] = {
    // Base metrics:
    { CVSS_KEY_access_vector,          "Access Vector",          "access-vector"          },
    { CVSS_KEY_access_complexity,      "Access Complexity",      "access-complexity"      },
    { CVSS_KEY_authentication,         "Authentication",         "authentication"         },
    { CVSS_KEY_confidentiality_impact, "Confidentiality Impact", "confidentiality-impact" },
    { CVSS_KEY_integrity_impact,       "Integrity Impact",       "integrity-impact"       },
    { CVSS_KEY_availability_impact,    "Availability Impact",    "availability-impact"    },
    // Temporal metrics:
    { CVSS_KEY_exploitability,    "Exploitability",    "exploitability"    },
    { CVSS_KEY_remediation_level, "Remediation Level", "remediation-level" },
    { CVSS_KEY_report_confidence, "Report Confidence", "report-confidence" },
    // Environmental metrics:
    { CVSS_KEY_collateral_damage_potential, "Collateral Damage Potential", "collateral-damage-potential" },
    { CVSS_KEY_target_distribution,         "Target Distribution",         "target-distribution"         },
    { CVSS_KEY_confidentiality_requirement, "Confidentiality Requirement", "confidentiality-requirement" },
    { CVSS_KEY_integrity_requirement,       "Integrity Requirement",       "integrity-requirement"       },
    { CVSS_KEY_availability_requirement,    "Availability Requirement",    "availability-requirement"    },
    // end of list
    { CVSS_KEY_NONE, "Invalid entry", NULL }
};

// lookup keytab entry by key or by xml element name
static const struct cvss_keytab_entry *cvss_keytab(enum cvss_key key, const char *elname)
{
    assert(key == CVSS_KEY_NONE || elname == NULL);
    const struct cvss_keytab_entry *e;
    for (e = CVSS_KEYTAB; e->key != CVSS_KEY_NONE; ++e)
        if (e->key == key || oscap_streq(e->xml_str, elname))
            break;
    return e;
}

struct cvss_valtab_entry {
    enum cvss_key key;
    unsigned value;
    const char *human_str;
    const char *vector_str;
    const char *xml_str;
    float weight;
};

static const struct cvss_valtab_entry CVSS_VALTAB[] = {

    // Base metrics:

    { CVSS_KEY_access_vector, CVSS_AV_NOT_SET,          "Not Set",          "AV:-", NULL,                 NAN },
    { CVSS_KEY_access_vector, CVSS_AV_LOCAL,            "Local",            "AV:L", "LOCAL",            0.395 },
    { CVSS_KEY_access_vector, CVSS_AV_ADJACENT_NETWORK, "Adjacent Network", "AV:A", "ADJACENT_NETWORK", 0.646 },
    { CVSS_KEY_access_vector, CVSS_AV_NETWORK,          "Network",          "AV:N", "NETWORK",          1.000 },

    { CVSS_KEY_access_complexity, CVSS_AC_NOT_SET, "Not Set", "AC:-", NULL,       NAN },
    { CVSS_KEY_access_complexity, CVSS_AC_HIGH,    "High",    "AC:H", "HIGH",   0.350 },
    { CVSS_KEY_access_complexity, CVSS_AC_MEDIUM,  "Medium",  "AC:M", "MEDIUM", 0.610 },
    { CVSS_KEY_access_complexity, CVSS_AC_LOW,     "Low",     "AC:L", "LOW",    0.710 },

    { CVSS_KEY_authentication, CVSS_AU_NOT_SET,  "Not Set",            "AU:-", NULL,                   NAN },
    { CVSS_KEY_authentication, CVSS_AU_MULTIPLE, "Multiple Instances", "AU:M", "MULTIPLE_INSTANCES", 0.450 },
    { CVSS_KEY_authentication, CVSS_AU_SINGLE,   "Single Instance",    "AU:S", "SINGLE_INSTANCE",    0.560 },
    { CVSS_KEY_authentication, CVSS_AU_NONE,     "None",               "AU:N", "NONE",               0.704 },

    { CVSS_KEY_confidentiality_impact, CVSS_IMP_NOT_SET,  "Not Set",  "C:-", NULL,         NAN },
    { CVSS_KEY_confidentiality_impact, CVSS_IMP_NONE,     "None",     "C:N", "NONE",     0.000 },
    { CVSS_KEY_confidentiality_impact, CVSS_IMP_PARTIAL,  "Partial",  "C:P", "PARTIAL",  0.275 },
    { CVSS_KEY_confidentiality_impact, CVSS_IMP_COMPLETE, "Complete", "C:C", "COMPLETE", 0.660 },

    { CVSS_KEY_integrity_impact, CVSS_IMP_NOT_SET,  "Not Set",  "I:-", NULL,         NAN },
    { CVSS_KEY_integrity_impact, CVSS_IMP_NONE,     "None",     "I:N", "NONE",     0.000 },
    { CVSS_KEY_integrity_impact, CVSS_IMP_PARTIAL,  "Partial",  "I:P", "PARTIAL",  0.275 },
    { CVSS_KEY_integrity_impact, CVSS_IMP_COMPLETE, "Complete", "I:C", "COMPLETE", 0.660 },

    { CVSS_KEY_availability_impact, CVSS_IMP_NOT_SET,  "Not Set",  "A:-", NULL,         NAN },
    { CVSS_KEY_availability_impact, CVSS_IMP_NONE,     "None",     "A:N", "NONE",     0.000 },
    { CVSS_KEY_availability_impact, CVSS_IMP_PARTIAL,  "Partial",  "A:P", "PARTIAL",  0.275 },
    { CVSS_KEY_availability_impact, CVSS_IMP_COMPLETE, "Complete", "A:C", "COMPLETE", 0.660 },

    // Temporal metrics:

    { CVSS_KEY_exploitability, CVSS_E_NOT_DEFINED,      "Not Defined",      "E:ND",  "NOT_DEFINED",      1.000 },
    { CVSS_KEY_exploitability, CVSS_E_UNPROVEN,         "Unproven",         "E:U",   "UNPROVEN",         0.850 },
    { CVSS_KEY_exploitability, CVSS_E_PROOF_OF_CONCEPT, "Proof-of-Concept", "E:POC", "PROOF_OF_CONCEPT", 0.900 },
    { CVSS_KEY_exploitability, CVSS_E_FUNCTIONAL,       "Functional",       "E:F",   "FUNCTIONAL",       0.950 },
    { CVSS_KEY_exploitability, CVSS_E_HIGH,             "High",             "E:H",   "HIGH",             1.000 },

    { CVSS_KEY_remediation_level, CVSS_RL_NOT_DEFINED,   "Not Defined",   "RL:ND", "NOT_DEFINED",   1.000 },
    { CVSS_KEY_remediation_level, CVSS_RL_OFFICIAL_FIX,  "Official Fix",  "RL:OF", "OFFICIAL_FIX",  0.870 },
    { CVSS_KEY_remediation_level, CVSS_RL_TEMPORARY_FIX, "Temporary Fix", "RL:TF", "TEMPORARY_FIX", 0.900 },
    { CVSS_KEY_remediation_level, CVSS_RL_WORKAROUND,    "Workaround",    "RL:W",  "WORKAROUND",    0.950 },
    { CVSS_KEY_remediation_level, CVSS_RL_UNAVAILABLE,   "Unavailable",   "RL:U",  "UNAVAILABLE",   1.000 },

    { CVSS_KEY_report_confidence, CVSS_RC_NOT_DEFINED,    "Not Defined",    "RC:ND", "NOT_DEFINED",    1.000 },
    { CVSS_KEY_report_confidence, CVSS_RC_UNCONFIRMED,    "Unconfirmed",    "RC:UC", "UNCONFIRMED",    0.900 },
    { CVSS_KEY_report_confidence, CVSS_RC_UNCORROBORATED, "Uncorroborated", "RC:UR", "UNCORROBORATED", 0.950 },
    { CVSS_KEY_report_confidence, CVSS_RC_CONFIRMED,      "Confirmed",      "RC:C",  "CONFIRMED",      1.000 },

    // Environmental metrics:

    { CVSS_KEY_collateral_damage_potential, CVSS_CDP_NOT_DEFINED, "Not Defined", "CDP:ND", "NOT_DEFINED", 0.000 },
    { CVSS_KEY_collateral_damage_potential, CVSS_CDP_NONE,        "None",        "CDP:N",  "NONE",        0.000 },
    { CVSS_KEY_collateral_damage_potential, CVSS_CDP_LOW,         "Low",         "CDP:L",  "LOW",         0.100 },
    { CVSS_KEY_collateral_damage_potential, CVSS_CDP_LOW_MEDIUM,  "Low-Medium",  "CDP:LM", "LOW_MEDIUM",  0.300 },
    { CVSS_KEY_collateral_damage_potential, CVSS_CDP_MEDIUM_HIGH, "Medium-High", "CDP:MH", "MEDIUM_HIGH", 0.400 },
    { CVSS_KEY_collateral_damage_potential, CVSS_CDP_HIGH,        "High",        "CDP:H",  "HIGH",        0.500 },

    { CVSS_KEY_target_distribution, CVSS_TD_NOT_DEFINED, "Not Defined", "TD:ND", "NOT_DEFINED", 1.000 },
    { CVSS_KEY_target_distribution, CVSS_TD_NONE,        "None",        "TD:N",  "NONE",        0.000 },
    { CVSS_KEY_target_distribution, CVSS_TD_LOW,         "Low",         "TD:L",  "LOW",         0.250 },
    { CVSS_KEY_target_distribution, CVSS_TD_MEDIUM,      "Medium",      "TD:M",  "MEDIUM",      0.750 },
    { CVSS_KEY_target_distribution, CVSS_TD_HIGH,        "High",        "TD:H",  "HIGH",        1.000 },

    { CVSS_KEY_confidentiality_requirement, CVSS_REQ_NOT_DEFINED, "Not Defined", "CR:ND", "NOT_DEFINED", 1.000 },
    { CVSS_KEY_confidentiality_requirement, CVSS_REQ_LOW,         "Low",         "CR:L",  "LOW",         0.500 },
    { CVSS_KEY_confidentiality_requirement, CVSS_REQ_MEDIUM,      "Medium",      "CR:M",  "MEDIUM",      1.000 },
    { CVSS_KEY_confidentiality_requirement, CVSS_REQ_HIGH,        "High",        "CR:H",  "HIGH",        1.510 },

    { CVSS_KEY_integrity_requirement, CVSS_REQ_NOT_DEFINED, "Not Defined", "IR:ND", "NOT_DEFINED", 1.000 },
    { CVSS_KEY_integrity_requirement, CVSS_REQ_LOW,         "Low",         "IR:L",  "LOW",         0.500 },
    { CVSS_KEY_integrity_requirement, CVSS_REQ_MEDIUM,      "Medium",      "IR:M",  "MEDIUM",      1.000 },
    { CVSS_KEY_integrity_requirement, CVSS_REQ_HIGH,        "High",        "IR:H",  "HIGH",        1.510 },

    { CVSS_KEY_availability_requirement, CVSS_REQ_NOT_DEFINED, "Not Defined", "AR:ND", "NOT_DEFINED", 1.000 },
    { CVSS_KEY_availability_requirement, CVSS_REQ_LOW,         "Low",         "AR:L",  "LOW",         0.500 },
    { CVSS_KEY_availability_requirement, CVSS_REQ_MEDIUM,      "Medium",      "AR:M",  "MEDIUM",      1.000 },
    { CVSS_KEY_availability_requirement, CVSS_REQ_HIGH,        "High",        "AR:H",  "HIGH",        1.510 },

    // End-of-list
    { CVSS_KEY_NONE, 0, "Invalid value", "!", NULL, NAN }
};

// valtab lookup: either by key&value or by vector string or by key+XMLvalue (pass exactly one of key+val or vec_str or key+xmlval)
static const struct cvss_valtab_entry *cvss_valtab(enum cvss_key key, unsigned val, const char *vec_str, const char *xmlval)
{
    const struct cvss_valtab_entry *entry;
    for (entry = CVSS_VALTAB; entry->key != CVSS_KEY_NONE; ++entry)
        if ((key == entry->key && (xmlval ? oscap_streq(entry->xml_str, xmlval) : val == entry->value)) || oscap_streq(vec_str, entry->vector_str))
            break;
    return entry;
}

struct cvss_impact *cvss_impact_new_from_vector(const char *cvss_vector)
{
    struct cvss_impact *impact = cvss_impact_new();
    char *vector_dup = oscap_strdup(cvss_vector);
    char *vector_start = vector_dup;
    char **components = NULL;
    size_t i;
    const struct cvss_valtab_entry *entry;
    struct cvss_metrics **mptr;

    if (cvss_vector == NULL) goto syntax_error;

    // vector in parenthesis
    if (vector_dup[0] == '(') {
        char *vector_end = vector_dup + strlen(vector_dup) - 1;
        if (*vector_end != ')') goto syntax_error;
        ++vector_start;
        *vector_end = '\0';
    }

    oscap_strtoupper(vector_start);
    // split vector to components
    components = oscap_split(vector_start, "/");
    for (i = 0; components[i] != NULL; ++i) {
        entry = cvss_valtab(0, 0, components[i], NULL);
        if (entry->key == CVSS_KEY_NONE) goto syntax_error;
        mptr = cvss_impact_metricsptr(impact, CVSS_CATEGORY(entry->key));
        if (*mptr == NULL) *mptr = cvss_metrics_new(CVSS_CATEGORY(entry->key));
        (*mptr)->metrics.ANY[CVSS_KEY_IDX(entry->key)] = entry->value;
    }

cleanup:
    free(vector_dup);
    free(components);
    return impact;

syntax_error:
    cvss_impact_free(impact);
    impact = NULL;
    goto cleanup;
}

struct cvss_impact *cvss_impact_new_from_xml(xmlTextReaderPtr reader)
{
    assert(reader != NULL);

    int depth = oscap_element_depth(reader);
    struct cvss_impact *ret = cvss_impact_new();
    xmlTextReaderRead(reader); // move to next element

    while (oscap_to_start_element(reader, depth + 1)) {
        struct cvss_metrics *mtx = cvss_metrics_new_from_xml(reader);
        if (mtx) cvss_impact_set_metrics(ret, mtx);
        // else issue a warning?
        xmlTextReaderRead(reader);
    }

    return ret;
}

struct cvss_impact *cvss_impact_clone(const struct cvss_impact *imp)
{
    if (imp == NULL) return NULL;
    struct cvss_impact *ret = cvss_impact_new();
    ret->base_metrics = cvss_metrics_clone(imp->base_metrics);
    ret->temporal_metrics = cvss_metrics_clone(imp->temporal_metrics);
    ret->environmental_metrics = cvss_metrics_clone(imp->environmental_metrics);
    return ret;
}

bool cvss_impact_export(const struct cvss_impact *imp, xmlTextWriterPtr writer)
{
    assert(writer != NULL);
    assert(imp != NULL);
    bool ret = true;

    xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "cvss", NS_VULN_URI);
    if (imp->base_metrics)          ret = ret && cvss_metrics_export(imp->base_metrics, writer);
    if (imp->temporal_metrics)      ret = ret && cvss_metrics_export(imp->temporal_metrics, writer);
    if (imp->environmental_metrics) ret = ret && cvss_metrics_export(imp->environmental_metrics, writer);
    xmlTextWriterEndElement(writer);

    return ret;
}

static size_t cvss_metrics_component_num(const struct cvss_metrics* metrics)
{
    switch (metrics->category) {
        case CVSS_BASE:          return CVSS_KEY_BASE_NUM;
        case CVSS_TEMPORAL:      return CVSS_KEY_TEMPORAL_NUM;
        case CVSS_ENVIRONMENTAL: return CVSS_KEY_ENVIRONMENTAL_NUM;
        default: assert(false); return 0;
    }
}

bool cvss_metrics_is_valid(const struct cvss_metrics* metrics)
{
    if (metrics == NULL) return false;

    if (metrics->category == CVSS_BASE)
        for (size_t i = 0; i < CVSS_KEY_BASE_NUM; ++i)
            if (metrics->metrics.BASE[i] == 0)
                return false;

    return true;
}

static char* cvss_metrics_to_vector(const struct cvss_metrics* metrics, char *out)
{
    if (metrics == NULL) return out;

    for (size_t i = 0; i < cvss_metrics_component_num(metrics); ++i)
        out += sprintf(out, "%s/", cvss_valtab(metrics->category | i, metrics->metrics.ANY[i], NULL, NULL)->vector_str);

    return out;
}

char *cvss_impact_to_vector(const struct cvss_impact* impact)
{
    assert(impact != NULL);

    // eight characters per component, 14 components
    char *result = calloc(1, sizeof(char) * 8 * 14);
    char *out = result;

    out = cvss_metrics_to_vector(impact->base_metrics, out);
    out = cvss_metrics_to_vector(impact->temporal_metrics, out);
    out = cvss_metrics_to_vector(impact->environmental_metrics, out);

    if (out > result) *--out = '\0';

    return result;
}

static unsigned cvss_impact_val(const struct cvss_impact *impact, enum cvss_key key)
{
    assert(impact != NULL);
    assert(key != CVSS_KEY_NONE);

    const struct cvss_metrics *metric = *cvss_impact_metricsptr((/*const*/ struct cvss_impact *) impact, CVSS_CATEGORY(key));
    if (metric == NULL) return -1;
    return metric->metrics.ANY[CVSS_KEY_IDX(key)];
}

static inline const struct cvss_valtab_entry *cvss_impact_entry(const struct cvss_impact *impact, enum cvss_key key)
{
    return cvss_valtab(key, cvss_impact_val(impact, key), NULL, NULL);
}

void cvss_impact_free(struct cvss_impact* impact)
{
    if (impact) {
        cvss_metrics_free(cvss_impact_get_base_metrics(impact));
        cvss_metrics_free(cvss_impact_get_temporal_metrics(impact));
        cvss_metrics_free(cvss_impact_get_environmental_metrics(impact));
        free(impact);
    }
}

//struct cvss_impact *cvss_impact_new_from_xml(const char *filename)

bool cvss_impact_set_metrics(struct cvss_impact* impact, struct cvss_metrics *metrics)
{
    assert(impact != NULL);
    assert(metrics != NULL);

    struct cvss_metrics **mptr = cvss_impact_metricsptr(impact, metrics->category);
    cvss_metrics_free(*mptr);
    *mptr = metrics;
    return true;
}

#define CVSS_W(key) (cvss_impact_entry(impact, CVSS_KEY_##key)->weight)

// round x to multiples of 0.1
float cvss_round(float x) { return (int) round(x * 10.0 + 0.00001) / 10.0; }

typedef float(*cvss_score_func)(const struct cvss_impact*);

float cvss_impact_base_exploitability_subscore(const struct cvss_impact* impact)
{
    assert(impact);
    return 20 * CVSS_W(access_vector) * CVSS_W(access_complexity) * CVSS_W(authentication);
}

float cvss_impact_base_impact_subscore(const struct cvss_impact* impact)
{
    assert(impact);
    return 10.41 * (1.0 - (1.0 - CVSS_W(confidentiality_impact)) * (1.0 - CVSS_W(integrity_impact)) * (1.0 - CVSS_W(availability_impact)));
}

static inline float cvss_impact_base_score_impl(const struct cvss_impact* impact, cvss_score_func impact_score_calculator)
{
    assert(impact);
    if (!cvss_metrics_is_valid(impact->base_metrics)) return NAN;
    float imp_s = impact_score_calculator(impact);
    float exp_s = cvss_impact_base_exploitability_subscore(impact);
    float f_imp = (imp_s == 0.0 ? 0.0 : 1.176);
    return cvss_round((0.6 * imp_s + 0.4 * exp_s - 1.5) * f_imp);
}

float cvss_impact_base_score(const struct cvss_impact* impact)
{
    assert(impact);
    return cvss_impact_base_score_impl(impact, cvss_impact_base_impact_subscore);
}

float cvss_impact_temporal_multiplier(const struct cvss_impact* impact)
{
    assert(impact);
    if (!cvss_metrics_is_valid(impact->temporal_metrics)) return NAN;
    return CVSS_W(exploitability) * CVSS_W(remediation_level) * CVSS_W(report_confidence);
}

float cvss_impact_temporal_score(const struct cvss_impact* impact)
{
    assert(impact);
    if (!cvss_metrics_is_valid(impact->temporal_metrics)) return NAN;
    return cvss_round(cvss_impact_base_score(impact) * cvss_impact_temporal_multiplier(impact));
}

float cvss_impact_base_adjusted_impact_subscore(const struct cvss_impact* impact)
{
    assert(impact);
    if (!cvss_metrics_is_valid(impact->environmental_metrics) || !cvss_metrics_is_valid(impact->base_metrics))
        return NAN;

    float c = CVSS_W(confidentiality_impact) * CVSS_W(confidentiality_requirement);
    float i = CVSS_W(integrity_impact)       * CVSS_W(integrity_requirement);
    float a = CVSS_W(availability_impact)    * CVSS_W(availability_requirement);
    float imp = 10.41 * (1.0 - (1.0 - c) * (1.0 - i) * (1.0 - a));
    return imp <= 10.0 ? imp : 10.0;
}

float cvss_impact_adjusted_base_score(const struct cvss_impact* impact)
{
    assert(impact);
    return cvss_impact_base_score_impl(impact, cvss_impact_base_adjusted_impact_subscore);
}

float cvss_impact_adjusted_temporal_score(const struct cvss_impact* impact)
{
    assert(impact);
    return cvss_round(cvss_impact_adjusted_base_score(impact) * cvss_impact_temporal_multiplier(impact));
}

float cvss_impact_environmental_score(const struct cvss_impact* impact)
{
    assert(impact);
    if (!cvss_metrics_is_valid(impact->environmental_metrics)) return NAN;
    float temp_s = cvss_impact_adjusted_temporal_score(impact);
    if (isnan(temp_s)) return NAN;
    return cvss_round((temp_s + (10.0 - temp_s) * CVSS_W(collateral_damage_potential)) * CVSS_W(target_distribution));
}

static void cvss_metrics_describe(const struct cvss_metrics *metrics, FILE *f)
{
    if (metrics == NULL) return;

    for (size_t i = 0; i < cvss_metrics_component_num(metrics); ++i) {
        const struct cvss_keytab_entry *key = cvss_keytab(metrics->category | i, NULL);
        const struct cvss_valtab_entry *val = cvss_valtab(metrics->category | i, metrics->metrics.ANY[i], NULL, NULL);
        fprintf(f, "%-30s %-20s [%-6s]\n", key->human_str, val->human_str, val->vector_str);
    }
}

void cvss_impact_describe(const struct cvss_impact *impact, FILE *f)
{
    assert(f != NULL);
    if (impact == NULL) return;

    char *vec = cvss_impact_to_vector(impact);
    if (vec) {
        fprintf(f, "CVSS vector: %s\n\n", vec);
        free(vec);
    }

    if (impact->base_metrics) {
        fprintf(f, "------------------------ Base Metrics ----------------------\n");
        cvss_metrics_describe(impact->base_metrics, f);
        fprintf(f, "Exploitability subscore: %4.1f\n", cvss_impact_base_exploitability_subscore(impact));
        fprintf(f, "Impact subscore: %4.1f\n", cvss_impact_base_impact_subscore(impact));
        fprintf(f, "Base score: %4.1f\n\n", cvss_impact_base_score(impact));
    }

    if (impact->temporal_metrics) {
        fprintf(f, "---------------------- Temporal Metrics --------------------\n");
        cvss_metrics_describe(impact->temporal_metrics, f);
        fprintf(f, "Temporal multiplier: %4.1f\n", cvss_impact_temporal_multiplier(impact));
        fprintf(f, "Temporal score: %4.1f\n\n", cvss_impact_temporal_score(impact));
    }

    if (impact->environmental_metrics) {
        fprintf(f, "------------------- Environmental Metrics ------------------\n");
        cvss_metrics_describe(impact->environmental_metrics, f);
        fprintf(f, "Adjusted impact base subscore: %4.1f\n", cvss_impact_base_adjusted_impact_subscore(impact));
        fprintf(f, "Adjusted base score: %4.1f\n", cvss_impact_adjusted_base_score(impact));
        fprintf(f, "Adjusted temporal score: %4.1f\n", cvss_impact_adjusted_temporal_score(impact));
        fprintf(f, "Environmental score: %4.1f\n\n", cvss_impact_environmental_score(impact));
    }
}

OSCAP_GETTER(struct cvss_metrics*, cvss_impact, base_metrics)
OSCAP_GETTER(struct cvss_metrics*, cvss_impact, temporal_metrics)
OSCAP_GETTER(struct cvss_metrics*, cvss_impact, environmental_metrics)

struct cvss_metrics *cvss_metrics_new(enum cvss_category category)
{
    assert(category != CVSS_NONE);

    struct cvss_metrics *metrics = calloc(1, sizeof(struct cvss_metrics));
    metrics->category = category;
    metrics->score = NAN;
    return metrics;
}

struct cvss_metrics *cvss_metrics_new_from_xml(xmlTextReaderPtr reader)
{
    assert(reader != NULL);
    int depth = oscap_element_depth(reader);
    enum cvss_category cat = CVSS_NONE;
    const char *elname = (const char *) xmlTextReaderConstLocalName(reader);

    if (oscap_streq(elname, "base_metrics"))          cat = CVSS_BASE;
    if (oscap_streq(elname, "temporal_metrics"))      cat = CVSS_TEMPORAL;
    if (oscap_streq(elname, "environmental_metrics")) cat = CVSS_ENVIRONMENTAL;

    if (cat == CVSS_NONE) return NULL;

    struct cvss_metrics *ret = cvss_metrics_new(cat);

    ret->upgraded_from_version = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "upgraded-from-version");
    xmlTextReaderRead(reader);

    while (oscap_to_start_element(reader, depth + 1)) {
        elname = (const char *) xmlTextReaderConstLocalName(reader);
        if (oscap_streq(elname, "score")) ret->score = atof(oscap_element_string_get(reader));
        else if (oscap_streq(elname, "source")) cvss_metrics_set_source(ret, oscap_element_string_get(reader));
        else if (oscap_streq(elname, "generated-on-datetime")) cvss_metrics_set_generated_on_datetime(ret, oscap_element_string_get(reader));
        else {
            const struct cvss_valtab_entry *val = cvss_valtab(cvss_keytab(0, elname)->key, 0, NULL, oscap_element_string_get(reader));
            if (CVSS_CATEGORY(val->key) == cat) ret->metrics.ANY[CVSS_KEY_IDX(val->key)] = val->value;
        }
        xmlTextReaderRead(reader);
    }

    return ret;
}

struct cvss_metrics *cvss_metrics_clone(const struct cvss_metrics* met)
{
    if (met == NULL) return NULL;
    struct cvss_metrics *ret = cvss_metrics_new(met->category);
    if (ret == NULL) return NULL;
    ret->score = met->score;
    ret->source = oscap_strdup(met->source);
    ret->upgraded_from_version = oscap_strdup(met->upgraded_from_version);
    ret->generated_on_datetime = oscap_strdup(met->generated_on_datetime);
    for (size_t i = 0; i < cvss_metrics_component_num(met); ++i)
        ret->metrics.ANY[i] = met->metrics.ANY[i];
    return ret;
}

bool cvss_metrics_export(const struct cvss_metrics *m, xmlTextWriterPtr writer)
{
    assert(writer != NULL);
    if (!cvss_metrics_is_valid(m)) return false;

    const char *elname = NULL;
    switch (m->category) {
        case CVSS_BASE:          elname = "base_metrics";          break;
        case CVSS_TEMPORAL:      elname = "temporal_metrics";      break;
        case CVSS_ENVIRONMENTAL: elname = "environmental_metrics"; break;
        default: assert(false); return false;
    }

    xmlTextWriterStartElementNS(writer, NULL, BAD_CAST elname, NS_CVSS_URI);
    if (m->upgraded_from_version) xmlTextWriterWriteAttribute(writer, BAD_CAST "upgraded-from-version", BAD_CAST m->upgraded_from_version);

    if (!isnan(m->score)) {
        char *score_str = oscap_sprintf("%.1f", m->score);
        xmlTextWriterWriteElementNS(writer, NULL, BAD_CAST "score", NULL, BAD_CAST score_str);
        free(score_str);
    }

    for (size_t i = 0; i < cvss_metrics_component_num(m); ++i) {
        const struct cvss_valtab_entry *val = cvss_valtab(m->category | i, m->metrics.ANY[i], NULL, NULL);
        const struct cvss_keytab_entry *key = cvss_keytab(m->category | i, NULL);
        if (key->xml_str != NULL && val->xml_str != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, BAD_CAST key->xml_str, NULL, BAD_CAST val->xml_str);
    }

    if (m->source) xmlTextWriterWriteElementNS(writer, NULL, BAD_CAST "source", NULL, BAD_CAST m->source);
    if (m->generated_on_datetime) xmlTextWriterWriteElementNS(writer, NULL, BAD_CAST "generated-on-datetime", NULL, BAD_CAST m->generated_on_datetime);


    xmlTextWriterEndElement(writer);

    return true;
}

void cvss_metrics_free(struct cvss_metrics* metrics)
{
    if (metrics) {
        free(metrics->source);
        free(metrics->upgraded_from_version);
        free(metrics->generated_on_datetime);
        free(metrics);
    }
}

#define CVSS_METRIC_ACCESSORS_GEN(cat, key, type) \
    enum cvss_##type cvss_metrics_get_##key(const struct cvss_metrics* metrics) \
    { if (metrics->category != CVSS_##cat) return 0; return metrics->metrics.cat[CVSS_KEY_IDX(CVSS_KEY_##key)]; } \
    bool cvss_metrics_set_##key(struct cvss_metrics* metrics, enum cvss_##type newval) \
    { if (metrics->category != CVSS_##cat) return false; metrics->metrics.cat[CVSS_KEY_IDX(CVSS_KEY_##key)] = newval; return true; }
#define CVSS_METRIC_ACCESSORS_CIA(cat, key) CVSS_METRIC_ACCESSORS_GEN(cat, confidentiality_##key, cia_##key) \
    CVSS_METRIC_ACCESSORS_GEN(cat, integrity_##key, cia_##key) CVSS_METRIC_ACCESSORS_GEN(cat, availability_##key, cia_##key)
#define CVSS_METRIC_ACCESSORS(cat, key) CVSS_METRIC_ACCESSORS_GEN(cat, key, key)

CVSS_METRIC_ACCESSORS(BASE, access_vector)
CVSS_METRIC_ACCESSORS(BASE, access_complexity)
CVSS_METRIC_ACCESSORS(BASE, authentication)
CVSS_METRIC_ACCESSORS_CIA(BASE, impact)
CVSS_METRIC_ACCESSORS(TEMPORAL, exploitability)
CVSS_METRIC_ACCESSORS(TEMPORAL, remediation_level)
CVSS_METRIC_ACCESSORS(TEMPORAL, report_confidence)
CVSS_METRIC_ACCESSORS(ENVIRONMENTAL, collateral_damage_potential)
CVSS_METRIC_ACCESSORS(ENVIRONMENTAL, target_distribution)
CVSS_METRIC_ACCESSORS_CIA(ENVIRONMENTAL, requirement)

OSCAP_ACCESSOR_STRING(cvss_metrics, source)
OSCAP_ACCESSOR_STRING(cvss_metrics, upgraded_from_version)
OSCAP_ACCESSOR_STRING(cvss_metrics, generated_on_datetime)
OSCAP_ACCESSOR_SIMPLE(float, cvss_metrics, score)
OSCAP_GETTER(enum cvss_category, cvss_metrics, category)



