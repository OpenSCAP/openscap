/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALSYS
 * Interface for System Characteristics model.
 * @{
 *
 * Class diagram
 * \image html system_characteristic.png
 *
 * @file
 *
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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
 *      Šimon Lukašík
 */


#ifndef OVAL_SYSTEM_CHARACTERISTICS
#define OVAL_SYSTEM_CHARACTERISTICS

#include "oscap.h"
#include "oscap_source.h"
#include "oval_types.h"
#include "oval_definitions.h"
#include "oscap_export.h"

/// System characteristics result flag
typedef enum {
	SYSCHAR_FLAG_UNKNOWN = 0,
	SYSCHAR_FLAG_ERROR = 1,
	SYSCHAR_FLAG_COMPLETE = 2,
	SYSCHAR_FLAG_INCOMPLETE = 3,
	SYSCHAR_FLAG_DOES_NOT_EXIST = 4,
	SYSCHAR_FLAG_NOT_COLLECTED = 5,
	SYSCHAR_FLAG_NOT_APPLICABLE = 6
} oval_syschar_collection_flag_t;

/// System characteristics status
typedef enum {
	SYSCHAR_STATUS_UNKNOWN = 0,
	SYSCHAR_STATUS_ERROR = 1,
	SYSCHAR_STATUS_EXISTS = 2,
	SYSCHAR_STATUS_DOES_NOT_EXIST = 3,
	SYSCHAR_STATUS_NOT_COLLECTED = 4
} oval_syschar_status_t;

/// Message level.
typedef enum {
        OVAL_MESSAGE_LEVEL_NONE = 0,
        OVAL_MESSAGE_LEVEL_DEBUG = 1,
        OVAL_MESSAGE_LEVEL_INFO = 2,
        OVAL_MESSAGE_LEVEL_WARNING = 3,
        OVAL_MESSAGE_LEVEL_ERROR = 4,
        OVAL_MESSAGE_LEVEL_FATAL = 5
} oval_message_level_t;

OSCAP_API const char *oval_syschar_collection_flag_get_text(oval_syschar_collection_flag_t flag);
OSCAP_API const char *oval_syschar_status_get_text(oval_syschar_status_t status);
OSCAP_API const char *oval_message_level_text(oval_message_level_t);






/**
 * @struct oval_syschar_model
 * OVAL System Characteristics model.
 * Object model holds OVAL system characteristics
 * structure instances.
 */
struct oval_syschar_model;

/**
 * @struct oval_sysinfo
 * General instrumented host description.
 * Instances of Oval_sysinfo specify general information about instrumented host systems,
 * including information that can be used to identify the system.
 */
struct oval_sysinfo;
/**
 * @struct oval_sysinfo_iterator
 */
struct oval_sysinfo_iterator;

/**
 * @struct oval_syschar
 * Characteristics bound to an @ref Oval_object.
 * Instances of Oval_syschar provide relate an Oval_object and @ref Oval_sysitem instances.
 *
 * If an OVAL Object does not exist on the system, then an object is still provided but with the flag attribute set to 'does not exist'
 * and with no reference child elements. This shows that the object was looked for but not found on the system.
 * If no object element is written in this case, users of the system characteristics file will not know whether
 * the object was not found or no attempt was made to collect it.
 *
 * The flag attribute holds information regarding the outcome of the data collection.
 * For example, if there was an error looking for items that match the object specification,
 * then the flag would be 'error'. Please refer to the description of FlagEnumeration for details about the different flag values.
 */
struct oval_syschar;
/**
 * @struct oval_syschar_iterator
 * @see oval_syschar_model_get_syschars
 */
struct oval_syschar_iterator;

/**
 * @struct oval_sysint
 * Network interface description.
 * Instances of Oval_sysint are used to describe existing network interfaces on the system.
 * This information can help identify a specific system on a given network.  *
 */
struct oval_sysint;
/**
 * @struct oval_sysint_iterator
 * @see oval_sysinfo_get_interfaces
 */
struct oval_sysint_iterator;

/**
 * @struct oval_sysitem
 * Each instance of Oval_sysitem represents one item.
 * An item might be a file, a rpm, a process, etc.
 * This element is extended by the different component schemas through substitution groups.
 * Each item represents a unique instance of an object as specified by an OVAL Object.
 * For example, a single file or a single user. Each item my be referenced by more than one object in
 * the collected object section. Please refer to the description of ItemType for more details about the information stored in items.
 */
struct oval_sysitem;
/**
 * @struct oval_sysitem_iterator
 * @see: oval_syschar_get_sysitem
 */
struct oval_sysitem_iterator;

/**
 * @struct oval_sysent
 */
struct oval_sysent;
/**
 * @struct oval_sysent_iterator
 * @see: oval_sysitem_get_sysents
 */
struct oval_sysent_iterator;

/**
 * @struct oval_message
 * OVAL message
 */
struct oval_message;
/**
 * @struct oval_message_iterator
 */
struct oval_message_iterator;

/**
 * @struct oval_variable_binding
 * Binding of an value to an OVAL variable.
 */
struct oval_variable_binding;
/**
 * @struct oval_variable_binding_iterator
 */
struct oval_variable_binding_iterator;





/**
 * Create new oval_syschar_model.
 * The new model is bound to a specified oval_definition_model and variable bindings.
 * @param definition_model the specified oval_definition_model.
 * @memberof oval_syschar_model
 */
OSCAP_API struct oval_syschar_model *oval_syschar_model_new(struct oval_definition_model *definition_model);

/**
 * Import the content from the oscap_source into an oval_syschar_model.
 * If imported content specifies a model entity that is already registered within the model its content is overwritten.
 * @param model the merge target model
 * @param source The oscap_source to import data from.
 * @return zero on success or non zero value if an error occurred
 * @memberof oval_syschar_model
 */
OSCAP_API int oval_syschar_model_import_source(struct oval_syschar_model *model, struct oscap_source *source);

/**
 * Copy an oval_syschar_model.
 * @return A copy of the specified @ref oval_syschar_model.
 * @memberof oval_syschar_model
 */
OSCAP_API struct oval_syschar_model *oval_syschar_model_clone(struct oval_syschar_model *);
/**
 * Export system characteristics into file.
 * @memberof oval_syschar_model
 */
OSCAP_API int oval_syschar_model_export(struct oval_syschar_model *, const char *file);
/**
 * Free memory allocated to a specified syschar model.
 * @param model the specified syschar model
 * @memberof oval_syschar_model
 */
OSCAP_API void oval_syschar_model_free(struct oval_syschar_model *model);

/**
 * @name Setters
 * @{
 */
OSCAP_API void oval_syschar_model_set_generator(struct oval_syschar_model *model, struct oval_generator *generator);
/**
 * Bind a variable model to the definitions bound to the syschar model.
 * @return zero on success or non zero value if an error occurred
 * @memberof oval_syschar_model
 */
OSCAP_API int oval_syschar_model_bind_variable_model(struct oval_syschar_model *, struct oval_variable_model *);
/**
 * @memberof oval_syschar_model
 */
OSCAP_API void oval_syschar_model_set_sysinfo(struct oval_syschar_model *model, struct oval_sysinfo *sysinfo);
/** @} */

/**
 * @name Getters
 * @{
 */
OSCAP_API struct oval_generator *oval_syschar_model_get_generator(struct oval_syschar_model *model);
/**
 * Return related oval_definition_model from an oval_syschar_model.
 * @param model the specified oval_syschar_model.
 * @memberof oval_syschar_model
 */
OSCAP_API struct oval_definition_model *oval_syschar_model_get_definition_model(struct oval_syschar_model *model);
/**
 * Return an iterator over the oval_sychar objects persisted by this model.
 * @param model the specified oval_syschar_model.
 * @memberof oval_syschar_model
 */
OSCAP_API struct oval_syschar_iterator *oval_syschar_model_get_syschars(struct oval_syschar_model *model);
/**
 * Return default sysinfo bound to syschar model.
 * @param model the specified oval_syschar_model.
 * @memberof oval_syschar_model
 */
OSCAP_API struct oval_sysinfo *oval_syschar_model_get_sysinfo(struct oval_syschar_model *model);
/**
 * Return the oval_syschar bound to a specified object_id.
 * Returns NULL if the object_id does not resolve to an oval_object in the bound oval_definition_model.
 * @param model the queried oval_syschar_model.
 * @param object_id the specified object_id.
 * @memberof oval_syschar_model
 */
OSCAP_API struct oval_syschar *oval_syschar_model_get_syschar(struct oval_syschar_model *model, const char *object_id);
/**
 * Get the oval_values bound to a specified variable.
 * @memberof oval_syschar_model
 */
OSCAP_API int oval_syschar_model_compute_variable(struct oval_syschar_model *, struct oval_variable *);
OSCAP_API oval_syschar_collection_flag_t oval_variable_get_collection_flag(struct oval_variable *);
/**
 * @memberof oval_syschar_model
 */
OSCAP_API struct oval_sysitem *oval_syschar_model_get_sysitem(struct oval_syschar_model *, const char *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/** @} */






/**
 * @memberof oval_sysinfo
 */
OSCAP_API struct oval_sysinfo *oval_sysinfo_new(struct oval_syschar_model *);
/**
 * @return A copy of the specified @ref oval_sysinfo.
 * @memberof oval_sysinfo
 */
OSCAP_API struct oval_sysinfo *oval_sysinfo_clone(struct oval_syschar_model *new_model, struct oval_sysinfo *old_sysinfo);
/**
 * @memberof oval_sysinfo
 */
OSCAP_API void oval_sysinfo_free(struct oval_sysinfo *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_sysinfo
 */
OSCAP_API void oval_sysinfo_set_os_name(struct oval_sysinfo *, char *);
/**
 * @memberof oval_sysinfo
 */
OSCAP_API void oval_sysinfo_set_os_version(struct oval_sysinfo *, char *);
/**
 * @memberof oval_sysinfo
 */
OSCAP_API void oval_sysinfo_set_os_architecture(struct oval_sysinfo *, char *);
/**
 * @memberof oval_sysinfo
 */
OSCAP_API void oval_sysinfo_set_primary_host_name(struct oval_sysinfo *, char *);
/**
 * @memberof oval_sysinfo
 */
OSCAP_API void oval_sysinfo_add_interface(struct oval_sysinfo *, struct oval_sysint *);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get operating system name.
 * @memberof oval_sysinfo
 */
OSCAP_API char *oval_sysinfo_get_os_name(struct oval_sysinfo *);

/**
 * Get operating system version.
 * @memberof oval_sysinfo
 */
OSCAP_API char *oval_sysinfo_get_os_version(struct oval_sysinfo *);

/**
 * Get operating system architecture.
 * @memberof oval_sysinfo
 */
OSCAP_API char *oval_sysinfo_get_os_architecture(struct oval_sysinfo *);

/**
 * Get primary host name of the tested machine.
 * @memberof oval_sysinfo
 */
OSCAP_API char *oval_sysinfo_get_primary_host_name(struct oval_sysinfo *);

/**
 * Get an iterator to the list of network interfaces.
 * @memberof oval_sysinfo
 */
OSCAP_API struct oval_sysint_iterator *oval_sysinfo_get_interfaces(struct oval_sysinfo *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_sysinfo_iterator
 */
OSCAP_API bool oval_sysinfo_iterator_has_more(struct oval_sysinfo_iterator *);
/**
 * @memberof oval_sysinfo_iterator
 */
OSCAP_API struct oval_sysinfo *oval_sysinfo_iterator_next(struct oval_sysinfo_iterator *);
/**
 * @memberof oval_sysinfo_iterator
 */
OSCAP_API void oval_sysinfo_iterator_free(struct oval_sysinfo_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/** @} */






/**
 * @memberof oval_syschar
 */
OSCAP_API struct oval_syschar *oval_syschar_new(struct oval_syschar_model *, struct oval_object *);
/**
 * @return A copy of the specified @ref oval_syschar.
 * @memberof oval_syschar
 */
OSCAP_API struct oval_syschar *oval_syschar_clone(struct oval_syschar_model *new_model, struct oval_syschar *old_syschar);
/**
 * @memberof oval_syschar
 */
OSCAP_API void oval_syschar_free(struct oval_syschar *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_syschar
 */
OSCAP_API void oval_syschar_add_variable_binding(struct oval_syschar *, struct oval_variable_binding *);
/**
 * @memberof oval_syschar
 */
OSCAP_API void oval_syschar_set_flag(struct oval_syschar *model, oval_syschar_collection_flag_t flag);
/**
 * @memberof oval_syschar
 */
OSCAP_API void oval_syschar_set_object(struct oval_syschar *, struct oval_object *);
/**
 * Gets the variable_instance attribute of the syschar.
 * @memberof oval_syschar
 * @returns currect variable_instance attribute assigned
 */
OSCAP_API int oval_syschar_get_variable_instance(const struct oval_syschar *syschar);
/**
 * Sets the variable_instance attribute of the syschar.
 * @memberof oval_syschar
 * @param syschar collected object
 * @param variable_instance_in new settings of variable_instance attribute
 */
OSCAP_API void oval_syschar_set_variable_instance(struct oval_syschar *syschar, int variable_instance_in);
/**
 * @memberof oval_syschar
 */
OSCAP_API void oval_syschar_add_sysitem(struct oval_syschar *, struct oval_sysitem *);
/**
 * @memberof oval_syschar
 */
OSCAP_API void oval_syschar_add_message(struct oval_syschar *syschar, struct oval_message *message);
/**
 * @memberof oval_syschar
 */
OSCAP_API void oval_syschar_add_new_message(struct oval_syschar *syschar, char *text, oval_message_level_t level);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get system characteristic flag.
 * @memberof oval_syschar
 */
OSCAP_API oval_syschar_collection_flag_t oval_syschar_get_flag(struct oval_syschar *);

/**
 * Get messages bound to this system characteristic.
 * @memberof oval_syschar
 */
OSCAP_API struct oval_message_iterator *oval_syschar_get_messages(struct oval_syschar *);

/**
 * Get object associated with this system characteristic.
 * @memberof oval_syschar
 */
OSCAP_API struct oval_object *oval_syschar_get_object(struct oval_syschar *);

/**
 * Get system characteristic variable bindings.
 * @memberof oval_syschar
 */
OSCAP_API struct oval_variable_binding_iterator *oval_syschar_get_variable_bindings(struct oval_syschar *);

/**
 * Get system characteristic data.
 * @memberof oval_syschar
 */
OSCAP_API struct oval_sysitem_iterator *oval_syschar_get_sysitem(struct oval_syschar *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_syschar_iterator
 */
OSCAP_API bool oval_syschar_iterator_has_more(struct oval_syschar_iterator *);
/**
 * @memberof oval_syschar_iterator
 */
OSCAP_API struct oval_syschar *oval_syschar_iterator_next(struct oval_syschar_iterator *);
/**
 * @memberof oval_syschar_iterator
 */
OSCAP_API void oval_syschar_iterator_free(struct oval_syschar_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/** @} */






/**
 * @memberof oval_sysint
 */
OSCAP_API struct oval_sysint *oval_sysint_new(struct oval_syschar_model *);
/**
 * @return A copy of the specified @ref oval_sysint.
 * @memberof oval_sysint
 */
OSCAP_API struct oval_sysint *oval_sysint_clone(struct oval_syschar_model *new_model, struct oval_sysint *old_sysint);
/**
 * @memberof oval_sysint
 */
OSCAP_API void oval_sysint_free(struct oval_sysint *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_sysint
 */
OSCAP_API void oval_sysint_set_name(struct oval_sysint *, char *);
/**
 * @memberof oval_sysint
 */
OSCAP_API void oval_sysint_set_ip_address(struct oval_sysint *, char *);
/**
 * @memberof oval_sysint
 */
OSCAP_API void oval_sysint_set_mac_address(struct oval_sysint *, char *);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get interface name.
 * @memberof oval_sysint
 */
OSCAP_API char *oval_sysint_get_name(struct oval_sysint *);

/**
 * Get interface IP address.
 * @memberof oval_sysint
 */
OSCAP_API char *oval_sysint_get_ip_address(struct oval_sysint *);

/**
 * Get interface MAC address.
 * @memberof oval_sysint
 */
OSCAP_API char *oval_sysint_get_mac_address(struct oval_sysint *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_sysint_iterator
 */
OSCAP_API bool oval_sysint_iterator_has_more(struct oval_sysint_iterator *);
/**
 * @memberof oval_sysint_iterator
 */
OSCAP_API struct oval_sysint *oval_sysint_iterator_next(struct oval_sysint_iterator *);
/**
 * @memberof oval_sysint_iterator
 */
OSCAP_API void oval_sysint_iterator_free(struct oval_sysint_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/** @} */






/**
 * @memberof oval_sysitem
 */
OSCAP_API struct oval_sysitem *oval_sysitem_new(struct oval_syschar_model *, const char *id);
/**
 * @return A copy of the specified @ref oval_sysitem.
 * @memberof oval_sysitem
 */
OSCAP_API struct oval_sysitem *oval_sysitem_clone(struct oval_syschar_model *new_model, struct oval_sysitem *old_data);
/**
 * @memberof oval_sysitem
 */
OSCAP_API void oval_sysitem_free(struct oval_sysitem *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_sysitem
 */
OSCAP_API void oval_sysitem_set_status(struct oval_sysitem *, oval_syschar_status_t);
/**
 * @memberof oval_sysitem
 */
OSCAP_API void oval_sysitem_set_subtype(struct oval_sysitem *sysitem, oval_subtype_t subtype);
/**
 * @memberof oval_sysitem
 */
OSCAP_API void oval_sysitem_add_message(struct oval_sysitem *, struct oval_message *);
/**
 * @memberof oval_sysitem
 */
OSCAP_API void oval_sysitem_add_sysent(struct oval_sysitem *, struct oval_sysent *);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get system data ID.
 * @memberof oval_sysitem
 */
OSCAP_API char *oval_sysitem_get_id(struct oval_sysitem *);
/**
 * Get system data status.
 * @memberof oval_sysitem
 */
OSCAP_API oval_syschar_status_t oval_sysitem_get_status(struct oval_sysitem *);
/**
 * Get system data individual items.
 * @memberof oval_sysitem
 */
OSCAP_API struct oval_sysent_iterator *oval_sysitem_get_sysents(struct oval_sysitem *);
/**
 * Get system data message.
 * @memberof oval_sysitem
 */
OSCAP_API struct oval_message_iterator *oval_sysitem_get_messages(struct oval_sysitem *);
/**
 * Get system data subtype.
 * @memberof oval_sysitem
 */
OSCAP_API oval_subtype_t oval_sysitem_get_subtype(struct oval_sysitem *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_sysitem_iterator
 */
OSCAP_API bool oval_sysitem_iterator_has_more(struct oval_sysitem_iterator *);
/**
 * @memberof oval_sysitem_iterator
 */
OSCAP_API struct oval_sysitem *oval_sysitem_iterator_next(struct oval_sysitem_iterator *);
/**
 * @memberof oval_sysitem_iterator
 */
OSCAP_API void oval_sysitem_iterator_free(struct oval_sysitem_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/** @} */






/**
 * @memberof oval_sysent
 */
OSCAP_API struct oval_sysent *oval_sysent_new(struct oval_syschar_model *);
/**
 * @return A copy of the specified @ref oval_sysent.
 * @memberof oval_sysent
 */
OSCAP_API struct oval_sysent *oval_sysent_clone(struct oval_syschar_model *new_model, struct oval_sysent *old_item);
/**
 * @memberof oval_sysent
 */
OSCAP_API void oval_sysent_free(struct oval_sysent *);
/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_sysent
 */
OSCAP_API void oval_sysent_set_name(struct oval_sysent *sysent, char *name);
/**
 * @memberof oval_sysent
 */
OSCAP_API void oval_sysent_set_value(struct oval_sysent *sysent, char *value);
/**
 * @memberof oval_sysent
 */
OSCAP_API void oval_sysent_add_record_field(struct oval_sysent *, struct oval_record_field *);
/**
 * @memberof oval_sysent
 */
OSCAP_API void oval_sysent_set_status(struct oval_sysent *sysent, oval_syschar_status_t status);
/**
 * @memberof oval_sysent
 */
OSCAP_API void oval_sysent_set_datatype(struct oval_sysent *sysent, oval_datatype_t type);
/**
 * @memberof oval_sysent
 */
OSCAP_API void oval_sysent_set_mask(struct oval_sysent *sysent, int mask);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get system data item name.
 * @memberof oval_sysent
 */
OSCAP_API char *oval_sysent_get_name(struct oval_sysent *);

/**
 * Get system data item value.
 * @memberof oval_sysent
 */
OSCAP_API char *oval_sysent_get_value(struct oval_sysent *);

/**
 * @memberof oval_sysent
 */
OSCAP_API struct oval_record_field_iterator *oval_sysent_get_record_fields(struct oval_sysent *);

/**
 * Get system data item status.
 * @memberof oval_sysent
 */
OSCAP_API oval_syschar_status_t oval_sysent_get_status(struct oval_sysent *);

/**
 * Get system data item data type.
 * @memberof oval_sysent
 */
OSCAP_API oval_datatype_t oval_sysent_get_datatype(struct oval_sysent *);

/**
 * Get system data item mask.
 * @memberof oval_sysent
 */
OSCAP_API int oval_sysent_get_mask(struct oval_sysent *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_sysent_iterator
 */
OSCAP_API bool oval_sysent_iterator_has_more(struct oval_sysent_iterator *);
/**
 * @memberof oval_sysent_iterator
 */
OSCAP_API struct oval_sysent *oval_sysent_iterator_next(struct oval_sysent_iterator *);
/**
 * @memberof oval_sysent_iterator
 */
OSCAP_API void oval_sysent_iterator_free(struct oval_sysent_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/** @} */

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_record_field
 */
OSCAP_API void oval_record_field_set_status(struct oval_record_field *, oval_syschar_status_t);
/** @} */
/**
 * @name Getters
 * @{
 */
/**
 * @memberof oval_record_field
 */
OSCAP_API oval_syschar_status_t oval_record_field_get_status(struct oval_record_field *);
/** @} */

/**
 * @memberof oval_message
 */
OSCAP_API struct oval_message *oval_message_new(void);
/**
 * @return A copy of the specified @ref oval_message.
 * @memberof oval_message
 */
OSCAP_API struct oval_message *oval_message_clone(struct oval_message *old_message);
/**
 * @memberof oval_message
 */
OSCAP_API void oval_message_free(struct oval_message *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_message
 */
OSCAP_API void oval_message_set_text(struct oval_message *, char *);
/**
 * @memberof oval_message
 */
OSCAP_API void oval_message_set_level(struct oval_message *, oval_message_level_t);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get OVAL message text.
 * @memberof oval_message
 */
OSCAP_API char *oval_message_get_text(struct oval_message *message);
/**
 * Get OVAL message level.
 * @memberof oval_message
 */
OSCAP_API oval_message_level_t oval_message_get_level(struct oval_message *message);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_message_iterator
 */
OSCAP_API bool oval_message_iterator_has_more(struct oval_message_iterator *oc_message);
/**
 * @memberof oval_message_iterator
 */
OSCAP_API struct oval_message *oval_message_iterator_next(struct oval_message_iterator *oc_message);
/**
 * @memberof oval_message_iterator
 */
OSCAP_API void oval_message_iterator_free(struct oval_message_iterator *oc_message);
/** @} */


/**
 * @memberof oval_variable_binding
 */
OSCAP_API struct oval_variable_binding *oval_variable_binding_new(struct oval_variable *, char *);
/**
 * @return A copy of the specified @ref oval_variable_binding.
 * @memberof oval_variable_binding
 */
struct oval_variable_binding *oval_variable_binding_clone(struct oval_variable_binding *,
							  struct oval_definition_model *);
/**
 * @memberof oval_variable_binding
 */
OSCAP_API void oval_variable_binding_free(struct oval_variable_binding *);

/**
 * @name Setters
 * @{
 */
/**
 * @memberof oval_variable_binding
 */
OSCAP_API void oval_variable_binding_set_variable(struct oval_variable_binding *, struct oval_variable *);
/**
 * @memberof oval_variable_binding
 */
OSCAP_API void oval_variable_binding_add_value(struct oval_variable_binding *, char *);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get variable for this binding.
 * @memberof oval_variable_binding
 */
OSCAP_API struct oval_variable *oval_variable_binding_get_variable(struct oval_variable_binding *);
/**
 * Get value of this binding.
 * @memberof oval_variable_binding
 */
OSCAP_API struct oval_string_iterator *oval_variable_binding_get_values(struct oval_variable_binding *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @memberof oval_variable_binding_iterator
 */
OSCAP_API bool oval_variable_binding_iterator_has_more(struct oval_variable_binding_iterator *);
/**
 * @memberof oval_variable_binding_iterator
 */
OSCAP_API struct oval_variable_binding *oval_variable_binding_iterator_next(struct oval_variable_binding_iterator *);
/**
 * @memberof oval_variable_binding_iterator
 */
OSCAP_API void oval_variable_binding_iterator_free(struct oval_variable_binding_iterator *);
/** @} */

/**
 * @}END OVALSYS
 * @}END OVAL
 */
#endif
