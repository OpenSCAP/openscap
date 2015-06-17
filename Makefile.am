ACLOCAL_AMFLAGS = -I m4

SUBDIRS = lib src src/OVAL/probes utils tests xsl schemas cpe

dist_pkgdata_DATA = dist/fedora/scap-fedora14-oval.xml \
                    dist/fedora/scap-fedora14-xccdf.xml \
                    dist/rhel6/scap-rhel6-oval.xml \
                    dist/rhel6/scap-rhel6-xccdf.xml

if WANT_PYTHON
  SUBDIRS += swig
else
if WANT_PERL
    SUBDIRS += swig
endif
endif

if WANT_SCE
SUBDIRS += src/SCE

sectoolscedir= $(pkgdatadir)/sectool-sce

sectoolsce_SCRIPTS = dist/fedora/sectool-xccdf/00_integrity.sh \
                     dist/fedora/sectool-xccdf/01_bootloader.sh \
                     dist/fedora/sectool-xccdf/01_disk_usage.sh \
                     dist/fedora/sectool-xccdf/01_group.sh \
                     dist/fedora/sectool-xccdf/01_passwd.sh \
                     dist/fedora/sectool-xccdf/01_shadow.sh \
                     dist/fedora/sectool-xccdf/02_home_files.sh \
                     dist/fedora/sectool-xccdf/02_root_dirs.sh \
                     dist/fedora/sectool-xccdf/03_path.sh \
                     dist/fedora/sectool-xccdf/04_firewall.sh \
                     dist/fedora/sectool-xccdf/05_netserv.sh \
                     dist/fedora/sectool-xccdf/05_openssh.sh \
                     dist/fedora/sectool-xccdf/05_openvpn.sh \
                     dist/fedora/sectool-xccdf/05_removed_libs.sh \
                     dist/fedora/sectool-xccdf/05_xinetd.sh \
                     dist/fedora/sectool-xccdf/07_log_files.sh \
                     dist/fedora/sectool-xccdf/08_pam.sh \
                     dist/fedora/sectool-xccdf/08_permissions.sh \
                     dist/fedora/sectool-xccdf/09_exec_shield.sh \
                     dist/fedora/sectool-xccdf/09_selinux.sh \
                     dist/fedora/sectool-xccdf/09_va_randomization.sh \
                     dist/fedora/sectool-xccdf/11_aliases.sh \
                     dist/fedora/sectool-xccdf/12_cron.sh \
                     dist/fedora/sectool-xccdf/14_nfs.sh \
                     dist/fedora/sectool-xccdf/15_tcp_wrappers.sh

sectoolsce_DATA =    dist/fedora/sectool-xccdf/sectool-xccdf.xml
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libopenscap.pc

bashcompdir = $(sysconfdir)/bash_completion.d
bashcomp_DATA = dist/bash_completion.d/oscap


EXTRA_DIST =	m4/gnulib-cache.m4 \
		dist/rhel5/openscap.spec \
		dist/fedora/openscap.spec \
		dist/fedora/oscap-scan.sys \
		dist/fedora/oscap-scan.init \
		dist/fedora/scap-fedora14-oval.xml \
		dist/fedora/scap-fedora14-xccdf.xml \
		dist/rhel6/scap-rhel6-oval.xml \
		dist/rhel6/scap-rhel6-xccdf.xml \
		dist/rhel6/openscap.spec \
		dist/bash_completion.d/oscap \
		docs/Doxyfile \
		docs/title.doc \
		docs/modules \
		docs/examples/oval_probes.py \
		docs/examples/package-test.xml \
                docs/oscap-scan.cron \
		dist/fedora/sectool-xccdf/00_integrity.sh \
		dist/fedora/sectool-xccdf/01_bootloader.sh \
		dist/fedora/sectool-xccdf/01_disk_usage.sh \
		dist/fedora/sectool-xccdf/01_group.sh \
		dist/fedora/sectool-xccdf/01_passwd.sh \
		dist/fedora/sectool-xccdf/01_shadow.sh \
		dist/fedora/sectool-xccdf/02_home_files.sh \
		dist/fedora/sectool-xccdf/02_root_dirs.sh \
		dist/fedora/sectool-xccdf/03_path.sh \
		dist/fedora/sectool-xccdf/04_firewall.sh \
		dist/fedora/sectool-xccdf/05_netserv.sh \
		dist/fedora/sectool-xccdf/05_openssh.sh \
		dist/fedora/sectool-xccdf/05_openvpn.sh \
		dist/fedora/sectool-xccdf/05_removed_libs.sh \
		dist/fedora/sectool-xccdf/05_xinetd.sh \
		dist/fedora/sectool-xccdf/07_log_files.sh \
		dist/fedora/sectool-xccdf/08_pam.sh \
		dist/fedora/sectool-xccdf/08_permissions.sh \
		dist/fedora/sectool-xccdf/09_exec_shield.sh \
		dist/fedora/sectool-xccdf/09_selinux.sh \
		dist/fedora/sectool-xccdf/09_va_randomization.sh \
		dist/fedora/sectool-xccdf/11_aliases.sh \
		dist/fedora/sectool-xccdf/12_cron.sh \
		dist/fedora/sectool-xccdf/14_nfs.sh \
		dist/fedora/sectool-xccdf/15_tcp_wrappers.sh \
		dist/fedora/sectool-xccdf/sectool-xccdf.xml

dist-hook: ChangeLog
	cd $(distdir)/docs && doxygen Doxyfile

ChangeLog:
	git log | sed '/^commit/d; /^Merge/d' > ChangeLog

CONFIG_CLEAN_FILES = config/* run

clean-generic:
	rm -rf autom4te*.cache libopenscap.pc
