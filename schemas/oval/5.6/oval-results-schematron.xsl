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
<axsl:template match="/"><axsl:apply-templates select="/" mode="M6"/><axsl:apply-templates select="/" mode="M7"/><axsl:apply-templates select="/" mode="M8"/><axsl:apply-templates select="/" mode="M9"/><axsl:apply-templates select="/" mode="M10"/><axsl:apply-templates select="/" mode="M11"/><axsl:apply-templates select="/" mode="M12"/><axsl:apply-templates select="/" mode="M13"/><axsl:apply-templates select="/" mode="M14"/><axsl:apply-templates select="/" mode="M15"/><axsl:apply-templates select="/" mode="M16"/><axsl:apply-templates select="/" mode="M17"/><axsl:apply-templates select="/" mode="M18"/><axsl:apply-templates select="/" mode="M19"/><axsl:apply-templates select="/" mode="M20"/><axsl:apply-templates select="/" mode="M21"/><axsl:apply-templates select="/" mode="M22"/><axsl:apply-templates select="/" mode="M23"/><axsl:apply-templates select="/" mode="M24"/><axsl:apply-templates select="/" mode="M25"/><axsl:apply-templates select="/" mode="M26"/><axsl:apply-templates select="/" mode="M27"/><axsl:apply-templates select="/" mode="M28"/><axsl:apply-templates select="/" mode="M29"/><axsl:apply-templates select="/" mode="M30"/><axsl:apply-templates select="/" mode="M31"/><axsl:apply-templates select="/" mode="M32"/><axsl:apply-templates select="/" mode="M33"/><axsl:apply-templates select="/" mode="M34"/><axsl:apply-templates select="/" mode="M35"/><axsl:apply-templates select="/" mode="M36"/><axsl:apply-templates select="/" mode="M37"/><axsl:apply-templates select="/" mode="M38"/><axsl:apply-templates select="/" mode="M39"/><axsl:apply-templates select="/" mode="M40"/><axsl:apply-templates select="/" mode="M41"/><axsl:apply-templates select="/" mode="M42"/><axsl:apply-templates select="/" mode="M43"/><axsl:apply-templates select="/" mode="M44"/><axsl:apply-templates select="/" mode="M45"/><axsl:apply-templates select="/" mode="M46"/><axsl:apply-templates select="/" mode="M47"/><axsl:apply-templates select="/" mode="M48"/><axsl:apply-templates select="/" mode="M49"/><axsl:apply-templates select="/" mode="M50"/><axsl:apply-templates select="/" mode="M51"/><axsl:apply-templates select="/" mode="M52"/><axsl:apply-templates select="/" mode="M53"/><axsl:apply-templates select="/" mode="M54"/><axsl:apply-templates select="/" mode="M55"/><axsl:apply-templates select="/" mode="M56"/><axsl:apply-templates select="/" mode="M57"/><axsl:apply-templates select="/" mode="M58"/><axsl:apply-templates select="/" mode="M59"/><axsl:apply-templates select="/" mode="M60"/><axsl:apply-templates select="/" mode="M61"/><axsl:apply-templates select="/" mode="M62"/><axsl:apply-templates select="/" mode="M63"/><axsl:apply-templates select="/" mode="M64"/><axsl:apply-templates select="/" mode="M65"/><axsl:apply-templates select="/" mode="M66"/><axsl:apply-templates select="/" mode="M67"/><axsl:apply-templates select="/" mode="M68"/><axsl:apply-templates select="/" mode="M69"/><axsl:apply-templates select="/" mode="M70"/><axsl:apply-templates select="/" mode="M71"/><axsl:apply-templates select="/" mode="M72"/><axsl:apply-templates select="/" mode="M73"/><axsl:apply-templates select="/" mode="M74"/><axsl:apply-templates select="/" mode="M75"/><axsl:apply-templates select="/" mode="M76"/><axsl:apply-templates select="/" mode="M77"/><axsl:apply-templates select="/" mode="M78"/><axsl:apply-templates select="/" mode="M79"/><axsl:apply-templates select="/" mode="M80"/><axsl:apply-templates select="/" mode="M81"/><axsl:apply-templates select="/" mode="M82"/><axsl:apply-templates select="/" mode="M83"/><axsl:apply-templates select="/" mode="M84"/><axsl:apply-templates select="/" mode="M85"/><axsl:apply-templates select="/" mode="M86"/><axsl:apply-templates select="/" mode="M87"/><axsl:apply-templates select="/" mode="M88"/><axsl:apply-templates select="/" mode="M89"/><axsl:apply-templates select="/" mode="M90"/><axsl:apply-templates select="/" mode="M91"/><axsl:apply-templates select="/" mode="M92"/><axsl:apply-templates select="/" mode="M93"/><axsl:apply-templates select="/" mode="M94"/><axsl:apply-templates select="/" mode="M95"/><axsl:apply-templates select="/" mode="M96"/><axsl:apply-templates select="/" mode="M97"/><axsl:apply-templates select="/" mode="M98"/><axsl:apply-templates select="/" mode="M99"/><axsl:apply-templates select="/" mode="M100"/><axsl:apply-templates select="/" mode="M101"/><axsl:apply-templates select="/" mode="M102"/><axsl:apply-templates select="/" mode="M103"/><axsl:apply-templates select="/" mode="M104"/><axsl:apply-templates select="/" mode="M105"/><axsl:apply-templates select="/" mode="M106"/><axsl:apply-templates select="/" mode="M107"/><axsl:apply-templates select="/" mode="M108"/><axsl:apply-templates select="/" mode="M109"/><axsl:apply-templates select="/" mode="M110"/><axsl:apply-templates select="/" mode="M111"/></axsl:template>

<!--SCHEMATRON PATTERNS-->


<!--PATTERN none_exist_value_dep-->


	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:tests/child::*" priority="1000" mode="M6">

		<!--REPORT -->
<axsl:if test="@check='none exist'"> DEPRECATED ATTRIBUTE VALUE IN: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ATTRIBUTE VALUE: <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M6"/></axsl:template><axsl:template match="text()" priority="-1" mode="M6"/><axsl:template match="@*|node()" priority="-2" mode="M6"><axsl:apply-templates select="@*|*" mode="M6"/></axsl:template>

<!--PATTERN system-->


	<!--RULE -->
