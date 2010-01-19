#!/usr/bin/env bash

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

  <definitions>

    <definition class="compliance" version="1" id="definition:1" comment="false">
      <criteria>
        <criteria operator="AND">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:2"/>
          <criterion test_ref="test:3"/>
          <criterion test_ref="test:4"/>
          <criterion test_ref="test:5"/>
          <criterion test_ref="test:6"/>
          <criterion test_ref="test:7"/>
          <criterion test_ref="test:8"/>
          <criterion test_ref="test:9"/>
          <criterion test_ref="test:10"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:2" comment="false">
      <criteria>
        <criteria operator="AND">
          <criterion test_ref="test:11"/>
          <criterion test_ref="test:12"/>
          <criterion test_ref="test:13"/>
          <criterion test_ref="test:14"/>
          <criterion test_ref="test:15"/>
          <criterion test_ref="test:16"/>
          <criterion test_ref="test:17"/>
          <criterion test_ref="test:18"/>
          <criterion test_ref="test:19"/>
          <criterion test_ref="test:20"/>
          <criterion test_ref="test:21"/>
          <criterion test_ref="test:22"/>
          <criterion test_ref="test:23"/>
          <criterion test_ref="test:24"/>
          <criterion test_ref="test:25"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:3" comment="false">
      <criteria>
        <criteria operator="AND">
          <criterion test_ref="test:25"/>
          <criterion test_ref="test:26"/>
          <criterion test_ref="test:27"/>
          <criterion test_ref="test:28"/>
          <criterion test_ref="test:29"/>
          <criterion test_ref="test:30"/>
          <criterion test_ref="test:31"/>
          <criterion test_ref="test:32"/>
          <criterion test_ref="test:33"/>
          <criterion test_ref="test:34"/>
          <criterion test_ref="test:35"/>
          <criterion test_ref="test:36"/>
          <criterion test_ref="test:37"/>
          <criterion test_ref="test:38"/>
          <criterion test_ref="test:39"/>
          <criterion test_ref="test:40"/>
          <criterion test_ref="test:41"/>
          <criterion test_ref="test:42"/>
          <criterion test_ref="test:43"/>
          <criterion test_ref="test:44"/>
          <criterion test_ref="test:45"/>
          <criterion test_ref="test:46"/>
          <criterion test_ref="test:47"/>
          <criterion test_ref="test:48"/>
          <criterion test_ref="test:49"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:4" comment="false">
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:5" comment="true">
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:6" comment="false">
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="test:2"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:7" comment="true">
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:8" comment="true">
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:9" comment="false">
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="test:2"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:10" comment="false">
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:11" comment="true">
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:12" comment="true">
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="test:2"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:13" comment="true">
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:14" comment="false">
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:15" comment="false">
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="test:2"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

  </definitions>

  <tests>

    <!-- ##### SINGLE OBJECT TESTS ##### -->

    <!-- ALL_EXIST -->

    <rpminfo_test check_existence="all_exist" comment="true" version="1" id="test:1" check="all">
      <object object_ref="object:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="all_exist" comment="false" version="1" id="test:2" check="all">
      <object object_ref="object:2"/>
    </rpminfo_test>

    <!-- ANY_EXIST -->

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:3" check="all">
      <object object_ref="object:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:4" check="all">
      <object object_ref="object:2"/>
    </rpminfo_test>

    <!-- AT_LEAST_ONE_EXISTS -->

    <rpminfo_test check_existence="at_least_one_exists" comment="true" version="1" id="test:5" check="all">
      <object object_ref="object:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="at_least_one_existS" comment="false" version="1" id="test:6" check="all">
      <object object_ref="object:2"/>
    </rpminfo_test>

    <!-- ONLY_ONE_EXISTS -->

    <rpminfo_test check_existence="only_one_exists" comment="true" version="1" id="test:7" check="all">
      <object object_ref="object:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="only_one_exists" comment="false" version="1" id="test:8" check="all">
      <object object_ref="object:2"/>
    </rpminfo_test>

    <!-- NONE_EXIST -->

    <rpminfo_test check_existence="none_exist" comment="false" version="1" id="test:9" check="all">
      <object object_ref="object:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="none_exist" comment="true" version="1" id="test:10" check="all">
      <object object_ref="object:2"/>
    </rpminfo_test>

    <!-- ##### MULTI OBJECT TESTS ##### -->

    <!-- ALL_EXIST -->

    <rpminfo_test check_existence="all_exist" comment="true" version="1" id="test:11" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="all_exist" comment="false" version="1" id="test:12" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="all_exist" comment="false" version="1" id="test:13" check="all">
      <object object_ref="object:2"/>
      <object object_ref="object:4"/>
    </rpminfo_test>

    <!-- ANY_EXIST -->

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:14" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:15" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:16" check="all">
      <object object_ref="object:2"/>
      <object object_ref="object:4"/>
    </rpminfo_test>

    <!-- AT_LEAST_ONE_EXISTS -->

    <rpminfo_test check_existence="at_least_one_exists" comment="true" version="1" id="test:17" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="at_least_one_exists" comment="true" version="1" id="test:18" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="at_least_one_exists" comment="false" version="1" id="test:19" check="all">
      <object object_ref="object:2"/>
      <object object_ref="object:4"/>
    </rpminfo_test>

    <!-- ONLY_ONE_EXISTS -->

    <rpminfo_test check_existence="only_one_exists" comment="false" version="1" id="test:20" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="only_one_exists" comment="true" version="1" id="test:21" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="only_one_exists" comment="false" version="1" id="test:22" check="all">
      <object object_ref="object:2"/>
      <object object_ref="object:4"/>
    </rpminfo_test>

    <!-- NONE_EXIST -->

    <rpminfo_test check_existence="none_exist" comment="false" version="1" id="test:23" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="none_exist" comment="false" version="1" id="test:24" check="all">
      <object object_ref="object:1"/>
      <object object_ref="object:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="none_exist" comment="true" version="1" id="test:25" check="all">
      <object object_ref="object:2"/>
      <object object_ref="object:4"/>
    </rpminfo_test>

    <!-- ##### SINGLE OBJECT-STATE TESTS ##### -->

    <!-- ALL_EXIST -->

    <rpminfo_test check_existence="all_exist" comment="true" version="1" id="test:26" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="all_exist" comment="true" version="1" id="test:27" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="all_exist" comment="false" version="1" id="test:28" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="all_exist" comment="false" version="1" id="test:29" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="all_exist" comment="false" version="1" id="test:30" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="all_exist" comment="false" version="1" id="test:31" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

    <!-- ANY_EXIST -->

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:32" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:33" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:34" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:35" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:36" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="any_exist" comment="true" version="1" id="test:37" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

    <!-- AT_LEAST_ONE_EXISTS -->

    <rpminfo_test check_existence="at_least_one_exists" comment="true" version="1" id="test:32" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="at_least_one_exists" comment="true" version="1" id="test:33" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="at_least_one_exists" comment="true" version="1" id="test:34" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="at_least_one_exists" comment="true" version="1" id="test:35" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="at_least_one_exists" comment="false" version="1" id="test:36" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="at_least_one_exists" comment="false" version="1" id="test:37" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

    <!-- ONLY_ONE_EXISTS -->

    <rpminfo_test check_existence="only_one_exists" comment="false" version="1" id="test:38" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="only_one_exists" comment="false" version="1" id="test:39" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="only_one_exists" comment="true" version="1" id="test:40" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="only_one_exists" comment="true" version="1" id="test:41" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="only_one_exists" comment="false" version="1" id="test:42" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="only_one_exists" comment="false" version="1" id="test:43" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

    <!-- NONE_EXIST -->

    <rpminfo_test check_existence="none_exist" comment="false" version="1" id="test:44" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="none_exist" comment="false" version="1" id="test:45" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

    <rpminfo_test check_existence="none_exist" comment="false" version="1" id="test:46" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="none_exist" comment="false" version="1" id="test:47" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:3"/>
    </rpminfo_test>

    <rpminfo_test check_existence="none_exist" comment="true" version="1" id="test:48" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:1"/>
    </rpminfo_test>

    <rpminfo_test check_existence="none_exist" comment="true" version="1" id="test:49" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:2"/>
    </rpminfo_test>

  </tests>

  <objects>

    <!-- EXISTING OBJECT -->
    <rpminfo_object version="1" id="object:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name>${1}</name>
    </rpminfo_object>

    <!-- NON-EXISTING OBJECT -->
    <rpminfo_object version="1" id="object:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name>${2}</name>
    </rpminfo_object>

    <!-- EXISTING OBJECT -->
    <rpminfo_object version="1" id="object:3" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name>${3}</name>
    </rpminfo_object>

    <!-- NON-EXISTING OBJECT -->
    <rpminfo_object version="1" id="object:4" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name>${4}</name>
    </rpminfo_object>

  </objects>

  <states>

    <!-- VALID STATE -->
    <rpminfo_state version="1" id="state:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <version>${5}</version>
    </rpminfo_state>

    <!-- VALID STATE PATTERN -->
    <rpminfo_state version="1" id="state:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <version operation="pattern match">${6}</version>
    </rpminfo_state>

    <!-- INVALID STATE -->
    <rpminfo_state version="1" id="state:3" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <version>${7}</version>
    </rpminfo_state>
 
    <!-- INVALID STATE PATTERN -->
    <rpminfo_state version="1" id="state:4" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <version operation="pattern match">${8}</version>
    </rpminfo_state>

  </states>

</oval_definitions>
EOF
