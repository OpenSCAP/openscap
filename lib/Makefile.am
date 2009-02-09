
AM_CFLAGS = -fPIC

lib_LTLIBRARIES = libcvsscalc.la \
                  libcve.la \
                  libcpe.la

include_HEADERS = libcvsscalc.h \
                  libcve.h \
                  libcpeuri.h \
                  libcpematch.h \
                  libcpedict.h \
                  libcpelang.h 

libcvsscalc_la_SOURCES = libcvsscalc.c
libcvsscalc_la_LIBADD = -lm

libcve_la_SOURCES = libcve.c
libcve_la_CPPFLAGS = -I$(top_srcdir) $(libxml_cflags)
libcve_la_LIBADD = $(libxml_libs)

libcpe_la_SOURCES = cpeuri.c cpematch.c cpedict.c cpelang.c
libcpe_la_CPPFLAGS = -I$(top_srcdir) $(libxml_cflags)  $(pcre_cflags)
libcpe_la_LIBADD = $(libxml_libs) $(pcre_libs)

