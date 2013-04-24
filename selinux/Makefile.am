
AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = oscap.if oscap.fc oscap.te

if WANT_SELINUX_POLICY

oscap.pp: oscap.fc  oscap.if  oscap.te
	make -f /usr/share/selinux/devel/Makefile

all-local: oscap.pp

install-data-local:
	install -m 644 oscap.pp /usr/share/selinux/packages
	install -m 644 oscap.if /usr/share/selinux/devel/include/contrib/

uninstall-local:
	rm -f /usr/share/selinux/packages/oscap.pp

clean-local:
	make -f /usr/share/selinux/devel/Makefile clean

endif
