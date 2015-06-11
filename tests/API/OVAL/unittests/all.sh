#!/bin/bash

. $srcdir/../../../test_common.sh

test_init test_api_oval_unittests.log
test_run "comment before root element" $srcdir/test_comment.sh
test_run "empty filename(pattern match)" $srcdir/test_empty_filename.sh
test_run "deprecated definition" $srcdir/test_deprecated_def.sh
test_run "applicability_check element" $srcdir/test_applicability_check.sh
test_run "count function"  $srcdir/test_count_function.sh
test_run "partial matches"  $srcdir/test_item_not_exist.sh
test_run "empty variable evaluation" $srcdir/test_oval_empty_variable_evaluation.sh
test_run "export of xsi:nil on pid entity of env.var.58_object" $srcdir/test_xsinil_envv58_pid.sh
test_run "Import content without proper namespaces" $srcdir/test_xmlns_missing.sh
test_run "int comparison - intmax_t" $srcdir/test_int_comparison.sh
test_run "evr_string comparison is superior to rpmvercmp" $srcdir/test_evr_string_comparison.sh
test_run "evr_string comparison regards missing epoch in content" $srcdir/test_evr_string_missing_epoch.sh
test_run "float comparison" $srcdir/test_float_comparison.sh
test_run "insensitive_equals on properties" $srcdir/test_envvar_insensitive_equals.sh
test_run "ipv6_address: 'superset of' operation" $srcdir/test_ipv6_super_set_of.sh
test_run "ipv6_address: 'subset of' operation" $srcdir/test_ipv6_subset_of.sh
test_run "ipv4_address: 'superset of' operation" $srcdir/test_ipv4_super_set_of.sh
test_run "ipv4_address: 'subset of' operation" $srcdir/test_ipv4_subset_of.sh
test_run "ipv4_address: comparison" $srcdir/test_ipv4_comparison.sh
test_run "textfilecontent: 'line' comparison" $srcdir/test_filecontent_line.sh
test_run "anyxml element" $srcdir/test_anyxml.sh
test_run "invalid regular expression" $srcdir/test_invalid_regex.sh
test_exit
