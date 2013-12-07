#!/usr/bin/env bash

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

      <generator>
            <oval:product_name>runlevel</oval:product_name>
            <oval:product_version>1.0</oval:product_version>
            <oval:schema_version>5.8</oval:schema_version>
            <oval:timestamp>2022-03-31T00:00:00-00:00</oval:timestamp>
      </generator>

  <definitions>
    <definition class="compliance" version="1" id="oval:1:def:1" > <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
        <criterion test_ref="oval:1:tst:1"/>
      </criteria>
    </definition>
  </definitions>

  <tests>
    <runlevel_test comment="true" version="1" id="oval:1:tst:1" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </runlevel_test>
  </tests>

  <objects>
    <runlevel_object version="1" id="oval:1:obj:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <service_name operation="pattern match">^((${1})|(${2}))$</service_name>
      <runlevel>3</runlevel>
    </runlevel_object>
  </objects>
</oval_definitions>
EOF
