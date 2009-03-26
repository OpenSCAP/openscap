/*
 * dom_parser.c
 *
 *  Created on: Feb 25, 2009
 *      Author: david.niemoller
 */
#include <libxml/xmlreader.h>
#include<stddef.h>
#include<liboval/oval_agent_api.h>
#include "includes/oval_parser_impl.h"
#include "includes/oval_definitions_impl.h"


struct oval_parser_context_s{
	int depth;
	struct oval_object_model_s *model;
};

struct oval_object_model_s *oval_parser_context_model(struct oval_parser_context_s *context){
	return context->model;
}


int _oval_parser_main_error_handler(struct oval_xml_error_s *error, void* user_arg){return 0;}

int oval_parser_main(int argc, char **argv)
{
    printf("Testing\n");

    char *docname;

    if (argc <= 1)
    {
            printf("Usage: %s docname\n", argv[0]);
            return(0);
    }

    struct oval_object_model_s *model = oval_object_model_new();
    oval_xml_error_handler eh = &_oval_parser_main_error_handler;

    docname = argv[1];
    oval_parser_parse (model,docname,eh,NULL);

	struct oval_iterator_definition_s *definitions = get_oval_definitions(model);
	if(!oval_iterator_definition_has_more(definitions))printf("NO DEFINITIONS FOUND\n");
	int index;for(index=1;oval_iterator_definition_has_more(definitions);index++){
		struct oval_definition_s* definition = oval_iterator_definition_next(definitions);
		oval_definition_to_print(definition, "",index);
	}

    return (1);
}

struct _libxml_user_s{
	oval_xml_error_handler error_handler;
	void*                  user_arg;
};

void _libxml_error_handler(void *user, const char *message, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator){
	char msgfield[strlen(message)+1];strcat(msgfield,message);
	struct oval_xml_error_s xml_error;
	xml_error.message     = msgfield;
	xml_error.severity    = severity;
	xml_error.system_id   = xmlTextReaderLocatorBaseURI(locator);
	xml_error.line_number = xmlTextReaderLocatorLineNumber(locator);

	(*((struct _libxml_user_s*)user)->error_handler)(&xml_error,((struct _libxml_user_s*)user)->user_arg);
	//(*user->error_handler)(NULL,NULL);
}

typedef int (*_oval_parser_process_tag_func)(xmlTextReaderPtr reader, struct oval_parser_context_s *context);

int _oval_parser_process_tags(xmlTextReaderPtr reader, struct oval_parser_context_s *context, _oval_parser_process_tag_func tag_func) {
	const int depth = xmlTextReaderDepth(reader);
	int return_code;
	while((return_code = xmlTextReaderRead(reader)) == 1){
		switch(xmlTextReaderNodeType(reader)){
			case XML_READER_TYPE_ELEMENT:{
				return_code = (*tag_func)(reader, context);
			}break;
			case XML_READER_TYPE_END_ELEMENT:{
				if(depth==xmlTextReaderDepth(reader))return return_code;
			}break;
		}
		if(return_code != 1)break;
	}
	return return_code;
}

int _oval_parser_process_node(xmlTextReaderPtr reader, struct oval_parser_context_s *context) {
	const char* oval_namespace = "http://oval.mitre.org/XMLSchema/oval-definitions-5";
	const char* tagname_definitions = "definitions";
	const char* tagname_tests       = "tests";
	const char* tagname_objects     = "objects";
	const char* tagname_states      = "states";
	const char* tagname_variables   = "variables";
	int return_code = xmlTextReaderRead(reader);
	while(return_code == 1){
		if(xmlTextReaderNodeType(reader)==XML_READER_TYPE_ELEMENT){
			if(xmlTextReaderDepth(reader)>0){
				xmlChar* tagname   = xmlTextReaderName(reader);
printf("DEBUG::_oval_parser_process_node::<%s>\n",tagname);
				xmlChar* namespace = xmlTextReaderNamespaceUri(reader);
				if((strcmp(tagname,tagname_definitions)==0)){
					return_code = _oval_parser_process_tags(reader, context, &oval_definition_parse_tag);
				}else if(strcmp(tagname,tagname_tests)==0){
					return_code = _oval_parser_process_tags(reader, context, &oval_test_parse_tag);
				}else if(strcmp(tagname,tagname_objects)==0){
					return_code = _oval_parser_process_tags(reader, context, &oval_object_parse_tag);
				}else if(strcmp(tagname,tagname_states)==0){
					return_code = _oval_parser_process_tags(reader, context, &oval_parser_skip_tag);
				}else if(strcmp(tagname,tagname_variables)==0){
					return_code = _oval_parser_process_tags(reader, context, &oval_parser_skip_tag);
				}else{
					return_code = oval_parser_skip_tag(reader,context);
				}
			}else return_code = xmlTextReaderRead(reader);
			if((return_code==1) && (xmlTextReaderNodeType(reader)!=XML_READER_TYPE_ELEMENT)){
				return_code = xmlTextReaderRead(reader);
			}
	    }else if(xmlTextReaderDepth(reader)>0){
			return_code = xmlTextReaderRead(reader);
		}else break;
	}
	return return_code;
}

