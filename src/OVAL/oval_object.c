/*
 * oval_object.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"
#include "oval_agent_api_impl.h"

typedef struct oval_object {
	oval_subtype_enum subtype;
	struct oval_collection *notes;
	char *comment;
	char *id;
	int deprecated;
	int version;
	struct oval_collection *object_content;
	struct oval_collection *behaviors;
} oval_object_t;

int oval_iterator_object_has_more(struct oval_iterator_object *oc_object)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_object);
}

struct oval_object *oval_iterator_object_next(struct oval_iterator_object
					      *oc_object)
{
	return (struct oval_object *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_object);
}

oval_family_enum oval_object_family(struct oval_object *object)
{
	return ((object->subtype) / 1000) * 1000;
}

oval_subtype_enum oval_object_subtype(struct oval_object * object)
{
	return ((struct oval_object *)object)->subtype;
}

struct oval_iterator_string *oval_object_notes(struct oval_object *object)
{
	return (struct oval_iterator_string *)oval_collection_iterator(object->
								       notes);
}

char *oval_object_comment(struct oval_object *object)
{
	return ((struct oval_object *)object)->comment;
}

char *oval_object_id(struct oval_object *object)
{
	return ((struct oval_object *)object)->id;
}

int oval_object_deprecated(struct oval_object *object)
{
	return ((struct oval_object *)object)->deprecated;
}

int oval_object_version(struct oval_object *object)
{
	return ((struct oval_object *)object)->version;
}

struct oval_iterator_object_content *oval_object_object_content(struct
								oval_object
								*object)
{
	return (struct oval_iterator_object_content *)
	    oval_collection_iterator(object->object_content);
}

struct oval_iterator_behavior *oval_object_behaviors(struct oval_object *object)
{
	return (struct oval_iterator_behavior *)
	    oval_collection_iterator(object->behaviors);
}

struct oval_object *oval_object_new()
{
	oval_object_t *object = (oval_object_t *) malloc(sizeof(oval_object_t));
	object->comment = NULL;
	object->id = NULL;
	object->subtype = OVAL_SUBTYPE_UNKNOWN;
	object->deprecated = 0;
	object->version = 0;
	object->behaviors = oval_collection_new();
	object->notes = oval_collection_new();
	object->object_content = oval_collection_new();
	return object;
}

void oval_object_free(struct oval_object *object)
{
	if (object->comment != NULL)
		free(object->comment);
	if (object->id != NULL)
		free(object->id);
	void free_behavior(void *behavior) {
		oval_behavior_free((struct oval_behavior *)behavior);
	}
	void free_notes(void *note) {
		free(note);
	}
	void free_content(void *content) {
		oval_object_content_free((struct oval_object_content *)content);
	}
	oval_collection_free_items(object->behaviors, &free_behavior);
	oval_collection_free_items(object->notes, &free_notes);
	oval_collection_free_items(object->object_content, &free_content);
	free(object);
}

void set_oval_object_id(struct oval_object *object, char *id)
{
	object->id = id;
}

void set_oval_object_subtype(struct oval_object *object,
			     oval_subtype_enum subtype)
{
	object->subtype = subtype;
}

void add_oval_object_notes(struct oval_object *object, char *note)
{
	oval_collection_add(object->notes, (void *)note);
}

void set_oval_object_comment(struct oval_object *object, char *comment)
{
	object->comment = comment;
}

void set_oval_object_deprecated(struct oval_object *object, int deprecated)
{
	object->deprecated = deprecated;
}

void set_oval_object_version(struct oval_object *object, int version)
{
	object->version = version;
}

void add_oval_object_object_content(struct oval_object *object,
				    struct oval_object_content *content)
{
	oval_collection_add(object->object_content, (void *)content);
}

void add_oval_object_behaviors(struct oval_object *object,
			       struct oval_behavior *behavior)
{
	oval_collection_add(object->behaviors, (void *)behavior);
}

int _oval_object_parse_notes(xmlTextReaderPtr reader,
			     struct oval_parser_context *context, void *user)
{
	struct oval_object *object = (struct oval_object *)user;
	void note_consumer(char *text, void *null) {
		add_oval_object_notes(object, text);
	}
	return oval_parser_text_value(reader, context, &note_consumer, NULL);
}

int _oval_object_parse_tag(xmlTextReaderPtr reader,
			   struct oval_parser_context *context, void *user)
{
	struct oval_object *object = (struct oval_object *)user;
	xmlChar *tagname = xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = 1;
	if ((strcmp(tagname, "notes") == 0)) {
		return_code =
		    oval_parser_parse_tag(reader, context,
					  &_oval_object_parse_notes, object);
	} else if (strcmp(tagname, "behaviors") == 0) {
		void behavior_consumer(struct oval_behavior *behavior,
				       void *null) {
			add_oval_object_behaviors(object, behavior);
		}
		return_code =
		    oval_behavior_parse_tag(reader, context,
					    oval_object_family(object),
					    &behavior_consumer, NULL);
	} else {
		void content_consumer(struct oval_object_content *content,
				      void *null) {
			add_oval_object_object_content(object, content);
		}
		return_code =
		    oval_object_content_parse_tag(reader, context,
						  &content_consumer, NULL);
	}
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_object_parse_tag::parse of %s terminated on error at <%s> line %d\n",
		     object->id, tagname, line);
	}
	free(tagname);
	free(namespace);
	return return_code;
}

int oval_object_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context)
{
	char *id = xmlTextReaderGetAttribute(reader, "id");
	struct oval_object_model *model = oval_parser_context_model(context);
	//printf("DEBUG::oval_object_parse_tag::id = %s\n", id);
	struct oval_object *object = get_oval_object_new(model, id);
	oval_subtype_enum subtype = oval_subtype_parse(reader);
	set_oval_object_subtype(object, subtype);
	char *comment = xmlTextReaderGetAttribute(reader, "comment");
	set_oval_object_comment(object, comment);
	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	set_oval_object_deprecated(object, deprecated);
	char *version = xmlTextReaderGetAttribute(reader, "version");
	set_oval_object_version(object, atoi(version));
	free(version);

	int return_code =
	    oval_parser_parse_tag(reader, context, &_oval_object_parse_tag,
				  object);
	return return_code;
}

void oval_object_to_print(struct oval_object *object, char *indent, int index)
{
	char nxtindent[100];
	*nxtindent = 0;
	if (strlen(indent) > 80) {
		strcat(nxtindent, "....");
		strcat(nxtindent, indent + 70);
	} else
		strcat(nxtindent, indent);
	if (index == 0)
		strcat(nxtindent, "OBJECT.");
	else {
		strcat(nxtindent, "OBJECT[");
		char itoad[10];
		*itoad = 0;
		itoa(index, itoad, 10);
		strcat(nxtindent, itoad);
		strcat(nxtindent, "].");
	}
	printf("%sID         = %s\n", nxtindent, oval_object_id(object));
	printf("%sFAMILY     = %d\n", nxtindent, oval_object_family(object));
	printf("%sSUBTYPE    = %d\n", nxtindent, oval_object_subtype(object));
	printf("%sVERSION    = %d\n", nxtindent, oval_object_version(object));
	printf("%sCOMMENT    = %s\n", nxtindent, oval_object_comment(object));
	printf("%sDEPRECATED = %d\n", nxtindent,
	       oval_object_deprecated(object));
	struct oval_iterator_string *notes = oval_object_notes(object);
	for (index = 1; oval_iterator_string_has_more(notes); index++) {
		printf("%sNOTE[%d]    = %s\n", nxtindent, index,
		       oval_iterator_string_next(notes));
	}
	struct oval_iterator_behavior *behaviors =
	    oval_object_behaviors(object);
	for (index = 1; oval_iterator_behavior_has_more(behaviors); index++) {
		struct oval_behavior *behavior =
		    oval_iterator_behavior_next(behaviors);
		oval_behavior_to_print(behavior, nxtindent, index);
	}
	struct oval_iterator_object_content *contents =
	    oval_object_object_content(object);
	for (index = 1; oval_iterator_object_content_has_more(contents);
	     index++) {
		struct oval_object_content *content =
		    oval_iterator_object_content_next(contents);
		oval_object_content_to_print(content, nxtindent, index);
	}
}
