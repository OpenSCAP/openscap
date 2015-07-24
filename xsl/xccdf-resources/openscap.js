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
    $(".toggle-rule-display").each(function(){
        toggleRuleDisplay(this);
    });
    original_treetable = $(".treetable").clone();
    $(".treetable").treetable({ column: 0, expandable: true, clickableNodeNames: true, initialState : "expanded", indent : 0 });
    is_original = true;
});

function Reset() {
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

function NewGroupLine(group_name)
{
	return "<tr class=\"rule-overview-inner-node\" data-tt-id=\"" + group_name + "\">" +
		"<td colspan=\"3\"><strong>" + group_name + "</strong></td></tr>";
}

function GetTargetGroupsList(rule, key)
{
	/* This function returns an array of target groups indentifiers */
	if (key == "severity") {
		var severity = rule.children("td:nth-child(2)").text();
		return [severity];
	} else if (key == "result") {
		var result = rule.children(".rule-result").text();
		return [result];
	}
	else {
		var ref_list = rule.attr(key);
		if (!ref_list) {
			ref_list = "unknown";
		}
		return ref_list.split(",");
	}
}

function GroupBy(key) {
	/* We must process grouping upon the original table.
	 * Otherwise, we would have unwanted duplicties in new table. */
	Reset();

	var lines = {};
	$(".rule-overview-leaf").each(function() {
		$(this).children("td:first").css("padding-left","0px");
		var id = $(this).attr("data-tt-id");
		var target_groups = GetTargetGroupsList($(this), key);
		for (i = 0; i < target_groups.length; i++) {
			var target_group = target_groups[i];
			if (!lines.hasOwnProperty(target_group)) {
				/* Create a new group */
				lines[target_group] = [NewGroupLine(target_group)];
			}
			var clone = $(this).clone();
			clone.attr("data-tt-id", id + "copy" + i);
			clone.attr("data-tt-parent-id", target_group);
			var new_line = clone.wrap("<div>").parent().html();
			lines[target_group].push(new_line);
		}
	});
	$(".treetable").remove();
	var html_text = "";
	for (x in lines) {
		html_text += lines[x].join("\n");
	}
	new_table ="<table class=\"treetable table table-bordered\"><thead><tr><th>Title</th> <th style=\"width: 120px; text-align: center\">Severity</th><th style=\"width: 120px; text-align: center\">Result</th></tr></thead><tbody>" + html_text + "</tbody></table>";
	$("#rule-overview").append(new_table);
	is_original = false;
	$(".treetable").treetable({ column: 0, expandable: true, initialState : "expanded", indent : 0 });

}
