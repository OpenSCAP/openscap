/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <libxml/tree.h>

#include "util.h"
#include "oscap_buffer.h"
#include "xml_iterate.h"
#include "debug_priv.h"
#include "_error.h"
#include "XCCDF/elements.h"
#include "XCCDF/xccdf_impl.h"
#include "xccdf_policy_priv.h"
#include "public/xccdf_policy.h"

struct _xccdf_text_substitution_data {
	struct xccdf_policy *policy;
	struct xccdf_rule_result *rule_result;
	bool ansible;
	enum {
		_TAILORING_TYPE = 1,
		_DOCUMENT_GENERATION_TYPE = 2,
		_ASSESSMENT_TYPE = 4
	} processing_type;		// Defines behaviour for fix/@use="legacy"
	// TODO: this shall carry also the @xml:lang.
	// TODO: This shall carry also xccdf:TestResult for xccdf:fact resolution
};

static bool _xhtml_is_supported_namespace(xmlNs *ns)
{
	return ns != NULL && oscap_streq((const char *) ns->href, (const char *) XCCDF_XHTML_NAMESPACE);
}

/* Quote a standalone, explicitly tagged Ansible string before interpolation.
 * At this point we can distinguish literal XCCDF data from YAML syntax supplied
 * by the template. Quoting afterwards would confuse values containing quotes
 * with templates that already quote their substitutions.
 */
static char *_ansible_quote_string(xmlNode *node, const char *value)
{
	if (node->prev == NULL || node->prev->type != XML_TEXT_NODE)
		return NULL;
	const char *prefix = (const char *) node->prev->content;
	const char *line = strrchr(prefix, '\n');
	line = line == NULL ? prefix : line + 1;
	while (*line == ' ' || *line == '\t')
		line++;
	/* Restrict this to a mapping value, not text inside a quoted scalar. */
	while ((*line >= 'a' && *line <= 'z') || (*line >= 'A' && *line <= 'Z') ||
			(*line >= '0' && *line <= '9') || *line == '_')
		line++;
	if (strcmp(line, ": !!str ") != 0)
		return NULL;
	if (node->next != NULL && (node->next->type != XML_TEXT_NODE ||
			(node->next->content[0] != '\n' && node->next->content[0] != '\0')))
		return NULL;

	struct oscap_buffer *quoted = oscap_buffer_new();
	oscap_buffer_append_string(quoted, "\"");
	for (const char *c = value; *c != '\0'; c++) {
		/* YAML treats these UTF-8 characters as line breaks even in quotes. */
		if ((unsigned char)c[0] == 0xc2 && (unsigned char)c[1] == 0x85) {
			oscap_buffer_append_string(quoted, "\\N");
			c++;
			continue;
		}
		if ((unsigned char)c[0] == 0xe2 && (unsigned char)c[1] == 0x80 &&
				((unsigned char)c[2] == 0xa8 || (unsigned char)c[2] == 0xa9)) {
			oscap_buffer_append_string(quoted, (unsigned char)c[2] == 0xa8 ? "\\L" : "\\P");
			c += 2;
			continue;
		}
		switch (*c) {
		case '"': oscap_buffer_append_string(quoted, "\\\""); break;
		case '\\': oscap_buffer_append_string(quoted, "\\\\"); break;
		case '\n': oscap_buffer_append_string(quoted, "\\n"); break;
		case '\r': oscap_buffer_append_string(quoted, "\\r"); break;
		case '\t': oscap_buffer_append_string(quoted, "\\t"); break;
		default: oscap_buffer_append_binary_data(quoted, c, 1);
		}
	}
	oscap_buffer_append_string(quoted, "\"");
	return oscap_buffer_bequeath(quoted);
}

