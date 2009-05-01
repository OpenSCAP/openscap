SUBDIRS = src

if WANT_PROBES
SUBDIRS += src/OVAL/probes
endif

SUBDIRS += examples

if WANT_BINDINGS
SUBDIRS += swig
endif

EXTRA_DIST = dist/fedora/openscap.spec docs/Doxyfile

dist-hook: ChangeLog
	cd $(distdir)/docs && doxygen Doxyfile

ChangeLog:	
	git log | sed '/^commit/d; /^Merge/d' > ChangeLog
