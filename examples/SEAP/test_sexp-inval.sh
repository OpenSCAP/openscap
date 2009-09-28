#!/bin/sh
PROG="./sexp_parser"

while read args; do
    ${PROG} "$args" > /dev/null 2>&1
    if [ $? -ne 1 ]; then
    	exit 1
    fi
done <<EOF
18446744073709551616
111111111111111111111
1e
11e
111e
1111e
11111e
111111e
1111111e
11111111e
111111111e
11111111111111111111111111111111111111111e
-1e
-11e
-111e
-1111e
-11111e
-111111e
-1111111e
-11111111e
+1e
+11e
+111e
+1111ee
+11111e
+111111e
+1111111e
+11111111e
+111111111e
.12e1.2
3.-12
+12.1-1
0.12.3
+0.1.23
-0.12.3
.123.
+.123.
-.123.
.0.
1..
-1.e
+1.e
12.4e
-12.4e
+12.3e
1e2.4
1e-2.4
1e+2.4
12e1.4
12e-1.4
12e+1.4
1e12.4
1e-12.4
1e+12.4
1E2.4
1E-2.4
1E+2.4
12E1.4
12E-1.4
12E+1.4
1E12.4
1E-12.4
1E+12.4
123.3e3.4
-12.3e3.4
+12.3e3.4
123.3e-3.4
123.3e+3.4
-12.3e-3.4
+12.3e+3.4
(a
(aa
(aaa
(aaaa
(aaaaa
(aaaaaa
(aaaaaaa
(aaaaaaaa
"(a b c)
"(aa b cc)
aaa b c)
aa bb b)
a 1 2)
1 a) 2
1 2 (a
'simple
'"test"
[INT8123
[UINT8]123)
[url]http://example.com"
[simple string])hello
#40414243
#4041424#)
#4#)
)#4#
#akjsd#
#234afg#
(#404#
[hex])#414243#
|TWFu|)
([base64]))|TWFu|
|TW9ua2V5Cg==|))
|TW9ua2V5Cg|(
|YmFzZTY0KE1vbmtleSkgPSB8VFc5dWEyVjVDZ3wK|)
(msg :id 123 (test "abcd") 123 [url]"http://www.example.com"))
(msg (:id 123 :hash [md5]|PNeg23b/ncpIl54kw5tAjA==| (test 123 "asdf" [wtf]"dlskflskdf"))
(msg :id 123(test 123 "asdf "bla"))
(msg :id 123(test 123"asdf""bla))
EOF

exit 0
