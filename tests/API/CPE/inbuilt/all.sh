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

. $srcdir/../../../test_common.sh

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

test_init test_api_cpe_inbuilt.log

test_run "ensure_oscap_version" ensure_oscap_version

test_exit
