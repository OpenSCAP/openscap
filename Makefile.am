ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

if WANT_PROBES
SUBDIRS += src/OVAL/probes
endif

if WANT_BINDINGS
SUBDIRS += swig
endif

SUBDIRS += utils tests xsl schemas

dist_pkgdata_DATA = dist/fedora/scap-fedora12-oval.xml \
                    dist/fedora/scap-fedora13-oval.xml \
                    dist/fedora/scap-fedora14-oval.xml \
                    dist/fedora/scap-fedora14-xccdf.xml

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libopenscap.pc

bashcompdir = $(sysconfdir)/bash_completion.d
bashcomp_DATA = dist/bash_completion.d/oscap


EXTRA_DIST =	dist/rhel5/openscap.spec \
		dist/rhel5/openscap-0.6.3-noValidate.patch \
		dist/fedora/openscap.spec \
		dist/fedora/oscap-scan.sys \
		dist/fedora/oscap-scan.init \
		dist/fedora/scap-fedora12-oval.xml \
		dist/fedora/scap-fedora13-oval.xml \
		dist/fedora/scap-fedora14-oval.xml \
		dist/fedora/scap-fedora14-xccdf.xml \
		dist/bash_completion.d/oscap \
		docs/Doxyfile \
		docs/title.doc \
		docs/modules \
		docs/examples/oval_probes.py \
		docs/examples/package-test.xml \
                docs/oscap-scan.cron

dist-hook: ChangeLog
	cd $(distdir)/docs && doxygen Doxyfile

ChangeLog:	
	git log | sed '/^commit/d; /^Merge/d' > ChangeLog

CONFIG_CLEAN_FILES = config/*
clean-generic:
	rm -rf autom4te*.cache libopenscap.pc

