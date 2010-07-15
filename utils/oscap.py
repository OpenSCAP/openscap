from openscap_api import *
import sys, getopt, string

VERBOSE = 0

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
        self.sess         = oval.agent.new_session(self.def_model)
        self.policy_model = xccdf.policy_model(self.benchmark)
        self.__set_policy(self.action.profile)

    def __get_opts(self, arguments):

        action = OSCAP_Action()
        if string.lower(arguments[0]) == "eval":
            setattr(self, "eval", self.__evaluate)
        elif string.lower(arguments[0]) == "validate-xml":
            setattr(self, "eval", self.__validate_xml)
        else: raise AttributeError("Bad XCCDF operation '%s'" % (arguments[0],))

        try:
            opts, args = getopt.getopt(arguments[1:], "h", ["help", "result-file=", "xccdf-profile=", "file-version="])
        except getopt.GetoptError, err:
            # print help information and exit:
            print str(err) # will print something like "option -a not recognized"
            sys.exit(2)
        for opt, arg in opts:
            if opt in ("-h", "--help"):
                pass
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
        result = oscap.reporter_message_get_user2num(msg)
        print "%s" % (oscap.reporter_message_get_string(msg))
        return True

    def __set_policy(self, policy_id):
        if policy_id == None: 
            self.policy = self.policy_model.policies.next()
            return

        self.policy = None
        policy_it = self.policy_model.policies
        while policy_it.has_more():
            policy = policy_it.next()
            if policy.id == policy_id:
                self.policy = policy
        if self.policy == None: raise AttributeError("No policy with '%s' profile id in Policy Model" % (policy_id,))

    def export(self, result=None):
        assert self.action.f_results != None, "Result file not specified"
        if result == None: result = self.result
        result.benchmark_uri = self.action.url_xccdf
        title = oscap.text()
        title.text = "OSCAP Python Test Result"
        result.title = title

        id = self.policy.profile.id
        if id != None:
            result.profile = id

        oval.agent_export_sysinfo_to_xccdf_result(self.sess, result)

        model_it = self.benchmark.models
        while model_it.has_more():
            result.score = self.policy.score(result, model_it.next().system)
        model_it.free()

        return result.export(self.action.f_results)

    def __evaluate(self, policy_id=None):
        if policy_id != None: self.__set_policy(policy_id)
        self.policy_model.register_output_callback(self.__callback, None)
        self.policy_model.register_engine_oval(self.sess)
        self.result = self.policy.evaluate()

        return self.result

    def __validate_xml(self):
        raise NotImplementedError

class OVAL_Handler(object):

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
        else: raise AttributeError("Bad OVAL operation '%s'" % (arguments[0],))

        try:
            opts, args = getopt.getopt(arguments[1:], "h", ["help", "result-file=", "file-version=", "--definitions", "--syschar", "--results"])
        except getopt.GetoptError, err:
            # print help information and exit:
            print str(err) # will print something like "option -a not recognized"
            sys.exit(2)
        for opt, arg in opts:
            if opt in ("-h", "--help"):
                pass
            if opt == "--result-file":
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
        result = oscap.reporter_message_get_user2num(msg)
        print "%s" % (oscap.reporter_message_get_string(msg))
        return True

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
        res_direct.free()


def main():
    arguments = sys.argv[1:]
    try:
        opts, args = getopt.getopt(arguments, "+qhV", ["quiet", "help", "version"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        sys.exit(2)
    for o, a in opts:
        if o == "-V":
            pass
            #print version
        elif o in ("-h", "--help"):
            pass
            #print help
            #sys.exit(0)
        elif o in ("-q", "--quiet"):
            VERBOSE = -1
        else:
            assert False, "unhandled option"
    # In args we have all arguments left after opts processing
    if string.lower(args[0]) == "xccdf":
        module = XCCDF_Handler(args[1:])
    elif string.lower(args[0]) == "oval":
        module = OVAL_Handler(args[1:])
    else: raise AttributeError("Bad module option '%s'" %(args[0],))

    module.eval()
    module.export()
    
if __name__ == "__main__":
    main()
