lib_LTLIBRARIES= libseap.la

libseap_la_SOURCES= bitmap.c \
		    bitmap.h \
		    common.c \
		    common.h \
		    seap.c \
		    xbase64.c \
		    xbase64.h \
		    xmalloc.c \
		    xmalloc.h \
		    scheme.h \
		    sch_dummy.c \
		    sch_dummy.h \
		    sch_cons.c \
		    sch_cons.h \
		    sch_pipe.c \
		    sch_pipe.h \
		    sch_generic.c \
		    sch_generic.h \
		    sexp-printf.c \
		    sexp-printf.h \
		    sexp-output.c \
		    sexp-output.h \
		    sexp-handler.c \
		    sexp-handler.h \
		    redblack.h

libseap_la_CFLAGS= -DUSE_XMALLOC -DXMALLOC_EXIT -DWANT_BASE64
libseap_la_LDFLAGS=

include_HEADERS= seap.h
includedir= $(prefix)/include
