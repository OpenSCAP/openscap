#!/bin/sh

RET_INTERNAL=2
RET_FAILED=1
RET_SUCCESS=0

ROOT=$1

if [ -z "$ROOT" ]; then
	echo "Usage: $1 <rootdir>"
	exit 1
fi

mkdir -p "${ROOT}" && pushd "${ROOT}" || exit $RET_INTERNAL

for subdir in A B C D E F; do
	mkdir -p "${subdir}" || exit $RET_INTERNAL
	pushd "${subdir}"
	for subdir_file in 0 1 2 3 4 5 6 7 8 9; do
		NAME="${subdir}${subdir_file}"
		RANDLEN=$(LANG=C egrep -ioam1 '[1-9][0-9]' /dev/urandom | head -n 1)
		touch $NAME
		openssl rand $RANDLEN > $NAME
	done
	popd
done
