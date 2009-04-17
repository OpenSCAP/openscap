#!/bin/sh

# Cleanup first
gmake -s clean distclean

./autogen.sh || exit 1

export CFLAGS="-fmudflap -lmudflap"
./configure && gmake -s || exit 1
 
export MUDFLAP_OPTIONS='-mode-check -viol-abort -check-initialization -heur-start-end -heur-stack-bound -heur-proc-map -internal-checking'
gmake -s check || exit 1

export MUDFLAP_OPTIONS='-mode-nop'
gmake distcheck -s || exit 1
