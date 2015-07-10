noinst_LTLIBRARIES = \
	libovalcmp.la \
	libovalresults.la

libovalcmp_la_SOURCES = \
	oval_cmp.c \
	oval_cmp_impl.h \
	oval_cmp_basic.c \
	oval_cmp_basic_impl.h \
	oval_cmp_evr_string.c \
	oval_cmp_evr_string_impl.h \
	oval_cmp_ip_address.c \
	oval_cmp_ip_address_impl.h

libovalresults_la_SOURCES = \
	oval_resModel.c \
	oval_resultCriteriaNode.c \
	oval_resultDefinition.c \
	oval_resultDefinitionIterator.c \
	oval_resultItem.c \
	oval_results_impl.h \
	oval_resultSystem.c \
	oval_resultTest.c \
	oval_resultTestIterator.c

libovalcmp_la_CPPFLAGS = \
	@xml2_CFLAGS@ \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/public \
	-I$(top_srcdir)/src/source/public \
	-I$(top_srcdir)/src/OVAL/public

libovalresults_la_CPPFLAGS  = \
	@xml2_CFLAGS@ \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/public \
	-I$(top_srcdir)/src/source/public \
	-I$(top_srcdir)/src/OVAL/public \
	-I$(top_srcdir)/src/OVAL \
	-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
	-DSEAP_MSGID_BITS=32 \
	-DSEAP_THREAD_SAFE

libovalresults_la_LIBADD = \
	@xml2_LIBS@ \
	libovalcmp.la

pkginclude_HEADERS = $(top_srcdir)/src/OVAL/public/oval_results.h
