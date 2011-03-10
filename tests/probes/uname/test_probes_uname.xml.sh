#!/usr/bin/env bash

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

      <generator>
            <oval:product_name>uname</oval:product_name>
            <oval:product_version>1.0</oval:product_version>
            <oval:schema_version>5.4</oval:schema_version>
            <oval:timestamp>2008-03-31T00:00:00-00:00</oval:timestamp>
      </generator>

  <definitions>

    <definition class="compliance" version="1" id="oval:1:def:1">  <!-- comment="false" -->
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
          <criterion test_ref="oval:1:tst:41"/>
          <criterion test_ref="oval:1:tst:42"/>
          <criterion test_ref="oval:1:tst:43"/>
          <criterion test_ref="oval:1:tst:44"/>
          <criterion test_ref="oval:1:tst:45"/>
          <criterion test_ref="oval:1:tst:46"/>
          <criterion test_ref="oval:1:tst:47"/>
          <criterion test_ref="oval:1:tst:48"/>
          <criterion test_ref="oval:1:tst:49"/>
          <criterion test_ref="oval:1:tst:50"/>
          <criterion test_ref="oval:1:tst:51"/>
          <criterion test_ref="oval:1:tst:52"/>
          <criterion test_ref="oval:1:tst:53"/>
          <criterion test_ref="oval:1:tst:54"/>
          <criterion test_ref="oval:1:tst:55"/>
          <criterion test_ref="oval:1:tst:56"/>
          <criterion test_ref="oval:1:tst:57"/>
          <criterion test_ref="oval:1:tst:58"/>
          <criterion test_ref="oval:1:tst:59"/>
          <criterion test_ref="oval:1:tst:60"/>
          <criterion test_ref="oval:1:tst:61"/>
          <criterion test_ref="oval:1:tst:62"/>
          <criterion test_ref="oval:1:tst:63"/>
          <criterion test_ref="oval:1:tst:64"/>
          <criterion test_ref="oval:1:tst:65"/>
          <criterion test_ref="oval:1:tst:66"/>
          <criterion test_ref="oval:1:tst:67"/>
          <criterion test_ref="oval:1:tst:68"/>
          <criterion test_ref="oval:1:tst:69"/>
          <criterion test_ref="oval:1:tst:70"/>
          <criterion test_ref="oval:1:tst:71"/>
          <criterion test_ref="oval:1:tst:72"/>
          <criterion test_ref="oval:1:tst:73"/>
          <criterion test_ref="oval:1:tst:74"/>
          <criterion test_ref="oval:1:tst:75"/>
          <criterion test_ref="oval:1:tst:76"/>
          <criterion test_ref="oval:1:tst:77"/>
          <criterion test_ref="oval:1:tst:78"/>
          <criterion test_ref="oval:1:tst:79"/>
          <criterion test_ref="oval:1:tst:80"/>
          <criterion test_ref="oval:1:tst:81"/>
          <criterion test_ref="oval:1:tst:82"/>
          <criterion test_ref="oval:1:tst:83"/>
          <criterion test_ref="oval:1:tst:84"/>
          <criterion test_ref="oval:1:tst:85"/>
          <criterion test_ref="oval:1:tst:86"/>
          <criterion test_ref="oval:1:tst:87"/>
          <criterion test_ref="oval:1:tst:88"/>
          <criterion test_ref="oval:1:tst:89"/>
          <criterion test_ref="oval:1:tst:90"/>
          <criterion test_ref="oval:1:tst:91"/>
          <criterion test_ref="oval:1:tst:92"/>
          <criterion test_ref="oval:1:tst:93"/>
          <criterion test_ref="oval:1:tst:94"/>
          <criterion test_ref="oval:1:tst:95"/>
          <criterion test_ref="oval:1:tst:96"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:2">  <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:26"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:3">  <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:25"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:4">  <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="oval:1:tst:26"/>
          <criterion test_ref="oval:1:tst:27"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:5">  <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:26"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:6">  <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:25"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:7">  <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="OR">
          <criterion test_ref="oval:1:tst:26"/>
          <criterion test_ref="oval:1:tst:27"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:8">  <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:26"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:9">  <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:25"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:10">  <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="XOR">
          <criterion test_ref="oval:1:tst:26"/>
          <criterion test_ref="oval:1:tst:27"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:11">  <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:26"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:12">  <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="oval:1:tst:25"/>
          <criterion test_ref="oval:1:tst:25"/>
        </criteria>
      </criteria>
    </definition>

    <definition class="compliance" version="1" id="oval:1:def:13">  <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="ONE">
          <criterion test_ref="oval:1:tst:26"/>
          <criterion test_ref="oval:1:tst:27"/>
        </criteria>
      </criteria>
    </definition>

  </definitions>

  <tests>

    <!-- DEFAULT STATELESS OBJECT -->

    <!-- check="all" -->

    <uname_test version="1" id="oval:1:tst:1" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check="at least one" -->

    <uname_test version="1" id="oval:1:tst:2" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:3" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check="only one" -->

    <uname_test version="1" id="oval:1:tst:4" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- STATELESS OBJECT -->

    <!-- check_existence="all_exist" check="all" -->

    <uname_test version="1" id="oval:1:tst:5" check_existence="all_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="all_exist" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:6" check_existence="all_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="all_exist" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:7" check_existence="all_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="all_exist" check="only one" -->

    <uname_test version="1" id="oval:1:tst:8" check_existence="all_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="at_least_one_exists" check="all" -->

    <uname_test version="1" id="oval:1:tst:9" check_existence="at_least_one_exists" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="at_least_one_exists" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:10" check_existence="at_least_one_exists" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="at_least_one_exists" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:11" check_existence="at_least_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="at_least_one_exists" check="only one" -->

    <uname_test version="1" id="oval:1:tst:12" check_existence="at_least_one_exists" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="any_exist" check="all" -->

    <uname_test version="1" id="oval:1:tst:13" check_existence="any_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="any_exist" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:14" check_existence="any_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="any_exist" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:15" check_existence="any_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="any_exist" check="only one" -->

    <uname_test version="1" id="oval:1:tst:16" check_existence="any_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="none_exist" check="all" -->

    <uname_test version="1" id="oval:1:tst:17" check_existence="none_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="none_exist" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:18" check_existence="none_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="none_exist" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:19" check_existence="none_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="none_exist" check="only one" -->

    <uname_test version="1" id="oval:1:tst:20" check_existence="none_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="only_one_exists" check="all" -->

    <uname_test version="1" id="oval:1:tst:21" check_existence="only_one_exists" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="only_one_exists" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:22" check_existence="only_one_exists" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="only_one_exists" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:23" check_existence="only_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- check_existence="only_one_exists" check="only one" -->

    <uname_test version="1" id="oval:1:tst:24" check_existence="only_one_exists" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
    </uname_test>

    <!-- DEFAULT OBJECT WITH STATE -->

    <!-- check="all" -->

    <uname_test version="1" id="oval:1:tst:25" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:26" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:27" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check="at least one" -->

    <uname_test version="1" id="oval:1:tst:28" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:29" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:30" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:31" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:32" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:33" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check="only one" -->

    <uname_test version="1" id="oval:1:tst:34" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:35" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:36" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- OBJECT WITH STATE -->

    <!-- check_existence="all_exist" check="all" -->

    <uname_test version="1" id="oval:1:tst:37" check_existence="all_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:38" check_existence="all_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:39" check_existence="all_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="all_exist" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:40" check_existence="all_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:41" check_existence="all_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:42" check_existence="all_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="all_exist" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:43" check_existence="all_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:44" check_existence="all_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:45" check_existence="all_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="all_exist" check="only one" -->

    <uname_test version="1" id="oval:1:tst:46" check_existence="all_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:47" check_existence="all_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:48" check_existence="all_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="any_exist" check="all" -->

    <uname_test version="1" id="oval:1:tst:49" check_existence="any_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:50" check_existence="any_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:51" check_existence="any_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="any_exist" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:52" check_existence="any_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:53" check_existence="any_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:54" check_existence="any_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="any_exist" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:55" check_existence="any_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:56" check_existence="any_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:57" check_existence="any_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="any_exist" check="only one" -->

    <uname_test version="1" id="oval:1:tst:58" check_existence="any_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:59" check_existence="any_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:60" check_existence="any_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="at_least_one_exists" check="all" -->

    <uname_test version="1" id="oval:1:tst:61" check_existence="at_least_one_exists" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:62" check_existence="at_least_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:63" check_existence="at_least_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="at_least_one_exists" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:64" check_existence="at_least_one_exists" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:65" check_existence="at_least_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:66" check_existence="at_least_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="at_least_one_exists" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:67" check_existence="at_least_one_exists" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:68" check_existence="at_least_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:69" check_existence="at_least_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="at_least_one_exists" check="only one" -->

    <uname_test version="1" id="oval:1:tst:70" check_existence="at_least_one_exists" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:71" check_existence="at_least_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:72" check_existence="at_least_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="none_exist" check="all" -->

    <uname_test version="1" id="oval:1:tst:73" check_existence="none_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:74" check_existence="none_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:75" check_existence="none_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="none_exist" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:76" check_existence="none_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:77" check_existence="none_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:78" check_existence="none_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="none_exist" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:79" check_existence="none_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:80" check_existence="none_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:81" check_existence="none_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="none_exist" check="only one" -->

    <uname_test version="1" id="oval:1:tst:82" check_existence="none_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:83" check_existence="none_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:84" check_existence="none_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="only_one_exists" check="all" -->

    <uname_test version="1" id="oval:1:tst:85" check_existence="only_one_exists" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:86" check_existence="only_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:87" check_existence="only_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="only_one_exists" check="at least one" -->

    <uname_test version="1" id="oval:1:tst:88" check_existence="only_one_exists" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:89" check_existence="only_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:90" check_existence="only_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="only_one_exists" check="none satisfy" -->

    <uname_test version="1" id="oval:1:tst:91" check_existence="only_one_exists" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:92" check_existence="only_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:93" check_existence="only_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

    <!-- check_existence="only_one_exists" check="only one" -->

    <uname_test version="1" id="oval:1:tst:94" check_existence="only_one_exists" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:95" check_existence="only_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </uname_test>

    <uname_test version="1" id="oval:1:tst:96" check_existence="only_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </uname_test>

  </tests>

  <objects>

    <!-- OBJECT -->
    <uname_object version="1" id="oval:1:obj:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix"/>

  </objects>

  <states>

    <!-- FULLY TRUE STATE -->
    <uname_state version="1" id="oval:1:ste:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <machine_class>`uname -m`</machine_class>
      <node_name>`uname -n`</node_name>
      <os_name>`uname -s`</os_name>
      <os_release>`uname -r`</os_release>
      <os_version>`uname -v`</os_version>
      <processor_type>`uname -p`</processor_type>
    </uname_state>

    <!-- FULLY FALSE STATE -->
    <uname_state version="1" id="oval:1:ste:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <machine_class>X`uname -m`</machine_class>
      <node_name>X`uname -n`</node_name>
      <os_name>X`uname -s`</os_name>
      <os_release>X`uname -r`</os_release>
      <os_version>X`uname -v`</os_version>
      <processor_type>X`uname -p`</processor_type>
    </uname_state>

    <!-- MIXED STATE :-) -->
    <uname_state version="1" id="oval:1:ste:3" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <machine_class>X`uname -m`</machine_class>
      <node_name>X`uname -n`</node_name>
      <os_name>`uname -s`</os_name>
      <os_release>`uname -r`</os_release>
      <os_version>`uname -v`</os_version>
      <processor_type>X`uname -p`</processor_type>
    </uname_state>

  </states>

</oval_definitions>
EOF
