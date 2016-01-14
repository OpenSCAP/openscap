
noinst_LTLIBRARIES = libds.la

libds_la_SOURCES = ds_common.c \
		   ds_common.h \
		   ds_rds_session.c \
		   ds_rds_session_priv.h \
		   ds_sds_session.c \
		   ds_sds_session_priv.h \
		   sds.c \
		   sds_priv.h \
		   sds_index.c \
		   sds_index_priv.h \
		   rds.c \
		rds_asset_index.c \
		   rds_priv.h \
		   rds_index.c \
		   rds_index_priv.h

libds_la_CPPFLAGS  = @xml2_CFLAGS@ \
		     -I$(top_srcdir)/src \
		     -I$(top_srcdir)/src/DS/public \
		     -I$(top_srcdir)/src/DS \
		     -I$(top_srcdir)/src/XCCDF \
		     -I$(top_srcdir)/src/OVAL \
		     -I$(top_srcdir)/src/CPE/public \
		     -I$(top_srcdir)/src/common \
		     -I$(top_srcdir)/src/common/public \
		     -I$(top_srcdir)/src/source/public \
		     -I$(top_srcdir)/lib

libds_la_LDFLAGS = @xml2_LIBS@

pkginclude_HEADERS = \
	public/ds_rds_session.h \
	public/ds_sds_session.h \
	public/scap_ds.h

