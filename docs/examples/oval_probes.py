#!/usr/bin/env python

# Author: Ed Sealing <eseaing@tresys.com>
#         Francisco Slavin <fslavin@tresys.com>
#
# Copyright (C) 2010 Tresys Technology, LLC
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


# Basic Python implementation of oval_probes.c
# Printing of function names was done to discover possible performance gaps.
#
# Usage:
#       ./oval_probes.py -d oval_definition.xml
# Output:
#       System Characteristics File
#       Results file


import sys, getopt, os
from platform import machine

# Determine location of libraries using platform and Python Versioning
pyVersion = str(sys.version_info[0])+'.'+str(sys.version_info[1])
if machine() == 'x86_64':
    libPath='/usr/local/lib64/python'+pyVersion+'/site-packages'
else:
    libPath='/usr/local/lib/python'+pyVersion+'/site-packages'
sys.path.append(libPath)

from openscap import *


def main():
    try:
        # Defining usage
        opts, args = getopt.getopt(sys.argv[1:], "d:r:h", 
                                    ["definition=", "results=", "help"])

    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)


    try:

        # Default input file & output directory
        def_file = ""
        results_dir = "./"

        # Handling arguments
        for opt, arg in opts:
            if   opt in ("-h", "--help"):
                usage()
                sys.exit()
            elif opt in ("-d", "--definition"):
                def_file = arg
            elif opt in ("-r", "--results"):
                results_dir = arg

        if results_dir[-1] != "/": results_dir = results_dir + "/"

        # Populate the definition model
        if def_file == "":
            print "You must select a definition file using the -d flag"
            usage()
            sys.exit(2)
        deffile = oscap_import_source_new_file(def_file, 'UTF-8')
        def_model = oval_definition_model_new()
        print "Importing Definition File to Model..."
        oval_definition_model_import(def_model, deffile, None)
        oscap_import_source_free(deffile)

        # Create the syschar model
        print "Creating System Characteristics Model..."
        sys_model = oval_syschar_model_new(def_model)

        # Call the probes
        print "Probing Objects..."
        oval_syschar_model_probe_objects(sys_model)

        # Report the syschars in an XML file
#        print "Exporting System Characteristics Model..."
        print "oval_syschar_model_get_syschars..."
        syschars = oval_syschar_model_get_syschars(sys_model)
        outfile = results_dir + os.path.split(def_file)[1] + "sysChars.xml"
        print "oscap_export_target_new_file..."
        syschar_out = oscap_export_target_new_file(outfile, 'UTF-8')
        print "oval_syschar_model_export..."
        oval_syschar_model_export(sys_model, syschar_out)
        print "oscap_export_target_free..."    
        oscap_export_target_free(syschar_out)


        # Create the results model
        print "Creating Results Model..."
        sys_models = [sys_model, None]
        print "oval_results_model_new..."
        res_model = oval_results_model_new(def_model, sys_models)

        # Set up directives
        print "Setting up Directives..."
        res_direct = oval_result_directives_new(res_model)
        oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE, 1)
        oval_result_directives_set_reported(res_direct, OVAL_RESULT_FALSE, 1)
        oval_result_directives_set_reported(res_direct, OVAL_RESULT_UNKNOWN, 1)
        oval_result_directives_set_reported(res_direct, OVAL_RESULT_ERROR, 1)
        oval_result_directives_set_reported(res_direct, OVAL_RESULT_NOT_EVALUATED, 1)
        oval_result_directives_set_reported(res_direct, OVAL_RESULT_NOT_APPLICABLE, 1)
        oval_result_directives_set_reported(res_direct, OVAL_RESULT_FALSE, OVAL_DIRECTIVE_CONTENT_FULL)
        oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE, OVAL_DIRECTIVE_CONTENT_FULL)

        # Report the results in an XML file
        print "Exporting Results File..."
        outfile = results_dir + os.path.split(def_file)[1] + "results.xml"
        print "oscap_export_target_new_file..."
        result_out = oscap_export_target_new_file(outfile, 'UTF-8')
        print "oval_results_model_export..."
        oval_results_model_export(res_model, res_direct, result_out)
        print "oscap_export_target_free"
        oscap_export_target_free(result_out)

        oval_definition_model_free(def_model)
        oval_syschar_model_free(sys_model)
        oval_results_model_free(res_model)    
        oval_result_directives_free(res_direct)    

    except Exception, err:
        print str(err)
        sys.exit(2)


# Instructions
def usage():
    print "Usage:\n \
    -d  --definition - set a specific OVAL definition file \n \
    -r  --result     - set a specific directory for results"



if __name__ == "__main__":
    main()
