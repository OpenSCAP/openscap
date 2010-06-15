#!/bin/sh

export OVAL_PROBE_DIR="`pwd`/../src/OVAL/probes/"
echo ""
echo "---------------- Valgrind checks -----------------"
./vgrun.sh "./test_probes OVAL/probes/file-set2.xml /dev/null"
./vgrun.sh "./test_probes OVAL/probes/test_probes_file.xml /dev/null"
./vgrun.sh "./test_probes OVAL/probes/test_probes_textfilecontent54.xml /dev/null"
./vgrun.sh "./test_probes OVAL/probes/test_probes_family.xml /dev/null"
echo "--------------------------------------------------"

exit 0