static int _xccdf_text_substitution_cb(xmlNode **node, void *user_data)
{
	struct _xccdf_text_substitution_data *data = (struct _xccdf_text_substitution_data *) user_data;
	if (node == NULL || *node == NULL || data == NULL)
		return 1;

	if (oscap_streq((const char *) (*node)->name, "sub") && xccdf_is_supported_namespace((*node)->ns)) {
		if ((*node)->children != NULL)
			dW("The xccdf:sub element SHALL NOT have any content.");
		char *sub_idref = (char *) xmlGetProp(*node, BAD_CAST "idref");
		if (oscap_streq(sub_idref, NULL)) {
			oscap_seterr(OSCAP_EFAMILY_XCCDF, "The xccdf:sub MUST have a single @idref attribute.");
			free(sub_idref); // It may be an empty string.
			return 2;
		}
		// Sub element may refer to xccdf:Value or to xccdf:plain-text

		struct xccdf_benchmark *benchmark = xccdf_policy_get_benchmark(data->policy);
		if (benchmark == NULL)
			return 1;
		struct xccdf_item *value = xccdf_benchmark_get_item(benchmark, sub_idref);

		const char *result = NULL;
		if (value != NULL && xccdf_item_get_type(value) == XCCDF_VALUE) {
			// When the <xccdf:sub> element's @idref attribute holds the id of an <xccdf:Value>
			// element, the <xccdf:sub> element's @use attribute MUST be consulted.
			char *sub_use = (char *) xmlGetProp(*node, BAD_CAST "use");
			if (oscap_streq(sub_use, NULL) || oscap_streq(sub_use, "legacy")) {
				// If the value of the @use attribute is "legacy", then during Tailoring,
				// process the <xccdf:sub> element as if @use was set to "title". but
				// during Document Generation or Assessment, process the <xccdf:sub>
				// element as if @use was set to "value".
				free(sub_use);
				sub_use = oscap_strdup((data->processing_type & _TAILORING_TYPE) ? "title" : "value");
			}

			if (oscap_streq(sub_use, "title")) {
				// TODO: @xml:lang
				struct oscap_text_iterator *title_it = xccdf_item_get_title(value);
				if (oscap_text_iterator_has_more(title_it))
					result = oscap_text_get_text(oscap_text_iterator_next(title_it));
				oscap_text_iterator_free(title_it);
			} else {
				if (!oscap_streq(sub_use, "value"))
					dW("xccdf:sub/@idref='%s' has incorrect @use='%s'! Using @use='value' instead.", sub_idref, sub_use);
				result = xccdf_policy_get_value_of_item(data->policy, value);
			}
			free(sub_use);
		} else { // This xccdf:sub probably refers to the xccdf:plain-text
			result = xccdf_benchmark_get_plain_text(benchmark, sub_idref);
		}

		if (result == NULL) {
			oscap_seterr(OSCAP_EFAMILY_XCCDF, "Could not resolve xccdf:sub/@idref='%s'!", sub_idref);
			free(sub_idref);
			return 2;
		}
		free(sub_idref);

		char *quoted = data->ansible ? _ansible_quote_string(*node, result) : NULL;
		xmlNode *new_node = xmlNewText(BAD_CAST (quoted != NULL ? quoted : result));
		free(quoted);
		xmlReplaceNode(*node, new_node);
		xmlFreeNode(*node);
		*node = new_node;
		return 0;
	} else if (oscap_streq((const char *) (*node)->name, "object") && _xhtml_is_supported_namespace((*node)->ns)) {
		char *object_data = (char *) xmlGetProp(*node, BAD_CAST "data");
		if (object_data == NULL || strncmp(object_data, "#xccdf:", strlen("#xccdf:")) != 0) {
			free(object_data);
			return 0; // Not an error, unless it shall be resolved by XCCDF
		}

		struct xccdf_benchmark *benchmark = xccdf_policy_get_benchmark(data->policy);
		if (benchmark == NULL)
			return 1;

		const char *result = NULL;
		if (strncmp(object_data, "#xccdf:value:", strlen("#xccdf:value:")) == 0) {
			const char *value_id = object_data + strlen("#xccdf:value:");

			struct xccdf_item *item = xccdf_benchmark_get_item(benchmark, value_id);
			if (item != NULL && xccdf_item_get_type(item) == XCCDF_VALUE) {
				result = xccdf_policy_get_value_of_item(data->policy, item);
			} else {
				result = xccdf_benchmark_get_plain_text(benchmark, value_id);
				if (result == NULL) {
					dW("Text substitution for xccdf:fact is not supported!"); // TODO.
				}
			}
		}
		else if (strncmp(object_data, "#xccdf:title:", strlen("#xccdf:title:")) == 0) {
			const char *title_id = object_data + strlen("#xccdf:title:");

			struct xccdf_item *item = xccdf_benchmark_get_item(benchmark, title_id);
			if (item != NULL) {
				// TODO: @xml:lang
				struct oscap_text_iterator *title_it = xccdf_item_get_title(item);
				if (oscap_text_iterator_has_more(title_it))
					result = oscap_text_get_text(oscap_text_iterator_next(title_it));
				oscap_text_iterator_free(title_it);
			}
		}
		else {
			// Let's not consider this as an error. Since in similar cases NISTIR-7275r4
			// suggests to retain the <object> element.
			dW("Unsupported XCCDF uri: xhtml:object/@data='%s'", object_data);
			free(object_data);
			return 0;
		}
		free(object_data);
		xmlNode *new_node = xmlNewText(BAD_CAST result);
		xmlReplaceNode(*node, new_node);
		xmlFreeNode(*node);
		*node = new_node;
		return 0;
	} else if (oscap_streq((const char *) (*node)->name, "instance") && xccdf_is_supported_namespace((*node)->ns)) {
		const char *result;
		// <instance> elements
		if ((*node)->children != NULL)
			dW("The xccdf:instance element SHALL NOT have any content.");
		if (data->rule_result == NULL)
			return 1;
		struct xccdf_instance_iterator *instances = xccdf_rule_result_get_instances(data->rule_result);
		if (xccdf_instance_iterator_has_more(instances)) {
			struct xccdf_instance *instance = xccdf_instance_iterator_next(instances);
			result = xccdf_instance_get_content(instance);
			xccdf_instance_iterator_free(instances);
		}
		else {
			xccdf_instance_iterator_free(instances);
			dW("The xccdf:rule-result/xccdf:instance element was not found.");
			return 1;
		}
		xmlNode *new_node = xmlNewText(BAD_CAST result);
		xmlReplaceNode(*node, new_node);
		xmlFreeNode(*node);
		*node = new_node;
		return 0;
	} else {
		return 0;
	}
}

