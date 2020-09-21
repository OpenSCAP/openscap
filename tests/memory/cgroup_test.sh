#!/bin/bash

# Description: Test for openscap excessive memory consumption.
# Author: Matus Marhefka <mmarhefk@redhat.com>, Jan Černý <jcerny@redhat.com>

set -e -o pipefail

build_dir="../../build"

profile="xccdf_org.ssgproject.content_profile_oom"
os_name=$(sed -nr "s/^NAME=\"?([^\"]*)\"?/\1/p" /etc/os-release)
os_version=$(sed -nr "s/^VERSION=\"?([^\"]*)\"?/\1/p" /etc/os-release)

if [[ $os_name == "Red Hat Enterprise Linux"* ]] ; then
    if [[ $os_version =~ ^7.* ]] ; then
        tailoring="ssg-rhel7-oom-tailoring.xml"
        ds="ssg-rhel7-ds.xml"
    elif [[ $os_version =~ ^8.* ]] ; then
        tailoring="ssg-rhel8-oom-tailoring.xml"
        ds="ssg-rhel8-ds.xml"
    else
        echo "This test doesn't support $os_name $os_version" >&2
        exit 1
    fi
else
    echo "This test doesn't support $os_name $os_version" >&2
    exit 1
fi

# Extracting source datastream
rm -f "$ds"
bunzip2 -k "$ds.bz2"

# This will emulate running on a system with 1 CPU and low memory.
# Memory is limited to 700 MB, even though minimal memory requirements
# are between 1-2 GB per single CPU core and they differ based on
# CPU architecture and RHEL versions. The 700 MB limit should be more
# than enough, especially considering that there is an additional
# software running on customers systems.
# More information about memory requirements can be found at
# https://access.redhat.com/articles/rhel-limits#minimum-required-memory-3

# Create a cgroup for memory and cpuset named "oscap_cgroup"
cgcreate -g "memory,cpuset:oscap_cgroup"
# Limit "oscap_cgroup" to single CPU core
cgset -r cpuset.cpus=0 -r cpuset.mems=0 "oscap_cgroup"
# Limit "oscap_cgroup" memory to 700 MB
cgset -r memory.limit_in_bytes=700M "oscap_cgroup"

# We need to set also a limit on swap space, without this a process
# running in "oscap_cgroup" would be able to use unlimited swap space
# and the memory issue might not be triggered.
# The option memory.memsw.limit_in_bytes is a limit of memory+swap
# usage together. So in case that memory.limit_in_bytes is equal to
# memory.memsw.limit_in_bytes this means that 0 swap is allowed.

# Limit "oscap_cgroup" memory+swap to 700 MB
cgset -r memory.memsw.limit_in_bytes=700M "oscap_cgroup"

stderr="$(mktemp)"

# Run oscap in "oscap_cgroup" on single CPU core and with limited memory
$build_dir/run cgexec -g "memory,cpuset:oscap_cgroup" $build_dir/utils/oscap xccdf eval --tailoring-file $tailoring --profile $profile --results $results --report $report $ds 2>$stderr || ret="$?"

grep -qiE "(^E:|killed)" "$stderr" && exit 1
rm -f "$stderr"

dmesg_out="$(mktemp)"
dmesg -H | grep oscap &> "$dmesg_out" || ret="$?"
grep -qiE "(killed|out of memory|oom)" "$dmesg_out" && exit 1
rm -f "$dmesg_out"

# Delete "oscap_cgroup"
cgdelete "memory,cpuset:oscap_cgroup"
rm -f "$ds"