<axsl:template match="oval-res:system[oval-res:tests]" priority="1001" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='full' or /oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='full'"/><axsl:otherwise>the tests element should not be included unless full results are to be provided (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:system[not(oval-res:tests)]" priority="1000" mode="M7">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='thin' and /oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='thin'"/><axsl:otherwise>the tests element should be included when full results are specified (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template><axsl:template match="text()" priority="-1" mode="M7"/><axsl:template match="@*|node()" priority="-2" mode="M7"><axsl:apply-templates select="@*|*" mode="M7"/></axsl:template>

<!--PATTERN directives-->


	<!--RULE -->
<axsl:template match="oval-res:definition[@result='true' and oval-res:criteria]" priority="1011" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='full'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='true' and not(oval-res:criteria)]" priority="1010" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should not be included (see directives) xx<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='thin'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='false' and oval-res:criteria]" priority="1009" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='full'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='false' and not(oval-res:criteria)]" priority="1008" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='thin'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='unknown' and oval-res:criteria]" priority="1007" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='full'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='unknown' and not(oval-res:criteria)]" priority="1006" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='thin'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='error' and oval-res:criteria]" priority="1005" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='full'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='error' and not(oval-res:criteria)]" priority="1004" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='thin'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not evaluated' and oval-res:criteria]" priority="1003" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='full'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not evaluated' and not(oval-res:criteria)]" priority="1002" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='thin'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not applicable' and oval-res:criteria]" priority="1001" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='full'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should contain THIN content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not applicable' and not(oval-res:criteria)]" priority="1000" mode="M8">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@reported='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should not be included (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='thin'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should contain FULL content (see directives)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template><axsl:template match="text()" priority="-1" mode="M8"/><axsl:template match="@*|node()" priority="-2" mode="M8"><axsl:apply-templates select="@*|*" mode="M8"/></axsl:template>

<!--PATTERN testids-->


	<!--RULE -->
<axsl:template match="oval-res:test" priority="1000" mode="M9">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@test_id = ../../oval-res:definitions//oval-res:criterion/@test_ref"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@test_id"/><axsl:text/> - the specific test is not used in any definition's criteria<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M9"/></axsl:template><axsl:template match="text()" priority="-1" mode="M9"/><axsl:template match="@*|node()" priority="-2" mode="M9"><axsl:apply-templates select="@*|*" mode="M9"/></axsl:template>

<!--PATTERN empty_def_doc-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions" priority="1000" mode="M10">

		<!--ASSERT -->
<axsl:choose><axsl:when test="oval-def:definitions or oval-def:tests or oval-def:objects or oval-def:states or oval-def:variables"/><axsl:otherwise>A valid OVAL Definition document must contain at least one definitions, tests, objects, states, or variables element. The optional definitions, tests, objects, states, and variables sections define the specific characteristics that should be evaluated on a system to determine the truth values of the OVAL Definition Document. To be valid though, at least one definitions, tests, objects, states, or variables element must be present.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M10"/></axsl:template><axsl:template match="text()" priority="-1" mode="M10"/><axsl:template match="@*|node()" priority="-2" mode="M10"><axsl:apply-templates select="@*|*" mode="M10"/></axsl:template>

<!--PATTERN required_criteria-->


	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:definitions/oval-def:definition[@deprecated='false' or not(@deprecated)]" priority="1000" mode="M11">

		<!--ASSERT -->
<axsl:choose><axsl:when test="oval-def:criteria"/><axsl:otherwise>A valid OVAL Definition document must contain a criteria unless the definition is a deprecated definition.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template><axsl:template match="text()" priority="-1" mode="M11"/><axsl:template match="@*|node()" priority="-2" mode="M11"><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template>

