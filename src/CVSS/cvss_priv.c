/*! \file cvss_priv.c
 *  \brief Interface to Common Vulnerability Scoring System Version 2
 * 
 *  See details at http://nvd.nist.gov/cvss.cfm
 *  
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
 *      Maros Barabas <mbarabas@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <string.h>

#include "public/cvss.h"
#include "cvss_priv.h"

#include "common/list.h"
#include "common/_error.h"

/***************************************************************************/
/* Variable definitions
 * */

/*
 */
struct cvss_entry {

        char *score;
        char *AV;
        char *AC;
        char *authentication;
        /* impacts */
        char *imp_confidentiality;
        char *imp_integrity;
        char *imp_availability;
        /**/
        char *source;
        char *generated;
};
OSCAP_ACCESSOR_STRING(cvss_entry, score)
OSCAP_ACCESSOR_STRING(cvss_entry, AV)
OSCAP_ACCESSOR_STRING(cvss_entry, AC)
OSCAP_ACCESSOR_STRING(cvss_entry, authentication)
OSCAP_ACCESSOR_STRING(cvss_entry, imp_confidentiality)
OSCAP_ACCESSOR_STRING(cvss_entry, imp_integrity)
OSCAP_ACCESSOR_STRING(cvss_entry, imp_availability)
OSCAP_ACCESSOR_STRING(cvss_entry, source)
OSCAP_ACCESSOR_STRING(cvss_entry, generated)

struct cvss_map_av {
        cvss_access_vector_t id;
        const char *name;
};
static const struct cvss_map_av CVSS_MAP_AV[] = {
        {AV_LOCAL,              "LOCAL"},
        {AV_ADJACENT_NETWORK,   "ADJACENT_NETWORK"},
        {AV_NETWORK,            "NETWORK"}
};

struct cvss_map_ac {
        cvss_access_complexity_t id;
        const char *name;
};
static const struct cvss_map_ac CVSS_MAP_AC[] = {
        {AC_HIGH,   "HIGH"},
        {AC_MEDIUM, "MEDIUM"},
        {AC_LOW,    "LOW"}
};

struct cvss_map_auth {
        cvss_authentication_t id;
        const char *name;
};
static const struct cvss_map_auth CVSS_MAP_AUTH[] = {
        {AU_NONE,               "NONE"},
        {AU_SINGLE_INSTANCE,    "SINGLE_INSTANCE"},
        {AU_MULTIPLE_INSTANCE,  "MULTIPLE_INSTANCE"}
};

/* cvss_avail_impact_t is the same as other impacts
 * -- TODO: for all impacts separate structures 
 */
struct cvss_map_imp {
        cvss_avail_impact_t id;
        const char *name;
};
static const struct cvss_map_imp CVSS_MAP_IMP[] = {
        {AI_NONE,       "NONE"},
        {AI_PARTIAL,    "PARTIAL"},
        {AI_COMPLETE,   "COMPLETE"}
};

/* End of variable definitions
 * */
/***************************************************************************/

/***************************************************************************/
/* XML string variables definitions
 * */
#define TAG_CVSS_STR BAD_CAST "cvss"
#define TAG_BASE_METRICS_STR BAD_CAST "base_metrics"
#define TAG_SCORE_STR BAD_CAST "score"
#define TAG_ACCESS_VECTOR_STR BAD_CAST "access-vector"
#define TAG_ACCESS_COMPLEXITY_STR BAD_CAST "access-complexity"
#define TAG_AUTHENTICATION_STR BAD_CAST "authentication"
#define TAG_CONFIDENTIALITY_IMPACT_STR BAD_CAST "confidentiality-impact"
#define TAG_INTEGRITY_IMPACT_STR BAD_CAST "integrity-impact"
#define TAG_AVAILABILITY_IMPACT_STR BAD_CAST "availability-impact"
#define TAG_GENERATED_ON_DATETIME_STR BAD_CAST "generated-on-datetime"
#define TAG_SOURCE_STR BAD_CAST "source"

