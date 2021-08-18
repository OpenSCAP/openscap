<?xml version="1.0" standalone="yes"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:iso="http://purl.oclc.org/dsdl/schematron" xmlns:xccdf="http://checklists.nist.gov/xccdf/1.2" xmlns:cpe2="http://cpe.mitre.org/language/2.0" xmlns:xhtml="http://www.w3.org/1999/xhtml" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:dsig="http://www.w3.org/2000/09/xmldsig#" version="1.0"><!--Implementers: please note that overriding process-prolog or process-root is 
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
<axsl:template match="/"><axsl:apply-templates select="/" mode="M9"/><axsl:apply-templates select="/" mode="M10"/><axsl:apply-templates select="/" mode="M11"/><axsl:apply-templates select="/" mode="M12"/><axsl:apply-templates select="/" mode="M13"/><axsl:apply-templates select="/" mode="M14"/><axsl:apply-templates select="/" mode="M15"/><axsl:apply-templates select="/" mode="M16"/><axsl:apply-templates select="/" mode="M17"/><axsl:apply-templates select="/" mode="M18"/><axsl:apply-templates select="/" mode="M19"/><axsl:apply-templates select="/" mode="M20"/><axsl:apply-templates select="/" mode="M21"/><axsl:apply-templates select="/" mode="M22"/><axsl:apply-templates select="/" mode="M23"/><axsl:apply-templates select="/" mode="M24"/><axsl:apply-templates select="/" mode="M25"/><axsl:apply-templates select="/" mode="M26"/><axsl:apply-templates select="/" mode="M27"/><axsl:apply-templates select="/" mode="M28"/><axsl:apply-templates select="/" mode="M29"/><axsl:apply-templates select="/" mode="M30"/><axsl:apply-templates select="/" mode="M31"/><axsl:apply-templates select="/" mode="M32"/><axsl:apply-templates select="/" mode="M33"/><axsl:apply-templates select="/" mode="M34"/><axsl:apply-templates select="/" mode="M35"/><axsl:apply-templates select="/" mode="M36"/><axsl:apply-templates select="/" mode="M37"/><axsl:apply-templates select="/" mode="M38"/><axsl:apply-templates select="/" mode="M39"/><axsl:apply-templates select="/" mode="M40"/><axsl:apply-templates select="/" mode="M41"/><axsl:apply-templates select="/" mode="M42"/><axsl:apply-templates select="/" mode="M43"/><axsl:apply-templates select="/" mode="M44"/><axsl:apply-templates select="/" mode="M45"/><axsl:apply-templates select="/" mode="M46"/><axsl:apply-templates select="/" mode="M47"/><axsl:apply-templates select="/" mode="M48"/><axsl:apply-templates select="/" mode="M49"/><axsl:apply-templates select="/" mode="M50"/><axsl:apply-templates select="/" mode="M51"/><axsl:apply-templates select="/" mode="M52"/><axsl:apply-templates select="/" mode="M53"/><axsl:apply-templates select="/" mode="M54"/><axsl:apply-templates select="/" mode="M55"/><axsl:apply-templates select="/" mode="M56"/><axsl:apply-templates select="/" mode="M57"/><axsl:apply-templates select="/" mode="M58"/><axsl:apply-templates select="/" mode="M59"/><axsl:apply-templates select="/" mode="M60"/></axsl:template>

<!--SCHEMATRON PATTERNS-->


<!--PATTERN xhtml_object_data_prefix-->


	<!--RULE -->
<axsl:template match="xhtml:object" priority="1000" mode="M9">

		<!--ASSERT -->
<axsl:choose><axsl:when test="starts-with(@data,'#xccdf:')"/><axsl:otherwise>Error: The @data attribute of an XHTML 'object' element must be prefixed with '#xccdf:'. See the XCCDF 1.2.1 specification, Section 7.2.3.6.3.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M9"/></axsl:template><axsl:template match="text()" priority="-1" mode="M9"/><axsl:template match="@*|node()" priority="-2" mode="M9"><axsl:apply-templates select="@*|*" mode="M9"/></axsl:template>

<!--PATTERN xhtml_object_data_values-->


	<!--RULE -->
<axsl:template match="xhtml:object" priority="1000" mode="M10">

		<!--ASSERT -->
<axsl:choose><axsl:when test="starts-with(@data,'#xccdf:title:') or starts-with(@data,'#xccdf:value:')"/><axsl:otherwise>Warning: The @data attribute of an XHTML 'object' element should have a pattern of either '#xccdf:value:[id]' or '#xccdf:title:[id]'. See the XCCDF 1.2.1 specification, Section 7.2.3.6.3.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M10"/></axsl:template><axsl:template match="text()" priority="-1" mode="M10"/><axsl:template match="@*|node()" priority="-2" mode="M10"><axsl:apply-templates select="@*|*" mode="M10"/></axsl:template>

