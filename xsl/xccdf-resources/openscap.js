function openRuleDetailsDialog(rule_result_id)
{
    $("#detail-modal").remove();

    var closebutton = $('<button type="button" class="close btn btn-sm btn-default" data-dismiss="modal" aria-hidden="true" title="Close">&#x274c;</button>');
    var modal = $('<div id="detail-modal" class="modal fade" tabindex="-1" role="dialog" aria-hidden="true"><div id="detail-modal-body" class="modal-body"></div></div>');

    $("body").prepend(modal);

    var clone = $("#rule-detail-" + rule_result_id).clone();
    clone.attr("id", "");
    clone.children(".panel-heading").append(closebutton);
    closebutton.css( { "float" : "right" } );
    closebutton.css( { "margin-top" : "-=23px" } );
    $("#detail-modal-body").append(clone);

    $("#detail-modal").modal();

    return false;
}

function toggleRuleDisplay(checkbox)
{
    var result = checkbox.value;

    if (checkbox.checked)
    {
        $(".rule-overview-leaf-" + result).removeClass("rule-result-filtered");
        $(".rule-detail-" + result).removeClass("rule-result-filtered");
    }
    else
    {
        $(".rule-overview-leaf-" + result).addClass("rule-result-filtered");
        $(".rule-detail-" + result).addClass("rule-result-filtered");
    }
    stripeTreeTable();
}

function toggleResultDetails(button)
{
    var result_details = $("#result-details");

    if (result_details.is(":visible"))
    {
        result_details.hide();
        $(button).html("Show all result details");
    }
    else
    {
        result_details.show();
        $(button).html("Hide all result details");
    }

    return false;
}

function ruleSearchMatches(detail_leaf, keywords)
{
    if (keywords.length == 0)
        return true;

    var match = true;
    var checked_keywords = detail_leaf.children(".keywords").text().toLowerCase();

    var index;
    for (index = 0; index < keywords.length; ++index)
    {
        if (checked_keywords.indexOf(keywords[index].toLowerCase()) < 0)
        {
            match = false;
            break;
        }
    }

    return match;
}

function ruleSearch()
{
    var search_input = $("#search-input").val();
    var keywords = search_input.split(/[\s,\.;]+/);
    var matches = 0;

    $(".rule-detail").each(function(){
        // rule-detail-rrid, that's offset 12
        var rrid = $(this).attr("id").substring(12);

        var overview_leaf = $("#rule-overview-leaf-" + rrid);
        var detail_leaf = $(this);

        if (ruleSearchMatches(detail_leaf, keywords))
        {
            overview_leaf.removeClass("search-no-match");
            detail_leaf.removeClass("search-no-match");
            ++matches;
        }
        else
        {
            overview_leaf.addClass("search-no-match");
            detail_leaf.addClass("search-no-match");
        }
    });

    if (!search_input)
        $("#search-matches").html("");
    else if (matches > 0)
        $("#search-matches").html(matches.toString() + " rules match.");
    else
        $("#search-matches").html("No rules match your search criteria!");
}

var is_original = true;
var original_treetable = null;

$(document).ready( function() {
    $("#result-details").hide();
    $(".js-only").show();
    $(".form-group select").val("default");
    $(".toggle-rule-display").each(function(){
        toggleRuleDisplay(this);
    });
    original_treetable = $(".treetable").clone();
    $(".treetable").treetable({ column: 0, expandable: true, clickableNodeNames: true, initialState : "expanded", indent : 0 });
    is_original = true;
    stripeTreeTable();
});

function resetTreetable() {
    if (!is_original) {
        $(".treetable").remove();
        $("#rule-overview").append(original_treetable.clone());
        $(".treetable").treetable({ column: 0, expandable: true, clickableNodeNames: true, initialState : "expanded", indent : 0 });
        $(".toggle-rule-display").each(function(){
            toggleRuleDisplay(this);
        });
        is_original = true;
    }
}

