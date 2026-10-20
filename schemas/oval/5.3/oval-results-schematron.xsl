<?xml version="1.0" standalone="yes"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:iso="http://purl.oclc.org/dsdl/schematron" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:oval-res="http://oval.mitre.org/XMLSchema/oval-results-5" xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5" version="1.0"><!--Implementers: please note that overriding process-prolog or process-root is 
    the preferred method for meta-stylesheets to use where possible. -->
<axsl:param name="archiveDirParameter"/><axsl:param name="archiveNameParameter"/><axsl:param name="fileNameParameter"/><axsl:param name="fileDirParameter"/>

<!--PHASES-->


<!--PROLOG-->


<!--KEYS-->


<!--DEFAULT RULES-->


<!--MODE: SCHEMATRON-SELECT-FULL-PATH-->
<!--This mode can be used to generate an ugly though full XPath for locators-->
<axsl:template match="*" mode="schematron-select-full-path"><axsl:apply-templates select="." mode="schematron-get-full-path"/></axsl:template>

<!--MODE: SCHEMATRON-FULL-PATH-->
<!--This mode can be used to generate an ugly though full XPath for locators-->
<axsl:template match="*" mode="schematron-get-full-path"><axsl:apply-templates select="parent::*" mode="schematron-get-full-path"/><axsl:text>/</axsl:text><axsl:choose><axsl:when test="namespace-uri()=''"><axsl:value-of select="name()"/><axsl:variable name="p_1" select="1+    count(preceding-sibling::*[name()=name(current())])"/><axsl:if test="$p_1&gt;1 or following-sibling::*[name()=name(current())]">[<axsl:value-of select="$p_1"/>]</axsl:if></axsl:when><axsl:otherwise><axsl:text>*[local-name()='</axsl:text><axsl:value-of select="local-name()"/><axsl:text>' and namespace-uri()='</axsl:text><axsl:value-of select="namespace-uri()"/><axsl:text>']</axsl:text><axsl:variable name="p_2" select="1+   count(preceding-sibling::*[local-name()=local-name(current())])"/><axsl:if test="$p_2&gt;1 or following-sibling::*[local-name()=local-name(current())]">[<axsl:value-of select="$p_2"/>]</axsl:if></axsl:otherwise></axsl:choose></axsl:template><axsl:template match="@*" mode="schematron-get-full-path"><axsl:text>/</axsl:text><axsl:choose><axsl:when test="namespace-uri()=''">@<axsl:value-of select="name()"/></axsl:when><axsl:otherwise><axsl:text>@*[local-name()='</axsl:text><axsl:value-of select="local-name()"/><axsl:text>' and namespace-uri()='</axsl:text><axsl:value-of select="namespace-uri()"/><axsl:text>']</axsl:text></axsl:otherwise></axsl:choose></axsl:template>

<!--MODE: SCHEMATRON-FULL-PATH-2-->
<!--This mode can be used to generate prefixed XPath for humans-->
<axsl:template match="node() | @*" mode="schematron-get-full-path-2"><axsl:for-each select="ancestor-or-self::*"><axsl:text>/</axsl:text><axsl:value-of select="name(.)"/><axsl:if test="preceding-sibling::*[name(.)=name(current())]"><axsl:text>[</axsl:text><axsl:value-of select="count(preceding-sibling::*[name(.)=name(current())])+1"/><axsl:text>]</axsl:text></axsl:if></axsl:for-each><axsl:if test="not(self::*)"><axsl:text/>/@<axsl:value-of select="name(.)"/></axsl:if></axsl:template>

<!--MODE: GENERATE-ID-FROM-PATH -->
<axsl:template match="/" mode="generate-id-from-path"/><axsl:template match="text()" mode="generate-id-from-path"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:value-of select="concat('.text-', 1+count(preceding-sibling::text()), '-')"/></axsl:template><axsl:template match="comment()" mode="generate-id-from-path"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:value-of select="concat('.comment-', 1+count(preceding-sibling::comment()), '-')"/></axsl:template><axsl:template match="processing-instruction()" mode="generate-id-from-path"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:value-of select="concat('.processing-instruction-', 1+count(preceding-sibling::processing-instruction()), '-')"/></axsl:template><axsl:template match="@*" mode="generate-id-from-path"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:value-of select="concat('.@', name())"/></axsl:template><axsl:template match="*" mode="generate-id-from-path" priority="-0.5"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:text>.</axsl:text><axsl:value-of select="concat('.',name(),'-',1+count(preceding-sibling::*[name()=name(current())]),'-')"/></axsl:template><!--MODE: SCHEMATRON-FULL-PATH-3-->
<!--This mode can be used to generate prefixed XPath for humans 
	(Top-level element has index)-->
