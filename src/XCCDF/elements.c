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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */



#include "elements.h"
#include <string.h>
#include <strings.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

struct xccdf_element_spec {
	enum xccdf_element id;  ///< element ID
	const char* ns;   ///< namespace URI
	const char* name; ///< element name
};

/// CPE version 2 Language XML namespace
#define CPE2L_XMLNS "http://cpe.mitre.org/language/2.0"
/// CPE version 1 dictionary XML namespace
#define CPE1D_XMLNS "http://cpe.mitre.org/XMLSchema/cpe/1.0"

/// Mapping of the element names to the symbolic constants
const struct xccdf_element_spec XCCDF_ELEMENT_MAP[] = {
	{ XCCDFE_BENCHMARK,         XCCDF_XMLNS, "Benchmark"              },
	{ XCCDFE_GROUP,             XCCDF_XMLNS, "Group"                  },
	{ XCCDFE_RULE,              XCCDF_XMLNS, "Rule"                   },
	{ XCCDFE_VALUE,             XCCDF_XMLNS, "Value"                  },
	{ XCCDFE_PROFILE,           XCCDF_XMLNS, "Profile"                },
	{ XCCDFE_TESTRESULT,        XCCDF_XMLNS, "TestResult"             },
	{ XCCDFE_RESULT_BENCHMARK,  XCCDF_XMLNS, "benchmark"              },
	{ XCCDFE_CHECK,             XCCDF_XMLNS, "check"                  },
	{ XCCDFE_CHECK_IMPORT,      XCCDF_XMLNS, "check-import"           },
	{ XCCDFE_CHECK_EXPORT,      XCCDF_XMLNS, "check-export"           },
	{ XCCDFE_CHECK_CONTENT,     XCCDF_XMLNS, "check-content"          },
	{ XCCDFE_CHECK_CONTENT_REF, XCCDF_XMLNS, "check-content-ref"      },
	{ XCCDFE_CHOICES,           XCCDF_XMLNS, "choices"                },
	{ XCCDFE_CHOICE,            XCCDF_XMLNS, "choice"                 },
	{ XCCDFE_COMPLEX_CHECK,     XCCDF_XMLNS, "complex-check"          },
	{ XCCDFE_CONFLICTS,         XCCDF_XMLNS, "conflicts"              },
	{ XCCDFE_CPE_LIST,			CPE1D_XMLNS, "cpe-list"               },
	{ XCCDFE_DEFAULT,           XCCDF_XMLNS, "default"                },
	{ XCCDFE_DESCRIPTION,       XCCDF_XMLNS, "description"            },
	{ XCCDFE_FACT,              XCCDF_XMLNS, "fact"                   },
	{ XCCDFE_FIX,               XCCDF_XMLNS, "fix"                    },
	{ XCCDFE_FIXTEXT,           XCCDF_XMLNS, "fixtext"                },
	{ XCCDFE_FRONT_MATTER,      XCCDF_XMLNS, "front-matter"           },
	{ XCCDFE_IDENT,             XCCDF_XMLNS, "ident"                  },
	{ XCCDFE_IDENTITY,          XCCDF_XMLNS, "identity"               },
	{ XCCDFE_IMPACT_METRIC,     XCCDF_XMLNS, "impact-metric"          },
	{ XCCDFE_INSTANCE,          XCCDF_XMLNS, "instance"               },
	{ XCCDFE_LOWER_BOUND,       XCCDF_XMLNS, "lower-bound"            },
	{ XCCDFE_MATCH,             XCCDF_XMLNS, "match"                  },
	{ XCCDFE_MESSAGE,           XCCDF_XMLNS, "message"                },
	{ XCCDFE_METADATA,          XCCDF_XMLNS, "metadata"               },
	{ XCCDFE_MODEL,             XCCDF_XMLNS, "model"                  },
	{ XCCDFE_NEW_RESULT,        XCCDF_XMLNS, "new-result"             },
	{ XCCDFE_NOTICE,            XCCDF_XMLNS, "notice"                 },
	{ XCCDFE_OLD_RESULT,        XCCDF_XMLNS, "old-result"             },
	{ XCCDFE_ORGANIZATION,      XCCDF_XMLNS, "organization"           },
	{ XCCDFE_OVERRIDE,          XCCDF_XMLNS, "override"               },
	{ XCCDFE_PARAM,             XCCDF_XMLNS, "param"                  },
	{ XCCDFE_PLAIN_TEXT,        XCCDF_XMLNS, "plain-text"             },
	{ XCCDFE_PLATFORM,          XCCDF_XMLNS, "platform"               },
	{ XCCDFE_CPE2_PLATFORMSPEC, CPE2L_XMLNS, "platform-specification" },
	{ XCCDFE_RESULT_PROFILE,    XCCDF_XMLNS, "profile"                },
	{ XCCDFE_PROFILE_NOTE,      XCCDF_XMLNS, "profile-note"           },
	{ XCCDFE_QUESTION,          XCCDF_XMLNS, "question"               },
	{ XCCDFE_RATIONALE,         XCCDF_XMLNS, "rationale"              },
	{ XCCDFE_REAR_MATTER,       XCCDF_XMLNS, "rear-matter"            },
	{ XCCDFE_REFERENCE,         XCCDF_XMLNS, "reference"              },
	{ XCCDFE_REFINE_RULE,       XCCDF_XMLNS, "refine-rule"            },
	{ XCCDFE_REFINE_VALUE,      XCCDF_XMLNS, "refine-value"           },
	{ XCCDFE_REMARK,            XCCDF_XMLNS, "remark"                 },
	{ XCCDFE_REQUIRES,          XCCDF_XMLNS, "requires"               },
	{ XCCDFE_RESULT,            XCCDF_XMLNS, "result"                 },
	{ XCCDFE_RULE_RESULT,       XCCDF_XMLNS, "rule-result"            },
	{ XCCDFE_SCORE,             XCCDF_XMLNS, "score"                  },
	{ XCCDFE_SELECT,            XCCDF_XMLNS, "select"                 },
	{ XCCDFE_SET_VALUE,         XCCDF_XMLNS, "set-value"              },
	{ XCCDFE_SIGNATURE,         XCCDF_XMLNS, "signature"              },
	{ XCCDFE_SOURCE,            XCCDF_XMLNS, "source"                 },
	{ XCCDFE_STATUS,            XCCDF_XMLNS, "status"                 },
	{ XCCDFE_SUB,               XCCDF_XMLNS, "sub"                    },
	{ XCCDFE_TARGET,            XCCDF_XMLNS, "target"                 },
	{ XCCDFE_TARGET_ADDRESS,    XCCDF_XMLNS, "target-address"         },
	{ XCCDFE_TARGET_FACTS,      XCCDF_XMLNS, "target-facts"           },
	{ XCCDFE_TITLE,             XCCDF_XMLNS, "title"                  },
	{ XCCDFE_UPPER_BOUND,       XCCDF_XMLNS, "upper-bound"            },
	{ XCCDFE_VALUE_VAL,         XCCDF_XMLNS, "value"                  },
	{ XCCDFE_VERSION,           XCCDF_XMLNS, "version"                },
	{ XCCDFE_WARNING,           XCCDF_XMLNS, "warning"                },
	{ 0, NULL, NULL }
};

