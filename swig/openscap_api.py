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

"""Python module for openscap implementing openscap API
"""
__author__ = 'Maros Barabas'
__version__ = '1.0'

import logging                  # Logger for debug/info/error messages
logger = logging.getLogger("openscap")

from sys import version_info
if version_info >= (2, 6, 0):
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
    import _openscap_py as OSCAP

del version_info

import os

class OSCAP_List(list):
    """OSCAP List class is designed to store lists generated from openscap iterators. All functions that return iterators
    are preprocessed by creation of OSCAP List instance and move all objects given by oscap list iteration loop to list.

    OSCAP List class implement standard Python list."""

    def remove(self, item):
        """Function to remove item from list. This removed item should be also removed from parent oscap list.
        This function is supported only if there exists reset function on iterators. Exception is throwed otherwise."""

        try:
            self.iterator.reset()
            while self.iterator.has_more():
                litem = self.iterator.next()
                if (type(item) == str and type(litem) == str and litem == item) or ("instance" in item.__dict__ and litem.instance == item.instance):
                    self.iterator.remove()
                    list.remove(self, item)
        except NameError:
            raise Exception("Removing %s items throught oscap list is not allowed. Please use appropriate function." \
                        % (self.iterator.object[:self.iterator.object.find("_iterator")],))

    def __del__(self):
        """Free the list structure"""
        self.iterator.free()


    def generate(self, iterator):
        """Generate all object from oscap list throught iterators and store them in list object.
        Do not call this function on your own !"""
        self.iterator = iterator

        while iterator.has_more():
            list.append(self, iterator.next())

    def append(self, item, n=1):
        """This function is not allowed. Please use appropriate function from library."""
        raise Exception("Append %s item throught oscap list is not allowed. Please use appropriate function." \
                        % (self.iterator.object[:self.iterator.object.find("_iterator")],))

    def extend(self, item, n=1):
        """This function is not allowed. Please use appropriate function from library."""
        raise Exception("Extending %s items throught oscap list is not allowed. Please use appropriate function." \
                        % (self.iterator.object[:self.iterator.object.find("_iterator")],))

    def insert(self, item, n=1):
        """This function is not allowed. Please use appropriate function from library."""
        raise Exception("Inserting %s items to oscap list is not allowed. Please use appropriate function." \
                        % (self.iterator.object[:self.iterator.object.find("_iterator")],))

    def sort(self, item, n=1):
        """This function is not allowed. Please use appropriate function from library."""
        raise Exception("Sorting %s items in oscap list is not allowed." \
                        % (self.iterator.object[:self.iterator.object.find("_iterator")],))

    def reverse(self, item, n=1):
        """This function is not allowed. Please use appropriate function from library."""
        raise Exception("Reversing %s items in oscap list is not allowed." \
                        % (self.iterator.object[:self.iterator.object.find("_iterator")],))


