<?xml version="1.0" standalone="yes"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:iso="http://purl.oclc.org/dsdl/schematron" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:oval-dir="http://oval.mitre.org/XMLSchema/oval-directives-5" xmlns:oval-res="http://oval.mitre.org/XMLSchema/oval-results-5" xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5" version="1.0"><!--Implementers: please note that overriding process-prolog or process-root is 
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
<axsl:template match="/"><axsl:apply-templates select="/" mode="M11"/><axsl:apply-templates select="/" mode="M12"/><axsl:apply-templates select="/" mode="M13"/><axsl:apply-templates select="/" mode="M14"/><axsl:apply-templates select="/" mode="M15"/><axsl:apply-templates select="/" mode="M16"/><axsl:apply-templates select="/" mode="M17"/><axsl:apply-templates select="/" mode="M18"/><axsl:apply-templates select="/" mode="M19"/><axsl:apply-templates select="/" mode="M20"/><axsl:apply-templates select="/" mode="M21"/><axsl:apply-templates select="/" mode="M22"/><axsl:apply-templates select="/" mode="M23"/><axsl:apply-templates select="/" mode="M24"/><axsl:apply-templates select="/" mode="M25"/><axsl:apply-templates select="/" mode="M26"/><axsl:apply-templates select="/" mode="M27"/><axsl:apply-templates select="/" mode="M28"/><axsl:apply-templates select="/" mode="M29"/><axsl:apply-templates select="/" mode="M30"/><axsl:apply-templates select="/" mode="M31"/><axsl:apply-templates select="/" mode="M32"/><axsl:apply-templates select="/" mode="M33"/><axsl:apply-templates select="/" mode="M34"/><axsl:apply-templates select="/" mode="M35"/></axsl:template>

<!--SCHEMATRON PATTERNS-->


<!--PATTERN oval_none_exist_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:tests/child::*" priority="1000" mode="M11">

		<!--REPORT -->
<axsl:if test="@check='none exist'">
                                             DEPRECATED ATTRIBUTE VALUE IN: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ATTRIBUTE VALUE:
                                        <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template><axsl:template match="text()" priority="-1" mode="M11"/><axsl:template match="@*|node()" priority="-2" mode="M11"><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template>

<!--PATTERN oval-res_directives_include_oval_definitions-->


	<!--RULE -->
<axsl:template match="oval-res:oval_results/oval-res:directives[@include_source_definitions='true' or @include_source_definitions='1' or not(@include_source_definitions)]" priority="1001" mode="M12">

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-res:oval_results[oval-def:oval_definitions]"/><axsl:otherwise>
                                                  The source OVAL Definition document must be included when the directives include_source_definitions attribute is set to true.
                                             <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M12"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:oval_results/oval-res:directives[@include_source_definitions='false' or @include_source_definitions='0']" priority="1000" mode="M12">

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-res:oval_results[not(oval-def:oval_definitions)]"/><axsl:otherwise>
                                                  The source OVAL Definition document must not be included when the directives include_source_definitions attribute is set to false.
                                             <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M12"/></axsl:template><axsl:template match="text()" priority="-1" mode="M12"/><axsl:template match="@*|node()" priority="-2" mode="M12"><axsl:apply-templates select="@*|*" mode="M12"/></axsl:template>

<!--PATTERN oval-res_system-->


	<!--RULE -->
<axsl:template match="oval-res:system[oval-res:tests]" priority="1001" mode="M13">

		<!--ASSERT -->
