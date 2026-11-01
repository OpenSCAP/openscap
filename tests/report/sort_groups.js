const assert = require("assert");
const fs = require("fs");
const path = require("path");
const vm = require("vm");

const xslDir = path.resolve(__dirname, "../../xsl");
const source = fs.readFileSync(path.join(xslDir, "xccdf-resources/openscap.js"), "utf8");
const bundle = fs.readFileSync(path.join(xslDir, "xccdf-resources.xsl"), "utf8");
const start = bundle.indexOf("function openRuleDetailsDialog(");
const end = bundle.indexOf("]]></xsl:template>", start);
assert(start >= 0 && end > start, "Report JavaScript must be present in the generated bundle");

function permutations(values) {
    if (values.length === 0)
        return [[]];
    return values.flatMap((value, index) =>
        permutations(values.filter((_, other) => other !== index))
            .map(rest => [value, ...rest]));
}

for (const [name, script] of [
    ["source", source],
    ["generated bundle", bundle.slice(start, end)]
]) {
    // Loading the report's sorting functions does not require running DOM setup.
    const context = vm.createContext({
        document: {},
        $: () => ({ready: () => {}})
    });
    vm.runInContext(script, context);

    function check(input, expected, key = "cis") {
        assert.deepStrictEqual(
            Array.from(context.sortGroups([...input], key)),
            expected,
            `${name}: ${key} sorting of ${JSON.stringify(input)}`
        );
    }

    const referenceKeys = [
        "cui", "nist", "anssi", "pcidss4", "nist-csf", "bsi", "ccn", "cis",
        "cis-csc", "ism", "disa", "app-srg-ctr", "app-srg", "os-srg", "stigid",
        "stigref_vulnerability_id", "stigref", "cjis", "hipaa", "isa-62443-2013",
        "isa-62443-2009", "cobit5", "iso27001-2013", "nerc-cip", "ospp",
        "pcidss", "dcid"
    ];
    const mixed = ["1.2.1.1", "1.2.1.2", "enable_authselect", "reload_dconf_db"];
    for (const key of referenceKeys) {
        for (const input of permutations(mixed))
            check(input, mixed, key);
    }

    for (const input of permutations(["AU-9(3)", "AU-12 b", "AU-12(c)"]))
        check(input, ["AU-9(3)", "AU-12 b", "AU-12(c)"], "nist");
    for (const input of permutations(["1", "01", "2", "10"]))
        check(input, ["01", "1", "2", "10"]);

    check(["1.2.10", "1.2.2", "1.2", "1.10.1", "1.2.1"],
        ["1.2", "1.2.1", "1.2.2", "1.2.10", "1.10.1"]);
    check(["AC-2(10)", "AC-2(2)", "AC-2", "AC-1"],
        ["AC-1", "AC-2", "AC-2(2)", "AC-2(10)"], "nist");
    check(["unknown", "1.2.1.2", "1.2.1.1"], ["1.2.1.1", "1.2.1.2", "unknown"]);
    check([], []);
    check(["1.2.1.2"], ["1.2.1.2"]);
    check(["1.2.1.2", "1.2.1.2"], ["1.2.1.2", "1.2.1.2"]);
    for (const input of permutations(["low", "high", "medium"]))
        check(input, ["high", "medium", "low"], context.KeysEnum.SEVERITY);
    for (const input of permutations(["pass", "notapplicable", "fail"]))
        check(input, ["fail", "notapplicable", "pass"], context.KeysEnum.RESULT);

    let resets = 0;
    context.resetTreetable = () => { resets++; };
    context.$ = () => { throw new Error("Default grouping must not rebuild the table"); };
    context.groupRulesBy(context.KeysEnum.DEFAULT);
    assert.strictEqual(resets, 1, `${name}: default grouping restores the original table`);
    console.log(`${name}: report group sorting tests passed`);
}
