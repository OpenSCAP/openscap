%{!?python_sitelib: %global python_sitelib %(%{__python} -c "from distutils.sysconfig import get_python_lib; print get_python_lib()")}
%{!?python_sitearch: %global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print get_python_lib(1)")}

%define relabel_files() \
restorecon -R /usr/bin/oscap /usr/libexec/openscap; \

Name:           openscap
Version:        0.9.13
Release:        1%{?dist}
Summary:        Set of open source libraries enabling integration of the SCAP line of standards
Group:          System Environment/Libraries
License:        LGPLv2+
URL:            http://www.open-scap.org/
Source0:        http://fedorahosted.org/releases/o/p/openscap/%{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  swig libxml2-devel libxslt-devel perl-XML-Parser
BuildRequires:  rpm-devel
BuildRequires:  libgcrypt-devel
BuildRequires:  pcre-devel
BuildRequires:  libacl-devel
BuildRequires:  libselinux-devel libcap-devel
BuildRequires:  libblkid-devel
%if %{?_with_check:1}%{!?_with_check:0}
BuildRequires:  perl-XML-XPath
%endif
Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
OpenSCAP is a set of open source libraries providing an easier path
for integration of the SCAP line of standards. SCAP is a line of standards
managed by NIST with the goal of providing a standard language
for the expression of Computer Network Defense related information.

%package        devel
Summary:        Development files for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}
Requires:       libxml2-devel
Requires:       pkgconfig

%description    devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.

%package        python
Summary:        Python bindings for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}
BuildRequires:  python-devel

%description    python
The %{name}-python package contains the bindings so that %{name}
libraries can be used by python.

%package        perl
Summary:        Perl bindings for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}
Requires:       perl(:MODULE_COMPAT_%(eval "`%{__perl} -V:version`"; echo $version))
BuildRequires:  perl-devel

%description    perl
The %{name}-perl package contains the bindings so that %{name}
libraries can be used by perl.


%package        utils
Summary:        Openscap utilities
Group:          Applications/System
Requires:       %{name} = %{version}-%{release}
Requires:       libcurl >= 7.12.0
Requires:       rpmdev-setuptree rpmbuild
BuildRequires:  libcurl-devel >= 7.12.0

%description    utils
The %{name}-utils package contains oscap command-line tool. The oscap
is configuration and vulnerability scanner, capable of performing
compliance checking using SCAP content.

%package        content
Summary:        SCAP content
Group:          Applications/System
Requires:       %{name} = %{version}-%{release}
BuildArch:      noarch

%description    content
Example of SCAP content for Fedora. Please note that this content
is for testing purposes only.

%package        content-sectool
Summary:        Sectool content
Group:          Applications/System
Requires:       %{name} = %{version}-%{release}
Requires:       %{name}-engine-sce
BuildArch:      noarch

%description    content-sectool
SCAP/SCE content that conforms to sectool checks.

%package        extra-probes
Summary:        SCAP probes
Group:          Applications/System
Requires:       %{name} = %{version}-%{release}
BuildRequires:  openldap-devel
BuildRequires:  GConf2-devel

%description    extra-probes
The %{name}-extra-probes package contains additional probes that are not
commonly used and require additional dependencies.

%package        extra-probes-sql
Summary:        SCAP probes for Database
Group:          Applications/System
Requires:       %{name} = %{version}-%{release}
BuildRequires:  opendbx-devel

%description    extra-probes-sql
The %{name}-extra-probes-sql package contains additional OpenSCAP probes
for querying database objects. Users are advised to install appropriate
opendbx backend package along this one.

%package        engine-sce
Summary:	Script Check Engine plug-in for OpenSCAP
Group:          Applications/System
Requires:       %{name} = %{version}-%{release}

%description    engine-sce
The Script Check Engine is non-standard extension to SCAP protocol. This
engine allows content authors to avoid OVAL language and write their assessment
commands using a scripting language (Bash, Perl, Python, Ruby, ...).

