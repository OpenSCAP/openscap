if WANT_PROBES_INDEPENDENT
INDEPENDENT_SUBDIRS = family filehash textfilecontent54 sysinfo
endif

if WANT_PROBES_UNIX
UNIX_SUBDIRS = file uname shadow process password runlevel interface
endif

if WANT_PROBES_LINUX
LINUX_SUBDIRS = rpminfo
endif

if VALGRIND_PRESENT
VALGRIND_SUBDIRS = vgtest
endif

if ENV_PRESENT
ENV_SUBDIRS = environmentvariable 
endif

SUBDIRS = $(INDEPENDENT_SUBDIRS) $(UNIX_SUBDIRS) $(LINUX_SUBDIRS) $(VALGRIND_SUBDIRS) $(ENV_SUBDIRS)

