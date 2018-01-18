#!/usr/bin/env bash

# Author:
#      Peter Vrabec, <pvrabec@redhat.com>
#	   Daniel Kopecek <dkopecek@redhat.com>


. $builddir/tests/test_common.sh
. $srcdir/test_mitre_common.sh

# Test Cases.
test_init

test_run "linux-def_partition_test.xml" test_mitre linux-def_partition_test.xml "true"
test_run "linux-def_rpminfo_test.xml" test_mitre linux-def_rpminfo_test.xml "true"
test_run "linux-def_rpmverify_test.xml" test_mitre linux-def_rpmverify_test.xml "true"
# Fedora 18 and RHEL-7 - no allow_console_login
if [[ ( ${DISTRO#Fedora} != "$DISTRO" && $DISTRO_RELEASE -lt 18 ) || \
	( ${DISTRO#Red Hat} != "$DISTRO" && $DISTRO_RELEASE -lt 7 ) ]]; then
	test_run "linux-def_selinuxboolean_test.xml" test_mitre linux-def_selinuxboolean_test.xml "true"
fi

if [ $SELINUX_ENABLED -eq 0 ]; then
	test_run "linux-def_selinuxsecuritycontext_test.xml" test_mitre linux-def_selinuxsecuritycontext_test.xml "true"
fi

test_run "linux-def_inetlisteningservers_test.xml" test_mitre linux-def_inetlisteningservers_test.xml "true"
# Unsupported objects on Fedora
#test_run "linux-def_slackwarepkginfo_test.xml" test_mitre linux-def_slackwarepkginfo_test.xml "unknown"

if [[ $DISTRO_NAME == "Debian" ]]; then
	test_run "linux-def_dpkginfo_test.xml" test_mitre linux-def_dpkginfo_test.xml "true"
fi

test_exit cleanup_mitre