<!--PATTERN test_type-->


	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:tests/*[@check_existence='none_exist']" priority="1000" mode="M12">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(*[name()='state'])"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="@id"/><axsl:text/> - No state should be referenced when check_existence has a value of 'none_exist'.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M12"/></axsl:template><axsl:template match="text()" priority="-1" mode="M12"/><axsl:template match="@*|node()" priority="-2" mode="M12"><axsl:apply-templates select="@*|*" mode="M12"/></axsl:template>

<!--PATTERN setobjref-->


	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:object_reference" priority="1002" mode="M13">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template>

	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:object_reference" priority="1001" mode="M13">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template>

	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:set/oval-def:object_reference" priority="1000" mode="M13">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template><axsl:template match="text()" priority="-1" mode="M13"/><axsl:template match="@*|node()" priority="-2" mode="M13"><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template>

<!--PATTERN value-->


	<!--RULE -->
<axsl:template match="oval-def:constant_variable/oval-def:value" priority="1000" mode="M14">

		<!--ASSERT -->
<axsl:choose><axsl:when test=".!=''"/><axsl:otherwise>The value element of the constant_variable <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> can not be empty.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M14"/></axsl:template><axsl:template match="text()" priority="-1" mode="M14"/><axsl:template match="@*|node()" priority="-2" mode="M14"><axsl:apply-templates select="@*|*" mode="M14"/></axsl:template>

<!--PATTERN literal_component-->


	<!--RULE -->
<axsl:template match="oval-def:literal_component" priority="1000" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test=".!=''"/><axsl:otherwise>The literal_component of variable <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> can not be empty.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template><axsl:template match="text()" priority="-1" mode="M15"/><axsl:template match="@*|node()" priority="-2" mode="M15"><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

<!--PATTERN item_field-->


	<!--RULE -->
<axsl:template match="oval-def:object_component" priority="1000" mode="M16">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@item_field!=''"/><axsl:otherwise>The item_field attribute of an object_component of variable <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> can not be empty.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M16"/></axsl:template><axsl:template match="text()" priority="-1" mode="M16"/><axsl:template match="@*|node()" priority="-2" mode="M16"><axsl:apply-templates select="@*|*" mode="M16"/></axsl:template>

<!--PATTERN arithmeticfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:arithmetic/oval-def:literal_component" priority="1001" mode="M17">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='float' or @datatype='int'"/><axsl:otherwise>A literal_component used by an arithmetic function must have a datatype of float or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M17"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:arithmetic/oval-def:variable_component" priority="1000" mode="M17"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="//oval-def:variables/*[@id=$var_ref]/@datatype='float' or //oval-def:variables/*[@id=$var_ref]/@datatype='int'"/><axsl:otherwise>The variable referenced by the arithmetic function must have a datatype of float or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M17"/></axsl:template><axsl:template match="text()" priority="-1" mode="M17"/><axsl:template match="@*|node()" priority="-2" mode="M17"><axsl:apply-templates select="@*|*" mode="M17"/></axsl:template>

<!--PATTERN beginfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:begin/oval-def:literal_component" priority="1001" mode="M18">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the begin function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M18"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:begin/oval-def:variable_component" priority="1000" mode="M18"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="//oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the begin function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M18"/></axsl:template><axsl:template match="text()" priority="-1" mode="M18"/><axsl:template match="@*|node()" priority="-2" mode="M18"><axsl:apply-templates select="@*|*" mode="M18"/></axsl:template>

<!--PATTERN concatfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:concat/oval-def:literal_component" priority="1001" mode="M19">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the concat function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M19"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:concat/oval-def:variable_component" priority="1000" mode="M19"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="//oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the concat function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M19"/></axsl:template><axsl:template match="text()" priority="-1" mode="M19"/><axsl:template match="@*|node()" priority="-2" mode="M19"><axsl:apply-templates select="@*|*" mode="M19"/></axsl:template>

<!--PATTERN endfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:end/oval-def:literal_component" priority="1001" mode="M20">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the end function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:end/oval-def:variable_component" priority="1000" mode="M20"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="//oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the end function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template><axsl:template match="text()" priority="-1" mode="M20"/><axsl:template match="@*|node()" priority="-2" mode="M20"><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template>

<!--PATTERN escaperegexfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:escape_regex/oval-def:literal_component" priority="1001" mode="M21">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the escape_regex function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M21"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:escape_regex/oval-def:variable_component" priority="1000" mode="M21"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="//oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the escape_regex function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M21"/></axsl:template><axsl:template match="text()" priority="-1" mode="M21"/><axsl:template match="@*|node()" priority="-2" mode="M21"><axsl:apply-templates select="@*|*" mode="M21"/></axsl:template>

<!--PATTERN splitfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:split/oval-def:literal_component" priority="1001" mode="M22">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the split function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:split/oval-def:variable_component" priority="1000" mode="M22"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="//oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the split function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template><axsl:template match="text()" priority="-1" mode="M22"/><axsl:template match="@*|node()" priority="-2" mode="M22"><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template>

<!--PATTERN substringfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:substring/oval-def:literal_component" priority="1001" mode="M23">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the substring function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:substring/oval-def:variable_component" priority="1000" mode="M23"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="//oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the substring function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template><axsl:template match="text()" priority="-1" mode="M23"/><axsl:template match="@*|node()" priority="-2" mode="M23"><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template>

<!--PATTERN timedifferencefunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:time_difference/oval-def:literal_component" priority="1001" mode="M24">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string' or @datatype='int'"/><axsl:otherwise>A literal_component used by the time_difference function must have a datatype of string or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:time_difference/oval-def:variable_component" priority="1000" mode="M24"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="//oval-def:variables/*[@id=$var_ref]/@datatype='string' or //oval-def:variables/*[@id=$var_ref]/@datatype='int'"/><axsl:otherwise>The variable referenced by the time_difference function must have a datatype of string or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template><axsl:template match="text()" priority="-1" mode="M24"/><axsl:template match="@*|node()" priority="-2" mode="M24"><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template>

<!--PATTERN regexcapturefunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:regex_capture/oval-def:literal_component" priority="1001" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the regex_capture function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:regex_capture/oval-def:variable_component" priority="1000" mode="M25"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="//oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the regex_capture function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template><axsl:template match="text()" priority="-1" mode="M25"/><axsl:template match="@*|node()" priority="-2" mode="M25"><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template>

<!--PATTERN definition_entity_rules-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@var_ref]|oval-def:states/*/*[@var_ref]" priority="1010" mode="M26"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test=".=''"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so no value should be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="( (not(@datatype)) and ('string' = //oval-def:variables/*[@id=$var_ref]/@datatype) ) or (@datatype = //oval-def:variables/*[@id=$var_ref]/@datatype)"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="$var_ref"/><axsl:text/> - inconsistent datatype between the variable and an associated var_ref<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[not(@datatype)]|oval-def:states/*/*[not(@datatype)]" priority="1009" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='case insensitive equals' or @operation='case insensitive not equal' or @operation='pattern match'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given the lack of a declared datatype (hence a default datatype of string).<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='binary']|oval-def:states/*/*[@datatype='binary']" priority="1008" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of binary.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='boolean']|oval-def:states/*/*[@datatype='boolean']" priority="1007" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of boolean.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='evr_string']|oval-def:states/*/*[@datatype='evr_string']" priority="1006" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or  @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of evr_string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='fileset_revision']|oval-def:states/*/*[@datatype='fileset_revision']" priority="1005" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or  @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of fileset_revision.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='float']|oval-def:states/*/*[@datatype='float']" priority="1004" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of float.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='ios_version']|oval-def:states/*/*[@datatype='ios_version']" priority="1003" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of ios_version.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='int']|oval-def:states/*/*[@datatype='int']" priority="1002" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='bitwise and' or @operation='bitwise or'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='string']|oval-def:states/*/*[@datatype='string']" priority="1001" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='case insensitive equals' or @operation='case insensitive not equal' or @operation='pattern match'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='version']|oval-def:states/*/*[@datatype='version']" priority="1000" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of version.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template><axsl:template match="text()" priority="-1" mode="M26"/><axsl:template match="@*|node()" priority="-2" mode="M26"><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

<!--PATTERN entityobjectbaserules-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@var_ref]" priority="1001" mode="M27">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@var_check"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_check should also be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@var_check]" priority="1000" mode="M27">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@var_ref"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_check has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_ref should also be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template><axsl:template match="text()" priority="-1" mode="M27"/><axsl:template match="@*|node()" priority="-2" mode="M27"><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template>

<!--PATTERN entitystatebaserules-->


	<!--RULE -->
<axsl:template match="oval-def:states/*/*[@var_ref]" priority="1001" mode="M28">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@var_check"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_check should also be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:states/*/*[@var_check]" priority="1000" mode="M28">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@var_ref"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_check has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_ref should also be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template><axsl:template match="text()" priority="-1" mode="M28"/><axsl:template match="@*|node()" priority="-2" mode="M28"><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template>

