Summary: Test package foobar
Name: foobar
Version: 1.0
Release: 1
Group: Testing
License: GPLv2+
BuildArch: noarch

%description
Simple rpm test package (%{name}).

%prep
mkdir -p %{buildroot}%{_sysconfdir}
echo "%{name}" > %{buildroot}%{_sysconfdir}/%{name}

%files
%{_sysconfdir}/%{name}