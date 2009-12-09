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

#ifndef OVAL_SYSTEM_CHARACTERISTICS
#define OVAL_SYSTEM_CHARACTERISTICS

#include "oval_definitions.h"

/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALSYS OVAL System Characteristics Interface
 * OVAL System Characteristics interface.
 * @{
 */
/// System characteristics result flag
typedef enum {
	SYSCHAR_FLAG_UNKNOWN         = 0,
	SYSCHAR_FLAG_ERROR           = 1,
	SYSCHAR_FLAG_COMPLETE        = 2,
	SYSCHAR_FLAG_INCOMPLETE      = 3,
	SYSCHAR_FLAG_DOES_NOT_EXIST  = 4,
	SYSCHAR_FLAG_NOT_COLLECTED   = 5,
	SYSCHAR_FLAG_NOT_APPLICABLE  = 6
} oval_syschar_collection_flag_t;

/// System characteristics status
typedef enum{
	SYSCHAR_STATUS_UNKNOWN        = 0,
	SYSCHAR_STATUS_ERROR          = 1,
	SYSCHAR_STATUS_EXISTS         = 2,
	SYSCHAR_STATUS_DOES_NOT_EXIST = 3,
	SYSCHAR_STATUS_NOT_COLLECTED  = 4
} oval_syschar_status_t;
/**
* @addtogroup OVALSYS_setters Setters
* @{
* @ref OVALSYS set methods.
*	These methods will not change the state of a locked instance.
*	@see oval_syschar_model_get_locked
*	@see oval_syschar_model_set_locked
* @}
* @addtogroup OVALSYS_getters Getters
* @{
* @ref OVALSYS get methods.
* @}
* @addtogroup OVALSYS_service Service
* @{
* @ref OVALSYS import/export methods.
* @}
* @addtogroup OVALSYS_eval    Evaluators
* @{
* @ref OVALSYS evaluation methods.
* @}
*/
/**
 * @struct oval_syschar_model
 * OVAL System Characteristics model.
 * Object model holds OVAL system characteristics
 * structure instances.
 */
struct oval_syschar_model;

/**
 * @addtogroup Oval_sysint
 * @{
 * Network interface description.
 * Instances of Oval_sysint are used to describe existing network interfaces on the system.
 * This information can help identify a specific system on a given network. *
 * @addtogroup oval_sysint_setters Setters
 * @{
 * @ref Oval_sysint set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_sysint_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_sysint_getters Getters
 * @{
 * @ref Oval_sysint get methods.
 * @}
 * @addtogroup oval_sysint_iterators Iterators
 * @{
 * @ref Oval_sysint iterator methods.
 * @}
 * @addtogroup oval_sysint_eval Evaluators
 * @{
 * @ref Oval_sysint evaluator methods
 * @}
 */
/**
 * @struct oval_sysint
 * Handle: Oval_systint.
 */
struct oval_sysint;
/**
 * @struct oval_sysint_iterator
 * Handle: Oval_systint iterator.
 */
struct oval_sysint_iterator;
/**
 * @}
 * @addtogroup Oval_sysinfo
 * @{
 * General instrumented host description.
 * Instances of Oval_sysinfo specify general information about instrumented host systems,
 * including information that can be used to identify the system.
 * @addtogroup oval_sysinfo_setters Setters
 * @{
 * @ref Oval_sysinfo set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_sysinfo_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_sysinfo_getters Getters
 * @{
 * @ref Oval_sysinfo get methods.
 * @}
 * @addtogroup oval_sysinfo_iterators Iterators
 * @{
 * @ref Oval_sysinfo iterator methods.
 * @}
 * @addtogroup oval_sysinfo_eval Evaluators
 * @{
 * @ref Oval_sysinfo evaluator methods
 * @}
 */
/**
 * @struct oval_sysinfo
 * Handle: Oval_sysinfo
 */
struct oval_sysinfo;
/**
 * @struct oval_sysinfo_iterator
 * Handle: Oval_sysinfo iterator
 */
struct oval_sysinfo_iterator;
/**
 * @}
 * @addtogroup Oval_syschar
 * @{
 * Characteristics bound to an @ref Oval_object.
 * Instances of Oval_syschar provide relate an Oval_object and @ref Oval_sysdata instances.
 *
 * If an OVAL Object does not exist on the system, then an object is still provided but with the flag attribute set to 'does not exist'
 * and with no reference child elements. This shows that the object was looked for but not found on the system.
 * If no object element is written in this case, users of the system characteristics file will not know whether
 * the object was not found or no attempt was made to collect it.
 *
 * The flag attribute holds information regarding the outcome of the data collection.
 * For example, if there was an error looking for items that match the object specification,
 * then the flag would be 'error'. Please refer to the description of FlagEnumeration for details about the different flag values.
 * @addtogroup oval_syschar_setters Setters
 * @{
 * @ref Oval_syschar set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_syschar_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_syschar_getters Getters
 * @{
 * @ref Oval_syschar get methods.
 * @}
 * @addtogroup oval_syschar_iterators Iterators
 * @{
 * @ref Oval_syschar iterator methods.
 * @}
 * @addtogroup oval_syschar_eval Evaluators
 * @{
 * @ref Oval_syschar evaluator methods
 * @}
 */
/**
 * @struct oval_syschar
 * Handle: Oval_syschar
 */
struct oval_syschar;
/**
 * @struct oval_syschar_iterator
 * Handle: Oval_syschar iterator
 */
struct oval_syschar_iterator;
/**
 * @}
 * @addtogroup Oval_sysdata
 * @{
 * Data item aggregator.
 * Instances of Oval_sysdata aggregate one or more item elements. Each item defines a specific piece of data on the system.
 * @addtogroup oval_sysdata_setters Setters
 * @{
 * @ref Oval_sysdata set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_sysdata_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_sysdata_getters Getters
 * @{
 * @ref Oval_sysdata get methods.
 * @}
 * @addtogroup oval_sysdata_iterators Iterators
 * @{
 * @ref Oval_sysdata iterator methods.
 * @}
 * @addtogroup oval_sysdata_eval Evaluators
 * @{
 * @ref Oval_sysdata evaluator methods
 * @}
 */
/**
 * @struct oval_sysdata
 * Handle: Oval_sysdata
 */
struct oval_sysdata;
/**
 * @struct oval_sysdata_iterator
 * Handle: Oval_sysdata iterator
 */
struct oval_sysdata_iterator;
/**
 * @}
 * @addtogroup Oval_sysitem
 * @{
 * Data item aggregator.
 * Instances of Oval_sysitem holds information about a specific item on a system.
 * An item might be a file, a rpm, a process, etc.
 * This element is extended by the different component schemas through substitution groups.
 * Each item represents a unique instance of an object as specified by an OVAL Object.
 * For example, a single file or a single user. Each item my be referenced by more than one object in
 * the collected object section. Please refer to the description of ItemType for more details about the information stored in items.
 * @addtogroup oval_sysitem_setters Setters
 * @{
 * @ref Oval_sysitem set methods.
 *	These methods will not change the state of a locked instance.
 *	@see oval_sysitem_get_locked
 *	@see oval_syschar_model_set_lock
 * @}
 * @addtogroup oval_sysitem_getters Getters
 * @{
 * @ref Oval_sysitem get methods.
 * @}
 * @addtogroup oval_sysitem_iterators Iterators
 * @{
 * @ref Oval_sysitem iterator methods.
 * @}
 * @addtogroup oval_sysitem_eval Evaluators
 * @{
 * @ref Oval_sysitem evaluator methods
 * @}
 */
/**
 * @struct oval_sysitem
 * Handle: Oval_sysitem
 */
struct oval_sysitem;
/**
 * @struct oval_sysitem_iterator
 * Handle: Oval_sysitem iterator
 */
struct oval_sysitem_iterator;
/**
 * @}
 */
/**
 * Create new oval_syschar_model.
 * The new model is bound to a specified oval_definition_model and variable bindings.
 * @param definition_model the specified oval_definition_model.
 * @ingroup OVALSYS
 */
struct oval_syschar_model *oval_syschar_model_new(
		struct oval_definition_model *definition_model);
/**
 * Copy an oval_syschar_model.
 * @ingroup OVALSYS
 */
struct oval_syschar_model *oval_syschar_model_clone(struct oval_syschar_model *);
/**
 * free memory allocated to a specified syschar model.
 * @param model the specified syschar model
 * @ingroup OVALSYS
 */
void oval_syschar_model_free(struct oval_syschar_model *model);

/**
 * Return related oval_definition_model from an oval_syschar_model.
 * @param model the specified oval_syschar_model.
 * @ingroup OVALSYS_getters
 */
struct oval_definition_model *oval_syschar_model_get_definition_model(
		struct oval_syschar_model *model);
/**
 * Return an iterator over the oval_sychar objects persisted by this model.
 * @param model the specified oval_syschar_model.
 * @ingroup OVALSYS_getters
 */
struct oval_syschar_iterator *oval_syschar_model_get_syschars(
		struct oval_syschar_model *model);

/**
 * Return default sysinfo bound to syschar model.
 * @param model the specified oval_syschar_model.
 * @ingroup OVALSYS_getters
 */
struct oval_sysinfo *oval_syschar_model_get_sysinfo(struct oval_syschar_model *model);

/**
 * Return the oval_syschar bound to a specified object_id.
 * Returns NULL if the object_id does not resolve to an oval_object in the bound oval_definition_model.
 * @param model the queried oval_syschar_model.
 * @param object_id the specified object_id.
 * @ingroup OVALSYS_getters
 */

struct oval_syschar *oval_syschar_model_get_syschar(
		struct oval_syschar_model *model,
		char *object_id);


/**
 * Bind a variable model to the definitions bound to the syschar model.
 * @ingroup OVALSYS_setters
 */
void oval_syschar_model_bind_variable_model
	(struct oval_syschar_model *, struct oval_variable_model *);


/**
 * Probe oval_objects bound to oval_syschar_model
 * @ingroup OVALSYS_eval
 */
void oval_syschar_model_probe_objects(struct oval_syschar_model *);

/**
 * Get the collection flag associated with a specified oval_variable
 * @ingroup OVALSYS_getters
 */
oval_syschar_collection_flag_t oval_syschar_model_get_variable_collection_flag(struct oval_syschar_model *, struct oval_variable *);

/**
 * Get the oval_values bound to a specified variable.
 * @ingroup OVALSYS_getters
 */
struct oval_value_iterator *oval_syschar_model_get_variable_values(struct oval_syschar_model *, struct oval_variable *);
/**
 * @ingroup OVALSYS_getters
 */
struct oval_sysdata *oval_syschar_model_get_sysdata(struct oval_syschar_model *, char*);
/**
 * @ingroup OVALSYS_setters
 */
void oval_syschar_model_add_syschar(struct oval_syschar_model *model, struct oval_syschar *syschar);
/**
 * @ingroup OVALSYS_setters
 */
void oval_syschar_model_add_sysdata(struct oval_syschar_model *model, struct oval_sysdata *sysdata);
/**
 * @ingroup OVALSYS_setters
 */
void oval_syschar_model_set_sysinfo(struct oval_syschar_model *model, struct oval_sysinfo *sysinfo);
/**
 * @ingroup OVALSYS_setters
 */
void oval_syschar_model_add_variable_binding(struct oval_syschar_model *model, struct oval_variable_binding *binding);

/**
 * Export system characteristics as a XML file.
 * @ingroup OVALSYS_service
 */
int oval_syschar_model_export(
		struct oval_syschar_model *, struct oval_export_target *);

/**
 * Load OVAL system characteristics from a file.
 * @ingroup OVALSYS_service
 */
void oval_syschar_model_import(struct oval_syschar_model*, struct oval_import_source*,
			oval_xml_error_handler, void*);




/**
 * @ingroup oval_sysint_iterators
 */
bool                 oval_sysint_iterator_has_more(struct oval_sysint_iterator *);
/**
 * @ingroup oval_sysint_iterators
 */
struct oval_sysint *oval_sysint_iterator_next    (struct oval_sysint_iterator *);
/**
 * @ingroup oval_sysint_iterators
 */
void                oval_sysint_iterator_free    (struct oval_sysint_iterator *);

/**
 * Get interface name.
 * @ingroup oval_sysint_getters
 */
char *oval_sysint_get_name       (struct oval_sysint *);

/**
 * Get interface IP address.
 * @ingroup oval_sysint_getters
 */
char *oval_sysint_get_ip_address (struct oval_sysint *);

/**
 * Get interface MAC address.
 * @ingroup oval_sysint_getters
 */
char *oval_sysint_get_mac_address(struct oval_sysint *);
/**
 * @ingroup oval_sysinfo_iterators
 */
bool                  oval_sysinfo_iterator_has_more(struct oval_sysinfo_iterator *);
/**
 * @ingroup oval_sysinfo_iterators
 */
struct oval_sysinfo *oval_sysinfo_iterator_next    (struct oval_sysinfo_iterator *);
/**
 * @ingroup oval_sysinfo_iterators
 */
void                 oval_sysinfo_iterator_free    (struct oval_sysinfo_iterator *);

/**
 * Get operating system name.
 * @ingroup oval_sysinfo_getters
 */
char                        *oval_sysinfo_get_os_name          (struct oval_sysinfo *);

/**
 * Get operating system version.
 * @ingroup oval_sysinfo_getters
 */
char                        *oval_sysinfo_get_os_version       (struct oval_sysinfo *);

/**
 * Get operating system architecture.
 * @ingroup oval_sysinfo_getters
 */
char                        *oval_sysinfo_get_os_architecture  (struct oval_sysinfo *);

/**
 * Get primary host name of the tested machine.
 * @ingroup oval_sysinfo_getters
 */
char                        *oval_sysinfo_get_primary_host_name(struct oval_sysinfo *);

/**
 * Get an iterator to the list of network interfaces.
 * @ingroup oval_sysinfo_getters
 */
struct oval_sysint_iterator *oval_sysinfo_get_interfaces       (struct oval_sysinfo *);
/**
 * @ingroup oval_sysdata_iterators
 */
bool                  oval_sysdata_iterator_has_more(struct oval_sysdata_iterator *);
/**
 * @ingroup oval_sysdata_iterators
 */
struct oval_sysdata *oval_sysdata_iterator_next    (struct oval_sysdata_iterator *);
/**
 * @ingroup oval_sysdata_iterators
 */
void                 oval_sysdata_iterator_free    (struct oval_sysdata_iterator *);

/**
 * Get system data subtype.
 * @ingroup oval_sysdata_getters
 */
oval_subtype_t                oval_sysdata_get_subtype      (struct oval_sysdata *);

/**
 * Get system data ID.
 * @ingroup oval_sysdata_getters
 */
char                         *oval_sysdata_get_id           (struct oval_sysdata *);

/**
 * Get system data status.
 * @ingroup oval_sysdata_getters
 */
oval_syschar_status_t         oval_sysdata_get_status       (struct oval_sysdata *);

/**
 * Get system data individual items.
 * @ingroup oval_sysdata_getters
 */
struct oval_sysitem_iterator *oval_sysdata_get_items        (struct oval_sysdata *);

/**
 * Get system data message.
 * @ingroup oval_sysdata_getters
 */
char                         *oval_sysdata_get_message      (struct oval_sysdata *);

/**
 * Get system data message level.
 * @ingroup oval_sysdata_getters
 */
oval_message_level_t          oval_sysdata_get_message_level(struct oval_sysdata *);
/**
 * @ingroup oval_sysitem_iterators
 */
bool                  oval_sysitem_iterator_has_more(struct oval_sysitem_iterator *);
/**
 * @ingroup oval_sysitem_iterators
 */
struct oval_sysitem *oval_sysitem_iterator_next    (struct oval_sysitem_iterator *);
/**
 * @ingroup oval_sysitem_iterators
 */
void                 oval_sysitem_iterator_free    (struct oval_sysitem_iterator *);

/**
 * Get system data item name.
 * @ingroup oval_sysitem_getters
 */
char                    *oval_sysitem_get_name    (struct oval_sysitem *);

/**
 * Get system data item value.
 * @ingroup oval_sysitem_getters
 */
char                    *oval_sysitem_get_value   (struct oval_sysitem *);

/**
 * Get system data item status.
 * @ingroup oval_sysitem_getters
 */
oval_syschar_status_t    oval_sysitem_get_status  (struct oval_sysitem *);

/**
 * Get system data item data type.
 * @ingroup oval_sysitem_getters
 */
oval_datatype_t          oval_sysitem_get_datatype(struct oval_sysitem *);

/**
 * Get system data item mask.
 * @ingroup oval_sysitem_getters
 */
int                      oval_sysitem_get_mask    (struct oval_sysitem *);
/**
 * @ingroup oval_syschar_iterators
 */
bool                  oval_syschar_iterator_has_more(struct oval_syschar_iterator *);
/**
 * @ingroup oval_syschar_iterators
 */
struct oval_syschar *oval_syschar_iterator_next    (struct oval_syschar_iterator *);
/**
 * @ingroup oval_syschar_iterators
 */
void                 oval_syschar_iterator_free    (struct oval_syschar_iterator *);

/**
 * Get system characteristic flag.
 * @ingroup oval_syschar_getters
 */
oval_syschar_collection_flag_t         oval_syschar_get_flag             (struct oval_syschar *);

/**
 * Get messages bound to this system characteristic.
 * @ingroup oval_syschar_getters
 */
struct oval_message_iterator          *oval_syschar_get_messages         (struct oval_syschar *);

/**
 * Get object associated with this system characteristic.
 * @ingroup oval_syschar_getters
 */
struct oval_object                    *oval_syschar_get_object           (struct oval_syschar *);

/**
 * Get system characteristic variable bindings.
 * @ingroup oval_syschar_getters
 */
struct oval_variable_binding_iterator *oval_syschar_get_variable_bindings(struct oval_syschar *);

/**
 * Get system characteristic data.
 * @ingroup oval_syschar_getters
 */
struct oval_sysdata_iterator *oval_syschar_get_sysdata              (struct oval_syschar *);


const char *oval_syschar_collection_flag_get_text(oval_syschar_collection_flag_t flag);
const char *oval_syschar_status_get_text(oval_syschar_status_t status);

/**
 * @ingroup Oval_sysint
 */
struct oval_sysint *oval_sysint_new(void);
/**
 * @ingroup Oval_sysint
 */
struct oval_sysint *oval_sysint_clone(struct oval_sysint *);
/**
 * @ingroup Oval_sysint
 */
void oval_sysint_free(struct oval_sysint *);
/**
 * @ingroup oval_sysint_setters
 */
void oval_sysint_set_name(struct oval_sysint *, char *);
/**
 * @ingroup oval_sysint_setters
 */
void oval_sysint_set_ip_address(struct oval_sysint *, char *);
/**
 * @ingroup oval_sysint_setters
 */
void oval_sysint_set_mac_address(struct oval_sysint *, char *);
/**
 * @ingroup Oval_sysinfo
 */
struct oval_sysinfo *oval_sysinfo_new(void);
/**
 * @ingroup Oval_sysinfo
 */
struct oval_sysinfo *oval_sysinfo_clone(struct oval_sysinfo *);
/**
 * @ingroup Oval_sysinfo
 */
void oval_sysinfo_free(struct oval_sysinfo *);
/**
 * @ingroup oval_sysinfo_setters
 */
void oval_sysinfo_set_os_name(struct oval_sysinfo *, char *);
/**
 * @ingroup oval_sysinfo_setters
 */
void oval_sysinfo_set_os_version(struct oval_sysinfo *, char *);
/**
 * @ingroup oval_sysinfo_setters
 */
void oval_sysinfo_set_os_architecture(struct oval_sysinfo *, char *);
/**
 * @ingroup oval_sysinfo_setters
 */
void oval_sysinfo_set_primary_host_name(struct oval_sysinfo *, char *);
/**
 * @ingroup oval_sysinfo_setters
 */
void oval_sysinfo_add_interface(struct oval_sysinfo *, struct oval_sysint *);
/**
 * @ingroup Oval_sysdata
 */
struct oval_sysdata *oval_sysdata_new(char *id);
/**
 * @ingroup Oval_sysdata
 */
struct oval_sysdata *oval_sysdata_clone(struct oval_sysdata *old_data, struct oval_syschar_model *model);
/**
 * @ingroup Oval_sysdata
 */
void oval_sysdata_free(struct oval_sysdata *);
/**
 * @ingroup oval_sysdata_setters
 */
void oval_sysdata_set_status(struct oval_sysdata *, oval_syschar_status_t);
/**
 * @ingroup oval_sysdata_setters
 */
void oval_sysdata_set_subtype(struct oval_sysdata *sysdata, oval_subtype_t subtype);
/**
 * @ingroup oval_sysdata_setters
 */
void oval_sysdata_set_message(struct oval_sysdata *data, char *message);
/**
 * @ingroup oval_sysdata_setters
 */
void oval_sysdata_set_message_level(struct oval_sysdata *data, oval_message_level_t level);
/**
 * @ingroup oval_sysdata_setters
 */
void oval_sysdata_add_item(struct oval_sysdata *, struct oval_sysitem *);
/**
 * @ingroup Oval_syschar
 */
struct oval_syschar *oval_syschar_new(struct oval_object *);
/**
 * @ingroup Oval_syschar
 */
struct oval_syschar *oval_syschar_clone(struct oval_syschar *old_syschar, struct oval_syschar_model *sys_model);
/**
 * @ingroup Oval_syschar
 */
void oval_syschar_free(struct oval_syschar *);
/**
 * @ingroup oval_syschar_setters
 */
void oval_syschar_add_message(struct oval_syschar *syschar, struct oval_message *message);
/**
 * @ingroup oval_syschar_setters
 */
void oval_syschar_set_flag(struct oval_syschar *model, oval_syschar_collection_flag_t flag);
/**
 * @ingroup oval_syschar_setters
 */
void oval_syschar_set_object(struct oval_syschar *, struct oval_object *);
/**
 * @ingroup oval_syschar_setters
 */
void oval_syschar_add_variable_binding(struct oval_syschar *, struct oval_variable_binding *);
/**
 * @ingroup oval_syschar_setters
 */
void oval_syschar_add_sysdata(struct oval_syschar *, struct oval_sysdata *);
/**
 * @ingroup Oval_sysitem
 */
struct oval_sysitem *oval_sysitem_new(void);
/**
 * @ingroup Oval_sysitem
 */
struct oval_sysitem *oval_sysitem_clone(struct oval_sysitem *old_item);
/**
 * @ingroup Oval_sysitem
 */
void oval_sysitem_free(struct oval_sysitem *);
/**
 * @ingroup oval_sysitem_setters
 */
void oval_sysitem_set_name(struct oval_sysitem *sysitem, char *name);
/**
 * @ingroup oval_sysitem_setters
 */
void oval_sysitem_set_value(struct oval_sysitem *sysitem, char *value);
/**
 * @ingroup oval_sysitem_setters
 */
void oval_sysitem_set_status(struct oval_sysitem *sysitem, oval_syschar_status_t status);
/**
 * @ingroup oval_sysitem_setters
 */
void oval_sysitem_set_datatype(struct oval_sysitem *sysitem, oval_datatype_t type);
/**
 * @ingroup oval_sysitem_setters
 */
void oval_sysitem_set_mask(struct oval_sysitem *sysitem, int mask);
/**
 * @}END OVALSYS
 * @}END OVAL
 */
#endif
