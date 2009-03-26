/*
 * oval_definition.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"
#include "includes/oval_string_map_impl.h"
#include "includes/oval_agent_api_impl.h"

typedef struct oval_definition_s{
	char* id                          ;
	int version                       ;
	oval_definition_class_enum class  ;
	int deprecated                    ;
	char* title                       ;
	char* description                 ;
	struct oval_collection_s *affected      ;
	struct oval_collection_s *reference     ;
	struct oval_criteria_node_s *criteria   ;
} oval_definition_t;


char* oval_definition_id                          (struct oval_definition_s *definition){
	return ((struct oval_definition_s*)definition)->id;
}
int oval_definition_version                       (struct oval_definition_s *definition){
	return ((struct oval_definition_s*)definition)->version;
}
oval_definition_class_enum oval_definition_class  (struct oval_definition_s *definition){
	return ((struct oval_definition_s*)definition)->class;
}
int oval_definition_deprecated                    (struct oval_definition_s *definition){
	return ((struct oval_definition_s*)definition)->deprecated;
}
char* oval_definition_title                       (struct oval_definition_s *definition){
	return ((struct oval_definition_s*)definition)->title;
}
char* oval_definition_description                 (struct oval_definition_s *definition){
	return ((struct oval_definition_s*)definition)->description;
}
struct oval_iterator_affected_s *oval_definition_affected  (struct oval_definition_s *definition){
	return (struct oval_iterator_affected_s*)oval_collection_iterator(definition->affected);
}
struct oval_iterator_reference_s *oval_definition_reference(struct oval_definition_s *definition){
	return (struct oval_iterator_reference_s*)oval_collection_iterator(definition->reference);
}
struct oval_criteria_node_s *oval_definition_criteria        (struct oval_definition_s *definition){
	return ((struct oval_definition_s*)definition)->criteria;
}

struct oval_definition_s * oval_definition_new(){
	struct oval_definition_s *definition = (struct oval_definition_s*)malloc(sizeof(oval_definition_t));
	definition->id          = NULL;
	definition->version     = 0;
	definition->class       = CLASS_UNKNOWN;
	definition->deprecated  = 0;
	definition->title       = NULL;
	definition->description = NULL;
	definition->affected    = oval_collection_new();
	definition->reference   = oval_collection_new();
	definition->criteria    = NULL;
	return definition;
}
void  oval_definition_free(struct oval_definition_s *definition){
	if(definition->id          != NULL)free(definition->id);
	if(definition->title       != NULL)free(definition->title);
	if(definition->description != NULL)free(definition->description);
	if(definition->criteria    != NULL)oval_criteria_node_free(definition->criteria);
	void free_affected(struct oval_collection_item_s *affected){oval_affected_free(affected);}
	oval_collection_free_items(definition->affected, &free_affected);
	void free_reference(struct oval_collection_item_s *reference){oval_reference_free(reference);}
	oval_collection_free_items(definition->reference,&free_reference);
	free(definition);
}

int   oval_iterator_definition_has_more      (struct oval_iterator_definition_s *oc_definition){
	return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_definition);
}
struct oval_definition_s *oval_iterator_definition_next         (struct oval_iterator_definition_s *oc_definition){
	return (struct oval_definition_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_definition);
}

void set_oval_definition_id         (struct oval_definition_s *definition, char* id){
	definition->id = id;
};
void set_oval_definition_version    (struct oval_definition_s *definition, int version){
	definition->version = version;
};
void set_oval_definition_class      (struct oval_definition_s *definition, oval_definition_class_enum class){
	definition->class = class;
};
void set_oval_definition_deprecated (struct oval_definition_s *definition, int deprecated){
	definition->deprecated = deprecated;
};
void set_oval_definition_title      (struct oval_definition_s *definition, char* title){
	definition->title = title;
};
void set_oval_definition_description(struct oval_definition_s *definition, char* description){
	definition->description = description;
};
void set_oval_definition_criteria   (struct oval_definition_s *definition, struct oval_criteria_node_s *criteria){
	definition->criteria = criteria;
};
void add_oval_definition_affected   (struct oval_definition_s *definition, struct oval_affected_s *affected){
};
void add_oval_definition_reference  (struct oval_definition_s *definition, struct oval_reference_s *reference){
};

struct oval_string_map_s *_odaclassMap = NULL;
typedef struct _odaclass_s{
	int value;
} _odaclass_t;
void _odaclass_set(char* name, int val){
	_odaclass_t *enumval = (_odaclass_t*)malloc(sizeof(_odaclass_t));
	enumval->value = val;
	oval_string_map_put(_odaclassMap, name, (struct oval_collection_item_s*)enumval);
}
oval_definition_class_enum _odaclass(char* class){
	if(_odaclassMap==NULL){
		_odaclassMap = oval_string_map_new();
		_odaclass_set("compliance"   , CLASS_COMPLIANCE);
		_odaclass_set("inventory"    , CLASS_INVENTORY);
		_odaclass_set("miscellaneous", CLASS_MISCELLANEOUS);
		_odaclass_set("patch"        , CLASS_PATCH);
		_odaclass_set("vulnerability", CLASS_VULNERABILITY);
	}
	_odaclass_t *valstar = (_odaclass_t*)oval_string_map_get_value(_odaclassMap, class);
	return (valstar==NULL)?CLASS_UNKNOWN:valstar->value;
}

void _oval_definition_title_consumer(char* string, void* user){
	struct oval_definition_s *definition = (struct oval_definition_s*)user;
	char *title = definition->title;
	if(title==NULL) title = string;
	else{
		int newsize = strlen(title)+strlen(string)+1;
		char* newtitle = (char*)malloc(newsize*sizeof(char));
		strcat(newtitle,title);
		strcat(newtitle,string);
		free(title);
		free(string);
		title = newtitle;
	}
	definition->title = title;
}

void _oval_definition_description_consumer(char* string, void* user){
	struct oval_definition_s *definition = (struct oval_definition_s*)user;
	char *description = definition->description;
	if(description==NULL) description = string;
	else{
		int newsize = strlen(description)+strlen(string)+1;
		char* newdescription = (char*)malloc(newsize*sizeof(char));
		strcat(newdescription,description);
		strcat(newdescription,string);
		free(description);
		free(string);
		description = newdescription;
	}
	definition->description = description;
}

void _oval_definition_affected_consumer(struct oval_affected_s *affected, void* user){
	struct oval_definition_s *definition = (struct oval_definition_s*)user;
	oval_collection_add(definition->affected, affected);
}

int _oval_definition_parse_metadata(xmlTextReaderPtr reader, struct oval_parser_context_s *context, void* user) {
	struct oval_definition_s *definition = (struct oval_definition_s*)user;
	xmlChar* tagname   = xmlTextReaderName(reader);
	xmlChar* namespace = xmlTextReaderNamespaceUri(reader);
	int return_code;
	if((strcmp(tagname,"title")==0)){
		return_code = oval_parser_text_value(reader, context, &_oval_definition_title_consumer, definition);
	}else if(strcmp(tagname,"description")==0){
		return_code = oval_parser_text_value(reader, context, &_oval_definition_description_consumer, definition);
	}else if(strcmp(tagname,"affected")==0){
		return_code = oval_affected_parse_tag(reader, context, &_oval_definition_affected_consumer, definition );
	}else if(strcmp(tagname,"oval_repository")==0){//NOOP
		return_code = oval_parser_skip_tag(reader,context);
	}else{
		int linno = xmlTextReaderGetParserLineNumber(reader);
		printf("NOTICE::(oval_definition)skipping <%s> depth = %d line = %d\n",tagname,xmlTextReaderDepth(reader),linno);
		return_code = oval_parser_skip_tag(reader,context);
	}
	return return_code;
}

void _oval_definition_criteria_consumer(struct oval_criteria_node_s *criteria, void* user){
	struct oval_definition_s *definition = (struct oval_definition_s*)user;
	definition->criteria = criteria;
}

int _oval_definition_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context_s *context, void* user) {
	/*
	TODO: definition->reference   = oval_collection_new();
	 */
	struct oval_definition_s *definition = (struct oval_definition_s*)user;
	xmlChar* tagname   = xmlTextReaderName(reader);
	xmlChar* namespace = xmlTextReaderNamespaceUri(reader);
	int return_code;
	if((strcmp(tagname,"metadata")==0)){
		return_code = oval_parser_parse_tag(reader, context, &_oval_definition_parse_metadata, definition);
	}else if((strcmp(tagname,"criteria")==0)){
		return_code = oval_criteria_parse_tag
			(reader, context, &_oval_definition_criteria_consumer, definition);
	}else{
		int linno = xmlTextReaderGetParserLineNumber(reader);
		printf("NOTICE::(oval_definition)skipping <%s> depth = %d line = %d\n",tagname,xmlTextReaderDepth(reader),linno);
		return_code = oval_parser_skip_tag(reader,context);
	}
	return return_code;
}

