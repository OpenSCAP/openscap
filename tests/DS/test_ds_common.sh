#!/usr/bin/env bash

function assert_correct_xlinks()
{
	local DS=$1
	local stderr=$(mktemp)
	$OSCAP info $DS 2> $stderr
	diff $stderr /dev/null
	rm $stderr

	# First of all make sure that there is at least one ds:component-ref.
	[ "$($XPATH $DS 'count(//*[local-name()="component-ref"])')" != "0" ]
	# We want to catch cases when this element has different namespace.
	local ns=$($XPATH $DS 'name(//*[local-name()="component-ref"][1])' | sed 's/:.*$/:/')
	[ "$ns" != "component-ref" ] || ns=""
	# Ensure there is at least some xlink.
	[ "`$XPATH $DS \"count(//${ns}component-ref/@xlink:href)\"`" != "0" ]
	# This asserts that there is none component-ref/xlink:href broken.
	# Previously, we have seen datastreams with broken xlinks (see trac#286).
	[ "`$XPATH $DS  \"count(//${ns}component-ref[substring(@xlink:href, 2, 10000) != (//${ns}component/@${ns}id | //${ns}extended-component/@${ns}id)])\"`" == "0" ]
}
