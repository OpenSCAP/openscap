#!/usr/bin/env bash

TMP_P=(`LD_PRELOAD= ps -A -o pid -o ppid -o comm | awk '$1 != 2 && $2 != 2 {print $3}' | \
        sed -n '2,30p'`)

COUNTER=1
for I in "${TMP_P[@]}"; do
    J="`echo $I | sed 's/^-/\\\\-/'`"
    if [ `LD_PRELOAD= ps -A -o comm| grep $J | wc -l` -eq 1 ]; then
	PROCS[$COUNTER]="$I"
	COUNTER=$[$COUNTER+1];
    fi
done

function getField {
    COMM="`echo ${2} | sed 's/^-/\\\\-/'`"
    echo `LD_PRELOAD= ps -A -o comm -o ${1} | grep ${COMM} | awk '{ print $2 }'`
}

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

      <generator>
            <oval:product_name>process</oval:product_name>
            <oval:product_version>1.0</oval:product_version>
            <oval:schema_version>5.4</oval:schema_version>
            <oval:timestamp>2008-03-31T00:00:00-00:00</oval:timestamp>
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
I=1
while [ $I -le "${#PROCS[@]}" ]; do
    echo "<criterion test_ref=\"oval:1:tst:${I}\"/>"
    I=$[$I+1]
done
cat <<EOF
        </criteria>
      </criteria>
    </definition>

  </definitions>

  <tests>

EOF

I=1
while [ $I -le "${#PROCS[@]}" ]; do
    cat <<EOF
    <process_test version="1" id="oval:1:tst:${I}" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:${I}"/>
      <state state_ref="oval:1:ste:${I}"/>
    </process_test>
EOF
    I=$[$I+1]
done

cat <<EOF
  </tests>

  <objects>
EOF

I=1
while [ $I -le "${#PROCS[@]}" ]; do
    cat <<EOF
    <process_object version="1" id="oval:1:obj:${I}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <command>`getField 'comm' ${PROCS[$I]}`</command>
    </process_object>
EOF
    I=$[$I+1]
done

cat <<EOF
  </objects>

  <states>
EOF

I=1
while [ $I -le "${#PROCS[@]}" ]; do
    cat <<EOF
    <process_state version="1" id="oval:1:ste:${I}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <command>`getField 'comm' ${PROCS[$I]}`</command>
      <pid datatype="int">`getField 'pid' ${PROCS[$I]}`</pid>
      <ppid datatype="int">`getField 'ppid' ${PROCS[$I]}`</ppid>
      <scheduling_class>`getField 'class' ${PROCS[$I]}`</scheduling_class>
      <start_time>`getField 'stime' ${PROCS[$I]} | sed 's/\([A-Z][a-z]\{2\}\)\([0-9]\{1,2\}\)/\1_\2/'`</start_time>
      <user_id datatype="int">`getField 'uid' ${PROCS[$I]}`</user_id>
    </process_state>
EOF
    I=$[$I+1]
done

cat <<EOF
  </states>

</oval_definitions>
EOF

exit ${#PROCS[@]};