<axsl:template match="node() | @*" mode="schematron-get-full-path-3"><axsl:for-each select="ancestor-or-self::*"><axsl:text>/</axsl:text><axsl:value-of select="name(.)"/><axsl:if test="parent::*"><axsl:text>[</axsl:text><axsl:value-of select="count(preceding-sibling::*[name(.)=name(current())])+1"/><axsl:text>]</axsl:text></axsl:if></axsl:for-each><axsl:if test="not(self::*)"><axsl:text/>/@<axsl:value-of select="name(.)"/></axsl:if></axsl:template>

<!--MODE: GENERATE-ID-2 -->
<axsl:template match="/" mode="generate-id-2">U</axsl:template><axsl:template match="*" mode="generate-id-2" priority="2"><axsl:text>U</axsl:text><axsl:number level="multiple" count="*"/></axsl:template><axsl:template match="node()" mode="generate-id-2"><axsl:text>U.</axsl:text><axsl:number level="multiple" count="*"/><axsl:text>n</axsl:text><axsl:number count="node()"/></axsl:template><axsl:template match="@*" mode="generate-id-2"><axsl:text>U.</axsl:text><axsl:number level="multiple" count="*"/><axsl:text>_</axsl:text><axsl:value-of select="string-length(local-name(.))"/><axsl:text>_</axsl:text><axsl:value-of select="translate(name(),':','.')"/></axsl:template><!--Strip characters--><axsl:template match="text()" priority="-1"/>

<!--SCHEMA METADATA-->
<axsl:template match="/"><axsl:apply-templates select="/" mode="M6"/><axsl:apply-templates select="/" mode="M7"/><axsl:apply-templates select="/" mode="M8"/><axsl:apply-templates select="/" mode="M9"/><axsl:apply-templates select="/" mode="M10"/><axsl:apply-templates select="/" mode="M11"/><axsl:apply-templates select="/" mode="M12"/><axsl:apply-templates select="/" mode="M13"/><axsl:apply-templates select="/" mode="M14"/><axsl:apply-templates select="/" mode="M15"/><axsl:apply-templates select="/" mode="M16"/></axsl:template>

<!--SCHEMATRON PATTERNS-->


<!--PATTERN system-->


	<!--RULE -->
<axsl:template match="oval-res:system[oval-res:tests]" priority="1001" mode="M6">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='full'"/><axsl:otherwise>the tests element should not be included unless full results are to be provided (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M6"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:system[not(oval-res:tests)]" priority="1000" mode="M6">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='thin'"/><axsl:otherwise>the tests element should be included when full results are specified (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M6"/></axsl:template><axsl:template match="text()" priority="-1" mode="M6"/><axsl:template match="@*|node()" priority="-2" mode="M6"><axsl:apply-templates select="@*|*" mode="M6"/></axsl:template>

<!--PATTERN directives-->


	<!--RULE -->
<axsl:template match="oval-res:definition[@result='true' and oval-res:criteria]" priority="1011" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='full'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='true' and not(oval-res:criteria)]" priority="1010" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should not be included (see directives) xx<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='thin'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='false' and oval-res:criteria]" priority="1009" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='full'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='false' and not(oval-res:criteria)]" priority="1008" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='thin'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='unknown' and oval-res:criteria]" priority="1007" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='full'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='unknown' and not(oval-res:criteria)]" priority="1006" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='thin'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='error' and oval-res:criteria]" priority="1005" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='full'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='error' and not(oval-res:criteria)]" priority="1004" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='thin'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not evaluated' and oval-res:criteria]" priority="1003" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='full'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not evaluated' and not(oval-res:criteria)]" priority="1002" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='thin'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not applicable' and oval-res:criteria]" priority="1001" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='full'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not applicable' and not(oval-res:criteria)]" priority="1000" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@reported='true'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='thin'"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template><axsl:template match="text()" priority="-1" mode="M7"/><axsl:template match="@*|node()" priority="-2" mode="M7"><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

<!--PATTERN testids-->


	<!--RULE -->
