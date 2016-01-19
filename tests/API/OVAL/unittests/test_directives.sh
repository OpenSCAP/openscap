#!/bin/bash

# Test basic usage of OVAL Directives. OVAL Directives here are used to exclude
# <definitions> element from the resulting OVAL Results file.

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
stderr=$(mktemp ${name}.err.XXXXXX)

$OSCAP oval eval --results $result --directives $srcdir/$name.xml $srcdir/comment.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
[ -f $result ]

assert_exists 0 '/oval_results/oval_definitions'
assert_exists 1 '/oval_results/directives[@include_source_definitions="false"]'
assert_exists 1 '/oval_results/directives/definition_true[@reported="true" and @content="full"]'
assert_exists 1 '/oval_results/directives/definition_false[@reported="true" and @content="full"]'
assert_exists 1 '/oval_results/directives/definition_unknown[@reported="true" and @content="full"]'
assert_exists 1 '/oval_results/directives/definition_error[@reported="true" and @content="full"]'
assert_exists 1 '/oval_results/directives/definition_not_evaluated[@reported="true" and @content="full"]'
assert_exists 1 '/oval_results/directives/definition_not_applicable[@reported="true" and @content="full"]'

rm $result
