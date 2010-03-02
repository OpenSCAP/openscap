#!/usr/bin/env bash

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

  <tests>
EOF

for S in `chkconfig --list | awk '{print $1}'`; do
    for L in `chkconfig $S --list | awk '{print $2 " " $3 " " $4 " " $5 " " $6 " " $7 " " $8}'`; do
	LEVEL=`echo $L | awk -F : '{print $1}'`
	STATE=`echo $L | awk -F : '{print $2}'`
	
	echo "     <runlevel_test check_existence=\"all_exist\" comment=\"true\" version=\"1\" id=\"test:${S}-${LEVEL}-${STATE}-T\" check=\"all\" xmlns=\"http://oval.mitre.org/XMLSchema/oval-definitions-5#unix\">"
        echo "       <object object_ref=\"object:${S}-${LEVEL}-${STATE}\"/>"

	if [ $STATE"X" = "onX" ]; then
            echo "       <state state_ref=\"state:true\"/>"
	else
            echo "       <state state_ref=\"state:false\"/>"
	fi

	echo "     </runlevel_test>"

	echo "     <runlevel_test check_existence=\"all_exist\" comment=\"false\" version=\"1\" id=\"test:${S}-${LEVEL}-${STATE}-F\" check=\"all\" xmlns=\"http://oval.mitre.org/XMLSchema/oval-definitions-5#unix\">"
        echo "       <object object_ref=\"object:${S}-${LEVEL}-${STATE}\"/>"

	if [ $STATE"X" = "onX" ]; then
            echo "       <state state_ref=\"state:false\"/>"
	else
            echo "       <state state_ref=\"state:true\"/>"
	fi

	echo "     </runlevel_test>"
    done
done

cat <<EOF
  </tests>

  <objects>
EOF

for S in `chkconfig --list | awk '{print $1}'`; do
    for L in `chkconfig $S --list | awk '{print $2 " " $3 " " $4 " " $5 " " $6 " " $7 " " $8}'`; do
	LEVEL=`echo $L | awk -F : '{print $1}'`
	STATE=`echo $L | awk -F : '{print $2}'`
	echo "     <runlevel_object version=\"1\" id=\"object:${S}-${LEVEL}-${STATE}\" xmlns=\"http://oval.mitre.org/XMLSchema/oval-definitions-5#unix\">"
	echo "       <service_name>${S}</service_name>"
	echo "       <runlevel>${LEVEL}</runlevel>"
	echo "     </runlevel_object>"
    done
done
    
cat <<EOF
  </objects>

  <states>

    <!-- TRUE STATE -->
    <runlevel_state version="1" id="state:true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <start datatype="boolean">true</start>
    </runlevel_state>

    <!-- FALSE STATE -->
    <runlevel_state version="1" id="state:false" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <start datatype="boolean">false</start>
    </runlevel_state>

  </states>

</oval_definitions>
EOF
