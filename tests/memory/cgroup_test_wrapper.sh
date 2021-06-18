#!/bin/bash

# Create a cgroup for memory and cpuset named "oscap_cgroup"
sudo -n cgcreate -t $USER:$USER -a $USER:$USER -g "memory,cpuset:oscap_cgroup"
# Limit "oscap_cgroup" to single CPU core
sudo -n cgset -r cpuset.cpus=0 -r cpuset.mems=0 "oscap_cgroup"
# Limit "oscap_cgroup" memory to 700 MB
sudo -n cgset -r memory.limit_in_bytes=700M "oscap_cgroup"

# We need to set also a limit on swap space, without this a process
# running in "oscap_cgroup" would be able to use unlimited swap space
# and the memory issue might not be triggered.
# The option memory.memsw.limit_in_bytes is a limit of memory+swap
# usage together. So in case that memory.limit_in_bytes is equal to
# memory.memsw.limit_in_bytes this means that 0 swap is allowed.

# Limit "oscap_cgroup" memory+swap to 700 MB
sudo -n cgset -r memory.memsw.limit_in_bytes=700M "oscap_cgroup"

./cgroup_test.sh

# Delete "oscap_cgroup"
sudo -n cgdelete "memory,cpuset:oscap_cgroup"