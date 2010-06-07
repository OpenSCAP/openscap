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

ENV_VAR=( "PATH" "PWD" "HOME" "DISPLAY" "LANG" "EDITOR" )

I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do    
    VAR_VAL[$I]="`env | grep -e "^${ENV_VAR[$I]}" | awk -F '=' '{print $2}'`"
    I=$[$I+1]
done

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

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
while [ $I -lt ${#ENV_VAR[@]} ]; do
    cat <<EOF
    <environmentvariable_test version="1" id="oval:1:tst:$[$I+1]" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <object object_ref="oval:1:obj:$[$I+1]"/>
      <state state_ref="oval:1:ste:$[$I+1]"/>
    </environmentvariable_test>

EOF
    I=$[$I+1]
done
    
    cat <<EOF

  </tests>

  <objects>

EOF

I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do
    cat <<EOF
    <environmentvariable_object version="1" id="oval:1:obj:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
      <name>${ENV_VAR[$I]}</name>
    </environmentvariable_object>

EOF
    I=$[$I+1]
done
    
    cat <<EOF

  </objects>

  <states>

EOF

I=0
while [ $I -lt ${#ENV_VAR[@]} ]; do
    cat <<EOF
  <environmentvariable_state version="1" id="oval:1:ste:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
    <name>${ENV_VAR[$I]}</name>
    <value>${VAR_VAL[$I]}</value>
  </environmentvariable_state>

EOF
    I=$[$I+1]
done
    
    cat <<EOF

  </states>

</oval_definitions>
EOF

exit ${#ENV_VAR[@]}
