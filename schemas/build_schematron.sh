#!/usr/bin/env bash
#
# Prerequisities
#
# 1. xsltproc tool
#   $ yum install libxslt
#
# 2. ISO schematron XSLT v1 files
#   $ wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/schematron/iso-schematron-xslt1.zip
#   $ unzip iso-schematron-xslt1.zip
#
#
# 3. XSLT template to extract SCH from XSD
#   $ wget https://github.com/OVALProject/Language/blob/${OVAL_VERSION}/tools/ExtractSchFromXSD.xsl
#
# Creating a validation XSL file
#
# 1. use xsltproc to create a *.sch file, e.g. for oval definitions:
#   $ xsltproc ExtractSchFromXSD.xsl oval-definitions-schema.xsd > oval-definitions-schematron.sch
#
# 2. use xsltproc to generate target .xsl, which will be used to validate documents
#   $ xsltproc iso_schematron_skeleton_for_xslt1.xsl oval-definitions-schematron.sch >oval/${OVAL_VERSION}/oval-definitions-schematron.xsl
#
# 3. Make sure you do not override workarounds we carry on. (You can use git-merge).
#
#
# Validating an document without oscap tool
#
#   $ xsltproc oval/${OVAL_VERSION}/oval-definitions-schematron.xsl  path/to/your/file.xml



version="$1"

wget -N https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/schematron/iso-schematron-xslt1.zip
unzip -o iso-schematron-xslt1.zip

wget https://github.com/OVALProject/Language/blob/$version/tools/ExtractSchFromXSD.xsl

xsltproc ExtractSchFromXSD.xsl oval-definitions-schema.xsd > oval-definitions-schematron.sch
xsltproc iso_schematron_skeleton_for_xslt1.xsl "oval-definitions-schematron.sch" >"oval-definitions-schematron.xsl"

xsltproc ExtractSchFromXSD.xsl oval-system-characteristics-schema.xsd > oval-system-characteristics-schematron.sch
xsltproc iso_schematron_skeleton_for_xslt1.xsl "oval-system-characteristics-schematron.sch" >"oval-system-characteristic-schematron.xsl"

xsltproc ExtractSchFromXSD.xsl oval-results-schema.xsd > oval-results-schematron.sch
xsltproc iso_schematron_skeleton_for_xslt1.xsl "oval-results-schematron.sch" >"oval-results-schematron.xsl"

xsltproc ExtractSchFromXSD.xsl oval-variables-schema.xsd > oval-variables-schematron.sch
xsltproc iso_schematron_skeleton_for_xslt1.xsl "oval-variables-schematron.sch" >"oval-variables-schematron.xsl"

xsltproc ExtractSchFromXSD.xsl oval-directives-schema.xsd > oval-directives-schematron.sch
xsltproc iso_schematron_skeleton_for_xslt1.xsl "oval-directives-schematron.sch" >"oval-directives-schematron.xsl"