%package        selinux
Summary:        SELinux policy module for openscap
Group:          System Environment/Base
Requires:       %{name}-utils = %{version}-%{release}
Requires:       policycoreutils, libselinux-utils
Requires(post): selinux-policy-base, policycoreutils
Requires(postun): policycoreutils
BuildRequires:  selinux-policy-devel
BuildArch:      noarch

%description    selinux
This package installs and sets up the  SELinux policy security module for openscap.

%prep
%setup -q

%build
%ifarch sparc64
#sparc64 need big PIE
export CFLAGS="$RPM_OPT_FLAGS -fPIE"
export LDFLAGS="-pie -Wl,-z,relro -Wl,-z,now"
%else
export CFLAGS="$RPM_OPT_FLAGS -fpie"
export LDFLAGS="-pie -Wl,-z,relro -Wl,-z,now"
%endif

%configure --enable-sce --enable-perl --enable-selinux_policy

make %{?_smp_mflags}
# Remove shebang from bash-completion script
sed -i '/^#!.*bin/,+1 d' dist/bash_completion.d/oscap

%check
#to run make check use "--with check"
%if %{?_with_check:1}%{!?_with_check:0}
make check
%endif

%install
rm -rf $RPM_BUILD_ROOT

make install INSTALL='install -p' DESTDIR=$RPM_BUILD_ROOT

# create symlinks to default content
ln -s  %{_datadir}/openscap/scap-fedora14-oval.xml $RPM_BUILD_ROOT/%{_datadir}/openscap/scap-oval.xml
ln -s  %{_datadir}/openscap/scap-fedora14-xccdf.xml $RPM_BUILD_ROOT/%{_datadir}/openscap/scap-xccdf.xml

# remove content for another OS
rm $RPM_BUILD_ROOT/%{_datadir}/openscap/scap-rhel6-oval.xml
rm $RPM_BUILD_ROOT/%{_datadir}/openscap/scap-rhel6-xccdf.xml

# bash-completion script
mkdir -p $RPM_BUILD_ROOT/%{_sysconfdir}/bash_completion.d
install -pm 644 dist/bash_completion.d/oscap $RPM_BUILD_ROOT%{_sysconfdir}/bash_completion.d/oscap

find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%post selinux
semodule -n -i %{_datadir}/selinux/packages/oscap.pp
if /usr/sbin/selinuxenabled ; then
    /usr/sbin/load_policy
    %relabel_files
fi;
exit 0

%postun -p /sbin/ldconfig

%postun selinux
if [ $1 -eq 0 ]; then
    semodule -n -r oscap
    if /usr/sbin/selinuxenabled ; then
       /usr/sbin/load_policy
       %relabel_files
    fi;
fi;
exit 0

