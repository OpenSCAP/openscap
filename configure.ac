define([AC_INIT_NOTICE],
[### Generated automatically using autoconf version] AC_ACVERSION [
### Copyright 2009 Peter Vrabec <pvrabec@redhat.com>
###
### Permission is hereby granted, free of charge, to any person obtaining a
### copy of this software and associated documentation files (the "Software"),
### to deal in the Software without restriction, including without limitation
### the rights to use, copy, modify, merge, publish, distribute, sublicense,
### and/or sell copies of the Software, and to permit persons to whom the
### Software is furnished to do so, subject to the following conditions:
###
### The above copyright notice and this permission notice shall be included
### in all copies or substantial portions of the Software.
###
### THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
### IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
### FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
### THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
### OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
### ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
### OTHER DEALINGS IN THE SOFTWARE.
###
### For usage, run `./configure --help'
### For more detailed information on installation, read the file `INSTALL'.
###
### If configuration succeeds, status is in the file `config.status'.
### A log of configuration tests is in `config.log'.
])

AC_INIT([openscap], [0.1.1], [open-scap-list@redhat.com])
AC_CONFIG_HEADERS([config.h])

echo Configuring openscap $VERSION

AM_INIT_AUTOMAKE([foreign])
AM_PROG_LIBTOOL
AM_PATH_PYTHON

echo .
echo Checking for programs

AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_AWK

echo .
echo Checking for header files

AC_HEADER_STDC

AC_CONFIG_FILES([Makefile lib/Makefile swig/Makefile examples/Makefile])
AC_OUTPUT

echo .
echo "
  Openscap Version:       $VERSION
  Target:                 $target
  Installation prefix:    $prefix
  Compiler:               $CC
  Compiler flags:         `echo $CFLAGS | fmt -w 50 | sed 's,^,,'`
"

