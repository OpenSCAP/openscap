/*
 * Open Vulnerability and Assessment Language
 * (http://oval.mitre.org/)
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Maros Barabas   <mbarabas@redhat.com>
 *
 */


/* Convert a tuple element to a C array */
/* SWIG can't understand __attribute__(x), so we make it go away */
#define __attribute__(x)

#if defined(SWIGPYTHON)
/* Definitions for PYTHON */

%ignore xccdf_policy_evaluate;
%ignore xccdf_policy_model_register_callback;

%typemap(in) time_t
{
    if (PyLong_Check($input))
        $1 = (time_t) PyLong_AsLong($input);
    else if (PyInt_Check($input))
        $1 = (time_t) PyInt_AsLong($input);
    else if (PyFloat_Check($input))
        $1 = (time_t) PyFloat_AsDouble($input);
    else {
        PyErr_SetString(PyExc_TypeError,"Expected a large number");
        return NULL;
    }
}

%typemap (in) void*
{
    if (PyCObject_Check($input))
    {
        $1 = PyCObject_AsVoidPtr($input);
    }
}

%typemap(in) struct cpe_name ** {

    int i; 
    int a_size = 0;
    int res_o = 0;
    a_size = (int) PySequence_Length($input);

    if (!PySequence_Check($input)) {
        PyErr_SetString(PyExc_ValueError,"Expected a sequence");
        return NULL;
    }
    if (a_size <= 0) {
        PyErr_SetString(PyExc_ValueError,"Expected not empty sequence");
        return NULL;
    }

    $1 = (struct cpe_name **) malloc(a_size*sizeof(struct cpe_name *));
    for (i = 0; i < a_size; i++) {
        void *arg = 0;
        PyObject *obj = PySequence_GetItem($input,i);
        if (obj == NULL) {
            SWIG_exception_fail(SWIG_ArgError(res_o), "in argument '" "cpe_name" "' substitution '" "', can't access sequence");
            return NULL; /*5956*/
        }
        res_o = SWIG_ConvertPtr(obj, &arg, SWIGTYPE_p_cpe_name, 0 );
        if (!SWIG_IsOK(res_o)) {
            SWIG_exception_fail(SWIG_ArgError(res_o), "in argument '" "cpe_name" "' substitution invalid types");
            return NULL;
        }
        $1[i] = (struct cpe_name *) arg;
    }
}
%typemap(in) struct oval_syschar_model ** {

    int i; 
    int a_size = 0;
    int res_o = 0;
    a_size = (int) PySequence_Length($input);

    if (!PySequence_Check($input)) {
        PyErr_SetString(PyExc_ValueError,"Expected a sequence");
        return NULL;
    }
    if (a_size <= 0) {
        PyErr_SetString(PyExc_ValueError,"Expected not empty sequence");
        return NULL;
    }

    $1 = (struct oval_syschar_model **) malloc(a_size*sizeof(struct oval_syschar_model *));
    for (i = 0; i < a_size; i++) {
        void *arg = 0;
        PyObject *obj = PySequence_GetItem($input,i);
        if (obj == NULL) {
            SWIG_exception_fail(SWIG_ArgError(res_o), "in argument '" "oval_syschar_model" "' substitution '" "', can't access sequence");
            return NULL;
        }
        res_o = SWIG_ConvertPtr(obj, &arg, SWIGTYPE_p_oval_syschar_model, 0 );
        if (!SWIG_IsOK(res_o)) {
            SWIG_exception_fail(SWIG_ArgError(res_o), "in argument '" "oval_syschar_model" "' substitution invalid types");
            return NULL;
        }
        $1[i] = (struct oval_syschar_model *) arg;
    }
}
#elif defined(SWIGPERL)
/* Definitions for PERL */
#endif

%module openscap
%{
 #include "../src/common/public/oscap.h"
 #include "../src/common/public/error.h"
 #include "../src/common/public/debug.h"
 #include "../src/common/public/alloc.h"
 #include "../src/common/public/text.h"
%}

%include "../src/common/public/oscap.h"
%include "../src/common/public/error.h"
%include "../src/common/public/debug.h"
%include "../src/common/public/alloc.h"
%include "../src/common/public/text.h"

#ifdef WANT_CCE
%module openscap
%{
 #include "../src/CCE/public/cce.h"
%}
%include "../src/CCE/public/cce.h"
#endif


#ifdef WANT_CPE
%module openscap
%{
 #include "../src/CPE/public/cpeuri.h"
 #include "../src/CPE/public/cpedict.h"
 #include "../src/CPE/public/cpelang.h"
%}
%include "../src/CPE/public/cpeuri.h"
%include "../src/CPE/public/cpedict.h"
%include "../src/CPE/public/cpelang.h"
#endif


#ifdef WANT_CVE
%module openscap
%{
 #include "../src/CVE/public/cve.h"
%}
%include "../src/CVE/public/cve.h"
#endif


#ifdef WANT_CVSS
%module openscap
%{
 #include "../src/CVSS/public/cvss.h"
%}
%include "../src/CVSS/public/cvss.h"
#endif


#ifdef WANT_OVAL
%module openscap
%{
 #include "../src/OVAL/public/oval_agent_api.h"
 #include "../src/OVAL/public/oval_definitions.h"
 #include "../src/OVAL/public/oval_system_characteristics.h"
 #include "../src/OVAL/public/oval_results.h"
 #include "../src/OVAL/public/oval_types.h"
 #include "../src/OVAL/public/oval_variables.h"
 #include "../src/OVAL/public/oval_probe.h"
%}
%include "../src/OVAL/public/oval_agent_api.h"
%include "../src/OVAL/public/oval_definitions.h"
%include "../src/OVAL/public/oval_system_characteristics.h"
%include "../src/OVAL/public/oval_results.h"
%include "../src/OVAL/public/oval_types.h"
%include "../src/OVAL/public/oval_variables.h"
%include "../src/OVAL/public/oval_probe.h"
#endif


#ifdef WANT_XCCDF
%module openscap
%{
 #include "../src/XCCDF/public/xccdf.h"
 #include "../src/XCCDF_POLICY/public/xccdf_policy.h"
%}
%include "../src/XCCDF/public/xccdf.h"
%include "../src/XCCDF_POLICY/public/xccdf_policy.h"
#endif

/*%typemap(in) value[ANY] {

    int i;
    PyErr_SetString(PyExc_ValueError,"Expected a sequence");
    return NULL;
    if (!PySequence_Check($input)) {
        PyErr_SetString(PyExc_ValueError,"Expected a sequence");
        return NULL;
    }
    if (PySequence_Length($input) != $1_dim0) {
            PyErr_SetString(PyExc_ValueError,"Size mismatch. Expected $1_dim0 elements");
            return NULL;
    }
    $1 = (void **) malloc($1_dim0*sizeof( Py_TYPE(PySequence_GetItem($input, 0)) ));

    for (i = 0; i < $1_dim0; i++) {
        PyObject *o = PySequence_GetItem($input,i);
        if (Py_TYPE(PySequence_GetItem($input,i)) != Py_TYPE(PySequence_GetItem($input,0)) {
                PyErr_SetString(PyExc_ValueError,"Type mismatch");
                free($1);
                return NULL;
        }
        $1[i] = (PyTYPE(PySequence_GetItem($input, i))) o;
    }

}

*/
