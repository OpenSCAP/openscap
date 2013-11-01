
AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = oscap.if oscap.fc oscap.te

if WANT_SELINUX_POLICY

oscap.pp: oscap.fc  oscap.if  oscap.te
	make -f /usr/share/selinux/devel/Makefile

all: oscap.pp

install: oscap.pp
	mkdir -p $(DESTDIR)$(datadir)/selinux/packages $(DESTDIR)$(datadir)/selinux/devel/include/contrib/
	install -m 644 oscap.pp $(DESTDIR)$(datadir)/selinux/packages
	install -m 644 oscap.if $(DESTDIR)$(datadir)/selinux/devel/include/contrib/

uninstall:
	rm -f $(DESTDIR)$(datadir)/selinux/packages/oscap.pp

clean:
	make -f /usr/share/selinux/devel/Makefile clean

endif
