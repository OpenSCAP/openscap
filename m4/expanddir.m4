dnl Expand the variable $prefix to the full pathname, 
dnl so we don't end up with a useless string like "$prefix/"
AC_DEFUN([EXPAND_DIR], [
	$1=$2
	$1=`(
		test "x$prefix" = xNONE && prefix="$ac_default_prefix"
		test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
		eval echo \""[$]$1"\"
	    )`
	])


