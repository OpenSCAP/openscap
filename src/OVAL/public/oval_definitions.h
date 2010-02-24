/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALDEF OVAL Definitions
 * Public interface for OVAL Definitions.
 * @{
 *
 * Class diagram
 * \image html definition_model.png
 *
 * @file
 *
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */


#ifndef OVAL_DEFINITIONS
#define OVAL_DEFINITIONS

#include "oval_types.h"
#include <stdbool.h>


/**
 * Affected family enumeration.
 * Since new family values can only be added with new version of the schema,
 * the value OVAL_AFCFML_UNDEFINED is to be used when the desired family is not available
 */
typedef enum {
	OVAL_AFCFML_UNKNOWN = 0,
	OVAL_AFCFML_CATOS = 1,
	OVAL_AFCFML_IOS = 2,
	OVAL_AFCFML_MACOS = 3,
	OVAL_AFCFML_PIXOS = 4,
	OVAL_AFCFML_UNDEFINED = 5,
	OVAL_AFCFML_UNIX = 6,
	OVAL_AFCFML_WINDOWS = 7
} oval_affected_family_t;

/// Criteria node type
typedef enum {
	OVAL_NODETYPE_UNKNOWN = 0,
	OVAL_NODETYPE_CRITERIA = 1,
	OVAL_NODETYPE_CRITERION = 2,
	OVAL_NODETYPE_EXTENDDEF = 3
} oval_criteria_node_type_t;

/// Boolean operators
typedef enum {
	OVAL_OPERATOR_UNKNOWN = 0,
	OVAL_OPERATOR_AND = 1,
	OVAL_OPERATOR_ONE = 2,
	OVAL_OPERATOR_OR = 3,
	OVAL_OPERATOR_XOR = 4
} oval_operator_t;

/// Operations
typedef enum {
	OVAL_OPERATION_UNKNOWN = 0,
	OVAL_OPERATION_EQUALS = 1,
	OVAL_OPERATION_NOT_EQUAL = 2,
	OVAL_OPERATION_CASE_INSENSITIVE_EQUALS = 3,
	OVAL_OPERATION_CASE_INSENSITIVE_NOT_EQUAL = 4,
	OVAL_OPERATION_GREATER_THAN = 5,
	OVAL_OPERATION_LESS_THAN = 6,
	OVAL_OPERATION_GREATER_THAN_OR_EQUAL = 7,
	OVAL_OPERATION_LESS_THAN_OR_EQUAL = 8,
	OVAL_OPERATION_BITWISE_AND = 9,
	OVAL_OPERATION_BITWISE_OR = 10,
	OVAL_OPERATION_PATTERN_MATCH = 11
} oval_operation_t;

/// Definition classes
typedef enum {
	/**Not valid*/ OVAL_CLASS_UNKNOWN = 0,
	/**Valid*/ OVAL_CLASS_COMPLIANCE = 1,
	/**Valid*/ OVAL_CLASS_INVENTORY = 2,
	/**Valid*/ OVAL_CLASS_MISCELLANEOUS = 3,
	/**Valid*/ OVAL_CLASS_PATCH = 4,
	/**Valid*/ OVAL_CLASS_VULNERABILITY = 5,
} oval_definition_class_t;

/// Existence check enumeration
typedef enum {
	OVAL_EXISTENCE_UNKNOWN = 0,
	OVAL_ALL_EXIST = 1,
	OVAL_ANY_EXIST = 2,
	OVAL_AT_LEAST_ONE_EXISTS = 3,
	OVAL_ONLY_ONE_EXISTS = 4,
	OVAL_NONE_EXIST = 5
} oval_existence_t;

/// Check enumeration
typedef enum {
	OVAL_CHECK_UNKNOWN = 0,
	OVAL_CHECK_ALL = 1,
	OVAL_CHECK_AT_LEAST_ONE = 2,
	OVAL_CHECK_NONE_EXIST = 3,
	OVAL_CHECK_NONE_SATISFY = 4,
	OVAL_CHECK_ONLY_ONE = 5
} oval_check_t;

/// Type of object content
typedef enum {
	OVAL_OBJECTCONTENT_UNKNOWN = 0,
	OVAL_OBJECTCONTENT_ENTITY = 1,
	OVAL_OBJECTCONTENT_SET = 2
} oval_object_content_type_t;

/// Type of OVAL entity
typedef enum {
	OVAL_ENTITY_TYPE_UNKNOWN,
	OVAL_ENTITY_TYPE_ANY,
	OVAL_ENTITY_TYPE_BINARY,
	OVAL_ENTITY_TYPE_BOOLEAN,
	OVAL_ENTITY_TYPE_FLOAT,
	OVAL_ENTITY_TYPE_INTEGER,
	OVAL_ENTITY_TYPE_STRING,
} oval_entity_type_t;

/// Datatypes
typedef enum {
	OVAL_DATATYPE_UNKNOWN = 0,
	OVAL_DATATYPE_BINARY = 1,
	OVAL_DATATYPE_BOOLEAN = 2,
	OVAL_DATATYPE_EVR_STRING = 3,
	OVAL_DATATYPE_FILESET_REVISION = 4,
	OVAL_DATATYPE_FLOAT = 5,
	OVAL_DATATYPE_IOS_VERSION = 6,
	OVAL_DATATYPE_VERSION = 7,
	OVAL_DATATYPE_INTEGER = 8,
	OVAL_DATATYPE_STRING = 9
} oval_datatype_t;

/// Varref types
typedef enum {
	OVAL_ENTITY_VARREF_UNKNOWN,
	OVAL_ENTITY_VARREF_NONE,
	OVAL_ENTITY_VARREF_ATTRIBUTE,
	OVAL_ENTITY_VARREF_ELEMENT
} oval_entity_varref_type_t;

/// OVAL set object types
typedef enum {
	OVAL_SET_UNKNOWN,
	OVAL_SET_AGGREGATE,
	OVAL_SET_COLLECTIVE
} oval_setobject_type_t;

/// Set operations
typedef enum {
	OVAL_SET_OPERATION_UNKNOWN,
	OVAL_SET_OPERATION_COMPLEMENT = 1,
	OVAL_SET_OPERATION_INTERSECTION = 2,
	OVAL_SET_OPERATION_UNION = 3
} oval_setobject_operation_t;

/// OVAL variable types
typedef enum {
	OVAL_VARIABLE_UNKNOWN,
	OVAL_VARIABLE_EXTERNAL,
	OVAL_VARIABLE_CONSTANT,
	OVAL_VARIABLE_LOCAL
} oval_variable_type_t;

#define OVAL_FUNCTION 10
/// Component types
typedef enum {
	OVAL_COMPONENT_UNKNOWN = 0,
	OVAL_COMPONENT_LITERAL = 1,
	OVAL_COMPONENT_OBJECTREF = 2,
	OVAL_COMPONENT_VARREF = 3,
	OVAL_COMPONENT_FUNCTION = 4,
	OVAL_FUNCTION_BEGIN = OVAL_FUNCTION + 1,
	OVAL_FUNCTION_CONCAT = OVAL_FUNCTION + 2,
	OVAL_FUNCTION_END = OVAL_FUNCTION + 3,
	OVAL_FUNCTION_SPLIT = OVAL_FUNCTION + 4,
	OVAL_FUNCTION_SUBSTRING = OVAL_FUNCTION + 5,
	OVAL_FUNCTION_TIMEDIF = OVAL_FUNCTION + 6,
	OVAL_FUNCTION_ESCAPE_REGEX = OVAL_FUNCTION + 7,
	OVAL_FUNCTION_REGEX_CAPTURE = OVAL_FUNCTION + 8,
	OVAL_FUNCTION_ARITHMETIC = OVAL_FUNCTION + 9
} oval_component_type_t;

/// Arithmetic format enumeration
typedef enum {
	OVAL_ARITHMETIC_UNKNOWN = 0,
	OVAL_ARITHMETIC_ADD = 1,
	OVAL_ARITHMETIC_MULTIPLY = 2,
	OVAL_ARITHMETIC_SUBTRACT = 3,	//NOT YET SUPPORTED BY OVAL
	OVAL_ARITHMETIC_DIVIDE = 4	//NOT YET SUPPORTED BY OVAL
} oval_arithmetic_operation_t;

/// Datetime format enumeration.
typedef enum {
	OVAL_DATETIME_UNKNOWN = 0,
	OVAL_DATETIME_YEAR_MONTH_DAY = 1,
	OVAL_DATETIME_MONTH_DAY_YEAR = 2,
	OVAL_DATETIME_DAY_MONTH_YEAR = 3,
	OVAL_DATETIME_WIN_FILETIME = 4,
	OVAL_DATETIME_SECONDS_SINCE_EPOCH = 5
} oval_datetime_format_t;




/**
 * Get the family associated with a given subtype.
 */
oval_family_t oval_subtype_get_family(oval_subtype_t);
const char *oval_operator_get_text(oval_operator_t);
const char *oval_subtype_get_text(oval_subtype_t);
const char *oval_family_get_text(oval_family_t);
const char *oval_check_get_text(oval_check_t);
const char *oval_existence_get_text(oval_existence_t);
const char *oval_affected_family_get_text(oval_affected_family_t);
const char *oval_datatype_get_text(oval_datatype_t);
const char *oval_operation_get_text(oval_operation_t);
const char *oval_set_operation_get_text(oval_setobject_operation_t);
const char *oval_datetime_format_get_text(oval_datetime_format_t);
const char *oval_arithmetic_operation_get_text(oval_arithmetic_operation_t);




/**
 * @struct oval_definition_model
 * Oval definition model
 */
struct oval_definition_model;

/**
 * @struct oval_definition
 * Oval definition specification.
 * A definition is the key structure in OVAL. It is analogous to the logical sentence or proposition:
 * if a computer's state matches the configuration parameters laid out in the criteria, then that computer exhibits the state described.
 */
struct oval_definition;
/**
 * @struct oval_definition_iterator
 * @see oval_definition_model_get_definitions
 */
struct oval_definition_iterator;
/**
 * @struct oval_test
 * An Oval_test specifies a technical control by identifying an @ref oval_object that is evaluated on an instrumented host platform and,
 * optionally, an @ref Oval_state instance that matches the object query.
 */
struct oval_test;
/**
 * @struct oval_test_iterator
 * @see oval_definition_model_get_tests
 */
struct oval_test_iterator;
/**
 * @struct oval_object
 * An Oval_object instance describes a set of items to look for on an instrumented host platform.
 * A simple object will usually result in a single file, process, etc being identified.
 * But through the use pattern matches, sets, and variables, multiple matching objects can be identified.
 * Therefore an OVAL Object can identify a set of many individual items on a system.
 * This set of items defined by the OVAL Object can then be used by an OVAL Test and compared against an OVAL State.
 */
struct oval_object;
/**
 * @struct oval_object_iterator
 * @see oval_definition_model_get_objects
 */
struct oval_object_iterator;
/**
 * @struct oval_state
 * An Oval_state instance is a collection of one or more characteristics pertaining to a specific object type.
 * The Oval_state is used by an during the evaluation of an Oval_test to determine if a set of items queried
 * by an Oval_object on a instrumented host meet certain characteristics.
 */
struct oval_state;
/**
 * @struct oval_state_iterator
 * @see oval_definition_model_get_states 
 */
struct oval_state_iterator;
/**
 * @struct oval_variable
 * An oval variable resolves an @ref oval_value stream which specifies constraints for @ref oval_object\b s and @ref oval_state\b s.
 *	- if @ref oval_variable_type == @ref OVAL_VARIABLE_EXTERNAL
 *		- The value stream is determined by @ref oval_definition_model_bind_variable_model
 *	- if @ref oval_variable_type == @ref OVAL_VARIABLE_CONSTANT
 *		- The value stream is determined by a set of specified constants (see @ref oval_variable_add_value)
 *	- if @ref oval_variable_type == @ref OVAL_VARIABLE_LOCAL
 *		- The value stream is specified by either a single component or a complex function, meaning that a value can be as simple as a literal string or as
 *		complex as multiple registry keys concatenated together.
 *		Note that if an individual component is used and it returns multiple values, then there will be multiple values associated with the Oval_local.
 *		For example, if an object is specified as the local source and it references a file object that identifies a set of 5 files,
 *		then the local variable would represent these 5 values.
 */
struct oval_variable;
/**
 * @struct oval_variable_iterator
 * @see oval_definition_model_get_variables
 */
struct oval_variable_iterator;
/**
 * @struct oval_affected
 * Each OVAL Definition specifies certain type of affected system(s). The family, platform(s),
 * and product(s) of this target are described by the Oval_affected component whose main purpose is to provide hints
 * for tools using OVAL Definitions. For instance, to help a reporting tool only use Windows definitions,
 * or to preselect only Red Hat definitions to be evaluated. Note, the inclusion of a particular platform
 * or product does not mean the definition is physically checking for the existence of the platform or product.
 * For the actual test to be performed, the correct test must still be included in the definition's criteria section.
 * 
 * The AffectedType complex type details the specific system, application, subsystem, library, etc. for which a
 * definition has been written. If a definition is not tied to a specific product, then this element should not
 * be included. The absence of the platform or product element can be thought of as definition applying to
 * all platforms or products. The inclusion of a particular platform or product does not mean the definition
 * is physically checking for the existence of the platform or product. For the actual test to be performed,
 * the correct test must still be included in the definition's criteria section. To increase the utility of this element,
 * care should be taken when assigning and using strings for product names. The schema places no restrictions on the values
 * that can be assigned, potentially leading to many different representations of the same value. For example
 * 'Internet Explorer' and 'IE'. The current convention is to fully spell out all terms, and avoid the use of
 * abbreviations at all costs.
 */
struct oval_affected;
/**
 * @struct oval_affected_iterator
 * @see oval_definition_get_affected
 */
struct oval_affected_iterator;
/**
 * @struct oval_reference
 * An Oval_reference links an @ref Oval_definition to a definitive external reference.
 * For example, CVE Identifiers for vulnerabilities.
 * The intended purpose for this reference is to link the definition to a variety of other sources that address the same
 * issue being specified by the Oval_definition.
 */
struct oval_reference;
/**
 * @struct oval_reference_iterator
 * @see oval_definition_get_references
 */
struct oval_reference_iterator;
/**
 * @struct oval_criteria_node
 * Specify @ref oval_definition dependencies.
 * An Oval_criteria_node specifies @ref oval_definition dependencies as follows:
 *	- if @ref oval_criteria_node_get_type == @ref OVAL_NODETYPE_CRITERIA - a hierarchic dependency on other Oval_criteria_nodes
 *	- if @ref oval_criteria_node_get_type == @ref OVAL_NODETYPE_CRITERION - an @ref oval_test dependency
 *	- if @ref oval_criteria_node_get_type == @ref OVAL_NODETYPE_EXTENDDEF - an @ref oval_definition dependency
 */
struct oval_criteria_node;
/**
 * @struct oval_criteria_node_iterator
 * @see oval_criteria_node_get_subnodes
 */
struct oval_criteria_node_iterator;
/**
 * @struct oval_object_content
 * An Oval_object_content instance specifies an @ref oval_object field.
 */
struct oval_object_content;
/**
 * @struct oval_object_content_iterator
 * @see oval_object_get_object_contents
 */
struct oval_object_content_iterator;
/**
 * @struct oval_behavior
 * OVAL behavior
 */
struct oval_behavior;
/**
 * @struct oval_behavior_iterator
 * @see oval_object_get_behaviors
 */
struct oval_behavior_iterator;
/**
 * @struct oval_state_content
 * An Oval_state_content instance specifies an @ref oval_state field.
 */
struct oval_state_content;
/**
 * @struct oval_state_content_iterator
 * @see oval_state_get_contents
 */
struct oval_state_content_iterator;
/**
 * @struct oval_value
 * OVAL object or item entity value
 */
struct oval_value;
/**
 * @struct oval_value_iterator
 * @see oval_variable_get_values
 */
struct oval_value_iterator;
/**
 * @struct oval_entity
 * OVAL entity
 */
struct oval_entity;
/**
 * @struct oval_entity_iterator
 */
struct oval_entity_iterator;
/**
 * @struct oval_setobject
 * OVAL set object
 */
struct oval_setobject;
/**
 * @struct oval_setobject_iteraor
 * @see oval_setobject_get_subsets
 */
struct oval_setobject_iterator;
/**
 * @struct oval_component
 * OVAL variable component
 * Oval_component instances specify evaluation constraints on local variables (see @ref oval_variable_get_type).
 *
 *	- If @ref oval_component_get_type == @ref OVAL_FUNCTION_CONCAT - Concatenation function
 *		- Oval_function_CONCAT takes two or more components and sub-concatenates them together to form a single string.
 *		The first component makes up the begining of the resulting string and any following components are added to the end it.
 *		If one of the components returns multiple values then the concat function would be performed multiple times and the end result would be an array of values for the local variable.
 *		For example assume a local variable has two sub-components: a basic component element returns the values "abc" and "def", and a literal component element that has a value of "xyz".
 *		The local_variable element would be evaluated to have two values, "abcxyz" and "defxyz". If one of the components does not exist,
 *		then the result of the concat operation should be does not exist.
 *	- If @ref oval_component_get_type == @ref OVAL_FUNCTION_ARITHMETIC - Arithmetic function.
 *		- The arithmetic function takes two or more integer or float components and performs a basic mathmetical function on them.
 *		The result of this function in a single integer or float unless one of the components returns multiple values.
 *		In this case the specified arithmetic function would be performed multiple times and the end result would be an array of values for the local variable.
 *		For example assume a local_variable specifies the arithmetic function with an arithmetic_operation of "add" and has two sub-components under this function:
 *		the first component returns multiple values "1" and "2", and the second component returns multiple values "3" and "4" and "5".
 *		The local_variable element would be evaluated to have six values: 1+3, 1+4, 1+5, 2+3, 2+4, and 2+5.
 *	- If @ref oval_component_get_type == @ref OVAL_FUNCTION_REGEX_CAPTURE - Regex_capture function.
 *		- The regex_capture function captures a single substring from a string component.
 *		  The 'pattern' attribute provides a regular expression that must contain a single subexpression (using parentheses).
 *		  The first match of the subexpression is considered the captured substring.
 *		  For example, the pattern ^abc(.*)xyz$ would capture a substring from each of the string component's values if the value starts with abc and ends with xyz.
 *		  In this case the subexpression would be all the characters that exist in between the abc and the xyz.
 *		  If more than one subexpression is supplied only the first match is considered.
 *		  If more than one match is identified by a single subexpression only the first match is considered.
 *		  If no matches are found or a subexpression is not supplied the function will evaluate to an empty string.
 *		  Note that subexpressions match the longest possible substrings.
 *	- If @ref oval_component_get_type == @ref OVAL_FUNCTION_BEGIN - Begin function
 *		- The begin function takes a single string component and defines a character (or string) that the component string should start with.
 *		  The character attribute defines the specific character (or string).
 *		  The character (or string) is only added to the component string if the component string doesn't already start with the specified character (or string).
 *	- If @ref oval_component_get_type == @ref OVAL_FUNCTION_END - End function
 *		- The end function takes a single string component and defines a character (or string) that the component string should end with.
 *		  The character attribute defines the specific character (or string).
 *		  The character (or string) is only added to the component string if the component string doesn't already end with the specified character (or string).
 *		  If the desired end character is a string, then the entire end string must exist at the end if the component string.
 *		  If the entire end string is not present then the entire end string is appended to the component string.
 *	- If @ref oval_component_get_type == @ref OVAL_FUNCTION_SPLIT - Split function
 *		- The split function takes a single string component and turns it into multiple values based on a delimiter string.
 *		  For example, assume that a basic component element returns the value "a-b-c-d" to the split function with the delimiter set to "-".
 *		  The local_variable element would be evaluated to have four values "a", "b", "c", and "d".
 *		  If the basic component returns a value that begins, or ends, with a delimiter, the local_variable element would contain empty string
 *		  values at the beginning, or end, of the set of values returned for that string component.
 *		  For example, if the delimiter is "-", and the basic component element returns the value "-a-a-",
 *		  the local_variable element would be evaluated to have four values "", "a", "a", and "".
 *		  Likewise, if the basic component element returns a value that contains adjacent delimiters such as "---",
 *		  the local_variable element would be evaluated to have four values "", "", "", and "".
 *		  Lastly, if the basic component element used by the split function returns multiple values,
 *		  then the split function is performed multiple times, and all of the results, from each of the split functions, are returned.
 *	- If @ref oval_component_get_type == @ref OVAL_FUNCTION_SUBSTRING - Substring function
 *		- The substring function takes a single string component and produces a single value that contains a portion
 *		  of the original string. The substring_start attribute defines the starting position in the original string.
 *		  Note, to include the first character of the string, the start position would be 1.
 *		  Also note that a value less than one also means starting at the first character of the string.
 *		  The substring_length attribute defines how many character after and including the starting character to include.
 *		  Note that a substring_length value greater than the actual length of the string or a negative value means to
 *		  include all the characters after the starting character.
 *		  For example assume a basic component element that returns the value "abcdefg" with a substring_start value of
 *		  3 and a substring_length value of 2. The local_variable element would be evaluate to have a single value of
 *		  "cd". If the string component used by the substring function returns multiple values, then the substring
 *		  operation is performed multiple times and results in multiple values for the component.
 *	- If @ref oval_component_get_type == @ref OVAL_FUNCTION_TIMEDIF - Time difference function
 *		- Oval_function_TIMEDIF calculates the difference in seconds between date-time values.
 *		  If one component is specified, the values of that component are subtracted from the current time (UTC).
 *		  If two components are specified, the value of the second component is subtracted from the value of the first component.
 *		  If the component(s) contain multiple values,
 *		  the operation is performed multiple times on the Cartesian product of the component(s) and the result is an array of time difference values.
 *		  For example, assume a local_variable specifies the time_difference function and has two sub-components under this function: the first
 *		  component returns multiple values "04/02/2009" and "04/03/2009", and the second component returns multiple values "02/02/2005"
 *		  and "02/03/2005" and "02/04/2005".
 *		  The local_variable element would be evaluated to have six values: (ToSeconds("04/02/2009") - ToSeconds("02/02/2005")),
 *		  (ToSeconds("04/02/2009") - ToSeconds("02/03/2005")), (ToSeconds("04/02/2009") - ToSeconds("02/04/2005")),
 *		  (ToSeconds("04/03/2009") - ToSeconds("02/02/2005")), (ToSeconds("04/03/2009") - ToSeconds("02/03/2005")),
 *		  and (ToSeconds("04/03/2009") - ToSeconds("02/04/2005"))..
 *	- If @ref oval_component_get_type == @ref OVAL_FUNCTION_ESCAPE_REGEX - Escape Regular Expression function
 *		- Oval_function_ESCAPE_REGEX takes a single string component and escapes all the regular expression characters.
 *		The purpose for this is that many times, a component used in pattern match needs to be treated as a literal string and not a regular expression.
 *		For example, assume a basic component element that identifies a file path that is held in the Windows registry.
 *		This path is a string that might contain regular expression characters.
 *		These characters are likely not intended to be treated as regular expression characters and need to be escaped.
 *		This function allows a definition writer to mark convert the values of components to regular expression format.
 *		Note that when using regular expressions, OVAL supports a common subset of the regular expression character
 *		classes, operations, expressions and other lexical tokens defined within Perl 5's regular expression specification.
 *		For more information on the supported regular expression syntax in OVAL see: http://oval.mitre.org/language/about/re_support_5.6.html.
 *	- If @ref oval_component_get_type == @ref OVAL_COMPONENT_LITERAL - Literal component
 *		- Oval_literal evaluates to a literal value.
 *	- If @ref oval_component_get_type == @ref OVAL_COMPONENT_OBJECTREF - Object component
 *		- Oval_component_object binds an Oval_object.
 *		  During evaluation of the component, the object is queried and the resultant item stream is transformed to provide the Oval_value stream that
 *		  constitutes the evaluation result of this component.  The item stream transformation consists of using the component object_field attribute to identify the name of a specific item field
 *		  whose value is appended to the component value stream.
 *	- If @ref oval_component_get_type == @ref OVAL_COMPONENT_VARREF - Variable component
 *		- Oval_variable_component binds an Oval_variable.  The component evaluation is the result of the evaluation of the bound
 *		  Oval_variable.
 */
struct oval_component;
/**
 * @struct oval_component_iterator
 * @see oval_component_get_function_components
 */
struct oval_component_iterator;
/**
 * @struct oval_string_iterator
 * @see oval_definition_get_notes
 * @see oval_test_get_notes
 * @see oval_object_get_notes
 * @see oval_state_get_notes
 * @see oval_affected_get_platforms
 * @see oval_affected_get_products
 */
struct oval_string_iterator;






/**
 * Import the content from a specified XML stream into a oval_definition_model, return -1 if an error occurred.
 * The imported XML stream must be
 * If the input_source specifies a model entity (id=A, version=V) and the model specifies no entity (id=A, ...) then (id=A, version=V) is merged into the model.
 * If the input_source specifies a model entity (id=A, version=V) and the model specifies an entity (id=A, ...) then (id=A, version=V) then the source specification is skipped.
 * @param model the merge target model.
 * @param source an <oval_def:oval_definitions> XML stream
 * @param error_handler callback for detected error conditions (may be NULL)
 * @param client pointer to client data (may be NULL)
 * @memberof oval_definition_model
 */
int oval_definition_model_import(struct oval_definition_model *model, struct oscap_import_source *source, void *client);
/**
 * Create an empty oval_definition_model.
 * @memberof oval_definition_model
 */
struct oval_definition_model *oval_definition_model_new(void);
/**
 * Copy an oval_definition_model.
 * @memberof oval_definition_model
 */
struct oval_definition_model *oval_definition_model_clone(struct oval_definition_model *);
/**
 * @memberof oval_definition_model 
 */
int oval_definition_model_export(struct oval_definition_model *, struct oscap_export_target *);
/**
 * Free OVAL object model.
 * @memberof oval_definition_model
 */
void oval_definition_model_free(struct oval_definition_model *model);

/**
 * @name Setters
 * @{
 */
/**
 * Bind an oval_variable_model to the specified oval_definition_model.
 * @memberof oval_definition_model
 */
void oval_definition_model_bind_variable_model(struct oval_definition_model *, struct oval_variable_model *);
/**
 * @memberof oval_definition_model
 */
void oval_definition_model_add_definition(struct oval_definition_model *, struct oval_definition *);
/**
 * @memberof oval_definition_model
 */
void oval_definition_model_add_test(struct oval_definition_model *, struct oval_test *);
/**
 * @memberof oval_definition_model
 */
void oval_definition_model_add_object(struct oval_definition_model *, struct oval_object *);
/**
 * @memberof oval_definition_model
 */
void oval_definition_model_add_state(struct oval_definition_model *, struct oval_state *);
/**
 * @memberof oval_definition_model
 */
void oval_definition_model_add_variable(struct oval_definition_model *, struct oval_variable *);
/**
 * Lock the definition_model instance.
 * The state of a locked instance cannot be changed.
 * This operation has no effect if the model is already locked.
 * @memberof oval_definition_model
 */
void oval_definition_model_lock(struct oval_definition_model *definition_model);
/** @} */


/**
 * @name Getters
 * @{
 */
/**
 * Returns the appended @ref oval_definition having the specified id.
 * IF the specified id does not resolve to an appended Oval_definition the method shall return NULL.
 * @see oval_definition_add_definition
 * @param id the definition id.
 * @memberof oval_definition_model
 */
struct oval_definition *oval_definition_model_get_definition(struct oval_definition_model *, char *id);
/**
 * Get oval test by ID.
 * Return a designated oval_test from the specified oval_definition_model.
 * If the specified id does not resolve to an oval_test the function returns NULL.
 * @param model the queried model.
 * @param id the test id.
 * @memberof oval_definition_model
 */
struct oval_test *oval_definition_model_get_test(struct oval_definition_model *model, char *id);
/**
 * Get OVAL object by ID.
 * Return a designated oval_object from the specified oval_definition_model.
 * If the specified id does not resolve to an oval_object the function returns NULL.
 * @param model the queried model.
 * @param id the object id.
 * @memberof oval_definition_model
 */
struct oval_object *oval_definition_model_get_object(struct oval_definition_model *model, char *id);
/**
 * Get OVAL state by ID.
 * Return a designated oval_state from the specified oval_definition_model.
 * If the specified id does not resolve to an oval_state the function returns NULL.
 * @param model the queried model.
 * @param id the state id.
 * @memberof oval_definition_model
 */
struct oval_state *oval_definition_model_get_state(struct oval_definition_model *model, char *id);
/**
 * Get OVAL variable by ID.
 * Return a designated oval_variable from the specified oval_definition_model.
 * If the specified id does not resolve to an oval_variable the function returns NULL.
 * @param model the queried model.
 * @param id the variable id.
 * @memberof oval_definition_model
 */
struct oval_variable *oval_definition_model_get_variable(struct oval_definition_model *model, char *id);
/**
 * Returns all appended @ref oval_definition instances.
 * @memberof oval_definition_model
 */
struct oval_definition_iterator *oval_definition_model_get_definitions(struct oval_definition_model *model);
/**
 * Get OVAL tests.
 * Return all oval_tests from the specified oval_definition_model.
 * @param model the queried model.
 * @memberof oval_definition_model
 */
struct oval_test_iterator *oval_definition_model_get_tests(struct oval_definition_model *model);
/**
 * Get OVAL objects.
 * Return all oval_objects from the specified oval_definition_model.
 * @param model the queried model.
 * @memberof oval_definition_model
 */
struct oval_object_iterator *oval_definition_model_get_objects(struct oval_definition_model *model);
/**
 * Get OVAL states.
 * Return all oval_states from the specified oval_definition_model.
 * @param model the queried model.
 * @memberof oval_definition_model
 */
struct oval_state_iterator *oval_definition_model_get_states(struct oval_definition_model *model);
/**
 * Get OVAL variables.
 * Return all oval_variables from the specified oval_definition_model.
 * @param model the queried model.
 * @memberof oval_definition_model
 */
struct oval_variable_iterator *oval_definition_model_get_variables(struct oval_definition_model *model);
/**
 * Return <b>true</b> if the definition_model instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_definition_model
 */
bool oval_definition_model_is_locked(struct oval_definition_model *definition_model);

/**
 * Get supported version of OVAL XML
 * @return version of XML file format
 * @memberof oval_definition
 */
const char * oval_definition_model_supported(void);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the definition_model instance is valid
 * @memberof oval_definition_model
 */
bool oval_definition_model_is_valid(struct oval_definition_model *definition_model);
/** @} */






/**
 * Construct an instance of @ref oval_definition.
 * Initialized attribute values are
 * 	- id bound to value specified by id parameter.
 *	- version bound to 0.
 *	- class bound to @ref OVAL_CLASS_UNKNOWN
 *	- deprecated bound to <b>false</b>
 *	- title bound to NULL
 *	- description bound to NULL
 *	- criteria bound to NULL
 *	- affected bound to empty iterator
 *	- references bound to empty iterator
 *	- notes bound to empty iterator
 * @param id - (non-NULL) A copy of this string is bound to the id attribute of the created instance.
 * @memberof oval_definition
 */
struct oval_definition *oval_definition_new(struct oval_definition_model *, char *id);
/**
 * Release an instance of @ref oval_definition.
 * All attributes of the Oval_definition are also released.
 *
 * @note Applications should not call this method to free an Oval_definition that is bound
 * to an instance of @ref OVALDEF.  These bound definitions are released when the @ref OVALDEF are
 * released.
 * @memberof oval_definition
 */
void oval_definition_free(struct oval_definition *);

/**
 * @name Setters
 * @{
 */
/**
 * Set attribute @ref oval_definition->version.
 * This method shall overwrite a zero version attribute value with the value of the version parameter.
 * Valid values of the version attribute shall not be overwritten by this method.
 * @param version - the required version
 * @memberof oval_definition
 */
void oval_definition_set_version(struct oval_definition *, int version);
/**
 * Set attribute @ref oval_definition->class.
 * This method shall overwrite a @ref OVAL_CLASS_UNKNOWN class attribute value with the value of the class parameter.
 * Valid values of the class attribute shall not be overwritten by this method.
 * @param class - the required class
 * @memberof oval_definition
 */
void oval_definition_set_class(struct oval_definition *, oval_definition_class_t);
/**
 * Set attribute @ref oval_definition->deprecated.
 * @param deprecated - the required deprecation toggle.
 * @memberof oval_definition
 */
void oval_definition_set_deprecated(struct oval_definition *, bool deprecated);
/**
 * Set attribute @ref oval_definition->title.
 * This method shall overwrite a NULL title attribute value with a copy of the title parameter.
 * Valid values of the title attribute shall not be overwritten by this method.
 * @param title - the required title
 * @memberof oval_definition
 */
void oval_definition_set_title(struct oval_definition *, char *title);
/**
 * Set attribute @ref oval_definition->description.
 * This method shall overwrite a NULL description attribute value with a copy of the description parameter.
 * Valid values of the description attribute shall not be overwritten by this method.
 * @param description - the required description
 * @memberof oval_definition
 */
void oval_definition_set_description(struct oval_definition *, char *description);
/**
 * Append instance of @ref oval_affected to attribute @ref oval_definition->affected.
 * @note Instances of Oval_affected bound to Oval_definition by this method should not be subsequently freed by
 * the application using @ref oval_affected_free. These instances shall be freed by the API when
 * the Oval_definition is freed.
 * @note A given instance of Oval_affected should not be appended by an application to more than one instance of Oval_definition, nor should
 * the given instance of Oval_affected be appended more than once to a single instance of Oval_definition.
 * @param affected - appended instance of Oval_affected.
 * @memberof oval_definition
 */
void oval_definition_add_affected(struct oval_definition *, struct oval_affected *affected);
/**
 * Append instance of @ref oval_reference to attribute @ref oval_definition->references.
 * @note Instances of Oval_reference bound to Oval_definition by this method should not be subsequently freed by
 * the application using @ref oval_reference_free. These instances shall be freed by the API when
 * the Oval_definition is freed.
 * @note A given instance of Oval_reference should not be appended by an application to more than one instance of Oval_definition, nor should
 * the given instance of Oval_reference be appended more than once to a single instance of Oval_definition.
 * @param reference - appended instance of Oval_reference.
 * @memberof oval_definition
 */
void oval_definition_add_reference(struct oval_definition *, struct oval_reference *reference);
/**
 * Append a copy of the note parameter to attribute @ref Oval_definition->notes.
 * @param note - the note text.
 * @memberof oval_definition
 */
void oval_definition_add_note(struct oval_definition *, char *note);
/**
 * Set attribute @ref oval_definition->criteria.
 * This method shall overwrite a NULL criteria attribute value with the criteria parameter only if the criteria parameter is an instance of @ref Oval_criteria
 * (i.e. criteria->type == @ref OVAL_NODETYPE_CRITERIA).
 * Other values of the criteria attribute shall not be overwritten by this method.
 * @note Instances of Oval_criteria bound to Oval_definition by this method should not be subsequently freed by the application using oval_criteria_node_free.
 * These instances shall be freed by the API when the Oval_definition is freed.
 *
 * @note An application should not bind a given instance of Oval_criteria to more than one instance of Oval_definition,
 * nor should a given instance of Oval_criteria be bound to both an Oval_definition and an Oval_criteria (using @ref oval_criteria_node_add_subnode).
 * @param criteria - the required instance of Oval_criteria
 * @memberof oval_definition
 */
void oval_definition_set_criteria(struct oval_definition *, struct oval_criteria_node *criteria);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Returns attribute @ref oval_definition->id (identifier).
 * @memberof oval_definition
 */
char *oval_definition_get_id(struct oval_definition *);
/**
 * Returns attribute @ref oval_definition->version.
 * @memberof oval_definition
 */
int oval_definition_get_version(struct oval_definition *);
/**
 * Returns attribute @ref oval_definition->class.
 * @memberof oval_definition
 */
oval_definition_class_t oval_definition_get_class(struct oval_definition *);
/**
 * Returns attribute @ref oval_definition->deprecated.
 * @memberof oval_definition
 */
bool oval_definition_get_deprecated(struct oval_definition *);
/**
 * Returns attribute @ref oval_definition->title.
 * @memberof oval_definition
 */
char *oval_definition_get_title(struct oval_definition *);
/**
 * Returns attribute @ref oval_definition->description.
 * @memberof oval_definition
 */
char *oval_definition_get_description(struct oval_definition *);
/**
 * Returns attribute @ref oval_definition->affected.
 * The iterator returned by this method should be freed after use by the calling application.
 * @memberof oval_definition
 */
struct oval_affected_iterator *oval_definition_get_affected(struct oval_definition *);
/**
 * Returns attribute @ref oval_definition->references.
 * The iterator returned by this method should be freed after use by the calling application.
 * @memberof oval_definition
 */
struct oval_reference_iterator *oval_definition_get_references(struct oval_definition *);
/**
 * Returns attribute @ref oval_definition->notes.
 * The iterator returned by this method should be freed after use by the calling application.
 * @memberof oval_definition
 */
struct oval_string_iterator *oval_definition_get_notes(struct oval_definition *);
/**
 * Returns attribute @ref oval_definition->criteria.
 * @memberof oval_definition
 */
struct oval_criteria_node *oval_definition_get_criteria(struct oval_definition *);
/**
 * return <b>true</b> if the definition instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_definition
 */
bool oval_definition_is_locked(struct oval_definition *definition);

/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Returns <b>true</b> if the @ref oval_definition is valid.
 * An Oval_definition is valid if all the following are true
 * 	- attribute_id is bound to a valid oval definition identifier.
 * 	- attribute version is bound to a positive integer.
 * 	- attribute class is bound to a valid value of @ref OVAL_CLASS_UNKNOWN
 *	- attribute title is bound to a non-null string.
 *	- attribute description is bound to a non-null string.
 *	- attribute criteria is bound to a valid instance of @ref Oval_criteria.
 * @memberof oval_definition
 */
bool oval_definition_is_valid(struct oval_definition *definition);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * Returns <b>true</b> if the iterator contains more instances of @ref oval_definition.
 * @memberof oval_definition_iterator
 */
bool oval_definition_iterator_has_more(struct oval_definition_iterator *);
/**
 * Returns the next iterated instance of @ref oval_definition.
 * NULL is returned if the iterator is exhausted (@ref oval_definition_iterator_has_more == <b>false</b>)
 * @memberof oval_definition_iterator
 */
struct oval_definition *oval_definition_iterator_next(struct oval_definition_iterator *);
/**
 * Free the iterator.
 * @memberof oval_definition_iterator
 */
void oval_definition_iterator_free(struct oval_definition_iterator *);
/** @} */







/**
 * Construct new instance of @ref oval_test.
 * Attribute values shall be initialized:
 *	- family -- initialized to @ref OVAL_FAMILY_UNKNOWN
 *	- subtype -- initialized to @ref OVAL_SUBTYPE_UNKNOWN
 *	- notes -- initialized to empty iterator.
 *	- comment -- initialized to NULL
 *	- id -- initialized to a copy of the id parameter
 *	- deprecated -- initialized to <b>false</b>
 *	- version -- initialized to zero
 *	- existence -- initialized to @ref OVAL_CHECK_UNKNOWN
 *	- check -- initialized to @ref OVAL_CHECK_UNKNOWN
 *	- object -- initialized to NULL
 *	- state -- initialized to NULL
 *
 * @note This method shall not construct a new instance of Oval_test and shall return NULL if the text of the id parameter is not matched
 * by the regular expression <b>^oval:[\.A-Za-z0-9_\-]+:tst:[1-9][0-9]*$</b>.
 * @param id - (Not NULL) the text of the required test id.
 * @memberof oval_test
 */
struct oval_test *oval_test_new(struct oval_definition_model *, char *id);
/**
 * Destruct instance of @ref oval_test.
 * @memberof oval_test
 */
void oval_test_free(struct oval_test *);

/**
 * @name Setters
 * @{
 */
/**
 * Sets attributes @ref oval_test->subtype and @ref Oval_test->family.
 * If Oval_test->subtype == @ref OVAL_SUBTYPE_UNKNOWN and parameter subtype <> @ref OVAL_SUBTYPE_UNKNOWN,
 * this method shall overwrite Oval_test->subtype with the value of the subtype parameter and Oval_test->family
 * with the value of @ref oval_family_t corresponding to the specified subtype.
 * Otherwise, the state of the Oval_test instance shall not be changed by this method.
 * @param subtype - the required subtype value.
 * @memberof oval_test
 */
void oval_test_set_subtype(struct oval_test *, oval_subtype_t subtype);
/**
 * Appends a copy of the note parameter to attribute @ref oval_test->notes.
 * @param note - (Not NULL) the text of the appended note.
 * @memberof oval_test
 */
void oval_test_add_note(struct oval_test *, char *note);
/**
 * Sets a copy of the comment parameter to attribute @ref oval_test->comment.
 * @param comment - (Not NULL) the text of the comment.
 * @memberof oval_test
 */
void oval_test_set_comment(struct oval_test *, char *comment);
/**
 * Sets attribute @ref oval_test->deprecated.
 * @memberof oval_test
 */
void oval_test_set_deprecated(struct oval_test *, bool deprecated);
/**
 * Sets attribute @ref oval_test->version.
 * If Oval_test->version == 0 and parameter version >0,
 * this method shall overwrite Oval_test->version with the parameter value.
 * Otherwise, the method shall leave the Oval_test state unchanged.
 * @param version - (>0) the required version
 * @memberof oval_test
 */
void oval_test_set_version(struct oval_test *, int version);
/**
 * Sets attribute @ref oval_test->existence.
 * If Oval_test->existence == @ref OVAL_CHECK_UNKNOWN and parameter existence <> @ref OVAL_CHECK_UNKNOWN,
 * this method shall overwrite Oval_test->existence with the parameter value.
 * Otherwise, the method shall leave the Oval_test state unchanged.
 * @param existence - (<> @ref OVAL_CHECK_UNKNOWN) the required existence
 * @memberof oval_test
 */
void oval_test_set_existence(struct oval_test *, oval_existence_t);
/**
 * Sets attribute @ref oval_test->check.
 * If Oval_test->check == @ref OVAL_CHECK_UNKNOWN and parameter check <> @ref OVAL_CHECK_UNKNOWN,
 * this method shall overwrite Oval_test->check with the parameter check.
 * Otherwise, the method shall leave the Oval_test state unchanged.
 * @note If Oval_test->state == NULL, the value of Oval_test->check is ignored.
 * @param check - (<> @ref OVAL_CHECK_UNKNOWN) the required check
 * @memberof oval_test
 */
void oval_test_set_check(struct oval_test *, oval_check_t);
/**
 * Sets attribute @ref oval_test->object.
 * If Oval_test->object == NULL and parameter object <> NULL,
 * this method shall overwrite Oval_test->object with the parameter object.
 * Otherwise, the method shall leave the Oval_test state unchanged.
 * @param object - (<> NULL) the required object
 * @memberof oval_test
 */
void oval_test_set_object(struct oval_test *, struct oval_object *);
/**
 * Sets attribute @ref oval_test->state.
 * If Oval_test->state == NULL and parameter state <> NULL,
 * this method shall overwrite Oval_test->state with the parameter state.
 * Otherwise, the method shall leave the Oval_test state unchanged.
 * @param state - (<> NULL) the required state
 * @memberof oval_test
 */
void oval_test_set_state(struct oval_test *, struct oval_state *state);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Returns attribute @ref Oval_test->family
 * @memberof oval_test
 */
oval_family_t oval_test_get_family(struct oval_test *);
/**
 * Returns attribute @ref Oval_test->subtype
 * @memberof oval_test
 */
oval_subtype_t oval_test_get_subtype(struct oval_test *);
/**
 * Returns attribute @ref oval_test->notes.
 * @note The iterator returned by this method should be freed by the calling application.
 * @memberof oval_test
 */
struct oval_string_iterator *oval_test_get_notes(struct oval_test *);
/**
 * Returns attribute @ref Oval_test->comment.
 * @memberof oval_test
 */
char *oval_test_get_comment(struct oval_test *);
/**
 * Returns attribute @ref oval_test->id.
 * @memberof oval_test
 */
char *oval_test_get_id(struct oval_test *);
/**
 * Returns attribute @ref oval_test->deprecated.
 * @memberof oval_test
 */
bool oval_test_get_deprecated(struct oval_test *);
/**
 * Returns attribute @ref oval_test->version.
 * @memberof oval_test
 */
int oval_test_get_version(struct oval_test *);
/**
 * Returns attribute @ref oval_test->existence.
 * @memberof oval_test
 */
oval_existence_t oval_test_get_existence(struct oval_test *);
/**
 * Returns attribute @ref oval_test->check.
 * @memberof oval_test
 */
oval_check_t oval_test_get_check(struct oval_test *);
/**
 * Returns attribute @ref oval_test->object.
 * @memberof oval_test
 */
struct oval_object *oval_test_get_object(struct oval_test *);
/**
 * Returns attribute @ref oval_test->state.
 * @memberof oval_test
 */
struct oval_state *oval_test_get_state(struct oval_test *);
/**
 * return <b>true</b> if the test instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_test
 */
bool oval_test_is_locked(struct oval_test *test);

/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Returns <b>true</b> if @ref oval_test is valid.
 * An Oval_test is valid if all of the following are true:
 * 	- The value of the version attribute is not zero.
 * 	- The object attribute is not NULL
 * 	- The object attribute is valid
 * 	- The subtype of the object attribute is the same as the subtype of the test.
 * 	- The value of the existence attribute is not @ref OVAL_CHECK_UNKNOWN
 * 	- If the state attribute is not NULL
 *		- the subtype of the state attribute is the same as the subtype of the test
 *		- the state attribute is valid
 * 		- the value of the operator attribute is not @ref OVAL_CHECK_UNKNOWN
 * 		- the value of the check attribute is not @ref OVAL_CHECK_UNKNOWN
 * @memberof oval_test
 */
bool oval_test_is_valid(struct oval_test *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * Returns <b>true</b> if the iterator is not exhausted.
 * @memberof oval_test_iterator
 */
bool oval_test_iterator_has_more(struct oval_test_iterator *);
/**
 * Returns the next instance of @ref oval_test.
 * @memberof oval_test_iterator
 */
struct oval_test *oval_test_iterator_next(struct oval_test_iterator *);
/**
 * Frees the iterator.
 * @memberof oval_test_iterator
 */
void oval_test_iterator_free(struct oval_test_iterator *);
/** @} */






/**
 * Construct new intance of @ref oval_object.
 * Attribute values shall be initialized:
 *	- family -- initialized to @ref OVAL_FAMILY_UNKNOWN
 *	- subtype -- initialized to @ref OVAL_SUBTYPE_UNKNOWN
 *	- notes -- initialized to empty iterator
 *	- comment -- initialized to NULL
 *	- id -- initialized to a copy of the id parameter
 *	- deprecated -- initialized to <b>false</b>
 *	- version -- initialized to zero
 *	- contents -- initialized to empty iterator
 *	- behaviors -- initialized to empty iterator
 *
 * @note This method shall not construct a new instance of Oval_object and shall return NULL if the text of the id parameter is not matched
 * by the regular expression <b>^oval:[\.A-Za-z0-9_\-]+:obj:[1-9][0-9]*$</b>.
 * @param id - (Not NULL) the text of the required object id.
 * @memberof oval_object
 */
struct oval_object *oval_object_new(struct oval_definition_model *, char *id);
/**
 * Free instance of @ref oval_object
 * @memberof oval_object
 */
void oval_object_free(struct oval_object *);

/**
 * @name Setters
 * @{
 */
/**
 * Sets attributes @ref oval_object->subtype and @ref oval_object->family.
 * If Oval_object->subtype == @ref OVAL_SUBTYPE_UNKNOWN and parameter subtype <> @ref OVAL_SUBTYPE_UNKNOWN,
 * this method shall overwrite Oval_object->subtype with the value of the subtype parameter and Oval_object->family
 * with the value of @ref oval_family_t corresponding to the specified subtype.
 * Otherwise, the state of the Oval_object instance shall not be changed by this method.
 * @param subtype - the required subtype value.
 * @memberof oval_object
 */
void oval_object_set_subtype(struct oval_object *, oval_subtype_t subtype);
/**
 * Appends a copy of the note parameter to attribute @ref oval_object->notes.
 * @param note - (Not NULL) the text of the appended note.
 * @memberof oval_object
 */
void oval_object_add_note(struct oval_object *, char *note);

/**
 * Sets a copy of the comment parameter to attribute @ref oval_object->comment.
 * @param comment - (Not NULL) the text of the comment.
 * @memberof oval_object
 */
void oval_object_set_comment(struct oval_object *, char *comment);

/**
 * Sets attribute @ref oval_object->deprecated.
 * @memberof oval_object
 */
void oval_object_set_deprecated(struct oval_object *, bool deprecated);
/**
 * Sets attribute @ref oval_object->version.
 * If Oval_object->version == 0 and parameter version >0,
 * this method shall overwrite Oval_object->version with the parameter value.
 * Otherwise, the method shall leave the Oval_object state unchanged.
 * @param version - (>0) the required version
 * @memberof oval_object
 */
void oval_object_set_version(struct oval_object *, int version);
/**
 * Append instance of @ref oval_object_content to attribute @ref oval_object->object_contents.
 *
 * Oval_object_contents appended to a given Oval_object shall be freed by the API when the Oval_object is freed.
 *
 * @note A specific instance of Oval_object_content should not be appended to more than one Oval_object,
 * and an instance of Oval_object_content should not be appended more than once to a single Oval_object.
 * @note An instance of Oval_object_content that is appended to an Oval_object should not be freed
 * independently by the application using @ref oval_object_content_free.
 * @param content - (Not NULL) the Oval_object_content to be appended.
 * @memberof oval_object
 */
void oval_object_add_object_content(struct oval_object *, struct oval_object_content *content);
/**
 * Append instance of @ref oval_behavior to attribute @ref oval_object->behaviors.
 *
 * Oval_behaviors appended to a given Oval_object shall be freed by the API when the Oval_object is freed.
 *
 * @note A specific instance of Oval_behavior should not be appended to more than one Oval_object,
 * and an instance of Oval_behavior should not be appended more than once to a single Oval_object.
 * @note An instance of Oval_behavior that is appended to an Oval_object should not be freed
 * independently by the application using @ref oval_behavior_free.
 * @param behavior - (Not NULL) the Oval_behavior to be appended.
 * @memberof oval_object
 */
void oval_object_add_behavior(struct oval_object *, struct oval_behavior *behavior);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Returns attribute @ref oval_object->family
 * @memberof oval_object
 */
oval_family_t oval_object_get_family(struct oval_object *);
/**
 * Returns the name of an @ref oval_object.
 * This is a convenience method that is equivalent to @ref oval_subtype_get_text (@ref oval_object_get_subtype)+"_object".
 * @memberof oval_object
 */
const char *oval_object_get_name(struct oval_object *);
/**
 * Returns attribute @ref oval_object->subtype
 * @memberof oval_object
 */
oval_subtype_t oval_object_get_subtype(struct oval_object *);
/**
 * Returns attribute @ref oval_object->notes.
 * @note The iterator returned by this method should be freed by the calling application.
 * @memberof oval_object
 */
struct oval_string_iterator *oval_object_get_notes(struct oval_object *);
/**
 * Returns attribute @ref oval_object->comment.
 * @memberof oval_object
 */
char *oval_object_get_comment(struct oval_object *);
/**
 * Returns attribute @ref oval_object->id.
 * @memberof oval_object
 */
char *oval_object_get_id(struct oval_object *);

/**
 * Returns attribute @ref oval_object->deprecated.
 * @memberof oval_object
 */
bool oval_object_get_deprecated(struct oval_object *);
/**
 * Returns attribute @ref oval_object->version.
 * @memberof oval_object
 */
int oval_object_get_version(struct oval_object *);
/**
 * Returns attribute @ref oval_object->contents.
 * @memberof oval_object
 */
struct oval_object_content_iterator *oval_object_get_object_contents(struct oval_object *);
/**
 * Returns attribute @ref oval_object->behaviors.
 * @memberof oval_object
 */
struct oval_behavior_iterator *oval_object_get_behaviors(struct oval_object *);
/**
 * return <b>true</b> if the object instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_object
 */
bool oval_object_is_locked(struct oval_object *object);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the object instance is valid
 * @memberof oval_object
 */
bool oval_object_is_valid(struct oval_object *object);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * Returns <b>true</b> if the iterator is not exhausted.
 * @memberof oval_object_iterator
 */
bool oval_object_iterator_has_more(struct oval_object_iterator *);
/**
 * Returns the next instance of @ref oval_object.
 * @memberof oval_object_iterator
 */
struct oval_object *oval_object_iterator_next(struct oval_object_iterator *);
/**
 * Frees the iterator.
 * @memberof oval_object_iterator
 */
void oval_object_iterator_free(struct oval_object_iterator *);
/** @} */






/**
 * Construct new intance of @ref oval_state.
 * Attribute values shall be initialized:
 *	- family -- initialized to @ref OVAL_FAMILY_UNKNOWN
 *	- subtype -- initialized to @ref OVAL_SUBTYPE_UNKNOWN
 *	- notes -- initialized to empty iterator
 *	- comment -- initialized to NULL
 *	- id -- initialized to a copy of the id parameter
 *	- deprecated -- initialized to <b>false</b>
 *	- version -- initialized to zero
 *	- operator -- initialized to @ref OVAL_OPERATOR_AND
 *	- contents -- initialized to empty iterator
 *
 * @note This method shall not construct a new instance of Oval_state and shall return NULL if the text of the id parameter is not matched
 * by the regular expression <b>^oval:[\.A-Za-z0-9_\-]+:stt:[1-9][0-9]*$</b>.
 * @param id - (Not NULL) the text of the required state id.
 * @memberof oval_state
 */
struct oval_state *oval_state_new(struct oval_definition_model *, char *id);

/**
 * Free instance of @ref oval_state
 * @memberof oval_state
 */
void oval_state_free(struct oval_state *);

/**
 * @name Setters
 * @{
 */
/**
 * Sets attributes @ref oval_state->subtype and @ref oval_state->family.
 * If oval_state->subtype == @ref OVAL_SUBTYPE_UNKNOWN and parameter subtype <> @ref OVAL_SUBTYPE_UNKNOWN,
 * this method shall overwrite oval_state->subtype with the value of the subtype parameter and oval_state->family
 * with the value of @ref oval_family_t corresponding to the specified subtype.
 * Otherwise, the state of the oval_state instance shall not be changed by this method.
 * @param subtype - the required subtype value.
 * @memberof oval_state
 */
void oval_state_set_subtype(struct oval_state *, oval_subtype_t subtype);
/**
 * Appends a copy of the note parameter to attribute @ref oval_state->notes.
 * @param note - (Not NULL) the text of the appended note.
 * @memberof oval_state
 */
void oval_state_add_note(struct oval_state *, char *note);
/**
 * Sets a copy of the comment parameter to attribute @ref oval_state->comment.
 * @param comment - (Not NULL) the text of the comment.
 * @memberof oval_state
 */
void oval_state_set_comment(struct oval_state *, char *comment);
/**
 * Sets attribute @ref oval_state->deprecated.
 * @memberof oval_state
 */
void oval_state_set_deprecated(struct oval_state *, bool deprecated);
/**
 * Sets attribute @ref oval_state->version.
 * If oval_state->version == 0 and parameter version >0,
 * this method shall overwrite oval_state->version with the parameter value.
 * Otherwise, the method shall leave the oval_state state unchanged.
 * @param version - (>0) the required version
 * @memberof oval_state
 */
void oval_state_set_version(struct oval_state *, int version);
/**
 * Sets attribute @ref oval_state->operator.
 * @memberof oval_state
 */
void oval_state_set_operator(struct oval_state *, oval_operator_t);
/**
 * Append instance of @ref oval_state_content to attribute @ref oval_state->state_contents.
 *
 * oval_state_contents appended to a given oval_state shall be freed by the API when the oval_state is freed.
 *
 * @note A specific instance of oval_state_content should not be appended to more than one oval_state,
 * and an instance of oval_state_content should not be appended more than once to a single oval_state.
 * @note An instance of oval_state_content that is appended to an oval_state should not be freed
 * independently by the application using @ref oval_state_content_free.
 * @param content - (Not NULL) the oval_state_content to be appended.
 * @memberof oval_state
 */
void oval_state_add_content(struct oval_state *, struct oval_state_content *content);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Returns attribute @ref oval_state->family
 * @memberof oval_state
 */
oval_family_t oval_state_get_family(struct oval_state *);
/**
 * Returns the name of an @ref oval_state.
 * This is a convenience method that is equivalent to @ref oval_subtype_get_text (@ref oval_state_get_subtype)+"_state".
 * @memberof oval_state
 */
const char *oval_state_get_name(struct oval_state *);
/**
 * Returns attribute @ref oval_state->subtype
 * @memberof oval_state
 */
oval_subtype_t oval_state_get_subtype(struct oval_state *);
/**
 * Returns attribute @ref oval_state->notes.
 * @note The iterator returned by this method should be freed by the calling application.
 * @memberof oval_state
 */
struct oval_string_iterator *oval_state_get_notes(struct oval_state *);
/**
 * Returns attribute @ref oval_state->comment.
 * @memberof oval_state
 */
char *oval_state_get_comment(struct oval_state *);
/**
 * Returns attribute @ref oval_state->id.
 * @memberof oval_state
 */
char *oval_state_get_id(struct oval_state *);
/**
 * Returns attribute @ref oval_state->deprecated.
 * @memberof oval_state
 */
bool oval_state_get_deprecated(struct oval_state *);
/**
 * Returns attribute @ref oval_state->version.
 * @memberof oval_state
 */
int oval_state_get_version(struct oval_state *);
/**
 * Returns attribute @ref oval_state->operator.
 * @memberof oval_state
 */
int oval_state_get_operator(struct oval_state *);
/**
 * Returns attribute @ref oval_state->contents.
 * @memberof oval_state
 */
struct oval_state_content_iterator *oval_state_get_contents(struct oval_state *);
/**
 * return <b>true</b> if the state instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_state
 */
bool oval_state_is_locked(struct oval_state *state);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * Returns <b>true</b> if the iterator is not exhausted.
 * @memberof oval_state_iterator
 */
bool oval_state_iterator_has_more(struct oval_state_iterator *);
/**
 * Returns the next instance of @ref oval_state.
 * @memberof oval_state_iterator
 */
struct oval_state *oval_state_iterator_next(struct oval_state_iterator *);
/**
 * Frees the iterator.
 * @memberof oval_state_iterator
 */
void oval_state_iterator_free(struct oval_state_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the state instance is valid
 * @memberof oval_state
 */
bool oval_state_is_valid(struct oval_state *state);
/** @} */






/**
 * Construct new instance of @ref oval_variable.
 * Attribute values shall be initialized:
 *	- id -- initialized to a copy of the id parameter
 *	- type -- initialized to the type parameter
 *	- version -- initialized to zero
 *	- deprecated -- initialized to <b>false</b>
 *	- comment -- initialized to NULL
 *	- datatype -- initialzed to @ref OVAL_DATATYPE_UNKNOWN
 *	- If type == @ref OVAL_VARIABLE_CONSTANT
 *		- values -- initialized to empty iterator
 *	- If type == @ref OVAL_VARIABLE_EXTERNAL
 *		- values -- initialized to NULL
 *	- If type == @ref OVAL_VARIABLE_LOCAL
 *		- component -- initialized to NULL
 *
 * This method shall not construct a new instance of oval_variable and shall return NULL
 * 	- if the text of the id parameter is not matched by the regular expression <b>^oval:[\.A-Za-z0-9_\-]+:var:[1-9][0-9]*$</b>.
 *	- if the value of the type parameter is @ref OVAL_VARIABLE_UNKNOWN.
 *
 * @param id - (Not NULL) the text of the required variable id.
 * @param type - (Not @ref OVAL_VARIABLE_UNKNOWN) the required type.
 * @memberof oval_variable
 */
struct oval_variable *oval_variable_new(struct oval_definition_model *model, char *id, oval_variable_type_t type);
/**
 * Free instance of @ref oval_variable.
 * @memberof oval_variable
 */
void oval_variable_free(struct oval_variable *);

/**
 * @name Setters
 * @{
 */
/**
 * set attribute @ref oval_variable->comment.
 * If attribute oval_variable->comment == NULL this method shall overwrite the attribute with a copy of the comment parameter.
 * Otherwise the variable state shall be unchanged.
 * @param comm - (Not NULL) a copy of the comment parameter is set as  the comment attribute.
 * @memberof oval_variable
 */
void oval_variable_set_comment(struct oval_variable *, char *comment);
/**
 * set attribute @ref oval_variable->version.
 * If attribute oval_variable->version == 0 this method shall overwrite the attribute with the version parameter.
 * Otherwise the variable state shall be unchanged.
 * @param version - (>0) the required version.
 * @memberof oval_variable
 */
void oval_variable_set_version(struct oval_variable *, int version);
/**
 * set attribute @ref oval_variable->deprecated.
 * @param deprecated - the required deprecation toggle.
 * @memberof oval_variable
 */
void oval_variable_set_deprecated(struct oval_variable *, bool deprecated);
/**
 * set attribute @ref oval_variable->datatype.
 * If attribute oval_variable->datatype == @ref OVAL_DATATYPE_UNKNOWN this method shall overwrite the attribute with the datatype parameter.
 * Otherwise the variable state shall be unchanged.
 * @param datatype - (Not @ref OVAL_DATATYPE_UNKNOWN) a the required datatype.
 * @memberof oval_variable
 */
void oval_variable_set_datatype(struct oval_variable *, oval_datatype_t);
/**
 * Append an instance of @ref Oval_value to the attribute @ref Oval_constant->values.
 * If attribute type <> @ref OVAL_VARIABLE_CONSTANT or the value parameter is NULL the state of the oval_variable shall not be changed by this
 * method.
 * Otherwise, The value parameter shall be appended and shall be freed by the API when the Oval_constant is freed
 *
 * @note When a value is appended to an Oval_constant by an application, that value should not be subsequently
 * freed by the application using @ref oval_value_free.
 * @note An application should not append an Oval_value to more than one Oval_constant, and that
 * an Oval_value should not be bound more than once to a single Oval_constant.
 * @param value - the required value.
 * @memberof oval_variable
 */
void oval_variable_add_value(struct oval_variable *, struct oval_value *);	//type==OVAL_VARIABLE_CONSTANT
/**
 * Bind an instance of @ref Oval_component to the attribute @ref Oval_local->component.
 * If attribute type <> @ref OVAL_VARIABLE_LOCAL, the component attribute <> NULL or the component parameter is NULL the state of the oval_variable shall not be changed by this
 * method.
 * Otherwise, The component parameter shall be bound to the component attribute and shall be freed by the API when the Oval_local is freed
 *
 * @note When an Oval_component is bound to an Oval_local by an application, the Oval_component should not be subsequently
 * freed by the application using @ref oval_component_free.
 * @note An application should not bind a single Oval_component to more than one Oval_local or to an Oval_local and an @ref Oval_function.
 * @param component - the required component.
 * @memberof oval_variable
 */
void oval_variable_set_component(struct oval_variable *, struct oval_component *component);	//type==OVAL_VARIABLE_LOCAL
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Returns attribute @ref oval_variable->id.
 * @memberof oval_variable
 */
char *oval_variable_get_id(struct oval_variable *);
/**
 * Returns attribute @ref oval_variable->comment.
 * @memberof oval_variable
 */
char *oval_variable_get_comment(struct oval_variable *);
/**
 * Returns attribute @ref oval_variable->version.
 * @memberof oval_variable
 */
int oval_variable_get_version(struct oval_variable *);
/**
 * Returns attribute @ref oval_variable->deprecated.
 * @memberof oval_variable
 */
bool oval_variable_get_deprecated(struct oval_variable *);
/**
 * Returns attribute @ref oval_variable->type.
 * @memberof oval_variable
 */
oval_variable_type_t oval_variable_get_type(struct oval_variable *);
/**
 * Returns attribute @ref oval_variable->datatype.
 * @memberof oval_variable
 */
oval_datatype_t oval_variable_get_datatype(struct oval_variable *);
/**
 * Returns attribute @ref Oval_external/@ref Oval_constant->values.
 * If attribute type == @ref OVAL_VARIABLE_LOCAL or @ref OVAL_VARIABLE_UNKNOWN, this method shall return NULL
 * @memberof oval_variable
 */
struct oval_value_iterator *oval_variable_get_values(struct oval_variable *);	//type==OVAL_VARIABLE_CONSTANT
/**
 * Returns attribute @ref Oval_local->component.
 * If attribute type <> @ref OVAL_VARIABLE_LOCAL this method shall return NULL.
 * @memberof oval_variable
 */
struct oval_component *oval_variable_get_component(struct oval_variable *);	//type==OVAL_VARIABLE_LOCAL
/**
 * Returns attribute @ref Oval_component_type->text.
 * @memberof oval_variable
 */
const char *oval_component_type_get_text(oval_component_type_t type);
/**
 * return <b>true</b> if the variable instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_variable
 */
bool oval_variable_is_locked(struct oval_variable *variable);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * Returns <b>true</b> if iterator not exhausted.
 * @memberof oval_variable_iterator
 */
bool oval_variable_iterator_has_more(struct oval_variable_iterator *);
/**
 * Returns next instance of @ref oval_variable.
 * @memberof oval_variable_iterator
 */
struct oval_variable *oval_variable_iterator_next(struct oval_variable_iterator *);
/**
 * Free iterator.
 * @memberof oval_variable_iterator
 */
void oval_variable_iterator_free(struct oval_variable_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Returns <b>true</b> if @ref oval_variable is valid.
 * An oval_variable is valid if all of the following are true:
 * 	- The value of the id attribute is matched by the regular expression <b>^oval:[\.A-Za-z0-9_\-]+:var:[1-9][0-9]*$</b>
 * 	- The value of the type attribute is not @ref OVAL_VARIABLE_UNKNOWN
 * 	- The value of the version attribute is a positive integer
 * 	- The value of the datatype attribute is not @ref OVAL_DATATYPE_UNKNOWN
 *	- If type == @ref OVAL_VARIABLE_CONSTANT
 *		- At least one Oval_value is appended to the values attribute.
 *	- If type == @ref OVAL_VARIABLE_LOCAL
 *		- The component attribute is not NULL.
 *		- The bound Oval_component is valid (@ref oval_component_is_valid).
 * @memberof oval_variable
 */
bool oval_variable_is_valid(struct oval_variable *);
/** @} */






/**
 * Construct instance of @ref oval_affected.
 * @memberof oval_affected
 */
struct oval_affected *oval_affected_new(struct oval_definition_model *);
/**
 * Clone instance of @ref oval_affected.
 * @memberof oval_affected
 */
struct oval_affected *oval_affected_clone(struct oval_definition_model *new_model, struct oval_affected *old_affected);
/**
 * Release instance of @ref oval_affected.
 * @memberof oval_affected
 */
void oval_affected_free(struct oval_affected *);

/**
 * @name Setters
 * @{
 */
/**
 * Set @ref oval_affected family.
 * @memberof oval_affected
 */
void oval_affected_set_family(struct oval_affected *, oval_affected_family_t family);
/**
 * Append name to @ref oval_affected platform names.
 * @memberof oval_affected
 */
void oval_affected_add_platform(struct oval_affected *, char *platform_name);
/**
 * Append name to @ref oval_affected product names.
 * @memberof oval_affected
 */
void oval_affected_add_product(struct oval_affected *, char *product_name);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get member value @ref oval_affected family.
 * @note A family value of @ref OVAL_AFCFML_UNDEFINED signifies some family
 * other than one of the defined values is targeted.
 * @memberof oval_affected
 */
oval_affected_family_t oval_affected_get_family(struct oval_affected *);
/**
 * Get member values @ref oval_affected platform_names.
 * If the returned iterator is empty, then the associated Oval_definition is not constrained to a specific platform choice.
 * @memberof oval_affected
 */
struct oval_string_iterator *oval_affected_get_platforms(struct oval_affected *);
/**
 * Get member values @ref oval_affected product_names.
 * If the returned iterator is empty, then the associated Oval_definition is not constrained to a specific product choice.
 * @memberof oval_affected
 */
struct oval_string_iterator *oval_affected_get_products(struct oval_affected *);
/**
 * Return <b>true</b> if the instance of oval_affected is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_affected
 */
bool oval_affected_is_locked(struct oval_affected *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Returns <b>true</b>
 * @memberof oval_affected
 */
bool oval_affected_is_valid(struct oval_affected *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * Return <b>true</b> if iterator has more @ref oval_affected.
 * @memberof oval_affected_iterator
 */
bool oval_affected_iterator_has_more(struct oval_affected_iterator *);
/**
 * Return next instance of @ref oval_affected from iterator.
 * @memberof oval_affected_iterator
 */
struct oval_affected *oval_affected_iterator_next(struct oval_affected_iterator *);
/**
 * Release instance of @ref oval_affected_ iterator.
 * @memberof oval_affected_iterator
 */
void oval_affected_iterator_free(struct oval_affected_iterator *);
/** @} */






/**
 * @memberof oval_reference
 */
struct oval_reference *oval_reference_new(struct oval_definition_model *);
/**
 * @memberof oval_reference
 */
struct oval_reference *oval_reference_clone
    (struct oval_definition_model *new_model, struct oval_reference *old_reference);
/**
 * @memberof oval_reference
 */
void oval_reference_free(struct oval_reference *);


/**
 * @name Setters
 * @{
 */

/**
 * Set OVAL reference source
 * @memberof oval_reference
 */
void oval_reference_set_source(struct oval_reference *, char *);
/**
 * @memberof oval_reference
 */
void oval_reference_set_id(struct oval_reference *, char *);
/**
 * @memberof oval_reference
 */
void oval_reference_set_url(struct oval_reference *, char *);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get OVAL reference source.
 * @memberof oval_reference
 */
char *oval_reference_get_source(struct oval_reference *);
/**
 * Get OVAL reference ID.
 * @memberof oval_reference
 */
char *oval_reference_get_id(struct oval_reference *);
/**
 * Get OVAL reference URL.
 * @memberof oval_reference
 */
char *oval_reference_get_url(struct oval_reference *);
/**
 * return <b>true</b> if the reference instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_reference
 */
bool oval_reference_is_locked(struct oval_reference *reference);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_reference_iterator
 */
bool oval_reference_iterator_has_more(struct oval_reference_iterator *);
/**
 * @memberof oval_reference_iterator
 */
struct oval_reference *oval_reference_iterator_next(struct oval_reference_iterator *);
/**
 * @memberof oval_reference_iterator
 */
void oval_reference_iterator_free(struct oval_reference_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * return <b>true</b> if the reference instance is valid
 * @memberof oval_reference
 */
bool oval_reference_is_valid(struct oval_reference *reference);
/** @} */







/**
 * Construct an instance of @ref oval_criteria_node.
 * Initialized attribute values are:
 * 		- type initialized to value of type parameter.
 *		- negate -- initialized to <b>false</b>
 *		- comment -- initialized to NULL
 *		- If type == @ref OVAL_NODETYPE_CRITERIA (@ref Oval_criteria):
 *			- operator -- initialized to @ref OVAL_OPERATOR_UNKNOWN
 *			- subnodes -- initialized to empty iterator
 *		- If type == @ref OVAL_NODETYPE_CRITERION (@ref Oval_criterion):
 *			- test -- initialized to NULL
 *		- If type == @ref OVAL_NODETYPE_EXTENDDEF (@ref Oval_extends):
 *			- definition -- initialized to NULL
 * Careful ! Calling this function with inappropriate number of arguments will cause undefined behavior !
 *
 * If the type parameter is entered as @ref OVAL_NODETYPE_UNKNOWN, no instance shall be constructed
 * and the method shall return NULL.
 * @param type - the required node type.
 * @memberof oval_criteria_node
 */
struct oval_criteria_node *oval_criteria_node_new(struct oval_definition_model *, oval_criteria_node_type_t type);
/**
 * Clone an instance of @ref oval_criteria_node.
 * @memberof oval_criteria_node
 */
struct oval_criteria_node *oval_criteria_node_clone
    (struct oval_definition_model *new_model, struct oval_criteria_node *old_node);
/**
 * Free an instance of @ref oval_criteria_node.
 * @memberof oval_criteria_node
 */
void oval_criteria_node_free(struct oval_criteria_node *);

/**
 * @name Setters
 * @{
 */
/**
 * Set attribute @ref Oval_criteria_node->negate.
 * @param negate - the required value of the negate attribute
 * @memberof oval_criteria_node
 */
void oval_criteria_node_set_negate(struct oval_criteria_node *, bool negate);
/**
 * Set attribute @ref Oval_criteria_node->type.
 * @param type - the required value of the type attribute
 * @memberof oval_criteria_node
 */
void oval_criteria_set_node_type(struct oval_criteria_node *node, oval_criteria_node_type_t type);
/**
 * set attribute @ref Oval_criteria_node->comment.
 * @param comm - (Not NULL) a copy of the comment parameter is set as  the comment attribute.
 * @memberof oval_criteria_node
 */
void oval_criteria_node_set_comment(struct oval_criteria_node *, char *comment);
/**
 * Set attribute @ref Oval_criteria->operator.
 * If Oval_criteria_node->type == @ref OVAL_NODETYPE_CRITERIA and the value of the operator attribute is @ref OVAL_OPERATOR_UNKNOWN,
 * this method shall overwrite the operator attribute value with the operator parameter.
 * Otherwise the Oval_criteria_node state shall not be changed by this method.
 * @memberof oval_criteria_node
 */
void oval_criteria_node_set_operator(struct oval_criteria_node *, oval_operator_t op);
/**
 * Append instance of @ref Oval_criteria_node to attribute @ref Oval_criteria->subnodes.
 * If Oval_criteria_node->type <> @ref OVAL_NODETYPE_CRITERIA, this method shall return without changing the Oval_criteria_node state.
 * @note Instances of Oval_criteria_node bound to an Oval_criteria by this method should not be subsequently freed by the application using oval_criteria_node_free.
 * These instances shall be freed by the API when the Oval_criteria is freed.
 * @note An application should not bind a given Oval_criteria_node as a subnode to more than one Oval_criteria,
 * nor should a given Oval_criteria_node be bound more than once to a single Oval_criteria.
 * @param - (Not NULL) the subnode to be appended.
 * @memberof oval_criteria_node
 */
void oval_criteria_node_add_subnode(struct oval_criteria_node *, struct oval_criteria_node *node);
/**
 * Sets attribute @ref Oval_criterion->test.
 * If Oval_criteria_node->type == @ref OVAL_NODETYPE_CRITERION and the value of the test attribute is NULL,
 * this method shall overwrite the test attribute value with the test parameter.
 * Otherwise the Oval_criteria_node state shall not be changed by this method.
 * @memberof oval_criteria_node
 */
void oval_criteria_node_set_test(struct oval_criteria_node *, struct oval_test *);
/**
 * Sets attribute @ref Oval_extends->definition.
 * If Oval_criteria_node->type == @ref OVAL_NODETYPE_EXTENDDEF and the value of the definition attribute is NULL,
 * this method shall overwrite the definition attribute value with the definition parameter.
 * Otherwise the Oval_criteria_node state shall not be changed by this method.
 * @memberof oval_criteria_node
 */
void oval_criteria_node_set_definition(struct oval_criteria_node *, struct oval_definition *);	//type==NODETYPE_EXTENDDEF
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Returns attribute @ref Oval_criteria_node->type.
 * @memberof oval_criteria_node
 */
oval_criteria_node_type_t oval_criteria_node_get_type(struct oval_criteria_node *);
/**
 * Returns attribute @ref Oval_criteria_node->negate.
 * @memberof oval_criteria_node
 */
bool oval_criteria_node_get_negate(struct oval_criteria_node *);
/**
 * Returns attribute @ref Oval_criteria_node->comment.
 * @memberof oval_criteria_node
 */
char *oval_criteria_node_get_comment(struct oval_criteria_node *);
/**
 * Returns attribute @ref Oval_criteria->operator HOWDI.
 * @note If Oval_criteria_node->type <> @ref OVAL_NODETYPE_CRITERIA, this method shall return @ref OVAL_OPERATOR_UNKNOWN.
 * @memberof oval_criteria_node
 */
oval_operator_t oval_criteria_node_get_operator(struct oval_criteria_node *);
/**
 * Returns attribute @ref Oval_criteria_node->subnodes.
 * If Oval_criteria_node->type <> @ref OVAL_NODETYPE_CRITERIA, this method shall return NULL.
 * @note An iterator returned by this method should be freed by the calling application.
 * @memberof oval_criteria_node
 */
struct oval_criteria_node_iterator *oval_criteria_node_get_subnodes(struct oval_criteria_node *);
/**
 * Returns attribute @ref Oval_criterion->test.
 * If Oval_criteria_node->type <> @ref OVAL_NODETYPE_CRITERION, this method shall return NULL.
 * @memberof oval_criteria_node
 */
struct oval_test *oval_criteria_node_get_test(struct oval_criteria_node *);
/**
 * Returns attribute @ref Oval_extends->definition.
 * If Oval_criteria_node->type <> @ref OVAL_NODETYPE_EXTENDDEF, this method shall return NULL.
 * @memberof oval_criteria_node
 */
struct oval_definition *oval_criteria_node_get_definition(struct oval_criteria_node *);
/**
 * Return <b>true</b> if the instance of Oval_criteria_node is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_criteria_node
 */
bool oval_criteria_node_is_locked(struct oval_criteria_node *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * Returns <b>true</b> if the iterator is not exhausted.
 * @memberof oval_criteria_node_iterator
 */
bool oval_criteria_node_iterator_has_more(struct oval_criteria_node_iterator *);
/**
 * Returns the next instance of @ref Oval_criteria_node from the iterator.
 * Returns NULL if the iterator is exhausted.
 * @memberof oval_criteria_node_iterator
 */
struct oval_criteria_node *oval_criteria_node_iterator_next(struct oval_criteria_node_iterator *);
/**
 * Free the iterator.
 * @memberof oval_criteria_node_iterator
 */
void oval_criteria_node_iterator_free(struct oval_criteria_node_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Returns <b>true</b> if @ref Oval_criteria_node is valid.
 * An Oval_criteria_node is valid if one of the following is true:
 * 	- The type attribute is @ref OVAL_NODETYPE_CRITERIA (@ref Oval_criteria)-- AND
 * 		- The operator attribute is not @ref OVAL_OPERATOR_UNKNOWN -- AND
 * 		- The subnode attribute is not an empty iterator -- AND
 * 		- Each iterated Oval_criteria_node is valid.
 * 	- The type attribute is @ref OVAL_NODETYPE_CRITERION (@ref Oval_criterion) -- AND
 * 		- The test attribute is not NULL -- AND
 * 		- The referenced test is valid.
 * 	- The type attribute is @ref OVAL_NODETYPE_EXTENDDEF (@ref Oval_extends) -- AND
 * 		- The test attribute is not NULL -- AND
 * 		- The referenced test is valid.
 * @memberof oval_criteria_node
 */
bool oval_criteria_node_is_valid(struct oval_criteria_node *);
/** @} */







/**
 * @memberof oval_object_content
 */
struct oval_object_content *oval_object_content_new(struct oval_definition_model *model, oval_object_content_type_t type);

/**
 * @memberof oval_object_content
 */
struct oval_object_content *oval_object_content_clone
    (struct oval_definition_model *new_model, struct oval_object_content *old_content);

/**
 * @memberof oval_object_content
 */
void oval_object_content_free(struct oval_object_content *);

/**
 * @name Setters
 * @{
 */

/**
 * @memberof oval_object_content
 */
void oval_object_content_set_type(struct oval_object_content *, oval_object_content_type_t);
/**
 * @memberof oval_object_content
 */
void oval_object_content_set_field_name(struct oval_object_content *, char *);
/**
 * @memberof oval_object_content
 */
void oval_object_content_set_entity(struct oval_object_content *, struct oval_entity *);	//type == OVAL_OBJECTCONTENT_ENTITY
/**
 * @memberof oval_object_content
 */
void oval_object_content_set_varCheck(struct oval_object_content *, oval_check_t);	//type == OVAL_OBJECTCONTENT_ENTITY
/**
 * @memberof oval_object_content
 */
void oval_object_content_set_setobject(struct oval_object_content *, struct oval_setobject *);	//type == OVAL_OBJECTCONTENT_SET
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get field name of a object content.
 * @memberof oval_object_content
 */
char *oval_object_content_get_field_name(struct oval_object_content *);
/**
 * Get type of a object content (entity or set).
 * @memberof oval_object_content
 */
oval_object_content_type_t oval_object_content_get_type(struct oval_object_content *);
/**
 * @memberof oval_object_content
 * Get entity of a simple object content.
 */
struct oval_entity *oval_object_content_get_entity(struct oval_object_content *);	//type == OVAL_OBJECTCONTENT_ENTITY
/**
 * Get varCheck of a simple object content.
 * @memberof oval_object_content
 */
oval_check_t oval_object_content_get_varCheck(struct oval_object_content *);	//type == OVAL_OBJECTCONTENT_ENTITY
/**
 * Get varCheck of a set object content.
 * @memberof oval_object_content
 */
struct oval_setobject *oval_object_content_get_setobject(struct oval_object_content *);	//type == OVAL_OBJECTCONTENT_SET
/**
 * return <b>true</b> if the object_content instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_object_content
 */
bool oval_object_content_is_locked(struct oval_object_content *object_content);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_object_content_iterator
 */
bool oval_object_content_iterator_has_more(struct oval_object_content_iterator *);
/**
 * @memberof oval_object_content_iterator
 */
struct oval_object_content *oval_object_content_iterator_next(struct oval_object_content_iterator *);
/**
 * @memberof oval_object_content_iterator
 */
void oval_object_content_iterator_free(struct oval_object_content_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the object_content instance is valid
 * @memberof oval_object_content
 */
bool oval_object_content_is_valid(struct oval_object_content *object_content);
/** @} */







/**
 * @memberof oval_behavior
 */
struct oval_behavior *oval_behavior_new(struct oval_definition_model *);

/**
 * @memberof oval_behavior
 */
struct oval_behavior *oval_behavior_clone(struct oval_definition_model *new_model, struct oval_behavior *old_behavior);
/**
 * @memberof oval_behavior
 */
void oval_behavior_free(struct oval_behavior *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_behavior
 */
void oval_behavior_set_keyval(struct oval_behavior *behavior, const char *key, const char *value);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get OVAL behavior name.
 * @memberof oval_behavior
 */
char *oval_behavior_get_key(struct oval_behavior *);
/**
 * Get OVAL behavior value.
 * @memberof oval_behavior
 */
char *oval_behavior_get_value(struct oval_behavior *);
/**
 * return <b>true</b> if the behavior instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_behavior
 */
bool oval_behavior_is_locked(struct oval_behavior *behavior);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_behavior_iterator
 */
bool oval_behavior_iterator_has_more(struct oval_behavior_iterator *);
/**
 * @memberof oval_behavior_iterator
 */
struct oval_behavior *oval_behavior_iterator_next(struct oval_behavior_iterator *);
/**
 * @memberof oval_behavior_iterator
 */
void oval_behavior_iterator_free(struct oval_behavior_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * @memberof oval_behavior
 * Return <b>true</b> if the behavior instance is valid
 */
bool oval_behavior_is_valid(struct oval_behavior *behavior);
/** @} */







/**
 * @memberof oval_state_content
 */
struct oval_state_content *oval_state_content_new(struct oval_definition_model *);
/**
 * @memberof oval_state_content
 */
struct oval_state_content *oval_state_content_clone (struct oval_definition_model *new_model, struct oval_state_content *old_content);
/**
 * @memberof oval_state_content
 */
void oval_state_content_free(struct oval_state_content *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_state_content
 */
void oval_state_content_set_entity(struct oval_state_content *, struct oval_entity *);
/**
 * @memberof oval_state_content
 */
void oval_state_content_set_varcheck(struct oval_state_content *, oval_check_t);
/**
 * @memberof oval_state_content
 */
void oval_state_content_set_entcheck(struct oval_state_content *, oval_check_t);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get entity of a state content.
 * @memberof oval_state_content
 */
struct oval_entity *oval_state_content_get_entity(struct oval_state_content *);
/**
 * Get multipe variable values processing of a state content.
 * @memberof oval_state_content
 */
oval_check_t oval_state_content_get_var_check(struct oval_state_content *);
/**
 * Get multipe entities processing of a state content.
 * @memberof oval_state_content
 */
oval_check_t oval_state_content_get_ent_check(struct oval_state_content *);
/**
 * Return <b>true</b> if the setobject instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_state_content
 */
bool oval_state_content_is_locked(struct oval_state_content *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @ingroup oval_state_content_iterators
 * @memberof oval_state_content_iterator
 */
bool oval_state_content_iterator_has_more(struct oval_state_content_iterator *);
/**
 * @memberof oval_state_content_iterator
 */
struct oval_state_content *oval_state_content_iterator_next(struct oval_state_content_iterator *);
/**
 * @memberof oval_state_content_iterator
 */
void oval_state_content_iterator_free(struct oval_state_content_iterator *);
/** @} */






/**
 * @memberof oval_value
 */
struct oval_value *oval_value_new(oval_datatype_t datatype, char *text_value);
/**
 * @memberof oval_value
 */
struct oval_value *oval_value_clone(struct oval_value *old_value);
/**
 * @memberof oval_value
 */
void oval_value_free(struct oval_value *);

/**
 * @name Getters
 * @{
 */
/**
 * Get OVAL value datatype.
 * @memberof oval_value
 */
oval_datatype_t oval_value_get_datatype(struct oval_value *);
/**
 * Get OVAL value as a text.
 * @memberof oval_value
 */
char *oval_value_get_text(struct oval_value *);
/**
 * Get OVAL value as a piece of raw binary data.
 * @memberof oval_value
 */
unsigned char *oval_value_get_binary(struct oval_value *);	//datatype==OVAL_DATATYPE_BINARY
/**
 * Get OVAL value as a boolean.
 * @memberof oval_value
 */
bool oval_value_get_boolean(struct oval_value *);	//datatype==OVAL_DATATYPE_BOOLEAN
/**
 * Get OVAL value as a floating point number.
 * @memberof oval_value
 */
float oval_value_get_float(struct oval_value *);	//datatype==OVAL_DATATYPE_FLOAT
/**
 * Get OVAL value as an integer.
 * @memberof oval_value
 */
long oval_value_get_integer(struct oval_value *);	//datatype==OVAL_DATATYPE_INTEGER
/**
 * return <b>true</b> if the value instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_value
 */
bool oval_value_is_locked(struct oval_value *value);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_value_iterator
 */
bool oval_value_iterator_has_more(struct oval_value_iterator *);
/**
 * @memberof oval_value_iterator
 */
struct oval_value *oval_value_iterator_next(struct oval_value_iterator *);
/**
 * @memberof oval_value_iterator
 */
int oval_value_iterator_remaining(struct oval_value_iterator *iterator);
/**
 * @memberof oval_value_iterator
 */
void oval_value_iterator_free(struct oval_value_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the value instance is valid
 * @memberof oval_value
 */
bool oval_value_is_valid(struct oval_value *value);
/** @} */







/**
 * @memberof oval_entity
 */
struct oval_entity *oval_entity_new(struct oval_definition_model *);
/**
 * @memberof oval_entity
 */
struct oval_entity *oval_entity_clone(struct oval_definition_model *model, struct oval_entity *old_entity);
/**
 * @memberof oval_entity
 */
void oval_entity_free(struct oval_entity *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_entity
 */
void oval_entity_set_type(struct oval_entity *, oval_entity_type_t);
/**
 * @memberof oval_entity
 */
void oval_entity_set_datatype(struct oval_entity *, oval_datatype_t);
/**
 * @memberof oval_entity
 */
void oval_entity_set_mask(struct oval_entity *, int);
/**
 * @memberof oval_entity
 */
void oval_entity_set_varref_type(struct oval_entity *, oval_entity_varref_type_t);
/**
 * @memberof oval_entity
 */
void oval_entity_set_variable(struct oval_entity *, struct oval_variable *);
/**
 * @memberof oval_entity
 */
void oval_entity_set_value(struct oval_entity *, struct oval_value *);
/**
 * @memberof oval_entity
 */
void oval_entity_set_name(struct oval_entity *, char *);
/**
 * @memberof oval_entity
 */
void oval_entity_set_operation(struct oval_entity *, oval_operation_t);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get OVAL entity name.
 * @memberof oval_entity
 */
char *oval_entity_get_name(struct oval_entity *);
/**
 * Get OVAL entity type.
 * @memberof oval_entity
 */
oval_entity_type_t oval_entity_get_type(struct oval_entity *);
/**
 * Get OVAL entity datatype.
 * @memberof oval_entity
 */
oval_datatype_t oval_entity_get_datatype(struct oval_entity *);
/**
 * Get OVAL entity operation type.
 * @memberof oval_entity
 */
oval_operation_t oval_entity_get_operation(struct oval_entity *);
/**
 * Get OVAL entity varref variable.
 * @memberof oval_entity
 */
struct oval_variable *oval_entity_get_variable(struct oval_entity *);
/**
 * Get OVAL entity value.
 * @memberof oval_entity
 */
struct oval_value *oval_entity_get_value(struct oval_entity *);
/**
 * Get OVAL entity mask.
 * @memberof oval_entity
 */
int oval_entity_get_mask(struct oval_entity *);
/**
 * Get OVAL entity varref type.
 * @memberof oval_entity
 */
oval_entity_varref_type_t oval_entity_get_varref_type(struct oval_entity *);
/**
 * return <b>true</b> if the entity instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_entity
 */
bool oval_entity_is_locked(struct oval_entity *entity);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_entity_iterator
 */
bool oval_entity_iterator_has_more(struct oval_entity_iterator *);
/**
 * @memberof oval_entity_iterator
 */
struct oval_entity *oval_entity_iterator_next(struct oval_entity_iterator *);
/**
 * @memberof oval_entity_iterator
 */
void oval_entity_iterator_free(struct oval_entity_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the entity instance is valid
 * @memberof oval_entity
 */
bool oval_entity_is_valid(struct oval_entity *entity);
/** @} */







/**
 * @memberof oval_setobject
 */
struct oval_setobject *oval_setobject_new(struct oval_definition_model *);
/**
 * @memberof oval_setobject
 */
struct oval_setobject *oval_setobject_clone(struct oval_definition_model *new_model, struct oval_setobject *old_setobject);
/**
 * @memberof oval_setobject
 */
void oval_setobject_free(struct oval_setobject *);

/**
 * @name Setters
 * @{
 */
/**
 */
void oval_setobject_set_type(struct oval_setobject *, oval_setobject_type_t);
/**
 * @memberof oval_setobject
 */
void oval_setobject_set_operation(struct oval_setobject *, oval_setobject_operation_t);
/**
 * @memberof oval_setobject
 */
void oval_setobject_add_subset(struct oval_setobject *, struct oval_setobject *);	//type==OVAL_SET_AGGREGATE;
/**
 * @memberof oval_setobject
 */
void oval_setobject_add_object(struct oval_setobject *, struct oval_object *);	//type==OVAL_SET_COLLECTIVE;
/**
 * @memberof oval_setobject
 */
void oval_setobject_add_filter(struct oval_setobject *, struct oval_state *);	//type==OVAL_SET_COLLECTIVE;
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get OVAL set object type.
 * @memberof oval_setobject
 */
oval_setobject_type_t oval_setobject_get_type(struct oval_setobject *);
/**
 * Get OVAL set object operation type.
 * @memberof oval_setobject
 */
oval_setobject_operation_t oval_setobject_get_operation(struct oval_setobject *);
/**
 * Get OVAL set object subsets.
 * This works only with sets of OVAL_SET_AGGREGATE type.
 * @memberof oval_setobject
 */
struct oval_setobject_iterator *oval_setobject_get_subsets(struct oval_setobject *);	//type==OVAL_SET_AGGREGATE;
/**
 * Get OVAL set object referenced objects.
 * This works only with sets of OVAL_SET_COLLECTIVE type.
 * @memberof oval_setobject
 */
struct oval_object_iterator *oval_setobject_get_objects(struct oval_setobject *);	//type==OVAL_SET_COLLECTIVE;
/**
 * Get OVAL set object filters.
 * This works only with sets of OVAL_SET_COLLECTIVE type.
 * @memberof oval_setobject
 */
struct oval_state_iterator *oval_setobject_get_filters(struct oval_setobject *);	//type==OVAL_SET_COLLECTIVE;
/**
 * return <b>true</b> if the setobject instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_setobject
 */
bool oval_setobject_is_locked(struct oval_setobject *setobject);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_setobject_iterator
 */
bool oval_setobject_iterator_has_more(struct oval_setobject_iterator *);
/**
 * @memberof oval_setobject_iterator
 */
struct oval_setobject *oval_setobject_iterator_next(struct oval_setobject_iterator *);
/**
 * @memberof oval_setobject_iterator
 */
void oval_setobject_iterator_free(struct oval_setobject_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the setobject instance is valid
 * @memberof oval_setobject
 */
bool oval_setobject_is_valid(struct oval_setobject *setobject);
/** @} */







/**
 * Construct new intance of @ref Oval_component.
 * Attribute values shall be initialized:
 *	- type -- initialized to the value of the type parameter.
 *	- If type == @ref OVAL_FUNCTION_CONCAT
 *		- components -- initialized to empty iterator
 *	- If type == @ref OVAL_FUNCTION_ESCAPE_REGEX
 *		- components -- initialized to empty iterator
 *	- If type == @ref OVAL_FUNCTION_ARITHMETIC
 *		- arithmetic_operation -- initialized to @ref OVAL_ARITHMETIC_UNKNOWN
 *		- components -- initialized to empty iterator
 *	- If type == @ref OVAL_FUNCTION_BEGIN
 *		- prefix -- initialized to NULL
 *		- components -- initialized to empty iterator
 *	- If type == @ref OVAL_FUNCTION_END
 *		- suffix -- initialized to NULL
 *		- components -- initialized to empty iterator
 *	- If type == @ref OVAL_FUNCTION_SPLIT
 *		- delimiter -- initialized to NULL
 *		- components -- initialized to empty iterator
 *	- If type == @ref OVAL_FUNCTION_SUBSTRING
 *		- start, length -- initialized to zero
 *		- components -- initialized to empty iterator
 *	- If type == @ref OVAL_FUNCTION_TIMEDIF
 *		- timedif_format_1, timedif_format_2 -- initialized to @ref OVAL_DATETIME_UNKNOWN
 *		- components -- initialized to empty iterator
 *	- If type == @ref OVAL_FUNCTION_REGEX_CAPTURE
 *		- pattern -- initialized to NULL
 *		- components -- initialized to empty iterator
 *	- If type == @ref OVAL_COMPONENT_LITERAL
 *		- literal -- initialized to NULL
 *	- If type == @ref OVAL_COMPONENT_OBJECTREF
 *		- object -- initialized to NULL
 *		- object_field -- initialized to NULL
 *	- If type == @ref OVAL_COMPONENT_VARREF
 *		- variable -- initialized to NULL
 *
 * This method shall not construct a new instance of Oval_object and shall return NULL if the type parameter is entered as @ref OVAL_COMPONENT_UNKNOWN.
 * @param type - the required component type.
 * @memberof oval_component
 */
struct oval_component *oval_component_new(struct oval_definition_model *, oval_component_type_t type);
/**
 * Clone instance of @ref Oval_component.
 * @memberof oval_component
 */
struct oval_component *oval_component_clone(struct oval_definition_model *new_model,
					    struct oval_component *old_component);
/**
 * Free instance of @ref Oval_component
 * @memberof oval_component
 */
void oval_component_free(struct oval_component *);

/**
 * @name Setters
 * @{
 */
/**
 * Set type of component @ref Oval_component->type
 * @memberof oval_component
 */
void oval_component_set_type(struct oval_component *component, oval_component_type_t type);
/**
 * set attribute @ref Oval_component_object->object.
 * IF component->type <> @ref OVAL_COMPONENT_OBJECTREF OR component->object <> NULL, this method does nothing .
 * @memberof oval_component
 */
void oval_component_set_object(struct oval_component *, struct oval_object *object);
/**
 * set attribute @ref Oval_component_object->object_field.
 * IF component->type <> @ref OVAL_COMPONENT_OBJECTREF OR component->object_field <> NULL, this method does nothing.
 * OTHERWISE the method uses a copy of the field parameter to set component->object_field
 * @memberof oval_component
 */
void oval_component_set_object_field(struct oval_component *, char *field);
/**
 * set attribute @ref Oval_component_object->variable.
 * IF component->type <> @ref OVAL_COMPONENT_OBJECTREF OR component->variable <> NULL, this method does nothing.
 * @memberof oval_component
 */
void oval_component_set_variable(struct oval_component *, struct oval_variable *variable);
/**
 * set attribute @ref Oval_component_object->object_field.
 * IF component->type <> @ref OVAL_COMPONENT_OBJECTREF OR component->object_field <> NULL, this method does nothing.
 * OTHERWISE the method uses a copy of the field parameter to set component->object_field
 * @memberof oval_component
 */
void oval_component_add_function_component(struct oval_component *, struct oval_component *);	//type==OVAL_COMPONENT_FUNCTION
/**
 * @memberof oval_component
 */
void oval_component_set_arithmetic_operation(struct oval_component *, oval_arithmetic_operation_t);	//type==OVAL_COMPONENT_ARITHMETIC
/**
 * @memberof oval_component
 */
void oval_component_set_prefix(struct oval_component *, char *);	//type==OVAL_COMPONENT_BEGIN
/**
 * @memberof oval_component
 */
void oval_component_set_suffix(struct oval_component *, char *);	//type==OVAL_COMPONENT_END
/**
 * @memberof oval_component
 */
void oval_component_set_split_delimiter(struct oval_component *, char *);	//type==OVAL_COMPONENT_SPLIT
/**
 * @memberof oval_component
 */
void oval_component_set_substring_start(struct oval_component *, int);	//type==OVAL_COMPONENT_SUBSTRING
/**
 * @memberof oval_component
 */
void oval_component_set_substring_length(struct oval_component *, int);	//type==OVAL_COMPONENT_SUBSTRING
/**
 * @memberof oval_component
 */
void oval_component_set_timedif_format_1(struct oval_component *, oval_datetime_format_t);	//type==OVAL_COMPONENT_TIMEDIF
/**
 * @memberof oval_component
 */
void oval_component_set_timedif_format_2(struct oval_component *, oval_datetime_format_t);	//type==OVAL_COMPONENT_TIMEDIF
/**
 * @memberof oval_component
 */
void oval_component_set_regex_pattern(struct oval_component *, char *);	//type==OVAL_COMPONENT_REGEX_CAPTURE
/**
 * @memberof oval_component
 */
void oval_component_set_literal_value(struct oval_component *, struct oval_value *);	//type==OVAL_COMPONENT_LITERAL
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Returns attribute @ref Oval_component->type
 * @memberof oval_component
 */
oval_component_type_t oval_component_get_type(struct oval_component *);
/**
 * Returns attribute @ref Oval_component_object->object.
 * IF component->type <> @ref OVAL_COMPONENT_OBJECTREF, this method shall return NULL.
 * @note applications should not free the @ref Oval_object returned by this method
 * @memberof oval_component
 */
struct oval_object *oval_component_get_object(struct oval_component *);	//type==OVAL_COMPONENT_OBJECTREF
/**
 * Returns attribute @ref Oval_component_object->object_field.
 * IF component->type <> @ref OVAL_COMPONENT_OBJECTREF, this method shall return NULL.
 * @note applications should not free the char* returned by this method
 * @memberof oval_component
 */
char *oval_component_get_object_field(struct oval_component *);
/**
 * Returns attribute @ref Oval_component_variable->variable.
 * IF component->type <> @ref OVAL_COMPONENT_VARREF, this method shall return NULL.
 * @note applications should not free the @ref Oval_variable returned by this method
 * @memberof oval_component
 */
struct oval_variable *oval_component_get_variable(struct oval_component *);
/**
 * Returns attribute @ref Oval_function->components.
 * IF component->type < @ref OVAL_COMPONENT_FUNCTION, this method shall return NULL.
 * @memberof oval_component
 */
struct oval_component_iterator *oval_component_get_function_components(struct oval_component *);	//type==OVAL_COMPONENT_FUNCTION
/**
 * Returns attribute @ref Oval_function_ARITHMETIC->arithmetic_operation.
 * IF component->type <> @ref OVAL_FUNCTION_ARITHMETIC, this method shall return @ref OVAL_ARITHMETIC_UNKNOWN.
 * @memberof oval_component
 */
oval_arithmetic_operation_t oval_component_get_arithmetic_operation(struct oval_component *);	//type==OVAL_COMPONENT_ARITHMETIC
/**
 * Returns attribute @ref Oval_function_BEGIN->prefix.
 * IF component->type <> @ref OVAL_FUNCTION_BEGIN, this method shall return NULL
 * @note applications should not free the char* returned by this method
 * @memberof oval_component
 */
char *oval_component_get_prefix(struct oval_component *);	//type==OVAL_COMPONENT_BEGIN
/**
 * Returns attribute @ref Oval_function_END->suffix.
 * IF component->type <> @ref OVAL_FUNCTION_END, this method shall return NULL
 * @note applications should not free the char* returned by this method
 * @memberof oval_component
 */
char *oval_component_get_suffix(struct oval_component *);	//type==OVAL_COMPONENT_END
/**
 * Returns attribute @ref Oval_function_SPLIT->delimiter.
 * IF component->type <> @ref OVAL_FUNCTION_SPLIT, this method shall return NULL
 * @note applications should not free the char* returned by this method
 * @memberof oval_component
 */
char *oval_component_get_split_delimiter(struct oval_component *);	//type==OVAL_COMPONENT_SPLIT
/**
 * Returns attribute @ref Oval_function_SUBSTRING->start.
 * IF component->type <> @ref OVAL_FUNCTION_SUBSTRING, this method shall return 0
 * @memberof oval_component
 */
int oval_component_get_substring_start(struct oval_component *);	//type==OVAL_COMPONENT_SUBSTRING
/**
 * Returns attribute @ref Oval_function_SUBSTRING->length.
 * IF component->type <> @ref OVAL_FUNCTION_SUBSTRING, this method shall return 0
 * @memberof oval_component
 */
int oval_component_get_substring_length(struct oval_component *);	//type==OVAL_COMPONENT_SUBSTRING
/**
 * Returns attribute @ref Oval_function_TIMEDIF->timedif_format_1.
 * IF component->type <> @ref OVAL_FUNCTION_TIMEDIF, this method shall return @ref OVAL_TIMEDATE_UNKNOWN
 * @memberof oval_component
 */
oval_datetime_format_t oval_component_get_timedif_format_1(struct oval_component *);	//type==OVAL_COMPONENT_TIMEDIF
/**
 * Returns attribute @ref Oval_function_TIMEDIF->timedif_format_2.
 * IF component->type <> @ref OVAL_FUNCTION_TIMEDIF, this method shall return @ref OVAL_TIMEDATE_UNKNOWN
 * @memberof oval_component
 */
oval_datetime_format_t oval_component_get_timedif_format_2(struct oval_component *);	//type==OVAL_COMPONENT_TIMEDIF
/**
 * Returns attribute @ref Oval_function_REGEX_CAPTURE->pattern.
 * IF component->type <> @ref OVAL_FUNCTION_REGEX_CAPTURE, this method shall return NULL
 * @note applications should not free the char* returned by this method
 * @memberof oval_component
 */
char *oval_component_get_regex_pattern(struct oval_component *);	//type==OVAL_COMPONENT_REGEX_CAPTURE
/**
 * Returns attribute @ref Oval_literal->literal.
 * IF component->type <> @ref OVAL_COMPONENT_LITERAL, this method shall return NULL
 * @note applications should not free the @ref Oval_value returned by this method
 * @memberof oval_component
 */
struct oval_value *oval_component_get_literal_value(struct oval_component *);	//type==OVAL_COMPONENT_LITERAL
/**
 * return <b>true</b> if the component instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_component
 */
bool oval_component_is_locked(struct oval_component *component);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * Return <b>true</b> if the iterator is not exhausted.
 * @memberof oval_component_iterator
 */
bool oval_component_iterator_has_more(struct oval_component_iterator *);
/**
 * return the next instance of @ref Oval_component.
 * If the iterator is exhausted this method shall return NULL.
 * @memberof oval_component_iterator
 */
struct oval_component *oval_component_iterator_next(struct oval_component_iterator *);
/**
 * Free the iterator.
 * @memberof oval_component_iterator
 */
void oval_component_iterator_free(struct oval_component_iterator *);
/**
 * How many remains.
 * @memberof oval_component_iterator
 */
int oval_component_iterator_remaining(struct oval_component_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/**
 * Returns <b>true</b> if the @ref Oval_component is valid.
 * An Oval_component is valid if one of the following is true:
 * 	- type == @ref OVAL_FUNCTION_CONCAT <b>AND</b>
 *		  components.length > 0 <b>AND</b>
 *		  all appended components are valid
 * 	- type == @ref OVAL_FUNCTION_ARITHMETIC <b>AND</b>
 * 		  arithmetic_operation <> @ref OVAL_ARITHMETIC_UNKNOWN <b>AND</b>
 *		  components.length at least 2 <b>AND</b>
 *		  all appended components are valid
 *	- type == @ref OVAL_FUNCTION_BEGIN <b>AND</b>
 *		  prefix <> NULL AMD
 *		  components.length == 1 <b>AND</b>
 *		  the appended component is valid
 *	- type == @ref OVAL_FUNCTION_END <b>AND</b>
 *		  suffix <> NULL
 *		  components.length == 1 <b>AND</b>
 *		  the appended component is valid
 *	- type == @ref OVAL_FUNCTION_SPLIT <b>AND</b>
 *		  delimiter <> NULL
 *		  components.length == 1 <b>AND</b>
 *		  the appended component is valid
 *	- type == @ref OVAL_FUNCTION_SUBSTRING <b>AND</b>
 *		  start > 0 <b>AND</b> length > 0
 *		  components.length == 1 <b>AND</b>
 *		  the appended component is valid
 *	- type == @ref OVAL_FUNCTION_TIMEDIF <b>AND</b>
 *		  timedif_format_1 <> OVAL_DATETIME_FORMAT_UNKNOWN <b>AND</b>
 *		  components.length > 0 <b>AND</b>
 *		  all appended components are valid <b>AND</b>
 *		  <b>IF</b> timedif_format_2 <> OVAL_DATETIME_FORMAT_UNKNOWN <b>THEN</b>
 *		  components.length is divisible by 2
 *	- type == @ref OVAL_FUNCTION_REGEX_CAPTURE <b>AND</b>
 *		  pattern <> NULL
 *		  components.length == 1 <b>AND</b>
 *		  the appended component is valid
 *	- type == @ref OVAL_FUNCTION_ESCAPE_REGEX <b>AND</b>
 *		  components.length == 1 <b>AND</b>
 *		  the appended component is valid
 *	- type == @ref OVAL_COMPONENT_LITERAL <b>AND</b>
 *		  literal <> NULL
 *	- type == @ref OVAL_COMPONENT_OBJECTREF
 *		  object <> NULL <b>AND</b> object is a valid Oval_object <b>AND</b>
 *		  object_field <> NULL
 *	- type == @ref OVAL_COMPONENT_VARREF
 *		  variable <> NULL <b>AND</b> variable is a valid Oval_variable
 * @memberof oval_component
 */
bool oval_component_is_valid(struct oval_component *);
/** @} */







/**
 * @name Iterators
 * @{
 */
/**
 * Returns <b>true</b> if the iterator is not exhausted.
 * @memberof oval_string_iterator
 */
bool oval_string_iterator_has_more(struct oval_string_iterator *);
/**
 * Returns the next instance of char *.
 * @memberof oval_string_iterator
 */
char *oval_string_iterator_next(struct oval_string_iterator *);
/**
 * Return number for remaining char * elements
 * @memberof oval_string_iterator
 */
int oval_string_iterator_remaining(struct oval_string_iterator *);
/**
 * Frees the iterator.
 * @memberof oval_string_iterator
 */
void oval_string_iterator_free(struct oval_string_iterator *);
/** @} */


















/**
 * @) END OVALDEF
 * @) END OVAL
 */
#endif
