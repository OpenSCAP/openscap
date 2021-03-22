#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

# on some systems sysctl might live in sbin, which can cause problems for
# non root users
PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin

# non root users are not able to access some kernel params, so they get blacklisted
SYSCTL_BLACKLIST='
	fs.protected_hardlinks
	fs.protected_fifos
	fs.protected_regular
	fs.protected_symlinks
	kernel.cad_pid
	kernel.unprivileged_userns_apparmor_policy
	kernel.usermodehelper.bset
	kernel.usermodehelper.inheritable
	net.core.bpf_jit_harden
	net.core.bpf_jit_kallsyms
	net.core.bpf_jit_limit
	net.ipv4.tcp_fastopen_key
	stable_secret
	vm.mmap_rnd_bits
	vm.mmap_rnd_compat_bits
	vm.stat_refresh'

SYSCTL_BLACKLIST_REGEX="$(printf '\|%s' $SYSCTL_BLACKLIST)"
# strip leading '\|'
SYSCTL_BLACKLIST_REGEX=${SYSCTL_BLACKLIST_REGEX:2}

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
		sysctl -aN --deprecated 2> /dev/null | grep -v $SYSCTL_BLACKLIST_REGEX | tr "/" "." | sort -u > "$sysctlNames"
		;;
esac

grep unix-sys:name "$result" | grep -v $SYSCTL_BLACKLIST_REGEX | xsed -E 's;.*>(.*)<.*;\1;g' | sort > "$ourNames"

# If procps_ver > 3.3.12 we need to filter *stable_secret and vm.stat_refresh
# options from the sysctl output, for more details see
# https://github.com/OpenSCAP/openscap/issues/1152.
procps_ver="$(package_version procps-ng procps)"

lowest_ver=$(echo -e "3.3.12\n$procps_ver" | sort -V | head -n1)
if [ "$procps_ver" != "$lowest_ver" ]; then
	sed -i '/net.ipv6.conf.*stable_secret$/d' "$sysctlNames"
	sed -i '/.*vm.stat_refresh/d' "$sysctlNames"
fi

echo "Diff (sysctlNames / ourNames): ------"
diff "$sysctlNames" "$ourNames"
echo "-------------------------------------"

# remove oscap error message related to permissions from stderr
sed -i -E "/^E: oscap: +Can't read sysctl value from /d" "$stderr"

# remove oscap error message related to gibberish binary entries
# that can't fit into 8K buffer and result in errno 14
# (for example /proc/sys/kernel/spl/hostid could be the case)
sed -i -E "/^E: oscap: +An error.*14, Bad address/d" "$stderr"

echo "Errors (without messages related to permissions):"
cat "$stderr"

[ ! -s $stderr ]

rm $stderr $result $ourNames $sysctlNames
}

perform_test