<!--PATTERN xhtml_object_data_values_exist-->


	<!--RULE -->
<axsl:template match="xhtml:object[starts-with(@data,'#xccdf:value:')]" priority="1000" mode="M11"><axsl:variable name="object_data_value_valid" select="not(substring-after(@data,'#xccdf:value:')='')"/><axsl:variable name="object_data_id" select="substring-after(@data,'#xccdf:value:')"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="($object_data_value_valid) and (/*//xccdf:plain-text[@id=$object_data_id]|/*//xccdf:Value[@id=$object_data_id]|/*//xccdf:fact[@id=$object_data_id])"/><axsl:otherwise>Warning: The given id '<axsl:text/><axsl:value-of select="$object_data_id"/><axsl:text/>' should match a the @id attribute of a 'plain-text', 'Value', or 'fact' element. See the XCCDF 1.2.1 specification, Section 7.2.3.6.3.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template><axsl:template match="text()" priority="-1" mode="M11"/><axsl:template match="@*|node()" priority="-2" mode="M11"><axsl:apply-templates select="@*|*" mode="M11"/></axsl:template>

<!--PATTERN xhtml_a_name_link_prefix-->


	<!--RULE -->
<axsl:template match="xhtml:a[@name]" priority="1000" mode="M12">

		<!--ASSERT -->
<axsl:choose><axsl:when test="starts-with(@name,'#xccdf:link:')"/><axsl:otherwise>Warning: The @name attribute of an XHTML 'a' element should be prefixed with '#xccdf:link:'. See the XCCDF 1.2.1 specification, Section 7.2.3.6.4.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M12"/></axsl:template><axsl:template match="text()" priority="-1" mode="M12"/><axsl:template match="@*|node()" priority="-2" mode="M12"><axsl:apply-templates select="@*|*" mode="M12"/></axsl:template>

<!--PATTERN xccdf_signature_dsig_ref-->


	<!--RULE -->
<axsl:template match="xccdf:signature" priority="1000" mode="M13">

		<!--ASSERT -->
<axsl:choose><axsl:when test="./dsig:Signature/dsig:Reference"/><axsl:otherwise>Warning: A 'signature' element should contain an XML dsig 'Signature' element, which should contain one or more 'References' elements. See the XCCDF 1.2.1 specification, Section 6.2.7.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template><axsl:template match="text()" priority="-1" mode="M13"/><axsl:template match="@*|node()" priority="-2" mode="M13"><axsl:apply-templates select="@*|*" mode="M13"/></axsl:template>

<!--PATTERN xccdf_contains_title_child-->


	<!--RULE -->
<axsl:template match="xccdf:Benchmark|xccdf:Rule|xccdf:Group|xccdf:Value|xccdf:Profile|xccdf:Tailoring/xccdf:title" priority="1000" mode="M14">

		<!--ASSERT -->
<axsl:choose><axsl:when test="./xccdf:title"/><axsl:otherwise>Warning: A '<axsl:text/><axsl:value-of select="local-name()"/><axsl:text/>' element should contain a 'title' element.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M14"/></axsl:template><axsl:template match="text()" priority="-1" mode="M14"/><axsl:template match="@*|node()" priority="-2" mode="M14"><axsl:apply-templates select="@*|*" mode="M14"/></axsl:template>

<!--PATTERN benchmark_multiple_status_dates-->


	<!--RULE -->
<axsl:template match="xccdf:Benchmark|xccdf:Rule|xccdf:Group|xccdf:Value|xccdf:Profile" priority="1000" mode="M15">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(count(./xccdf:status) &lt; 2) or ((count(./xccdf:status) &gt; 1) and not(./xccdf:status[not(@date)]))"/><axsl:otherwise>Error: A '<axsl:text/><axsl:value-of select="local-name()"/><axsl:text/>' element with multiple 'status' elements must have the date attribute present in all 'status' elements. See the XCCDF 1.2.1 specification, Section 6.2.8.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template><axsl:template match="text()" priority="-1" mode="M15"/><axsl:template match="@*|node()" priority="-2" mode="M15"><axsl:apply-templates select="@*|*" mode="M15"/></axsl:template>

<!--PATTERN benchmark_extend_self_type-->


	<!--RULE -->
<axsl:template match="xccdf:Rule[@extends]|xccdf:Group[@extends]|xccdf:Value[@extends]|xccdf:Profile[@extends]" priority="1000" mode="M16"><axsl:variable name="extend_ref" select="./@extends"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="local-name() = local-name(/*//xccdf:Rule[@id = $extend_ref]|/*//xccdf:Group[@id = $extend_ref]|/*//xccdf:Value[@id = $extend_ref]|/*//xccdf:Profile[@id = $extend_ref])"/><axsl:otherwise>Error: A '<axsl:text/><axsl:value-of select="local-name()"/><axsl:text/>' element with an @extends attribute must extend another '<axsl:text/><axsl:value-of select="local-name()"/><axsl:text/>' element. <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M16"/></axsl:template><axsl:template match="text()" priority="-1" mode="M16"/><axsl:template match="@*|node()" priority="-2" mode="M16"><axsl:apply-templates select="@*|*" mode="M16"/></axsl:template>