<!--PATTERN sc_entity_rules-->


	<!--RULE -->
<axsl:template match="oval-sc:system_data/*/*" priority="1000" mode="M29">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@status) or @status='exists' or .=''"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a value for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity should only be supplied if the status attribute is 'exists'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@mask) or @mask='false' or .=''"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a value for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity should only be supplied if the mask attribute is 'false'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M29"/></axsl:template><axsl:template match="text()" priority="-1" mode="M29"/><axsl:template match="@*|node()" priority="-2" mode="M29"><axsl:apply-templates select="@*|*" mode="M29"/></axsl:template>

<!--PATTERN famtst-->


	<!--RULE -->
<axsl:template match="ind-def:family_test/ind-def:object" priority="1001" mode="M30">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:family_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a family_test must reference a family_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:family_test/ind-def:state" priority="1000" mode="M30">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:family_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a family_test must reference a family_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template><axsl:template match="text()" priority="-1" mode="M30"/><axsl:template match="@*|node()" priority="-2" mode="M30"><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template>

<!--PATTERN famstefamily-->


	<!--RULE -->
<axsl:template match="ind-def:family_state/ind-def:family" priority="1000" mode="M31">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the family entity of a family_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M31"/></axsl:template><axsl:template match="text()" priority="-1" mode="M31"/><axsl:template match="@*|node()" priority="-2" mode="M31"><axsl:apply-templates select="@*|*" mode="M31"/></axsl:template>

<!--PATTERN md5tst_dep-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_test" priority="1000" mode="M32">

		<!--REPORT -->
<axsl:if test="true()"> DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
            <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template><axsl:template match="text()" priority="-1" mode="M32"/><axsl:template match="@*|node()" priority="-2" mode="M32"><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

<!--PATTERN md5tst-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_test/ind-def:object" priority="1001" mode="M33">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:filemd5_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a filemd5_test must reference a filemd5_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:filemd5_test/ind-def:state" priority="1000" mode="M33">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:filemd5_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a filemd5_test must reference a filemd5_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template><axsl:template match="text()" priority="-1" mode="M33"/><axsl:template match="@*|node()" priority="-2" mode="M33"><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template>

<!--PATTERN md5obj_dep-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_object" priority="1000" mode="M34">

		<!--REPORT -->
<axsl:if test="true()"> DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
            <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M34"/></axsl:template><axsl:template match="text()" priority="-1" mode="M34"/><axsl:template match="@*|node()" priority="-2" mode="M34"><axsl:apply-templates select="@*|*" mode="M34"/></axsl:template>

<!--PATTERN md5objpath-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_object/ind-def:path" priority="1000" mode="M35">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filemd5_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M35"/></axsl:template><axsl:template match="text()" priority="-1" mode="M35"/><axsl:template match="@*|node()" priority="-2" mode="M35"><axsl:apply-templates select="@*|*" mode="M35"/></axsl:template>

<!--PATTERN md5objfilename-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_object/ind-def:filename" priority="1000" mode="M36">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filemd5_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M36"/></axsl:template><axsl:template match="text()" priority="-1" mode="M36"/><axsl:template match="@*|node()" priority="-2" mode="M36"><axsl:apply-templates select="@*|*" mode="M36"/></axsl:template>

<!--PATTERN md5ste_dep-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_state" priority="1000" mode="M37">

		<!--REPORT -->
<axsl:if test="true()"> DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
            <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M37"/></axsl:template><axsl:template match="text()" priority="-1" mode="M37"/><axsl:template match="@*|node()" priority="-2" mode="M37"><axsl:apply-templates select="@*|*" mode="M37"/></axsl:template>

<!--PATTERN md5stepath-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_state/ind-def:path" priority="1000" mode="M38">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filemd5_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M38"/></axsl:template><axsl:template match="text()" priority="-1" mode="M38"/><axsl:template match="@*|node()" priority="-2" mode="M38"><axsl:apply-templates select="@*|*" mode="M38"/></axsl:template>

<!--PATTERN md5stefilename-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_state/ind-def:filename" priority="1000" mode="M39">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filemd5_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M39"/></axsl:template><axsl:template match="text()" priority="-1" mode="M39"/><axsl:template match="@*|node()" priority="-2" mode="M39"><axsl:apply-templates select="@*|*" mode="M39"/></axsl:template>

<!--PATTERN md5stemd5-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_state/ind-def:md5" priority="1000" mode="M40">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the md5 entity of a filemd5_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template><axsl:template match="text()" priority="-1" mode="M40"/><axsl:template match="@*|node()" priority="-2" mode="M40"><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template>

<!--PATTERN hashtst-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_test/ind-def:object" priority="1001" mode="M41">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:filehash_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a filehash_test must reference a filesha1_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M41"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:filehash_test/ind-def:state" priority="1000" mode="M41">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:filehash_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a filehash_test must reference a filesha1_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M41"/></axsl:template><axsl:template match="text()" priority="-1" mode="M41"/><axsl:template match="@*|node()" priority="-2" mode="M41"><axsl:apply-templates select="@*|*" mode="M41"/></axsl:template>

<!--PATTERN hashobjfilepath-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_object/ind-def:filepath" priority="1000" mode="M42">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filepath entity of a filehash_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M42"/></axsl:template><axsl:template match="text()" priority="-1" mode="M42"/><axsl:template match="@*|node()" priority="-2" mode="M42"><axsl:apply-templates select="@*|*" mode="M42"/></axsl:template>

<!--PATTERN hashobjpath-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_object/ind-def:path" priority="1000" mode="M43">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filehash_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M43"/></axsl:template><axsl:template match="text()" priority="-1" mode="M43"/><axsl:template match="@*|node()" priority="-2" mode="M43"><axsl:apply-templates select="@*|*" mode="M43"/></axsl:template>

<!--PATTERN hashobjfilename-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_object/ind-def:filename" priority="1000" mode="M44">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filehash_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template><axsl:template match="text()" priority="-1" mode="M44"/><axsl:template match="@*|node()" priority="-2" mode="M44"><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template>

