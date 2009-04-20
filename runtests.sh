#!/bin/sh

# Cleanup first
gmake -s clean distclean
./autogen.sh || exit 1

# fstack-protector-all
echo '***************** CHECK: SSP *******************'

export CFLAGS="-fstack-protector-all -g -O1"
./configure && gmake -s && gmake check || exit 1
gmake -s clean

# mudflap
echo '***************** CHECK: mudflap *******************'

export CFLAGS="-fmudflap -lmudflap -g -O1"
export MUDFLAP_OPTIONS='-mode-nop'
./configure && gmake -s || exit 1
export MUDFLAP_OPTIONS='-mode-check -viol-abort -check-initialization -heur-start-end -heur-stack-bound -heur-proc-map -internal-checking'
gmake -s check || exit 1
gmake -s clean

# malloc checks
echo '***************** CHECK: malloc *******************'

export CFLAGS="-g -O1"
./configure && gmake -s || exit 1
case "$(uname -s)" in
	[Ll][Ii][Nn][Uu][Xx])
		export MALLOC_CHECK_=3
		;;
	[Ff][Rr][Ee][Ee][Bb][Ss][Dd])
		export MALLOC_OPTIONS=AJ
		;;
	*)
		echo "Sorry, don't know how to enable malloc debuging"
		exit 1
		;;
esac
gmake -s check || exit 1
unset MALLOC_CHECK_
unset MALLOC_OPTIONS
gmake -s clean
