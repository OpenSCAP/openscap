#!/usr/bin/env bash
lines=${XCCDF_VALUE_LINES}

# padding numbers so one line is exactly 10 bytes - 9 numbers and newline
# this way last line prints size of the output

case $(uname) in

	# FreeBSD's seq is significantly slower than GNU's seq, in order
	# to work around this, we just print 10 bytes without the count in
	# each loop and only output the loop count at the very end. This
	# should still properly test streaming a lot of data into stdin
	# and stdout like the original test on Linux was designed to do.
	FreeBSD)
		padlength=5

		for i in `seq ${lines}`; do
			echo "000000000"
		done

		strlen=$(echo $i | wc -c)
		strlen=$((strlen-2))
		printf '0%*.*s0%s0\n' 0 $((padlength - strlen)) "$pad" "$i"

		for i in `seq ${lines}`; do
			echo "100000000" >&2
		done

		strlen=$(echo $i | wc -c)
		strlen=$((strlen-2))
		printf '1%*.*s0%s0\n' 0 $((padlength - strlen)) "$pad" "$i"
		;;
	*)
		for i in `seq -w 0000001 ${lines}`; do
			echo "0${i}0"
		done

		for i in `seq -w 0000001 ${lines}`; do
			echo "1${i}0" >&2
		done
		;;
esac

exit ${XCCDF_RESULT_PASS}