<!--PATTERN hashstefilepath-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state/ind-def:filepath" priority="1000" mode="M45">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filepath entity of a filehash_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M45"/></axsl:template><axsl:template match="text()" priority="-1" mode="M45"/><axsl:template match="@*|node()" priority="-2" mode="M45"><axsl:apply-templates select="@*|*" mode="M45"/></axsl:template>

<!--PATTERN hashstepath-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state/ind-def:path" priority="1000" mode="M46">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filehash_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M46"/></axsl:template><axsl:template match="text()" priority="-1" mode="M46"/><axsl:template match="@*|node()" priority="-2" mode="M46"><axsl:apply-templates select="@*|*" mode="M46"/></axsl:template>

<!--PATTERN hashstefilename-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state/ind-def:filename" priority="1000" mode="M47">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filehash_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M47"/></axsl:template><axsl:template match="text()" priority="-1" mode="M47"/><axsl:template match="@*|node()" priority="-2" mode="M47"><axsl:apply-templates select="@*|*" mode="M47"/></axsl:template>

<!--PATTERN hashstemd5-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state/ind-def:md5" priority="1000" mode="M48">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the md5 entity of a filehash_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M48"/></axsl:template><axsl:template match="text()" priority="-1" mode="M48"/><axsl:template match="@*|node()" priority="-2" mode="M48"><axsl:apply-templates select="@*|*" mode="M48"/></axsl:template>

<!--PATTERN hashstesha1-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state/ind-def:sha1" priority="1000" mode="M49">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sha1 entity of a filehash_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M49"/></axsl:template><axsl:template match="text()" priority="-1" mode="M49"/><axsl:template match="@*|node()" priority="-2" mode="M49"><axsl:apply-templates select="@*|*" mode="M49"/></axsl:template>

<!--PATTERN envtst-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_test/ind-def:object" priority="1001" mode="M50">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:environmentvariable_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an environmentvariable_test must reference a environmentvariable_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_test/ind-def:state" priority="1000" mode="M50">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:environmentvariable_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an environmentvariable_test must reference a environmentvariable_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template><axsl:template match="text()" priority="-1" mode="M50"/><axsl:template match="@*|node()" priority="-2" mode="M50"><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template>

<!--PATTERN envobjname-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_object/ind-def:name" priority="1000" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an environmentvariable_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template><axsl:template match="text()" priority="-1" mode="M51"/><axsl:template match="@*|node()" priority="-2" mode="M51"><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

<!--PATTERN envstename-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_state/ind-def:name" priority="1000" mode="M52">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an environmentvariable_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M52"/></axsl:template><axsl:template match="text()" priority="-1" mode="M52"/><axsl:template match="@*|node()" priority="-2" mode="M52"><axsl:apply-templates select="@*|*" mode="M52"/></axsl:template>

<!--PATTERN envstevalue-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_state/ind-def:value" priority="1000" mode="M53">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M53"/></axsl:template><axsl:template match="text()" priority="-1" mode="M53"/><axsl:template match="@*|node()" priority="-2" mode="M53"><axsl:apply-templates select="@*|*" mode="M53"/></axsl:template>

<!--PATTERN ldaptst-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_test/ind-def:object" priority="1001" mode="M54">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:ldap_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an ldap_test must reference an ldap_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:ldap_test/ind-def:state" priority="1000" mode="M54">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:ldap_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an ldap_test must reference an ldap_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template><axsl:template match="text()" priority="-1" mode="M54"/><axsl:template match="@*|node()" priority="-2" mode="M54"><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template>

<!--PATTERN ldapobjsuffix-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_object/ind-def:suffix" priority="1000" mode="M55">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the suffix entity of an ldap_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M55"/></axsl:template><axsl:template match="text()" priority="-1" mode="M55"/><axsl:template match="@*|node()" priority="-2" mode="M55"><axsl:apply-templates select="@*|*" mode="M55"/></axsl:template>

<!--PATTERN ldapobjrelative_dn-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_object/ind-def:relative_dn" priority="1000" mode="M56">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the relative_dn entity of an ldap_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M56"/></axsl:template><axsl:template match="text()" priority="-1" mode="M56"/><axsl:template match="@*|node()" priority="-2" mode="M56"><axsl:apply-templates select="@*|*" mode="M56"/></axsl:template>

<!--PATTERN ldapobjattribute-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_object/ind-def:attribute" priority="1000" mode="M57">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the attribute entity of an ldap_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M57"/></axsl:template><axsl:template match="text()" priority="-1" mode="M57"/><axsl:template match="@*|node()" priority="-2" mode="M57"><axsl:apply-templates select="@*|*" mode="M57"/></axsl:template>

<!--PATTERN ldapstesuffix-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_state/ind-def:suffix" priority="1000" mode="M58">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the suffix entity of an ldap_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M58"/></axsl:template><axsl:template match="text()" priority="-1" mode="M58"/><axsl:template match="@*|node()" priority="-2" mode="M58"><axsl:apply-templates select="@*|*" mode="M58"/></axsl:template>

<!--PATTERN ldapsterelative_dn-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_state/ind-def:relative_dn" priority="1000" mode="M59">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the relative_dn entity of an ldap_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M59"/></axsl:template><axsl:template match="text()" priority="-1" mode="M59"/><axsl:template match="@*|node()" priority="-2" mode="M59"><axsl:apply-templates select="@*|*" mode="M59"/></axsl:template>

<!--PATTERN ldapsteattribute-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_state/ind-def:attribute" priority="1000" mode="M60">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the attribute entity of an ldap_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M60"/></axsl:template><axsl:template match="text()" priority="-1" mode="M60"/><axsl:template match="@*|node()" priority="-2" mode="M60"><axsl:apply-templates select="@*|*" mode="M60"/></axsl:template>

<!--PATTERN ldapsteobject_class-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_state/ind-def:object_class" priority="1000" mode="M61">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the object_class entity of an ldap_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M61"/></axsl:template><axsl:template match="text()" priority="-1" mode="M61"/><axsl:template match="@*|node()" priority="-2" mode="M61"><axsl:apply-templates select="@*|*" mode="M61"/></axsl:template>

<!--PATTERN ldapsteldaptype-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_state/ind-def:ldaptype" priority="1000" mode="M62">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ldaptype entity of an ldap_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M62"/></axsl:template><axsl:template match="text()" priority="-1" mode="M62"/><axsl:template match="@*|node()" priority="-2" mode="M62"><axsl:apply-templates select="@*|*" mode="M62"/></axsl:template>

