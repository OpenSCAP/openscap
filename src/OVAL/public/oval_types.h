/**
 * @addtogroup OVAL
 * @{
 *
 * @file
 *
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 */


/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#ifndef OVAL_TYPES_H_
#define OVAL_TYPES_H_

#include <stdbool.h>
#include <oscap.h>


/// OVAL family
typedef enum {
	/**
	 * Unknown platform
	 * @see oval_subtype_t
	 */
	OVAL_FAMILY_UNKNOWN = 0,
	/**
	 * AIX family
	 * @see oval_aix_subtype_t
	 */
	OVAL_FAMILY_AIX = 1000,
	/**
	 * Apache family
	 * @see oval_apache_subtype_t
	 */
	OVAL_FAMILY_APACHE = 2000,
	/**
	 * CatOS family
	 * @see oval_catos_subtype_t
	 */
	OVAL_FAMILY_CATOS = 3000,
	/**
	 * ESX family
	 * @see oval_esx_subtype_t
	 */
	OVAL_FAMILY_ESX = 4000,
	/**
	 * FreeBSD family
	 * @see oval_freebsd_subtype_t
	 */
	OVAL_FAMILY_FREEBSD = 5000,
	/**
	 * HPUX family
	 * @see oval_hpux_subtype_t
	 */
	OVAL_FAMILY_HPUX = 6000,
	/**
	 * Platform independent family
	 * @see oval_independent_subtype_t
	 */
	OVAL_FAMILY_INDEPENDENT = 7000,
	/**
	 * IOS family
	 * @see oval_ios_subtype_t
	 */
	OVAL_FAMILY_IOS = 8000,
	/**
	 * Linux family
	 * @see oval_linux_subtype_t
	 */
	OVAL_FAMILY_LINUX = 9000,
	/**
	 * MacOS family
	 * @see oval_macos_subtype_t
	 */
	OVAL_FAMILY_MACOS = 10000,
	/**
	 * PIXOS family
	 * @see oval_pixos_subtype_t
	 */
	OVAL_FAMILY_PIXOS = 11000,
	/**
	 * Solaris family
	 * @see oval_solaris_subtype_t
	 */
	OVAL_FAMILY_SOLARIS = 12000,
	/**
	 * Unix family
	 * @see oval_unix_subtype_t
	 */
	OVAL_FAMILY_UNIX = 13000,
	/**
	 * Windows family
	 * @see oval_windows_subtype_t
	 */
	OVAL_FAMILY_WINDOWS = 14000,
	OVAL_FAMILY_ANDROID = 15000,
	OVAL_FAMILY_APPLE_IOS = 16000,
	OVAL_FAMILY_ASA = 17000,
	OVAL_FAMILY_IOS_XE = 18000,
	OVAL_FAMILY_JUNOS = 19000,
	OVAL_FAMILY_NETCONF = 20000,
	OVAL_FAMILY_SHAREPOINT = 21000,
} oval_family_t;

