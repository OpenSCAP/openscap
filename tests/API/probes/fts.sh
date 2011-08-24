#!/usr/bin/env bash
#
# Copyright 2011 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# Authors:
#      Daniel Kopecek <dkopecek@redhat.com>
#
. ${srcdir}/../../test_common.sh

function gen_tree {
	echo "Generating tree for traversal" >&2

	mkdir -p $ROOT/{d1/{d11/d111,d12},d2/d21}
	touch $ROOT/{d1/{d11/{d111/f1111,f111,f112,f113},d12/f121,f11},d2/{d21/f211,f21}}
}

function oval_fts {
	shift
	./oval_fts_list "$@" | sort | tee oval_fts_list.out | \
		sed "s|${ROOT}/||" | tr '\n' ',' > oval_fts_list.out2
	if [ $? -ne 0 ]; then
		echo "oval_fts_list failed"
		return 2
	fi

	shift 4
	echo -e "expected result:\n$1\noval_fts_list.out2:"
	cat oval_fts_list.out2
	echo
	if [ "$(cat oval_fts_list.out2 | openssl md5)" == \
		"$(echo -n $1 | openssl md5)" ] ; then
		return 0
	else
		return 1
	fi
}

# init
test_init "fts.log"
ROOT="$(mktemp -d -t openscap-fts.XXXXXX)"
gen_tree "$ROOT"

while read args; do
	[ -z "$args" ] && continue
	# todo: print test name
	test_run "OVAL_FTS" oval_fts $args
done <<EOF
"test 1" \
'' '' \
'((filepath :operation 5) "'$ROOT'/d1/d12/f121")' \
'((behaviors :max_depth "-1" :recurse "symlinks and directories" :recurse_direction "none" :recurse_file_system "all"))' \
d1/d12/f121,

"test 2" \
'' '' \
'((filepath :operation 11) "^'$ROOT'/d1/.*/f1111")' \
'((behaviors :max_depth "-1" :recurse "symlinks and directories" :recurse_direction "none" :recurse_file_system "all"))' \
d1/d11/d111/f1111,

"test 3" \
'((path :operation 5) "'$ROOT'/d2")' \
'((filename :operation 5) "f21")' \
'' \
'((behaviors :max_depth "-1" :recurse "symlinks and directories" :recurse_direction "none" :recurse_file_system "all"))' \
d2/f21,

"test 4" \
'((path :operation 5) "'$ROOT'/d1/d11")' \
'((filename :operation 11) "^f11[23]$")' \
'' \
'((behaviors :max_depth "-1" :recurse "symlinks and directories" :recurse_direction "none" :recurse_file_system "all"))' \
d1/d11/f112,d1/d11/f113,

"test 5" \
'((path :operation 11) "^'$ROOT'/d1/d1[12]$")' \
'((filename :operation 11) "^f..1$")' \
'' \
'((behaviors :max_depth "-1" :recurse "symlinks and directories" :recurse_direction "none" :recurse_file_system "all"))' \
d1/d11/f111,d1/d12/f121,

"test 6" \
'((path :operation 11) "^'$ROOT'/d1/.*")' \
'((filename :operation 5) "f1111")' \
'' \
'((behaviors :max_depth "-1" :recurse "symlinks and directories" :recurse_direction "none" :recurse_file_system "all"))' \
d1/d11/d111/f1111,

"test 7" \
'((path :operation 5) "'$ROOT'/d1")' \
'((filename :operation 5) "f112")' \
'' \
'((behaviors :max_depth "-1" :recurse "symlinks and directories" :recurse_direction "down" :recurse_file_system "all"))' \
d1/d11/f112,

"test 8" \
'((path :operation 5) "'$ROOT'/d1")' \
'((filename :operation 11) "^f.*1$")' \
'' \
'((behaviors :max_depth "1" :recurse "symlinks and directories" :recurse_direction "down" :recurse_file_system "all"))' \
d1/d11/f111,d1/d12/f121,d1/f11,

"test 9" \
'((path :operation 5) "'$ROOT'/d1/d11/d111")' \
'((filename :operation 5) "f11")' \
'' \
'((behaviors :max_depth "3" :recurse "symlinks and directories" :recurse_direction "up" :recurse_file_system "all"))' \
d1/f11,

EOF

# cleanup
rm -rf "$ROOT"
test_exit
