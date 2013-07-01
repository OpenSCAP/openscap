noinst_LTLIBRARIES = libovaladt.la

libovaladt_la_SOURCES = \
	oval_collection.c \
	oval_collection_impl.h \
	oval_string_map.c \
	oval_string_map_impl.h

libovaladt_la_CPPFLAGS  = \
	@xml2_CFLAGS@ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/OVAL/public \
	-I$(srcdir)/probes/SEAP/public \
	-I$(top_srcdir)/src/OVAL/probes/SEAP/generic \
	-DSEAP_MSGID_BITS=32 \
	-DSEAP_THREAD_SAFE

libovaladt_la_LIBADD = \
	@xml2_LIBS@

pkginclude_HEADERS = $(top_srcdir)/src/OVAL/public/oval_adt.h
