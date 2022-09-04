# This spec file is not synchronized to the Fedora downstream.
# It serves as Fedora CI configuration and as support for downstream updates.
Name:           openscap
Version:        1.3.6
Release:        0%{?dist}
Epoch:          1
Summary:        Set of open source libraries enabling integration of the SCAP line of standards
License:        LGPLv2+
URL:            http://www.open-scap.org/
Source0:        https://github.com/OpenSCAP/%{name}/releases/download/%{version}/%{name}-%{version}.tar.gz
Source1:        https://github.com/OpenSCAP/yaml-filter/archive/refs/tags/v0.2.0.tar.gz
BuildRequires:  make
BuildRequires:  cmake >= 2.8
BuildRequires:  ninja-build
BuildRequires:  pkg-config
BuildRequires:  systemd-rpm-macros
BuildRequires:  gcc
BuildRequires:  gcc-c++
BuildRequires:  swig libxml2-devel libxslt-devel perl-generators perl-XML-Parser
BuildRequires:  rpm-devel
BuildRequires:  libgcrypt-devel
BuildRequires:  pcre-devel
BuildRequires:  libacl-devel
BuildRequires:  libselinux-devel
BuildRequires:  libcap-devel
BuildRequires:  libblkid-devel
BuildRequires:  bzip2-devel
BuildRequires:  asciidoc
BuildRequires:  openldap-devel
BuildRequires:  glib2-devel
BuildRequires:  dbus-devel
BuildRequires:  libyaml-devel
BuildRequires:  xmlsec1-devel xmlsec1-openssl-devel
BuildRequires:  systemd
%if %{with check}
BuildRequires:  bzip2
BuildRequires:  chkconfig
BuildRequires:  diffutils
BuildRequires:  findutils
BuildRequires:  gawk
BuildRequires:  hostname
BuildRequires:  ipcalc
BuildRequires:  iproute
BuildRequires:  libselinux-utils
BuildRequires:  lua
BuildRequires:  openssl
BuildRequires:  perl(XML::Parser)
BuildRequires:  perl(XML::XPath)
BuildRequires:  procps
BuildRequires:  rpm-build
BuildRequires:  tar
BuildRequires:  tcpdump
%endif
Requires:       bash
Requires:       bzip2-libs
Requires:       dbus
Requires:       glib2
Requires:       libacl
Requires:       libblkid
Requires:       libcap
Requires:       libselinux
Requires:       openldap
Requires:       popt
# Fedora has procps-ng, which provides procps
Requires:       procps
Requires:       xmlsec1 xmlsec1-openssl

%description
OpenSCAP is a set of open source libraries providing an easier path
for integration of the SCAP line of standards. SCAP is a line of standards
managed by NIST with the goal of providing a standard language
for the expression of Computer Network Defense related information.

%package        devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{epoch}:%{version}-%{release}
Requires:       libxml2-devel
Requires:       pkgconfig
BuildRequires:  doxygen

%description    devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.

%package        python3
Summary:        Python 3 bindings for %{name}
Requires:       %{name}%{?_isa} = %{epoch}:%{version}-%{release}
BuildRequires:  python3-devel

%description    python3
The %{name}-python3 package contains the bindings so that %{name}
libraries can be used by python3.

%package        scanner
Summary:        OpenSCAP Scanner Tool (oscap)
Requires:       %{name}%{?_isa} = %{epoch}:%{version}-%{release}
Requires:       libcurl >= 7.12.0
BuildRequires:  libcurl-devel >= 7.12.0

%description    scanner
The %{name}-scanner package contains oscap command-line tool. The oscap
is configuration and vulnerability scanner, capable of performing
compliance checking using SCAP content.

%package        utils
Summary:        OpenSCAP Utilities
Requires:       %{name}%{?_isa} = %{epoch}:%{version}-%{release}
Requires:       rpmdevtools rpm-build
Requires:       %{name}-scanner%{?_isa} = %{epoch}:%{version}-%{release}

%description    utils
The %{name}-utils package contains command-line tools build on top
of OpenSCAP library. Historically, openscap-utils included oscap
tool which is now separated to %{name}-scanner sub-package.

%package        engine-sce
Summary:        Script Check Engine plug-in for OpenSCAP
Requires:       %{name}%{?_isa} = %{epoch}:%{version}-%{release}

%description    engine-sce
The Script Check Engine is non-standard extension to SCAP protocol. This
engine allows content authors to avoid OVAL language and write their assessment
commands using a scripting language (Bash, Perl, Python, Ruby, ...).