<!--PATTERN benchmark_check_import_empty-->


	<!--RULE -->
<axsl:template match="xccdf:Rule/xccdf:check/xccdf:check-import" priority="1000" mode="M17">

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(./*|text()) = 0"/><axsl:otherwise>Error: A 'check-import' element within a 'Rule' element must have an empty body. See the XCCDF 1.2.1 specification, Section 6.4.4.4.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M17"/></axsl:template><axsl:template match="text()" priority="-1" mode="M17"/><axsl:template match="@*|node()" priority="-2" mode="M17"><axsl:apply-templates select="@*|*" mode="M17"/></axsl:template>

<!--PATTERN benchmark_unique_match_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:match)&gt;1]/xccdf:match" priority="1000" mode="M18"><axsl:variable name="selector" select="concat('',./@selector)"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(../xccdf:match[(@selector = $selector or (not(@selector) and $selector = ''))]) = 1"/><axsl:otherwise>Error: A 'Value' element containing multiple 'match' elements must have a unique @selector attribute for each 'match' element.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M18"/></axsl:template><axsl:template match="text()" priority="-1" mode="M18"/><axsl:template match="@*|node()" priority="-2" mode="M18"><axsl:apply-templates select="@*|*" mode="M18"/></axsl:template>

<!--PATTERN benchmark_omitted_match_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:match[not(@selector)]) &gt; 1]/xccdf:match[not(@selector)]" priority="1000" mode="M19">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Error: A 'Value' element may only contain zero or one 'match' elements with an omitted @selector attribute.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M19"/></axsl:template><axsl:template match="text()" priority="-1" mode="M19"/><axsl:template match="@*|node()" priority="-2" mode="M19"><axsl:apply-templates select="@*|*" mode="M19"/></axsl:template>

<!--PATTERN benchmark_unique_lower_bound_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:lower-bound)&gt;1]/xccdf:lower-bound" priority="1000" mode="M20"><axsl:variable name="selector" select="concat('',./@selector)"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(../xccdf:lower-bound[(@selector = $selector or (not(@selector) and $selector = ''))]) = 1"/><axsl:otherwise>Error: A 'Value' element containing multiple 'lower-bound' elements must have a unique @selector attribute for each 'lower-bound' element.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template><axsl:template match="text()" priority="-1" mode="M20"/><axsl:template match="@*|node()" priority="-2" mode="M20"><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template>

<!--PATTERN benchmark_omitted_lower_bound_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:lower-bound[not(@selector)]) &gt; 1]/xccdf:lower-bound[not(@selector)]" priority="1000" mode="M21">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Error: A 'Value' element may only contain zero or one 'lower-bound' elements with an omitted @selector attribute.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M21"/></axsl:template><axsl:template match="text()" priority="-1" mode="M21"/><axsl:template match="@*|node()" priority="-2" mode="M21"><axsl:apply-templates select="@*|*" mode="M21"/></axsl:template>

<!--PATTERN benchmark_unique_upper_bound_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:upper-bound)&gt;1]/xccdf:upper-bound" priority="1000" mode="M22"><axsl:variable name="selector" select="concat('',./@selector)"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(../xccdf:upper-bound[(@selector = $selector or (not(@selector) and $selector = ''))]) = 1"/><axsl:otherwise>Error: A 'Value' element containing multiple 'upper-bound' elements must have a unique @selector attribute for each 'upper-bound' element.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template><axsl:template match="text()" priority="-1" mode="M22"/><axsl:template match="@*|node()" priority="-2" mode="M22"><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template>

<!--PATTERN benchmark_omitted_upper_bound_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:upper-bound[not(@selector)]) &gt; 1]/xccdf:upper-bound[not(@selector)]" priority="1000" mode="M23">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Error: A 'Value' element may only contain zero or one 'upper-bound' elements with an omitted @selector attribute.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template><axsl:template match="text()" priority="-1" mode="M23"/><axsl:template match="@*|node()" priority="-2" mode="M23"><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template>

<!--PATTERN benchmark_unique_choices_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:choices)&gt;1]/xccdf:choices" priority="1000" mode="M24"><axsl:variable name="selector" select="concat('',./@selector)"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(../xccdf:choices[(@selector = $selector or (not(@selector) and $selector = ''))]) = 1"/><axsl:otherwise>Error: A 'Value' element containing multiple 'choices' elements must have a unique @selector attribute for each 'choices' element.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template><axsl:template match="text()" priority="-1" mode="M24"/><axsl:template match="@*|node()" priority="-2" mode="M24"><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template>

<!--PATTERN benchmark_omitted_choices_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:choices[not(@selector)]) &gt; 1]/xccdf:choices[not(@selector)]" priority="1000" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Error: A 'Value' element may only contain zero or one 'choices' elements with an omitted @selector attribute.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template><axsl:template match="text()" priority="-1" mode="M25"/><axsl:template match="@*|node()" priority="-2" mode="M25"><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template>

<!--PATTERN benchmark_unique_value_value_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:value|xccdf:complex-value)&gt;1]/xccdf:value|xccdf:Value[count(xccdf:value|xccdf:complex-value)&gt;1]/xccdf:complex-value" priority="1000" mode="M26"><axsl:variable name="selector" select="concat('',./@selector)"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(../xccdf:value[(@selector = $selector or (not(@selector) and $selector = ''))]|../xccdf:complex-value[(@selector = $selector or (not(@selector) and $selector = ''))]) = 1"/><axsl:otherwise>Error: A 'Value' element containing multiple 'value' or 'complex-value' elements must have a unique @selector attribute for each 'value' or 'complex-value' element.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template><axsl:template match="text()" priority="-1" mode="M26"/><axsl:template match="@*|node()" priority="-2" mode="M26"><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

<!--PATTERN benchmark_omitted_value_value_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:value[not(@selector)]|xccdf:complex-value[not(@selector)]) &gt; 1]" priority="1000" mode="M27">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Error: A 'Value' element may only contain zero or one 'value' or 'complex-value' elements with an omitted @selector attribute.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template><axsl:template match="text()" priority="-1" mode="M27"/><axsl:template match="@*|node()" priority="-2" mode="M27"><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template>

<!--PATTERN benchmark_unique_value_default_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:default|xccdf:complex-default)&gt;1]/xccdf:default|xccdf:Value[count(xccdf:default|xccdf:complex-default)&gt;1]/xccdf:complex-default" priority="1000" mode="M28"><axsl:variable name="selector" select="concat('',./@selector)"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(../xccdf:default[(@selector = $selector or (not(@selector) and $selector = ''))]|../xccdf:complex-default[(@selector = $selector or (not(@selector) and $selector = ''))]) = 1"/><axsl:otherwise>Error: A 'Value' element containing multiple 'default' or 'complex-default' elements must have a unique @selector attribute for each 'default' or 'complex-default' element.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template><axsl:template match="text()" priority="-1" mode="M28"/><axsl:template match="@*|node()" priority="-2" mode="M28"><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template>

<!--PATTERN benchmark_omitted_value_default_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[count(xccdf:default[not(@selector)]|xccdf:complex-default[not(@selector)]) &gt; 1]" priority="1000" mode="M29">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Error: A 'Value' element may only contain zero or one 'default' or 'complex-default' elements with an omitted @selector attribute.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M29"/></axsl:template><axsl:template match="text()" priority="-1" mode="M29"/><axsl:template match="@*|node()" priority="-2" mode="M29"><axsl:apply-templates select="@*|*" mode="M29"/></axsl:template>

<!--PATTERN benchmark_rule_group_idref_exists-->


	<!--RULE -->
<axsl:template match="xccdf:Profile[xccdf:select|xccdf:refine-rule]/xccdf:select|xccdf:refine-rule" priority="1000" mode="M30"><axsl:variable name="idref" select="./@idref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/*//xccdf:Rule[@id=$idref or @cluster-id=$idref]|/*//xccdf:Group[@id=$idref or @cluster-id=$idref]"/><axsl:otherwise>Error: The given @idref attribute '<axsl:text/><axsl:value-of select="$idref"/><axsl:text/>' must match a the @id or @cluster-id attributes of a 'Rule' or 'Group' element. See the XCCDF 1.2.1 specification, Section 6.5.3.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template><axsl:template match="text()" priority="-1" mode="M30"/><axsl:template match="@*|node()" priority="-2" mode="M30"><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template>

<!--PATTERN benchmark_value_idref_exists-->


	<!--RULE -->
<axsl:template match="xccdf:Profile[xccdf:refine-value|xccdf:set-value|xccdf:set-complex-value]/xccdf:refine-value|xccdf:set-value|xccdf:set-complex-value" priority="1000" mode="M31"><axsl:variable name="idref" select="./@idref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="/*//xccdf:Value[@id=$idref or @cluster-id=$idref]"/><axsl:otherwise>Error: The given @idref attribute '<axsl:text/><axsl:value-of select="$idref"/><axsl:text/>' must match a the @id or @cluster-id attributes of a 'Value' element. See the XCCDF 1.2.1 specification, Section 6.5.3.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M31"/></axsl:template><axsl:template match="text()" priority="-1" mode="M31"/><axsl:template match="@*|node()" priority="-2" mode="M31"><axsl:apply-templates select="@*|*" mode="M31"/></axsl:template>

<!--PATTERN benchmark_rule_result_override-->


	<!--RULE -->
<axsl:template match="xccdf:TestResult[xccdf:rule-result[xccdf:override]]/xccdf:rule-result/xccdf:result" priority="1000" mode="M32"><axsl:variable name="current_result" select="."/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="../xccdf:override/xccdf:new-result = $current_result"/><axsl:otherwise>Error: The value of the 'result' element must match the value of the 'new-result' element in an 'override' element when present. See the XCCDF 1.2.1 specification, Section 6.6.4.3.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template><axsl:template match="text()" priority="-1" mode="M32"/><axsl:template match="@*|node()" priority="-2" mode="M32"><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

<!--PATTERN benchmark_rule_result_idref_exists-->


	<!--RULE -->
<axsl:template match="xccdf:TestResult[xccdf:rule-result]/xccdf:rule-result" priority="1000" mode="M33"><axsl:variable name="idref" select="./@idref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="./ancestor::xccdf:Benchmark//xccdf:Rule[@id=$idref]"/><axsl:otherwise>Error: The given @idref attribute '<axsl:text/><axsl:value-of select="$idref"/><axsl:text/>' must match a the @id attribute of a 'Rule' element. See the XCCDF 1.2.1 specification, Section 6.6.4.1.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template><axsl:template match="text()" priority="-1" mode="M33"/><axsl:template match="@*|node()" priority="-2" mode="M33"><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template>

<!--PATTERN benchmark_contains_metadata-->


	<!--RULE -->
<axsl:template match="xccdf:Benchmark[(count(xccdf:metadata) = 0) or (not(xccdf:metadata/*[namespace-uri() = 'http://purl.org/dc/elements/1.1/']))]" priority="1000" mode="M34">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: A 'Benchmark' element should have a 'metadata' element, and it should contain a child from the Dublin Core schema. See the XCCDF 1.2.1 specification, Section 6.2.4.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M34"/></axsl:template><axsl:template match="text()" priority="-1" mode="M34"/><axsl:template match="@*|node()" priority="-2" mode="M34"><axsl:apply-templates select="@*|*" mode="M34"/></axsl:template>

<!--PATTERN benchmark_platform_invalid_prefix-->


	<!--RULE -->
<axsl:template match="xccdf:platform[not(starts-with(@idref,'cpe:/') or starts-with(@idref,'cpe:2.3:') or starts-with(@idref,'#'))]" priority="1000" mode="M35">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: The @idref attribute of a 'platform' element must begin with 'cpe:/' (CPE name version 2.2 and earlier),'cpe:2.3:' (CPE name version 2.3), or '#' (followed by the @id value of a CPE 'platform-specification' element). See the XCCDF 1.2.1 specification, Section 6.2.5.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M35"/></axsl:template><axsl:template match="text()" priority="-1" mode="M35"/><axsl:template match="@*|node()" priority="-2" mode="M35"><axsl:apply-templates select="@*|*" mode="M35"/></axsl:template>

<!--PATTERN benchmark_platform_prefix_deprecated-->


	<!--RULE -->
<axsl:template match="xccdf:platform[starts-with(@idref,'cpe:/')]" priority="1000" mode="M36">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: The 'cpe:/' prefix (CPE URI binding) is allowed within an @idref attribute, but the CPE Formatted String binding is preferred. See the XCCDF 1.2.1 specification, Section 6.2.5.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M36"/></axsl:template><axsl:template match="text()" priority="-1" mode="M36"/><axsl:template match="@*|node()" priority="-2" mode="M36"><axsl:apply-templates select="@*|*" mode="M36"/></axsl:template>

<!--PATTERN benchmark_platform_specification_exists-->


	<!--RULE -->
<axsl:template match="xccdf:platform[starts-with(@idref,'#')]" priority="1000" mode="M37"><axsl:variable name="platformid" select="substring-after(@idref,'#')"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="./ancestor::xccdf:Benchmark/cpe2:platform-specification/cpe2:platform[@id = $platformid]"/><axsl:otherwise>Warning: No matching 'platform' element with an @id of '<axsl:text/><axsl:value-of select="$platformid"/><axsl:text/>' found within a 'platform-specification' element of the 'Benchmark' element. See the XCCDF 1.2.1 specification, Section 6.2.5.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M37"/></axsl:template><axsl:template match="text()" priority="-1" mode="M37"/><axsl:template match="@*|node()" priority="-2" mode="M37"><axsl:apply-templates select="@*|*" mode="M37"/></axsl:template>

<!--PATTERN benchmark_platform_exists-->


	<!--RULE -->
<axsl:template match="xccdf:Benchmark[not(xccdf:platform)]" priority="1000" mode="M38">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: The 'Benchmark' element has no platform specified, which implies the benchmark applies to all platforms. Applicable platforms should be indicated if possible. See the XCCDF 1.2.1 specification, Section 6.2.5.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M38"/></axsl:template><axsl:template match="text()" priority="-1" mode="M38"/><axsl:template match="@*|node()" priority="-2" mode="M38"><axsl:apply-templates select="@*|*" mode="M38"/></axsl:template>

<!--PATTERN benchmark_description_exists-->


	<!--RULE -->
<axsl:template match="xccdf:Benchmark[not(xccdf:description)]" priority="1000" mode="M39">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: A 'Benchmark' element should have at least one 'description' element.  See the XCCDF 1.2.1 specification, Section 6.3.2.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M39"/></axsl:template><axsl:template match="text()" priority="-1" mode="M39"/><axsl:template match="@*|node()" priority="-2" mode="M39"><axsl:apply-templates select="@*|*" mode="M39"/></axsl:template>

<!--PATTERN benchmark_conflicts_exists-->


	<!--RULE -->
<axsl:template match="xccdf:Rule[xccdf:conflicts]/xccdf:conflicts|xccdf:Group[xccdf:conflicts]/xccdf:conflicts" priority="1000" mode="M40"><axsl:variable name="idref" select="@idref"/><axsl:variable name="parent_id" select="../@id"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="./ancestor::xccdf:Benchmark/xccdf:Rule[not(@id = $parent_id) and @id = $idref]|./ancestor::xccdf:Benchmark/xccdf:Group[not(@id = $parent_id) and @id = $idref]"/><axsl:otherwise>Warning: The @idref attribute in a 'conflicts' element should match the @id attribute of a different 'Rule' or 'Group' element. See the XCCDF 1.2.1 specification, Section 6.4.1.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template><axsl:template match="text()" priority="-1" mode="M40"/><axsl:template match="@*|node()" priority="-2" mode="M40"><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template>

<!--PATTERN benchmark_note_tag_exists-->


	<!--RULE -->
<axsl:template match="xccdf:Profile[@note-tag]" priority="1000" mode="M41"><axsl:variable name="note_id" select="@note-tag"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="./ancestor::xccdf:Benchmark/xccdf:Rule[xccdf:profile-note[@tag = $note_id]]"/><axsl:otherwise>Warning: The @note-tag attribute in a 'Profile' element should match the @tag attribute of a 'profile-note' element within a 'Rule' element. See the XCCDF 1.2.1 specification, Section 6.4.4.2.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M41"/></axsl:template><axsl:template match="text()" priority="-1" mode="M41"/><axsl:template match="@*|node()" priority="-2" mode="M41"><axsl:apply-templates select="@*|*" mode="M41"/></axsl:template>

<!--PATTERN benchmark_value_match_datatype-->


	<!--RULE -->
<axsl:template match="xccdf:Value[xccdf:match]" priority="1000" mode="M42">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@type) or @type = 'string' or @type = 'number'"/><axsl:otherwise>Warning: The datatype of a 'Value' element should be 'string' or 'number' if there is a child 'match' element. See the XCCDF 1.2.1 specification, Section 6.4.5.2.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M42"/></axsl:template><axsl:template match="text()" priority="-1" mode="M42"/><axsl:template match="@*|node()" priority="-2" mode="M42"><axsl:apply-templates select="@*|*" mode="M42"/></axsl:template>

