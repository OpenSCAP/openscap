#!/usr/bin/env bash

echo "Test Probes: FILEHASH test" > /tmp/test_probes_filehash58.tmp

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

      <generator>
            <oval:product_name>filehash58</oval:product_name>
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
          <criterion test_ref="oval:1:tst:97"/>
          <criterion test_ref="oval:1:tst:98"/>
          <criterion test_ref="oval:1:tst:99"/>
          <criterion test_ref="oval:1:tst:100"/>
          <criterion test_ref="oval:1:tst:101"/>
          <criterion test_ref="oval:1:tst:102"/>
          <criterion test_ref="oval:1:tst:103"/>
          <criterion test_ref="oval:1:tst:104"/>
          <criterion test_ref="oval:1:tst:105"/>
          <criterion test_ref="oval:1:tst:106"/>
          <criterion test_ref="oval:1:tst:107"/>
          <criterion test_ref="oval:1:tst:108"/>
          <criterion test_ref="oval:1:tst:109"/>
          <criterion test_ref="oval:1:tst:110"/>
          <criterion test_ref="oval:1:tst:111"/>
          <criterion test_ref="oval:1:tst:112"/>
          <criterion test_ref="oval:1:tst:113"/>
          <criterion test_ref="oval:1:tst:114"/>
          <criterion test_ref="oval:1:tst:115"/>
          <criterion test_ref="oval:1:tst:116"/>
          <criterion test_ref="oval:1:tst:117"/>
          <criterion test_ref="oval:1:tst:118"/>
          <criterion test_ref="oval:1:tst:119"/>
          <criterion test_ref="oval:1:tst:120"/>
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
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:5"/>
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
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:1"/>
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
          <criterion test_ref="oval:1:tst:5"/>
          <criterion test_ref="oval:1:tst:5"/>
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
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:5"/>
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
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:1"/>
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
          <criterion test_ref="oval:1:tst:5"/>
          <criterion test_ref="oval:1:tst:5"/>
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
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:5"/>
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
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:1"/>
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
          <criterion test_ref="oval:1:tst:5"/>
          <criterion test_ref="oval:1:tst:5"/>
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
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:5"/>
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
          <criterion test_ref="oval:1:tst:1"/>
          <criterion test_ref="oval:1:tst:1"/>
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
          <criterion test_ref="oval:1:tst:5"/>
          <criterion test_ref="oval:1:tst:5"/>
        </criteria>
      </criteria>
    </definition>

  </definitions>

  <tests>

    <!-- DEFAULT STATELESS OBJECT -->

    <!-- check="all" -->

    <filehash58_test version="1" id="oval:1:tst:1" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:2" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:3" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:4" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check="all" -->

    <filehash58_test version="1" id="oval:1:tst:5" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:6" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:7" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:8" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- STATELESS OBJECT -->

    <!-- check_existence="all_exist" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:9" check_existence="all_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:10" check_existence="all_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:11" check_existence="all_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:12" check_existence="all_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:13" check_existence="at_least_one_exists" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:14" check_existence="at_least_one_exists" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:15" check_existence="at_least_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:16" check_existence="at_least_one_exists" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:17" check_existence="any_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:18" check_existence="any_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:19" check_existence="any_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:20" check_existence="any_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:21" check_existence="none_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:22" check_existence="none_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:23" check_existence="none_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:24" check_existence="none_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:25" check_existence="only_one_exists" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:26" check_existence="only_one_exists" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:27" check_existence="only_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:28" check_existence="only_one_exists" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:29" check_existence="all_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:30" check_existence="all_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:31" check_existence="all_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:32" check_existence="all_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:33" check_existence="at_least_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:34" check_existence="at_least_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:35" check_existence="at_least_one_exists" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:36" check_existence="at_least_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:37" check_existence="any_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:38" check_existence="any_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:39" check_existence="any_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:40" check_existence="any_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:41" check_existence="none_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:42" check_existence="none_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:43" check_existence="none_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:44" check_existence="none_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:45" check_existence="only_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:46" check_existence="only_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:47" check_existence="only_one_exists" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:48" check_existence="only_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
    </filehash58_test>

    <!-- DEFAULT OBJECT WITH STATE -->

    <!-- check="all" -->

    <filehash58_test version="1" id="oval:1:tst:49" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:50" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:51" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:52" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:53" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:54" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:55" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:56" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:57" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:58" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:59" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:60" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- OBJECT WITH STATE -->

    <!-- check_existence="all_exist" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:61" check_existence="all_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:62" check_existence="all_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:63" check_existence="all_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:64" check_existence="all_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:65" check_existence="all_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:66" check_existence="all_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:67" check_existence="all_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:68" check_existence="all_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:69" check_existence="all_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="all_exist" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:70" check_existence="all_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:71" check_existence="all_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:72" check_existence="all_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:73" check_existence="any_exist" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:74" check_existence="any_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:75" check_existence="any_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:76" check_existence="any_exist" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:77" check_existence="any_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:78" check_existence="any_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:79" check_existence="any_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:80" check_existence="any_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:81" check_existence="any_exist" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="any_exist" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:82" check_existence="any_exist" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:83" check_existence="any_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:84" check_existence="any_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:85" check_existence="at_least_one_exists" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:86" check_existence="at_least_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:87" check_existence="at_least_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:88" check_existence="at_least_one_exists" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:89" check_existence="at_least_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:90" check_existence="at_least_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:91" check_existence="at_least_one_exists" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:92" check_existence="at_least_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:93" check_existence="at_least_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="at_least_one_exists" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:94" check_existence="at_least_one_exists" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:95" check_existence="at_least_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:96" check_existence="at_least_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:97" check_existence="none_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:98" check_existence="none_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:99" check_existence="none_exist" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:100" check_existence="none_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:101" check_existence="none_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:102" check_existence="none_exist" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:103" check_existence="none_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:104" check_existence="none_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:105" check_existence="none_exist" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="none_exist" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:106" check_existence="none_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:107" check_existence="none_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:108" check_existence="none_exist" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="all" -->

    <filehash58_test version="1" id="oval:1:tst:109" check_existence="only_one_exists" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:110" check_existence="only_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:111" check_existence="only_one_exists" check="all" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="at least one" -->

    <filehash58_test version="1" id="oval:1:tst:112" check_existence="only_one_exists" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:113" check_existence="only_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:114" check_existence="only_one_exists" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="none satisfy" -->

    <filehash58_test version="1" id="oval:1:tst:115" check_existence="only_one_exists" check="none satisfy" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:116" check_existence="only_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:117" check_existence="only_one_exists" check="none satisfy" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

    <!-- check_existence="only_one_exists" check="only one" -->

    <filehash58_test version="1" id="oval:1:tst:118" check_existence="only_one_exists" check="only one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:119" check_existence="only_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:2"/>
    </filehash58_test>

    <filehash58_test version="1" id="oval:1:tst:120" check_existence="only_one_exists" check="only one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </filehash58_test>

  </tests>

  <objects>

    <!-- EXISTING OBJECT -->
    <filehash58_object version="1" id="oval:1:obj:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <path>/tmp</path>
      <filename>test_probes_filehash58.tmp</filename>
    </filehash58_object>

    <!-- NON-EXISTING OBJECT -->
    <filehash58_object version="1" id="oval:1:obj:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <path>/tmp</path>
      <filename>test_probes_filehash58.invalid</filename>
    </filehash58_object>

  </objects>

  <states>

    <!-- FULLY TRUE STATE -->
    <filehash58_state version="1" id="oval:1:ste:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <path>/tmp</path>
      <filename>test_probes_filehash58.tmp</filename>
      <md5>`md5sum /tmp/test_probes_filehash58.tmp | awk '{print $1}'`</md5>
      <sha1>`sha1sum /tmp/test_probes_filehash58.tmp | awk '{print $1}'`</sha1>
    </filehash58_state>

    <!-- FULLY FALSE STATE -->
    <filehash58_state version="1" id="oval:1:ste:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <path>/tmp</path>
      <filename>test_probes_filehash58.tmp</filename>
      <md5>incorrect-md5</md5>
      <sha1>incorrect-sha1</sha1>
    </filehash58_state>

    <!-- MIXED STATE -->
    <filehash58_state version="1" id="oval:1:ste:3" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <path>/tmp</path>
      <filename>test_probes_filehash58.tmp</filename>
      <md5>incorrect-md5</md5>
      <sha1>`sha1sum /tmp/test_probes_filehash58.tmp | awk '{print $1}'`</sha1>
    </filehash58_state>

  </states>

</oval_definitions>
EOF
