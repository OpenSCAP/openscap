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
 * Authors:
 *	Simon Lukasik <slukasik@redhat.com>
 */

#include <stdio.h>
#include <assert.h>

#include <ds_rds_session.h>
#include <oscap_source.h>
#include <xccdf_benchmark.h>
#include <xccdf_policy.h>

#include "unit_helper.h"

int main(int argc, char *argv[])
{
	assert(argc == 2);
	struct oscap_source *rds_source = oscap_source_new_from_file(argv[1]);
	struct ds_rds_session *rds = ds_rds_session_new_from_source(rds_source);
	assert(rds != NULL);
	struct oscap_source *tr_source = ds_rds_session_select_report(rds, NULL);
	assert(tr_source != NULL);
	struct xccdf_result *tr = xccdf_result_import_source(tr_source);
	assert(tr != NULL);
	struct xccdf_rule_result *rr = xccdf_result_get_rule_result_by_id(tr, "x");
	assert(rr == NULL);
	rr = xccdf_result_get_rule_result_by_id(tr, "xccdf_org.ssgproject.content_rule_disable_prelink");
	assert(rr != NULL);
	struct oscap_text* waiver = oscap_text_new();
	oscap_text_set_text(waiver, "I believe this should have passed");
	assert(xccdf_rule_result_override(rr, XCCDF_RESULT_PASS, "yesterday", "intern contractor", waiver));
	tr_source = xccdf_result_export_source(tr, NULL);
	assert(tr_source != NULL);

	// modifies the dom
	assert(ds_rds_session_replace_report_with_source(rds, tr_source) == 0);
	// saves the modified dom
	assert(oscap_source_save_as(rds_source, "test_xccdf_overrides.xml.out") == 0);

	xccdf_result_free(tr);
	ds_rds_session_free(rds);
	oscap_source_free(rds_source);
	return 0;
}

