#!/usr/bin/env bash
#
# Copyright 2011 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# Authors:
#      Daniel Kopecek <dkopecek@redhat.com>
#
. ${srcdir}/../../test_common.sh

# Test Cases.
function oval_fts {
	ROOT="/tmp/ovalroot"

	echo "Generating tree"
	./gentree.sh "$ROOT"
	
	if [ $? -ne 0 ]; then
		echo "gentree.sh failed"
		return 2
	fi
	
	find $ROOT -type f | sort > tree1.log
	
	echo "OVAL_FTS walk"
	./oval_fts_list '((path :operation 5) "'$ROOT'")'	\
		        '((filename :operation 11) ".*")'	\
	        	''					\
		        '((behaviors :recurse "directories" :recurse_file_system "local" :max_depth "-1" :recurse_direction "down"))' | sort > tree2.log

	if [ $? -ne 0 ]; then
		echo "oval_fts_list failed"
		return 2
	fi

	if [ "$(openssl md5 tree1.log | sed -n 's|.*=.\(.*\)$|\1|p')" != "$(openssl md5 tree2.log | sed -n 's|.*=.\(.*\)$|\1|p')" ]; then
		return 1
	else
		return 0
	fi
}

# Testing.
test_init "fts.log"
test_run "OVAL_FTS" oval_fts
test_exit