enum xccdf_element xccdf_element_get(xmlTextReaderPtr reader)
{
	if (xmlTextReaderNodeType(reader) != 1)
		return XCCDFE_ERROR;

	const struct xccdf_element_spec* mapptr;

	for (mapptr = XCCDF_ELEMENT_MAP; mapptr->id != 0; ++mapptr) {
		const char* name  = (const char*) xmlTextReaderConstLocalName(reader);
		const char* nsuri = (const char*) xmlTextReaderConstNamespaceUri(reader);
		if ((!name && !nsuri) ||
			(name && nsuri && strcmp(mapptr->name, name) == 0 && strcmp(mapptr->ns, nsuri) == 0))
				return mapptr->id;
	}

	return XCCDFE_UNMATCHED;
}

struct xccdf_attribute_spec {
	enum xccdf_attribute id;  ///< element ID
	const char* ns;   ///< namespace URI
	const char* name; ///< element name
};

const struct xccdf_attribute_spec XCCDF_ATTRIBUTE_MAP[] = {
	{ XCCDFA_ABSTRACT,        XCCDF_XMLNS, "abstract"        },
	{ XCCDFA_AUTHENTICATED,   XCCDF_XMLNS, "authenticated"   },
	{ XCCDFA_AUTHORITY,       XCCDF_XMLNS, "authority"       },
	{ XCCDFA_CATEGORY,        XCCDF_XMLNS, "category"        },
	{ XCCDFA_CLUSTER_ID,      XCCDF_XMLNS, "cluster-id"      },
	{ XCCDFA_COMPLEXITY,      XCCDF_XMLNS, "complexity"      },
	{ XCCDFA_CONTEXT,         XCCDF_XMLNS, "context"         },
	{ XCCDFA_DATE,            XCCDF_XMLNS, "date"            },
	{ XCCDFA_DISRUPTION,      XCCDF_XMLNS, "disruption"      },
	{ XCCDFA_END_TIME,        XCCDF_XMLNS, "end-time"        },
	{ XCCDFA_EXPORT_NAME,     XCCDF_XMLNS, "export-name"     },
	{ XCCDFA_EXTENDS,         XCCDF_XMLNS, "extends"         },
	{ XCCDFA_FIXREF,          XCCDF_XMLNS, "fixref"          },
	{ XCCDFA_HIDDEN,          XCCDF_XMLNS, "hidden"          },
	{ XCCDFA_HREF,            XCCDF_XMLNS, "href"            },
	{ XCCDFA_ID,              XCCDF_XMLNS, "id"              },
	{ XCCDFA_IDREF,           XCCDF_XMLNS, "idref"           },
	{ XCCDFA_IID,             XCCDF_XMLNS, "Id"              },
	{ XCCDFA_IMPORT_NAME,     XCCDF_XMLNS, "import-name"     },
	{ XCCDFA_INTERACTIVE,     XCCDF_XMLNS, "interactive"     },
	{ XCCDFA_INTERFACEHINT,   XCCDF_XMLNS, "interfaceHint"   },
	{ XCCDFA_MULTIPLE,        XCCDF_XMLNS, "multiple"        },
	{ XCCDFA_MUSTMATCH,       XCCDF_XMLNS, "mustMatch"       },
	{ XCCDFA_NAME,            XCCDF_XMLNS, "name"            },
	{ XCCDFA_NEGATE,          XCCDF_XMLNS, "negate"          },
	{ XCCDFA_NOTE_TAG,        XCCDF_XMLNS, "note-tag"        },
	{ XCCDFA_OPERATOR,        XCCDF_XMLNS, "operator"        },
	{ XCCDFA_OVERRIDE,        XCCDF_XMLNS, "override"        },
	{ XCCDFA_PARENTCONTEXT,   XCCDF_XMLNS, "parentContext"   },
	{ XCCDFA_PLATFORM,        XCCDF_XMLNS, "platform"        },
	{ XCCDFA_PRIVILEDGED,     XCCDF_XMLNS, "priviledged"     },
	{ XCCDFA_PROHIBITCHANGES, XCCDF_XMLNS, "prohibitChanges" },
	{ XCCDFA_REBOOT,          XCCDF_XMLNS, "reboot"          },
	{ XCCDFA_RESOLVED,        XCCDF_XMLNS, "resolved"        },
	{ XCCDFA_ROLE,            XCCDF_XMLNS, "role"            },
	{ XCCDFA_SELECTED,        XCCDF_XMLNS, "selected"        },
	{ XCCDFA_SELECTOR,        XCCDF_XMLNS, "selector"        },
	{ XCCDFA_SEVERITY,        XCCDF_XMLNS, "severity"        },
	{ XCCDFA_START_TIME,      XCCDF_XMLNS, "start-time"      },
	{ XCCDFA_STRATEGY,        XCCDF_XMLNS, "strategy"        },
	{ XCCDFA_STYLE,           XCCDF_XMLNS, "style"           },
	{ XCCDFA_STYLE_HREF,      XCCDF_XMLNS, "style-href"      },
	{ XCCDFA_SYSTEM,          XCCDF_XMLNS, "system"          },
	{ XCCDFA_TAG,             XCCDF_XMLNS, "tag"             },
	{ XCCDFA_TEST_SYSTEM,     XCCDF_XMLNS, "test-system"     },
	{ XCCDFA_TIME,            XCCDF_XMLNS, "time"            },
	{ XCCDFA_TYPE,            XCCDF_XMLNS, "type"            },
	{ XCCDFA_UPDATE,          XCCDF_XMLNS, "update"          },
	{ XCCDFA_URI,             XCCDF_XMLNS, "uri"             },
	{ XCCDFA_VALUE_ID,        XCCDF_XMLNS, "value-id"        },
	{ XCCDFA_WEIGHT,          XCCDF_XMLNS, "weight"          },
	{ 0, NULL, NULL }
};

