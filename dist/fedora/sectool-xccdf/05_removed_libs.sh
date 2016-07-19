#!/bin/bash

# Copyright (C) 2008 Red Hat, Inc.
# Written by Dan Kopecek <dkopecek@redhat.com>
# Original idea comes from the Tiger project
# Wed Feb  6 17:33:20 CET 2008
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>


PROC='/proc'

echo "If this test prints out a lot of warnings/errors then try to restart running processes or reboot your system."

ret=$XCCDF_RESULT_PASS

while read process; do
    NAME="$(echo ${process/[[:space:]][0-9]*/})"
    PID=$(echo ${process/$NAME/})

    if [[ -f "${PROC}/${PID}/maps" ]]; then
	while read inolib; do
		LIB="$(echo ${inolib/[0-9]*[[:space:]]/})"
		INO=$(echo ${inolib/$LIB/})

		if [[ -n "${LIB}" ]]; then
			if [[ -f "${LIB}" ]]; then
				if (( $(stat -c '%i' "${LIB}") != ${INO} )); then
					echo "Process ${NAME} (pid: ${PID}) is using shared object ${LIB} that was probably updated!"
					ret=$XCCDF_RESULT_FAIL
				fi
			else
				echo "Process ${NAME} (pid: ${PID}) is using shared object ${LIB} that doesn't exist!"
				ret=$XCCDF_RESULT_FAIL
			fi
		fi
	done <<EOF
`cat "${PROC}/${PID}/maps" | sed -n 's|^.*[[:space:]]\([0-9]\{1,\}[[:space:]]*/[[:print:]]*\.so[0-9\.]*\)$|\1|p' | sort | uniq`
EOF
    fi
done <<EOF
`ps --no-heading -eo comm,pid`
EOF

exit $ret

