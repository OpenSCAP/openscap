#!/usr/bin/env bash

# number of processes used in the content
PCNT=5
# a list of relevant process information obtained from 'ps'
declare -A PROCSALL
# count duplicates
declare -A dups

IFS=$'\n'
# the '=' is there to remove the header
ps=( $(ps -A -o etime= -o pid= -o ppid= -o class= -o uid= -o comm= -o start= | \
# replace spaces in 'etime' with '-' to enable sorting
sed 'h;s/.\{2,9\}:.. / /;x;s/\(.\{2,9\}:..\) .*/\1/;s/ /-/g;G;s/\n//') )
IFS=$' \t\n'

for l in "${ps[@]}"; do
    # todo: spaces in 'comm' are a problem
    a=( $l )
    ((dups[${a[5]}]++))
    # skip kthreads and direct descendants
    [ ${a[1]} == 2 -o ${a[2]} == 2 ] && continue
    PROCSALL[${a[1]}]="$l"
done

# prefer the longest running processes to prevent race conditions
IFS=$'\n'
timelst=( $(sort -r <<<"${PROCSALL[*]}") )
IFS=$' \t\n'

# create a pid list for queries
declare PROCS
PROCS[0]=dummy
for l in "${timelst[@]}"; do
    [ ${#PROCS[@]} -eq $PCNT ] && break;
    a=( $l )
    [ ${dups[${a[5]}]} -eq 1 ] && PROCS+=( ${a[1]} )
done

function getField {
    a=( ${PROCSALL[$2]} )
    case "$1" in
        comm)
            echo "${a[5]}"
            ;;
        pid)
            echo "${a[1]}"
            ;;
        ppid)
            echo "${a[2]}"
            ;;
        class)
            echo "${a[3]}"
            ;;
        uid)
            echo "${a[4]}"
            ;;
        start)
            echo "${a[6]}"
            ;;
        *)
            echo "null"
            ;;
    esac
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
while [ $I -lt "${#PROCS[@]}" ]; do
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
while [ $I -lt "${#PROCS[@]}" ]; do
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
while [ $I -lt "${#PROCS[@]}" ]; do
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
while [ $I -lt "${#PROCS[@]}" ]; do
    cat <<EOF
    <process_state version="1" id="oval:1:ste:${I}" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <command>`getField 'comm' ${PROCS[$I]}`</command>
      <pid datatype="int">`getField 'pid' ${PROCS[$I]}`</pid>
      <ppid datatype="int">`getField 'ppid' ${PROCS[$I]}`</ppid>
      <scheduling_class>`getField 'class' ${PROCS[$I]}`</scheduling_class>
      <start_time>`getField 'start' ${PROCS[$I]} | sed 's/\([A-Z][a-z]\{2\}\)\([0-9]\{1,2\}\)/\1_\2/'`</start_time>
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