<!--PATTERN ldapstevalue-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_state/ind-def:value" priority="1000" mode="M63">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M63"/></axsl:template><axsl:template match="text()" priority="-1" mode="M63"/><axsl:template match="@*|node()" priority="-2" mode="M63"><axsl:apply-templates select="@*|*" mode="M63"/></axsl:template>

<!--PATTERN sqltst-->


	<!--RULE -->
<axsl:template match="ind-def:sql_test/ind-def:object" priority="1001" mode="M64">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:sql_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sql_test must reference a sql_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M64"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:sql_test/ind-def:state" priority="1000" mode="M64">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:sql_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sql_test must reference a sql_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M64"/></axsl:template><axsl:template match="text()" priority="-1" mode="M64"/><axsl:template match="@*|node()" priority="-2" mode="M64"><axsl:apply-templates select="@*|*" mode="M64"/></axsl:template>

<!--PATTERN sqlobjdengine-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:engine" priority="1000" mode="M65">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the engine entity of an sql_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the engine entity of an sql_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M65"/></axsl:template><axsl:template match="text()" priority="-1" mode="M65"/><axsl:template match="@*|node()" priority="-2" mode="M65"><axsl:apply-templates select="@*|*" mode="M65"/></axsl:template>

<!--PATTERN sqlobjversion-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:version" priority="1000" mode="M66">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of an sql_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the version entity of an sql_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M66"/></axsl:template><axsl:template match="text()" priority="-1" mode="M66"/><axsl:template match="@*|node()" priority="-2" mode="M66"><axsl:apply-templates select="@*|*" mode="M66"/></axsl:template>

<!--PATTERN sqlobjconnection_string-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:connection_string" priority="1000" mode="M67">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the connection_string entity of an sql_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the connection_string entity of an sql_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M67"/></axsl:template><axsl:template match="text()" priority="-1" mode="M67"/><axsl:template match="@*|node()" priority="-2" mode="M67"><axsl:apply-templates select="@*|*" mode="M67"/></axsl:template>

<!--PATTERN sqlobjsql-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:sql" priority="1000" mode="M68">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sql entity of a sql_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the sql entity of an sql_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M68"/></axsl:template><axsl:template match="text()" priority="-1" mode="M68"/><axsl:template match="@*|node()" priority="-2" mode="M68"><axsl:apply-templates select="@*|*" mode="M68"/></axsl:template>

<!--PATTERN sqlsteengine-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:engine" priority="1000" mode="M69">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the engine entity of an sql_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M69"/></axsl:template><axsl:template match="text()" priority="-1" mode="M69"/><axsl:template match="@*|node()" priority="-2" mode="M69"><axsl:apply-templates select="@*|*" mode="M69"/></axsl:template>

<!--PATTERN sqlsteversion-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:version" priority="1000" mode="M70">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of an sql_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M70"/></axsl:template><axsl:template match="text()" priority="-1" mode="M70"/><axsl:template match="@*|node()" priority="-2" mode="M70"><axsl:apply-templates select="@*|*" mode="M70"/></axsl:template>

<!--PATTERN sqlsteconnection_string-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:connection_string" priority="1000" mode="M71">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the connection_string entity of an sql_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M71"/></axsl:template><axsl:template match="text()" priority="-1" mode="M71"/><axsl:template match="@*|node()" priority="-2" mode="M71"><axsl:apply-templates select="@*|*" mode="M71"/></axsl:template>

<!--PATTERN sqlstesql-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:sql" priority="1000" mode="M72">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sql entity of a sql_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M72"/></axsl:template><axsl:template match="text()" priority="-1" mode="M72"/><axsl:template match="@*|node()" priority="-2" mode="M72"><axsl:apply-templates select="@*|*" mode="M72"/></axsl:template>

<!--PATTERN sqlsteresult-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:result" priority="1000" mode="M73">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M73"/></axsl:template><axsl:template match="text()" priority="-1" mode="M73"/><axsl:template match="@*|node()" priority="-2" mode="M73"><axsl:apply-templates select="@*|*" mode="M73"/></axsl:template>

<!--PATTERN txt54tst-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_test/ind-def:object" priority="1001" mode="M74">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:textfilecontent54_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a textfilecontent54_test must reference a textfilecontent54_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M74"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_test/ind-def:state" priority="1000" mode="M74">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:textfilecontent54_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a textfilecontent54_test must reference a textfilecontent54_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M74"/></axsl:template><axsl:template match="text()" priority="-1" mode="M74"/><axsl:template match="@*|node()" priority="-2" mode="M74"><axsl:apply-templates select="@*|*" mode="M74"/></axsl:template>

<!--PATTERN txt54objfilepath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:filepath" priority="1000" mode="M75">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filepath entity of a textfilecontent54_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M75"/></axsl:template><axsl:template match="text()" priority="-1" mode="M75"/><axsl:template match="@*|node()" priority="-2" mode="M75"><axsl:apply-templates select="@*|*" mode="M75"/></axsl:template>

<!--PATTERN txt54objpath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:path" priority="1000" mode="M76">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a textfilecontent54_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M76"/></axsl:template><axsl:template match="text()" priority="-1" mode="M76"/><axsl:template match="@*|node()" priority="-2" mode="M76"><axsl:apply-templates select="@*|*" mode="M76"/></axsl:template>

<!--PATTERN txt54objfilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:filename" priority="1000" mode="M77">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a textfilecontent54_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M77"/></axsl:template><axsl:template match="text()" priority="-1" mode="M77"/><axsl:template match="@*|node()" priority="-2" mode="M77"><axsl:apply-templates select="@*|*" mode="M77"/></axsl:template>

<!--PATTERN txt54objpattern-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:pattern" priority="1000" mode="M78">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pattern entity of a textfilecontent54_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="@operation='pattern match'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the pattern entity of a textfilecontent54_object should be 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M78"/></axsl:template><axsl:template match="text()" priority="-1" mode="M78"/><axsl:template match="@*|node()" priority="-2" mode="M78"><axsl:apply-templates select="@*|*" mode="M78"/></axsl:template>