class OSCAP_Object(object):
    """
    Abstract class that represents all structures, functions and averything from
    openscap library. Each structure from library is mapped inside OSCAP Object
    with "object" and "instance" parameters.

    "object" is variable of this class that keeps string representation of
    type of the structure
    "instance" is a variable of this class that keeps the pointer to the real
    C structure.
    """

    def __init__(self, object, instance=None):
        """ Called when the instance is created """
        dict.__setattr__(self, "object", object)
        dict.__setattr__(self, "instance", instance)

    @staticmethod
    def new(retobj):
        if type(retobj).__name__ in ('SwigPyObject', 'PySwigObject'):
            # Extract the name of structure from "<num>_p_<name>"
            structure = retobj.__str__()[retobj.__str__().find("_p_")+3:]
            return OSCAP_Object(structure, retobj)
        else:
            return retobj

    def __eq__(self, other):
        """ Two OSCAP Objects are compared by their string representations
        which reflect type and instance.
        """
        return str.__eq__(self.__repr__(), other.__repr__())

    def __repr__(self):
        return "<Oscap Object of type '%s' with instance '%s'>" % (self.object, self.instance)

    def __func_wrapper(self, func, value=None):
        """ This is only a wrapper for getter_wrapper - another wrapper for
        openscap library functions.
        """

        def __getter_wrapper(*args, **kwargs):
            """ This function is a wrapper for function objects of openscap library.
            Each function is called with variable number of parameters cause we don't
            know how many parameters each function takes. This is based on try-except
            methot that we try to call the function and if it fell down we try another
            number of parameters.
            This is based on knowledge that C language will always
            cause error when the function is called with wrong number of parameters.
            """
            newargs = ()
            for arg in args:
                if isinstance(arg, OSCAP_Object):
                    newargs += (arg.instance,)
                else:
                    newargs += (arg,)

            try:
                retobj = func()
            except TypeError as err:
                try:
                    retobj = func(*newargs)
                except TypeError as err:
                    if self.instance:
                        try:
                            retobj = func(self.instance)
                        except TypeError as err:
                            try:
                                retobj = func(self.instance, *newargs)
                            except TypeError as err:
                                raise TypeError("Wrong number of arguments in function %s" % (func.__name__,))
                    else:
                        raise TypeError("%s: No instance or wrong number of parameters" % (func.__name__))

            if retobj is None:
                return None
            elif retobj.__str__().find("iterator") != -1:
                # We have an iterator here
                list = OSCAP_List()
                list.generate(OSCAP_Object.new(retobj))
                list.object = self.object
                return list
            return OSCAP_Object.new(retobj)

        return __getter_wrapper

    def __getattr__(self, name):
        """ Called when an attribute lookup has not found the attribute in the usual places (i.e.
        it is not an instance attribute nor is it found in the class tree for self). name is
        the attribute name."""

        if name == "export" and self.object == "xccdf_policy":
            return self.policy_export

        if name in self.__dict__:
            return self.__dict__[name]

        # If attribute is not in a local dictionary, look for it in a library
        func = OSCAP.__dict__.get(name)
        if func != None:
            return func

        """ Looking for function object_subject() """
        obj = OSCAP.__dict__.get(self.object+"_"+name)
        if obj != None:
            if callable(obj):
                return self.__func_wrapper(obj)

        """ Looking for function object_get_subject() """
        obj = OSCAP.__dict__.get(self.object+"_get_"+name)
        if obj != None:
            try:
                return self.__func_wrapper(obj)()
            except:
                return self.__func_wrapper(obj)

        """ There is not function with the name 'name' let return the OSCAP_Object
        This should return None, why is this here ? TODO
        """
        return OSCAP_Object(self.object+"_"+name)

    def __dir__(self):
        """Lists all attributes inside this object.

        This is mainly used by auto-completion and for dir(obj) in interactive prompt.
        (only available in Python 2.6 and newer but doesn't hurt anything in older Pythons)
        """

        ret = list()
        ret.extend(dir(type(self)))
        ret.extend(list(self.__dict__))

        # we intentionally don't add all functions from the library, having
        # them in getattr has IMO not been the right call, they would just
        # clutter everything...

        for key in OSCAP.__dict__.iterkeys():
            if key.startswith(self.object + "_"):
                # the getattr wrapper only deals with callables
                if callable(OSCAP.__dict__[key]):
                    ret.append(key[len(self.object) + 1:])

        # we also don't add the object_get_{name} methods as {name}, it IMO
        # only allows bugs to pass as working code

        return sorted(ret)

    def __call__(self, *args, **kwargs):
        newargs = ()
        for arg in args:
            if isinstance(arg, OSCAP_Object):
                newargs += (arg.instance,)
            else:
                newargs += (arg,)

        # It's maybe looking for "new" ?
        obj = OSCAP.__dict__.get(self.object+"_new")
        if obj != None:
            return OSCAP_Object.new(obj(*newargs))
        else:
            raise NameError("name '"+self.object+"' is not defined")

    def __setattr__(self, name, value):
        if name in self.__dict__:
            return self.__dict__[name]

        obj = OSCAP.__dict__.get(self.object+"_set_"+name)
        if obj is None:
            obj = OSCAP.__dict__.get(self.object+"_add_"+name)
        if obj is None:
            return None

        if isinstance(value, OSCAP_Object):
            value = value.instance
        return obj(self.instance, value)

    """
    def __del__(self):
        #print "Free ", self.object
        if "instance" in self.__dict__ and self.__dict__["instance"] != None:
            # In what situations we need to free objects ?
            if self.object.find("iterator") > -1:

                self.free()
    """

    def free(self):
        if self.object == "oval_agent_session":
            return OSCAP.oval_agent_destroy_session(self.instance)
        #print "Free on demand ", self.object
        if "instance" in self.__dict__ and self.__dict__["instance"] != None:
            obj = OSCAP.__dict__.get(self.object+"_free")
            if obj != None:
                if callable(obj):
                    obj(self.__dict__["instance"])
                    dict.__setattr__(self, "instance", None)
            else:
                raise Exception("Can't free %s" % (self.object,))

    """ ********* Implementation of non-trivial functions ********* """

    def __start_callback(self, rule, obj):
        return obj[0](OSCAP_Object("xccdf_rule", rule), obj[1])

    def __output_callback(self, rule_result, obj):
        return obj[0](OSCAP_Object("xccdf_rule_result", rule_result), obj[1])

    def register_start_callback(self, cb, usr):
        if self.object != "xccdf_policy_model":
            raise TypeError("Wrong call of register_start_callback function on %s" % (self.object,))
        return OSCAP.xccdf_policy_model_register_start_callback_py(self.instance, self.__start_callback, (cb, usr))

    def register_output_callback(self, cb, usr):
        if self.object != "xccdf_policy_model":
            raise TypeError("Wrong call of register_output_callback function on %s" % (self.object,))
        return OSCAP.xccdf_policy_model_register_output_callback_py(self.instance, self.__output_callback, (cb, usr))

    def register_engine_oval(self, sess):
        if self.object != "xccdf_policy_model":
            raise TypeError("Wrong call of register_engine_oval function on %s" % (self.object,))
        return OSCAP.xccdf_policy_model_register_engine_oval(self.instance, sess.instance)

    def register_engine_sce(self, parameters):
        if self.object != "xccdf_policy_model":
            raise TypeError("Wrong call of register_engine_sce function on %s" % (self.object,))
        return OSCAP.xccdf_policy_model_register_engine_sce(self.instance, parameters.instance)

    def agent_eval_system(self, sess, cb, usr):
        if self.object != "oval":
            raise TypeError("Wrong call of oval_agent_eval_system function on %s" % (self.object,))
        return OSCAP.oval_agent_eval_system_py(sess.instance, self.__output_callback, (cb, usr))

    def query_sysinfo(self):
        if self.object != "oval_probe_session_t":
            raise TypeError("Wrong call of oval_probe_session_query_sysinfo function on %s" % (self.object,))
        return OSCAP.oval_probe_session_query_sysinfo(self.instance)

    def query_objects(self):
        if self.object != "oval_probe_session_t":
            raise TypeError("Wrong call of oval_probe_session_query_objects function on %s" % (self.object,))
        return OSCAP.oval_probe_session_query_objects(self.instance)

    def validate_document(self, file, doctype, version, cb, usr):
        if self.object != "oscap":
            raise TypeError("Wrong call of validate_document function on %s" % (self.object,))
        return OSCAP.oscap_validate_document_py(file, doctype, version, self.__output_callback, (cb, usr))

    """ ********* Implementation of required high level functions ********* """

    def get_all_values(self):

        if self.object != "xccdf_item":
            item = self.to_item()
            if item is None:
                return None
        else:
            item = self
        values = []

        if item.type == OSCAP.XCCDF_BENCHMARK:
            values.extend(item.to_benchmark().values)
        elif item.type == OSCAP.XCCDF_GROUP:
            values.extend(item.to_group().values)
        else:
            return []

        for content in item.content:
            values.extend(content.get_all_values())

        return values


    def get_values_by_rule_id(self, id, check=None):
        """get_values_by_rule_id -- Get all Value elements that are referenced by rule with specified ID
        If check is not None, then it is (very ugly) recursive call
        """

        if self.object != "xccdf_policy":
            raise TypeError("Wrong call of \"get_values_by_rule_id\" function. Should be xccdf_policy (have %s)" %(self.object,))
        items = []
        values = []

        # Case 1: check is not None -- we have recursive call
        if check != None:
            if check.complex:
                # This check is complext so there is more checks within
                for child in check.children:
                    values.extend(self.get_values_by_rule_id(id, check=child))
            else:
                for export in check.exports:
                    values.append(export.value)
            return values

        # Case 2: check is None -- this is regular call of function
        item = self.model.benchmark.get_item(id)
        if item.type != OSCAP.XCCDF_RULE:
            raise TypeError("Wrong type of item with id \"%s\". Expected XCCDF_RULE, got " % (id, item.type))
        rule = item.to_rule()
        for check in rule.checks:
            if check.complex:
                # This check is complext so there is more checks within
                for child in check.children:
                    values.extend(self.get_values_by_rule_id(id, check=child))
            else:
                for export in check.exports:
                    values.append(export.value)

        for value in self.model.benchmark.get_all_values():
            if value.id in values:
                items.append(self.__parse_value(value))

        return items

    def __parse_value(self, value):

        # get value properties
        item = {}
        item["id"] = value.id
        item["lang"] = self.model.benchmark.lang
        item["titles"] = {}
        item["descs"] = {}
        # Titles / Questions
        if len(value.question):
            for question in value.question:
                item["titles"][question.lang] = question.text
        else:
            for title in value.title:
                item["titles"][title.lang] = title.text
        if item["lang"] not in item["titles"]:
            item["titles"][item["lang"]] = ""
        # Descriptions
        for desc in value.description:
            item["descs"][desc.lang] = desc.text
        if item["lang"] not in item["descs"]:
            item["descs"][item["lang"]] = ""
        # Type
        item["type"] = value.type
        # Values
        item["options"] = {}
        item["choices"] = {}
        for instance in value.instances:
            item["options"][instance.selector] = instance.value
            if len(instance.choices):
                item["choices"][instance.selector] = instance.choices

        #Get regexp match from match of elements

        # Get regexp match from match elements
        item["match"] = "|".join([i.match for i in value.instances if i.match])

        # Get regexp match from type of value
        if not len(item["match"]):
            item["match"] = ["", "^[\\d]+$", "^.*$", "^[01]$"][value.type]

        if self.profile != None:
            for r_value in self.profile.refine_values:
                if r_value.item == value.id:
                    item["selected"] = (r_value.selector, item["options"][r_value.selector])
            for s_value in self.profile.setvalues:
                if s_value.item == value.id:
                    item["selected"] = ('', s_value.value)

        if "selected" not in item:
            if "" in item["options"]:
                item["selected"] = ('', item["options"][""])
            else:
                item["selected"] = ('', '')

        """
        print "ID: \r\t\t", item["id"]
        print "Language: \r\t\t", item["lang"]
        print "Titles: \r\t\t", item["titles"]
        print "Descriptions: \r\t\t", item["descs"]
        print "Type: \r\t\t", ["", "Number", "String", "Boolean"][item["type"]]
        print "Options: \r\t\t", item["options"]
        print "Choices: \r\t\t", item["choices"]
        print "Match: \r\t\t", item["match"]
        print "Selected: \r\t\t", item["selected"]
        print
        """

        return item


    def get_tailor_items(self):
        """xccdf_policy.get_tailor_items() -- Get all items that can be tailored by tool.
        Function will return all values that can be tailored by specified XCCDF Policy's Profile

        Function will return list of items where item is dictionary with key representation:
            "id"        - id of value
            "lang"      - default language of document
            "titles"    - list of tuples (language, title)
            "descs"     - list of tuples (language, description)
            "type"      - type of value represented by integer: {0:"", 1:"Number", 2:"String", 3:"Boolean"}
            "options"   - dictionary of options where key is selector and value is Value instance value
            "match"   - Regexp that input must match
            "selected"  - tuple (selector, value) of default or choosen value instance"""

        if self.object != "xccdf_policy":
            raise TypeError("Wrong call of \"get_tailor_items\" function. Should be xccdf_policy (have %s)" %(self.object,))
        items = []

        for value in self.model.benchmark.get_all_values():
            items.append(self.__parse_value(value))

        return items

    def set_tailor_items(self, items):
        """xccdf_policy.set_tailor_items(items) -- Set tailored items to selected XCCDF Profile
        Function will set all refine-values, setvalues to the selected XCCDF Policy's profile.
        All refines should be specified in 'items' list. All existing refine elements that are not included in
        'items' list will be let unchanched.

        Example:
          value = { "id": value_id
                    "value": default_value }
          items = [value]
          xccdf_policy.set_tailor_items(items)"""

        if self.object != "xccdf_policy":
            raise TypeError("Wrong call of \"set_tailor_items\" function. Should be xccdf_policy (have %s)" %(self.object,))
        if len(items) == 0:
            return

        # items = [{id, value}]
        for item in items:
            selector = None
            value = self.model.benchmark.item(item["id"]).to_value()
            for instance in value.instances:
                if item["value"] == instance.value:
                    selector = instance.selector

            oper = remarks = setvalue = None
            for r_value in self.profile.refine_values[:]:
                if r_value.item == item["id"]:
                    oper = r_value.oper
                    remarks = r_value.remarks
                    self.profile.refine_values.remove(r_value)
            for s_value in self.profile.setvalues[:]:
                if s_value.item == item["id"]:
                    setvalue = s_value.value
                    self.profile.setvalues.remove(s_value)

            if selector != None and selector != '':
                r_value = xccdf.refine_value()
                r_value.item = item["id"]
                r_value.selector = selector
                if oper != None:
                    r_value.oper = oper
                if remarks != None:
                    for remark in remarks:
                        r_value.add_remark(remark)
                self.profile.add_refine_value(r_value)
            elif selector is None:
                s_value = xccdf.setvalue()
                s_value.item = item["id"]
                s_value.value = item["value"]
                self.profile.add_setvalue(s_value)

    def set_refine_rule(self, id, weight=None, severity=None, role=None):
        """xccdf_policy.set_refine_rules(refines) -- Set weight, severity and role of the rule in selected Profile.
        Function will set all refine-rules to the selected XCCDF Policy's profile.

        Example:
          xccdf_policy.set_refine_rule("rul-2.1", severity=oscap.XCCDF_SEVERITY_HIGH)
        """

        if self.object != "xccdf_policy":
            raise TypeError("Wrong call of \"set_refine_rule\" function. Should be xccdf_policy (have %s)" %(self.object,))
        if id is None:
            raise AttributeError("Missing ID of rule in xccdf_policy.set_refine_rule function")

        rule = self.model.benchmark.item(id).to_rule()
        if rule is None:
            raise Exception("Rule \"%s\" not found in benchmark" % (id,))

        if not weight and not severity and not role:
            return

        refine = xccdf.refine_rule()
        refine.item = id
        for r_rule in self.profile.refine_rules[:]:
            if r_rule.item == id:
                refine.weight = r_rule.weight
                refine.severity = r_rule.severity
                refine.role = r_rule.role
                self.profile.refine_rules.remove(r_rule)

        # Set new weight of the rule
        if weight != None:
            refine.weight = weight

        # Set new severity of the rule
        if severity != None:
            refine.severity = severity

        # Set new role of the rule
        if role != None:
            refine.role = role

        self.profile.add_refine_rule(refine)


    def get_all_rules(self):
        """xccdf_policy.get_all_rules() -- Get all rules/selectors and titles from benchmark
        """

        if self.object != "xccdf_policy":
            raise TypeError("Wrong call of \"get_all_rules\" function. Should be xccdf_policy (have %s)" %(self.object,))
        pass #TODO


    def set_rules(self, rules):
        """xccdf_policy.set_rules(rules) -- Set which rules are selected by given XCCDF Profile
        Function set selectors for given XCCDF Policy's profile.
        Selectors are represented by ID strings in 'rules' list. All existing selectors that are not included
        in 'rules' list will be deleted.

        Example:
          # We want to have selected only first rule and second group
          xccdf_policy.set_rules(["id-rule-1", "id-group-2"])"""

        if self.object != "xccdf_policy":
            raise TypeError("Wrong call of \"set_rules\" function. Should be xccdf_policy (have %s)" %(self.object,))

        for select in self.selects:
            if select.item not in rules:
                select.selected = False
            else:
                rules.remove(select.item)
                select.selected = True

        for id in rules:
            select = xccdf.select()
            select.selected = True
            select.item = id
            self.add_select(select)

    def init(self, path, paths={}):
        """xccdf.init(path) -- Initialize openscap library
        Provides standard initialization of OPENScap library.
        Parameter 'path' is the path to XCCDF File.
        Parameter 'paths' is dictionary where key is file identificator and value path to the file.

        Initialization has next steps:
         - Parse oscap configuration file with path to XML files
         - Import default XCCDF document as specified in configuration file
         - Import all definitions files that are required for XCCDF evaluation

        Function returns dictionary with keys:
            "policy_model"   - XCCDF Policy Model loaded from XCCDF file
            "def_models"     - list of OVAL Definitions models from OVAL files
            "sessions"       - dictionary of OVAL Agent sessions provided by OVAL Definitions models

        All returned objects have to be freed by user. Use functions:
            retval["policy_model"].free()
            for model in retval["def_models"]:
                model.free()
            for sess in retval["sessions"]:
                sess.free()
        """

        if path is None:
            return None

        OSCAP.oscap_init()
        dirname = os.path.dirname(path)
        f_XCCDF = path

        benchmark = self.benchmark_import(f_XCCDF)
        if benchmark.instance is None:
            if OSCAP.oscap_err():
                desc = OSCAP.oscap_err_desc()
            else:
                desc = "Unknown error, please report this bug (http://bugzilla.redhat.com/)"
            raise ImportError("Benchmark \"%s\" loading failed: %s" % (f_XCCDF, desc))
        policy_model = self.policy_model(benchmark)
        files = policy_model.get_files()
        def_models = []
        sessions = {}
        names = {}
        for file in files.strings:
            if file in paths:
                f_OVAL = paths[file]
            else:
                f_OVAL = os.path.join(dirname, file)
            if os.path.exists(f_OVAL):
                def_model = oval.definition_model_import(f_OVAL)
                if def_model.instance is None:
                    if OSCAP.oscap_err():
                        desc = OSCAP.oscap_err_desc()
                    else:
                        desc = "Unknown error, please report this bug (http://bugzilla.redhat.com/)"
                    raise ImportError("Cannot import definition model for \"%s\": %s" % (f_OVAL, desc))
                def_models.append(def_model)
                sess = oval.agent_new_session(def_model, file)
                if sess is None or sess.instance is None:
                    if OSCAP.oscap_err():
                        desc = OSCAP.oscap_err_desc()
                    else:
                        desc = "Unknown error, please report this bug (http://bugzilla.redhat.com/)"
                    raise ImportError("Cannot create agent session for \"%s\": %s" % (f_OVAL, desc))
                sessions[file] = sess
                names[file] = [sess, def_model]
                policy_model.register_engine_oval(sess)
            else:
                print("WARNING: Skipping %s file which is referenced from XCCDF content" % (f_OVAL,))
        files.free()
        return {"def_models":def_models, "sessions":sessions, "policy_model":policy_model, "xccdf_path":f_XCCDF, "names":names}

    def policy_export(self, result=None, title=None, filename=None, prefix=None, path=None, sessions=None, variables=True):
        """Export all files for given policy.
        """

        if self.object != "xccdf_policy":
            raise TypeError("Wrong call of \"export\" function. Should be xccdf_policy (have %s)" %(self.object,))

        # FIXME: We clone both benchmark and the result to avoid changing them when just export is requested
        #        Although this is the right behavior, it is potentially wasteful, result.clone() and benchmark.clone()
        #        could potentially take a lot of time to complete.
        #        A better solution would be to add the result, export and then remove the result
        #        (with appropriate exception safety of course) or even better, allow export with custom result list.

        result_clone = result.clone()
        result_clone.benchmark_uri = path or "benchmark.xml"
        o_title = common.text()
        o_title.text = title
        result_clone.title = o_title
        result_clone.fill_sysinfo()

        files = [filename]

        benchmark_clone = self.model.benchmark.clone()

        for model in benchmark_clone.models:
            result_clone.score = self.score(result_clone, model.system)

        benchmark_clone.add_result(result_clone)

        benchmark_clone.export(filename)

        dirname = os.path.dirname(filename)
        for path in sessions.keys():
            sess = sessions[path]
            rmodel = oval.agent_get_results_model(sess)
            pfile = path+".result.xml"
            OSCAP.oval_results_model_export(rmodel.instance, None, os.path.join(dirname, pfile))
            files.append(pfile)
            if variables:
                dmodel = rmodel.definition_model
                for i, vmodel in enumerate(dmodel.variable_models):
                    vfile = "%s.variables-%d.xml" % (path, i)
                    vmodel.export(os.path.join(dirname, vfile))
                    files.append(vfile)

        return files

    def destroy(self, sdir):

        OSCAP.oscap_cleanup()
        for model in sdir["def_models"]+sdir["sessions"].values()+[sdir["policy_model"]]:
            model.free()


