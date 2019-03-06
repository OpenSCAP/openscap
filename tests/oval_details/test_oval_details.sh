#!/usr/bin/env bash

# OpenSCAP test suite
# Tests OVAL details in OpenSCAP reports
# Author: Jan Černý <jcerny@redhat.com>

. $builddir/tests/test_common.sh

output_dir=`mktemp -d -t oval_details_XXXXXX`

# Test cases.

function test_oval_details_implicit {
  # Tests if OVAL Details are present in HTML report by default
  # without specifing --oval-results --results (new behavior)
  xccdffile=$srcdir/$1.xccdf.xml
  reportfile=$output_dir/$1.report.html
  resultfile="$1.result.xml"
  oval_resultfile="$1.oval.xml.result.xml"
  $OSCAP xccdf eval --report $reportfile $xccdffile
  [ -f $reportfile ]
  [ ! -f $resultfile ]
  [ ! -f $output_dir/$resultfile ]
  [ ! -f $oval_resultfile ]
  [ ! -f $output_dir/$oval_resultfile ]
  grep -i $2 $reportfile >/dev/null && grep -i $3 $reportfile >/dev/null
  rm $reportfile
}

function test_oval_details_explicit {
  # Tests if OVAL details are present in HTML report when options
  # --oval-results and --results are explicitely specified
  # (backwards-compatible behavior)
  xccdffile=$srcdir/$1.xccdf.xml
  resultfile=$output_dir/$1.result.xml
  reportfile=$output_dir/$1.report.html
  oval_resultfile="$1.oval.xml.result.xml"
  $OSCAP xccdf eval --results $resultfile --oval-results --report $reportfile $xccdffile
  grep -i $2 $reportfile >/dev/null && grep -i $3 $reportfile >/dev/null
  [ -f $reportfile ]
  [ -f $resultfile ]
  [ -f $oval_resultfile ]
  rm $reportfile
  rm $resultfile
  rm $oval_resultfile
}

# Testing.

test_init
if ! [ -f countries.xml ] ; then
  cp $srcdir/countries.src.xml ./countries.xml
fi
if ! [ -f foo.txt ] ; then
  cp $srcdir/foo.src.txt ./foo.txt
fi

test_run "test_oval_details_file_object_implicit" test_oval_details_implicit file "path.*UID.*permissions" "/dev/null"
test_run "test_oval_details_file_object_explicit" test_oval_details_explicit file "path.*UID.*permissions" "/dev/null"
test_run "test_oval_details_partition_object_implicit" test_oval_details_implicit partition "mount point.*device.*uuid" "/"
test_run "test_oval_details_partition_object_explicit" test_oval_details_explicit partition "mount point.*device.*uuid" "/"
test_run "test_oval_details_rpminfo_object_implicit" test_oval_details_implicit rpminfo "name.*release.*version" "rpm"
test_run "test_oval_details_rpminfo_object_explicit" test_oval_details_explicit rpminfo "name.*release.*version" "rpm"
if ! pidof systemd > /dev/null ; then
  test_run "test_oval_details_runlevel_object_implicit" test_oval_details_implicit runlevel "service name.*runlevel" "smartd"
  test_run "test_oval_details_runlevel_object_explicit" test_oval_details_explicit runlevel "service name.*runlevel" "smartd"
fi
test_run "test_oval_details_sysctl_object_implicit" test_oval_details_implicit sysctl "name.*value" "net\.ipv4\.ip_forward"
test_run "test_oval_details_sysctl_object_explicit" test_oval_details_explicit sysctl "name.*value" "net\.ipv4\.ip_forward"
test_run "test_oval_details_textfilecontent54_object_implicit" test_oval_details_implicit textfilecontent54 "path.*content" "foo\.txt.*Hello"
test_run "test_oval_details_textfilecontent54_object_explicit" test_oval_details_explicit textfilecontent54 "path.*content" "foo\.txt.*Hello"
test_run "test_oval_details_variable_object_implicit" test_oval_details_implicit variable "var ref.*value" "oval:x:var:1.*42"
test_run "test_oval_details_variable_object_explicit" test_oval_details_explicit variable "var ref.*value" "oval:x:var:1.*42"
test_run "test_oval_details_xmlfilecontent_object_implicit" test_oval_details_implicit xmlfilecontent "filepath.*xpath.*value of" "countries\.xml.*London"
test_run "test_oval_details_xmlfilecontent_object_explicit" test_oval_details_explicit xmlfilecontent "filepath.*xpath.*value of" "countries\.xml.*London"

rm -rf $output_dir
rm -f ./foo.txt ./countries.xml
test_exit
