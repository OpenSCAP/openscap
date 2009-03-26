/*
 * oval_affected.c
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "includes/oval_definitions_impl.h"
#include "includes/oval_collection_impl.h"
#include "includes/oval_parser_impl.h"
#include "includes/oval_string_map_impl.h"

	typedef struct oval_affected_s{
		oval_affected_family_enum family;
		struct oval_collection_s *platforms;
		struct oval_collection_s *products;
	} oval_affected_t;

	int   oval_iterator_affected_has_more      (struct oval_iterator_affected_s *oc_affected){
		return oval_collection_iterator_has_more((struct oval_iterator_s*)oc_affected);
	}
	struct oval_affected_s *oval_iterator_affected_next         (struct oval_iterator_affected_s *oc_affected){
		return (struct oval_affected_s*)oval_collection_iterator_next((struct oval_iterator_s*)oc_affected);
	}

	oval_affected_family_enum oval_affected_family  (struct oval_affected_s *affected){
		return ((struct oval_affected_s*)affected)->family;
	};

	struct oval_iterator_string_s *oval_affected_platform(struct oval_affected_s *affected){
		return (struct oval_iterator_string_s*)oval_collection_iterator(affected->platforms);
	};
	struct oval_iterator_string_s *oval_affected_product (struct oval_affected_s *affected){
		return (struct oval_iterator_string_s*)oval_collection_iterator(affected->products);
	}

	struct oval_affected_s *oval_affected_new(){
		struct oval_affected_s *affected = (struct oval_affected_s*)malloc(sizeof(oval_affected_t));
		affected->family    = AFCFML_UNKNOWN;
		affected->platforms = oval_collection_new();
		affected->products  = oval_collection_new();
		return affected;
	}
	void  oval_affected_free(struct oval_affected_s *affected){
		void free_string(void* string){
			free(string);
		}
		oval_collection_free_items(affected->platforms, &free_string);
		oval_collection_free_items(affected->products , &free_string);
		free(affected);
	}

	void set_oval_affected_family  (struct oval_affected_s *affected, oval_affected_family_enum family){
		affected->family = family;
	}
	void add_oval_affected_platform(struct oval_affected_s *affected, char* platform){
		oval_collection_add(affected->platforms, platform);
	}
	void add_oval_affected_product (struct oval_affected_s *affected, char* product){
		oval_collection_add(affected->products, product);
	}

	struct oval_string_map_s *_odafamilyMap = NULL;
	typedef struct _odafamily_s{
		int value;
	} _odafamily_t;
	void _odafamily_set(char* name, int val){
		_odafamily_t *enumval = (_odafamily_t*)malloc(sizeof(_odafamily_t));
		enumval->value = val;
		oval_string_map_put(_odafamilyMap, name, enumval);
	}
	oval_affected_family_enum _odafamily(char* family){
		if(_odafamilyMap==NULL){
			_odafamilyMap = oval_string_map_new();
			_odafamily_set("catos"    , AFCFML_CATOS);
			_odafamily_set("ios"      , AFCFML_IOS);
			_odafamily_set("macos"    , AFCFML_MACOS);
			_odafamily_set("pixos"    , AFCFML_PIXOS);
			_odafamily_set("undefined", AFCFML_UNDEFINED);
			_odafamily_set("unix"     , AFCFML_UNIX);
			_odafamily_set("windows"  , AFCFML_WINDOWS);
		}
		_odafamily_t *valstar = (_odafamily_t*)oval_string_map_get_value(_odafamilyMap, family);
		return (valstar==NULL)?CLASS_UNKNOWN:valstar->value;
	}

	int _oval_affected_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context_s *context, void* user) {
		struct oval_affected_s *affected = (struct oval_affected_s*)user;
		int return_code;
		xmlChar* tagname   = xmlTextReaderName(reader);
		xmlChar* namespace = xmlTextReaderNamespaceUri(reader);
		if(strcmp(tagname,"platform")==0){
			char* platform = NULL;
			void consumer(char* text, void* user){
				if(platform==NULL)platform = text;
				else{
					int size = strlen(platform)+strlen(text)+1;
					char* newtext = (char*)malloc(size*sizeof(char*));
					*newtext = 0;
					strcat(newtext,platform);
					strcat(newtext,text);
					free(platform);
					free(text);
					platform = newtext;
				}
			}
			return_code = oval_parser_text_value(reader, context, &consumer, affected);
			if(platform!=NULL)add_oval_affected_platform(affected, platform);
		}else if(strcmp(tagname,"product")==0){
			char* product = NULL;
			void consumer(char* text, void* user){
				if(product==NULL)product = text;
				else{
					int size = strlen(product)+strlen(text)+1;
					char* newtext = (char*)malloc(size*sizeof(char*));
					*newtext = 0;
					strcat(newtext,product);
					strcat(newtext,text);
					free(text);
					free(product);
					product = newtext;
				}
			}
			return_code = oval_parser_text_value(reader, context, &consumer, affected);
			if(product!=NULL)add_oval_affected_product(affected, product);
		}else{
			int linno = xmlTextReaderGetParserLineNumber(reader);
			printf("NOTICE::(oval_affected)skipping <%s> depth = %d line = %d\n",tagname,xmlTextReaderDepth(reader),linno);
			return_code = oval_parser_skip_tag(reader,context);
		}
		return return_code;
	}
	int oval_affected_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context_s *context, oval_affected_consumer consumer, void* user) {
		struct oval_affected_s *affected = oval_affected_new();
		xmlChar* tagname   = xmlTextReaderName(reader);
		xmlChar* namespace = xmlTextReaderNamespaceUri(reader);
		char* family = xmlTextReaderGetAttribute(reader, "family");
		set_oval_affected_family  (affected, _odafamily(family));
		free(family);
		(*consumer)(affected, user);
		return oval_parser_parse_tag(reader, context, &_oval_affected_parse_tag, affected);
	}
	void oval_affected_to_print(struct oval_affected_s *affected, char* indent, int index){
		char nxtindent[100];*nxtindent = 0;
		strcat(nxtindent,indent);
		if(index==0)strcat(nxtindent,"AFFECTED.");
		else{
			strcat(nxtindent,"AFFECTED[");
			char itoad[10];itoa(index,itoad,10);
			strcat(nxtindent,itoad);
			strcat(nxtindent,"].");
		}
		printf("%sFAMILY = %d\n",nxtindent,affected->family);
		struct oval_iterator_s *platforms = oval_collection_iterator(affected->platforms);
		for(index=1;oval_collection_iterator_has_more(platforms);index++){
			struct oval_collection_item_s *platform = oval_collection_iterator_next(platforms);
			printf("%sPLATFORM[%d] = %s\n",nxtindent,index,platform);
		}
		struct oval_iterator_s *products = oval_collection_iterator(affected->products);
		for(index=1;oval_collection_iterator_has_more(products);index++){
			struct oval_collection_item_s *product = oval_collection_iterator_next(products);
			printf("%sPRODUCT[%d] = %s\n",nxtindent,index,product);
		}
	}

