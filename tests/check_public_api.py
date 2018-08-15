#!/usr/bin/python3

import subprocess
import tempfile
import os
import re
import sys


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
    symbols = {line.split()[2] for line in nm_output.splitlines()}
    return symbols


def get_public_headers(prefix):
    """
    It is useful to check if all the public headers are actually installed
    under $PREFIX/include/openscap.
    :param prefix: CMAKE_INSTALL_PREFIX
    :return: a set of public headers filepaths
    """
    public_headers = set()
    include_path = os.path.join(prefix, "include", "openscap")
    for root, dirs, files in os.walk(include_path):
        for f in files:
            full_path = os.path.join(root, f)
            public_headers.add(full_path)
    return public_headers


def get_public_symbols_from_header(header_path):
    """
    Parses a header file to find symbols marked by OSCAP_API macro.
    :param header_path: path to the header file
    :return: a list of public symbols in the header file
    """
    symbols = set()
    prototype_regex = re.compile(r"^[ \t]*OSCAP_API[^;]*;", re.MULTILINE)
    with open(header_path, "r") as header_file:
        content = header_file.read()
    prototypes = prototype_regex.findall(content)
    function_name_regex = re.compile(r"([a-zA-Z0-9_]+)\s*\(.*\)")
    for p in prototypes:
        p = p.replace("\n", " ")
        f_match = function_name_regex.search(p)
        f_name = f_match.group(1) if f_match else "unknown"
        symbols.add(f_name)
    return symbols


def get_public_symbols_from_headers(headers):
    """
    Find public (exported) symbols in header files by finding symbols
    marked as OSCAP_API.
    :param headers: a list of public headers file paths
    :return: a set of public symbols in public headers
    """
    symbols = set()
    for h in headers:
        symbols.update(get_public_symbols_from_header(h))
    return symbols


def _run_command(command):
    try:
        subprocess.run(command, check=True,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                       encoding="utf-8")
    except subprocess.CalledProcessError as e:
        print(e.output)
        print(e.stderr, file=sys.stderr)
        raise RuntimeError


def analyze_project_artifacts(src_dir):
    """
    Analyze the project and find all the symbols declared as public in
    public header files and all the symbols actually exported in built
    library. This is done on a real build and a real installation.
    The artifacts are installed in a temporary directory which is
    automatically removed when this function returns.
    :param src_dir: Path to project sources
    :return: tuple (header_symbols, so_symbols)
    """
    cwd = os.getcwd()
    build_dir = os.path.normpath(os.path.join(src_dir, "build"))
    build_dir_contents = os.listdir(build_dir)
    if ".gitkeep" in build_dir_contents:
        build_dir_contents.remove(".gitkeep")
    if build_dir_contents:
        print("Directory '%s' is not empty" % build_dir, file=sys.stderr)
        raise RuntimeError
    os.chdir(build_dir)
    with tempfile.TemporaryDirectory() as prefix:
        cmake_command = ["cmake",
                         "-DENABLE_PYTHON3=FALSE",
                         "-DENABLE_PERL=FALSE",
                         "-DCMAKE_INSTALL_PREFIX=" + prefix,
                         "-DENABLE_TESTS=FALSE",
                         ".."]

        _run_command(cmake_command)
        make_command = ["make"]
        _run_command(make_command)
        make_docs_command = ["make", "docs"]
        _run_command(make_docs_command)
        make_install_command = ["make", "install"]
        _run_command(make_install_command)
        public_headers = get_public_headers(prefix)
        so_symbols = get_public_symbols_from_shared_object(prefix)
        header_symbols = get_public_symbols_from_headers(public_headers)
    os.chdir(cwd)
    return header_symbols, so_symbols


def main():
    src_dir = os.path.join(os.getcwd(), "..")

    try:
        header_symbols, so_symbols = analyze_project_artifacts(src_dir)
    except RuntimeError as e:
        print("Could not analyze OpenSCAP public API", file=sys.stderr)
        sys.exit(1)

    print("Shared object symbols: %d" % len(so_symbols))
    print("Public header symbols: %d\n" % len(header_symbols))
    print()

    so_only = so_symbols.difference(header_symbols)
    print("The following %d symbols are exported in binary, "
          "but are not present in public header files:" % len(so_only))
    for s in sorted(so_only):
        print(s)
    print()

    header_only = header_symbols.difference(so_symbols)
    print("The following %d symbols are present in public header files, "
          "but are not exported in binary:" % len(header_only))
    for s in sorted(header_only):
        print(s)


if __name__ == "__main__":
    main()