bool xccdf_attribute_has(xmlTextReaderPtr reader, enum xccdf_attribute attr)
{
	return xccdf_attribute_get(reader, attr) != NULL;
}

const char* xccdf_attribute_get(xmlTextReaderPtr reader, enum xccdf_attribute attr)
{
	bool found = false;
	const struct xccdf_attribute_spec* mapptr = XCCDF_ATTRIBUTE_MAP;

	while (mapptr->id) {
		if (attr == mapptr->id) {
			found = true;
			break;
		}
		++mapptr;
	}

	if (!found) return NULL;

	if (xmlTextReaderMoveToAttribute(reader, BAD_CAST mapptr->name) != 1) return NULL;
	// do not check the XML namespace for now... maybe a libxml bug?
	// if (strcmp((const char*)xmlTextReaderConstNamespaceUri(reader), BAD_CAST mapptr->ns) != 0) return NULL;

	return (const char*) xmlTextReaderConstValue(reader);
}

char* xccdf_attribute_copy(xmlTextReaderPtr reader, enum xccdf_attribute attr)
{
	const char* ret = xccdf_attribute_get(reader, attr);
	if (ret) return strdup(ret);
	return NULL;
}

const struct xccdf_string_map XCCDF_BOOL_MAP[] = {
	{ true, "true" }, { true, "True" }, { true, "TRUE" },
	{ true, "yes"  }, { true, "Yes"  }, { true, "YES"  },
	{ true, "1" }, { false, NULL }
};