/// Unknown subtypes
typedef enum {
	OVAL_SUBTYPE_UNKNOWN = 0,
	OVAL_SUBTYPE_ALL = 1,
	OVAL_SUBTYPE_SYSINFO = 2,

/// AIX subtypes
	OVAL_AIX_FILESET = OVAL_FAMILY_AIX + 1,
	OVAL_AIX_FIX = OVAL_FAMILY_AIX + 2,
	OVAL_AIX_OSLEVEL = OVAL_FAMILY_AIX + 3,
	OVAL_AIX_INTERIM_FIX = OVAL_FAMILY_AIX + 4,
	OVAL_AIX_NO = OVAL_FAMILY_AIX + 5,

/// Android subtypes
	OVAL_ANDROID_APPMANAGER = OVAL_FAMILY_ANDROID + 1,
	OVAL_ANDROID_BLUETOOTH = OVAL_FAMILY_ANDROID + 2,
	OVAL_ANDROID_CAMERA = OVAL_FAMILY_ANDROID + 3,
	OVAL_ANDROID_CERTIFICATE = OVAL_FAMILY_ANDROID + 4,
	OVAL_ANDROID_DEVICE_SETTINGS = OVAL_FAMILY_ANDROID + 5,
	OVAL_ANDROID_ENCRYPTION = OVAL_FAMILY_ANDROID + 6,
	OVAL_ANDROID_LOCATION_SERVICE = OVAL_FAMILY_ANDROID + 7,
	OVAL_ANDROID_NETWORK = OVAL_FAMILY_ANDROID + 8,
	OVAL_ANDROID_PASSWORD = OVAL_FAMILY_ANDROID + 9,
	OVAL_ANDROID_SYSTEM_DETAILS = OVAL_FAMILY_ANDROID + 10,
	OVAL_ANDROID_WIFI = OVAL_FAMILY_ANDROID + 11,
	OVAL_ANDROID_WIFI_NETWORK = OVAL_FAMILY_ANDROID + 12,
	OVAL_ANDROID_TELEPHONY = OVAL_FAMILY_ANDROID + 13,

/// Apache subtypes
	OVAL_APACHE_HTTPD = OVAL_FAMILY_APACHE + 1,
	OVAL_APACHE_VERSION = OVAL_FAMILY_APACHE + 2,

/// Apple iOS subtypes
	OVAL_APPLE_IOS_GLOBAL_RESTRICTIONS = OVAL_FAMILY_APPLE_IOS + 1,
	OVAL_APPLE_IOS_PASSCODE_POLICY = OVAL_FAMILY_APPLE_IOS + 2,
	OVAL_APPLE_IOS_PROFILE = OVAL_FAMILY_APPLE_IOS + 3,

/// Cisco ASA
	OVAL_ASA_ACL = OVAL_FAMILY_ASA + 1,
	OVAL_ASA_CLASS_MAP = OVAL_FAMILY_ASA + 2,
	OVAL_ASA_INTERFACE = OVAL_FAMILY_ASA + 3,
	OVAL_ASA_LINE = OVAL_FAMILY_ASA + 4,
	OVAL_ASA_POLICY_MAP = OVAL_FAMILY_ASA + 5,
	OVAL_ASA_SERVICE_POLICY = OVAL_FAMILY_ASA + 6,
	OVAL_ASA_SNMP_HOST = OVAL_FAMILY_ASA + 7,
	OVAL_ASA_SNMP_USER = OVAL_FAMILY_ASA + 8,
	OVAL_ASA_SNMP_GROUP = OVAL_FAMILY_ASA + 9,
	OVAL_ASA_TCP_MAP = OVAL_FAMILY_ASA + 10,
	OVAL_ASA_VERSION = OVAL_FAMILY_ASA + 11,

/// CatOS subtypes
	OVAL_CATOS_LINE = OVAL_FAMILY_CATOS + 1,
	OVAL_CATOS_MODULE = OVAL_FAMILY_CATOS + 2,
	OVAL_CATOS_VERSION_55 = OVAL_FAMILY_CATOS + 3,
	OVAL_CATOS_VERSION = OVAL_FAMILY_CATOS + 4,

/// ESX subtypes
	OVAL_ESX_PATCH = OVAL_FAMILY_ESX + 1,
	OVAL_ESX_PATCH56 = OVAL_FAMILY_ESX + 2,
	OVAL_ESX_VERSION = OVAL_FAMILY_ESX + 3,
	OVAL_ESX_VISDKMANAGEDOBJECT = OVAL_FAMILY_ESX + 4,

/// FreeBSD subtypes
	OVAL_FREEBSD_PORT_INFO = OVAL_FAMILY_FREEBSD + 1,

/// HP-UX subtypes
	OVAL_HPUX_GETCONF = OVAL_FAMILY_HPUX + 1,
	OVAL_HPUX_PATCH_53 = OVAL_FAMILY_HPUX + 2,
	OVAL_HPUX_PATCH = OVAL_FAMILY_HPUX + 3,
	OVAL_HPUX_SOFTWARE_LIST = OVAL_FAMILY_HPUX + 4,
	OVAL_HPUX_TRUSTED_ACCOUNTS = OVAL_FAMILY_HPUX + 5,
	OVAL_HPUX_NDD = OVAL_FAMILY_HPUX + 6,
	OVAL_HPUX_SWLIST = OVAL_FAMILY_HPUX + 7,
	OVAL_HPUX_TRUSTED = OVAL_FAMILY_HPUX + 8,

/// Platform independent subtypes
	OVAL_INDEPENDENT_FAMILY = OVAL_FAMILY_INDEPENDENT + 1,
	OVAL_INDEPENDENT_FILE_MD5 = OVAL_FAMILY_INDEPENDENT + 2,
	OVAL_INDEPENDENT_FILE_HASH = OVAL_FAMILY_INDEPENDENT + 3,
	OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE = OVAL_FAMILY_INDEPENDENT + 4,
	OVAL_INDEPENDENT_SQL = OVAL_FAMILY_INDEPENDENT + 5,
	OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54 = OVAL_FAMILY_INDEPENDENT + 6,
	OVAL_INDEPENDENT_TEXT_FILE_CONTENT = OVAL_FAMILY_INDEPENDENT + 7,
	OVAL_INDEPENDENT_UNKNOWN = OVAL_FAMILY_INDEPENDENT + 8,
	OVAL_INDEPENDENT_VARIABLE = OVAL_FAMILY_INDEPENDENT + 9,
	OVAL_INDEPENDENT_XML_FILE_CONTENT = OVAL_FAMILY_INDEPENDENT + 10,
        OVAL_INDEPENDENT_LDAP57 = OVAL_FAMILY_INDEPENDENT + 11,
	OVAL_INDEPENDENT_LDAP = OVAL_FAMILY_INDEPENDENT + 12,
        OVAL_INDEPENDENT_SQL57 = OVAL_FAMILY_INDEPENDENT + 13,
	OVAL_INDEPENDENT_ENVIRONMENT_VARIABLE58 = OVAL_FAMILY_INDEPENDENT + 14,
	OVAL_INDEPENDENT_FILE_HASH58 = OVAL_FAMILY_INDEPENDENT + 15,
	OVAL_INDEPENDENT_SYSCHAR_SUBTYPE = OVAL_FAMILY_INDEPENDENT + 16,
	OVAL_INDEPENDENT_YAML_FILE_CONTENT = OVAL_FAMILY_INDEPENDENT + 17,

/// IOS subtypes
	OVAL_IOS_GLOBAL = OVAL_FAMILY_IOS + 1,
	OVAL_IOS_INTERFACE = OVAL_FAMILY_IOS + 2,
	OVAL_IOS_LINE = OVAL_FAMILY_IOS + 3,
	OVAL_IOS_SNMP = OVAL_FAMILY_IOS + 4,
	OVAL_IOS_TCLSH = OVAL_FAMILY_IOS + 5,
	OVAL_IOS_VERSION_55 = OVAL_FAMILY_IOS + 6,
	OVAL_IOS_ACL = OVAL_FAMILY_IOS + 7,
	OVAL_IOS_BGP_NEIGHBOR = OVAL_FAMILY_IOS + 8,
	OVAL_IOS_ROUTER = OVAL_FAMILY_IOS + 9,
	OVAL_IOS_ROUTING_PROTOCOL_AUTH_INTF = OVAL_FAMILY_IOS + 10,
	OVAL_IOS_SECTION = OVAL_FAMILY_IOS + 11,
	OVAL_IOS_VERSION = OVAL_FAMILY_IOS + 12,
	OVAL_IOS_SNMP_COMMUNITY = OVAL_FAMILY_IOS + 13,
	OVAL_IOS_SNMP_GROUP = OVAL_FAMILY_IOS + 14,
	OVAL_IOS_SNMP_HOST = OVAL_FAMILY_IOS + 15,
	OVAL_IOS_SNMP_USER = OVAL_FAMILY_IOS + 16,
	OVAL_IOS_SNMP_VIEW = OVAL_FAMILY_IOS + 17,

/// IOS-XE subtypes
	OVAL_IOS_XE_GLOBAL = OVAL_FAMILY_IOS_XE + 1,
	OVAL_IOS_XE_INTERFACE = OVAL_FAMILY_IOS_XE + 2,
	OVAL_IOS_XE_LINE = OVAL_FAMILY_IOS_XE + 3,
	OVAL_IOS_XE_ACL = OVAL_FAMILY_IOS_XE + 4,
	OVAL_IOS_XE_BGP_NEIGHBOR = OVAL_FAMILY_IOS_XE + 5,
	OVAL_IOS_XE_ROUTER = OVAL_FAMILY_IOS_XE + 6,
	OVAL_IOS_XE_ROUTING_PROTOCOL_AUTH_INTF = OVAL_FAMILY_IOS_XE + 7,
	OVAL_IOS_XE_SECTION = OVAL_FAMILY_IOS_XE + 8,
	OVAL_IOS_XE_VERSION = OVAL_FAMILY_IOS_XE + 9,
	OVAL_IOS_XE_SNMP_COMMUNITY = OVAL_FAMILY_IOS_XE + 10,
	OVAL_IOS_XE_SNMP_GROUP = OVAL_FAMILY_IOS_XE + 11,
	OVAL_IOS_XE_SNMP_HOST = OVAL_FAMILY_IOS_XE + 12,
	OVAL_IOS_XE_SNMP_USER = OVAL_FAMILY_IOS_XE + 13,
	OVAL_IOS_XE_SNMP_VIEW = OVAL_FAMILY_IOS_XE + 14,

/// Junos subtypes
	OVAL_JUNOS_XML_CONFIG = OVAL_FAMILY_JUNOS + 1,
	OVAL_JUNOS_SHOW = OVAL_FAMILY_JUNOS + 2,
	OVAL_JUNOS_VERSION = OVAL_FAMILY_JUNOS + 3,
	OVAL_JUNOS_XML_SHOW = OVAL_FAMILY_JUNOS + 4,

/// Linux subtypes
	OVAL_LINUX_DPKG_INFO = OVAL_FAMILY_LINUX + 1,
	OVAL_LINUX_INET_LISTENING_SERVERS = OVAL_FAMILY_LINUX + 2,
	OVAL_LINUX_RPM_INFO = OVAL_FAMILY_LINUX + 3,
	OVAL_LINUX_SLACKWARE_PKG_INFO_TEST = OVAL_FAMILY_LINUX + 4,
        OVAL_LINUX_PARTITION = OVAL_FAMILY_LINUX + 5,
	OVAL_LINUX_IFLISTENERS = OVAL_FAMILY_LINUX + 6,
	OVAL_LINUX_RPMVERIFY = OVAL_FAMILY_LINUX + 7,
	OVAL_LINUX_RPMVERIFYFILE = OVAL_FAMILY_LINUX + 71,
	OVAL_LINUX_RPMVERIFYPACKAGE = OVAL_FAMILY_LINUX + 72,
	OVAL_LINUX_SELINUXBOOLEAN = OVAL_FAMILY_LINUX + 8,
	OVAL_LINUX_SELINUXSECURITYCONTEXT = OVAL_FAMILY_LINUX + 9,
	OVAL_LINUX_INET_LISTENING_SERVER = OVAL_FAMILY_LINUX + 102,
	OVAL_LINUX_SYSTEMDUNITPROPERTY = OVAL_FAMILY_LINUX + 103,
	OVAL_LINUX_SYSTEMDUNITDEPENDENCY = OVAL_FAMILY_LINUX + 104,
	OVAL_LINUX_FWUPDSECATTR = OVAL_FAMILY_LINUX + 105,

/// MacOS subtypes
	OVAL_MACOS_ACCOUNT_INFO = OVAL_FAMILY_MACOS + 1,
	OVAL_MACOS_INET_LISTENING_SERVERS = OVAL_FAMILY_MACOS + 2,
	OVAL_MACOS_NVRAM_INFO = OVAL_FAMILY_MACOS + 3,
	OVAL_MACOS_PWPOLICY = OVAL_FAMILY_MACOS + 4,
	OVAL_MACOS_AUTHORIZATION_DB = OVAL_FAMILY_MACOS + 5,
	OVAL_MACOS_CORE_STORAGE = OVAL_FAMILY_MACOS + 6,
	OVAL_MACOS_DISKUTIL = OVAL_FAMILY_MACOS + 7,
	OVAL_MACOS_GATEKEEPER = OVAL_FAMILY_MACOS + 8,
	OVAL_MACOS_INET_LISTENING_SERVER_510 = OVAL_FAMILY_MACOS + 9,
	OVAL_MACOS_KEYCHAIN = OVAL_FAMILY_MACOS + 10,
	OVAL_MACOS_LAUNCHD = OVAL_FAMILY_MACOS + 11,
	OVAL_MACOS_NVRAM = OVAL_FAMILY_MACOS + 12,
	OVAL_MACOS_PLIST = OVAL_FAMILY_MACOS + 13,
	OVAL_MACOS_PLIST510 = OVAL_FAMILY_MACOS + 14,
	OVAL_MACOS_PLIST511 = OVAL_FAMILY_MACOS + 15,
	OVAL_MACOS_PWPOLICY59 = OVAL_FAMILY_MACOS + 16,
	OVAL_MACOS_RLIMIT = OVAL_FAMILY_MACOS + 17,
	OVAL_MACOS_SOFTWARE_UPDATE = OVAL_FAMILY_MACOS + 18,
	OVAL_MACOS_SYSTEM_PROFILER = OVAL_FAMILY_MACOS + 19,
	OVAL_MACOS_SYSTEM_SETUP = OVAL_FAMILY_MACOS + 20,

/// NETCONF subtypes
	OVAL_NETCONF_CONFIG = OVAL_FAMILY_NETCONF + 1,

/// PIXOS subtypes
	OVAL_PIXOS_LINE = OVAL_FAMILY_PIXOS + 1,
	OVAL_PIXOS_VERSION = OVAL_FAMILY_PIXOS + 2,

/// Windows SharePoint Services subtypes
	OVAL_SHAREPOINT_SPWEBAPPLICATION = OVAL_FAMILY_SHAREPOINT + 1,
	OVAL_SHAREPOINT_SPGROUP = OVAL_FAMILY_SHAREPOINT + 2,
	OVAL_SHAREPOINT_SPWEB = OVAL_FAMILY_SHAREPOINT + 3,
	OVAL_SHAREPOINT_SPLIST = OVAL_FAMILY_SHAREPOINT + 4,
	OVAL_SHAREPOINT_SPANTIVIRUSSETTINGS = OVAL_FAMILY_SHAREPOINT + 5,
	OVAL_SHAREPOINT_SPSITEADMINISTRATION = OVAL_FAMILY_SHAREPOINT + 6,
	OVAL_SHAREPOINT_SPSITE = OVAL_FAMILY_SHAREPOINT + 7,
	OVAL_SHAREPOINT_SPCRAWLRULE = OVAL_FAMILY_SHAREPOINT + 8,
	OVAL_SHAREPOINT_SPJOBDEFINITION = OVAL_FAMILY_SHAREPOINT + 9,
	OVAL_SHAREPOINT_SPJOBDEFINITION510 = OVAL_FAMILY_SHAREPOINT + 10,
	OVAL_SHAREPOINT_BESTBET = OVAL_FAMILY_SHAREPOINT + 11,
	OVAL_SHAREPOINT_INFOPOLICYCOLL = OVAL_FAMILY_SHAREPOINT + 12,
	OVAL_SHAREPOINT_SPDIAGNOSTICSSERVICE = OVAL_FAMILY_SHAREPOINT + 13,
	OVAL_SHAREPOINT_SPDIAGNOSTICSLEVEL = OVAL_FAMILY_SHAREPOINT + 14,
	OVAL_SHAREPOINT_SPPOLICYFEATURE = OVAL_FAMILY_SHAREPOINT + 15,
	OVAL_SHAREPOINT_SPPOLICY = OVAL_FAMILY_SHAREPOINT + 16,

/// Solaris subtypes
	OVAL_SOLARIS_ISAINFO = OVAL_FAMILY_SOLARIS + 1,
	OVAL_SOLARIS_PACKAGE = OVAL_FAMILY_SOLARIS + 2,
	OVAL_SOLARIS_PATCH = OVAL_FAMILY_SOLARIS + 3,
	OVAL_SOLARIS_SMF = OVAL_FAMILY_SOLARIS + 4,
	OVAL_SOLARIS_PATCH54 = OVAL_FAMILY_SOLARIS + 5,
	OVAL_SOLARIS_NDD = OVAL_FAMILY_SOLARIS + 6,
	OVAL_SOLARIS_PACKAGECHECK = OVAL_FAMILY_SOLARIS + 7,
	OVAL_SOLARIS_FACET = OVAL_FAMILY_SOLARIS + 8,
	OVAL_SOLARIS_IMAGE = OVAL_FAMILY_SOLARIS + 9,
	OVAL_SOLARIS_PACKAGE511 = OVAL_FAMILY_SOLARIS + 10,
	OVAL_SOLARIS_PACKAGEAVOIDLIST = OVAL_FAMILY_SOLARIS + 11,
	OVAL_SOLARIS_PACKAGEFREEZELIST = OVAL_FAMILY_SOLARIS + 12,
	OVAL_SOLARIS_PACKAGEPUBLISHER = OVAL_FAMILY_SOLARIS + 13,
	OVAL_SOLARIS_SMFPROPERTY = OVAL_FAMILY_SOLARIS + 14,
	OVAL_SOLARIS_VARIANT = OVAL_FAMILY_SOLARIS + 15,
	OVAL_SOLARIS_VIRTUALIZATIONINFO = OVAL_FAMILY_SOLARIS + 16,

/// Unix subtypes
	OVAL_UNIX_FILE = OVAL_FAMILY_UNIX + 1,
	OVAL_UNIX_INETD = OVAL_FAMILY_UNIX + 2,
	OVAL_UNIX_INTERFACE = OVAL_FAMILY_UNIX + 3,
	OVAL_UNIX_PASSWORD = OVAL_FAMILY_UNIX + 4,
	OVAL_UNIX_PROCESS = OVAL_FAMILY_UNIX + 5,
	OVAL_UNIX_RUNLEVEL = OVAL_FAMILY_UNIX + 6,
	OVAL_UNIX_SCCS = OVAL_FAMILY_UNIX + 7,
	OVAL_UNIX_SHADOW = OVAL_FAMILY_UNIX + 8,
	OVAL_UNIX_UNAME = OVAL_FAMILY_UNIX + 9,
	OVAL_UNIX_XINETD = OVAL_FAMILY_UNIX + 10,
	OVAL_UNIX_DNSCACHE = OVAL_FAMILY_UNIX + 11,
	OVAL_UNIX_SYSCTL = OVAL_FAMILY_UNIX + 12,
	OVAL_UNIX_PROCESS58 = OVAL_FAMILY_UNIX + 13,
	OVAL_UNIX_FILEEXTENDEDATTRIBUTE = OVAL_FAMILY_UNIX + 14,
	OVAL_UNIX_GCONF = OVAL_FAMILY_UNIX + 15,
	OVAL_UNIX_ROUTINGTABLE = OVAL_FAMILY_UNIX + 16,
	OVAL_UNIX_SYMLINK = OVAL_FAMILY_UNIX + 17,

/// Windows subtypes
	OVAL_WINDOWS_ACCESS_TOKEN = OVAL_FAMILY_WINDOWS + 1,
	OVAL_WINDOWS_ACTIVE_DIRECTORY = OVAL_FAMILY_WINDOWS + 2,
	OVAL_WINDOWS_AUDIT_EVENT_POLICY = OVAL_FAMILY_WINDOWS + 3,
	OVAL_WINDOWS_AUDIT_EVENT_POLICY_SUBCATEGORIES = OVAL_FAMILY_WINDOWS + 4,
	OVAL_WINDOWS_FILE = OVAL_FAMILY_WINDOWS + 5,
	OVAL_WINDOWS_FILE_AUDITED_PERMISSIONS_53 = OVAL_FAMILY_WINDOWS + 6,
	OVAL_WINDOWS_FILE_AUDITED_PERMISSIONS = OVAL_FAMILY_WINDOWS + 7,
	OVAL_WINDOWS_FILE_EFFECTIVE_RIGHTS_53 = OVAL_FAMILY_WINDOWS + 8,
	OVAL_WINDOWS_FILE_EFFECTIVE_RIGHTS = OVAL_FAMILY_WINDOWS + 9,
	OVAL_WINDOWS_GROUP = OVAL_FAMILY_WINDOWS + 10,
	OVAL_WINDOWS_GROUP_SID = OVAL_FAMILY_WINDOWS + 11,
	OVAL_WINDOWS_INTERFACE = OVAL_FAMILY_WINDOWS + 12,
	OVAL_WINDOWS_LOCKOUT_POLICY = OVAL_FAMILY_WINDOWS + 13,
	OVAL_WINDOWS_METABASE = OVAL_FAMILY_WINDOWS + 14,
	OVAL_WINDOWS_PASSWORD_POLICY = OVAL_FAMILY_WINDOWS + 15,
	OVAL_WINDOWS_PORT = OVAL_FAMILY_WINDOWS + 16,
	OVAL_WINDOWS_PRINTER_EFFECTIVE_RIGHTS = OVAL_FAMILY_WINDOWS + 17,
	OVAL_WINDOWS_PROCESS = OVAL_FAMILY_WINDOWS + 18,
	OVAL_WINDOWS_REGISTRY = OVAL_FAMILY_WINDOWS + 19,
	OVAL_WINDOWS_REGKEY_AUDITED_PERMISSIONS_53 = OVAL_FAMILY_WINDOWS + 20,
	OVAL_WINDOWS_REGKEY_AUDITED_PERMISSIONS = OVAL_FAMILY_WINDOWS + 21,
	OVAL_WINDOWS_REGKEY_EFFECTIVE_RIGHTS_53 = OVAL_FAMILY_WINDOWS + 22,
	OVAL_WINDOWS_REGKEY_EFFECTIVE_RIGHTS = OVAL_FAMILY_WINDOWS + 23,
	OVAL_WINDOWS_SHARED_RESOURCE = OVAL_FAMILY_WINDOWS + 24,
	OVAL_WINDOWS_SID = OVAL_FAMILY_WINDOWS + 25,
	OVAL_WINDOWS_SID_SID = OVAL_FAMILY_WINDOWS + 26,
	OVAL_WINDOWS_UAC = OVAL_FAMILY_WINDOWS + 27,
	OVAL_WINDOWS_USER = OVAL_FAMILY_WINDOWS + 28,
	OVAL_WINDOWS_USER_SID_55 = OVAL_FAMILY_WINDOWS + 29,
	OVAL_WINDOWS_USER_SID = OVAL_FAMILY_WINDOWS + 30,
	OVAL_WINDOWS_VOLUME = OVAL_FAMILY_WINDOWS + 31,
	OVAL_WINDOWS_WMI = OVAL_FAMILY_WINDOWS + 32,
	OVAL_WINDOWS_WUA_UPDATE_SEARCHER = OVAL_FAMILY_WINDOWS + 33,
	OVAL_WINDOWS_ACTIVE_DIRECTORY_57 = OVAL_FAMILY_WINDOWS + 34,
	OVAL_WINDOWS_CMDLET = OVAL_FAMILY_WINDOWS + 35,
	OVAL_WINDOWS_DNSCACHE = OVAL_FAMILY_WINDOWS + 36,
	OVAL_WINDOWS_LICENSE = OVAL_FAMILY_WINDOWS + 37,
	OVAL_WINDOWS_NTUSER = OVAL_FAMILY_WINDOWS + 38,
	OVAL_WINDOWS_PEHEADER = OVAL_FAMILY_WINDOWS + 39,
	OVAL_WINDOWS_PROCESS_58 = OVAL_FAMILY_WINDOWS + 40,
	OVAL_WINDOWS_SERVICE = OVAL_FAMILY_WINDOWS + 41,
	OVAL_WINDOWS_SERVICE_EFFECTIVE_RIGHTS = OVAL_FAMILY_WINDOWS + 42,
	OVAL_WINDOWS_SHARED_RESOURCE_AUDITED_PERMISSIONS = OVAL_FAMILY_WINDOWS + 43,
	OVAL_WINDOWS_SHARED_RESOURCE_EFFECTIVE_RIGHTS = OVAL_FAMILY_WINDOWS + 44,
	OVAL_WINDOWS_SYSTEM_METRIC = OVAL_FAMILY_WINDOWS + 45,
	OVAL_WINDOWS_USER_RIGHT = OVAL_FAMILY_WINDOWS + 46,
	OVAL_WINDOWS_WMI_57 = OVAL_FAMILY_WINDOWS + 47,
	OVAL_WINDOWS_JUNCTION = OVAL_FAMILY_WINDOWS + 48,

	// deprecated, use OVAL_WINDOWS_AUTID_EVENT_POLICY_SUBCATEGORIES instead
	OVAL_WINDOWS_AUDIT_EVENT_SUBCATEGORIES = OVAL_FAMILY_WINDOWS + 4,
	// deprecated, use OVAL_WINDOWS_UAC instead
	OVAL_WINDOWS_USER_ACCESS_CONTROL = OVAL_FAMILY_WINDOWS + 27,
} oval_subtype_t;

