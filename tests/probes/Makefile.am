if WANT_PROBES_INDEPENDENT
INDEPENDENT_SUBDIRS = family filehash filehash58 textfilecontent54 sysinfo
endif

if WANT_PROBES_UNIX
UNIX_SUBDIRS = file uname shadow process password runlevel interface xinetd fileextendedattribute
endif

if WANT_PROBES_LINUX
LINUX_SUBDIRS = rpminfo iflisteners

if ENABLE_PROBES_SELINUX
LINUX_SUBDIRS += selinuxboolean
endif

endif

if WANT_PROBES_SOLARIS
SOLARIS_SUBDIRS = isainfo
endif

if ENABLE_VALGRIND_TESTS
VALGRIND_SUBDIRS = vgtest
endif

if ENV_PRESENT
ENV_SUBDIRS = environmentvariable environmentvariable58
endif

SUBDIRS = $(INDEPENDENT_SUBDIRS) $(UNIX_SUBDIRS) $(LINUX_SUBDIRS) $(SOLARIS_SUBDIRS) $(VALGRIND_SUBDIRS) $(ENV_SUBDIRS)