<axsl:template match="oval-res:test" priority="1000" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@test_id = ../../oval-res:definitions//oval-res:criterion/@test_ref"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@test_id"/><axsl:text/> - the specific test is not used in any definition's criteria<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template><axsl:template match="text()" priority="-1" mode="M8"/><axsl:template match="@*|node()" priority="-2" mode="M8"><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

<!--PATTERN empty_def_doc-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions" priority="1000" mode="M9">

		<!--ASSERT -->
<axsl:choose><axsl:when test="oval-def:definitions or oval-def:tests or oval-def:objects or oval-def:states or oval-def:variables"/><axsl:otherwise>A valid OVAL Definition document must contain at least one definitions, tests, objects, states, or variables element. The optional definitions, tests, objects, states, and variables sections define the specific characteristics that should be evaluated on a system to determine the truth values of the OVAL Definition Document. To be valid though, at least one definitions, tests, objects, states, or variables element must be present.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M9"/></axsl:template><axsl:template match="text()" priority="-1" mode="M9"/><axsl:template match="@*|node()" priority="-2" mode="M9"><axsl:apply-templates select="@*|*" mode="M9"/></axsl:template>

<!--PATTERN test_type-->


	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:tests/*[@check_existence='none_exist']" priority="1000" mode="M10">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(*[name()='state'])"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="@id"/><axsl:text/> - No state should be referenced when check_existence has a value of 'none_exist'.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M10"/></axsl:template><axsl:template match="text()" priority="-1" mode="M10"/><axsl:template match="@*|node()" priority="-2" mode="M10"><axsl:apply-templates select="@*|*" mode="M10"/></axsl:template>

<!--PATTERN setobjref-->


	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:object_reference" priority="1002" mode="M11">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template>

	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:object_reference" priority="1001" mode="M11">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template>

	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:set/oval-def:object_reference" priority="1000" mode="M11">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="../../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template><axsl:template match="text()" priority="-1" mode="M11"/><axsl:template match="@*|node()" priority="-2" mode="M11"><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template>

<!--PATTERN value-->


	<!--RULE -->
<axsl:template match="oval-def:constant_variable/oval-def:value" priority="1000" mode="M12">

		<!--ASSERT -->
<axsl:choose><axsl:when test=".!=''"/><axsl:otherwise>The value element of the constant_variable <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> can not be empty.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M12"/></axsl:template><axsl:template match="text()" priority="-1" mode="M12"/><axsl:template match="@*|node()" priority="-2" mode="M12"><axsl:apply-templates select="@*|*" mode="M12"/></axsl:template>

<!--PATTERN literal_component-->


	<!--RULE -->
<axsl:template match="oval-def:literal_component" priority="1000" mode="M13">

		<!--ASSERT -->
<axsl:choose><axsl:when test=".!=''"/><axsl:otherwise>The literal_component of variable <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> can not be empty.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template><axsl:template match="text()" priority="-1" mode="M13"/><axsl:template match="@*|node()" priority="-2" mode="M13"><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template>

<!--PATTERN item_field-->


	<!--RULE -->
<axsl:template match="oval-def:object_component" priority="1000" mode="M14">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@item_field!=''"/><axsl:otherwise>The item_field attribute of an object_component of variable <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> can not be empty.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M14"/></axsl:template><axsl:template match="text()" priority="-1" mode="M14"/><axsl:template match="@*|node()" priority="-2" mode="M14"><axsl:apply-templates select="@*|*" mode="M14"/></axsl:template>

<!--PATTERN entityvarref-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*|oval-def:states/*/*" priority="1000" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@var_ref) or .=''"/><axsl:otherwise>
                    <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var-ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so no value should be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template><axsl:template match="text()" priority="-1" mode="M15"/><axsl:template match="@*|node()" priority="-2" mode="M15"><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

<!--PATTERN entitystatus-->


	<!--RULE -->
<axsl:template match="oval-sc:system_data/*/*" priority="1000" mode="M16">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@status) or @status='exists' or .=''"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a value for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity should only be supplied if the status attribute is 'exists'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@mask) or @mask='false' or .=''"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a value for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity should only be supplied if the mask attribute is 'false'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M16"/></axsl:template><axsl:template match="text()" priority="-1" mode="M16"/><axsl:template match="@*|node()" priority="-2" mode="M16"><axsl:apply-templates select="@*|*" mode="M16"/></axsl:template></axsl:stylesheet>
