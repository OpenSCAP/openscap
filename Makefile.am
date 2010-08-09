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

bashcompdir = $(sysconfdir)/bash_completion.d
bashcomp_DATA = dist/bash_completion.d/oscap

nobase_pkgdata_DATA =	schemas/oval/5.5/independent-system-characteristics-schema.xsd \
			schemas/oval/5.5/windows-definitions-schema.xsd \
			schemas/oval/5.5/windows-system-characteristics-schema.xsd \
			schemas/oval/5.5/aix-definitions-schema.xsd \
			schemas/oval/5.5/linux-definitions-schema.xsd \
			schemas/oval/5.5/apache-system-characteristics-schema.xsd \
			schemas/oval/5.5/hpux-system-characteristics-schema.xsd \
			schemas/oval/5.5/pixos-system-characteristics-schema.xsd \
			schemas/oval/5.5/oval-definitions-schema.xsd \
			schemas/oval/5.5/catos-system-characteristics-schema.xsd \
			schemas/oval/5.5/oval-common-schema.xsd \
			schemas/oval/5.5/unix-definitions-schema.xsd \
			schemas/oval/5.5/ios-definitions-schema.xsd \
			schemas/oval/5.5/esx-system-characteristics-schema.xsd \
			schemas/oval/5.5/linux-system-characteristics-schema.xsd \
			schemas/oval/5.5/independent-definitions-schema.xsd \
			schemas/oval/5.5/freebsd-definitions-schema.xsd \
			schemas/oval/5.5/solaris-definitions-schema.xsd \
			schemas/oval/5.5/macos-system-characteristics-schema.xsd \
			schemas/oval/5.5/hpux-definitions-schema.xsd \
			schemas/oval/5.5/unix-system-characteristics-schema.xsd \
			schemas/oval/5.5/ios-system-characteristics-schema.xsd \
			schemas/oval/5.5/oval-results-schema.xsd \
			schemas/oval/5.5/catos-definitions-schema.xsd \
			schemas/oval/5.5/pixos-definitions-schema.xsd \
			schemas/oval/5.5/aix-system-characteristics-schema.xsd \
			schemas/oval/5.5/macos-definitions-schema.xsd \
			schemas/oval/5.5/oval-system-characteristics-schema.xsd \
			schemas/oval/5.5/solaris-system-characteristics-schema.xsd \
			schemas/oval/5.5/freebsd-system-characteristics-schema.xsd \
			schemas/oval/5.5/xmldsig-core-schema.xsd \
			schemas/oval/5.5/oval-variables-schema.xsd \
			schemas/oval/5.5/esx-definitions-schema.xsd \
			schemas/oval/5.5/apache-definitions-schema.xsd \
			schemas/xccdf/1.1.4/xccdfp-1.1.xsd \
			schemas/xccdf/1.1.4/cpe-1.0.xsd \
			schemas/xccdf/1.1.4/cpe-language_2.0.xsd \
			schemas/xccdf/1.1.4/xccdf-schema.xsd \
			schemas/xccdf/1.1.4/simpledc20021212.xsd \
			schemas/xccdf/1.1.4/platform-0.2.3.xsd \
			schemas/xccdf/1.1.4/xml.xsd \
			schemas/xccdf/1.1.4/XMLSchema.dtd \
			schemas/xccdf/1.1.4/datatypes.dtd \
			xsl/xccdf-results-report.xsl \
			xsl/oval-results-report.xsl \
			xsl/oscap-common.xsl

EXTRA_DIST =	dist/fedora/openscap.spec \
		dist/fedora/oscap-scan.sys \
		dist/fedora/oscap-scan.init \
		dist/fedora/oscap-scan.cron \
		dist/fedora/scap-fedora12-oval.xml \
		dist/fedora/scap-fedora13-oval.xml \
		dist/bash_completion.d/oscap \
		docs/Doxyfile \
		docs/examples/oval_probes.py \
		docs/examples/package-test.xml \
		schemas/oval/5.5/independent-system-characteristics-schema.xsd \
		schemas/oval/5.5/windows-definitions-schema.xsd \
		schemas/oval/5.5/windows-system-characteristics-schema.xsd \
		schemas/oval/5.5/aix-definitions-schema.xsd \
		schemas/oval/5.5/linux-definitions-schema.xsd \
		schemas/oval/5.5/apache-system-characteristics-schema.xsd \
		schemas/oval/5.5/hpux-system-characteristics-schema.xsd \
		schemas/oval/5.5/pixos-system-characteristics-schema.xsd \
		schemas/oval/5.5/oval-definitions-schema.xsd \
		schemas/oval/5.5/catos-system-characteristics-schema.xsd \
		schemas/oval/5.5/oval-common-schema.xsd \
		schemas/oval/5.5/unix-definitions-schema.xsd \
		schemas/oval/5.5/ios-definitions-schema.xsd \
		schemas/oval/5.5/esx-system-characteristics-schema.xsd \
		schemas/oval/5.5/linux-system-characteristics-schema.xsd \
		schemas/oval/5.5/independent-definitions-schema.xsd \
		schemas/oval/5.5/freebsd-definitions-schema.xsd \
		schemas/oval/5.5/solaris-definitions-schema.xsd \
		schemas/oval/5.5/macos-system-characteristics-schema.xsd \
		schemas/oval/5.5/hpux-definitions-schema.xsd \
		schemas/oval/5.5/unix-system-characteristics-schema.xsd \
		schemas/oval/5.5/ios-system-characteristics-schema.xsd \
		schemas/oval/5.5/oval-results-schema.xsd \
		schemas/oval/5.5/catos-definitions-schema.xsd \
		schemas/oval/5.5/pixos-definitions-schema.xsd \
		schemas/oval/5.5/aix-system-characteristics-schema.xsd \
		schemas/oval/5.5/macos-definitions-schema.xsd \
		schemas/oval/5.5/oval-system-characteristics-schema.xsd \
		schemas/oval/5.5/solaris-system-characteristics-schema.xsd \
		schemas/oval/5.5/freebsd-system-characteristics-schema.xsd \
		schemas/oval/5.5/xmldsig-core-schema.xsd \
		schemas/oval/5.5/oval-variables-schema.xsd \
		schemas/oval/5.5/esx-definitions-schema.xsd \
		schemas/oval/5.5/apache-definitions-schema.xsd \
		schemas/xccdf/1.1.4/xccdfp-1.1.xsd \
		schemas/xccdf/1.1.4/cpe-1.0.xsd \
		schemas/xccdf/1.1.4/cpe-language_2.0.xsd \
		schemas/xccdf/1.1.4/xccdf-schema.xsd \
		schemas/xccdf/1.1.4/simpledc20021212.xsd \
		schemas/xccdf/1.1.4/platform-0.2.3.xsd \
		schemas/xccdf/1.1.4/xml.xsd \
		schemas/xccdf/1.1.4/XMLSchema.dtd \
		schemas/xccdf/1.1.4/datatypes.dtd \
		xsl/xccdf-results-report.xsl \
		xsl/oval-results-report.xsl \
		xsl/oscap-common.xsl

dist-hook: ChangeLog
	cd $(distdir)/docs && doxygen Doxyfile

ChangeLog:	
	git log | sed '/^commit/d; /^Merge/d' > ChangeLog

CONFIG_CLEAN_FILES = config/*
clean-generic:
	rm -rf autom4te*.cache

