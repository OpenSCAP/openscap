#!/usr/bin/env bash
#
# Fetches schema and schematron files from remote locations.
# Currently supports only Schematron for XCCDF.
#
# Author: Simon Lukasik

set -e -o pipefail


function fetch_schematron(){
	local skeleton=build/iso_schematron_skeleton_for_xslt1.xsl
	if [ ! -f $skeleton ]; then
		[ -d build ] || mkdir build/
		pushd build
		wget http://www.schematron.com/tmp/iso-schematron-xslt1.zip
		unzip iso-schematron-xslt1.zip
		popd
	fi
	local target=$1
	local url=$2
	wget -O ${target}.xsd $url
	xsltproc $skeleton ${target}.xsd > $target
	rm ${target}.xsd
}

fetch_schematron xccdf/1.2/xccdf_1.2-schematron.xsl		http://scap.nist.gov/schema/xccdf/1.2/xccdf_1.2.sch
