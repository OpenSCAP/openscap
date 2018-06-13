#!/usr/bin/python3

#
# Copyright 2011 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Authors:
#      "Martin Preisler" <mpreisle@redhat.com>
#

import argparse

parser = argparse.ArgumentParser(description = "Performs checks by given scripts and returns the result")
parser.add_argument("globs", metavar = "F", type = str, nargs = "+",
                   help = "File or glob of the check script to process")

args = parser.parse_args()

import glob
files = []
for g in args.globs:
    gfiles = glob.glob(g)
    files.extend(gfiles)
    if len(gfiles) == 0:
        print("No file matches '%s'" % (g))

import os

xccdf_result_map = dict()
xccdf_result_map["XCCDF_RESULT_PASS"] = 101
xccdf_result_map["XCCDF_RESULT_FAIL"] = 102
xccdf_result_map["XCCDF_RESULT_ERROR"] = 103
xccdf_result_map["XCCDF_RESULT_UNKNOWN"] = 104
xccdf_result_map["XCCDF_RESULT_NOT_APPLICABLE"] = 105
xccdf_result_map["XCCDF_RESULT_NOT_CHECKED"] = 106
xccdf_result_map["XCCDF_RESULT_NOT_SELECTED"] = 107
xccdf_result_map["XCCDF_RESULT_INFORMATIONAL"] = 108
xccdf_result_map["XCCDF_RESULT_FIXED"] = 109

xccdf_reverse_result_map = dict()
for key, value in xccdf_result_map.items():
    xccdf_reverse_result_map[value] = key

# set all XCCDF result types as environment variables
# (this is especially useful for bash scripts)
for key, value in xccdf_result_map.items():
    os.environ[key] = str(value)

import subprocess

for file_path in files:
    result = subprocess.call(["./%s" % (file_path)])
    if result in xccdf_reverse_result_map:
        result_text = xccdf_reverse_result_map[result]
    else:
        result_text = "Unknown exit code %i" % (result)
    print("Result of '%s' is %s" % (file_path, result_text))

