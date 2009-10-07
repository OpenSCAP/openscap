#!/bin/sh
# Cleanup first
gmake -s clean distclean
./autogen.sh || exit 1
CONFIGURE_ARGS="--disable-bindings"

# mudflap
echo '***************** CHECK: mudflap *******************'

export CFLAGS="-pthread -fmudflapth -lmudflapth -g -O1"
export MUDFLAP_OPTIONS='-mode-nop'

./configure $CONFIGURE_ARGS && gmake -s || exit 1

export MUDFLAP_OPTIONS='-mode-check -viol-nop -check-initialization -heur-start-end -heur-stack-bound -heur-proc-map -internal-checking'

gmake -s check || exit 1
gmake -s clean
