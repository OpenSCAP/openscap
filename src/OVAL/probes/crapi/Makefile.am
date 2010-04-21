noinst_LTLIBRARIES= libcrapi.la
libcrapi_la_SOURCES= 		\
		digest.c	\
		digest.h	\
		md5.c		\
		md5.h		\
		sha1.c		\
		sha1.h		\
		sha2.c		\
		sha2.h		\
		rmd160.c	\
		rmd160.h	\
		crapi.h		\
		crapi.c

libcrapi_la_LDFLAGS= @crapi_libs@
libcrapi_la_CFLAGS= @crapi_cflags@ -I. -I$(top_srcdir) -I$(top_srcdir)/src/common -I$(top_srcdir)/src/common/public -D_FILE_OFFSET_BITS=32