%package        engine-sce-devel
Summary:        Development files for %{name}-engine-sce
Requires:       %{name}-devel%{?_isa} = %{epoch}:%{version}-%{release}
Requires:       %{name}-engine-sce%{?_isa} = %{epoch}:%{version}-%{release}
Requires:       pkgconfig

%description    engine-sce-devel
The %{name}-engine-sce-devel package contains libraries and header files
for developing applications that use %{name}-engine-sce.

%package        containers
Summary:        Utils for scanning containers
Requires:       %{name} = %{epoch}:%{version}-%{release}
Requires:       %{name}-scanner%{?_isa} = %{epoch}:%{version}-%{release}
BuildArch:      noarch

%description    containers
Tool for scanning Atomic containers.

%prep
%autosetup -p1
tar xvzf %{SOURCE1} --directory=yaml-filter --strip-components=1

%build
%undefine __cmake_in_source_build
# gconf is a legacy system not used any more, and it blocks testing of oscap-anaconda-addon
# as gconf is no longer part of the installation medium
%cmake -G Ninja \
    -DENABLE_PERL=OFF \
    -DENABLE_DOCS=ON \
    -DOPENSCAP_PROBE_UNIX_GCONF=OFF \
    -DGCONF_LIBRARY=

%cmake_build
make docs

%check
%if %{with check}
# Tests use common files. Running tests parallel causes failed tests because of that.
%undefine _smp_mflags
%ctest
%endif

%install
%cmake_install

%if "0%{?!fedora:1}"
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'

# fix python shebangs
pathfix.py -i %{__python3} -p -n $RPM_BUILD_ROOT%{_bindir}/scap-as-rpm

%ldconfig_scriptlets

# enable oscap-remediate.service here for now
# https://github.com/hughsie/PackageKit/issues/401
# https://bugzilla.redhat.com/show_bug.cgi?id=1833176
mkdir -p %{buildroot}%{_unitdir}/system-update.target.wants/
ln -sf ../oscap-remediate.service %{buildroot}%{_unitdir}/system-update.target.wants/oscap-remediate.service
%endif

%files
%doc AUTHORS NEWS README.md
%license COPYING
%doc %{_pkgdocdir}/manual/
%dir %{_datadir}/openscap
%dir %{_datadir}/openscap/schemas
%dir %{_datadir}/openscap/xsl
%dir %{_datadir}/openscap/cpe
%{_libdir}/libopenscap.so.*
%{_datadir}/openscap/schemas/*
%{_datadir}/openscap/xsl/*
%{_datadir}/openscap/cpe/*

%files python3
%{python3_sitearch}/*

%files devel
%doc %{_pkgdocdir}/html/
%{_libdir}/libopenscap.so
%{_libdir}/pkgconfig/*.pc
%{_includedir}/openscap
%exclude %{_includedir}/openscap/sce_engine_api.h

%files scanner
%{_bindir}/oscap
%{_bindir}/oscap-chroot
%{_mandir}/man8/oscap.8*
%{_mandir}/man8/oscap-chroot.8*
%{_sysconfdir}/bash_completion.d
%{_libexecdir}/oscap-remediate
%{_unitdir}/oscap-remediate.service
%if "0%{?!fedora:1}"
%{_unitdir}/system-update.target.wants/
%endif

%files utils
%doc docs/oscap-scan.cron
%{_bindir}/autotailor
%{_bindir}/oscap-remediate-offline
%{_bindir}/oscap-ssh
%{_bindir}/oscap-vm
%{_bindir}/scap-as-rpm
%{_mandir}/man8/autotailor.8*
%{_mandir}/man8/oscap-remediate-offline.8*
%{_mandir}/man8/oscap-ssh.8*
%{_mandir}/man8/oscap-vm.8*
%{_mandir}/man8/scap-as-rpm.8*

%files engine-sce
%{_bindir}/oscap-run-sce-script
%{_libdir}/libopenscap_sce.so.*

%files engine-sce-devel
%{_libdir}/libopenscap_sce.so
%{_includedir}/openscap/sce_engine_api.h

%files containers
%{_bindir}/oscap-docker
%{_mandir}/man8/oscap-docker.8*
%{_bindir}/oscap-podman
%{_mandir}/man8/oscap-podman.8*
%{python3_sitelib}/oscap_docker_python/*

%changelog
%autochangelog
