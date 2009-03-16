noinst_LTLIBRARIES = libcce.la

libcce_la_SOURCES  = clearCCE.c initCCE.c list_Refs_add.c
libcce_la_SOURCES += list_Refs_clear.c list_cstring_add.c
libcce_la_SOURCES += list_cstring_clear.c parseDoc.c processDescription.c
libcce_la_SOURCES += processNode.c processParameter.c processRefs.c
libcce_la_SOURCES += processTechMech.c validate.c
libcce_la_SOURCES += list_Refs.h list_cstring.h

libcce_la_CFLAGS   = @libxml_cflags@
libcce_la_LDFLAGS  = @libxml_libs@

include_HEADERS = cce.h
includedir = $(prefix)/include/lib@PACKAGE@
