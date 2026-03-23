#!/usr/bin/env bash

passwd_file=$(mktemp)
readonly AWK_PRINT_FIRST_FIELD='{print $1}'

# getpwent returns duplicate entries for root and nobody users
# due to a bug in systemd-userdb.service that occurs
# in systemd 245
# https://github.com/systemd/systemd/issues/15160
if command -v systemctl >/dev/null 2>&1 && \
		systemctl --version | grep -q "systemd 245" ; then
	grep -Ev '^(root|nobody)' /etc/passwd > "$passwd_file"
else
	case "$(uname)" in
		# BSD passwd files may contain comments that are ignored by getpwent(3).
		Darwin|FreeBSD)
			grep -Ev '^(#|$)' /etc/passwd > "$passwd_file"
			;;
		# Fall back to the raw passwd file elsewhere.
		*)
			cp /etc/passwd "$passwd_file"
			;;
	esac
fi

LINES_COUNT=$(wc -l < "$passwd_file")

function getField {
    local field_name="$1"
    local line_number="$2"
    local line
    local username

    line=$(sed -n "${line_number}p" "$passwd_file")
    username=$(echo "$line" | awk -F':' "$AWK_PRINT_FIRST_FIELD")

    case "$field_name" in
	'username' )
	    echo "$username"
	    ;;
	'password' )
	    echo "$line" | awk -F':' '{print $2}'
	    ;;
	'user_id' )
	    case "$(uname)" in
		FreeBSD)
		    id -u "$username"
			;;
		Darwin)
		    id -u "$username"
			;;
		*)
		    echo "$line" | awk -F':' '{print $3}'
		    ;;
	    esac
	    ;;
	'group_id' )
	    case "$(uname)" in
		FreeBSD)
		    id -g "$username"
		    ;;
		Darwin)
		    id -g "$username"
		    ;;
		*)
		    echo "$line" | awk -F':' '{print $4}'
		    ;;
	    esac
	    ;;
	'gcos' )
	    echo "$line" | awk -F':' '{gsub(/&/,"&amp;",$5); print $5}'
	    ;;
	'home_dir' )
	    echo "$line" | awk -F':' '{print $6}'
	    ;;
	'login_shell' )
	    echo "$line" | awk -F':' '{print $7}'
	    ;;
    esac
}

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

      <generator>
            <oval:product_name>password</oval:product_name>
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
      <username>$(getField "username" "${I}")</username>
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
      <username>$(getField 'username' "$I")</username>
      <password>$(getField 'password' "$I")</password>
      <user_id datatype="int">$(getField 'user_id' "$I")</user_id>
      <group_id datatype="int">$(getField 'group_id' "$I")</group_id>
      <gcos>$(getField 'gcos' "$I")</gcos>
      <home_dir>$(getField 'home_dir' "$I")</home_dir>
      <login_shell>$(getField 'login_shell' "$I")</login_shell>
    </password_state>
EOF
    I=$[$I+1]
done

cat <<EOF
  </states>

</oval_definitions>
EOF
