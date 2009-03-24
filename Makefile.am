SUBDIRS = src swig examples
#examples
#swig

EXTRA_DIST = dist/fedora/openscap.spec docs/Doxyfile

dist-hook:
	cd $(distdir)/docs && doxygen Doxyfile
	git-log | sed '/^commit/d; /^Merge/d' > ChangeLog
	
