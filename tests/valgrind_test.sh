#!/usr/bin/env bash

# Runs valgrind tests.
#
# Authors:
#      Jan Černý <jcerny@redhat.com>

# set up variables
oscap_program=$actualdir/utils/.libs/oscap
valgrind_output=/tmp/valgrind_$$.log
logfile=$actualdir/tests/valgrind_test.log
suppfile=$actualdir/tests/suppressions.supp
valgrind_args="--trace-children=yes --free-fill=55 --malloc-fill=55 --leak-check=full --show-reachable=yes --show-leak-kinds=all --log-file=$valgrind_output --suppressions=$suppfile"


echo "VALGRIND TEST" >> $logfile
echo "working directory: " `pwd` >> $logfile
echo "command: oscap $@" >> $logfile

# run valgrind
valgrind $valgrind_args $oscap_program "$@" 
ret_val=$?

# analyse the valgrind log
if cat $valgrind_output | grep "All heap blocks were freed -- no leaks are possible" >/dev/null ;then
  echo "OK" >> $logfile
else
  cat $valgrind_output | grep "definitely lost: 0 bytes in 0 blocks" >/dev/null
  definitely_lost=$?
  cat $valgrind_output | grep "indirectly lost: 0 bytes in 0 blocks" >/dev/null
  indirectly_lost=$?
  cat $valgrind_output | grep "possibly lost: 0 bytes in 0 blocks" >/dev/null
  possibly_lost=$?
  cat $valgrind_output | grep "still reachable: 0 bytes in 0 blocks" >/dev/null
  still_reachable=$?
  # all errors weren't suppressed
  if ! [ $definitely_lost -eq 0 -a $indirectly_lost -eq 0 -a $possibly_lost -eq 0 -a $still_reachable -eq 0 ]; then
    echo "Memory leak detected!" >> $logfile
    cat $valgrind_output | sed 's/==.*== //' | sed '/^$/d' >> $logfile
    ret_val=1 # fail the test
  else
    echo "OK" >> $logfile
  fi
fi

echo "" >> $logfile
rm $valgrind_output
exit $ret_val
