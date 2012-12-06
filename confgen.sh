#!/bin/sh
D="$(pwd)"

echo -n "ac_probes.sh... "
C="$(./ac_probes/ac_probes.sh "$D/ac_probes/configure.ac.tpl" "$D/ac_probes/" "$D/src/OVAL/probes/")"

ret=$?
if [ $ret -ne 0 ]; then
    echo "failed: $ret"
    exit $ret
else
    echo "$C" > configure.ac
    echo "ok"
fi

echo -n "autogen.sh... "
./autogen.sh

ret=$?
if [ $ret -ne 0 ]; then
    echo "failed: $ret"
    exit $ret
else
    echo "ok"
fi
