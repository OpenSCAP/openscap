#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "result file: $result"
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

$OSCAP oval eval --results $result --variables $srcdir/external_variables.xml $srcdir/$name.oval.xml 2> $stderr
[ ! -s $stderr ] && rm $stderr
[ -s $result ]

assert_exists 4 '/oval_results/oval_definitions/variables/external_variable'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:1"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:1"]/possible_restriction'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:1"]/possible_restriction[@hint="hint"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:1"]/possible_restriction/restriction'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:1"]/possible_restriction/restriction[@operation="pattern match"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:1"]/possible_restriction/restriction[text()="^[0-9]{3}-[0-9]{3}-[0-9]{4}$"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:2"]'
assert_exists 2 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:2"]/possible_restriction'
assert_exists 2 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:2"]/possible_restriction/restriction'
assert_exists 2 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:2"]/possible_restriction/restriction[@operation="pattern match"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:2"]/possible_restriction[@hint="This restricts the variable value(s) to the 10 digit telephone number format xxx-xxx-xxxx"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:2"]/possible_restriction[@hint="This restricts the variable value(s) to the 1 plus 10 digit telephone number format x-xxx-xxx-xxxx"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:2"]/possible_restriction/restriction[text()="^[0-9]{3}-[0-9]{3}-[0-9]{4}$"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:2"]/possible_restriction/restriction[text()="^1-[0-9]{3}-[0-9]{3}-[0-9]{4}$"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:3"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:3"]/possible_value'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:3"]/possible_value[text()="0"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:4"]'
assert_exists 2 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:4"]/possible_value'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:4"]/possible_value[@hint="This restricts the variable value(s) to 0."]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:4"]/possible_value[text()="0"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:4"]/possible_value[text()="1"]'
assert_exists 1 '/oval_results/oval_definitions/variables/external_variable[@id="oval:x:var:4"]/possible_value[@hint="This restricts the variable value(s) to 1."]'

CO='/oval_results/results/system/oval_system_characteristics/collected_objects'
assert_exists 4 $CO'/object'
assert_exists 1 $CO'/object[@id="oval:x:obj:1"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:2"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:3"]'
assert_exists 1 $CO'/object[@flag="does not exist"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:3"]/message'
assert_exists 1 $CO'/object[@id="oval:x:obj:3"]/message[text()="Referenced variable has no values (oval:x:var:3)."]'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]/variable_value'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]/variable_value[@variable_id="oval:x:var:4"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]/variable_value[text()="1"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]/reference'

rm $result
