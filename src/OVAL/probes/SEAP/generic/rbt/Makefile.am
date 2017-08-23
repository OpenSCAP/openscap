AUTOMAKE_OPTIONS= subdir-objects
noinst_LTLIBRARIES= librbt.la

librbt_la_SOURCES=	rbt_common.c	\
			rbt_common.h	\
			rbt.h		\
			rbt_i32.c	\
			rbt_i32.h	\
			rbt_i64.c	\
			rbt_i64.h	\
			rbt_str.c	\
			rbt_str.h

librbt_la_CFLAGS=	-DRBT_IMPLICIT_LOCKING @pthread_CFLAGS@ \
			-I.					\
			-I$(top_srcdir) 			\
			-I$(top_srcdir)/src			\
			-I$(top_srcdir)/src/common		\
			-I$(top_srcdir)/src/OVAL/probes/SEAP	\
			-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
			-Wno-strict-aliasing

librbt_la_LDFLAGS= @pthread_LIBS@