#define NS_VULN_STR BAD_CAST "vuln"
#define NS_CVSS_STR BAD_CAST "cvss"
#define NS_VULN_URI BAD_CAST "http://scap.nist.gov/schema/vulnerability/0.4"
#define NS_CVSS_URI BAD_CAST "http://scap.nist.gov/schema/cvss-v2/0.2"
/* End of XML string variables definitions
 * */
/***************************************************************************/
int cvss_map_av_get(const char * string) {

        __attribute__nonnull__(string);

	const struct cvss_map_av* map = CVSS_MAP_AV;
	while (map->name) {
		if (!strcmp(string, map->name))
	                return map->id;
		++map;
	}
	return -1;
}
int cvss_map_ac_get(const char * string) {

        __attribute__nonnull__(string);

	const struct cvss_map_ac* map = CVSS_MAP_AC;
	while (map->name) {
		if (!strcmp(string, map->name))
	                return map->id;
		++map;
	}
	return -1;
}
int cvss_map_auth_get(const char * string) {

        __attribute__nonnull__(string);

	const struct cvss_map_auth* map = CVSS_MAP_AUTH;
	while (map->name) {
		if (!strcmp(string, map->name))
	                return map->id;
		++map;
	}
	return -1;
}
int cvss_map_imp_get(const char * string) {

        __attribute__nonnull__(string);

	const struct cvss_map_imp* map = CVSS_MAP_IMP;
	while (map->name) {
		if (!strcmp(string, map->name))
	                return map->id;
		++map;
	}
	return -1;
}

/***************************************************************************/
/* Constructors of CVSS structures cvss_*<structure>*_new()
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cvss_entry * cvss_entry_new() {

        struct cvss_entry *ret;

        ret = oscap_alloc(sizeof(struct cvss_entry));
        if (ret == NULL)
                return NULL;

        ret->score                  = NULL;
        ret->AV                     = NULL;
        ret->AC                     = NULL;
        ret->authentication         = NULL;
        ret->imp_confidentiality    = NULL;
        ret->imp_integrity          = NULL;
        ret->imp_availability       = NULL;
        ret->source                 = NULL;
        ret->generated              = NULL;

        return ret;
}

struct cvss_entry * cvss_entry_clone(struct cvss_entry * old_entry)
{
        struct cvss_entry * new_entry = cvss_entry_new();
        if ( new_entry == NULL ) 
            return NULL;

        new_entry->score = oscap_strdup(old_entry->score);
        new_entry->AV = oscap_strdup(old_entry->AV);
        new_entry->AC = oscap_strdup(old_entry->AC);
        new_entry->authentication = oscap_strdup(old_entry->authentication);
        new_entry->imp_confidentiality = oscap_strdup(old_entry->imp_confidentiality);
        new_entry->imp_integrity = oscap_strdup(old_entry->imp_integrity);
        new_entry->imp_availability = oscap_strdup(old_entry->imp_availability);
        new_entry->source = oscap_strdup(old_entry->source);
        new_entry->generated = oscap_strdup(old_entry->generated);

        return new_entry;
}

/* End of CVE structures' contructors
 * */
/***************************************************************************/

/***************************************************************************/
/* Declaration of static (private to this file) functions
 * These function shoud not be called from outside. For exporting these elements
 * has to call parent element's 
 */
void cvss_entry_free(struct cvss_entry * entry);

/* End of static declarations 
 * */
/***************************************************************************/

/* Function that jump to next XML starting element.
 */
