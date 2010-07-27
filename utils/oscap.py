# OVAL simple application for evaluating OVAL definition file.
# The output is system characteristic xml file and oval results xml file.
# Program iterate through tests in every definition and evaluate objects
# of these tests.
#
# Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
#      Maros Barabas  <mbarabas@redhat.com>

from openscap_api import *
import sys, getopt, string

VERBOSE = 0
LINE_LEN = 80

COLORS = {  '':'', "black":"30", "red":"31", "green":"32", "brown":"33", "blue":"34",
        "purple":"35", "cyan":"36", "lgray":"37", "gray":"1;30", "lred":"1;31",
        "lgreen":"1;32", "yellow":"1;33", "lblue":"1;34", "pink":"1;35",
        "lcyan":"1;36", "white":"1;37" }

XCCDF_RESULT = ["", "green", "red", "lred", "gray", "white", "white", "white", "blue", "yellow"]
OVAL_RESULT = {0:"", 1:"green", 2:"red", 4:"gray", 8:"lred", 16:"white", 32:"white"}

def _color(color, text):
    return "\033[%sm%s\033[0m" % (COLORS[color], text)

def _bold(text):
    return "\033[1m%s\033[0;0m" % text

def print_save(prefix, indent, string):

    if len(string) < LINE_LEN:
        print prefix, indent, string
    else:
        first = 0
        last_word_len = len(string.split()[-1])
        for n in range(len(string)/LINE_LEN+1):
            last = string[first+LINE_LEN:].find(" ") + first+LINE_LEN
            if last > (len(string)-last_word_len):
                last = len(string)
            s = string[first:last]
            first = last+1
            if len(s) > 0:
                print indent, s

class OSCAP_Action(object):

    def __init__(self):
        self.f_xccdf = None
        self.f_oval = None
        self.f_results = None
        self.url_xccdf = None
        self.url_oval = None
        self.profile = None
        self.cvss_metrics = None
        self.file_version = None
        self.doctype = None


class XCCDF_Handler(object):

    HINT = {"eval" : "Eval: Iterate throught XCCDF Rules and evaluate specified XCCDF Profile upon these rules",
            "validate-xml" : "Validate XCCDF Benchmark file" }

    """
    Init XCCDF Handler with specified parameters
    @param f_oval OVAL Definition file (mandatory)
    @param f_xccdf XCCDF Content file (mandatory)
    @param f_results XCCDF Result file (optional)
    @param profile ID of XCCDF Profile that will be evaluated (optional)
    """
    def __init__(self, args):
 
        self.action = self.__get_opts(args)
        if self.action == None: return

        self.def_model    = oval.definition_model_import(self.action.f_oval)
        self.benchmark    = xccdf.benchmark_import(self.action.f_xccdf)
        assert self.benchmark.instance != None, "Benchmark loading failed: %s" % (self.action.f_xccdf,)
        self.sess         = oval.agent.new_session(self.def_model)
        self.policy_model = xccdf.policy_model(self.benchmark)
        self.__set_policy(self.action.profile)

    def __get_opts(self, arguments):

        action = OSCAP_Action()
        if string.lower(arguments[0]) == "eval":
            setattr(self, "eval", self.__evaluate)
        elif string.lower(arguments[0]) == "validate-xml":
            setattr(self, "eval", self.__validate_xml)
        elif string.lower(arguments[0]) in ("-h", "--help"):
            self.help()
            sys.exit(0)
        else: raise AttributeError("Bad XCCDF operation '%s'" % (arguments[0],))

        try:
            opts, args = getopt.getopt(arguments[1:], "-h", ["help", "result-file=", "xccdf-profile=", "file-version="])
        except getopt.GetoptError, err:
            # print help information and exit:
            print str(err) # will print something like "option -a not recognized"
            sys.exit(2)
        for opt, arg in opts:
            if opt in ("-h", "--help"):
                if XCCDF_Handler.HINT.has_key(arguments[0]): self.help(XCCDF_Handler.HINT[arguments[0]])
                else: self.help("No help for option \"xccdf %s\"" % (arguments[0],))
                sys.exit(0)
            elif opt == "--result-file":
                action.f_results = arg
            elif opt == "--xccdf-profile":
                action.profile = arg
            elif opt == "--file-version":
                action.file_version = arg
            else:
                assert False, "unhandled option %s" % (opt,)

        if len(args) != 2: raise AttributeError("Need exactly 2 mandatory parameters: XCCDF file and OVAL file (%s given)" %(len(args)))
        action.f_oval = args[0]
        action.f_xccdf = args[1]
        # TODO Check files

        return action

    def __callback(self, msg, plugin):
        result = msg.user2num
        print_save( "\nRule ID:", "\r\t\t", _bold(msg.user1str) )
        print_save( "Title:", "\r\t\t", msg.user3str )
        print_save( "Descroption:", "\r\t\t", msg.string )
        print_save( "Result:", "\r\t\t", _bold(_color(XCCDF_RESULT[result], xccdf.test_result_type.get_text(msg.user2num))) )
        return 0

    def __set_policy(self, policy_id):
        if policy_id == None: 
            self.policy = self.policy_model.policies.next()
            return

        self.policy = None
        policy_list = self.policy_model.policies
        for policy in policy_list:
            if policy.id == policy_id:
                self.policy = policy
        if self.policy == None: raise AttributeError("No policy with '%s' profile id in Policy Model" % (policy_id,))

    def export(self, result=None):
        assert self.action.f_results != None, "Result file not specified"
        if result == None: result = self.result
        result.benchmark_uri = self.action.url_xccdf
        title = common.text()
        title.text = "OSCAP Python Test Result"
        result.title = title

        id = self.policy.id
        if id != None:
            result.profile = id

        oval.agent_export_sysinfo_to_xccdf_result(self.sess, result)

        model_list = self.benchmark.models
        for model in  model_list:
            result.score = self.policy.score(result, model.system)

        return result.export(self.action.f_results)

    def __evaluate(self, policy_id=None):
        if policy_id != None: self.__set_policy(policy_id)
        self.policy_model.register_output_callback(self.__callback, None)
        self.policy_model.register_engine_oval(self.sess)
        self.result = self.policy.evaluate()

        return self.result

    def __validate_xml(self):
        raise NotImplementedError

    def help(self, msg=None):

        print "Usage: %s [general-options] xccdf command [command-options] OVAL-DEFINITIONS-FILE XCCDF-FILE \n" \
		"(Specify the --help global option for a list of other help options)\n" \
		"\n" \
		"OVAL-DEFINITIONS-FILE is the OVAL XML file specified either by the full path to the xml file " \
		"or an URL from which to download it.\n" \
		"XCCDF-FILE is the XCCDF XML file specified either by the full path to the xml file " \
		"or an URL from which to download it.\n" \
		"\n" \
		"Commands:\n" \
		"   eval\r\t\t\t\t - Perform evaluation driven by XCCDF file and use OVAL as checking engine.\n" \
		"   validate-xml\r\t\t\t\t - validate XCCDF XML content.\n" \
		"\n" \
		"Command options:\n" \
		"   -h --help\r\t\t\t\t - show this help\n" \
		"   --result-file <file>\r\t\t\t\t - Write XCCDF Results into file.\n" \
		"   --profile <name>\r\t\t\t\t - The name of Profile to be evaluated.\n" % (sys.argv[0],)
        if msg != None:
            print "\n%s" % (msg,)


