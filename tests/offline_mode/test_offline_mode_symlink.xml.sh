#!/bin/bash

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:unix="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">
  <generator>
    <oval:schema_version>5.11</oval:schema_version>
    <oval:timestamp>0001-01-01T00:00:00+00:00</oval:timestamp>
  </generator>

  <definitions>
    <definition class="compliance" version="1" id="oval:x:def:1">
      <metadata>
        <title>x</title>
        <description>x</description>
        <affected family="unix">
          <platform>x</platform>
        </affected>
      </metadata>
      <criteria>
          <criterion test_ref="oval:x:tst:1" comment="always pass"/>
          <criterion test_ref="oval:x:tst:2" comment="always pass"/>
          <criterion test_ref="oval:x:tst:3" comment="always pass"/>
          <criterion test_ref="oval:x:tst:4" comment="always pass"/>
          <criterion test_ref="oval:x:tst:5" comment="always pass"/>
          <criterion test_ref="oval:x:tst:6" comment="always pass"/>
          <criterion test_ref="oval:x:tst:7" comment="always pass"/>
          <criterion test_ref="oval:x:tst:8" comment="always pass"/>
          <criterion test_ref="oval:x:tst:9" comment="always pass"/>
          <criterion test_ref="oval:x:tst:10" comment="always pass"/>
          <criterion test_ref="oval:x:tst:11" comment="always pass"/>
          <criterion test_ref="oval:x:tst:12" comment="always pass"/>
      </criteria>
    </definition>
  </definitions>

  <tests>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:1" version="1">
      <unix:object object_ref="oval:1:obj:1"/>
      <unix:state state_ref="oval:1:ste:1"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:2" version="1">
      <unix:object object_ref="oval:1:obj:2"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:3" version="1">
      <unix:object object_ref="oval:1:obj:3"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:4" version="1">
      <unix:object object_ref="oval:1:obj:4"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:5" version="1">
      <unix:object object_ref="oval:1:obj:5"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:6" version="1">
      <unix:object object_ref="oval:1:obj:6"/>
      <unix:state state_ref="oval:1:ste:6"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:7" version="1">
      <unix:object object_ref="oval:1:obj:7"/>
      <unix:state state_ref="oval:1:ste:7"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:8" version="1">
      <unix:object object_ref="oval:1:obj:8"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:9" version="1">
      <unix:object object_ref="oval:1:obj:9"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:10" version="1">
      <unix:object object_ref="oval:1:obj:10"/>
      <unix:state state_ref="oval:1:ste:10"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:11" version="1">
      <unix:object object_ref="oval:1:obj:11"/>
      <unix:state state_ref="oval:1:ste:11"/>
    </unix:symlink_test>
    <unix:symlink_test check="all" check_existence="all_exist" comment="x" id="oval:x:tst:12" version="1">
      <unix:object object_ref="oval:1:obj:12"/>
    </unix:symlink_test>
  </tests>

  <objects>
    <unix:symlink_object id="oval:1:obj:1" version="1">
        <unix:filepath>$1/normal_symlink</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:2" version="1">
        <unix:filepath>$1/broken_symlink</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:3" version="1">
        <unix:filepath>/etc/does_not_exist</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:4" version="1">
        <unix:filepath>$1/some_file</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:5" version="1">
        <unix:filepath>$1/circular_symlink</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:6" version="1">
        <unix:filepath>$1/chained_symlink</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:7" version="1">
        <unix:filepath>/etc/passwd</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:8" version="1">
        <unix:filepath>/normal_symlink_relative</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:9" version="1">
        <unix:filepath>/broken_symlink_relative</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:10" version="1">
        <unix:filepath>/path_traversal_symlink</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:11" version="1">
        <unix:filepath>/symlink_with_dot_in_path</unix:filepath>
    </unix:symlink_object>
    <unix:symlink_object id="oval:1:obj:12" version="1">
        <unix:filepath>/tmp/symlinktest</unix:filepath>
    </unix:symlink_object>
  </objects>

  <states>
    <unix:symlink_state id="oval:1:ste:1" version="1">
        <unix:filepath>$1/normal_symlink</unix:filepath>
        <unix:canonical_path>$1/some_file</unix:canonical_path>
    </unix:symlink_state>
    <unix:symlink_state id="oval:1:ste:6" version="1">
        <unix:filepath>$1/chained_symlink</unix:filepath>
        <unix:canonical_path>$1/other_file</unix:canonical_path>
    </unix:symlink_state>
    <unix:symlink_state id="oval:1:ste:7" version="1">
        <unix:filepath>$1/etc/passwd</unix:filepath>
        <unix:canonical_path>$1/etc/shadow</unix:canonical_path>
    </unix:symlink_state>
    <unix:symlink_state id="oval:1:ste:10" version="1">
        <unix:filepath>$1/path_traversal_symlink</unix:filepath>
        <unix:canonical_path>$1/file_in_root</unix:canonical_path>
    </unix:symlink_state>
    <unix:symlink_state id="oval:1:ste:11" version="1">
        <unix:filepath>$1/symlink_with_dot_in_path</unix:filepath>
        <unix:canonical_path>$1/some_file</unix:canonical_path>
    </unix:symlink_state>
  </states>

</oval_definitions>
EOF
