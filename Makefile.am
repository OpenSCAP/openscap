ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

if WANT_PROBES
SUBDIRS += src/OVAL/probes
endif

if WANT_BINDINGS
SUBDIRS += swig
endif

SUBDIRS += utils tests

dist_pkgdata_DATA = dist/fedora/scap-fedora12-oval.xml \
                    dist/fedora/scap-fedora13-oval.xml \
                    dist/fedora/oscap-scan.cron

EXTRA_DIST =	dist/fedora/openscap.spec \
		dist/fedora/oscap-scan.sys \
		dist/fedora/oscap-scan.init \
		dist/fedora/oscap-scan.cron \
		dist/fedora/scap-fedora12-oval.xml \
		dist/fedora/scap-fedora13-oval.xml \
		docs/Doxyfile \
		docs/examples/oval_probes.py \
		docs/examples/package-test.xml

dist-hook: ChangeLog
	cd $(distdir)/docs && doxygen Doxyfile

ChangeLog:	
	git log | sed '/^commit/d; /^Merge/d' > ChangeLog

CONFIG_CLEAN_FILES = config/*
clean-generic:
	rm -rf autom4te*.cache

