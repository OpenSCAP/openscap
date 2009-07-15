#!/bin/sh
PROG="./sexp_test"

RETN=1

while read args; do
    ${PROG} "$args"     > /dev/null 2>&1 || exit $RETN
    ${PROG} "($args)"   > /dev/null 2>&1 || exit $RETN
    ${PROG} "( $args)"  > /dev/null 2>&1 || exit $RETN
    ${PROG} "( $args )" > /dev/null 2>&1 || exit $RETN
    ${PROG} "($args )"  > /dev/null 2>&1 || exit $RETN
#   ${PROG} $args       > /dev/null 2>&1 || exit $RETN
done <<EOF
0
1
11
111
1111
11111
111111
1111111
11111111
111111111
1111111111
99999999999
18446744073709551615
9223372036854775807
9223372036854775808
-9223372036854775807
-9223372036854775808
4294967295
4294967296
2147483647
2147483648
-2147483647
-2147483648
65535
65536
131071
131072
-131071
-131072
255
256
127
128
-127
-128
-1
-11
-111
-1111
-11111
-111111
-1111111
-11111111
+1
+11
+111
+1111
+11111
+111111
+1111111
+11111111
+111111111
12
-12
+12
0.123
+0.123
-0.123
.123
+.123
-.123
.0
1.
-1.
+1.
12.
-12.
+12.
1e2
1e-2
1e+2
12e1
12e-1
12e+1
1e12
1e-12
1e+12
1E2
1E-2
1E+2
12E1
12E-1
12E+1
1E12
1E-12
1E+12
123.3e3
-12.3e3
+12.3e3
123.3e-3
123.3e+3
-12.3e-3
+12.3e+3
a
aa
aaa
aaaa
aaaaa
aaaaaa
aaaaaaa
aaaaaaaa
"(a b c)"
"(aa b cc)"
aaa b c
aa bb b
a 1 2
1 a 2
1 2 a
'simple'
'"test"'
[INT8]123
[UINT8]123
[url]"http://example.com"
[simple string]hello
#40414243#
#4041424#
#4#
#404#
[hex]#414243#
|TWFu|
[base64]|TWFu|
|TW9ua2V5Cg==|
|TW9ua2V5Cg|
|YmFzZTY0KE1vbmtleSkgPSB8VFc5dWEyVjVDZ3wK|
(msg :id 123 (test "abcd" 123 [url]"http://www.example.com"))
(msg :id 123 :hash [md5]|PNeg23b/ncpIl54kw5tAjA==| (test 123 "asdf" [wtf]"dlskflskdf"))
(msg :id 123(test 123 "asdf" "bla"))
(msg :id 123(test 123"asdf""bla"))
(msg)
("" "a" "aa" "aaa" "aaaa" "aaaaa")
(""("")"")
("asdf"("asdf")"asdf")
(m(m)m)
(a(a a)a)
(a())
((((((()))))))
()()()()()()()
()()()()()()
()()()()()
()()()()
()()
()
()(())((()))()
(1(2(3(4(5(6(7)))))))
[a]b
[a]b[c]d
[asdf](1 2 3 4)
[num]([int]1 [int]2 [char]'c' [string]"asdf")
1:a
3:abc
4:abc"
4:"abc
7:a c d e
1:a1:b1:c
2:ab2:cd
(1:a2:bc)
4|TWFu|
(4|TWFu|4|TWFu|)
4[type]4:abcd
(4[type]3:123)
4#4142#
4#4142#4#4142#
(4#4142#)
3#414#
(3#414#)
EOF
