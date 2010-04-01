#!/usr/bin/env bash

LINES_COUNT=$[`ps -A -o pid -o ppid -o comm | awk '$1 != 2 && $2 != 2 {print $3}' | sort -u | wc -l` - 10]

[ $LINES_COUNT -gt 10 ] && LINES_COUNT=10

function getField {
    echo `ps -A -o pid -o ppid -o comm -o ${1} | awk '$1 != 2 && $2 != 2 {print $3}' | sed -n "$[$2 + 1]p"`
}

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
EOF
I=1
while [ $I -le $LINES_COUNT ]; do
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
while [ $I -le $LINES_COUNT ]; do
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
while [ $I -le $LINES_COUNT ]; do
    cat <<EOF
    <process_object version="1" id="oval:1:obj:${I}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <command>`getField 'comm' ${I}`</command>
    </process_object>
EOF
    I=$[$I+1]
done

cat <<EOF
  </objects>

  <states>
EOF

I=1
while [ $I -le $LINES_COUNT ]; do
    cat <<EOF
    <process_state version="1" id="oval:1:ste:${I}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <command>`getField 'comm' $I`</command>
      <exec_time>`getField 'cputime' $I`</exec_time>
      <pid>`getField 'pid' $I`</pid>
      <ppid>`getField 'ppid' $I`</ppid>
      <scheduling_class>`getField 'cls' $I`</scheduling_class>
      <start_time>`getField 'start_time' $I`</start_time>
      <user_id>`getField 'uid' $I`</user_id>
    </process_state>
EOF
    I=$[$I+1]
done

cat <<EOF
  </states>

</oval_definitions>
EOF
