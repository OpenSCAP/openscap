#!/usr/bin/env bash

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

  <generator>
    <oval:product_name>iflisteners</oval:product_name>
    <oval:product_version>1.0</oval:product_version>
    <oval:schema_version>5.9</oval:schema_version>
    <oval:timestamp>2011-08-03T00:00:00-00:00</oval:timestamp>
  </generator>

  <definitions>

    <definition class="compliance" version="1" id="oval:1:def:1">  <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
        <criteria operator="AND">
	  <criterion test_ref="oval:1:tst:1" />
        </criteria>
      </criteria>
    </definition>

  </definitions>

  <tests>

    <!-- DEFAULT STATELESS OBJECT -->

    <!-- check="all" -->
    <iflisteners_test version="1" id="oval:1:tst:1" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </iflisteners_test>

  </tests>

  <objects>

    <iflisteners_object version="1" id="oval:1:obj:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <interface_name>lo</interface_name>
    </iflisteners_object>

  </objects>

  <states>

  <iflisteners_state version="1" id="oval:1:ste:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
    <interface_name>lo</interface_name>
    <protocol>ETH_P_ALL</protocol>
    <hw_address>00:00:00:00:00:00</hw_address>
    <program_name>tcpdump</program_name>
    <pid operation="greater than" datatype="int" >0</pid>
    <user_id>72</user_id>
  </iflisteners_state>

  </states>

</oval_definitions>
EOF

exit 1
