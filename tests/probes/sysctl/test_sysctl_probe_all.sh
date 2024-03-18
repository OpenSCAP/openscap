#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

# on some systems sysctl might live in sbin, which can cause problems for
# non root users
PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin

function perform_test {
	probecheck "sysctl" || return 255

	name=$(basename $0 .sh)

	result=$(mktemp ${name}.res.out.XXXXXX)
	stderr=$(mktemp ${name}.err.out.XXXXXX)
	ourNames=$(mktemp ${name}.our.out.XXXXXX)
	sysctlNames=$(mktemp ${name}.sysctl.out.XXXXXX)

	echo "Result file: $result"
	echo "Our names file: $ourNames"
	echo "Sysctl names file: $sysctlNames"
	echo "Errors file: $stderr"

	$OSCAP oval eval --results $result $srcdir/test_sysctl_probe_all.oval.xml > /dev/null 2>$stderr

	case $(uname) in
		FreeBSD)
			sysctl -aN 2> /dev/null > "$sysctlNames"
			;;
		Linux)
			# sysctl has duplicities in output
			# hide permission errors like: "sysctl: permission denied on key 'fs.protected_hardlinks'"
			# kernel parameters might use "/" and "." separators interchangeably - normalizing
			sysctl -a --deprecated 2> /dev/null | tr "/" "." | cut -d "=" -f 1 | tr -d " " | sort -u > "$sysctlNames"
			;;
	esac

	grep unix-sys:name "$result" | xsed -E 's;.*>(.*)<.*;\1;g' | sort > "$ourNames"

	echo "Diff (sysctlNames / ourNames): ------"
	diff "$sysctlNames" "$ourNames"
	echo "-------------------------------------"

	# remove oscap error message related to permissions from stderr
	sed -i -E "/^E: oscap: +Can't read sysctl value from /d" "$stderr"
	sed -i -E "/^E: oscap: +An error.*, Operation not permitted/d" "$stderr"

	# remove oscap error message related to gibberish binary entries
	# that can't fit into 8K buffer and result in errno 14
	# (for example /proc/sys/kernel/spl/hostid could be the case)
	sed -i -E "/^E: oscap: +An error.*14, Bad address/d" "$stderr"
	sed -i "/^.*hugepages.*$/d" "$stderr"

	echo "Errors (without messages related to permissions):"
	cat "$stderr"

	[ ! -s $stderr ]

	rm $stderr $result $ourNames $sysctlNames
}

perform_test