static int xmlTextReaderNextElement(xmlTextReaderPtr reader) {

        __attribute__nonnull__(reader);

        int ret;
        do { 
              ret = xmlTextReaderRead(reader); 
              /* if end of file */
              if (ret == 0) break;
        } while (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
        return ret;
}


/***************************************************************************/
/* Private parsing functions cve_*<structure>*_parse( xmlTextReaderPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cvss_entry * cvss_entry_parse(xmlTextReaderPtr reader) {

        __attribute__nonnull__(reader);

        struct cvss_entry *ret = NULL;

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVSS_STR) &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                xmlTextReaderNextElement(reader); /* We are on base_metrics now */

                if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BASE_METRICS_STR))
                    /* we want to end if there are no bas-metrics ! What else could be here ? */
                    /**/
                    return NULL;
                
                /* skip nodes until new element */
                xmlTextReaderNextElement(reader); /* We are on score now */

                ret = cvss_entry_new();
                if (ret == NULL)
                        return NULL;

                /* ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader)); */
                /* ret->xml.namespace = (char *) xmlTextReaderPrefix(reader); */

                /* CVSS-specification: score */ 
                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), TAG_CVSS_STR) != 0) {
                        
                    if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_SCORE_STR) &&
                        xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                ret->score = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ACCESS_VECTOR_STR) &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->AV = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ACCESS_COMPLEXITY_STR) &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->AC = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_AUTHENTICATION_STR) &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->authentication = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CONFIDENTIALITY_IMPACT_STR) &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->imp_confidentiality = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_INTEGRITY_IMPACT_STR) &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->imp_integrity = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_AVAILABILITY_IMPACT_STR) &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->imp_availability = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_SOURCE_STR) &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->source = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATED_ON_DATETIME_STR) &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->generated = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                            oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_EXMLELEM, "Unknown XML element in CVSS element");
                    }

                    xmlTextReaderRead(reader);
                }
        }

        return ret;
}

/***************************************************************************/
/* Private exporting functions cvss_*<structure>*_export( xmlTextWriterPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
void cvss_entry_export(const struct cvss_entry * entry, xmlTextWriterPtr writer) {

        __attribute__nonnull__(entry);
        __attribute__nonnull__(writer);

        xmlTextWriterStartElementNS(writer, NULL, TAG_CVSS_STR, NS_VULN_URI);
        xmlTextWriterStartElementNS(writer, NULL, TAG_BASE_METRICS_STR, NS_CVSS_URI);

        if ((entry->score) != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, TAG_SCORE_STR, NULL, 
                                                      BAD_CAST entry->score);
        if ((entry->AV) != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, TAG_ACCESS_VECTOR_STR, NULL, 
                                                         BAD_CAST entry->AV);
        if ((entry->AC) != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, TAG_ACCESS_COMPLEXITY_STR, NULL, 
                                                         BAD_CAST entry->AC);
        if ((entry->authentication) != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, TAG_AUTHENTICATION_STR, NULL, 
                                                         BAD_CAST entry->authentication);
        if ((entry->imp_confidentiality) != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, TAG_CONFIDENTIALITY_IMPACT_STR, NULL, 
                                                      BAD_CAST entry->imp_confidentiality);
        if ((entry->imp_integrity) != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, TAG_INTEGRITY_IMPACT_STR, NULL, 
                                                         BAD_CAST entry->imp_integrity);
        if ((entry->imp_availability) != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, TAG_AVAILABILITY_IMPACT_STR, NULL, 
                                                         BAD_CAST entry->imp_availability);
        if ((entry->source) != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, TAG_SOURCE_STR, NULL, 
                                                         BAD_CAST entry->source);
        if ((entry->generated) != NULL)
            xmlTextWriterWriteElementNS(writer, NULL, TAG_GENERATED_ON_DATETIME_STR, NULL, 
                                                                    BAD_CAST entry->generated);

        /*</base-metrics>*/
        xmlTextWriterEndElement(writer);
        /*</cvss>*/
        xmlTextWriterEndElement(writer);
        if (xmlGetLastError() != NULL) oscap_setxmlerr(xmlGetLastError());

}

/***************************************************************************/
/* Free functions - all will free their subtree so use carefuly !
 */
void cvss_entry_free(struct cvss_entry * entry) {

        if (entry == NULL) return;

        xmlFree(entry->score);
        xmlFree(entry->AV);
        xmlFree(entry->AC);
        xmlFree(entry->authentication);
        xmlFree(entry->imp_confidentiality);
        xmlFree(entry->imp_integrity);
        xmlFree(entry->imp_availability);
        xmlFree(entry->source);
        xmlFree(entry->generated);
        oscap_free(entry);
}
/* End of free functions
 * */
/***************************************************************************/


/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////  NEW API  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