<!--PATTERN benchmark_value_bounds_datatype-->


	<!--RULE -->
<axsl:template match="xccdf:Value[xccdf:lower-bound|xccdf:upper-bound]" priority="1000" mode="M43">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@type and @type = 'number'"/><axsl:otherwise>Warning: The datatype of a 'Value' element should be 'number' if there is a child 'lower-bound' or 'upper-bound' element. See the XCCDF 1.2.1 specification, Section 6.4.5.2.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M43"/></axsl:template><axsl:template match="text()" priority="-1" mode="M43"/><axsl:template match="@*|node()" priority="-2" mode="M43"><axsl:apply-templates select="@*|*" mode="M43"/></axsl:template>

<!--PATTERN benchmark_default_value_value_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[xccdf:value|xccdf:complex-value][count(xccdf:value[not(@selector) or @selector = '']|xccdf:complex-value[not(@selector) or @selector = '']) = 0]" priority="1000" mode="M44">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: All 'value' or 'complex-value' elements have non-empty @selector attribute values. The default selection will be the first of these elements. To explicitly designate a default, remove the selector of the default element. See the XCCDF 1.2.1 specification, Section 6.4.5.5.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template><axsl:template match="text()" priority="-1" mode="M44"/><axsl:template match="@*|node()" priority="-2" mode="M44"><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template>