/// severity level
typedef enum {
	XML_VALIDITY_WARNING = 1,
	XML_VALIDITY_ERROR = 2,
	XML_WARNING = 3,
#ifndef _WIN32
	XML_ERROR = 4,
#endif
	OVAL_LOG_INFO = 15,
	OVAL_LOG_DEBUG = 16,
	OVAL_LOG_WARN = 17
} oval_xml_severity_t;

/**
 * @}
 */

/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALRES
 * Interface for Results model
 * @{
 *
 * @typedef oval_result_t
 * Result values for the evaluation of an OVAL Definition or an OVAL Test
 */
typedef enum {
        OVAL_RESULT_TRUE = 1,    /**< Characteristics being evaluated match the information represented in the system characteristic. */
        OVAL_RESULT_FALSE = 2,   /**< Characteristics being evaluated do not match the information represented in the system characteristic. */
        OVAL_RESULT_UNKNOWN = 4, /**< Characteristics being evaluated can not be found in the system characteristic. */
        OVAL_RESULT_ERROR = 8,  /**< Characteristics being evaluated exist in the system characteristic file but there was an error either collecting information or in performing anaylsis. */
        OVAL_RESULT_NOT_EVALUATED = 16, /**< Choice was made not to evaluate the given definition or test. */
        OVAL_RESULT_NOT_APPLICABLE = 32 /**< Definition or test being evaluated is not valid on the given platform. */
} oval_result_t;
/** @} */
/** @} */


/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALVAR
 * Interface to Variable model
 * @{
 *
 * @struct oval_variable_model
 * The OVAL variable model facilitates access to external variable value bindings used to to constrain the evaluation of OVAL objects.
 */
struct oval_variable_model;
/** @} */
/** @} */

typedef enum {
        /**Not valid*/ OVAL_CLASS_UNKNOWN = 0,
        /**Valid*/ OVAL_CLASS_COMPLIANCE = 1,
        /**Valid*/ OVAL_CLASS_INVENTORY = 2,
        /**Valid*/ OVAL_CLASS_MISCELLANEOUS = 3,
        /**Valid*/ OVAL_CLASS_PATCH = 4,
        /**Valid*/ OVAL_CLASS_VULNERABILITY = 5,
} oval_definition_class_t;


#endif				/* OVAL_TYPES_H_ */
