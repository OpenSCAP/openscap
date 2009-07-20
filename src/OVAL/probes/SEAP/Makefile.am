AUTOMAKE_OPTIONS= subdir-objects nostdinc
noinst_LTLIBRARIES= libseap.la

libseap_la_SOURCES= generic/bfind.c		\
		    generic/bfind.h		\
		    generic/bitmap.c 		\
		    generic/bitmap.h		\
		    generic/common.c		\
		    generic/common.h		\
		    generic/redblack.h		\
		    generic/strto.c		\
		    generic/strto.h		\
		    generic/xbase64.c		\
		    generic/xbase64.h		\
		    sch_cons.c			\
		    sch_cons.h			\
		    sch_dummy.c			\
		    sch_dummy.h			\
		    sch_generic.c		\
		    sch_generic.h		\
		    sch_pipe.c			\
		    sch_pipe.h			\
		    seap-command-backendL.c	\
		    seap-command-backendL.h	\
		    seap-command-backendS.c	\
		    seap-command-backendS.h	\
		    seap-command.c		\
		    _seap-command.h		\
		    seap-descriptor.c		\
		    seap-descriptor.h		\
		    seap-scheme.c		\
		    seap-scheme.h		\
		    _seap-types.h		\
		    seap.c			\
		    _seap.h			\
		    sexp-handler.c		\
		    sexp-handler.h		\
		    sexp-manip.c		\
		    _sexp-manip.h		\
		    sexp-output.c		\
		    _sexp-output.h		\
		    sexp-parse.c		\
		    _sexp-parse.h		\
		    _sexp-types.h		\
		    sm_alloc.c			\
		    seap-debug.c

libseap_la_CFLAGS= -DUSE_XMALLOC -DXMALLOC_EXIT -DWANT_BASE64 -I$(top_srcdir) -I. -I public/
libseap_la_LDFLAGS=

include_HEADERS= public/seap-command.h	\
		 public/seap-debug.h	\
		 public/seap-types.h	\
		 public/seap.h		\
		 public/sexp-manip.h	\
		 public/sexp-output.h	\
		 public/sexp-parse.h	\
		 public/sexp-types.h	\
		 public/sexp.h		\
		 public/sm_alloc.h

includedir = $(prefix)/include/lib@PACKAGE@