<!--PATTERN benchmark_default_value_default_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[xccdf:default|xccdf:complex-default][count(xccdf:default[not(@selector) or @selector = '']|xccdf:complex-default[not(@selector) or @selector = '']) = 0]" priority="1000" mode="M45">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: All 'default' or 'complex-default' elements have non-empty @selector attribute values. The default selection will be the first of these elements. To explicitly designate a default, remove the selector of the default element. See the XCCDF 1.2.1 specification, Section 6.4.5.5.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M45"/></axsl:template><axsl:template match="text()" priority="-1" mode="M45"/><axsl:template match="@*|node()" priority="-2" mode="M45"><axsl:apply-templates select="@*|*" mode="M45"/></axsl:template>

<!--PATTERN benchmark_default_value_match_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[xccdf:match][count(xccdf:match[not(@selector) or @selector = '']) = 0]" priority="1000" mode="M46">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: All 'match' elements have non-empty @selector attribute values. This means that, by default, no 'match' element is used. To designate a default, remove the @selector from the desired default element. See the XCCDF 1.2.1 specification, Section 6.4.5.5.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M46"/></axsl:template><axsl:template match="text()" priority="-1" mode="M46"/><axsl:template match="@*|node()" priority="-2" mode="M46"><axsl:apply-templates select="@*|*" mode="M46"/></axsl:template>

