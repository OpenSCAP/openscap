#!/usr/bin/env bash

LINES_COUNT=`cat /etc/passwd | wc -l`

function getField {
    LINE=`sed -n "${I}p" /etc/passwd`
    case $1 in 
	'username' )
	    echo $LINE | awk -F ':' '{print $1}'
	    ;;
	'password' )
	    echo $LINE | awk -F ':' '{print $2}'
	    ;;
	'user_id' )
	    echo $LINE | awk -F ':' '{print $3}'
	    ;;
	'group_id' )
	    echo $LINE | awk -F ':' '{print $4}'
	    ;;
	'gcos' )
	    echo $LINE | awk -F ':' '{print $5}'
	    ;;
	'home_dir' )
	    echo $LINE | awk -F ':' '{print $6}'
	    ;;
	'login_shell' )
	    echo $LINE | awk -F ':' '{print $7}'
	    ;;
    esac
}

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
    <password_test version="1" id="oval:1:tst:${I}" check="all" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:${I}"/>
      <state state_ref="oval:1:ste:${I}"/>
    </password_test>
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
    <password_object version="1" id="oval:1:obj:${I}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <username>`getField "username" ${I}`</username>
    </password_object>
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
    <password_state version="1" id="oval:1:ste:${I}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <username>`getField 'username' $I`</username>
      <password>`getField 'password' $I`</password>
      <user_id>`getField 'user_id' $I`</user_id>
      <group_id>`getField 'group_id' $I`</group_id>
      <gcos>`getField 'gcos' $I`</gcos>
      <home_dir>`getField 'home_dir' $I`</home_dir>
      <login_shell>`getField 'login_shell' $I`</login_shell>
    </password_state>
EOF
    I=$[$I+1]
done

cat <<EOF
  </states>

</oval_definitions>
EOF
