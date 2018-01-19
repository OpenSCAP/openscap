#!/usr/bin/env bash

# OpenSCAP test suite
# Tests OVAL details in OpenSCAP reports
# Author: Jan Černý <jcerny@redhat.com>

. $builddir/tests/test_common.sh

output_dir=`mktemp -d -t oval_details_XXXXXX`

# Test cases.

function test_oval_details {
  xccdffile=$srcdir/$1.xccdf.xml
  resultfile=$output_dir/$1.result.xml
  reportfile=$output_dir/$1.report.html
  $OSCAP xccdf eval --results $resultfile --oval-results --report $reportfile $xccdffile
  grep -i $2 $reportfile >/dev/null && grep -i $3 $reportfile >/dev/null
  rm "$1.oval.xml.result.xml"
}

# Testing.

test_init
if ! [ -f countries.xml ] ; then
  cp $srcdir/countries.src.xml ./countries.xml
fi
if ! [ -f foo.txt ] ; then
  cp $srcdir/foo.src.txt ./foo.txt
fi

test_run "test_oval_details_file_object" test_oval_details file "path.*UID.*permissions" "/dev/null"
test_run "test_oval_details_partition_object" test_oval_details partition "mount point.*device.*uuid" "/"
test_run "test_oval_details_rpminfo_object" test_oval_details rpminfo "name.*release.*version" "rpm"
if ! pidof systemd > /dev/null ; then
  test_run "test_oval_details_runlevel_object" test_oval_details runlevel "service name.*runlevel" "smartd"
fi
test_run "test_oval_details_sysctl_object" test_oval_details sysctl "name.*value" "net\.ipv4\.ip_forward"
test_run "test_oval_details_textfilecontent54_object" test_oval_details textfilecontent54 "path.*content" "foo\.txt.*Hello"
test_run "test_oval_details_variable_object" test_oval_details variable "var ref.*value" "oval:x:var:1.*42"
test_run "test_oval_details_xmlfilecontent_object" test_oval_details xmlfilecontent "filepath.*xpath.*value of" "countries\.xml.*London"

rm -rf $output_dir
rm -f ./foo.txt ./countries.xml
test_exit
