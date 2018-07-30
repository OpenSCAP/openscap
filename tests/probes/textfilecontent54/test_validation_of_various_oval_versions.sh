#!/bin/bash

LST_VALID="
	tfc54-def-5.4-valid.xml
	tfc54-def-5.5-valid.xml
	tfc54-def-5.6-valid.xml
	tfc54-def-5.7-valid.xml
	tfc54-def-5.8-valid.xml
	tfc54-def-5.9-valid.xml
	tfc54-def-5.10-valid.xml
	tfc54-def-5.10.1-valid.xml
"
LST_INVALID="
	tfc54-def-5.4-invalid.xml
"

function tfc54_validation {
	local ret=0

	echo "*** Validating and evaluating correct OVAL content:"
	for i in $LST_VALID; do
		i="${srcdir}/$i"
		echo "Validating definitions '$i'."
		$OSCAP oval validate "$i" \
			|| { echo "Validation failed."; ret=1; continue; }
		echo "Evaluating definitions '$i'."
		r="$(basename $i .xml)-results.xml"
		$OSCAP oval eval --results "$r" "$i" \
			|| { echo "Evaluation failed."; ret=1; continue; }
		echo "Validating results '$r'."
		$OSCAP oval validate --results "$r" \
			|| { echo "Validation failed."; ret=1; }
	done

	echo "*** Validating incorrect OVAL content:"
	for i in $LST_INVALID; do
		echo "Validating content '$i'."
		$OSCAP oval validate "$i" \
			&& { echo "Validation incorrectly succeeded."; ret=1; }
	done

	return $ret
}

export OSCAP_FULL_VALIDATION=1
tfc54_validation
