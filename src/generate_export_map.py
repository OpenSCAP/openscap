#!/usr/bin/python3

import os
import sys
import re
import argparse


def find_public_headers(srcdir):
    public_headers = []
    for dirpath, dirnames, filenames in os.walk(srcdir):
        if "SCE" in dirpath:
            continue
        if dirpath.endswith("/public"):
            for name in filenames:
                if name.endswith(".h"):
                    full_path = os.path.abspath(os.path.join(dirpath, name))
                    public_headers.append(full_path)
    return public_headers


def get_public_symbols_from_header(header_path):
    symbols = set()
    prototype_regex = re.compile(r"^[ \t]*OSCAP_API[^;]*;", re.MULTILINE)
    with open(header_path, "r") as header_file:
        content = header_file.read()
    prototypes = prototype_regex.findall(content)
    function_name_regex = re.compile(r"([a-zA-Z0-9_]+)\s*\(.*\)")
    data_name_regex = re.compile(r"\s+([a-zA-Z0-9_]+)\s*;")
    for p in prototypes:
        p = p.replace("\n", " ")
        match = function_name_regex.search(p)
        if not match:
            match = data_name_regex.search(p)
        if not match:
            print("Invalid prototype '%s'" % p, file=sys.stderr)
            continue
        symbol_name = match.group(1)
        symbols.add(symbol_name)
    return symbols


def get_all_public_symbols(srcdir):
    symbols = set()
    public_headers = find_public_headers(srcdir)
    for header in public_headers:
        symbols.update(get_public_symbols_from_header(header))
    return sorted(symbols)


def generate_linux_export_map(symbols, file):
    prolog = "{\n" \
             "    global:\n"
    file.write(prolog)
    for s in symbols:
        file.write("        %s;\n" % s)
    epilog = "    local: *;\n" \
             "};\n"
    file.write(epilog)


def generate_osx_export_map(symbols, file):
    for s in symbols:
        file.write("_%s\n" % s)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("srcdir",
                        help="Source code directory")
    parser.add_argument("--output",
                        type=argparse.FileType('w'),
                        default=sys.stdout,
                        help="Specify file path to write the export map")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--linux", action="store_true")
    group.add_argument("--osx", action="store_true")
    args = parser.parse_args()
    symbols = get_all_public_symbols(args.srcdir)
    if args.linux:
        generate_linux_export_map(symbols, args.output)
    elif args.osx:
        generate_osx_export_map(symbols, args.output)
