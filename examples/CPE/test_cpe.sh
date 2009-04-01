#!/bin/sh

#./test_cpeuri
#./test_cpelang
./test_cpedict CPE/dict.xml > /dev/null &&
./test_cpedict CPE/dict.xml 'cpe:/a:3com:3c15100d' > /dev/null &&
./test_cpelang CPE/lang.xml > /dev/null
