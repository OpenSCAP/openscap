lib_LTLIBRARIES = libopenscap_sce.la

libopenscap_sce_la_SOURCES = sce_engine.c module_entry.c
libopenscap_sce_la_LIBADD = $(top_builddir)/lib/libgnu.la
libopenscap_sce_la_LDFLAGS = -no-undefined -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

# -I options go to CPPFLAGS, not CFLAGS
AM_CPPFLAGS =	@xml2_CFLAGS@ \
		-I$(srcdir)/public \
		-I$(top_srcdir)/src \
		-I$(top_srcdir)/src/common/public \
		-I$(top_srcdir)/src/source/public \
		-I$(top_srcdir)/src/XCCDF_POLICY/public \
		-I$(top_srcdir)/src/XCCDF/public \
		-I$(top_srcdir)/src/CPE/public

AM_LDFLAGS = @xml2_LIBS@

libopenscap_sce_la_LIBADD += ../libopenscap.la

pkginclude_HEADERS = public/sce_engine_api.h

