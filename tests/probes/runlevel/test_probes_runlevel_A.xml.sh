#!/usr/bin/env bash

. runlevel_helper.sh

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

      <generator>
            <oval:product_name>runlevel</oval:product_name>
            <oval:product_version>1.0</oval:product_version>
            <oval:schema_version>5.4</oval:schema_version>
            <oval:timestamp>2008-03-31T00:00:00-00:00</oval:timestamp>
      </generator>

  <definitions>
EOF

SERVICES_LIST=`get_services_list`
echo SERVICES_LIST=$SERVICES_LIST >&2
for S in $SERVICES_LIST; do
    echo looking at service $S >&2
    for L in `get_service_runlevels $S`; do
	LEVEL=`echo $L | awk -F : '{print $1}'`
	STATE=`echo $L | awk -F : '{print $2}'`
	cat<<EOF	
      <definition class="compliance" version="1" id="oval:${S}-${LEVEL}-${STATE}-T:def:1" > <!-- comment="true" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="oval:${S}-${LEVEL}-${STATE}-T:tst:1"/>
        </criteria>
      </criteria>
    </definition>

      <definition class="compliance" version="1" id="oval:${S}-${LEVEL}-${STATE}-F:def:1" > <!-- comment="false" -->
      <metadata>
        <title></title>
        <description></description>
      </metadata>
      <criteria>
	<criteria operator="AND">
          <criterion test_ref="oval:${S}-${LEVEL}-${STATE}-F:tst:1"/>
        </criteria>
      </criteria>
    </definition>

EOF
    done
done
cat<<EOF

  </definitions>
  <tests>

EOF

for S in $SERVICES_LIST; do
    for L in `get_service_runlevels $S`; do
	LEVEL=`echo $L | awk -F : '{print $1}'`
	STATE=`echo $L | awk -F : '{print $2}'`
	
	echo "     <runlevel_test check_existence=\"all_exist\" comment=\"true\" version=\"1\" id=\"oval:${S}-${LEVEL}-${STATE}-T:tst:1\" check=\"all\" xmlns=\"http://oval.mitre.org/XMLSchema/oval-definitions-5#unix\">"
        echo "       <object object_ref=\"oval:${S}-${LEVEL}-${STATE}:obj:1\"/>"

	if [ $STATE"X" = "onX" ]; then
            echo "       <state state_ref=\"oval:0:ste:1\"/>"
	else
            echo "       <state state_ref=\"oval:0:ste:2\"/>"
	fi

	echo "     </runlevel_test>"

	echo "     <runlevel_test check_existence=\"all_exist\" comment=\"false\" version=\"1\" id=\"oval:${S}-${LEVEL}-${STATE}-F:tst:1\" check=\"all\" xmlns=\"http://oval.mitre.org/XMLSchema/oval-definitions-5#unix\">"
        echo "       <object object_ref=\"oval:${S}-${LEVEL}-${STATE}:obj:1\"/>"

	if [ $STATE"X" = "onX" ]; then
            echo "       <state state_ref=\"oval:0:ste:2\"/>"
	else
            echo "       <state state_ref=\"oval:0:ste:1\"/>"
	fi

	echo "     </runlevel_test>"
    done
done

cat <<EOF
  </tests>

  <objects>
EOF

for S in $SERVICES_LIST; do
    for L in `get_service_runlevels $S`; do
	LEVEL=`echo $L | awk -F : '{print $1}'`
	STATE=`echo $L | awk -F : '{print $2}'`
	echo "     <runlevel_object version=\"1\" id=\"oval:${S}-${LEVEL}-${STATE}:obj:1\" xmlns=\"http://oval.mitre.org/XMLSchema/oval-definitions-5#unix\">"
	echo "       <service_name>${S}</service_name>"
	echo "       <runlevel>${LEVEL}</runlevel>"
	echo "     </runlevel_object>"
    done
done
    
cat <<EOF
  </objects>

  <states>

    <!-- TRUE STATE -->
    <runlevel_state version="1" id="oval:0:ste:1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <start datatype="boolean">true</start>
    </runlevel_state>

    <!-- FALSE STATE -->
    <runlevel_state version="1" id="oval:0:ste:2" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <start datatype="boolean">false</start>
    </runlevel_state>

  </states>

</oval_definitions>
EOF