<!--PATTERN benchmark_default_value_lower_bound_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[xccdf:lower-bound][count(xccdf:lower-bound[not(@selector) or @selector = '']) = 0]" priority="1000" mode="M47">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: All 'lower-bound' elements have non-empty @selector attribute values. This means that, by default, no 'lower-bound' element is used. To designate a default, remove the @selector from the desired default element. See the XCCDF 1.2.1 specification, Section 6.4.5.5..<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M47"/></axsl:template><axsl:template match="text()" priority="-1" mode="M47"/><axsl:template match="@*|node()" priority="-2" mode="M47"><axsl:apply-templates select="@*|*" mode="M47"/></axsl:template>

<!--PATTERN benchmark_default_value_upper_bound_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[xccdf:upper-bound][count(xccdf:upper-bound[not(@selector) or @selector = '']) = 0]" priority="1000" mode="M48">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: All 'upper-bound' elements have non-empty @selector attribute values. This means that, by default, no 'upper-bound' element is used. To designate a default, remove the @selector from the desired default element. See the XCCDF 1.2.1 specification, Section 6.4.5.5.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M48"/></axsl:template><axsl:template match="text()" priority="-1" mode="M48"/><axsl:template match="@*|node()" priority="-2" mode="M48"><axsl:apply-templates select="@*|*" mode="M48"/></axsl:template>