%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING ChangeLog NEWS README
%{_libdir}/libopenscap.so.*
%{_libexecdir}/openscap/probe_dnscache
%{_libexecdir}/openscap/probe_environmentvariable
%{_libexecdir}/openscap/probe_environmentvariable58
%{_libexecdir}/openscap/probe_family
%{_libexecdir}/openscap/probe_file
%{_libexecdir}/openscap/probe_fileextendedattribute
%{_libexecdir}/openscap/probe_filehash
%{_libexecdir}/openscap/probe_filehash58
%{_libexecdir}/openscap/probe_iflisteners
%{_libexecdir}/openscap/probe_inetlisteningservers
%{_libexecdir}/openscap/probe_interface
%{_libexecdir}/openscap/probe_partition
%{_libexecdir}/openscap/probe_password
%{_libexecdir}/openscap/probe_process
%{_libexecdir}/openscap/probe_process58
%{_libexecdir}/openscap/probe_routingtable
%{_libexecdir}/openscap/probe_rpminfo
%{_libexecdir}/openscap/probe_rpmverify
%{_libexecdir}/openscap/probe_rpmverifyfile
%{_libexecdir}/openscap/probe_rpmverifypackage
%{_libexecdir}/openscap/probe_runlevel
%{_libexecdir}/openscap/probe_selinuxboolean
%{_libexecdir}/openscap/probe_selinuxsecuritycontext
%{_libexecdir}/openscap/probe_shadow
%{_libexecdir}/openscap/probe_sysctl
%{_libexecdir}/openscap/probe_system_info
%{_libexecdir}/openscap/probe_textfilecontent
%{_libexecdir}/openscap/probe_textfilecontent54
%{_libexecdir}/openscap/probe_uname
%{_libexecdir}/openscap/probe_variable
%{_libexecdir}/openscap/probe_xinetd
%{_libexecdir}/openscap/probe_xmlfilecontent
%dir %{_datadir}/openscap
%dir %{_datadir}/openscap/schemas
%dir %{_datadir}/openscap/xsl
%dir %{_datadir}/openscap/cpe
%{_datadir}/openscap/schemas/*
%{_datadir}/openscap/xsl/*
%{_datadir}/openscap/cpe/*

%files python
%defattr(-,root,root,-)
%{python_sitearch}/*

%files perl
%defattr(-,root,root,-)
%{perl_vendorarch}/*
%{perl_vendorlib}/*

%files devel
%defattr(-,root,root,-)
%doc docs/{html,examples}/
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/pkgconfig/*.pc

%files utils
%defattr(-,root,root,-)
%doc docs/oscap-scan.cron
%{_mandir}/man8/*
%{_bindir}/*
%{_sysconfdir}/bash_completion.d

%files content
%defattr(-,root,root,-)
%{_datadir}/openscap/scap-oval.xml
%{_datadir}/openscap/scap-xccdf.xml
%{_datadir}/openscap/scap-fedora14-oval.xml
%{_datadir}/openscap/scap-fedora14-xccdf.xml

%files content-sectool
%defattr(-,root,root,-)
%{_datadir}/openscap/sectool-sce

%files extra-probes
%{_libexecdir}/openscap/probe_ldap57
%{_libexecdir}/openscap/probe_gconf

%files extra-probes-sql
%{_libexecdir}/openscap/probe_sql
%{_libexecdir}/openscap/probe_sql57

%files engine-sce
%{_libdir}/libopenscap_sce.so.*

%files selinux
%attr(0600,root,root) %{_datadir}/selinux/packages/oscap.pp
%{_datadir}/selinux/devel/include/contrib/oscap.if
# %{_mandir}/man8/openscap_selinux.8.*

%changelog
* Thu Sep 26 2013 Šimon Lukašík <slukasik@redhat.com> 0.9.12-2
- Start building SQL probes for Fedora

* Wed Sep 11 2013 Šimon Lukašík <slukasik@redhat.com> 0.9.12-1
- upgrade

* Wed Jul 17 2013 Petr Lautrbach <plautrba@redhat.com> 0.9.11-1
- upgrade

* Fri Jul 12 2013 Petr Lautrbach <plautrba@redhat.com> 0.9.10-1
- upgrade

* Wed Jul 10 2013 Petr Lautrbach <plautrba@redhat.com> 0.9.9-1
- upgrade

* Mon Jun 17 2013 Petr Lautrbach <plautrba@redhat.com> 0.9.8-1
- upgrade

* Fri Apr 26 2013 Petr Lautrbach <plautrba@redhat.com> 0.9.7-1
- upgrade
- add openscap-selinux sub-package

* Tue Apr 23 2013 Petr Lautrbach <plautrba@redhat.com> 0.9.6-1
- upgrade

* Tue Mar 19 2013 Petr Lautrbach <plautrba@redhat.com> 0.9.5-1
- upgrade

* Tue Feb 26 2013 Petr Lautrbach <plautrba@redhat.com> 0.9.4-1
- upgrade

* Mon Dec 17 2012 Petr Lautrbach <plautrba@redhat.com> 0.9.3-1
- upgrade

* Mon Nov 19 2012 Petr Lautrbach <plautrba@redhat.com> 0.9.2-1
- upgrade

* Mon Oct 22 2012 Petr Lautrbach <plautrba@redhat.com> 0.9.1-1
- upgrade

* Tue Sep 25 2012 Peter Vrabec <pvrabec@redhat.com> 0.9.0-1
- upgrade

