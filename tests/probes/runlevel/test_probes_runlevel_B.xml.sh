#!/usr/bin/env bash

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

      <generator>
            <oval:product_name>runlevel</oval:product_name>
            <oval:product_version>1.0</oval:product_version>
            <oval:schema_version>5.4</oval:schema_version>
            <oval:timestamp>2008-03-31T00:00:00-00:00</oval:timestamp>
      </generator>

  <definitions>

    <definition class="compliance" version="1" id="oval:1:def:1" > <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
        <criteria operator="AND">
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:2"/>
          <criterion test_ref="oval:1:tst:3"/>
          <criterion test_ref="oval:1:tst:4"/>
          <criterion test_ref="oval:1:tst:5"/>
          <criterion test_ref="oval:1:tst:6"/>
          <criterion test_ref="oval:1:tst:7"/>
          <criterion test_ref="oval:1:tst:8"/>
          <criterion test_ref="oval:1:tst:9"/>
          <criterion test_ref="oval:1:tst:10"/>
          <criterion test_ref="oval:1:tst:11"/>
          <criterion test_ref="oval:1:tst:12"/>
          <criterion test_ref="oval:1:tst:13"/>
          <criterion test_ref="oval:1:tst:14"/>
          <criterion test_ref="oval:1:tst:15"/>
          <criterion test_ref="oval:1:tst:16"/>
          <criterion test_ref="oval:1:tst:17"/>
          <criterion test_ref="oval:1:tst:18"/>
          <criterion test_ref="oval:1:tst:19"/>
          <criterion test_ref="oval:1:tst:20"/>
          <criterion test_ref="oval:1:tst:21"/>
          <criterion test_ref="oval:1:tst:22"/>
          <criterion test_ref="oval:1:tst:23"/>
          <criterion test_ref="oval:1:tst:24"/>
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:26"/>
          <criterion test_ref="oval:1:tst:27"/>
          <criterion test_ref="oval:1:tst:28"/>
          <criterion test_ref="oval:1:tst:29"/>
          <criterion test_ref="oval:1:tst:30"/>
          <criterion test_ref="oval:1:tst:31"/>
          <criterion test_ref="oval:1:tst:32"/>
          <criterion test_ref="oval:1:tst:33"/>
          <criterion test_ref="oval:1:tst:34"/>
          <criterion test_ref="oval:1:tst:35"/>
          <criterion test_ref="oval:1:tst:36"/>
          <criterion test_ref="oval:1:tst:37"/>
          <criterion test_ref="oval:1:tst:38"/>
          <criterion test_ref="oval:1:tst:39"/>
          <criterion test_ref="oval:1:tst:40"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:2" > <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:2"/>
        </criteria>
      </criteria>
    </definition>

   <definition class="compliance" version="1" id="oval:1:def:3" > <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:4" > <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="oval:1:tst:2"/>
          <criterion test_ref="oval:1:tst:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:5" > <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:6" > <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:7" > <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="oval:1:tst:2"/>
          <criterion test_ref="oval:1:tst:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:8" > <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:9" > <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:10" > <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="oval:1:tst:2"/>
          <criterion test_ref="oval:1:tst:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:11" > <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:12" > <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:13" > <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="oval:1:tst:2"/>
          <criterion test_ref="oval:1:tst:2"/>
        </criteria>
      </criteria>
    </definition>

  </definitions>

  <tests>

    <!-- ##### SINGLE OBJECT TESTS ##### -->

    <!-- ALL_EXIST -->

    <runlevel_test check_existence="all_exist" comment="true" version="1" id="oval:1:tst:1" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="oval:1:tst:2" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="oval:1:tst:3" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="true" version="1" id="oval:1:tst:4" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="oval:1:tst:5" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="oval:1:tst:6" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="oval:1:tst:7" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="oval:1:tst:8" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <!-- ANY_EXIST -->

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="oval:1:tst:9" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="false" version="1" id="oval:1:tst:10" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="false" version="1" id="oval:1:tst:11" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="oval:1:tst:12" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="oval:1:tst:13" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="oval:1:tst:14" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="oval:1:tst:15" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="oval:1:tst:16" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <!-- AT_LEAST_ONE_EXISTS -->

    <runlevel_test check_existence="at_least_one_exists" comment="true" version="1" id="oval:1:tst:17" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="oval:1:tst:18" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="oval:1:tst:19" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="true" version="1" id="oval:1:tst:20" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="oval:1:tst:21" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="oval:1:tst:22" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="oval:1:tst:23" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="oval:1:tst:24" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <!-- ONLY_ONE_EXISTS -->

    <runlevel_test check_existence="only_one_exists" comment="true" version="1" id="oval:1:tst:25" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="oval:1:tst:26" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="oval:1:tst:27" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="true" version="1" id="oval:1:tst:28" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="oval:1:tst:29" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="oval:1:tst:30" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="oval:1:tst:31" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="oval:1:tst:32" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <!-- NONE_EXIST -->

    <runlevel_test check_existence="none_exist" comment="false" version="1" id="oval:1:tst:33" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="false" version="1" id="oval:1:tst:34" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="false" version="1" id="oval:1:tst:35" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="false" version="1" id="oval:1:tst:36" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="true" version="1" id="oval:1:tst:37" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="true" version="1" id="oval:1:tst:38" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:3"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="true" version="1" id="oval:1:tst:39" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:2"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="true" version="1" id="oval:1:tst:40" check="all" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:4"/>
      <state state_ref="oval:1:ste:1"/>
    </runlevel_test>

  </tests>

  <objects>

    <!-- EXISTING OBJECT (VALID SERVICE RUNNING ON A VALID RUNLEVEL -->
    <runlevel_object version="1" id="oval:1:obj:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <service_name>${1}</service_name>
      <runlevel>3</runlevel>
    </runlevel_object>

    <!-- NON-EXISTING OBJECT (VALID SERVICE NOT-RUNNING ON A VALID RUNLEVEL -->
    <runlevel_object version="1" id="oval:1:obj:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <service_name>${2}</service_name>
      <runlevel>3</runlevel>
    </runlevel_object>

    <!-- NON-EXISTING OBJECT (VALID SERVICE NOT-RUNNING ON AN IN-VALID RUNLEVEL -->
    <runlevel_object version="1" id="oval:1:obj:3" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <service_name>${2}</service_name>
      <runlevel>100000</runlevel>
    </runlevel_object>

    <!-- NON-EXISTING OBJECT (INVALID SERVICE NOT-RUNNING ON A VALID RUNLEVEL -->
    <runlevel_object version="1" id="oval:1:obj:4" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <service_name>invalid_service</service_name>
      <runlevel>3</runlevel>
    </runlevel_object>

  </objects>

  <states>

    <!-- TRUE STATE -->
    <runlevel_state version="1" id="oval:1:ste:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <start datatype="boolean">true</start>
    </runlevel_state>

    <!-- FALSE STATE -->
    <runlevel_state version="1" id="oval:1:ste:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <start datatype="boolean">false</start>
    </runlevel_state>

  </states>

</oval_definitions>
EOF
