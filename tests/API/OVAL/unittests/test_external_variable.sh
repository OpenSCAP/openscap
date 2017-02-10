#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "result file: $result"
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

$OSCAP oval eval --results $result --variables $srcdir/external_variables.xml $srcdir/$name.oval.xml 2> $stderr
# filter out the expected warnings in stderr
sed -i -E "/^W: oscap:     Referenced variable has no values \(oval:x:var:[13689]\)/d" "$stderr"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

[ -s $result ]

assert_exists 10 '/oval_results/oval_definitions/variables/external_variable'
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
assert_exists 10 $CO'/object'
assert_exists 1 $CO'/object[@id="oval:x:obj:1"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:1"][@flag="does not exist"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:1"]/message'
assert_exists 1 $CO'/object[@id="oval:x:obj:1"]/message[text()="Referenced variable has no values (oval:x:var:1)."]'
assert_exists 1 $CO'/object[@id="oval:x:obj:2"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:2"]/variable_value'
assert_exists 1 $CO'/object[@id="oval:x:obj:2"]/variable_value[@variable_id="oval:x:var:2"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:2"]/variable_value[text()="123-456-7890"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:2"]/reference'
assert_exists 1 $CO'/object[@id="oval:x:obj:3"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:3"][@flag="does not exist"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:3"]/message'
assert_exists 1 $CO'/object[@id="oval:x:obj:3"]/message[text()="Referenced variable has no values (oval:x:var:3)."]'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]/variable_value'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]/variable_value[@variable_id="oval:x:var:4"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]/variable_value[text()="1"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:4"]/reference'

assert_exists 1 $CO'/object[@id="oval:x:obj:5"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:5"]/variable_value'
assert_exists 1 $CO'/object[@id="oval:x:obj:5"]/variable_value[@variable_id="oval:x:var:5"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:5"]/variable_value[text()="13"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:5"]/reference'

assert_exists 1 $CO'/object[@id="oval:x:obj:6"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:6"][@flag="does not exist"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:6"]/message'
assert_exists 1 $CO'/object[@id="oval:x:obj:6"]/message[text()="Referenced variable has no values (oval:x:var:6)."]'

assert_exists 1 $CO'/object[@id="oval:x:obj:7"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:7"]/variable_value'
assert_exists 1 $CO'/object[@id="oval:x:obj:7"]/variable_value[@variable_id="oval:x:var:7"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:7"]/variable_value[text()="300"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:7"]/reference'

assert_exists 1 $CO'/object[@id="oval:x:obj:8"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:8"][@flag="does not exist"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:8"]/message'
assert_exists 1 $CO'/object[@id="oval:x:obj:8"]/message[text()="Referenced variable has no values (oval:x:var:8)."]'

assert_exists 1 $CO'/object[@id="oval:x:obj:9"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:9"][@flag="does not exist"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:9"]/message'
assert_exists 1 $CO'/object[@id="oval:x:obj:9"]/message[text()="Referenced variable has no values (oval:x:var:9)."]'

assert_exists 1 $CO'/object[@id="oval:x:obj:10"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:10"]/variable_value'
assert_exists 1 $CO'/object[@id="oval:x:obj:10"]/variable_value[@variable_id="oval:x:var:10"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:10"]/variable_value[text()="17"]'
assert_exists 1 $CO'/object[@id="oval:x:obj:10"]/reference'

SD='/oval_results/results/system/oval_system_characteristics/system_data'
assert_exists 5 $SD'/ind-sys:variable_item'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:var_ref[text()="oval:x:var:2"]'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:value[text()="123-456-7890"]'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:var_ref[text()="oval:x:var:4"]'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:value[text()="1"]'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:var_ref[text()="oval:x:var:5"]'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:value[text()="13"]'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:var_ref[text()="oval:x:var:7"]'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:value[text()="300"]'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:var_ref[text()="oval:x:var:10"]'
assert_exists 1 $SD'/ind-sys:variable_item/ind-sys:value[text()="17"]'

rm $result
