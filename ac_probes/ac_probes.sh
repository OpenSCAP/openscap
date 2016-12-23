#!/usr/bin/env bash
#
# Generate autoconf probe stuff
#

if [[ -z "$1" || -z "$2" || -z "$3" ]]; then
    echo "Usage: $(basename $0) <configure.ac template> <ac_probes_dir> <probe_src_dir>"
    exit 1
fi

TEMPLATE="$1"
AC_PROBES_DIR="$2"
PROBE_SRCDIR="$3"

TEMPLATE_DECL_SECTION="@@@@PROBE_DECL@@@@"
TEMPLATE_HEADER_SECTION="@@@@PROBE_HEADERS@@@@"
TEMPLATE_LIBRARY_SECTION="@@@@PROBE_LIBRARIES@@@@"
TEMPLATE_TABLE_SECTION="@@@@PROBE_TABLE@@@@"
TEMPLATE_EVAL_SECTION="@@@@PROBE_EVAL@@@@"

TEMPDIR="$(mktemp -d)"
HEADERS_INTERNAL=(
    alloc.h
    bfind.h
    config.h
    probe/[a-zA-Z]*.h
    common/assume.h
    common/bfind.h
    common/debug_priv.h
    crapi/crapi.h
    cstdio
    cstring
    iostream
    oval_fts.h
    probe-api.h
    procfs.h
    seap.h
    sexp.h
    pcre.h
    regex.h)

SOURCES_REGEXP='(probe_.*_SOURCES|/.*\.[Cch][a-zA-Z]*\\?$)'
PROBES_SEDEXP='s|^.*probe_\(.*\)_SOURCES.*$|\1|p'
HEADER_SEDEXP='s|^.*include.*<[[:space:]]*\([^>]*\)[[:space:]]*>.*$|\1|p'

HEADER_OPT_START='^[[:space:]]*#[[:space:]]*[ie][lf].*[Hh][Aa][Vv][Ee].*$'
HEADER_OPT_END='^[[:space:]]*#[[:space:]]*e[ln].*$'

HEADER_OPT_SEDEXP="/${HEADER_OPT_START}/,/${HEADER_OPT_END}/ ${HEADER_SEDEXP}"

function ac_gen_probe_decl() {
    local name="$1"

    echo "probe_${name}_req_deps_ok=yes"
    echo "probe_${name}_req_deps_missing="
    echo "probe_${name}_opt_deps_ok=yes"
    echo "probe_${name}_opt_deps_missing="
}

#
# ac_gen_headerscheck <name> <yes/no> <headers>
#
function ac_gen_probe_headercheck() {
    local name="$1"
    local required="$2"
    local headers="$3"

    if [[ -z "$headers" ]]; then
	return
    fi

    #
    # AC_CHECK_HEADERS (header-file..., [action-if-found], [action-if-not-found], [includes])
    #
    if [[ "$required" == "yes" ]]; then
	d="required"
    else
	d="optional"
    fi

    echo "echo"
    echo "echo ' * Checking presence of $d headers for the $name probe'"

    if [[ "$required" == "yes" ]]; then
	echo "AC_CHECK_HEADERS([${headers}],[],[probe_${name}_req_deps_ok=no; probe_${name}_req_deps_missing='header files'],[-])"
    else
	echo "AC_CHECK_HEADERS([${headers}],[],[probe_${name}_opt_deps_ok=no],[-])"
    fi
    echo
}