function newGroupLine(group_name)
{
    return "<tr class=\"rule-overview-inner-node\" data-tt-id=\"" + group_name + "\">" +
        "<td colspan=\"3\"><strong>" + group_name + "</strong></td></tr>";
}

var KeysEnum = {
    DEFAULT: "default",
    SEVERITY: "severity",
    RESULT: "result",
    NIST: "http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-53r4.pdf",
    DISA: "http://iase.disa.mil/stigs/cci/Pages/index.aspx"
};

/* This function returns an array of target groups indentifiers */
function getTargetGroupsList(rule, key)
{
    switch(key) {
    case KeysEnum.SEVERITY:
        var severity = rule.children(".rule-severity").text();
        return [severity];
    case KeysEnum.RESULT:
        var result = rule.children(".rule-result").text();
        return [result];
    default:
        try {
            var references = JSON.parse(rule.attr("data-references"));
        } catch (err) {
            return ["unknown"];
        }
        if (!references.hasOwnProperty(key))
            return ["unknown"];
        return references[key];
    }
}

function sortGroups(groups, key)
{
    switch(key) {
    case KeysEnum.SEVERITY:
        return ["high", "medium", "low"];
    case KeysEnum.DISA:
        return groups.sort(function(a, b){
            return parseInt(a) - parseInt(b);
        });
    case KeysEnum.NIST:
        return groups.sort(function(a, b){
            var regex = /(\w\w)-(\d+)(.*)/;
            var a_parts = regex.exec(a);
            var b_parts = regex.exec(b);
            if (a_parts == null)
                return 1;
            if (b_parts == null)
                return -1;
            var result = a_parts[1].localeCompare(b_parts[1]);
            if (result != 0) {
                return result;
            } else {
                result = a_parts[2] - b_parts[2];
                if (result != 0) {
                    return result;
                } else {
                    return a_parts[3].localeCompare(b_parts[3]);
                }
            }
        });
    default:
        return groups.sort();
    }
}

function groupRulesBy(key) {
    /* We must reset the treetable and process grouping upon the original
     * table to avoid unwanted duplicties in new table. */
    resetTreetable();
    if (key == KeysEnum.DEFAULT)
        return;

    /* Browse the rules and sort them into groups */
    var lines = {};
    $(".rule-overview-leaf").each(function() {
        $(this).children("td:first").css("padding-left","0px");
        var id = $(this).attr("data-tt-id");
        var target_groups = getTargetGroupsList($(this), key);
        for (i = 0; i < target_groups.length; i++) {
            var target_group = target_groups[i];
            if (!lines.hasOwnProperty(target_group)) {
                /* Create a new group */
                lines[target_group] = [newGroupLine(target_group)];
            }
            var clone = $(this).clone();
            clone.attr("data-tt-id", id + "copy" + i);
            clone.attr("data-tt-parent-id", target_group);
            var new_line = clone.wrap("<div>").parent().html();
            lines[target_group].push(new_line);
        }
    });

    /* Remove old treetable and replace it with a new one */
    $(".treetable").remove();
    var groups = sortGroups(Object.keys(lines), key);
    var html_text = "";
    for (i = 0; i < groups.length; i++) {
        html_text += lines[groups[i]].join("\n");
    }
    new_table ="<table class=\"treetable table table-bordered\"><thead><tr><th>Group</th> <th style=\"width: 120px; text-align: center\">Severity</th><th style=\"width: 120px; text-align: center\">Result</th></tr></thead><tbody>" + html_text + "</tbody></table>";
    $("#rule-overview").append(new_table);
    is_original = false;
    $(".treetable").treetable({
        column: 0,
        expandable: true,
        clickableNodeNames: true,
        initialState: "expanded",
        indent: 0 });
    stripeTreeTable();
}

function stripeTreeTable()
{
    var rows = $(".rule-overview-leaf:not(.rule-result-filtered)");
    var even = false;
    $(rows).each(function(){
        $(this).css("background-color", even ? "#F9F9F9" : "inherit");
        even = !even;
    });
}
