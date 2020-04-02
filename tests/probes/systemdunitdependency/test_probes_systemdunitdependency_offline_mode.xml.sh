#!/usr/bin/env bash

VALID=$1

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

  <generator>
    <oval:product_name>systemdunitdependency</oval:product_name>
    <oval:product_version>1.0</oval:product_version>
    <oval:schema_version>5.11</oval:schema_version>
    <oval:timestamp>2014-06-18T00:00:00-00:00</oval:timestamp>
  </generator>

  <definitions>

    <definition class="compliance" version="1" id="oval:0:def:1"> <!-- comment="${VALID}" -->
      <metadata><title></title><description></description></metadata>
      <criteria operator="AND">
        <criterion test_ref="oval:0:tst:1"/>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:0:def:2"> <!-- comment="${VALID}" -->
      <metadata><title></title><description></description></metadata>
      <criteria>
        <criterion test_ref="oval:0:tst:2" comment="is local-fs.target a dependency of sysinit.target?"/>
      </criteria>
    </definition>

  </definitions>

  <tests>

    <systemdunitdependency_test check_existence="at_least_one_exists" version="1" id="oval:0:tst:1" check="all" comment="${VALID}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:0:obj:1"/>
    </systemdunitdependency_test>

    <systemdunitdependency_test id="oval:0:tst:2" check_existence="at_least_one_exists" check="all" comment="${VALID}" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:0:obj:2"/>
      <state state_ref="oval:0:ste:1"/>
    </systemdunitdependency_test>

  </tests>

  <objects>

    <systemdunitdependency_object version="1" id="oval:0:obj:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <unit>-.mount</unit>
    </systemdunitdependency_object>

    <systemdunitdependency_object id="oval:0:obj:2" comment="list of dependencies of sysinit.target" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <unit>sysinit.target</unit>
    </systemdunitdependency_object>

  </objects>

  <states>

    <systemdunitdependency_state id="oval:0:ste:1" comment="local-fs.target listed at least once in the dependencies" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <dependency entity_check="at least one">local-fs.target</dependency>
    </systemdunitdependency_state>

    <systemdunitproperty_state id="oval:0:ste:2" comment="ActiveState property equal to 'active'" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <value>active</value>
    </systemdunitproperty_state>

  </states>

</oval_definitions>
EOF