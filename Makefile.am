SUBDIRS = src


SUBDIRS += tests utils

if WANT_BINDINGS
SUBDIRS += swig
endif

if WANT_PROBES
SUBDIRS += src/OVAL/probes
endif

EXTRA_DIST = dist/fedora/openscap.spec docs/Doxyfile docs/examples/oval_probes.py docs/examples/package-test.xml

dist-hook: ChangeLog
	cd $(distdir)/docs && doxygen Doxyfile

ChangeLog:	
	git log | sed '/^commit/d; /^Merge/d' > ChangeLog
