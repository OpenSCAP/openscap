#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
#

import os
from import_handler import oscap, result2str, get_path

'''  Story

   1)       =======   import benchmark with xccdf.init (and not import_benchmark) ====

Benchmark id should be xccdf_com.example.www_benchmark_test
Profile should contains : xccdf_com.example.www_profile_1


   2)         =====    Testing XCCDF_POLICY    ========

When using this profile, get_tailoring_items() should returns this.
But instead of testing exactly this result (which will probably be improved
we only test expected required features, for instance it's better to check
if a feature is included (in order the test pass if more features are added later

[{'choices': {},
  'descs': {None: ''},
  'id': 'xccdf_com.example.www_value_1',
  'lang': None,
  'langs': {None, 'en-US'},
  'match': '^.*$',
  'options': {'dhcp_service': 'dhcpd',
              'ftp_service': 'tftpd',
              'telnet_service': 'telnet-server'},
  'selected': ('telnet_service', 'telnet-server'),
  'titles': {None: '',
             'en-US': 'Which package is installed ? '
                      '(telnet-server/dhcpd/tftpd) '},
  'type': 2}]

    3)        ======   Testing values   ========


Expected items in list returned by get_all_values():
first value id should be : xccdf_com.example.www_value_1  -----
with  title (en-us) = Which package is installed ? (telnet-server/dhcpd/tftpd)

    4)      ========   Refining some values and ensure it's working  ======

    TODO : export the tailored file, re-import it, check the modifications
    and ensure that the initial file isn't modified.

'''

# ======================     Part 1   ============================

print("opening " + get_path("samples/redhat_bench_example/benchmark_example_redhat.xml") + " ...")
benchmark_components = oscap.xccdf.init(get_path("samples/redhat_bench_example/"
                                                 "benchmark_example_redhat.xml"))

pm = benchmark_components['policy_model']
benchmark = pm.get_benchmark()
expected_benchmark_id = "xccdf_com.example.www_benchmark_test"
if benchmark.get_id() != expected_benchmark_id:
    raise Exception("Benchmark id should be {0} but is currently {1}"
                    .format(expected_benchmark_id, benchmark.get_id()))

print("Browsing profiles ...")
profiles = set()
for p in pm.get_benchmark().get_profiles():
    print("\t" + p.get_id())
    profiles.add(p.get_id())

if 'xccdf_com.example.www_profile_1' not in profiles:
    raise Exception("Profile xccdf_com.example.www_profile_1 should be present in get_profiles"
                    "but haven't be found. Current profiles : {0}".format(', '.join(profiles)))

profile = pm.get_benchmark().get_profile_by_id("xccdf_com.example.www_profile_1")
print("selected profile : " + profile.get_id())

profile_not_defined = pm.get_benchmark().get_profile_by_id("xccdf_com.example.profile_not_exists")
if profile_not_defined is not None:
    raise Exception("get_profile_by_id('xccdf_com.example.profile_not_exists') should returns "
                    "None but returned value is {0}".format(profile_not_defined))

# ======================     Part 2   ============================

policy = oscap.xccdf.policy_new(pm, profile)

if "'xccdf_policy'" not in policy.__repr__():
    raise Exception("Variable policy should be a swig object of type 'xccdf_policy'. "
                    "Current object representation : {0}".format(policy.__repr__()))


# ======================     Part 3   ============================


tailor_items = policy.get_tailor_items()

expected_val1_id = 'xccdf_com.example.www_value_1'
expected_val1_options = {'dhcp_service': 'dhcpd',
                         'ftp_service': 'tftpd',
                         'telnet_service': 'telnet-server'}

expected_val1_selected = ('telnet_service', 'telnet-server')

val1 = tailor_items[0]
print("First tailored value raw_content: {0}".format(val1))

if val1['id'] != expected_val1_id:
    raise Exception("Id of first tailored value should be {0} but is {1}"
                    .format(expected_val1_id, val1["id"]))

if val1['options'] != expected_val1_options:
    raise Exception("Options of tailored value {0} should be {1} but is {2}"
                    .format(val1['id'], expected_val1_options, val1["options"]))


if val1['selected'] != expected_val1_selected:
    raise Exception("Selected option of tailored value {0} should be {1} but is {2}"
                    .format(val1['id'], expected_val1_selected, val1["selected"]))

if 'en-US' not in val1['langs']:
    raise Exception("Available langs of tailored value {0} should contains en-US but is {1}"
                    .format(val1['id'], ', '.join(val1["langs"])))

if val1['match'] != '^.*$':
    raise Exception("Match propety of tailored value {0} should be '^.*$' but is {1}"
                    .format(val1['id'], val1["match"]))

if int(val1['type']) != 2:
    raise Exception("Type of tailored value {0} should be '2' but is {1}"
                    .format(val1['type'], val1["type"]))

print("All retrievied values seems OK (id, options, selected option, lang, match, type)")

# ======================     Part 4   ============================


''' Should replace the selected option with ('ftp_service', 'tftpd') '''
new_tailored_value = {'id': expected_val1_id,
                      'value': 'tftpd'
                      }

new_expected_selected_value = ()

policy.set_tailor_items([new_tailored_value])
val1bis = policy.get_tailor_items()

if val1bis[0]['selected'] != ('ftp_service', 'tftpd'):
    raise Exception("After setting of the default value {0}, its selected option "
                    "should be ('ftp_service', 'tftpd') but is currently {1}"
                    .format(expected_val1_id, val1bis['selected']))

print("Default value refining (set_tailoring_item) seems to work fine.")
