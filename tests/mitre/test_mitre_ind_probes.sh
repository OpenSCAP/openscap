#!/usr/bin/env bash

# Author:
#      Peter Vrabec, <pvrabec@redhat.com>
#	   Daniel Kopecek <dkopecek@redhat.com>


. $builddir/tests/test_common.sh
. $srcdir/test_mitre_common.sh

# Test Cases.
test_init "test_mitre_ind_probes.log"

test_run "ind-def_unknown_test.xml" test_mitre ind-def_unknown_test.xml "unknown"
test_run "ind-def_variable_test.xml" test_mitre ind-def_variable_test.xml "true"
test_run "ind-def_environmentvariable_test.xml" test_mitre ind-def_environmentvariable_test.xml "true"
test_run "ind-def_environmentvariable58_test.xml" test_mitre ind-def_environmentvariable_test.xml "true"
test_run "ind-def_family_test.xml" test_mitre ind-def_family_test.xml "true"
test_run "ind-def_textfilecontent54_test.xml" test_mitre ind-def_textfilecontent54_test.xml "true"
test_run "ind-def_textfilecontent_test.xml" test_mitre ind-def_textfilecontent_test.xml "true"
test_run "ind-def_xmlfilecontent_test.xml" test_mitre ind-def_xmlfilecontent_test.xml "true"
test_run "ind-def_filehash58_test.xml" test_mitre ind-def_filehash58_test.xml "true"

# does not work because of symplink `/etc/rc' -> `/etc/rc.d/rc' (oval:org.mitre.oval.test:tst:102)
#test_run "ind-def_filehash_test.xml" test_mitre ind-def_filehash_test.xml "true"

# Unsupported objects on Fedora
#test_run "ind-def_ldap_test.xml" test_mitre ind-def_ldap_test.xml "unknown"
#test_run "ind-def_sql_test.xml" test_mitre ind-def_sql_test.xml "unknown"

test_exit cleanup_mitre
