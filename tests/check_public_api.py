#!/usr/bin/python3

import subprocess
import tempfile
import os
import re


def get_public_symbols_from_shared_object(prefix):
    """
    Find public (exported) symbols in a shared library (*.so) by calling
    the 'nm' command.
    :param prefix: CMAKE_INSTALL_PREFIX
    :return: a set of public symbols
    """
    so_path = os.path.join(prefix, "lib64", "libopenscap.so")
    nm_command = ["nm", "--demangle", "--dynamic", "--defined-only", so_path]
    nm_output_bytes = subprocess.check_output(nm_command)
    nm_output = nm_output_bytes.decode(encoding="utf-8")
    symbols = [line.split()[2] for line in nm_output.splitlines()]
    return set(symbols)


def get_public_headers(prefix):
    """
    It is useful to check if all the public headers are actually installed under $PREFIX/include/openscap.
    :param prefix: CMAKE_INSTALL_PREFIX
    :return: a set of public headers file paths
    """
    public_headers = []
    include_path = os.path.join(prefix, "include", "openscap")
    for root, dirs, files in os.walk(include_path):
        for f in files:
            full_path = os.path.join(root, f)
            public_headers.append(full_path)
    return set(public_headers)


def _get_public_symbols_from_header(header_path):
    symbols = []
    prototype_regex = re.compile(r"^[ \t]*OSCAP_API[^;]*;", re.MULTILINE)
    with open(header_path, "r") as header_file:
        content = header_file.read()
    prototypes = prototype_regex.findall(content)
    function_name_regex = re.compile(r"([a-zA-Z0-9_]+)\s*\(.*\)")
    for p in prototypes:
        p = p.replace("\n", " ")
        f_match = function_name_regex.search(p)
        f_name = f_match.group(1) if f_match else "unknown"
        symbols.append(f_name)
    return symbols


def get_public_symbols_from_headers(headers):
    """
    Find public (exported) symbols in header files by finding symbols
    marked as OSCAP_API.
    :param headers: a list of public headers file paths
    :return: a set of public symbols of public headers file paths
    """
    symbols = []
    for h in headers:
        symbols.extend(_get_public_symbols_from_header(h))
    return set(symbols)


def analyze(src_dir):
    cwd = os.getcwd()
    #so_symbols = set()
    #header_symbols = set()
    with tempfile.TemporaryDirectory() as prefix:
        build_dir = os.path.join(src_dir, "build")
        os.chdir(build_dir)
        cmake_command = ["cmake",
                         "-DENABLE_PYTHON3=FALSE",
                         "-DENABLE_PERL=FALSE",
                         "-DCMAKE_INSTALL_PREFIX=" + prefix,
                         "-DENABLE_TESTS=FALSE",
                         ".."]
        subprocess.run(cmake_command)
        make_command = ["make"]
        subprocess.run(make_command)
        # TODO: remove make docs after https://github.com/OpenSCAP/openscap/issues/1161 is resolved
        make_docs_command = ["make", "docs"]
        subprocess.run(make_docs_command)
        make_install_command = ["make", "install"]
        subprocess.run(make_install_command)
        public_headers = get_public_headers(prefix)
        so_symbols = get_public_symbols_from_shared_object(prefix)
        header_symbols = get_public_symbols_from_headers(public_headers)
    os.chdir(cwd)
    return header_symbols, so_symbols


def main():
    src_dir = os.path.join(os.getcwd(), "..")
    header_symbols, so_symbols = analyze(src_dir)
    print("Shared object symbols: %d" % len(so_symbols))
    print("Public header symbols: %d" % len(header_symbols))
    print()
    so_only = so_symbols.difference(header_symbols)
    print("The following %d symbols are exported in binary, but are not present in public header files:" % len(so_only))
    for s in sorted(so_only):
        print(s)
    header_only = header_symbols.difference(so_symbols)
    print()
    print("The following %d symbols are present in public header files, but are not exported in binary:" % len(header_only))
    for s in sorted(header_only):
        print(s)


if __name__ == "__main__":
    main()