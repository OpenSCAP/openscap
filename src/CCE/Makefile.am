noinst_LTLIBRARIES = libcce.la

libcce_la_SOURCES  = cce.c clearCCE.c initCCE.c list_Refs_add.c
libcce_la_SOURCES += list_Refs_clear.c list_XMLCHAR_add.c
libcce_la_SOURCES += list_XMLCHAR_clear.c parseDoc.c processDescription.c
libcce_la_SOURCES += processNode.c processParameter.c processRefs.c
libcce_la_SOURCES += processTechMech.c validate.c
libcce_la_SOURCES += list_Refs.h list_xmlChar.h

libcce_la_CFLAGS   = @libxml_cflags@
libcce_la_LDFLAGS  = @libxml_libs@

include_HEADERS = cce.h
