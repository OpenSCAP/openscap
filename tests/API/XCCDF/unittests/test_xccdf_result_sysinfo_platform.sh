#!/usr/bin/env bash

. $builddir/tests/test_common.sh

if [ -n "${CUSTOM_OSCAP+x}" ] ; then
	exit 255
fi

case "$(uname)" in
	Darwin) ;;
	*) exit 255 ;;
esac

./test_xccdf_result_sysinfo_platform
