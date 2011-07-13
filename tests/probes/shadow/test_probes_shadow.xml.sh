#!/usr/bin/env bash

LINES_COUNT=`cat /etc/shadow | wc -l`

function getField {
    LINE=`sed -n "${I}p" /etc/shadow`
    case $1 in 
	'username' )
	    echo $LINE | awk -F':' '{print $1}'
	    ;;
	'password' )
	    echo $LINE | awk -F':' '{print $2}'
	    ;;
	'chg_lst' )
	    local CHGLST=`echo $LINE | awk -F':' '{print $3}'`
	    if [ "${CHGLST}X" = "X" ]; then echo "-1"; else echo "$CHGLST"; fi
	    ;;
	'chg_allow' )
	    local CHGALLOW=`echo $LINE | awk -F':' '{print $4}'`
	    if [ "${CHGALLOW}X" = "X" ]; then echo "-1"; else echo "$CHGALLOW"; fi
	    ;;
	'chg_req' )
	    local CHGREQ=`echo $LINE | awk -F':' '{print $5}'`
	    if [ "${CHGREQ}X" = "X" ]; then echo "-1"; else echo "$CHGREQ"; fi
	    ;;
	'exp_warn' )
	    local EXPWARN=`echo $LINE | awk -F':' '{print $6}'`
	    if [ "${EXPWARN}X" = "X" ]; then echo "-1"; else echo "$EXPWARN"; fi
	    ;;
	'exp_inact' )
	    local EXPINACT=`echo $LINE | awk -F':' '{print $7}'`
	    if [ "${EXPINACT}X" = "X" ]; then echo "-1"; else echo "$EXPINACT"; fi
	    ;;
	'exp_date' )
	    local EXPDATE=`echo $LINE | awk -F':' '{print $8}'`
	    if [ "${EXPDATE}X" = "X" ]; then echo "-1"; else echo "$EXPDATE"; fi
	    ;;
    esac
}

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

      <generator>
            <oval:product_name>shadow</oval:product_name>
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
    <shadow_test version="1" id="oval:1:tst:${I}" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:${I}"/>
      <state state_ref="oval:1:ste:${I}"/>
    </shadow_test>
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
    <shadow_object version="1" id="oval:1:obj:${I}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <username>`getField "username" ${I}`</username>
    </shadow_object>
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
    <shadow_state version="1" id="oval:1:ste:${I}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <username>`getField 'username' $I`</username>
      <password>`getField 'password' $I`</password>
      <chg_lst datatype="int">`getField 'chg_lst' $I`</chg_lst>
      <chg_allow datatype="int">`getField 'chg_allow' $I`</chg_allow>
      <chg_req datatype="int">`getField 'chg_req' $I`</chg_req>
      <exp_warn datatype="int">`getField 'exp_warn' $I`</exp_warn>
      <exp_inact datatype="int">`getField 'exp_inact' $I`</exp_inact>
      <exp_date datatype="int">`getField 'exp_date' $I`</exp_date>
    </shadow_state>
EOF
    I=$[$I+1]
done

cat <<EOF
  </states>

</oval_definitions>
EOF
