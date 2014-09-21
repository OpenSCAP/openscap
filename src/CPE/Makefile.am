noinst_LTLIBRARIES = libcpe.la

libcpe_la_SOURCES =	cpedict.c \
			cpelang.c \
			cpename.c \
			cpe_ctx_priv.c \
			cpe_ctx_priv.h \
			cpelang_priv.c \
			cpedict_priv.c \
			cpedict_ext_priv.c \
			cpelang_priv.h \
			cpedict_ext_priv.h \
			cpedict_priv.h \
			cpe_session_priv.h

libcpe_la_CPPFLAGS = @xml2_CFLAGS@ \
			@pcre_CFLAGS@ \
			-I$(srcdir)/public \
			-I$(top_srcdir)/src/common/public \
			-I$(top_srcdir)/src/source/public \
			-I$(top_srcdir)/src

libcpe_la_LDFLAGS =	@xml2_LIBS@ \
			@pcre_LIBS@

pkginclude_HEADERS = public/cpe_dict.h \
			public/cpe_lang.h \
			public/cpe_name.h
