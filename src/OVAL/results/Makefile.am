noinst_LTLIBRARIES = libovalresults.la

libovalresults_la_SOURCES = \
	oval_resultCriteriaNode.c \
	oval_resultDefinition.c \
	oval_resultDefinitionIterator.c \
	oval_results_impl.h \
	oval_resultTest.c \
	oval_resultTestIterator.c

libovalresults_la_CPPFLAGS  = \
	@xml2_CFLAGS@ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/public \
	-I$(top_srcdir)/src/OVAL/public \
	-I$(top_srcdir)/src/OVAL

libovalresults_la_LIBADD = \
	@xml2_LIBS@

pkginclude_HEADERS = $(top_srcdir)/src/OVAL/public/oval_results.h
