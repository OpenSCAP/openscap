/*
 * Open Vulnerability and Assessment Language
 * (http://oval.mitre.org/)
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

%module openscap
%{
 #include "../src/common/public/oscap.h"
 #include "../src/common/public/error.h"

%}
%include "../src/common/public/oscap.h"
%include "../src/common/public/error.h"

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
%}
%include "../src/XCCDF/public/xccdf.h"
#endif