int xccdf_policy_resolve_fix_substitution(struct xccdf_policy *policy, struct xccdf_fix *fix, struct xccdf_rule_result *rule_result, struct xccdf_result *test_result)
{
	struct _xccdf_text_substitution_data data;
	data.policy = policy;
	data.processing_type = _DOCUMENT_GENERATION_TYPE | _ASSESSMENT_TYPE;
	data.rule_result = rule_result;
	data.ansible = oscap_streq(xccdf_fix_get_system(fix), "urn:xccdf:fix:script:ansible");

	char *result = NULL;
	int res = xml_iterate_dfs(xccdf_fix_get_content(fix), &result, _xccdf_text_substitution_cb, &data);
	if (res == 0)
		xccdf_fix_set_content(fix, result);
	free(result);
	return res;
}

char* xccdf_policy_substitute(const char *text, struct xccdf_policy *policy) {
	struct _xccdf_text_substitution_data data;
	data.policy = policy;
	data.rule_result = NULL;
	data.ansible = false;
	/* We cannot anticipate processing type. But <title>'s are least probable. */
	data.processing_type = _DOCUMENT_GENERATION_TYPE | _ASSESSMENT_TYPE;

	char *resolved_text = NULL;
	if (xml_iterate_dfs(text, &resolved_text, _xccdf_text_substitution_cb, &data) != 0) {
		// Either warning or error occurred. Since prototype of this function
		// does not make possible warning notification -> We better scratch that.
		free(resolved_text);
		resolved_text = NULL;
	}
	return resolved_text;
}
