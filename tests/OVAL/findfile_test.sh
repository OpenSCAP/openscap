#!/bin/sh
PROG="./test_findfile"

output=`mktemp`
tmpdir=`mktemp -d`
RTN=0;

#empty list
${PROG} "/dir_not_exit" "neither_file_does" "0" "both" "down" "all" 2>/dev/null > $output
echo "()" | diff --ignore-all-space - $output;
if [ $? -ne 0 ] 
then
	RTN=1; 
fi

#paths but no filenames
mkdir $tmpdir/1aaa $tmpdir/2aaa $tmpdir/3aaa
${PROG} "$tmpdir/*aaa/" "file_not_exist" "0" "both" "down" "all" 2>/dev/null > $output
echo "(
 (\"$tmpdir/1aaa/\")
 (\"$tmpdir/2aaa/\")
 (\"$tmpdir/3aaa/\"))" | diff --ignore-all-space - $output;
if [ $? -ne 0 ] 
then
	RTN=1; 
fi

#paths and one filename with recursion depth = 1 
touch $tmpdir/1aaa/hit
${PROG} "$tmpdir/*aaa/" "hit" "1" "both" "down" "all" 2>/dev/null > $output
echo "(
 (\"$tmpdir/1aaa/\" \"hit\")
 (\"$tmpdir/2aaa/\")
 (\"$tmpdir/3aaa/\"))" | diff --ignore-all-space - $output;
if [ $? -ne 0 ] 
then
	RTN=1; 
fi


rm  $output;
rm -rf $tmpdir;

exit $RTN;