void oval_parser_parse
	(struct oval_object_model_s *model, char *docname, oval_xml_error_handler eh, void* user_arg){
    xmlTextReaderPtr reader;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlNode *node;
    int ret;
    doc = xmlParseFile(docname);
    reader = xmlNewTextReaderFilename(docname);
    if (reader != NULL)
    {
        struct _libxml_user_s user;
        user.error_handler = eh;
        user.user_arg      = user_arg;
        xmlTextReaderSetErrorHandler(reader,&_libxml_error_handler,&user);
    	struct oval_parser_context_s context;
    	context.depth = 0;
    	context.model = model;
        _oval_parser_process_node(reader, &context);
    }
}

int oval_parser_skip_tag(xmlTextReaderPtr reader, struct oval_parser_context_s *context) {
	int depth = xmlTextReaderDepth(reader);
	int return_code;
	while(((return_code = xmlTextReaderRead(reader))==1) && xmlTextReaderDepth(reader)>depth);
	return return_code;
}

int oval_parser_text_value(xmlTextReaderPtr reader, struct oval_parser_context_s *context, oval_xml_value_consumer consumer, void* user){
	int depth = xmlTextReaderDepth(reader);
	int return_code;
	while(((return_code = xmlTextReaderRead(reader))==1) && xmlTextReaderDepth(reader)>depth){
		int nodetype = xmlTextReaderNodeType(reader);
		if(nodetype==XML_READER_TYPE_CDATA || nodetype == XML_READER_TYPE_TEXT){
			char* value = xmlTextReaderValue(reader);
			(*consumer)(value,user);
		}
	}
	return return_code;
}

int oval_parser_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context_s *context, oval_xml_tag_parser tag_parser, void* user){
	int depth = xmlTextReaderDepth(reader);

	int return_code = xmlTextReaderRead(reader);
	while((return_code == 1) && (xmlTextReaderDepth(reader)>depth)){
		int linno = xmlTextReaderGetParserLineNumber(reader);
		int colno = xmlTextReaderGetParserColumnNumber(reader);
		if(xmlTextReaderNodeType(reader)==XML_READER_TYPE_ELEMENT){
			return_code = (*tag_parser)(reader, context, user);
			if(return_code==1){
				if(xmlTextReaderNodeType(reader)!=XML_READER_TYPE_ELEMENT){
					return_code = xmlTextReaderRead(reader);
				}else{
					int newlinno = xmlTextReaderGetParserLineNumber(reader);
					int newcolno = xmlTextReaderGetParserColumnNumber(reader);
					if(newlinno==linno && newcolno==colno)return_code=xmlTextReaderRead(reader);
				}
			}
		}else if(xmlTextReaderDepth(reader)>depth){
			return_code = xmlTextReaderRead(reader);
		}else break;
	}
	return return_code;
}

int oval_parser_boolean_attribute(xmlTextReaderPtr reader,char* attname,int defval){
	char* string = xmlTextReaderGetAttribute(reader,attname);
	int booval;
	if(string == NULL)         booval = defval;
	else{
		if (strlen(string)==1)booval = (*string=='1');
		else                  booval = (strcmp(string,"true")==0);
		free(string);
	}
	return booval;
}
