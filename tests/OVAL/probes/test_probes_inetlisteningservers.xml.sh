#!/usr/bin/env bash

TMP=`mktemp -t asdf.XXXX`

ss -a -u -t -n -p  | \
    sed -n '2,$p' | \
    awk '{print $1 " " $5 " " $6 " " $7}' | \
    sed 's/\([a-z]*\)\s\([a-z0-9\.:\*]*\):\([0-9\*]*\)\s\([a-z0-9\.:\*]*\):\([0-9\*]*\) users:(("\([a-zA-Z\.-]*\)",\([0-9]*\).*/\1 \2 \3 \4 \5 \6 \7 X/' | \
    grep "X" > $TMP

if [ ! -f "$TMP" ]; then
    exit 2
fi

PROTOCOL=( `cat $TMP | awk '{print $1}' | tr '\n' ' '` )
LOCAL_ADDRESS=( `cat $TMP | awk '{print $2}' | sed 's/\*/0.0.0.0/' | tr '\n' ' '` )
LOCAL_PORT=( `cat $TMP | awk '{print $3}' | sed 's/\*/0/' | tr '\n' ' '` )
FOREIGN_ADDRESS=( `cat $TMP | awk '{print $4}' | sed 's/\*/0.0.0.0/' | tr '\n' ' '` )
FOREIGN_PORT=( `cat $TMP | awk '{print $5}' | sed 's/\*/0/' | tr '\n' ' '` )
PROGRAM_NAME=( `cat $TMP | awk '{print $6}' | tr '\n' ' '` )
PID=( `cat $TMP | awk '{print $7}' | sed 's/^$/0/' | tr '\n' ' '` )

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
EOF
I=0
while [ $I -lt $[3*${#PROTOCOL[@]}] ]; do
    cat <<EOF
        <criterion test_ref="oval:1:tst:$[$I+1]"/>
EOF
    I=$[$I+1]
done

    cat <<EOF
      </criteria>
    </definition>      

  </definitions>

  <tests>
EOF

I=0
while [ $I -lt ${#PROTOCOL[@]} ]; do
    cat <<EOF

    <inetlisteningservers_test check="all" comment="true" id="oval:1:tst:$[$I+1]" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:1:obj:$[$I+1]"/>
    </inetlisteningservers_test>
EOF
    I=$[$I+1]
done

I=0
while [ $I -lt ${#PROTOCOL[@]} ]; do
    cat <<EOF

    <inetlisteningservers_test check="all" comment="true" id="oval:1:tst:$[$I+1+${#PROTOCOL[@]}]" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:1:obj:$[$I+1]"/>
      <state state_ref="oval:1:ste:$[$I+1]"/>
    </inetlisteningservers_test>
EOF
    I=$[$I+1]
done

I=0
while [ $I -lt ${#PROTOCOL[@]} ]; do
    cat <<EOF

    <inetlisteningservers_test check="all" comment="false" id="oval:1:tst:$[$I+1+2*${#PROTOCOL[@]}]" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <object object_ref="oval:1:obj:$[$I+1]"/>
      <state state_ref="oval:1:ste:$[1+${#PROTOCOL[@]}]"/>
    </inetlisteningservers_test>
EOF
    I=$[$I+1]
done

    cat <<EOF

  </tests>

  <objects>
EOF

I=0
while [ $I -lt ${#PROTOCOL[@]} ]; do
    cat <<EOF

    <inetlisteningservers_object id="oval:1:obj:$[$I+1]" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <protocol>${PROTOCOL[$I]}</protocol>
      <local_address>${LOCAL_ADDRESS[$I]}</local_address>
      <local_port>${LOCAL_PORT[$I]}</local_port>
    </inetlisteningservers_object>
EOF
    I=$[$I+1]
done

    cat <<EOF

  </objects>

  <states>
EOF
I=0
while [ $I -lt ${#PROTOCOL[@]} ]; do
    cat <<EOF

    <inetlisteningservers_state id="oval:1:ste:$[$I+1]" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <protocol>${PROTOCOL[$I]}</protocol>
      <local_address>${LOCAL_ADDRESS[$I]}</local_address>
      <local_port>${LOCAL_PORT[$I]}</local_port>
      <pid>${PID[$I]}</pid>
      <local_full_address>${LOCAL_ADDRESS[$I]}:${LOCAL_PORT[$I]}</local_full_address>
      <foreign_address>${FOREIGN_ADDRESS[$I]}</foreign_address>
      <foreign_port>${FOREIGN_PORT[$I]}</foreign_port>
      <foreign_full_address>${FOREIGN_ADDRESS[$I]}:${FOREIGN_PORT[$I]}</foreign_full_address>
      <program_name>${PROGRAM_NAME[$I]}</program_name>
    </inetlisteningservers_state>
EOF
    I=$[$I+1]
done

    cat <<EOF

    <inetlisteningservers_state id="oval:1:ste:$[${#PROTOCOL[@]}+1]" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
      <protocol>invalid_protocol</protocol>
      <local_address>1000.1000.1000.1000</local_address>
      <local_port>10000000</local_port>
      <local_full_address>1000.1000.1000.1000:10000000</local_full_address>
      <foreign_address>1000.1000.1000.1000</foreign_address>
      <foreign_port>10000000</foreign_port>
      <foreign_full_address>1000.1000.1000,1000:10000000</foreign_full_address>
      <program_name>invalid_program_name</program_name>
      <pid>1000000</pid>
    </inetlisteningservers_state>

  </states>
</oval_definitions>
EOF

rm -f $TMP

exit ${#PROTOCOL[@]}