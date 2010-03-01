#!/usr/bin/env bash

RPM_A_NAME=$1
RPM_B_NAME=$2

RPM_A_ARCH=`rpm --qf "%{ARCH}" -q $RPM_A_NAME`
RPM_B_ARCH=`rpm --qf "%{ARCH}" -q $RPM_B_NAME`

RPM_A_VERSION=`rpm --qf "%{VERSION}" -q $RPM_A_NAME`
RPM_B_VERSION=`rpm --qf "%{VERSION}" -q $RPM_B_NAME`

RPM_A_RELEASE=`rpm --qf "%{RELEASE}" -q $RPM_A_NAME`
RPM_B_RELEASE=`rpm --qf "%{RELEASE}" -q $RPM_B_NAME`

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

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
          <criterion test_ref="oval:1:tst:121"/>
          <criterion test_ref="oval:1:tst:122"/>
          <criterion test_ref="oval:1:tst:123"/>
          <criterion test_ref="oval:1:tst:124"/>
          <criterion test_ref="oval:1:tst:125"/>
          <criterion test_ref="oval:1:tst:126"/>
          <criterion test_ref="oval:1:tst:127"/>
          <criterion test_ref="oval:1:tst:128"/>
          <criterion test_ref="oval:1:tst:129"/>
          <criterion test_ref="oval:1:tst:130"/>
          <criterion test_ref="oval:1:tst:131"/>
          <criterion test_ref="oval:1:tst:132"/>
          <criterion test_ref="oval:1:tst:133"/>
          <criterion test_ref="oval:1:tst:134"/>
          <criterion test_ref="oval:1:tst:135"/>
          <criterion test_ref="oval:1:tst:136"/>
          <criterion test_ref="oval:1:tst:137"/>
          <criterion test_ref="oval:1:tst:138"/>
          <criterion test_ref="oval:1:tst:139"/>
          <criterion test_ref="oval:1:tst:140"/>
          <criterion test_ref="oval:1:tst:141"/>
          <criterion test_ref="oval:1:tst:142"/>
          <criterion test_ref="oval:1:tst:143"/>
          <criterion test_ref="oval:1:tst:144"/>
          <criterion test_ref="oval:1:tst:145"/>
          <criterion test_ref="oval:1:tst:146"/>
          <criterion test_ref="oval:1:tst:147"/>
          <criterion test_ref="oval:1:tst:148"/>
          <criterion test_ref="oval:1:tst:149"/>
          <criterion test_ref="oval:1:tst:150"/>
          <criterion test_ref="oval:1:tst:151"/>
          <criterion test_ref="oval:1:tst:152"/>
          <criterion test_ref="oval:1:tst:153"/>
          <criterion test_ref="oval:1:tst:154"/>
          <criterion test_ref="oval:1:tst:155"/>
          <criterion test_ref="oval:1:tst:156"/>
          <criterion test_ref="oval:1:tst:157"/>
          <criterion test_ref="oval:1:tst:158"/>
          <criterion test_ref="oval:1:tst:159"/>
          <criterion test_ref="oval:1:tst:160"/>
          <criterion test_ref="oval:1:tst:161"/>
          <criterion test_ref="oval:1:tst:162"/>
          <criterion test_ref="oval:1:tst:163"/>
          <criterion test_ref="oval:1:tst:164"/>
          <criterion test_ref="oval:1:tst:165"/>
          <criterion test_ref="oval:1:tst:166"/>
          <criterion test_ref="oval:1:tst:167"/>
          <criterion test_ref="oval:1:tst:168"/>
          <criterion test_ref="oval:1:tst:169"/>
          <criterion test_ref="oval:1:tst:170"/>
          <criterion test_ref="oval:1:tst:171"/>
          <criterion test_ref="oval:1:tst:172"/>
          <criterion test_ref="oval:1:tst:173"/>
          <criterion test_ref="oval:1:tst:174"/>
          <criterion test_ref="oval:1:tst:175"/>
          <criterion test_ref="oval:1:tst:176"/>
          <criterion test_ref="oval:1:tst:177"/>
          <criterion test_ref="oval:1:tst:178"/>
          <criterion test_ref="oval:1:tst:179"/>
          <criterion test_ref="oval:1:tst:180"/>
          <criterion test_ref="oval:1:tst:181"/>
          <criterion test_ref="oval:1:tst:182"/>
          <criterion test_ref="oval:1:tst:183"/>
          <criterion test_ref="oval:1:tst:184"/>
          <criterion test_ref="oval:1:tst:185"/>
          <criterion test_ref="oval:1:tst:186"/>
          <criterion test_ref="oval:1:tst:187"/>
          <criterion test_ref="oval:1:tst:188"/>
          <criterion test_ref="oval:1:tst:189"/>
          <criterion test_ref="oval:1:tst:190"/>
          <criterion test_ref="oval:1:tst:191"/>
          <criterion test_ref="oval:1:tst:192"/>
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
          <criterion test_ref="oval:1:tst:3"/>
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
          <criterion test_ref="oval:1:tst:3"/>
          <criterion test_ref="oval:1:tst:3"/>
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
          <criterion test_ref="oval:1:tst:3"/>
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
          <criterion test_ref="oval:1:tst:3"/>
          <criterion test_ref="oval:1:tst:3"/>
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
          <criterion test_ref="oval:1:tst:3"/>
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
          <criterion test_ref="oval:1:tst:3"/>
          <criterion test_ref="oval:1:tst:3"/>
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
          <criterion test_ref="oval:1:tst:3"/>
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
          <criterion test_ref="oval:1:tst:3"/>
          <criterion test_ref="oval:1:tst:3"/>
        </criteria>
      </criteria>
    </definition>

  </definitions>

  <tests>

    <!-- DEFAULT STATELESS OBJECT -->

    <!-- check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:1" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:2" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:3" check="all" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:4" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:5" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:6" check="at least one" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:7" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:8" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:9" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:10" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:11" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:12" check="only one" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- STATELESS OBJECT -->

    <!-- check_existence="all_exist" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:13" check_existence="all_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:14" check_existence="all_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:15" check_existence="all_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="all_exist" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:16" check_existence="all_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:17" check_existence="all_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:18" check_existence="all_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="all_exist" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:19" check_existence="all_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:20" check_existence="all_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:21" check_existence="all_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="all_exist" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:22" check_existence="all_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:23" check_existence="all_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:24" check_existence="all_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="at_least_one_exists" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:25" check_existence="at_least_one_exists" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:26" check_existence="at_least_one_exists" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:27" check_existence="at_least_one_exists" check="all" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="at_least_one_exists" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:28" check_existence="at_least_one_exists" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:29" check_existence="at_least_one_exists" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:30" check_existence="at_least_one_exists" check="at least one" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="at_least_one_exists" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:31" check_existence="at_least_one_exists" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:32" check_existence="at_least_one_exists" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:33" check_existence="at_least_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="at_least_one_exists" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:34" check_existence="at_least_one_exists" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:35" check_existence="at_least_one_exists" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:36" check_existence="at_least_one_exists" check="only one" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="any_exist" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:37" check_existence="any_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:38" check_existence="any_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:39" check_existence="any_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="any_exist" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:40" check_existence="any_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:41" check_existence="any_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:42" check_existence="any_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="any_exist" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:43" check_existence="any_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:44" check_existence="any_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:45" check_existence="any_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="any_exist" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:46" check_existence="any_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:47" check_existence="any_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:48" check_existence="any_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="none_exist" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:49" check_existence="none_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:50" check_existence="none_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:51" check_existence="none_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="none_exist" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:52" check_existence="none_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:53" check_existence="none_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:54" check_existence="none_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="none_exist" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:55" check_existence="none_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:56" check_existence="none_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:57" check_existence="none_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="none_exist" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:58" check_existence="none_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:59" check_existence="none_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:60" check_existence="none_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="only_one_exists" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:61" check_existence="only_one_exists" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:62" check_existence="only_one_exists" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:63" check_existence="only_one_exists" check="all" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="only_one_exists" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:64" check_existence="only_one_exists" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:65" check_existence="only_one_exists" check="at least one" comment="false">
      <object object_ref="oval:1:obj:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:66" check_existence="only_one_exists" check="at least one" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="only_one_exists" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:67" check_existence="only_one_exists" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:68" check_existence="only_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:69" check_existence="only_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- check_existence="only_one_exists" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:70" check_existence="only_one_exists" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:71" check_existence="only_one_exists" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:72" check_existence="only_one_exists" check="only one" comment="false">
      <object object_ref="oval:1:obj:3"/>
    </rpminfo_test>

    <!-- DEFAULT OBJECT WITH STATE -->

    <!-- check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:73" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:74" check="all" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:75" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:76" check="all" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:77" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:78" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:79" check="at least one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:80" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:81" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:82" check="at least one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:83" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:84" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:85" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:86" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:87" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:88" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:89" check="only one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:90" check="only one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:91" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:92" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- OBJECT WITH STATE -->

    <!-- check_existence="all_exist" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:93" check_existence="all_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:94" check_existence="all_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:95" check_existence="all_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:96" check_existence="all_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:97" check_existence="all_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="all_exist" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:98" check_existence="all_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:99" check_existence="all_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:100" check_existence="all_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:101" check_existence="all_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:102"  check_existence="all_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="all_exist" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:103" check_existence="all_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:104" check_existence="all_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:105" check_existence="all_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:106" check_existence="all_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:107" check_existence="all_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="all_exist" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:108" check_existence="all_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:109" check_existence="all_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:110" check_existence="all_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:111" check_existence="all_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:112" check_existence="all_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="any_exist" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:113" check_existence="any_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:114" check_existence="any_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:115" check_existence="any_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:116" check_existence="any_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:117" check_existence="any_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="any_exist" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:118" check_existence="any_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:119" check_existence="any_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:120" check_existence="any_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:121" check_existence="any_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:122"  check_existence="any_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="any_exist" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:123" check_existence="any_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:124" check_existence="any_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:125" check_existence="any_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:126" check_existence="any_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:127" check_existence="any_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="any_exist" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:128" check_existence="any_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:129" check_existence="any_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:130" check_existence="any_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:131" check_existence="any_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:132" check_existence="any_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="at_least_one_exists" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:133" check_existence="at_least_one_exists" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:134" check_existence="at_least_one_exists" check="all" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:135" check_existence="at_least_one_exists" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:136" check_existence="at_least_one_exists" check="all" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:137" check_existence="at_least_one_exists" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="at_least_one_exists" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:138" check_existence="at_least_one_exists" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:139" check_existence="at_least_one_exists" check="at least one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:140" check_existence="at_least_one_exists" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:141" check_existence="at_least_one_exists" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:142"  check_existence="at_least_one_exists" check="at least one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="at_least_one_exists" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:143" check_existence="at_least_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:144" check_existence="at_least_one_exists" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:145" check_existence="at_least_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:146" check_existence="at_least_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:147" check_existence="at_least_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="at_least_one_exists" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:148" check_existence="at_least_one_exists" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:149" check_existence="at_least_one_exists" check="only one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:150" check_existence="at_least_one_exists" check="only one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:151" check_existence="at_least_one_exists" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:152" check_existence="at_least_one_exists" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="none_exist" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:153" check_existence="none_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:154" check_existence="none_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:155" check_existence="none_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:156" check_existence="none_exist" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:157" check_existence="none_exist" check="all" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="none_exist" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:158" check_existence="none_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:159" check_existence="none_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:160" check_existence="none_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:161" check_existence="none_exist" check="at least one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:162"  check_existence="none_exist" check="at least one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="none_exist" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:163" check_existence="none_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:164" check_existence="none_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:165" check_existence="none_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:166" check_existence="none_exist" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:167" check_existence="none_exist" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="none_exist" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:168" check_existence="none_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:169" check_existence="none_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:170" check_existence="none_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:171" check_existence="none_exist" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:172" check_existence="none_exist" check="only one" comment="true">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="only_one_exists" check="all" -->

    <rpminfo_test version="1" id="oval:1:tst:173" check_existence="only_one_exists" check="all" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:174" check_existence="only_one_exists" check="all" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:175" check_existence="only_one_exists" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:176" check_existence="only_one_exists" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:177" check_existence="only_one_exists" check="all" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="only_one_exists" check="at least one" -->

    <rpminfo_test version="1" id="oval:1:tst:178" check_existence="only_one_exists" check="at least one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:179" check_existence="only_one_exists" check="at least one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:180" check_existence="only_one_exists" check="at least one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:181" check_existence="only_one_exists" check="at least one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:182"  check_existence="only_one_exists" check="at least one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="only_one_exists" check="none satisfy" -->

    <rpminfo_test version="1" id="oval:1:tst:183" check_existence="only_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:184" check_existence="only_one_exists" check="none satisfy" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:185" check_existence="only_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:186" check_existence="only_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:187" check_existence="only_one_exists" check="none satisfy" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <!-- check_existence="only_one_exists" check="only one" -->

    <rpminfo_test version="1" id="oval:1:tst:188" check_existence="only_one_exists" check="only one" comment="true">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:189" check_existence="only_one_exists" check="only one" comment="false">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:190" check_existence="only_one_exists" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:1"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:191" check_existence="only_one_exists" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:2"/>
    </rpminfo_test>

    <rpminfo_test version="1" id="oval:1:tst:192" check_existence="only_one_exists" check="only one" comment="false">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:3"/>
    </rpminfo_test>

  </tests>

  <objects>

    <!-- OBJECT WITH ONE ITEM -->
    <rpminfo_object version="1" id="oval:1:obj:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name>${RPM_A_NAME}</name>
    </rpminfo_object>

    <!-- OBJECT WITH TWO ITEMS -->
    <rpminfo_object version="1" id="oval:1:obj:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name operation="pattern match">^(${RPM_A_NAME}|${RPM_B_NAME})$</name>
    </rpminfo_object>

    <!-- OBJECT WITH NO ITEM -->
    <rpminfo_object version="1" id="oval:1:obj:3" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name>invalid_name</name>
    </rpminfo_object>

  </objects>

  <states>

    <!-- TRUE STATE -->
    <rpminfo_state version="1" id="oval:1:ste:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name>${RPM_A_NAME}</name>
      <arch>${RPM_A_ARCH}</arch>
      <release>${RPM_A_RELEASE}</release>
      <version>${RPM_A_VERSION}</version>
    </rpminfo_state>

    <!-- TRUE STATE -->
    <rpminfo_state version="1" id="oval:1:ste:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name operation="pattern match">^(${RPM_A_NAME}|${RPM_B_NAME})$</name>
      <arch operation="pattern match">^(${RPM_A_ARCH}|${RPM_B_ARCH})$</arch>
    </rpminfo_state>

    <!-- FALSE STATE -->
    <rpminfo_state version="1" id="oval:1:ste:3" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name>invalid</name>
      <version>0.0.0.0.0</version>
    </rpminfo_state>

  </states>

</oval_definitions>
EOF
