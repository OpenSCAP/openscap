if [  -x "`which chkconfig`" ] ; then
	HAVE_CHKCONFIG=y
else
	HAVE_CHKCONFIG=n
fi

# Gets the chkconfig list with chkconfig direclty if HAVE_CHKCONFIG
# Otherwise produces the same information in another way
get_services_list() {
	if [ "$HAVE_CHKCONFIG" = "y" ] ; then
		LC_ALL=C chkconfig --list | awk '{print $1}' | sort | uniq
	else
		ls /etc/init.d/ | egrep -v "(README|PRESERVE)" | xargs
	fi
}

# Gets the chkconfig level with chkconfig direclty if HAVE_CHKCONFIG
# Otherwise produces the same information in another way
get_service_runlevels() {

	local SRV=$1

	if [ "$HAVE_CHKCONFIG" = "y" ] ; then
		LC_ALL=C chkconfig $SRV --list | awk '{print $2 " " $3 " " $4 " " $5 " " $6 " " $7 " " $8}'
	else
		for RC in 0 1 2 3 4 5 6; do
			RCDIR=/etc/rc$RC.d
			if [ -d $RCDIR ]; then
				if [ -f $RCDIR/S[0-9][0-9]$SRV ] ; then
					STATE="on"
				else
					STATE="off"
				fi
				echo "$RC:$STATE"
			fi
		done
	fi
}

# Gets a list of services matching the given state at the given runlevel
get_services_matching() {
	local RUNLEVEL=$1
	local STATE=$2
	for S in `get_services_list`; do
		if get_service_runlevels $S | grep $RUNLEVEL:$STATE >/dev/null; then
			echo $S
		fi
	done
}
