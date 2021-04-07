<?xml version="1.0" standalone="yes"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:iso="http://purl.oclc.org/dsdl/schematron" xmlns:ai="http://scap.nist.gov/schema/asset-identification/1.1" xmlns:arf="http://scap.nist.gov/schema/asset-reporting-format/1.1" xmlns:cat="urn:oasis:names:tc:entity:xmlns:xml:catalog" xmlns:cpe-dict="http://cpe.mitre.org/dictionary/2.0" xmlns:cpe-lang="http://cpe.mitre.org/language/2.0" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:ds="http://scap.nist.gov/schema/scap/source/1.2" xmlns:dsig="http://www.w3.org/2000/09/xmldsig#" xmlns:java="java:gov.nist.secauto.scap.validation.schematron" xmlns:nvd-config="http://scap.nist.gov/schema/feed/configuration/0.1" xmlns:ocil="http://scap.nist.gov/schema/ocil/2.0" xmlns:oval-com="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval-res="http://oval.mitre.org/XMLSchema/oval-results-5" xmlns:oval-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5" xmlns:rc="http://scap.nist.gov/schema/reporting-core/1.1" xmlns:scap="http://scap.nist.gov/schema/scap/source/1.2" xmlns:scap-con="http://scap.nist.gov/schema/scap/constructs/1.2" xmlns:tmsad="http://scap.nist.gov/schema/xml-dsig/1.0" xmlns:xccdf="http://checklists.nist.gov/xccdf/1.2" xmlns:xcf="nist:scap:xslt:function" xmlns:xinclude="http://www.w3.org/2001/XInclude" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsd="http://www.w3.org/2001/XMLSchema" version="1.0"><!--Implementers: please note that overriding process-prolog or process-root is 
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
<axsl:template match="/"><axsl:apply-templates select="/" mode="M25"/></axsl:template>

<!--SCHEMATRON PATTERNS-->


<!--PATTERN scap-general-->


	<!--RULE scap-general-xccdf-benchmark-->
