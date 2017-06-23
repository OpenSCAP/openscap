#!/usr/bin/env bash

file=$1
if [[ -z "$file" ]]; then
    echo "Usage: clang_analyzer.sh ../lib/vasnprintf.c"
    exit
fi
func=$2
if [[ -z "$func" ]]; then
    echo "Usage: clang_analyzer.sh ../lib/vasnprintf.c decode_double"
fi
clang_version=$3
if [[ -z "$clang_version" ]]; then
    llvm_version=$(llvm-config --version)
    clang_version="${llvm_version/svn/}"
fi

cmd="clang -cc1 -analyze -disable-free -disable-llvm-verifier \
    -discard-value-names -main-file-name $file \
    -analyzer-store=region \
    -analyzer-opt-analyze-nested-blocks \
    -analyzer-eagerly-assume \
    -analyzer-checker=core \
    -analyzer-checker=apiModeling \
    -analyzer-checker=unix \
    -analyzer-checker=deadcode \
    -analyzer-checker=security.insecureAPI.UncheckedReturn \
    -analyzer-checker=security.insecureAPI.getpw \
    -analyzer-checker=security.insecureAPI.gets \
    -analyzer-checker=security.insecureAPI.mktemp \
    -analyzer-checker=security.insecureAPI.mkstemp \
    -analyzer-checker=security.insecureAPI.vfork \
    -analyzer-checker=nullability.NullPassedToNonnull \
    -analyzer-checker=nullability.NullReturnedFromNonnull \
    -analyzer-output plist -w -mrelocation-model pic -pic-level 2 \
    -mthread-model posix -fmath-errno -masm-verbose -mconstructor-aliases \
    -munwind-tables -fuse-init-array -target-cpu x86-64 \
    -momit-leaf-frame-pointer -dwarf-column-info -debugger-tuning=gdb \
    -resource-dir /usr/lib/clang/$clang_version -D HAVE_CONFIG_H -I .. -I ../.. \
    -I /usr/include/libxml2 -I ./public -I ../../src -I ../../src/common/public \
    -I ../../src/source/public -I ../../src/XCCDF_POLICY/public \
    -I ../../src/XCCDF/public -I ../../src/CPE/public -D _GNU_SOURCE \
    -D OSCAP_THREAD_SAFE -D _POSIX_C_SOURCE=200112L -D PIC \
    -internal-isystem /usr/local/include \
    -internal-isystem /usr/lib/clang/$clang_version/include \
    -internal-externc-isystem /include -internal-externc-isystem /usr/include \
    -O2 -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-function \
    -std=c99 -ferror-limit 19 -fmessage-length 0 -fobjc-runtime=gcc \
    -fdiagnostics-show-option -vectorize-loops -vectorize-slp \
    -analyzer-display-progress -analyzer-output=text -x c $file"
if [[ -n "$func" ]]; then
    cmd+="-analyze-function $func"
fi

echo $cmd
echo "========================================================================"
$cmd
