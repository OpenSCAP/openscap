noinst_LTLIBRARIES= libprobe.la

libprobe_la_CFLAGS=	@pthread_CFLAGS@				\
			@xml2_CFLAGS@					\
			@pcre_CFLAGS@					\
			-I$(top_srcdir)/lib				\
			-I$(top_srcdir)/src				\
			-I$(top_srcdir)/src/common			\
			-I$(top_srcdir)/src/common/public		\
			-I$(top_srcdir)/src/source/public		\
			-I$(top_srcdir)/src/OVAL/public			\
			-I$(top_srcdir)/src/OVAL/probes/public		\
			-I$(top_srcdir)/src/OVAL/probes/SEAP/public	\
			-DTHREAD_SAFE					\
			-DSEAP_THREAD_SAFE

libprobe_la_SOURCES=	\
			fini.c		\
			preload.c		\
			init.c			\
			main.c			\
			input_handler.c		\
			input_handler.h		\
			worker.c		\
			worker.h		\
			signal_handler.c	\
			signal_handler.h	\
			probe.h			\
			probe.c			\
			entcmp.c		\
			entcmp.h		\
			icache.c		\
			icache.h		\
			option.c		\
			option.h

libprobe_la_LIBADD= \
			$(top_builddir)/src/libopenscap.la	\
			$(top_builddir)/src/common/liboscapcommon.la \
			$(top_builddir)/src/OVAL/results/libovalcmp.la \
			@pthread_LIBS@
