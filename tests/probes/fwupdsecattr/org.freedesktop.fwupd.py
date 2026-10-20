# This is a template for D-Bus mock
# see init_dbus_mock() from test_common.sh.
# The Exit() method is expected by
# clean_dbus_mock() from the same file.

__author__ = 'Evgenii Kolesnikov'
__copyright__ = '''
(c) 2023 Red Hat Inc.
'''

import dbus


BUS_NAME = 'org.freedesktop.fwupd'
MAIN_OBJ = '/'
MAIN_IFACE = 'org.freedesktop.fwupd'
SYSTEM_BUS = False


def load(mock, _parameters):
    mock.AddMethods(MAIN_IFACE, [
        ('GetHostSecurityAttrs', '', 'aa{sv}', 'ret = self.SecurityAattrs'),
        ('Exit', '', '', 'sys.exit()'),
    ])

    mock.SecurityAattrs = [
        {
            'AppstreamId': 'org.fwupd.hsi.Kernel.Lockdown', 'HsiResult': dbus.UInt32(2)
        },
        {
            'AppstreamId': 'org.fwupd.hsi.Kernel.InvalidStatus', 'HsiResult': dbus.UInt32(200)
        }
    ]