<axsl:choose><axsl:when test="/oval-res:oval_results/oval-res:directives/*[@reported='true' or @reported='1']/@content='full'                                                 or /oval-res:oval_results/oval-res:directives/*[(@reported='true' or @reported='1') and not(@content)]                                                 or /oval-res:oval_results/oval-res:class_directives/*[@reported='true' or @reported='1']/@content='full'                                                 or /oval-res:oval_results/oval-res:class_directives/*[(@reported='true' or @reported='1') and not(@content)]"/><axsl:otherwise>
                                   The tests element should not be included unless full results are to be provided (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:system[not(oval-res:tests)]" priority="1000" mode="M13">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(oval-res:oval_results/oval-res:directives/*[@reported='true' or @reported='1']/@content='full')                                                  and not(/oval-res:oval_results/oval-res:directives/*[(@reported='true' or @reported='1') and not(@content)])                                                 and not(/oval-res:oval_results/oval-res:class_directives/*[@reported='true' or @reported='1']/@content='full')                                                 and not(/oval-res:oval_results/oval-res:class_directives/*[(@reported='true' or @reported='1') and not(@content)])"/><axsl:otherwise>
                                   The tests element should be included when full results are specified (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template><axsl:template match="text()" priority="-1" mode="M13"/><axsl:template match="@*|node()" priority="-2" mode="M13"><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template>

<!--PATTERN oval-res_mask_rule-->


	<!--RULE -->
<axsl:template match="/oval-res:oval_results/oval-res:results/oval-res:system/oval-sc:oval_system_characteristics/oval-sc:system_data/*/*|/oval-res:oval_results/oval-res:results/oval-res:system/oval-sc:oval_system_characteristics/oval-sc:system_data/*/*/*" priority="1000" mode="M14">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@mask) or @mask='false' or @mask='0' or .=''"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a value for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity should only be supplied if the mask attribute is 'false'.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M14"/></axsl:template><axsl:template match="text()" priority="-1" mode="M14"/><axsl:template match="@*|node()" priority="-2" mode="M14"><axsl:apply-templates select="@*|*" mode="M14"/></axsl:template>

<!--PATTERN oval-res_directives-->


	<!--RULE -->
<axsl:template match="oval-res:definition[@result='true' and oval-res:criteria]" priority="1011" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_true/@reported='1')                                    and not(oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_true/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_true/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='full')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_true/@content='full')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain THIN content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='true' and not(oval-res:criteria)]" priority="1010" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_true/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_true/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_true/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_true/@content='thin')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_true/@content='thin')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of TRUE should contain FULL content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='false' and oval-res:criteria]" priority="1009" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_false/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_false/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_false/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='full')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_false/@content='full')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should contain THIN content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='false' and not(oval-res:criteria)]" priority="1008" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_false/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_false/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_false/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_false/@content='thin')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_false/@content='thin')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of FALSE should contain FULL content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='unknown' and oval-res:criteria]" priority="1007" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_unknown/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_unknown/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='full')                                    and not(oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_unknown/@content='full')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should contain THIN content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='unknown' and not(oval-res:criteria)]" priority="1006" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_unknown/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_unknown/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_unknown/@content='thin')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_unknown/@content='thin')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of UNKNOWN should contain FULL content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='error' and oval-res:criteria]" priority="1005" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_error/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_error/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_error/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='full')                                    and not(oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_error/@content='full')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should contain THIN content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='error' and not(oval-res:criteria)]" priority="1004" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_error/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_unknown/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_unknown/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_error/@content='thin')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_error/@content='thin')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of ERROR should contain FULL content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not evaluated' and oval-res:criteria]" priority="1003" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_evaluated/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_evaluated/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='full')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_evaluated/@content='full')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should contain THIN content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not evaluated' and not(oval-res:criteria)]" priority="1002" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_evaluated/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_evaluated/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_not_evaluated/@content='thin')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_evaluated/@content='thin')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT EVALUATED should contain FULL content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not applicable' and oval-res:criteria]" priority="1001" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_applicable/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_applicable/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='full')                                    and not(oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_applicable/@content='full')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should contain THIN content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-res:definition[@result='not applicable' and not(oval-res:criteria)]" priority="1000" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@reported='true' or /oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@reported='1')                                    and not(/oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_applicable/@reported='true' or /oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_applicable/@reported='1')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should not be included (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((/oval-res:oval_results/oval-res:directives/oval-res:definition_not_applicable/@content='thin')                                    and not(oval-res:oval_results/oval-res:class_directives[@class = ./@class]))                                    or (/oval-res:oval_results/oval-res:class_directives[@class = ./@class]/oval-res:definition_not_applicable/@content='thin')"/><axsl:otherwise>
                                   <axsl:text/><axsl:value-of select="@definition_id"/><axsl:text/> - definitions with a result of NOT APPLICABLE should contain FULL content (see directives)
                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template><axsl:template match="text()" priority="-1" mode="M15"/><axsl:template match="@*|node()" priority="-2" mode="M15"><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

<!--PATTERN oval-res_testids-->


	<!--RULE -->
<axsl:template match="oval-res:test" priority="1000" mode="M16">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@test_id = ../../oval-res:definitions//oval-res:criterion/@test_ref"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="@test_id"/><axsl:text/> - the specified test is not used in any definition's criteria<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M16"/></axsl:template><axsl:template match="text()" priority="-1" mode="M16"/><axsl:template match="@*|node()" priority="-2" mode="M16"><axsl:apply-templates select="@*|*" mode="M16"/></axsl:template>

<!--PATTERN oval-def_empty_def_doc-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions" priority="1000" mode="M17">

		<!--ASSERT -->
<axsl:choose><axsl:when test="oval-def:definitions or oval-def:tests or oval-def:objects or oval-def:states or oval-def:variables"/><axsl:otherwise>A valid OVAL Definition document must contain at least one definitions, tests, objects, states, or variables element. The optional definitions, tests, objects, states, and variables sections define the specific characteristics that should be evaluated on a system to determine the truth values of the OVAL Definition Document. To be valid though, at least one definitions, tests, objects, states, or variables element must be present.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M17"/></axsl:template><axsl:template match="text()" priority="-1" mode="M17"/><axsl:template match="@*|node()" priority="-2" mode="M17"><axsl:apply-templates select="@*|*" mode="M17"/></axsl:template>

<!--PATTERN oval-def_required_criteria-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:definitions/oval-def:definition[(@deprecated='false' or @deprecated='0') or not(@deprecated)]" priority="1000" mode="M18">

		<!--ASSERT -->
<axsl:choose><axsl:when test="oval-def:criteria"/><axsl:otherwise>A valid OVAL Definition must contain a criteria unless the definition is a deprecated definition.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M18"/></axsl:template><axsl:template match="text()" priority="-1" mode="M18"/><axsl:template match="@*|node()" priority="-2" mode="M18"><axsl:apply-templates select="@*|*" mode="M18"/></axsl:template>

<!--PATTERN oval-def_test_type-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:tests/*[@check_existence='none_exist']" priority="1000" mode="M19">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(*[local-name()='state'])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="@id"/><axsl:text/> - No state should be referenced when check_existence has a value of 'none_exist'.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M19"/></axsl:template><axsl:template match="text()" priority="-1" mode="M19"/><axsl:template match="@*|node()" priority="-2" mode="M19"><axsl:apply-templates select="@*|*" mode="M19"/></axsl:template>

<!--PATTERN oval-def_setobjref-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:object_reference" priority="1002" mode="M20">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../..) = name(ancestor::oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:object_reference" priority="1001" mode="M20">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../..) = name(ancestor::oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:set/oval-def:object_reference" priority="1000" mode="M20">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../../..) = name(ancestor::oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template><axsl:template match="text()" priority="-1" mode="M20"/><axsl:template match="@*|node()" priority="-2" mode="M20"><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template>

<!--PATTERN oval-def_literal_component-->


	<!--RULE -->
<axsl:template match="oval-def:literal_component" priority="1000" mode="M21">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype='record')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="ancestor::*/@id"/><axsl:text/> - The 'record' datatype is prohibited on variables.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M21"/></axsl:template><axsl:template match="text()" priority="-1" mode="M21"/><axsl:template match="@*|node()" priority="-2" mode="M21"><axsl:apply-templates select="@*|*" mode="M21"/></axsl:template>

<!--PATTERN oval-def_arithmeticfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:arithmetic/oval-def:literal_component" priority="1001" mode="M22">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='float' or @datatype='int'"/><axsl:otherwise>A literal_component used by an arithmetic function must have a datatype of float or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:arithmetic/oval-def:variable_component" priority="1000" mode="M22"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype='float' or ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype='int'"/><axsl:otherwise>The variable referenced by the arithmetic function must have a datatype of float or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template><axsl:template match="text()" priority="-1" mode="M22"/><axsl:template match="@*|node()" priority="-2" mode="M22"><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template>

<!--PATTERN oval-def_beginfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:begin/oval-def:literal_component" priority="1001" mode="M23">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the begin function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:begin/oval-def:variable_component" priority="1000" mode="M23"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the begin function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template><axsl:template match="text()" priority="-1" mode="M23"/><axsl:template match="@*|node()" priority="-2" mode="M23"><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template>

<!--PATTERN oval-def_concatfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:concat/oval-def:literal_component" priority="1001" mode="M24">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the concat function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:concat/oval-def:variable_component" priority="1000" mode="M24"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the concat function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template><axsl:template match="text()" priority="-1" mode="M24"/><axsl:template match="@*|node()" priority="-2" mode="M24"><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template>

<!--PATTERN oval-def_endfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:end/oval-def:literal_component" priority="1001" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the end function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:end/oval-def:variable_component" priority="1000" mode="M25"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the end function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template><axsl:template match="text()" priority="-1" mode="M25"/><axsl:template match="@*|node()" priority="-2" mode="M25"><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template>

<!--PATTERN oval-def_escaperegexfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:escape_regex/oval-def:literal_component" priority="1001" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the escape_regex function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:escape_regex/oval-def:variable_component" priority="1000" mode="M26"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the escape_regex function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template><axsl:template match="text()" priority="-1" mode="M26"/><axsl:template match="@*|node()" priority="-2" mode="M26"><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

<!--PATTERN oval-def_splitfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:split/oval-def:literal_component" priority="1001" mode="M27">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the split function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:split/oval-def:variable_component" priority="1000" mode="M27"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the split function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template><axsl:template match="text()" priority="-1" mode="M27"/><axsl:template match="@*|node()" priority="-2" mode="M27"><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template>

<!--PATTERN oval-def_substringfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:substring/oval-def:literal_component" priority="1001" mode="M28">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the substring function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:substring/oval-def:variable_component" priority="1000" mode="M28"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the substring function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template><axsl:template match="text()" priority="-1" mode="M28"/><axsl:template match="@*|node()" priority="-2" mode="M28"><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template>

<!--PATTERN oval-def_timedifferencefunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:time_difference/oval-def:literal_component" priority="1001" mode="M29">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string' or @datatype='int'"/><axsl:otherwise>A literal_component used by the time_difference function must have a datatype of string or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M29"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:time_difference/oval-def:variable_component" priority="1000" mode="M29"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype='string' or ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype='int'"/><axsl:otherwise>The variable referenced by the time_difference function must have a datatype of string or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M29"/></axsl:template><axsl:template match="text()" priority="-1" mode="M29"/><axsl:template match="@*|node()" priority="-2" mode="M29"><axsl:apply-templates select="@*|*" mode="M29"/></axsl:template>

<!--PATTERN oval-def_regexcapturefunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:regex_capture/oval-def:literal_component" priority="1001" mode="M30">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the regex_capture function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:regex_capture/oval-def:variable_component" priority="1000" mode="M30"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the regex_capture function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template><axsl:template match="text()" priority="-1" mode="M30"/><axsl:template match="@*|node()" priority="-2" mode="M30"><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template>

<!--PATTERN oval-def_globtoregexfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:glob_to_regex/oval-def:literal_component" priority="1001" mode="M31">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the glob_to_regex function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M31"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:glob_to_regex/oval-def:variable_component" priority="1000" mode="M31"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the glob_to_regex function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M31"/></axsl:template><axsl:template match="text()" priority="-1" mode="M31"/><axsl:template match="@*|node()" priority="-2" mode="M31"><axsl:apply-templates select="@*|*" mode="M31"/></axsl:template>

<!--PATTERN oval-def_definition_entity_rules-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@var_ref]|oval-def:objects/*/*/*[@var_ref]|oval-def:states/*/*[@var_ref]|oval-def:states/*/*/*[@var_ref]" priority="1017" mode="M32"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test=".=''"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so no value should be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="( (not(@datatype)) and ('string' = ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype) ) or (@datatype = ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype)"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="$var_ref"/><axsl:text/> - inconsistent datatype between the variable and an associated var_ref<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@var_ref]|oval-def:objects/*/*/*[@var_ref]" priority="1016" mode="M32">

		<!--REPORT -->
<axsl:if test="not(@var_check)">
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_check should also be provided<axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@var_check]|oval-def:objects/*/*/*[@var_check]" priority="1015" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@var_ref"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_check has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_ref must also be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:states/*/*[@var_ref]|oval-def:states/*/*/*[@var_ref]" priority="1014" mode="M32">

		<!--REPORT -->
