noinst_LTLIBRARIES = libsce.la

libsce_la_SOURCES = $(common_sce_sources)

common_sce_sources = \
	sce_engine.c 

# -I options go to CPPFLAGS, not CFLAGS
AM_CPPFLAGS = @xml2_CFLAGS@ -I$(srcdir)/public -I$(top_srcdir)/src/common/public -I$(top_srcdir)/src 

if WANT_XCCDF
AM_CPPFLAGS += -I$(top_srcdir)/src/XCCDF_POLICY/public -I$(top_srcdir)/src/XCCDF/public -DENABLE_XCCDF
endif

AM_LDFLAGS = @xml2_LIBS@

pkginclude_HEADERS  =	public/sce_engine_api.h 
