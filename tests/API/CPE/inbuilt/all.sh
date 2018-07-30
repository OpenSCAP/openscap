#!/bin/bash
#
# Copyright 2013 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# This script is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

set -e -o pipefail

. $builddir/tests/test_common.sh

function ensure_oscap_version(){
	local name=$FUNCNAME
	local stdout=$(mktemp -t ${name}.out.XXXXXX)
	local stderr=$(mktemp -t ${name}.err.XXXXXX)
	$OSCAP --version 2> $stderr > $stdout
	[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
	grep "^Default CPE files: .*/cpe$" $stdout
	grep " Inbuilt CPE names " $stdout
	grep "^Red Hat Enterprise Linux . - cpe:/o:redhat:enterprise_linux:.$" $stdout
	rm $stdout
}

function _get_inbuilt_cpe_dir(){
	$OSCAP --version | awk -F: '/^Default CPE files: / {$1=""; print}'
}

function _get_inbuilt_cpe_path(){
	echo "$(_get_inbuilt_cpe_dir)/openscap-cpe-dict.xml"
}

function validate_inbuilt_dict(){
	local name=$FUNCNAME
	local path=$(_get_inbuilt_cpe_path)
	local output=$(mktemp -t ${name}.out.XXXXXX)
	[ -f $path ]
	$OSCAP cpe validate $path 2>&1 > $output
	[ -f $output ]; [ ! -s $output ]
	$OSCAP cpe validate $path 2>&1 > $output
	[ -f $output ]; [ ! -s $output ]
	rm $output
}

function info_inbuilt_dict(){
	local name=$FUNCNAME
	local path=$(_get_inbuilt_cpe_path)
	local stderr=$(mktemp -t ${name}.err.XXXXXX)
	[ -f $path ]
	$OSCAP info $path 2> $stderr
	[ -f $stderr ]; [ ! -s $stderr ];
	rm $stderr
}

test_init test_api_cpe_inbuilt.log

test_run "ensure_oscap_version" ensure_oscap_version
test_run "validate_inbuilt_dict" validate_inbuilt_dict
test_run "info_inbuilt_dict" info_inbuilt_dict

test_exit
