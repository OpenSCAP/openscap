#!/usr/bin/env bash

# Author:
#      Peter Vrabec, <pvrabec@redhat.com>
#	   Daniel Kopecek <dkopecek@redhat.com>


. $builddir/tests/test_common.sh
. $srcdir/test_mitre_common.sh

# Test Cases.
test_init

# does not work because of symplink `/etc/rc' -> `/etc/rc.d/rc' (oval:org.mitre.oval.test:tst:102)
#test_run "unix-def_file_test.xml" test_mitre unix-def_file_test.xml "true"

test_run "unix-def_password_test.xml" test_mitre unix-def_password_test.xml "true"

# these are outdated
#test_run "unix-def_process58_test.xml" test_mitre unix-def_process58_test.xml "true"
#test_run "unix-def_process_test.xml" test_mitre unix-def_process_test.xml "true"

# Fedora 16 and RHEL-7 - no runlevel
if [[ ( ${DISTRO#Fedora} != "$DISTRO" && $DISTRO_RELEASE -lt 16 ) || \
	( ${DISTRO#Red Hat} != "$DISTRO" && $DISTRO_RELEASE -lt 7 ) ]]; then
	test_run "unix-def_runlevel_test.xml" test_mitre unix-def_runlevel_test.xml "true"
fi

test_run "unix-def_uname_test.xml" test_mitre unix-def_uname_test.xml "true"

# needs only two interfaces - lo, eth0 - without ipv6 addresses
#test_run "unix-def_interface_test.xml" test_mitre unix-def_interface_test.xml "true"

# root needed
if [[ $(id -u) == 0 ]]; then
	test_run "unix-def_shadow_test.xml" test_mitre unix-def_shadow_test.xml "true"
fi

# install xinetd, telnet-server and tftp-server in order to test xinetd probe
if [[ -f "/etc/xinetd.conf" && -f "/etc/xinetd.d/tftp" && -f "/etc/xinetd.d/telnet" ]]; then
	test_run "unix-def_xinetd_test.xml" test_mitre unix-def_xinetd_test.xml "true"
fi

# Unsupported objects on Fedora
#test_run "unix-def_inetd_test.xml" test_mitre unix-def_inetd_test.xml "unknown"

test_exit cleanup_mitre
