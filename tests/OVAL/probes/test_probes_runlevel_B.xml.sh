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
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:2" comment="false">
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

   <definition class="compliance" version="1" id="definition:3" comment="true">
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:4" comment="false">
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="test:2"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:5" comment="true">
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:6" comment="true">
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:7" comment="false">
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="test:2"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:8" comment="true">
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:9" comment="false">
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:10" comment="false">
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="test:2"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:11" comment="true">
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:2"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:12" comment="false">
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="test:1"/>
          <criterion test_ref="test:1"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="definition:13" comment="false">
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

    <runlevel_test check_existence="all_exist" comment="true" version="1" id="test:1" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="test:2" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="test:3" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="true" version="1" id="test:4" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="test:5" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="test:6" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="false" version="1" id="test:7" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="all_exist" comment="true" version="1" id="test:8" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <!-- ANY_EXIST -->

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="test:9" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="false" version="1" id="test:10" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="false" version="1" id="test:11" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="test:12" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="test:13" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="test:14" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="test:15" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="any_exist" comment="true" version="1" id="test:16" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <!-- AT_LEAST_ONE_EXISTS -->

    <runlevel_test check_existence="at_least_one_exists" comment="true" version="1" id="test:17" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="test:18" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="test:19" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="true" version="1" id="test:20" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="test:21" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="test:22" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="false" version="1" id="test:23" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="at_least_one_exists" comment="true" version="1" id="test:24" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <!-- ONLY_ONE_EXISTS -->

    <runlevel_test check_existence="only_one_exists" comment="true" version="1" id="test:25" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="test:26" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="test:27" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="true" version="1" id="test:28" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="test:29" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="test:30" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="false" version="1" id="test:31" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="only_one_exists" comment="true" version="1" id="test:32" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <!-- NONE_EXIST -->

    <runlevel_test check_existence="none_exist" comment="false" version="1" id="test:33" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="false" version="1" id="test:34" check="all">
      <object object_ref="object:1"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="false" version="1" id="test:35" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="false" version="1" id="test:36" check="all">
      <object object_ref="object:2"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="true" version="1" id="test:37" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="true" version="1" id="test:38" check="all">
      <object object_ref="object:3"/>
      <state state_ref="state:false"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="true" version="1" id="test:39" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:true"/>
    </runlevel_test>

    <runlevel_test check_existence="none_exist" comment="false" version="1" id="test:40" check="all">
      <object object_ref="object:4"/>
      <state state_ref="state:false"/>
    </runlevel_test>

  </tests>

  <objects>

    <!-- EXISTING OBJECT (VALID SERVICE RUNNING ON A VALID RUNLEVEL -->
    <runlevel_object version="1" id="object:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <service_name>${1}</service_name>
      <runlevel>3</runlevel>
    </runlevel_object>

    <!-- NON-EXISTING OBJECT (VALID SERVICE NOT-RUNNING ON A VALID RUNLEVEL -->
    <runlevel_object version="1" id="object:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <service_name>${2}</service_name>
      <runlevel>3</runlevel>
    </runlevel_object>

    <!-- NON-EXISTING OBJECT (VALID SERVICE NOT-RUNNING ON AN IN-VALID RUNLEVEL -->
    <runlevel_object version="1" id="object:3" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <service_name>${2}</service_name>
      <runlevel>100000</runlevel>
    </runlevel_object>

    <!-- NON-EXISTING OBJECT (INVALID SERVICE NOT-RUNNING ON A VALID RUNLEVEL -->
    <runlevel_object version="1" id="object:4" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <service_name>invalid_service</service_name>
      <runlevel>3</runlevel>
    </runlevel_object>

  </objects>

  <states>

    <!-- TRUE STATE -->
    <runlevel_state version="1" id="state:true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <start datatype="boolean">true</start>
    </runlevel_state>

    <!-- FALSE STATE -->
    <runlevel_state version="1" id="state:false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <start datatype="boolean">false</start>
    </runlevel_state>

  </states>

</oval_definitions>
EOF
