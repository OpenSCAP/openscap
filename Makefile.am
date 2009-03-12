SUBDIRS = src swig
#examples

EXTRA_DIST = dist/fedora/openscap.spec docs/Doxyfile

dist-hook:
	cd $(distdir)/docs && doxygen Doxyfile
