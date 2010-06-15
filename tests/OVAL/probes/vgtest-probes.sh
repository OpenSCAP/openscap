#!/bin/sh

export OVAL_PROBE_DIR="`pwd`/../src/OVAL/probes/"
echo ""
echo "---------------- Valgrind checks -----------------"
./vgrun.sh "./test_probes OVAL/probes/file-set2.xml file-set2.result.xml"
./vgrun.sh "./test_probes OVAL/probes/test_probes_file.xml"
./vgrun.sh "./test_probes OVAL/probes/test_probes_textfilecontent54.xml"
./vgrun.sh "./test_probes OVAL/probes/test_probes_family.xml"
echo "--------------------------------------------------"

exit 0
