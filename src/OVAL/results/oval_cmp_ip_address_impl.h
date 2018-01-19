/**
 * @file oval_cmp_ip_address_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 *     Tomas Heinrich <theinric@redhat.com>
 *     Šimon Lukašík
 */

#ifndef OSCAP_OVAL_IP_ADDRESS_IMPL_H_
#define OSCAP_OVAL_IP_ADDRESS_IMPL_H_

#include "common/util.h"

#include "oval_definitions.h"
#include "oval_types.h"


/**
 * Compare two IP address or address sets (CIDR). The format of input string
 * shall conform to ipv4_address types from oval:SimpleDatatypeEnumeration.
 * This operation is not commutative, be aware of the order of operands.
 * @param af Internet address family (AF_INET or AF_INET6)
 * @param s1 ipv4 address as defined by state element
 * @param s2 ipv4 address as captured from system (from syschar object)
 * @param op type of comparison operation
 * @returns result of comparison as defined by OVAL specification
 */
oval_result_t oval_ipaddr_cmp(int af, const char *s1, const char *s2, oval_operation_t op);


#endif
