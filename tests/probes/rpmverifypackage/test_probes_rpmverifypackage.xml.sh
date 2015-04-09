#!/bin/bash

# This template generates OVAL definitions for rpmverifypackage_object.

RPM_NAME=$1
RPM_EPOCH=$2
RPM_ARCH=$3
RPM_VERSION=$4
RPM_RELEASE=$5

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

  <generator>
    <oval:product_name>me</oval:product_name>
    <oval:product_version>0.123</oval:product_version>
    <oval:schema_version>5.11</oval:schema_version>
    <oval:timestamp>2008-03-31T00:00:00-00:00</oval:timestamp>
  </generator>

  <definitions>
    <definition class="compliance" version="1" id="oval:x:def:1">
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
        <criterion test_ref="oval:x:tst:1"/>
      </criteria>
    </definition>
  </definitions>

  <tests>
    <rpmverifypackage_test id="oval:x:tst:1" version="1" check="all" comment="blablabla" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:x:obj:1"/>
    </rpmverifypackage_test>
  </tests>

  <objects>
    <rpmverifypackage_object id="oval:x:obj:1" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
        <name>$RPM_NAME</name>
        <epoch>$RPM_EPOCH</epoch>
        <version>$RPM_VERSION</version>
        <release>$RPM_RELEASE</release>
        <arch>$RPM_ARCH</arch>
    </rpmverifypackage_object>
  </objects>

</oval_definitions>
EOF