class OVAL_Handler(object):

    HINT = {"eval" : "Eval: Evaluate OVAL Definition file and create results",
            "validate-xml" : "Validate OVAL Definition file" }

    def __init__(self, args):
 
        self.action = self.__get_opts(args)
        if self.action == None: return
        
        self.def_model = oval.definition_model_import(self.action.f_oval)
        self.sys_model = oval.syschar_model(self.def_model)
        self.pb_sess   = oval.probe_session(self.sys_model)
        self.sess      = oval.agent.new_session(self.def_model)
        self.res_model = oval.agent.results_model(self.sess)

    def __get_opts(self, arguments):

        action = OSCAP_Action()
        if string.lower(arguments[0]) == "eval":
            setattr(self, "eval", self.__evaluate)
        elif string.lower(arguments[0]) == "collect":
            setattr(self, "eval", self.__collect)
        elif string.lower(arguments[0]) == "validate-xml":
            setattr(self, "eval", self.__validate_xml)
        elif string.lower(arguments[0]) in ("-h", "--help"):
            self.help()
            sys.exit(0)
        else: raise AttributeError("Bad OVAL operation '%s'" % (arguments[0],))

        try:
            opts, args = getopt.getopt(arguments[1:], "h", ["help", "result-file=", "file-version=", "--definitions", "--syschar", "--results"])
        except getopt.GetoptError, err:
            # print help information and exit:
            print str(err) # will print something like "option -a not recognized"
            sys.exit(2)
        for opt, arg in opts:
            if opt in ("-h", "--help"):
                if OVAL_Handler.HINT.has_key(arguments[0]): self.help(OVAL_Handler.HINT[arguments[0]])
                else: self.help("No help for option \"oval %s\"" % (arguments[0],))
                sys.exit(0)
            elif opt == "--result-file":
                action.f_results = arg
            elif opt == "--file-version":
                action.file_version = arg
            elif opt == "--definition":
                action.doctype = "definition"
            elif opt == "--syschar":
                action.doctype = "syschar"
            elif opt == "--results":
                action.doctype = "results"
            else:
                assert False, "unhandled option %s" % (opt,)

        if len(args) != 1: raise AttributeError("Need exactly 1 mandatory parameter: OVAL file (%s given)" %(len(args)))
        action.f_oval = args[0]
        # TODO Check files

        return action

    def __callback(self, msg, plugin):
        print "\nID: \r\t\t", _bold(msg.user1str)
        print_save( "Title:", "\r\t\t", msg.user3str )
        print_save( "Description:", "\r\t\t", msg.string )
        print "Result: \r\t\t", _bold(_color(OVAL_RESULT[msg.user2num], oval.result.get_text(msg.user2num)))
        return 0

    def __evaluate(self):
        self.result = oval.agent_eval_system(self.sess, self.__callback, None)

    def __collect(self):
        ret = self.pb_sess.query_sysinfo()
        if ret != 0: return -1
        ret = self.pb_sess.query_objects()
        if ret != 0: return -1

        self.sys_model.export("/dev/stdout")
        return 0

    def __validate_xml(self):
        raise NotImplementedError

    def export(self):
        res_direct = oval.result_directives(self.res_model)
        res_direct.set_reported(OSCAP.OVAL_RESULT_TRUE | OSCAP.OVAL_RESULT_FALSE | OSCAP.OVAL_RESULT_UNKNOWN | OSCAP.OVAL_RESULT_NOT_EVALUATED |
                              OSCAP.OVAL_RESULT_ERROR | OSCAP.OVAL_RESULT_NOT_APPLICABLE, True)
        res_direct.set_content(OSCAP.OVAL_RESULT_FALSE, OSCAP.OVAL_DIRECTIVE_CONTENT_FULL)
        res_direct.set_content(OSCAP.OVAL_RESULT_TRUE, OSCAP.OVAL_DIRECTIVE_CONTENT_FULL)
        self.res_model.export(res_direct, self.action.f_results)

    def help(self, msg=None):
        
        print "Usage: %s [general-options] oval command [command-options] OVAL-DEFINITIONS-FILE \n" \
                "(Specify the --help global option for a list of other help options)\n" \
                "\n" \
                "OVAL-DEFINITIONS-FILE is the OVAL XML file specified either by the full path to the xml file " \
                "or an URL from which to download it.\n" \
                "\n" \
                "Commands:\n" \
                "   collect\r\t\t\t\t - Probe the system and gather system characteristics for objects in OVAL Definition file.\n" \
                "   eval\r\t\t\t\t - Probe the system and evaluate all definitions from OVAL Definition file\n" \
                "   validate-xml\r\t\t\t\t - validate OVAL XML content.\n" \
                "\n" \
                "Command options:\n" \
                "   -h --help\r\t\t\t\t - show this help\n" \
                "   --result-file <file>\r\t\t\t\t - Write OVAL Results into file.\n" % (sys.argv[0],)
        if msg != None:
            print "\n%s" % (msg,)


