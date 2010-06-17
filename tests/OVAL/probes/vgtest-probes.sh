#!/usr/bin/env bash

VGTEST_TMPDIR="/tmp/vgtest"

if [[ ! -d "$VGTEST_TMPDIR" ]]; then
    mkdir -p "$VGTEST_TMPDIR" || exit 1
fi

for xmlgen in OVAL/probes/test_probes_*.xml.sh; do
    bash "$xmlgen" > "$VGTEST_TMPDIR/$(basename "$xmlgen" | sed 's|.sh$||')" 2> /dev/null
done

for xml in OVAL/probes/*.xml; do
    cp "$xml" "$VGTEST_TMPDIR"
done

export OVAL_PROBE_DIR="`pwd`/../src/OVAL/probes/"
export SEAP_DEBUGLOG_DISABLE=1
export SEXP_VALIDATE_DISABLE=1

echo ""
echo "---------------- Valgrind checks -----------------"
for xml in "$VGTEST_TMPDIR"/*.xml; do
    ./vgrun.sh "./test_probes $xml /dev/null"
done
echo "--------------------------------------------------"

exit 0