function ac_gen_probe_librarycheck() {
    local libinfodir="$1"
    local cflagsfile="$2"

    for libname in $(ls -1 "$libinfodir" | grep -v '~$'); do
	local name=
	local pkgconfig=
	local pkgconfig_name=
	local pkgconfig_minver="0.0"
	local functions_req=
	local functions_opt=
	local functions_lang=
	local probes_req=
	local probes_opt=

	. "${libinfodir}/${libname}" # load library info

	if [[ -z "$name" ]]; then
	    continue
	fi

	echo "echo"
	echo "echo '* Checking for ${libname} library used by: ${probes_req[*]} ${probes_opt[*]}'"
	if [[ "${pkgconfig}" == "yes" ]]; then
	    #
            # PKG_CHECK_MODULES(prefix, list-of-modules, action-if-found, action-if-not-found)
	    #
	    echo "PKG_CHECK_MODULES([${libname}], [${pkgconfig_name} >= ${pkgconfig_minver}],[],["

	    # non-pkgconfig check - fallback
	    echo "SAVE_LIBS=\$LIBS"
	    echo "AC_SEARCH_LIBS([${functions_req[0]}],[${name}],["
	    echo "${libname}_CFLAGS=;"
	    echo "${libname}_LIBS=-l${name};"
	    echo "],["

	    for probe_name in ${probes_req[*]}; do
		echo "probe_${probe_name}_req_deps_ok=no;"
		echo "probe_${probe_name}_req_deps_missing+=', ${libname}';"
	    done

	    for probe_name in ${probes_opt[*]}; do
		echo "probe_${probe_name}_opt_deps_ok=no;"
		echo "probe_${probe_name}_opt_deps_missing+=', ${libname}';"
	    done

	    echo "],[])"
	    echo "AC_SUBST([${libname}_CFLAGS])"
	    echo "AC_SUBST([${libname}_LIBS])"
	    echo "LIBS=\$SAVE_LIBS"
	    echo "])"

	    #
            # Collect CFLAGS for header checks
	    #
	    pkg-config ${pkgconfig_name} --modversion > /dev/null || exit 123

	    echo -n "\$(pkg-config ${pkgconfig_name} --cflags) " >> "${cflagsfile}"
	else
	    # non-pkgconfig check
	    echo "SAVE_LIBS=\$LIBS"
	    echo "AC_SEARCH_LIBS([${functions_req[0]}],[${name}],["

	    echo "${libname}_CFLAGS=;"
	    echo "${libname}_LIBS=-l${name};"

	    echo "],["

	    for probe_name in ${probes_req[*]}; do
		echo "probe_${probe_name}_req_deps_ok=no;"
		echo "probe_${probe_name}_req_deps_missing+=', ${libname} lib';"
	    done

	    for probe_name in ${probes_opt[*]}; do
		echo "probe_${probe_name}_opt_deps_ok=no;"
		echo "probe_${probe_name}_opt_deps_missing+=', ${libname} lib';"
	    done

	    echo "],[])"

	    echo "AC_SUBST([${libname}_CFLAGS])"
	    echo "AC_SUBST([${libname}_LIBS])"

	    echo "LIBS=\$SAVE_LIBS"
	fi

	echo "SAVE_LIBS=\$LIBS"
	echo "LIBS=\$${libname}_LIBS"

	if [[ -n "${functions_req[0]}" ]]; then
	    if [[ -n "${functions_lang}" ]]; then
		echo "AC_LANG_PUSH([${functions_lang}])"
	    fi

	    echo -n "AC_CHECK_FUNCS([${functions_req[*]}], [], ["

	    for probe_name in ${probes_req[*]}; do
		echo
		echo "probe_${probe_name}_req_deps_ok=no;"
		echo "probe_${probe_name}_req_deps_missing+=\", \$ac_func func\";"
	    done

	    echo "])"

	    if [[ -n "${functions_lang}" ]]; then
		echo "AC_LANG_POP([${functions_lang}])"
	    fi
	fi

	if [[ -n "${functions_opt[0]}" ]]; then
	    if [[ -n "${functions_lang}" ]]; then
		echo "AC_LANG_PUSH([${functions_lang}])"
	    fi

	    echo "AC_CHECK_FUNCS([${functions_opt[*]}],[],[])"

	    if [[ -n "${functions_lang}" ]]; then
		echo "AC_LANG_POP([${functions_lang}])"
	    fi
	fi

	echo "LIBS=\$SAVE_LIBS"

	unset functions_opt
	unset functions_req
	unset probes_opt
	unset probes_req
    done
}

function ac_gen_probe_tableentry() {
    local name="$1"

    echo 'if test "$probe_'${name}'_req_deps_ok" = "yes"; then'
    echo '  probe_'${name}'_table_result="yes"'
    echo 'else'
    echo '  probe_'${name}'_table_result="NO (missing: $probe_'${name}'_req_deps_missing)"'
    echo 'fi'
    echo 'printf "  %-28s %s\n" "'${name}':" "$probe_'${name}'_table_result"'
}

function ac_gen_probe_compileeval() {
    local name="$1"
    echo "AM_CONDITIONAL([probe_${name}_enabled], test \"\$probe_${name}_req_deps_ok\" = yes)"
    echo "probe_${name}_enabled=\$probe_${name}_req_deps_ok"
}

function replace_pattern_with_file() {
    local pattern="$1"
    local repfile="$2"

    sed "/${pattern}/ {r ${repfile}
d}" $3
}

# Cleanup
rm -rf "${TEMPDIR}"
mkdir  "${TEMPDIR}"

cd "${PROBE_SRCDIR}" || exit 9

# Generate source file list for each probe from Makefile.am
# skip system_info because we always build system_info
grep -E "${SOURCES_REGEXP}" Makefile.am | \
    grep -v "probe_system_info" | \
    sed -e '{:q;N;s/\\\n//g;t q;/\\$/ b q;}' | \
    sed 's|^[[:space:]]*\(probe_.*SOURCES\)[[:space:]]*=[[:space:]]*\(.*\)[[:space:]]*$|export \1="\2"|' > "${TEMPDIR}/vars" || exit 1

