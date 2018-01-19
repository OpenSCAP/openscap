/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
#ifndef OSCAP_XCCDF_RESULT_SCORING_H
#define OSCAP_XCCDF_RESULT_SCORING_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "common/public/oscap.h"
#include "common/util.h"
#include "public/xccdf_benchmark.h"


/**
 * Calculate new XCCDF Score for given xccdf:TestResult
 * @memberof xccdf_result
 * @param test_result XCCDF TestResult
 * @param benchmark XCCDF Benchmark which is origin of given XCCDF TestResult
 * @param score_system Scoring Model URI as described in XCCDF standard.
 */
struct xccdf_score *xccdf_result_calculate_score(struct xccdf_result *test_result, struct xccdf_item *benchmark, const char *score_system);

#endif
