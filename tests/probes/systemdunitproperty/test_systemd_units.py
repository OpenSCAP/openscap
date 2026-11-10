#!/usr/bin/env python3
"""Exercise both systemd probes on a private bus, without host systemd or root."""

import os
import re
import shlex
import subprocess
import tempfile
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path

import dbus
import dbusmock


BUS_NAME = 'org.freedesktop.systemd1'
MANAGER_PATH = '/org/freedesktop/systemd1'
MANAGER_IFACE = BUS_NAME + '.Manager'
UNIT_IFACE = BUS_NAME + '.Unit'
DEFINITIONS = 'http://oval.mitre.org/XMLSchema/oval-definitions-5'
LINUX = DEFINITIONS + '#linux'
COMMON = 'http://oval.mitre.org/XMLSchema/oval-common-5'
RESULTS = 'http://oval.mitre.org/XMLSchema/oval-results-5'
SYSCHAR = 'http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux'


class SystemdUnitsTest(dbusmock.DBusTestCase):
    @classmethod
    def setUpClass(cls):
        cls.start_session_bus()

    def check_units(self, files, loaded, missing=()):
        server = self.spawn_server(BUS_NAME, MANAGER_PATH, MANAGER_IFACE)
        try:
            obj = self.get_dbus().get_object(BUS_NAME, MANAGER_PATH)
            mock = dbus.Interface(obj, dbusmock.MOCK_IFACE)
            expected = {u for u, _ in files if '@.' not in u} | set(loaded)
            # This fake unit models systemd's dummy defaults for a missing name.
            # The probes must never query it merely because a template exists.
            paths = {u: MANAGER_PATH + '/unit/u' + str(i)
                     for i, u in enumerate(sorted(expected | set(missing) | {'user-runtime-dir.service'}))}
            rows = [(u, u, 'loaded', 'active', 'running', '', paths[u], 0, '', '/')
                    for u in loaded]
            rows += [(u, u, 'not-found', 'inactive', 'dead', '', paths[u], 0, '', '/')
                     for u in missing]
            mock.AddMethods(MANAGER_IFACE, [
                ('ListUnitFiles', '', 'a(ss)', 'ret = ' + repr([
                    ('/usr/lib/systemd/system/' + u, state) for u, state in files])),
                ('ListUnits', '', 'a(ssssssouso)', 'ret = ' + repr(rows)),
                ('LoadUnit', 's', 'o', 'ret = ' + repr(paths) + '[args[0]]'),
            ])
            for unit, path in paths.items():
                mock.AddObject(path, UNIT_IFACE, {
                    'LoadState': 'loaded' if unit in expected else 'not-found',
                    'Requires': dbus.Array([], signature='s'),
                    'Wants': dbus.Array([], signature='s'),
                }, [])

            root = ET.Element('oval_definitions', xmlns=DEFINITIONS)
            generator = ET.SubElement(root, 'generator')
            for key, value in [('product_name', 'OpenSCAP test'), ('product_version', '1'),
                               ('schema_version', '5.11'), ('timestamp', '2026-01-01T00:00:00')]:
                ET.SubElement(generator, '{' + COMMON + '}' + key).text = value
            definitions = ET.SubElement(root, 'definitions')
            tests = ET.SubElement(root, 'tests')
            objects = ET.SubElement(root, 'objects')
            cases = ['user-runtime-dir@1000.service', 'user-runtime-dir@.service',
                     'user-runtime-dir.service', 'disabled.service', 'persist@42.service',
                     'duplicate.target', 'transient.service', 'missing@1.service', None]
            number = 0
            for kind in ['systemdunitproperty', 'systemdunitdependency']:
                for unit in cases:
                    number += 1
                    ident = 'oval:0:{}:' + str(number)
                    definition = ET.SubElement(definitions, 'definition', {
                        'id': ident.format('def'), 'class': 'compliance', 'version': '1'})
                    metadata = ET.SubElement(definition, 'metadata')
                    ET.SubElement(metadata, 'title').text = 'Systemd unit enumeration'
                    ET.SubElement(metadata, 'description').text = 'Installed and loaded units'
                    ET.SubElement(ET.SubElement(definition, 'criteria'), 'criterion',
                                  test_ref=ident.format('tst'))
                    exists = bool(expected) if unit is None else unit in expected
                    test = ET.SubElement(tests, kind + '_test', {
                        'xmlns': LINUX, 'id': ident.format('tst'), 'version': '1',
                        'check': 'all', 'comment': 'true',
                        'check_existence': 'at_least_one_exists' if exists else 'none_exist'})
                    ET.SubElement(test, 'object', object_ref=ident.format('obj'))
                    objdef = ET.SubElement(objects, kind + '_object', {
                        'xmlns': LINUX, 'id': ident.format('obj'), 'version': '1'})
                    ET.SubElement(objdef, 'unit', operation='pattern match').text = (
                        '.*' if unit is None else '^' + re.escape(unit) + '$')
                    if kind == 'systemdunitproperty':
                        ET.SubElement(objdef, 'property').text = 'LoadState'

            env = dict(os.environ, DBUS_SYSTEM_BUS_ADDRESS=os.environ['DBUS_SESSION_BUS_ADDRESS'])
            with tempfile.TemporaryDirectory() as tmp:
                source, result = Path(tmp) / 'input.xml', Path(tmp) / 'results.xml'
                ET.ElementTree(root).write(source, encoding='utf-8', xml_declaration=True)
                run = subprocess.run(shlex.split(os.environ['OSCAP']) + [
                    'oval', 'eval', '--results', str(result), str(source)],
                    env=env, text=True, capture_output=True, timeout=30)
                self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
                self.assertEqual(run.stderr, '')
                collected = ET.parse(result)
                evaluated = collected.findall('.//{' + RESULTS + '}definition')
                self.assertEqual(len(evaluated), number)
                for definition in evaluated:
                    self.assertEqual(definition.get('result'), 'true', definition.attrib)
                for kind in ['systemdunitproperty', 'systemdunitdependency']:
                    items = collected.findall('.//{' + SYSCHAR + '}' + kind + '_item')
                    names = [item.find('{' + SYSCHAR + '}unit').text for item in items]
                    self.assertEqual(set(names), expected)
            calls = mock.GetMethodCalls('LoadUnit')
            loaded_names = [str(args[0]) for _, args in calls]
            self.assertNotIn('user-runtime-dir.service', loaded_names)
            self.assertNotIn('user-runtime-dir@.service', loaded_names)
            self.assertNotIn('missing@1.service', loaded_names)
            if 'duplicate.target' in expected:
                # Exact and wildcard queries from each probe, with no duplicate
                # callbacks from the two lists (or duplicate entries in a list).
                self.assertEqual(loaded_names.count('duplicate.target'), 4)
        finally:
            server.terminate()
            server.wait()

    def test_installed_and_loaded(self):
        self.check_units([
            ('user-runtime-dir@.service', 'static'), ('disabled.service', 'disabled'),
            ('duplicate.target', 'enabled'), ('duplicate.target', 'enabled'),
            ('persist@42.service', 'enabled')], [
                'user-runtime-dir@1000.service', 'duplicate.target', 'transient.service'])

    def test_no_loaded_units(self):
        self.check_units([('disabled.service', 'disabled'), ('persist@42.service', 'enabled')], [])

    def test_no_installed_units(self):
        self.check_units([], ['user-runtime-dir@1000.service', 'transient.service'])

    def test_missing_loaded_unit(self):
        self.check_units([], ['transient.service'], ['missing@1.service', 'user-runtime-dir.service'])

    def test_empty_lists(self):
        self.check_units([], [])


if __name__ == '__main__':
    unittest.main()