# ------------------------------------------------------------------------------------------------------------
# DS

class DS_Class(OSCAP_Object):

    def __init__(self):
        dict.__setattr__(self, "object", "ds")
        #dict.__setattr__(self, "version", OSCAP.oval_definition_model_supported())

    def __repr__(self):
        return "<Oscap Object of type 'DS Class' at %s>" % (hex(id(self)),)


# ------------------------------------------------------------------------------------------------------------
# XCCDF

class _XCCDF_Benchmark_Class(OSCAP_Object):

    def __init__(self, path):
        dict.__setattr__(self, "object", "xccdf_benchmark")
        dict.__setattr__(self, "instance", OSCAP.xccdf_benchmark_import_source(OSCAP.oscap_source_new_from_file(path)))

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
    """
    CVE Class
    """

    def __init__(self):
        dict.__setattr__(self, "object", "cve")
#        dict.__setattr__(self, "version", OSCAP.cve_model_supported())
        pass

    def __repr__(self):
        return "<Oscap Object of type 'CVE Class' at %s>" % (hex(id(self)),)

# ------------------------------------------------------------------------------------------------------------
# CPE

class CPE_Class(OSCAP_Object):
    """
    CPE Class
    """

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
    """
    CVSS Class
    """

    def __init__(self):
        dict.__setattr__(self, "object", "cvss")
        dict.__setattr__(self, "version", OSCAP.cvss_model_supported())
        pass

    def __repr__(self):
        return "<Oscap Object of type 'CVSS Class' at %s>" % (hex(id(self)),)

