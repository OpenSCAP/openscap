#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

# on some systems sysctl might live in sbin, which can cause problems for
# non root users
PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin

function perform_test {
	probecheck "sysctl" || return 255
	case $(uname) in
		Darwin)
			# macOS exposes many implementation-specific sysctls; this strict parity
			# test is intended for Linux/FreeBSD naming behavior.
			return 255
			;;
	esac

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
		Darwin)
			sysctl -aN 2> /dev/null > "$sysctlNames"
			;;
		Linux)
			# sysctl has duplicities in output
			# hide permission errors like: "sysctl: permission denied on key 'fs.protected_hardlinks'"
			# kernel parameters might use "/" and "." separators interchangeably - normalizing
			sysctl -a --deprecated 2> /dev/null | tr "/" "." | cut -d "=" -f 1 | tr -d " " | sort -u > "$sysctlNames"
			;;
		*)
			return 255
			;;
	esac

	grep unix-sys:name "$result" | xsed -E 's;.*>(.*)<.*;\1;g' | sort > "$ourNames"

	echo "Diff (sysctlNames / ourNames): ------"
	diff "$sysctlNames" "$ourNames"
	echo "-------------------------------------"

	# remove known, non-fatal errors from stderr in a portable way (BSD/GNU sed)
	tmp_filtered=$(mktemp ${name}.stderr.filtered.XXXXXX)
	sed -E "/^E: oscap: +Can't read sysctl value from /d" "$stderr" > "$tmp_filtered"
	mv "$tmp_filtered" "$stderr"
	tmp_filtered=$(mktemp ${name}.stderr.filtered.XXXXXX)
	sed -E "/^E: oscap: +An error.*, Operation not permitted/d" "$stderr" > "$tmp_filtered"
	mv "$tmp_filtered" "$stderr"

	# remove oscap error message related to gibberish binary entries
	# that can't fit into 8K buffer and result in errno 14
	# (for example /proc/sys/kernel/spl/hostid could be the case)
	tmp_filtered=$(mktemp ${name}.stderr.filtered.XXXXXX)
	sed -E "/^E: oscap: +An error.*14, Bad address/d" "$stderr" > "$tmp_filtered"
	mv "$tmp_filtered" "$stderr"
	tmp_filtered=$(mktemp ${name}.stderr.filtered.XXXXXX)
	sed "/^.*hugepages.*$/d" "$stderr" > "$tmp_filtered"
	mv "$tmp_filtered" "$stderr"

	echo "Errors (without messages related to permissions):"
	cat "$stderr"

	[ ! -s $stderr ]

	rm $stderr $result $ourNames $sysctlNames
}

perform_test
