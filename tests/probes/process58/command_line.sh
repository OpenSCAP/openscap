#!/bin/bash

set -e -o pipefail

function clean_processes {
	# Processes are in stopped state. SIGCONT cause their exiting
	[ -n "${ZOMBIE_PPID}" ] && kill -SIGCONT ${ZOMBIE_PPID}
	[ -n "${PID}" ] && kill -SIGCONT ${PID}
	[ -n "${ESCAPED_PPID}" ] && kill -SIGCONT ${ESCAPED_PID}
}
trap clean_processes EXIT

PROC="$srcdir/stopped_process.sh" # the process go to stopped state after start

# Parse "ps" and return command line of process with $PID
function get_process_cmdline() {
	local PID="$1"
	ps opid,cmd | grep -E "^\s*$PID\s+" | sed -E 's/^\s*'$PID'\s+(.*)$/\1/'
	return 0
}

# Wait for process start
# We can scan processes before process's exec() and get wrong command_line
function wait_for_process() {
	local PID="$1"
	for i in `seq 1 100`; # wait max 100 * 100ms
		do
			PROCESS_CMDLINE="`get_process_cmdline $PID`"
			[[ "${PROCESS_CMDLINE}" == *${PROC}* ]] && break
			sleep 0.1s
		done
}

# Actively wait for zombie process with specified PPID and return its PID
# Wait max 10 x 100ms
function get_zombie_pid_from_ppid() {
	local PARENT_PID="$1"
	for i in `seq 1 100`;
		do
			ZOMBIE_PID=`ps -ostate,pid --ppid ${PARENT_PID}| grep "^Z" | sed -E 's/^.\s*?([[:digit:]]+).*$/\1/'`
			[ "${ZOMBIE_PID}" != "" ] && break;
			sleep 0.1s
		done
	echo ${ZOMBIE_PID}
}

function get_command_line_node_text() {
	local PID="$1"
	local item='/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:process58_item[@status="exists"]/unix-sys:pid[text()="'${PID}'"]'
	assert_exists 1 "${item}"
	xpath $result "${item}/../unix-sys:command_line/text()" 2>/dev/null
}

function assert_match_command_line() {
	local PID="$1"
	local EXPECTED_COMMAND_LINE="$2"
	
	# get text from element and fix xpath bug
	cmdline="`get_command_line_node_text \"$PID\" | sed 's|&lt;|<|g' | sed 's|&gt;|>|g'`" 
	
	echo "$cmdline" | grep -qE "${EXPECTED_COMMAND_LINE}" || { # compare commands
		echo "Failed: expected command-line of process with PID=${PID} is '${EXPECTED_COMMAND_LINE}' but real is: '$cmdline'"
		exit 1
	}
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
	[ -n "${PID}" ]
	wait_for_process $PID
	# "/bin/bash ./stopped_process.sh param1 param2 param3"
	CMDLINE_REGEX='/(\w+/)+bash.*stopped_process\.sh param1 param2 param3$'


	# Run zombie process (without full cmdline)
	( : & exec "${PROC}" ) &
	ZOMBIE_PPID=$!
	ZOMBIE_PID=`get_zombie_pid_from_ppid ${ZOMBIE_PPID}`
	[ -n "${ZOMBIE_PPID}" ]
	wait_for_process ${ZOMBIE_PID}
	# "[command_line.sh] <defunct>"
	ZOMBIE_CMDLINE_REGEX='^\[command_line.sh\] <defunct>$'


	# Run process with special characters in parameters
	"${PROC}" escaped "`echo -ne \"\e\n\E[1;33m\"`" "\\\n\e" &
	ESCAPED_PID=$!
	[ -n "${ESCAPED_PID}" ]
	wait_for_process ${ESCAPED_PID}
	# "/bin/bash ./stopped_process.sh escaped . .[1;33m \\n\e"
	ESCAPED_CMDLINE_REGEX='/(\w+/)+bash.*stopped_process\.sh escaped \. \.\[1;33m \\\\n\\e$'

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
	assert_match_command_line "${PID}" "${CMDLINE_REGEX}"
	assert_match_command_line "${ZOMBIE_PID}" "${ZOMBIE_CMDLINE_REGEX}"
	assert_match_command_line "${ESCAPED_PID}" "${ESCAPED_CMDLINE_REGEX}"

	rm $result
