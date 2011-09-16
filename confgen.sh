#!/bin/sh
D="$(pwd)"

echo ac_probes.sh
./ac_probes/ac_probes.sh "$D/ac_probes/configure.ac.tpl" "$D/ac_probes/" "$D/src/OVAL/probes/" > configure.ac

echo autogen.sh
./autogen.sh
