#!/usr/bin/env bash

BOOLEANS=(`getsebool -a | cut -d " " -f 1 | head -n 3`)

I=0
while [ $I -lt ${#BOOLEANS[@]} ]; do
    STATE=`getsebool ${BOOLEANS[$I]} | cut -d " " -f 3`
    if [ $STATE = "on" ]; then
        BOOLEAN_VAR_N=1
        BOOLEAN_VAR_S="true"
    else
        BOOLEAN_VAR_N=0
        BOOLEAN_VAR_S="false"
    fi 
    eval "BOOLEANS_VAL_N[$I]=$BOOLEAN_VAR_N"
    eval "BOOLEANS_VAL_S[$I]=$BOOLEAN_VAR_S"
    I=$[$I+1]
done

C=$I

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

  <generator>
    <oval:product_name>selinuxboolean</oval:product_name>
    <oval:product_version>1.0</oval:product_version>
    <oval:schema_version>5.8</oval:schema_version>
    <oval:timestamp>2011-07-13T00:00:00-00:00</oval:timestamp>
  </generator>

  <definitions>

    <definition class="compliance" version="1" id="oval:1:def:1">  <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
        <criteria operator="AND">
EOF
I=0
while [ $I -lt ${#BOOLEANS[@]} ]; do
    echo "<criterion test_ref=\"oval:1:tst:$[$I+1]\"/>"
    echo "<criterion test_ref=\"oval:1:tst:$[$C+$I+1]\"/>"
    I=$[$I+1]
done
I=0
while [ $I -lt $(( ${#BOOLEANS[@]} - 1 )) ]; do
    echo "<criterion test_ref=\"oval:1:tst:$[$C+$C+$I+1]\"/>"
    I=$[$I+1]
done
cat <<EOF
        </criteria>
      </criteria>
    </definition>

  </definitions>

  <tests>

    <!-- DEFAULT STATELESS OBJECT -->

    <!-- check="all" -->

EOF

I=0
while [ $I -lt ${#BOOLEANS[@]} ]; do
    cat <<EOF
    <selinuxboolean_test version="1" id="oval:1:tst:$[$I+1]" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:1:obj:$[$I+1]"/>
      <state state_ref="oval:1:ste:$[$I+1]"/>
    </selinuxboolean_test>

    <selinuxboolean_test version="1" id="oval:1:tst:$[$C+$I+1]" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:1:obj:$[$C+1]"/>
      <state state_ref="oval:1:ste:$[$C+$I+1]"/>
    </selinuxboolean_test>
EOF
    I=$[$I+1]
done

I=0
while [ $I -lt $(( ${#BOOLEANS[@]} - 1 )) ]; do
    cat <<EOF
<selinuxboolean_test version="1" id="oval:1:tst:$[$C+$C+$I+1]" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:1:obj:$[$C+2]"/>
      <state state_ref="oval:1:ste:$[$C+$C+$I+1]"/>
    </selinuxboolean_test>
EOF
    I=$[$I+1]
done

    cat <<EOF

  </tests>

  <objects>

EOF

I=0
while [ $I -lt ${#BOOLEANS[@]} ]; do
    cat <<EOF
    <selinuxboolean_object version="1" id="oval:1:obj:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name>${BOOLEANS[$I]}</name>
    </selinuxboolean_object>

EOF
    I=$[$I+1]
done

cat <<EOF
    <selinuxboolean_object version="1" id="oval:1:obj:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name operation="pattern match">.*</name>
    </selinuxboolean_object>

    <selinuxboolean_object version="1" id="oval:1:obj:$[$I+2]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <name operation="not equal">${BOOLEANS[$I-1]}</name>
    </selinuxboolean_object>

EOF
    cat <<EOF

  </objects>

  <states>

EOF

I=0
while [ $I -lt ${#BOOLEANS[@]} ]; do
    cat <<EOF
  <selinuxboolean_state version="1" id="oval:1:ste:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
    <name>${BOOLEANS[$I]}</name>
    <current_status datatype="boolean">${BOOLEANS_VAL_N[$I]}</current_status>
    <pending_status datatype="boolean">${BOOLEANS_VAL_N[$I]}</pending_status>
  </selinuxboolean_state>

  <selinuxboolean_state version="1" id="oval:1:ste:$[$C+$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
    <name>${BOOLEANS[$I]}</name>
    <current_status datatype="boolean">${BOOLEANS_VAL_S[$I]}</current_status>
    <pending_status datatype="boolean">${BOOLEANS_VAL_S[$I]}</pending_status>
  </selinuxboolean_state>

EOF
    I=$[$I+1]
done

I=0
while [ $I -lt $(( ${#BOOLEANS[@]} - 1 )) ]; do
    cat <<EOF
  <selinuxboolean_state version="1" id="oval:1:ste:$[$C+$C+$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
    <name>${BOOLEANS[$I]}</name>
    <current_status datatype="boolean">${BOOLEANS_VAL_S[$I]}</current_status>
    <pending_status datatype="boolean">${BOOLEANS_VAL_N[$I]}</pending_status>
  </selinuxboolean_state>
EOF
    I=$[$I+1]
done

    cat <<EOF

  </states>

</oval_definitions>
EOF

exit $(( 3 * ${#BOOLEANS[@]} - 1))