int oval_definition_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context_s *context) {
	//struct oval_definition_s *definition = oval_definition_new();
	char* id = xmlTextReaderGetAttribute(reader, "id");
	struct oval_object_model_s *model = oval_parser_context_model(context);
	struct oval_definition_s *definition = get_oval_definition_new(model,id);
printf("DEBUG::processing definition %s\n",id);
	char* version = xmlTextReaderGetAttribute(reader, "version");
	set_oval_definition_version(definition,atoi(version));
	free(version);
	char* class = xmlTextReaderGetAttribute(reader, "class");
	set_oval_definition_class  (definition, _odaclass(class));
	free(class);
	int deprecated = oval_parser_boolean_attribute(reader,"deprecated",0);
	set_oval_definition_deprecated(definition,deprecated);
	int return_code = oval_parser_parse_tag(reader, context, &_oval_definition_parse_tag, definition);
	return return_code;
}

void oval_definition_to_print(struct oval_definition_s *definition, char* indent, int index){
	char nxtindent[100];*nxtindent = 0;
	strcat(nxtindent,indent);
	if(index==0)strcat(nxtindent,"DEFINITION.");
	else{
		strcat(nxtindent,"DEFINITION[");
		char itoad[10];*itoad=0;itoa(index,itoad,10);
		strcat(nxtindent,itoad);
		strcat(nxtindent,"].");
	}
	printf("%sID          = %s\n",nxtindent,definition->id);
	printf("%sVERSION     = %d\n",nxtindent,definition->version);
	printf("%sCLASS       = %d\n",nxtindent,definition->class);
	printf("%sDEPRECATED  = %d\n",nxtindent,definition->deprecated);
	printf("%sTITLE       = %s\n",nxtindent,definition->title);
	printf("%sDESCRIPTION = %s\n",nxtindent,definition->description);
	struct oval_iterator_s *affecteds = oval_collection_iterator(definition->affected);
	for(index=1;oval_collection_iterator_has_more(affecteds);index++){
		struct oval_collection_item_s *affected = oval_collection_iterator_next(affecteds);
		oval_affected_to_print(affected,nxtindent,index);
	}
	struct oval_iterator_s *references = oval_collection_iterator(definition->reference);
	for(index=1;oval_collection_iterator_has_more(references);index++){
		struct oval_collection_item_s *reference = oval_collection_iterator_next(references);
		oval_reference_to_print(reference,nxtindent,index);
	}
	if(definition->criteria!=NULL)oval_criteria_node_to_print(definition->criteria,nxtindent,0);
}