# ------------------------------------------------------------------------------------------------------------
# CCE

class CCE_Class(OSCAP_Object):
    """
    CCE Class
    """

    def __init__(self):
        dict.__setattr__(self, "object", "cce")
#        dict.__setattr__(self, "version", OSCAP.cce_supported())
        pass

    def __repr__(self):
        return "<Oscap Object of type 'CCE Class' at %s>" % (hex(id(self)),)

# ------------------------------------------------------------------------------------------------------------
# SCE

class SCE_Class(OSCAP_Object):
    """
    SCE Class
    """

    def __init__(self):
        dict.__setattr__(self, "object", "sce")
        #dict.__setattr__(self, "version", OSCAP.cce_supported())
        pass

    def __repr__(self):
        return "<Oscap Object of type 'SCE Class' at %s>" % (hex(id(self)),)

# ------------------------------------------------------------------------------------------------------------

""" This part is very IMPORTANT! Implement your application functions
to use openscap library this way:
    policy_titles = openscap.xccdf.policy.titles (this will reflect xccdf_policy_get_titles() func)
Below are particular objects for parts of openscap library module system. The only change is in using
oscap module wich is conflicting with oscap namespace. This module is renamed to common. All functions
using OSCAP functions should look like:
    openscap.common.debug.seterr(err) (this will reflect oscap_debug_seterr() func)
"""

ds = DS_Class()
xccdf = XCCDF_Class()
oval = OVAL_Class()
cve = CVE_Class()
cce = CCE_Class()
cpe = CPE_Class()
cvss = CVSS_Class()
sce = SCE_Class()
common = OSCAP_Object("oscap")
