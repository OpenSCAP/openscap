#!/usr/bin/env bash

. $builddir/tests/test_common.sh

function test_probes_behaviour_setup {
	rm -rf /tmp/scapVal && mkdir /tmp/scapVal

	mkdir /tmp/scapVal/File-Test-Level2
	mkdir /tmp/scapVal/File-Test-Level2/Level3-Folder010
	mkdir /tmp/scapVal/File-Test-Level2/Level3-Folder020
	mkdir /tmp/scapVal/File-Test-Level2/Level3-Folder030
	mkdir /tmp/scapVal/File-Test-Level2/Level3-Folder040
	mkdir /tmp/scapVal/File-Test-Level2/Level3-Folder050
	mkdir /tmp/scapVal/File-Test-Level2/Level3-Folder010/Level4-Folder011
	mkdir /tmp/scapVal/File-Test-Level2/Level3-Folder010/Level4-Folder011/Level5-Folder012

	echo "This is a Level3-File scap validation test file.">>/tmp/scapVal/File-Test-Level2/Level3-Folder010/Level3-File010.txt
	echo "This is a Level5-File 0121 test file for the scap validation program.">/tmp/scapVal/File-Test-Level2/Level3-Folder010/Level4-Folder011/Level5-Folder012/Level5-File0121.txt
	echo "This is a Level5-File 0122 test file for the scap validation program.">/tmp/scapVal/File-Test-Level2/Level3-Folder010/Level4-Folder011/Level5-Folder012/Level5-File0122.log

	rm -rf /tmp/scapVal-Sym && mkdir /tmp/scapVal-Sym
	mkdir /tmp/scapVal-Sym/File-Test-Level2-SymLinked
	mkdir /tmp/scapVal-Sym/File-Test-Level2-SymLinked/Level3-SymLinked-Folder010
	echo "This is a Level3-File test file for the scap validation program.">/tmp/scapVal-Sym/File-Test-Level2-SymLinked/Level3-SymLinked-Folder010/Level3-File.txt
	mkdir /tmp/scapVal-Sym/File-Test-Level2-SymLinked/Level3-SymLinked-Folder010/Level4-Folder
	echo "This is a Level4-File test file for the scap validation program.">/tmp/scapVal-Sym/File-Test-Level2-SymLinked/Level3-SymLinked-Folder010/Level4-Folder/Level4-File.txt

	ln -s /tmp/scapVal-Sym/File-Test-Level2-SymLinked/ /tmp/scapVal/File-Test-Level2/Level3-Folder040/this_is_a_symlink
	ln -s /opt /tmp/scapVal-Sym/this_is_a_symlink_to_opt
	echo "This is a Level2-File test file for the scap validation program.">/tmp/scapVal-Sym/File-Test-Level2-SymLinked/File-Test-Sym-Level2.txt
}

function test_probes_behaviour_cleanup {
	rm -rf /tmp/scapVal
	rm -rf /tmp/scapVal-Sym
}

function test_probes_file_behaviour {
	probecheck "file" || return 255

	local ret_val=0
	local DF="$srcdir/test_probes_file_behaviour.xml"
	result="$(mktemp results.XXXXXXX.xml)"

	test_probes_behaviour_setup

	$OSCAP oval eval --results $result $DF || ret_val=1
	$OSCAP oval validate $result || ret_val=1

	assert_exists 11 '//results//criterion' || ret_val=1
	assert_exists 11 '//results//criterion[@result="true"]' || ret_val=1

	test_probes_behaviour_cleanup

	return $ret_val
}

test_init

test_run "test_probes_file_behaviour" test_probes_file_behaviour

test_exit
