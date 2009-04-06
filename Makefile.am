SUBDIRS = src swig examples
#examples
#swig

EXTRA_DIST = dist/fedora/openscap.spec docs/Doxyfile

dist-hook: ChangeLog
	cd $(distdir)/docs && doxygen Doxyfile

ChangeLog:	
	git-log | sed '/^commit/d; /^Merge/d' > ChangeLog