bool xccdf_attribute_get_bool(xmlTextReaderPtr reader, enum xccdf_attribute attr)
{
	return string_to_enum(XCCDF_BOOL_MAP, xccdf_attribute_get(reader, attr));
}

float xccdf_attribute_get_float(xmlTextReaderPtr reader, enum xccdf_attribute attr)
{
	float res;
	if (xccdf_attribute_has(reader, attr) && sscanf(xccdf_attribute_get(reader, attr), "%f", &res) == 1) return res;
	else return NAN;
}

int string_to_enum(const struct xccdf_string_map* map, const char* str)
{
	while (map->string && (str == NULL || strcmp(map->string, str) != 0)) ++map;
	return map->value;
}

const char* enum_to_string(const struct xccdf_string_map* map, int val)
{
	while (map->string && map->value != val) ++map;
	return map->string;
}

bool xccdf_to_start_element(xmlTextReaderPtr reader, int depth)
{
	//int olddepth = xmlTextReaderDepth(reader);
	while (xmlTextReaderDepth(reader) >= depth) {
		switch (xmlTextReaderNodeType(reader)) {
			case 1:
				if (xmlTextReaderDepth(reader) == depth)
					return true;
			default: break;
		}
		if (xmlTextReaderRead(reader) != 1) break;
	}
	return false;
}

char* xccdf_element_string_copy(xmlTextReaderPtr reader)
{
	if (xmlTextReaderNodeType(reader) == 1 || xmlTextReaderNodeType(reader) == 2)
		xmlTextReaderRead(reader);
	if (xmlTextReaderHasValue(reader))
		return (char*) xmlTextReaderValue(reader);
	return NULL;
}

const char* xccdf_element_string_get(xmlTextReaderPtr reader)
{
	if (xmlTextReaderNodeType(reader) == 1 || xmlTextReaderNodeType(reader) == 2)
		xmlTextReaderRead(reader);
	if (xmlTextReaderHasValue(reader))
		return (const char*) xmlTextReaderConstValue(reader);
	return NULL;
}

int xccdf_element_depth(xmlTextReaderPtr reader)
{
	int depth = xmlTextReaderDepth(reader);
	switch (xmlTextReaderNodeType(reader)) {
		case 2: case 5: case 3: return depth - 1;
		default: return depth;
	}
}

void xccdf_print_depth(int depth)
{
	while (depth--) printf("  ");
}

void xccdf_print_max(const char* str, int max, const char* ellipsis)
{
    if (str) while (isspace(*str)) ++str;
	int len = strlen("(null)");;
	char buf[32];
	if (str) len = strlen(str);
	sprintf(buf, "%%.%ds", max);
	printf(buf, str);
	if (len > max) printf("%s", ellipsis);
}

char* xccdf_get_xml(xmlTextReaderPtr reader)
{
	return (char*) xmlTextReaderReadOuterXml(reader);
}

time_t xccdf_get_date(const char* date)
{
    if (date) {
        struct tm tm;
        memset(&tm, 0, sizeof(tm));
        if (sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) == 3) {
            tm.tm_mon -= 1;
            tm.tm_year -= 1900;
            return mktime(&tm);
        }
    }
    return 0;
}

time_t xccdf_get_datetime(const char* date)
{
    if (date) {
        struct tm tm;
        memset(&tm, 0, sizeof(tm));
        if (sscanf(date, "%d-%d-%dT%d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6) {
            tm.tm_mon -= 1;
            tm.tm_year -= 1900;
            return mktime(&tm);
        }
    }
    return 0;
}
