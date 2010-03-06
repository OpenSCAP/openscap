
SUBDIRS = src

if WANT_PROBES
SUBDIRS += src/OVAL/probes
endif

if WANT_BINDINGS
SUBDIRS += swig
endif

SUBDIRS += utils tests

EXTRA_DIST = dist/fedora/openscap.spec docs/Doxyfile docs/examples/oval_probes.py docs/examples/package-test.xml

dist-hook: ChangeLog
	cd $(distdir)/docs && doxygen Doxyfile

ChangeLog:	
	git log | sed '/^commit/d; /^Merge/d' > ChangeLog

CONFIG_CLEAN_FILES = config/*
clean-generic:
	rm -rf autom4te*.cache