class CVSS_Handler(object):

    def __init__(self, args):
        print "Sorry, CVSS module is not implemented yet. Please use compiled program utils/oscap instead."
        sys.exit(1)


def help():
    print "Usage: %s [general-options] module operation [operation-options-and-arguments]\n" \
            "\n" \
            "General options:\n" \
            "   -h --help\r\t\t\t\t - show this help\n" \
            "   -q --quiet\r\t\t\t\t - quiet mode\n" \
            "   -V --version\r\t\t\t\t - print info about supported SCAP versions\n" \
            "\n" "Module specific help\n" " %s [oval|xccdf|cvss] --help\n" % (sys.argv[0], sys.argv[0])

def versions():
	print "OSCAP util (oscap) 0.5.12\n","Copyright 2009,2010 Red Hat Inc., Durham, North Carolina.\n"
	print "OVAL Version: \r\t\t%s" % (oval.version)
	print "XCCDF Version: \r\t\t%s" % (xccdf.version)
	print "CVSS Version: \r\t\t%s" % (cvss.version)

def main():
    if len(sys.argv) < 2:
        help()
        sys.exit(1)

    arguments = sys.argv[1:]
    try:
        opts, args = getopt.getopt(arguments, "+qhV", ["quiet", "help", "version"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        sys.exit(2)
    for o, a in opts:
        if o in ("-V", "--version"):
            versions()
            sys.exit(0)
            #print version
        elif o in ("-h", "--help"):
            help()
            sys.exit(0)
        elif o in ("-q", "--quiet"):
            VERBOSE = -1
        else:
            assert False, "unhandled option"
    # In args we have all arguments left after opts processing
    if string.lower(args[0]) == "xccdf":
        module = XCCDF_Handler(args[1:])
    elif string.lower(args[0]) == "oval":
        module = OVAL_Handler(args[1:])
    elif string.lower(args[0]) == "cvss":
        module = CVSS_Handler(args[1:])
    else: raise AttributeError("Bad module option '%s'" %(args[0],))

    module.eval()
    if module.action.f_results != None: module.export()
    
if __name__ == "__main__":
    main()
