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

stderr="$(mktemp)"

# Run oscap in "oscap_cgroup" on single CPU core and with limited memory
$build_dir/run cgexec -g "memory,cpuset:oscap_cgroup" $build_dir/utils/oscap xccdf eval --tailoring-file $tailoring --profile $profile --results $results --report $report $ds 2>$stderr || ret="$?"

grep -qiE "(^E:|killed)" "$stderr" && exit 1
rm -f "$stderr"

dmesg_out="$(mktemp)"
dmesg -H | grep oscap &> "$dmesg_out" || ret="$?"
grep -qiE "(killed|out of memory|oom)" "$dmesg_out" && exit 1
rm -f "$dmesg_out"

rm -f "$ds"
