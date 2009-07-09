noinst_LTLIBRARIES= libseap.la

libseap_la_SOURCES= bitmap.c \
                    bitmap.h \
                    common.c \
                    common.h \
                    redblack.h \
                    sch_cons.c \
                    sch_cons.h \
                    sch_dummy.c \
                    sch_dummy.h \
                    sch_generic.c \
                    sch_generic.h \
                    sch_pipe.c \
                    sch_pipe.h \
                    seap-scheme.c \
                    seap-scheme.h \
                    seap.c \
                    sexp-handler.c \
                    sexp-manip.c \
                    sexp-output.c \
                    sexp-parse.c \
		    sexp-command.c \
                    xbase64.c \
                    xbase64.h \
                    xmalloc.c

libseap_la_CFLAGS= -DUSE_XMALLOC -DXMALLOC_EXIT -DWANT_BASE64
libseap_la_LDFLAGS=

include_HEADERS= seap.h \
                 seap-types.h \
                 sexp.h \
                 sexp-types.h \
                 sexp-output.h \
                 sexp-handler.h \
                 sexp-parse.h \
                 sexp-manip.h \
		 sexp-command.h \
                 xmalloc.h

includedir = $(prefix)/include/lib@PACKAGE@