<!--PATTERN txt54objinstance-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:instance" priority="1000" mode="M79">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the instance entity of a textfilecontent54_object should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M79"/></axsl:template><axsl:template match="text()" priority="-1" mode="M79"/><axsl:template match="@*|node()" priority="-2" mode="M79"><axsl:apply-templates select="@*|*" mode="M79"/></axsl:template>

<!--PATTERN txt54stefilepath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:filepath" priority="1000" mode="M80">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filepath entity of a textfilecontent54_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M80"/></axsl:template><axsl:template match="text()" priority="-1" mode="M80"/><axsl:template match="@*|node()" priority="-2" mode="M80"><axsl:apply-templates select="@*|*" mode="M80"/></axsl:template>

<!--PATTERN txt54stepath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:path" priority="1000" mode="M81">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a textfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M81"/></axsl:template><axsl:template match="text()" priority="-1" mode="M81"/><axsl:template match="@*|node()" priority="-2" mode="M81"><axsl:apply-templates select="@*|*" mode="M81"/></axsl:template>

<!--PATTERN txt54stefilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:filename" priority="1000" mode="M82">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a textfilecontent54_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M82"/></axsl:template><axsl:template match="text()" priority="-1" mode="M82"/><axsl:template match="@*|node()" priority="-2" mode="M82"><axsl:apply-templates select="@*|*" mode="M82"/></axsl:template>

<!--PATTERN txt54stepattern-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:pattern" priority="1000" mode="M83">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pattern entity of a textfilecontent54_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M83"/></axsl:template><axsl:template match="text()" priority="-1" mode="M83"/><axsl:template match="@*|node()" priority="-2" mode="M83"><axsl:apply-templates select="@*|*" mode="M83"/></axsl:template>

<!--PATTERN txt54steinstance-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:instance" priority="1000" mode="M84">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the instance entity of a textfilecontent54_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M84"/></axsl:template><axsl:template match="text()" priority="-1" mode="M84"/><axsl:template match="@*|node()" priority="-2" mode="M84"><axsl:apply-templates select="@*|*" mode="M84"/></axsl:template>

<!--PATTERN txt54stetext-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:text" priority="1000" mode="M85">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the text entity of a textfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M85"/></axsl:template><axsl:template match="text()" priority="-1" mode="M85"/><axsl:template match="@*|node()" priority="-2" mode="M85"><axsl:apply-templates select="@*|*" mode="M85"/></axsl:template>

<!--PATTERN txt54stesubexpression-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:subexpression" priority="1000" mode="M86">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M86"/></axsl:template><axsl:template match="text()" priority="-1" mode="M86"/><axsl:template match="@*|node()" priority="-2" mode="M86"><axsl:apply-templates select="@*|*" mode="M86"/></axsl:template>

<!--PATTERN txttst_dep-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_test" priority="1000" mode="M87">

		<!--REPORT -->
<axsl:if test="true()"> DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
            <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M87"/></axsl:template><axsl:template match="text()" priority="-1" mode="M87"/><axsl:template match="@*|node()" priority="-2" mode="M87"><axsl:apply-templates select="@*|*" mode="M87"/></axsl:template>

<!--PATTERN txttst-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_test/ind-def:object" priority="1001" mode="M88">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:textfilecontent_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a textfilecontent_test must reference a textfilecontent_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M88"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_test/ind-def:state" priority="1000" mode="M88">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:textfilecontent_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a textfilecontent_test must reference a textfilecontent_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M88"/></axsl:template><axsl:template match="text()" priority="-1" mode="M88"/><axsl:template match="@*|node()" priority="-2" mode="M88"><axsl:apply-templates select="@*|*" mode="M88"/></axsl:template>

<!--PATTERN txtobj_dep-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object" priority="1000" mode="M89">

		<!--REPORT -->
<axsl:if test="true()"> DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
            <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M89"/></axsl:template><axsl:template match="text()" priority="-1" mode="M89"/><axsl:template match="@*|node()" priority="-2" mode="M89"><axsl:apply-templates select="@*|*" mode="M89"/></axsl:template>

<!--PATTERN txtobjpath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object/ind-def:path" priority="1000" mode="M90">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a textfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M90"/></axsl:template><axsl:template match="text()" priority="-1" mode="M90"/><axsl:template match="@*|node()" priority="-2" mode="M90"><axsl:apply-templates select="@*|*" mode="M90"/></axsl:template>

<!--PATTERN txtobjfilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object/ind-def:filename" priority="1000" mode="M91">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a textfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M91"/></axsl:template><axsl:template match="text()" priority="-1" mode="M91"/><axsl:template match="@*|node()" priority="-2" mode="M91"><axsl:apply-templates select="@*|*" mode="M91"/></axsl:template>

<!--PATTERN txtobjline-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object/ind-def:line" priority="1000" mode="M92">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the line entity of a textfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="@operation='pattern match'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the line entity of a textfilecontent_object should be 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M92"/></axsl:template><axsl:template match="text()" priority="-1" mode="M92"/><axsl:template match="@*|node()" priority="-2" mode="M92"><axsl:apply-templates select="@*|*" mode="M92"/></axsl:template>

<!--PATTERN txtste_dep-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state" priority="1000" mode="M93">

		<!--REPORT -->
<axsl:if test="true()"> DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
            <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M93"/></axsl:template><axsl:template match="text()" priority="-1" mode="M93"/><axsl:template match="@*|node()" priority="-2" mode="M93"><axsl:apply-templates select="@*|*" mode="M93"/></axsl:template>

<!--PATTERN txtstepath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state/ind-def:path" priority="1000" mode="M94">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a textfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M94"/></axsl:template><axsl:template match="text()" priority="-1" mode="M94"/><axsl:template match="@*|node()" priority="-2" mode="M94"><axsl:apply-templates select="@*|*" mode="M94"/></axsl:template>

<!--PATTERN txtstefilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state/ind-def:filename" priority="1000" mode="M95">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a textfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M95"/></axsl:template><axsl:template match="text()" priority="-1" mode="M95"/><axsl:template match="@*|node()" priority="-2" mode="M95"><axsl:apply-templates select="@*|*" mode="M95"/></axsl:template>

<!--PATTERN txtsteline-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state/ind-def:line" priority="1000" mode="M96">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the line entity of a textfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M96"/></axsl:template><axsl:template match="text()" priority="-1" mode="M96"/><axsl:template match="@*|node()" priority="-2" mode="M96"><axsl:apply-templates select="@*|*" mode="M96"/></axsl:template>

