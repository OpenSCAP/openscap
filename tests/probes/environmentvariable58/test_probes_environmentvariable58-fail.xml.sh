#!/usr/bin/env bash

# i=0;
# IFS=$'\n'
# for line in `env`; do
#         VAR=`echo $line | sed 's/\([A-Za-z_1-9]*\)=.*$/\1/'`
#         if [ "x$VAR" == "x_" ] || [ "x$VAR" == "xSHLVL" ]; then
#                 continue;
#         fi
#         ENV_VAR[$i]="$VAR";
#         VAR_VAL[$i]=`echo $line | sed 's/\([A-Za-z_1-9]*\)=\(.*\)$/\2/'`
#         i=$[$i+1];
# done

ENV_VAR=(`env | sed -n 's|^\([A-Z]*\)=[a-zA-Z0-9/]\{1,\}$|\1|p' | egrep -v SHLVL`)

I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do
    eval "VAL=\$${ENV_VAR[$I]}"
    VAR_VAL[$I]=$VAL
    I=$[$I+1]
done

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

  <generator>
    <oval:product_name>environmentvariable58</oval:product_name>
    <oval:product_version>1.0</oval:product_version>
    <oval:schema_version>5.9</oval:schema_version>
    <oval:timestamp>2011-07-13T00:00:00-00:00</oval:timestamp>
  </generator>

  <definitions>

    <definition class="compliance" version="1" id="oval:1:def:1">  <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
        <criteria operator="OR">
          <criterion test_ref="oval:1:tst:1"/>
EOF
I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do
    cat <<EOF
          <criterion test_ref="oval:1:tst:$[I+2]"/>
EOF
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

    <environmentvariable58_test version="1" id="oval:1:tst:1" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:1"/>
      <state state_ref="oval:1:ste:1"/>
    </environmentvariable58_test>
EOF
I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do
    cat <<EOF
    <environmentvariable58_test version="1" id="oval:1:tst:$[I+2]" check="at least one" comment="false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:2"/>
      <state state_ref="oval:1:ste:$[I+2]"/>
    </environmentvariable58_test>
EOF
   I=$[$I+1]
done

cat <<EOF
  </tests>

  <objects>

    <environmentvariable58_object version="1" id="oval:1:obj:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <pid datatype="int" xsi:nil="true" />
      <name operation="not equal">${ENV_VAR[1]}</name>
    </environmentvariable58_object>

    <environmentvariable58_object version="1" id="oval:1:obj:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <pid datatype="int" xsi:nil="true" />
      <name operation="pattern match">_</name>
    </environmentvariable58_object>

  </objects>

  <states>

  <environmentvariable58_state version="1" id="oval:1:ste:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
    <pid operation="greater than" datatype="int">1</pid>
    <name>${ENV_VAR[1]}</name>
    <value>${VAR_VAL[1]}</value>
  </environmentvariable58_state>
EOF

I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do
    cat <<EOF
  <environmentvariable58_state version="1" id="oval:1:ste:$[$I+2]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
    <pid operation="greater than" datatype="int">1</pid>
    <name>${ENV_VAR[$I]}</name>
    <value>${VAR_VAL[$I]}</value>
  </environmentvariable58_state>
EOF
   I=$[$I+1]
done

cat <<EOF

  </states>

</oval_definitions>
EOF

exit $(( ${#ENV_VAR[@]} + 1 ))