<axsl:if test="not(@var_check)">
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_check should also be provided<axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:states/*/*[@var_check]|oval-def:states/*/*/*[@var_check]" priority="1013" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@var_ref"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_check has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_ref must also be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[not(@datatype)]|oval-def:objects/*/*/*[not(@datatype)]|oval-def:states/*/*[not(@datatype)]|oval-def:states/*/*/*[not(@datatype)]" priority="1012" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='case insensitive equals' or @operation='case insensitive not equal' or @operation='pattern match'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given the lack of a declared datatype (hence a default datatype of string).<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='binary']|oval-def:objects/*/*/*[@datatype='binary']|oval-def:states/*/*[@datatype='binary']|oval-def:states/*/*/*[@datatype='binary']" priority="1011" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of binary.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='boolean']|oval-def:objects/*/*/*[@datatype='boolean']|oval-def:states/*/*[@datatype='boolean']|oval-def:states/*/*/*[@datatype='boolean']" priority="1010" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of boolean.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='evr_string']|oval-def:objects/*/*/*[@datatype='evr_string']|oval-def:states/*/*[@datatype='evr_string']|oval-def:states/*/*/*[@datatype='evr_string']" priority="1009" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or  @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of evr_string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='fileset_revision']|oval-def:objects/*/*/*[@datatype='fileset_revision']|oval-def:states/*/*[@datatype='fileset_revision']|oval-def:states/*/*/*[@datatype='fileset_revision']" priority="1008" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or  @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of fileset_revision.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='float']|oval-def:objects/*/*/*[@datatype='float']|oval-def:states/*/*[@datatype='float']|oval-def:states/*/*/*[@datatype='float']" priority="1007" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of float.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='ios_version']|oval-def:objects/*/*/*[@datatype='ios_version']|oval-def:states/*/*[@datatype='ios_version']|oval-def:states/*/*/*[@datatype='ios_version']" priority="1006" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of ios_version.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='int']|oval-def:objects/*/*/*[@datatype='int']|oval-def:states/*/*[@datatype='int']|oval-def:states/*/*/*[@datatype='int']" priority="1005" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='bitwise and' or @operation='bitwise or'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='ipv4_address']|oval-def:objects/*/*/*[@datatype='ipv4_address']|oval-def:states/*/*[@datatype='ipv4_address']|oval-def:states/*/*/*[@datatype='ipv4_address']" priority="1004" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='subset of' or @operation='superset of'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of ipv4_address.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='ipv6_address']|oval-def:objects/*/*/*[@datatype='ipv6_address']|oval-def:states/*/*[@datatype='ipv6_address']|oval-def:states/*/*/*[@datatype='ipv6_address']" priority="1003" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='subset of' or @operation='superset of'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of ipv6_address.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='string']|oval-def:objects/*/*/*[@datatype='string']|oval-def:states/*/*[@datatype='string']|oval-def:states/*/*/*[@datatype='string']" priority="1002" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='case insensitive equals' or @operation='case insensitive not equal' or @operation='pattern match'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='version']|oval-def:objects/*/*/*[@datatype='version']|oval-def:states/*/*[@datatype='version']|oval-def:states/*/*/*[@datatype='version']" priority="1001" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of version.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='record']|oval-def:states/*/*[@datatype='record']" priority="1000" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of record.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template><axsl:template match="text()" priority="-1" mode="M32"/><axsl:template match="@*|node()" priority="-2" mode="M32"><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