<!--PATTERN benchmark_default_value_choices_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Value[xccdf:choices][count(xccdf:choices[not(@selector) or @selector = '']) = 0]" priority="1000" mode="M49">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: All 'choices' elements have non-empty @selector attribute values. This means that, by default, no 'choices' element is used. To designate a default, remove the @selector from the desired default element. See the XCCDF 1.2.1 specification, Section 6.4.5.5.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M49"/></axsl:template><axsl:template match="text()" priority="-1" mode="M49"/><axsl:template match="@*|node()" priority="-2" mode="M49"><axsl:apply-templates select="@*|*" mode="M49"/></axsl:template>

<!--PATTERN benchmark_multi_check_true-->


	<!--RULE -->
<axsl:template match="xccdf:complex-check[xccdf:check[@multi-check = 'true']]/xccdf:check[@multi-check = 'true']" priority="1000" mode="M50">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: A 'check' element within a 'complex-check' element with its @multi-check attribute set to 'true' must be ignored by the tools. See the XCCDF 1.2.1 specification, Section 6.4.4.4.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template><axsl:template match="text()" priority="-1" mode="M50"/><axsl:template match="@*|node()" priority="-2" mode="M50"><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template>

<!--PATTERN benchmark_default_rule_check_selector-->


	<!--RULE -->
<axsl:template match="xccdf:Rule[xccdf:check][count(xccdf:check[not(@selector) or @selector = '']) = 0]" priority="1000" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: All 'check' elements have non-empty @selector attribute values. This means that, by default, no 'check' element is used. To designate a default, remove the @selector from the desired default element. See the XCCDF 1.2.1 specification, Section 6.4.4.4.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template><axsl:template match="text()" priority="-1" mode="M51"/><axsl:template match="@*|node()" priority="-2" mode="M51"><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

<!--PATTERN benchmark_value_description_question_exists-->


	<!--RULE -->
<axsl:template match="xccdf:Value[not(xccdf:description|xccdf:question)]" priority="1000" mode="M52">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: A 'Value' element should contain at least one 'description' or 'question' element. See the XCCDF 1.2.1 specification, Section 6.4.5.4.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M52"/></axsl:template><axsl:template match="text()" priority="-1" mode="M52"/><axsl:template match="@*|node()" priority="-2" mode="M52"><axsl:apply-templates select="@*|*" mode="M52"/></axsl:template>

<!--PATTERN benchmark_testresult_test_system_exists-->


	<!--RULE -->
<axsl:template match="xccdf:TestResult[not(@test-system)]" priority="1000" mode="M53">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: A 'TestResult' element should have a @test-system attribute. See the XCCDF 1.2.1 specification, Section 6.6.2.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M53"/></axsl:template><axsl:template match="text()" priority="-1" mode="M53"/><axsl:template match="@*|node()" priority="-2" mode="M53"><axsl:apply-templates select="@*|*" mode="M53"/></axsl:template>

