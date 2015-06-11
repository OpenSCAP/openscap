#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "result file: $result"
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

echo "Collecting:"
$OSCAP oval collect --syschar $result $srcdir/$name.oval.xml 2> $stderr
[ ! -s $stderr ]

rm $stderr
[ -s $result ]

assert_exists 1 '/oval_system_characteristics'
assert_exists 1 '/oval_system_characteristics/generator'
assert_exists 1 '/oval_system_characteristics/system_info'
assert_exists 1 '/oval_system_characteristics/collected_objects'
assert_exists 1 '/oval_system_characteristics/collected_objects/object'
assert_exists 1 '/oval_system_characteristics/collected_objects/object[@flag="complete"]'
assert_exists 1 '/oval_system_characteristics/collected_objects/object[@version="1"]'
assert_exists 1 '/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:1"]'
assert_exists 1 '/oval_system_characteristics/collected_objects/object/reference'
assert_exists 1 '/oval_system_characteristics/collected_objects/object/reference/@item_ref'
assert_exists 1 '/oval_system_characteristics/system_data'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item'
assert_exists 2 '/oval_system_characteristics/system_data/unix-sys:process58_item/@*'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/@id'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item[@status="exists"]'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:command_line'
assert_exists 0 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:command_line/@*'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:exec_time'
assert_exists 0 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:exec_time/@*'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:pid'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:pid/@*'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:pid[@datatype="int"]'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:pid[text()="1"]'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:ppid'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:ppid/@*'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:ppid[@datatype="int"]'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:priority'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:priority/@*'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:priority[@datatype="int"]'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:scheduling_class'
assert_exists 0 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:scheduling_class/@*'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:start_time'
assert_exists 0 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:start_time/@*'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:tty'
assert_exists 0 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:tty/@*'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:user_id'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:user_id/@*'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:user_id[@datatype="int"]'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:user_id[text()="0"]'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:exec_shield'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:exec_shield/@*'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:exec_shield[@datatype="boolean"]'

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:loginuid'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:loginuid/@*'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:loginuid[@datatype="int"]'

if which getenforce; then
	# How do we query if selinux exists in a portable way?
	# We rather fail if someone has selinux but not selinux-devel.
	assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:selinux_domain_label'
	assert_exists 0 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:selinux_domain_label/@*'
fi

assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:session_id'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:session_id/@*'
assert_exists 1 '/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:session_id[@datatype="int"]'

rm $result
