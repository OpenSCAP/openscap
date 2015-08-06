#!/bin/bash

export LC_ALL=C
set -e -o pipefail

function clean_processes {
	# Processes are in stopped state. SIGCONT cause their exiting
	kill -SIGCONT ${ZOMBIE_PPID}
	kill -SIGCONT ${PID}
	kill -SIGCONT ${ESCAPED_PID}  
}
trap clean_processes EXIT

PROC="$srcdir/stopped_process.sh" # the process go to stopped state after start

# Parse "ps" and return command line of process with $PID
function get_process_cmdline() {
	local PID="$1"
	ps opid,cmd | grep -E "^\s*$PID\s+" | sed -E 's/^\s*'$PID'\s+(.*)$/\1/'
}

# Wait for command line of child
function get_child_cmdline() {
	local PID="$1"
	for i in `seq 1 10`;
		do
			PROCESS_CMDLINE="`get_process_cmdline $PID`"
			[[ "${PROCESS_CMDLINE}" == *${PROC}* ]] && break
			sleep 0.1s
		done
	echo "${PROCESS_CMDLINE}"
}

# Actively wait for zombie process with specified PPID and return its PID
# Wait max 10 x 100ms
function get_zombie_pid_from_ppid() {
	local PARENT_PID="$1"
	for i in `seq 1 10`;
		do
			ZOMBIE_PID=`ps -ostate,pid --ppid ${PARENT_PID}| grep "^Z" | sed -E 's/^.\s*?([[:digit:]]+).*$/\1/'`
			[ "${ZOMBIE_PID}" != "" ] && break;
			sleep 0.1s
		done
	echo ${ZOMBIE_PID}
}

# Return XPATH expression to search process58_item in oval-result with corresponding "PID" and "command_line"
function pid_command_line_xpath() {
		local PID="$1"
		local COMMAND_LINE="$2"
		local ITEM_PATH='/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:process58_item[@status="exists"]'
		echo "${ITEM_PATH}/unix-sys:pid[text()=\"$PID\"]"\
		"/../unix-sys:command_line[text()=\"${COMMAND_LINE}\"]"
}

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "result file: $result"
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

########################################################################
### Run monitored processes:
########################################################################
	# Run process with simple params
	"${PROC}" param1 param2 param3 &
	PID=$!
	CMDLINE="`get_child_cmdline $PID`"
	[ -n "$PID" ]
	[ -n "$CMDLINE" ]


	# Run zombie process (without full cmdline)
	( : & exec "${PROC}" ) &
	ZOMBIE_PPID=$!
	ZOMBIE_PID=`get_zombie_pid_from_ppid ${ZOMBIE_PPID}`
	ZOMBIE_CMDLINE="`get_process_cmdline ${ZOMBIE_PID}`"
	[ -n "${ZOMBIE_PPID}" ]
	[ -n "${ZOMBIE_CMDLINE}" ]

	# Run process with special characters in parameters
	"${PROC}" escaped "`echo -ne \"\e\n\E[1;33m\"`" "\\\n\e" &
	ESCAPED_PID=$!
	ESCAPED_CMDLINE="`get_child_cmdline ${ESCAPED_PID}`"
	[ -n "${ESCAPED_PID}" ]
	[ -n "${ESCAPED_CMDLINE}" ]

########################################################################
### Run evaluation:
########################################################################
	echo "Eval:"
	$OSCAP oval eval --results $result $srcdir/$name.oval.xml 2> $stderr
	[ ! -s $stderr ]
	rm $stderr

########################################################################
### Check results:
########################################################################
	[ -s $result ]
	assert_exists 1 "`pid_command_line_xpath \"${PID}\" \"${CMDLINE}\"`"
	assert_exists 1 "`pid_command_line_xpath \"${ZOMBIE_PID}\" \"${ZOMBIE_CMDLINE}\"`"
	assert_exists 1 "`pid_command_line_xpath \"${ESCAPED_PID}\" \"${ESCAPED_CMDLINE}\"`"

	rm $result