<!--PATTERN benchmark_rule_result_single_child-->


	<!--RULE -->
<axsl:template match="xccdf:TestResult/xccdf:rule-result" priority="1000" mode="M54">

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(./xccdf:check|./xccdf:complex-check) = 1"/><axsl:otherwise>Warning: A 'rule-result' element should have exactly one child 'check' or 'complex-check' element. This is the conventional way of linking to the checking-system results for this Rule.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template><axsl:template match="text()" priority="-1" mode="M54"/><axsl:template match="@*|node()" priority="-2" mode="M54"><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template>

<!--PATTERN benchmark_rule_result_check_valid-->


	<!--RULE -->
<axsl:template match="xccdf:TestResult/xccdf:rule-result/xccdf:check" priority="1000" mode="M55"><axsl:variable name="selector" select="concat('',./@selector)"/><axsl:variable name="idref" select="../@idref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="./ancestor::xccdf:Benchmark//xccdf:Rule[@id=$idref]/xccdf:check[(@selector = $selector) or (not(@selector) and $selector = '')]"/><axsl:otherwise>Warning: A 'check' element within a 'TestResult/rule-result' element should have a matching @selector attribute as a 'check' element within the referenced 'Rule' element.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M55"/></axsl:template><axsl:template match="text()" priority="-1" mode="M55"/><axsl:template match="@*|node()" priority="-2" mode="M55"><axsl:apply-templates select="@*|*" mode="M55"/></axsl:template>

<!--PATTERN benchmark_group_deprecated_extends-->


	<!--RULE -->
<axsl:template match="xccdf:Group[@extends]" priority="1000" mode="M56">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: Deprecated attribute @extends found within 'Group' element. See the XCCDF 1.2.1 specification, Sections 6.4.1 and A.4.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M56"/></axsl:template><axsl:template match="text()" priority="-1" mode="M56"/><axsl:template match="@*|node()" priority="-2" mode="M56"><axsl:apply-templates select="@*|*" mode="M56"/></axsl:template>

<!--PATTERN benchmark_group_deprecated_abstract-->


	<!--RULE -->
<axsl:template match="xccdf:Group[@abstract='true']" priority="1000" mode="M57">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: Deprecated behavior of @abstract attribute set to 'true' found within 'Group' element.  See the XCCDF 1.2.1 specification, Sections 6.4.1 and A.4.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M57"/></axsl:template><axsl:template match="text()" priority="-1" mode="M57"/><axsl:template match="@*|node()" priority="-2" mode="M57"><axsl:apply-templates select="@*|*" mode="M57"/></axsl:template>

<!--PATTERN benchmark_rule_deprecated_impact_metric-->


	<!--RULE -->
<axsl:template match="xccdf:Rule[xccdf:impact-metric]" priority="1000" mode="M58">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: Deprecated 'impact-metric' element found within a 'Rule' element. See the XCCDF 1.2.1 specification, Sections 6.4.4.2 and A.4.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M58"/></axsl:template><axsl:template match="text()" priority="-1" mode="M58"/><axsl:template match="@*|node()" priority="-2" mode="M58"><axsl:apply-templates select="@*|*" mode="M58"/></axsl:template>

<!--PATTERN tailoring_profile_extends_valid-->


	<!--RULE -->
<axsl:template match="xccdf:Tailoring/xccdf:Profile[@extends]" priority="1000" mode="M59"><axsl:variable name="extends_id" select="@extends"/><axsl:variable name="current_id" select="@id"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(../xccdf:Profile[not(@id = $current_id)][@id = $extends_id])"/><axsl:otherwise>Error: A 'Profile' element within a 'Tailoring' element may not extend another 'Profile' in that 'Tailoring' element. See the XCCDF 1.2.1 specification, Section 6.7.3.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M59"/></axsl:template><axsl:template match="text()" priority="-1" mode="M59"/><axsl:template match="@*|node()" priority="-2" mode="M59"><axsl:apply-templates select="@*|*" mode="M59"/></axsl:template>

<!--PATTERN tailoring_group_deprecated_abstract-->


	<!--RULE -->
<axsl:template match="xccdf:Tailoring/xccdf:Profile[@abstract='true']" priority="1000" mode="M60">

		<!--ASSERT -->
<axsl:choose><axsl:when test="false()"/><axsl:otherwise>Warning: 'Profiles' in 'Tailoring' elements may not be extended, so any declared abstract will never be used. See the XCCDF 1.2.1 specification, Section 6.7.2.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M60"/></axsl:template><axsl:template match="text()" priority="-1" mode="M60"/><axsl:template match="@*|node()" priority="-2" mode="M60"><axsl:apply-templates select="@*|*" mode="M60"/></axsl:template></axsl:stylesheet>
