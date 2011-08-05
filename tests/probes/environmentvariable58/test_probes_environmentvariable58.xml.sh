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

C=$I

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

    <definition class="compliance" version="1" id="oval:1:def:1">  <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
        <criteria operator="AND">
EOF
I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do
    echo "<criterion test_ref=\"oval:1:tst:$[$I+1]\"/>"
    echo "<criterion test_ref=\"oval:1:tst:$[$C+$I+1]\"/>"
    I=$[$I+1]
done
I=0
while [ $I -lt $(( ${#ENV_VAR[@]} - 1 )) ]; do
    echo "<criterion test_ref=\"oval:1:tst:$[$C+$C+$I+1]\"/>"
    I=$[$I+1]
done
    echo "<criterion test_ref=\"oval:1:tst:$[$C+$C+$I+1]\"/>"
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
while [ $I -lt ${#ENV_VAR[@]} ]; do
    cat <<EOF
    <environmentvariable58_test version="1" id="oval:1:tst:$[$I+1]" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:$[$I+1]"/>
      <state state_ref="oval:1:ste:$[$I+1]"/>
    </environmentvariable58_test>

    <environmentvariable58_test version="1" id="oval:1:tst:$[$C+$I+1]" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:$[$C+1]"/>
      <state state_ref="oval:1:ste:$[$C+$I+1]"/>
    </environmentvariable58_test>
EOF
    I=$[$I+1]
done

I=0
while [ $I -lt $(( ${#ENV_VAR[@]} - 1 )) ]; do
    cat <<EOF
<environmentvariable58_test version="1" id="oval:1:tst:$[$C+$C+$I+1]" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:$[$C+2]"/>
      <state state_ref="oval:1:ste:$[$C+$C+$I+1]"/>
    </environmentvariable58_test>
EOF
    I=$[$I+1]
done

    cat <<EOF
    <environmentvariable58_test version="1" id="oval:1:tst:$[$C+$C+$I+1]" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:$[$C+3]"/>
      <state state_ref="oval:1:ste:$[$C+$C+$I+1]"/>
    </environmentvariable58_test>

  </tests>

  <objects>

EOF

I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do
    cat <<EOF
    <environmentvariable58_object version="1" id="oval:1:obj:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <pid datatype="int" xsi:nil="true" />
      <name>${ENV_VAR[$I]}</name>
    </environmentvariable58_object>

EOF
    I=$[$I+1]
done

cat <<EOF
    <environmentvariable58_object version="1" id="oval:1:obj:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <pid datatype="int" xsi:nil="true" />
      <name operation="pattern match">.*</name>
    </environmentvariable58_object>

    <environmentvariable58_object version="1" id="oval:1:obj:$[$I+2]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <pid datatype="int" xsi:nil="true" />
      <name operation="not equal">${ENV_VAR[$I-1]}</name>
    </environmentvariable58_object>

    <environmentvariable58_object version="1" id="oval:1:obj:$[$I+3]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <pid datatype="int" operation="less than">2</pid>
      <name>PATH</name>
    </environmentvariable58_object>

EOF
    cat <<EOF

  </objects>

  <states>

EOF

I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do
    cat <<EOF
  <environmentvariable58_state version="1" id="oval:1:ste:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
    <pid operation="greater than" datatype="int">1</pid>
    <name>${ENV_VAR[$I]}</name>
    <value>${VAR_VAL[$I]}</value>
  </environmentvariable58_state>

  <environmentvariable58_state version="1" id="oval:1:ste:$[$C+$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
    <pid operation="greater than" datatype="int">1</pid>
    <name>${ENV_VAR[$I]}</name>
    <value>${VAR_VAL[$I]}</value>
  </environmentvariable58_state>

EOF
    I=$[$I+1]
done

I=0
while [ $I -lt $(( ${#ENV_VAR[@]} - 1 )) ]; do
    cat <<EOF
  <environmentvariable58_state version="1" id="oval:1:ste:$[$C+$C+$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
    <pid operation="greater than" datatype="int">1</pid>
    <name>${ENV_VAR[$I]}</name>
    <value>${VAR_VAL[$I]}</value>
  </environmentvariable58_state>
EOF
    I=$[$I+1]
done

    cat <<EOF
  <environmentvariable58_state version="1" id="oval:1:ste:$[$C+$C+$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
    <pid datatype="int">1</pid>
    <name>PATH</name>
    <value operation="pattern match">.*</value>
  </environmentvariable58_state>

  </states>

</oval_definitions>
EOF

exit $(( 3 * ${#ENV_VAR[@]}))