<!--PATTERN oval-def_no_var_ref_with_records-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='record']|oval-def:states/*/*[@datatype='record']" priority="1000" mode="M33">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@var_ref)"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of var_ref is prohibited when the datatype is 'record'.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template><axsl:template match="text()" priority="-1" mode="M33"/><axsl:template match="@*|node()" priority="-2" mode="M33"><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template>

<!--PATTERN oval-def_definition_entity_type_check_rules-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[not((@xsi:nil='1' or @xsi:nil='true')) and not(@var_ref) and @datatype='int']|oval-def:objects/*/*/*[not(@var_ref) and @datatype='int']|oval-def:states/*/*[not((@xsi:nil='1' or @xsi:nil='true')) and not(@var_ref) and @datatype='int']|oval-def:states/*/*/*[not(@var_ref) and @datatype='int']" priority="1000" mode="M34">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'.'))) and (number(.) = floor(.))"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M34"/></axsl:template><axsl:template match="text()" priority="-1" mode="M34"/><axsl:template match="@*|node()" priority="-2" mode="M34"><axsl:apply-templates select="@*|*" mode="M34"/></axsl:template>

<!--PATTERN oval-sc_entity_rules-->


	<!--RULE -->
<axsl:template match="oval-sc:system_data/*/*|oval-sc:system_data/*/*/*" priority="1001" mode="M35">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@status) or @status='exists' or .=''"/><axsl:otherwise>Warning: item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a value for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity should only be supplied if the status attribute is 'exists'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M35"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-sc:system_data/*/*[not((@xsi:nil='1' or @xsi:nil='true')) and @datatype='int']|oval-sc:system_data/*/*/*[not((@xsi:nil='1' or @xsi:nil='true')) and @datatype='int']" priority="1000" mode="M35">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'.'))) and (number(.) = floor(.))"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M35"/></axsl:template><axsl:template match="text()" priority="-1" mode="M35"/><axsl:template match="@*|node()" priority="-2" mode="M35"><axsl:apply-templates select="@*|*" mode="M35"/></axsl:template></axsl:stylesheet>
