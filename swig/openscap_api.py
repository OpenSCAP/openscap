# This file is a wrapper for python API for openscap
# library
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

DEBUG = 0

from sys import version_info
if version_info >= (2,6,0):
    def _import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_openscap_py', [dirname(__file__)])
        except ImportError:
            import _openscap_py as OSCAP
            return OSCAP
        if fp is not None:
            try:
                _mod = imp.load_module('_openscap_py', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    OSCAP = _import_helper()
    del _import_helper
else:
    import OSCAP

del version_info

# Abstract class of OSCAP Object
class OSCAP_Object(object):

    """ Called when the instance is created """
    def __init__(self, object, instance=None):
        dict.__setattr__(self, "object", object)
        if instance != None: dict.__setattr__(self, "instance", instance)

    @staticmethod
    def new(retobj):
        if type(retobj).__name__ == 'SwigPyObject':
            # If there is "struct <name> *":
            if retobj.__repr__().split("'")[1].split()[0] == "struct": structure = retobj.__repr__().split("'")[1].split()[1]
            # else there is "<name> ..."
            else: structure = retobj.__repr__().split("'")[1].split()[0]
            return OSCAP_Object(structure, retobj)
        else: return retobj

    def __repr__(self):
        return "<Oscap Object of type '%s' at %s>" % (self.object, hex(id(self)),)

    def __func_wrapper(self, func, value=None):

        def __getter_wrapper(*args, **kwargs):
            newargs = ()
            for arg in args:
                if isinstance(arg, OSCAP_Object):
                    newargs += (arg.instance,)
                else: newargs += (arg,)

            try:
                retobj = func()
            except TypeError as err:
                try:
                    retobj = func(self.instance)
                except TypeError as err:
                    try:
                        retobj = func(*newargs)
                    except TypeError as err:
                        try:
                            retobj = func(self.instance, *newargs)
                        except TypeError as err:
                            raise TypeError("Wrong number of arguments in function %s" % (func.__name__,))

            if retobj == None: return None
            return OSCAP_Object.new(retobj)
        
        return __getter_wrapper

    """ Called when an attribute lookup has not found the attribute in the usual places (i.e. 
        it is not an instance attribute nor is it found in the class tree for self). name is 
        the attribute name.
    """
    def __getattr__(self, name):
        if self.__dict__.has_key(name): 
            return self.__dict__[name]

        # If attribute is not in a local dictionary, look for it in a library
        func = OSCAP.__dict__.get(name)
        if func != None: return func

        obj = OSCAP.__dict__.get(self.object+"_"+name)
        if obj != None: 
            if callable(obj):
                return self.__func_wrapper(obj)

        obj = OSCAP.__dict__.get(self.object+"_get_"+name)
        if obj != None:
            try: return self.__func_wrapper(obj)()
            except: return self.__func_wrapper(obj)

        return OSCAP_Object(self.object+"_"+name)

    def __call__(self, *args, **kwargs):
        newargs = ()
        for arg in args:
            if isinstance(arg, OSCAP_Object):
                newargs += (arg.instance,)
            else: newargs += (arg,)

        # It's maybe looking for "new" ?
        obj = OSCAP.__dict__.get(self.object+"_new")
        if obj != None:
            return OSCAP_Object.new(obj(*newargs))
        else: raise NameError("name '"+self.object+"' is not defined")

    def __setattr__(self, name, value):
        if self.__dict__.has_key(name): 
            return self.__dict__[name]

        obj = OSCAP.__dict__.get(self.object+"_set_"+name)
        if obj == None:
            obj = OSCAP.__dict__.get(self.object+"_add_"+name) 
        if obj == None: 
            return None
        if isinstance(value, OSCAP_Object):
                    value = value.instance
        return obj(self.instance, value)

    def __del__(self):
        if self.__dict__.has_key("instance") and self.__dict__["instance"] != None:
            # In what situations we need to free objects ?
            if self.object.find("iterator") > -1 or \
                self.object.find("_model") > -1:

                self.free()

    def free(self):
        if self.__dict__.has_key("instance") and self.__dict__["instance"] != None:
            obj = OSCAP.__dict__.get(self.object+"_free")
            if obj != None:
                if callable(obj):
                    obj(self.__dict__["instance"])
                    dict.__setattr__(self, "instance", None)
            else: raise "Can't free %s" % (self.object,)

    """ ********* Implementation of non-trivial functions ********* """

    def __output_callback(self, msg, obj):
        return obj.__dict__["__output_cb"](OSCAP_Object("oscap_reporter_message", msg), obj.__dict__["__output_usr"])

    def register_output_callback(self, cb, usr):
        if self.object != "xccdf_policy_model": raise TypeError("Wrong call of register_output_callback function on %s" % (self.object,))
        dict.__setattr__(self, "__output_cb", cb)
        dict.__setattr__(self, "__output_usr", usr)
        return OSCAP.xccdf_policy_model_register_output_callback_py(self.instance, self.__output_callback, self)

    def register_engine_oval(self, sess):
        if self.object != "xccdf_policy_model": raise TypeError("Wrong call of register_engine_oval function on %s" % (self.object,))
        return OSCAP.xccdf_policy_model_register_engine_oval(self.instance, sess.instance)

    def agent_eval_system(self, sess, cb, usr):
        if self.object != "oval": raise TypeError("Wrong call of oval_agent_eval_system function on %s" % (self.object,))
        dict.__setattr__(self, "__output_cb", cb)
        dict.__setattr__(self, "__output_usr", usr)
        return OSCAP.oval_agent_eval_system_py(sess.instance, self.__output_callback, self)

    def query_sysinfo(self):
        if self.object != "oval_probe_session_t": raise TypeError("Wrong call of oval_probe_session_query_sysinfo function on %s" % (self.object,))
        return OSCAP.oval_probe_session_query_sysinfo(self.instance)

    def query_objects(self):
        if self.object != "oval_probe_session_t": raise TypeError("Wrong call of oval_probe_session_query_objects function on %s" % (self.object,))
        return OSCAP.oval_probe_session_query_objects(self.instance)

# ------------------------------------------------------------------------------------------------------------
# XCCDF

class _XCCDF_Benchmark_Class(OSCAP_Object):

    def __init__(self, path):
        #dict.__setattr__(self, "__name", "xccdf_benchmark")
        dict.__setattr__(self, "object", "xccdf_benchmark")
        dict.__setattr__(self, "instance", OSCAP.xccdf_benchmark_import(path))

    def __repr__(self):
        return "<Oscap Object of type 'XCCDF Benchmark' at %s>" % (hex(id(self)),)


class XCCDF_Class(OSCAP_Object):

    def __init__(self):
        dict.__setattr__(self, "object", "xccdf")
        dict.__setattr__(self, "version", OSCAP.xccdf_benchmark_supported())
        pass

    def __repr__(self):
        return "<Oscap Object of type 'XCCDF Class' at %s>" % (hex(id(self)),)

    """ Import XCCDF Benchmark 
    """
    def benchmark_import(self, path):
        return _XCCDF_Benchmark_Class(path)


# ------------------------------------------------------------------------------------------------------------
# OVAL

class OVAL_Class(OSCAP_Object):

    def __init__(self):
        dict.__setattr__(self, "object", "oval")
        dict.__setattr__(self, "version", OSCAP.oval_definition_model_supported())
        pass

    def __repr__(self):
        return "<Oscap Object of type 'OVAL Class' at %s>" % (hex(id(self)),)


# ------------------------------------------------------------------------------------------------------------
# CVE

class CVE_Class(OSCAP_Object):

    def __init__(self):
        dict.__setattr__(self, "object", "cve")
        dict.__setattr__(self, "version", OSCAP.cve_model_supported())
        pass

    def __repr__(self):
        return "<Oscap Object of type 'CVE Class' at %s>" % (hex(id(self)),)

# ------------------------------------------------------------------------------------------------------------
# CPE

class CPE_Class(OSCAP_Object):

    def __init__(self):
        dict.__setattr__(self, "object", "cpe")
        dict.__setattr__(self, "version", "CPE Lang: %s; CPE Dict: %s; CPE Name: %s" 
                % (OSCAP.cpe_lang_model_supported(), 
                    OSCAP.cpe_dict_model_supported(),
                    OSCAP.cpe_name_supported()))
        pass

    def __repr__(self):
        return "<Oscap Object of type 'CPE Class' at %s>" % (hex(id(self)),)

# ------------------------------------------------------------------------------------------------------------
# CVSS

class CVSS_Class(OSCAP_Object):

    def __init__(self):
        dict.__setattr__(self, "object", "cvss")
        dict.__setattr__(self, "version", OSCAP.cvss_model_supported())
        pass

    def __repr__(self):
        return "<Oscap Object of type 'CVSS Class' at %s>" % (hex(id(self)),)

# ------------------------------------------------------------------------------------------------------------
# CCE

class CCE_Class(OSCAP_Object):

    def __init__(self):
        dict.__setattr__(self, "object", "cce")
        dict.__setattr__(self, "version", OSCAP.cce_supported())
        pass

    def __repr__(self):
        return "<Oscap Object of type 'CCE Class' at %s>" % (hex(id(self)),)

# ------------------------------------------------------------------------------------------------------------

xccdf = XCCDF_Class()
oval  = OVAL_Class()
cve   = CVE_Class()
cce   = CCE_Class()
cpe   = CPE_Class()
cvss  = CVSS_Class()
common = OSCAP_Object("oscap")