<!--PATTERN txtstesubexpression-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state/ind-def:subexpression" priority="1000" mode="M97">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M97"/></axsl:template><axsl:template match="text()" priority="-1" mode="M97"/><axsl:template match="@*|node()" priority="-2" mode="M97"><axsl:apply-templates select="@*|*" mode="M97"/></axsl:template>

<!--PATTERN vattst-->


	<!--RULE -->
<axsl:template match="ind-def:variable_test/ind-def:object" priority="1001" mode="M98">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:variable_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a variable_test must reference a variable_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M98"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:variable_test/ind-def:state" priority="1000" mode="M98">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:variable_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a variable_test must reference a variable_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M98"/></axsl:template><axsl:template match="text()" priority="-1" mode="M98"/><axsl:template match="@*|node()" priority="-2" mode="M98"><axsl:apply-templates select="@*|*" mode="M98"/></axsl:template>

<!--PATTERN varobjvar_ref-->


	<!--RULE -->
<axsl:template match="ind-def:variable_object/ind-def:var_ref" priority="1000" mode="M99">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the var_ref entity of a variable_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@var_ref)"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - var_ref attribute for the var_ref entity of a variable_object is prohibited.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M99"/></axsl:template><axsl:template match="text()" priority="-1" mode="M99"/><axsl:template match="@*|node()" priority="-2" mode="M99"><axsl:apply-templates select="@*|*" mode="M99"/></axsl:template>

<!--PATTERN varstevar_ref-->


	<!--RULE -->
<axsl:template match="ind-def:variable_state/ind-def:var_ref" priority="1000" mode="M100">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the var_ref entity of a variable_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@var_ref)"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - var_ref attribute for the var_ref entity of a variable_state is prohibited.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M100"/></axsl:template><axsl:template match="text()" priority="-1" mode="M100"/><axsl:template match="@*|node()" priority="-2" mode="M100"><axsl:apply-templates select="@*|*" mode="M100"/></axsl:template>

<!--PATTERN varstevalue-->


	<!--RULE -->
<axsl:template match="ind-def:variable_state/ind-def:value" priority="1000" mode="M101">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M101"/></axsl:template><axsl:template match="text()" priority="-1" mode="M101"/><axsl:template match="@*|node()" priority="-2" mode="M101"><axsl:apply-templates select="@*|*" mode="M101"/></axsl:template>

<!--PATTERN xmltst-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_test/ind-def:object" priority="1001" mode="M102">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:xmlfilecontent_object/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a xmlfilecontent_test must reference a xmlfilecontent_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M102"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_test/ind-def:state" priority="1000" mode="M102">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:xmlfilecontent_state/@id"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a xmlfilecontent_test must reference a xmlfilecontent_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M102"/></axsl:template><axsl:template match="text()" priority="-1" mode="M102"/><axsl:template match="@*|node()" priority="-2" mode="M102"><axsl:apply-templates select="@*|*" mode="M102"/></axsl:template>

<!--PATTERN xmlobjfilepath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:filepath" priority="1000" mode="M103">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filepath entity of a xmlfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M103"/></axsl:template><axsl:template match="text()" priority="-1" mode="M103"/><axsl:template match="@*|node()" priority="-2" mode="M103"><axsl:apply-templates select="@*|*" mode="M103"/></axsl:template>

<!--PATTERN xmlobjpath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:path" priority="1000" mode="M104">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a xmlfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M104"/></axsl:template><axsl:template match="text()" priority="-1" mode="M104"/><axsl:template match="@*|node()" priority="-2" mode="M104"><axsl:apply-templates select="@*|*" mode="M104"/></axsl:template>

<!--PATTERN xmlobjfilename-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:filename" priority="1000" mode="M105">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a xmlfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M105"/></axsl:template><axsl:template match="text()" priority="-1" mode="M105"/><axsl:template match="@*|node()" priority="-2" mode="M105"><axsl:apply-templates select="@*|*" mode="M105"/></axsl:template>

<!--PATTERN xmlobjxpath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:xpath" priority="1000" mode="M106">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the xpath entity of a xmlfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the xpath entity of a xmlfilecontent_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M106"/></axsl:template><axsl:template match="text()" priority="-1" mode="M106"/><axsl:template match="@*|node()" priority="-2" mode="M106"><axsl:apply-templates select="@*|*" mode="M106"/></axsl:template>

<!--PATTERN xmlfilestefilepath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_state/ind-def:filepath" priority="1000" mode="M107">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filepath entity of a xmlfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M107"/></axsl:template><axsl:template match="text()" priority="-1" mode="M107"/><axsl:template match="@*|node()" priority="-2" mode="M107"><axsl:apply-templates select="@*|*" mode="M107"/></axsl:template>

<!--PATTERN xmlstepath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_state/ind-def:path" priority="1000" mode="M108">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a xmlfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M108"/></axsl:template><axsl:template match="text()" priority="-1" mode="M108"/><axsl:template match="@*|node()" priority="-2" mode="M108"><axsl:apply-templates select="@*|*" mode="M108"/></axsl:template>

<!--PATTERN xmlstefilename-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_state/ind-def:filename" priority="1000" mode="M109">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a xmlfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M109"/></axsl:template><axsl:template match="text()" priority="-1" mode="M109"/><axsl:template match="@*|node()" priority="-2" mode="M109"><axsl:apply-templates select="@*|*" mode="M109"/></axsl:template>

<!--PATTERN xmlstexpath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_state/ind-def:xpath" priority="1000" mode="M110">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the xpath entity of a xmlfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M110"/></axsl:template><axsl:template match="text()" priority="-1" mode="M110"/><axsl:template match="@*|node()" priority="-2" mode="M110"><axsl:apply-templates select="@*|*" mode="M110"/></axsl:template>

<!--PATTERN xmlstevalue_of-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_state/ind-def:value_of" priority="1000" mode="M111">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the value_of entity of a xmlfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M111"/></axsl:template><axsl:template match="text()" priority="-1" mode="M111"/><axsl:template match="@*|node()" priority="-2" mode="M111"><axsl:apply-templates select="@*|*" mode="M111"/></axsl:template></axsl:stylesheet>