<axsl:template match="xccdf:Benchmark" priority="1017" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:description"/><axsl:otherwise>Error: SRC-10-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="@xml:lang"/><axsl:otherwise>Error: SRC-2-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(.//xccdf:check-content)"/><axsl:otherwise>Error: SRC-25-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(.//xccdf:Value//xccdf:source) and not(.//xccdf:Value//xccdf:complex-value) and not(.//xccdf:Value//xccdf:complex-default) and not(.//xccdf:Value//xccdf:choices//xccdf:complex-choice)"/><axsl:otherwise>Error: SRC-276-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:version/@time"/><axsl:otherwise>Warning: SRC-3-2|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(.//xinclude:include)"/><axsl:otherwise>Error: SRC-339-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test=".//xccdf:version[string(@update)]"/><axsl:otherwise>Warning: SRC-341-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(.//xccdf:set-complex-value)"/><axsl:otherwise>Error: SRC-343-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="@style = 'SCAP_1.3'"/><axsl:otherwise>Warning: SRC-4-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:metadata"/><axsl:otherwise>Error: SRC-8-2|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:title"/><axsl:otherwise>Error: SRC-9-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']"/><axsl:otherwise>Error: SRC-9-3|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))"/><axsl:otherwise>Error: SRC-9-3|xccdf:Benchmark <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-xccdf-profile-->
<axsl:template match="xccdf:Profile" priority="1016" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:description"/><axsl:otherwise>Error: SRC-10-1|xccdf:Profile <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:title"/><axsl:otherwise>Error: SRC-9-1|xccdf:Profile <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']"/><axsl:otherwise>Error: SRC-9-3|xccdf:Profile <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))"/><axsl:otherwise>Error: SRC-9-3|xccdf:Profile <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-xccdf-value-->
<axsl:template match="xccdf:Value" priority="1015" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:description"/><axsl:otherwise>Error: SRC-10-1|xccdf:Value <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:title"/><axsl:otherwise>Error: SRC-9-1|xccdf:Value <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']"/><axsl:otherwise>Error: SRC-9-3|xccdf:Value <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))"/><axsl:otherwise>Error: SRC-9-3|xccdf:Value <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-xccdf-group-->
<axsl:template match="xccdf:Group" priority="1014" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:description"/><axsl:otherwise>Error: SRC-10-1|xccdf:Group <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@extends)"/><axsl:otherwise>SRC-354-1|xccdf:Group <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:title"/><axsl:otherwise>Error: SRC-9-1|xccdf:Group <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']"/><axsl:otherwise>Error: SRC-9-3|xccdf:Group <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))"/><axsl:otherwise>Error: SRC-9-3|xccdf:Group <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-xccdf-rule-->
<axsl:template match="xccdf:Rule" priority="1013" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:description"/><axsl:otherwise>Error: SRC-10-1|xccdf:Rule <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:title"/><axsl:otherwise>Error: SRC-9-1|xccdf:Rule <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']"/><axsl:otherwise>Error: SRC-9-3|xccdf:Rule <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))"/><axsl:otherwise>Error: SRC-9-3|xccdf:Rule <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:ident[@system = 'http://cce.mitre.org' or @system = 'http://cve.mitre.org' or @system = 'http://cpe.mitre.org']"/><axsl:otherwise>Warning: SRC-251-1|xccdf:Rule <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="@selected and @weight and @role and @severity"/><axsl:otherwise>Warning: A-26-1|xccdf:Rule <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-xccdf-metadata-->
<axsl:template match="xccdf:Benchmark/xccdf:metadata" priority="1012" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="dc:creator/text()"/><axsl:otherwise>Error: SRC-8-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="parent::xccdf:Benchmark/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="dc:publisher/text()"/><axsl:otherwise>Error: SRC-8-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="parent::xccdf:Benchmark/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="dc:contributor/text()"/><axsl:otherwise>Error: SRC-8-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="parent::xccdf:Benchmark/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="dc:source/text()"/><axsl:otherwise>Error: SRC-8-1|xccdf:Benchmark <axsl:text/><axsl:value-of select="parent::xccdf:Benchmark/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-system-cpe-dict-check-->
<axsl:template match="cpe-dict:check" priority="1011" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@system = 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system = 'http://scap.nist.gov/schema/ocil/2'"/><axsl:otherwise>Error: SRC-118-2|cpe-dict:cpe-item <axsl:text/><axsl:value-of select="ancestor::cpe-dict:cpe-item[1]/@name"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-system-cpe-lang-check-->
<axsl:template match="cpe-lang:check-fact-ref" priority="1010" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@system = 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system = 'http://scap.nist.gov/schema/ocil/2'"/><axsl:otherwise>Error: SRC-118-3|cpe-lang:platform <axsl:text/><axsl:value-of select="ancestor::cpe-lang:platform[1]/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-xccdf-check-->
<axsl:template match="xccdf:Rule/xccdf:check" priority="1009" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@system = 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system = 'http://scap.nist.gov/schema/ocil/2'"/><axsl:otherwise>Error: SRC-118-1|xccdf:Rule <axsl:text/><axsl:value-of select="ancestor::xccdf:Rule[1]/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(current()/parent::xccdf:Rule[substring(@id, string-length(@id) - string-length('security_patches_up_to_date') + 1) = 'security_patches_up_to_date']) or current()/@system = 'http://oval.mitre.org/XMLSchema/oval-definitions-5'"/><axsl:otherwise>Error: SRC-169-2|xccdf:Rule <axsl:text/><axsl:value-of select="parent::xccdf:Rule/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="xccdf:check-content-ref"/><axsl:otherwise>Error: SRC-175-1|xccdf:Rule <axsl:text/><axsl:value-of select="parent::xccdf:Rule/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-scap-data-stream-collection-->
<axsl:template match="scap:data-stream-collection " priority="1008" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="current()[@schematron-version='1.3']"/><axsl:otherwise>Error: SRC-330-2|scap:data-stream-collection<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-scap-content-->
<axsl:template match="scap:data-stream" priority="1007" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@use-case = 'CONFIGURATION' or @use-case = 'VULNERABILITY' or @use-case = 'INVENTORY' or @use-case = 'OTHER'"/><axsl:otherwise>Error: SRC-324-1|scap:data-stream <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-oval-def-->
<axsl:template match="oval-def:definition" priority="1006" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@class != 'compliance' or oval-def:metadata/oval-def:reference[@source='http://cce.mitre.org' or @source='CCE']"/><axsl:otherwise>Warning: SRC-207-1|oval-def:definition <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="@class != 'vulnerability' or oval-def:metadata/oval-def:reference[@source='http://cve.mitre.org' or @source='CVE']"/><axsl:otherwise>Warning: SRC-214-1|oval-def:definition <axsl:text/><axsl:value-of select="@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-oval-generator-->
<axsl:template match="oval-def:generator/oval-com:schema_version" priority="1005" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@platform or text()='5.3' or text()='5.4' or text()='5.5' or text()='5.6' or text()='5.7' or text()='5.8' or text()='5.9' or text()='5.10' or text()='5.10.1' or text()='5.11' or text()='5.11.1' or text()='5.11.2'"/><axsl:otherwise>Error: SRC-216-1<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@platform) or text()='5.11.1:1.0' or text()='5.11.1:1.1' or text()='5.11.1:1.2' or text()='5.11.2:1.0' or text()='5.11.2:1.1' or text()='5.11.2:1.2'"/><axsl:otherwise>Error: SRC-216-2<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-signature-sig-->
<axsl:template match="dsig:Signature" priority="1004" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="dsig:Object/dsig:Manifest"/><axsl:otherwise>Error: SRC-284-1|dsig:Signature <axsl:text/><axsl:value-of select="@Id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="dsig:Object/dsig:SignatureProperties/dsig:SignatureProperty/tmsad:signature-info"/><axsl:otherwise>Error: SRC-285-1|dsig:Signature <axsl:text/><axsl:value-of select="@Id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::ds:data-stream-collection[1]/ds:data-stream[concat('#',@id) = current()/dsig:SignedInfo/dsig:Reference[1]/@URI]"/><axsl:otherwise>Error: SRC-286-1|dsig:Signature <axsl:text/><axsl:value-of select="@Id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="dsig:Object/dsig:SignatureProperties[concat('#',@Id) = current()/dsig:SignedInfo/dsig:Reference[2]/@URI]"/><axsl:otherwise>Error: SRC-287-1|dsig:Signature <axsl:text/><axsl:value-of select="@Id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="dsig:Object/dsig:Manifest[concat('#',@Id) = current()/dsig:SignedInfo/dsig:Reference[3]/@URI]"/><axsl:otherwise>Error: SRC-288-1|dsig:Signature <axsl:text/><axsl:value-of select="@Id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="dsig:KeyInfo"/><axsl:otherwise>Warning: SRC-290-1|dsig:Signature <axsl:text/><axsl:value-of select="@Id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-general-xccdf-status-rule-value-date-->
<axsl:template match="xccdf:status" priority="1003" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="text()='draft' or text()= 'accepted'"/><axsl:otherwise>Error: SRC-5-1<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="@date!=''"/><axsl:otherwise>Error: SRC-5-2<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-cce-check-rule-1-->
<axsl:template match="xccdf:ident" priority="1002" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@system='http://cce.mitre.org' or @system='CCE') or text() != ''"/><axsl:otherwise>Warning: A-16-1|xccdf:Rule <axsl:text/><axsl:value-of select="ancestor::xccdf:Rule/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@system='http://cce.mitre.org' or @system='CCE') or starts-with(text(), 'CCE-')"/><axsl:otherwise>Error: A-17-1|<axsl:text/><axsl:value-of select="."/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-cce-check-rule-2-->
<axsl:template match="oval-def:reference" priority="1001" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@source='http://cce.mitre.org' or @source='CCE') or @ref_id!=''"/><axsl:otherwise>Warning: A-16-1|oval-def:definition <axsl:text/><axsl:value-of select="ancestor::oval-def:definition/@id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@source='http://cce.mitre.org' or @source='CCE') or starts-with(@ref_id, 'CCE-')"/><axsl:otherwise>Error: A-17-1|<axsl:text/><axsl:value-of select="@ref_id"/><axsl:text/><axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template>

	<!--RULE scap-check-system-content-match-rule-->
<axsl:template match="scap:check-system-content" priority="1000" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@content-type='OVAL_COMPLIANCE' or @content-type='OVAL_PATCH' or @content-type='CPE_INVENTORY' or @content-type='OVAL_VULNERABILITY') or oval-def:oval_definitions"/><axsl:otherwise>Error: A-18-1<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="@content-type != 'OCIL_QUESTIONS' or ocil:ocil"/><axsl:otherwise>Error: A-18-1<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template><axsl:template match="text()" priority="-1" mode="M25"/><axsl:template match="@*|node()" priority="-2" mode="M25"><axsl:apply-templates select="*|comment()|processing-instruction()" mode="M25"/></axsl:template></axsl:stylesheet>
