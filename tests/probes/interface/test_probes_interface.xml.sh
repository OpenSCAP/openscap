#!/usr/bin/env bash

declare -a NAME
declare -a HOST
declare -a MASK
declare -a BRDC

COUNT=0
#for all devices
for dev in `ip link show | sed '/^\s\+/d; s/^[^ ]\+ \([^ ]*\):.*/\1/'`; do
	#find ipv4 addresses
	for line in `ip -4 addr show dev $dev | awk '/\W+inet / {print "D_IPADDR="$2";D_BRDC="$4;}'`; do
		eval $line
		eval `ipcalc --netmask $D_IPADDR`
		eval `ipcalc --broadcast $D_IPADDR`
		if [ $dev == "lo" ]; then
			BROADCAST=
		fi
		if [ "$D_BRDC" != "$BROADCAST" ]; then
			D_BRDC=
		fi
		IPADDR=${D_IPADDR%%/*}
		eval "NAME[$COUNT]=$dev"
		eval "HOST[$COUNT]=$IPADDR"
		eval "MASK[$COUNT]=$NETMASK"
		eval "BRDC[$COUNT]=$BROADCAST"
		COUNT=$((COUNT + 1))
	done
	#find ipv6 addresses
	for line in `ip -6 addr show dev $dev | awk '/\W+inet6 / {print "IPADDR="$2;}'`; do
		eval $line
		eval "NAME[$COUNT]=$dev"
		eval "HOST[$COUNT]=$IPADDR"
		eval "MASK[$COUNT]="
		eval "BRDC[$COUNT]="
		COUNT=$((COUNT + 1))
	done
done

cat <<EOF
<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">

      <generator>
            <oval:product_name>interface</oval:product_name>
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
I=0
while [ $I -lt ${#NAME[@]} ]; do
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
while [ $I -lt ${#NAME[@]} ]; do
    cat <<EOF
    <interface_test version="1" id="oval:1:tst:$[$I+1]" check="at least one" comment="true" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <object object_ref="oval:1:obj:$[$I+1]"/>
      <state state_ref="oval:1:ste:$[$I+1]"/>
    </interface_test>

EOF
    I=$[$I+1]
done
    
    cat <<EOF

  </tests>

  <objects>

EOF

I=0
while [ $I -lt ${#NAME[@]} ]; do
    cat <<EOF
    <interface_object version="1" id="oval:1:obj:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
      <name>${NAME[$I]}</name>
    </interface_object>

EOF
    I=$[$I+1]
done
    
    cat <<EOF

  </objects>

  <states>

EOF

I=0
while [ $I -lt ${#NAME[@]} ]; do
    cat <<EOF
  <interface_state version="1" id="oval:1:ste:$[$I+1]" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix">
    <name>${NAME[$I]}</name>
    <inet_addr>${HOST[$I]}</inet_addr>
    <broadcast_addr>${BRDC[$I]}</broadcast_addr>
    <netmask>${MASK[$I]}</netmask>
  </interface_state>

EOF
    I=$[$I+1]
done
    
    cat <<EOF

  </states>

</oval_definitions>
EOF

exit ${#NAME[@]}