sed  -n "${PROBES_SEDEXP}" "${TEMPDIR}/vars" > "${TEMPDIR}/names" || exit 2

# Load _SOURCES
. "${TEMPDIR}/vars"

# Make a list of excluded header files separated by '|' (and then construct a regexp from it)
OIFS=$IFS
export IFS="|"
EXCLUDE_HEADERS="${HEADERS_INTERNAL[*]}"
export IFS=$OIFS

echo > "${TEMPDIR}/ac_probes.check.out"
echo "echo '  === probes ==='" > "${TEMPDIR}/ac_probes.table.out"
echo > "${TEMPDIR}/ac_probes.decl.out"
echo > "${TEMPDIR}/ac_probes.eval.out"
echo > "${TEMPDIR}/ac_probes.check_cflags.out"

while read probe_name; do
    src_files=$(eval "echo \$probe_${probe_name}_SOURCES")

    # extract all header files
    sed -n "${HEADER_SEDEXP}" $src_files | grep -vE "($EXCLUDE_HEADERS)" | sort | uniq > "${TEMPDIR}/headers.${probe_name}"
    # extract optional header files
    sed -n "${HEADER_OPT_SEDEXP}" $src_files | grep -vE "($EXCLUDE_HEADERS)" | sort | uniq > "${TEMPDIR}/headers.${probe_name}.opt"
    # generate list of required header files (required = all - optional)
    diff --left-column "${TEMPDIR}/headers.${probe_name}.opt" "${TEMPDIR}/headers.${probe_name}" | sed -n 's|> \(.*\)$|\1|p' > "${TEMPDIR}/headers.${probe_name}.req"

    #
    # Generate autoconf code
    #

    # declaration
    ac_gen_probe_decl "$probe_name" >> "${TEMPDIR}/ac_probes.decl.out"

    # required header check
    ac_gen_probe_headercheck "$probe_name" "yes" "$(cat ${TEMPDIR}/headers.${probe_name}.req | tr '\n' ' ')" >> "${TEMPDIR}/ac_probes.check.out"

    # optional header check
    ac_gen_probe_headercheck "$probe_name" "no" "$(cat ${TEMPDIR}/headers.${probe_name}.opt | tr '\n' ' ')" >> "${TEMPDIR}/ac_probes.check.out"

    # table entry
    ac_gen_probe_tableentry "$probe_name" >> "${TEMPDIR}/ac_probes.table.out"

    # result evaluation
    ac_gen_probe_compileeval "$probe_name" >> "${TEMPDIR}/ac_probes.eval.out"

done <<EOF
`cat "${TEMPDIR}/names"`
EOF

cd - > /dev/null

# library check
ac_gen_probe_librarycheck "${AC_PROBES_DIR}/libs" "${TEMPDIR}/ac_probes.check_cflags.out" > "${TEMPDIR}/ac_probes.libs.out"

# regen check.out file and include CFLAGS handling
echo "SAVE_CPPFLAGS=\"\$CPPFLAGS\"" > "${TEMPDIR}/ac_probes.check.out2"
echo "CPPFLAGS=\"\$CPPFLAGS $(cat "${TEMPDIR}/ac_probes.check_cflags.out" | tr '\n' ' ')\"" >> "${TEMPDIR}/ac_probes.check.out2"
cat "${TEMPDIR}/ac_probes.check.out" >> "${TEMPDIR}/ac_probes.check.out2"
echo "CPPFLAGS=\"\$SAVE_CPPFLAGS\"" >> "${TEMPDIR}/ac_probes.check.out2"
mv "${TEMPDIR}/ac_probes.check.out2" "${TEMPDIR}/ac_probes.check.out"

# Generate configure.ac from the template
cat "${TEMPLATE}" |\
replace_pattern_with_file "${TEMPLATE_DECL_SECTION}"    "${TEMPDIR}/ac_probes.decl.out" |\
replace_pattern_with_file "${TEMPLATE_HEADER_SECTION}"  "${TEMPDIR}/ac_probes.check.out"|\
replace_pattern_with_file "${TEMPLATE_LIBRARY_SECTION}" "${TEMPDIR}/ac_probes.libs.out" |\
replace_pattern_with_file "${TEMPLATE_EVAL_SECTION}"    "${TEMPDIR}/ac_probes.eval.out" |\
replace_pattern_with_file "${TEMPLATE_TABLE_SECTION}"   "${TEMPDIR}/ac_probes.table.out"

rm -f "${TEMPDIR}"/* && rmdir "${TEMPDIR}"
