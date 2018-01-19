/**
 * @file cvss_priv.h
 * \brief Interface to Common Vulnerability Scoring System Version 2
 *
 *  See details at http://nvd.nist.gov/cvss.cfm
 *
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      Maros Barabas <mbarabas@redhat.com>
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifndef CVSS_PRIV_H_
#define CVSS_PRIV_H_

#include <stdlib.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "public/cvss_score.h"
#include "common/util.h"


#define CVSSMAX(a, b) ((a) > (b) ? (a) : (b))

struct cvss_impact;
struct cvss_metrics;

enum cvss_key {
    CVSS_KEY_NONE = CVSS_NONE,

    CVSS_KEY_access_vector = CVSS_BASE,
    CVSS_KEY_access_complexity,
    CVSS_KEY_authentication,
    CVSS_KEY_confidentiality_impact,
    CVSS_KEY_integrity_impact,
    CVSS_KEY_availability_impact,
    CVSS_KEY_BASE_END_,
    CVSS_KEY_BASE_NUM = CVSS_KEY_BASE_END_ - CVSS_BASE,

    CVSS_KEY_exploitability = CVSS_TEMPORAL,
    CVSS_KEY_remediation_level,
    CVSS_KEY_report_confidence,
    CVSS_KEY_TEMPORAL_END_,
    CVSS_KEY_TEMPORAL_NUM = CVSS_KEY_TEMPORAL_END_ - CVSS_TEMPORAL,

    CVSS_KEY_collateral_damage_potential = CVSS_ENVIRONMENTAL,
    CVSS_KEY_target_distribution,
    CVSS_KEY_confidentiality_requirement,
    CVSS_KEY_integrity_requirement,
    CVSS_KEY_availability_requirement,
    CVSS_KEY_ENVIRONMENTAL_END_,
    CVSS_KEY_ENVIRONMENTAL_NUM = CVSS_KEY_ENVIRONMENTAL_END_ - CVSS_ENVIRONMENTAL,
};

// extract category from key
#define CVSS_CATEGORY(key) ((key) & ~0xff)
// extract key index within the category
#define CVSS_KEY_IDX(key) ((key) & 0xff)

struct cvss_impact {
    struct cvss_metrics *base_metrics;
    struct cvss_metrics *temporal_metrics;
    struct cvss_metrics *environmental_metrics;
};

struct cvss_metrics {
    enum cvss_category category;
    float score;
    char *source;
    char *upgraded_from_version;
    char *generated_on_datetime;
    union {
        unsigned BASE[CVSS_KEY_BASE_NUM];
        unsigned TEMPORAL[CVSS_KEY_TEMPORAL_NUM];
        unsigned ENVIRONMENTAL[CVSS_KEY_ENVIRONMENTAL_NUM];
        unsigned ANY[CVSSMAX(CVSSMAX(CVSS_KEY_BASE_NUM, CVSS_KEY_TEMPORAL_NUM), CVSS_KEY_ENVIRONMENTAL_NUM)];
    } metrics;
};

struct cvss_impact *cvss_impact_new_from_xml(xmlTextReaderPtr reader);
bool cvss_impact_export(const struct cvss_impact *imp, xmlTextWriterPtr writer);
struct cvss_metrics *cvss_metrics_new_from_xml(xmlTextReaderPtr reader);
bool cvss_metrics_export(const struct cvss_metrics *m, xmlTextWriterPtr writer);


#endif
