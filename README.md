# OpenSCAP

[![Build status](https://ci.appveyor.com/api/projects/status/3o5fnld1m98bo0so/branch/master?svg=true)](https://ci.appveyor.com/project/OpenSCAP/openscap/branch/master)

Open Source Security Compliance Solution

## About

The oscap program is a command line tool that allows users to load, scan,
validate, edit, and export SCAP documents.

 * Homepage of the project: [www.open-scap.org](http://www.open-scap.org)
 * Manual: [Oscap User Manual](https://github.com/OpenSCAP/openscap/blob/maint-1.2/docs/manual/manual.adoc)
 * For new contributors: [How to contribute](https://github.com/OpenSCAP/openscap/blob/maint-1.2/docs/contribute/contribute.adoc)

## Compilation

Choose *1a* or *1b* depending on whether you want sources from a release tarball or the git repository.


1) a) Use a release tarball:
```
# replace ${version} with the desired version
wget https://github.com/OpenSCAP/openscap/releases/download/${version}/openscap-${version}.tar.gz
tar -xzpf openscap-${version}.tar.gz
cd openscap-${version}
```

**OR**

1) b) Use fresh sources from git repository.

```
git clone https://github.com/OpenSCAP/openscap.git
cd openscap
```

2) To build the library you will also need the following build dependencies
(some of these are optional, if they are not detected, openscap will be compiled
without respective optional features):
```
cmake \
dbus-devel GConf2-devel libacl-devel libblkid-devel libcap-devel libcurl-devel \
libgcrypt-devel libselinux-devel libxml2-devel libxslt-devel make openldap-devel \
pcre-devel perl-XML-Parser perl-XML-XPath perl-devel python-devel rpm-devel swig \
bzip2-devel
```
On Ubuntu 16.04 the command to install these package is
```code
sudo apt-get install -y autoconf automake libtool make libdbus-1-dev libdbus-glib-1-dev libcurl4-openssl-dev libgcrypt20-dev libselinux1-dev libxslt1-dev libgconf2-dev libacl1-dev libblkid-dev libcap-dev libxml2-dev libldap2-dev libpcre3-dev python-dev swig libxml-parser-perl libxml-xpath-perl libperl5.22 python-dev libbz2-dev librpm-dev swig
```

When you have all the build dependencies installed you can run the following
commands to build the library:
```
cd build/
cmake ..
make
```

3) After building the library you might want to run library self-checks. To do
that you need to have these additional packages installed:
```
wget lua which procps-ng initscripts chkconfig sendmail
```
and it is also required to have `sendmail` service running on the system:
```
systemctl start sendmail.service
```
Now you can execute the following command to run library self-checks:
```
make test
```
Note: If you want to run `make distcheck` you will also need to install
`asciidoctor`. You can either install `rubygem-asciidoctor` package (available
on Fedora), or you can install `rubygems` package and then run
`gem install asciidoctor`.

It's also possible to use the make check to test any other oscap binary present in the system. You just have to set the path of the binary to the CUSTOM_OSCAP variable:
```
export CUSTOM_OSCAP=/usr/bin/oscap; make test
```
Not every check tests the oscap tool, however, when the CUSTOM_OSCAP variable is set, only the checks which do are executed.


4) Run the installation procedure by executing the following command:
```
make install
```


## Use cases

### SCAP Content Validation

 * The following example shows how to validate a given source data stream; all components within the data stream are validated (XCCDF, OVAL, OCIL, CPE, and possibly other components):
```
oscap ds sds-validate scap-ds.xml
```


### Scanning

 * To evaluate all definitions within the given OVAL Definition file, run the following command:
```
oscap oval eval --results oval-results.xml scap-oval.xml
```
*where scap-oval.xml is the OVAL Definition file and oval-results.xml is the OVAL Result file.*

 * To evaluate all definitions from the OVAL component that are part of a particular data stream within a SCAP data stream collection, run the following command:
```
oscap oval eval --datastream-id ds.xml --oval-id xccdf.xml --results oval-results.xml scap-ds.xml
```
*where ds.xml is the given data stream, xccdf.xml is an XCCDF file specifying the OVAL component, oval-results.xml is the OVAL Result file, and scap-ds.xml is a file representing the SCAP data stream collection.*

 * To evaluate a specific profile in an XCCDF file run this command:
```
oscap xccdf eval --profile Desktop --results xccdf-results.xml --cpe cpe-dictionary.xml scap-xccdf.xml
```
*where scap-xccdf.xml is the XCCDF document, Desktop is the selected profile from the XCCDF document, xccdf-results.xml is a file storing the scan results, and cpe-dictionary.xml is the CPE dictionary.*

 * To evaluate a specific XCCDF benchmark that is part of a data stream within a SCAP data stream collection run the following command:
```
oscap xccdf eval --datastream-id ds.xml --xccdf-id xccdf.xml --results xccdf-results.xml scap-ds.xml
```
*where scap-ds.xml is a file representing the SCAP data stream collection, ds.xml is the particular data stream, xccdf.xml is ID of the component-ref pointing to the desired XCCDF document, and xccdf-results.xml is a file containing the scan results.*


### Document generation

 * without XCCDF rules
```
oscap xccdf generate guide XCCDF-FILE > XCCDF-GUIDE-FILE
```

 * with XCCDF rules
```
oscap xccdf generate guide --profile PROFILE XCCDF-FILE > XCCDF-GUIDE-FILE
```

 *  generate report from scanning
```
oscap xccdf generate report XCCDF-RESULT-FILE > XCCDF-REPORT-FILE
```
