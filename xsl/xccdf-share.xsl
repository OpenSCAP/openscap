<?xml version="1.0" encoding="utf-8" ?>

<!--
Copyright 2010 - 2014 Red Hat Inc., Durham, North Carolina.
All Rights Reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

Authors:
     Martin Preisler <mpreisle@redhat.com>
     Lukas Kuklinek <lkuklinek@redhat.com>
-->

<xsl:stylesheet version="1.1"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.2">

<xsl:template name="rule-result-tooltip">
    <xsl:param name="ruleresult"/>
    <!-- The texts are sourced from XCCDF 1.2 specification with minor modifications -->
    <xsl:choose>
        <xsl:when test="$ruleresult = 'pass'">The target system or system component satisfied all the conditions of the rule.</xsl:when>
        <xsl:when test="$ruleresult = 'fixed'">The Rule had failed, but was then fixed (possibly by a tool that can automatically apply remediation, or possibly by the human auditor).</xsl:when>
        <xsl:when test="$ruleresult = 'informational'">The Rule was checked, but the output from the checking engine is simply information for auditors or administrators; it is not a compliance category. This status value is designed for Rule elements whose main purpose is to extract information from the target rather than test the target.</xsl:when>

        <xsl:when test="$ruleresult = 'fail'">The target system or system component did not satisfy at least one condition of the rule.</xsl:when>
        <xsl:when test="$ruleresult = 'error'">The checking engine could not complete the evaluation, therefore the status of the target's compliance with the rule is not certain. This could happen, for example, if a testing tool was run with insufficient privileges and could not gather all of the necessary information.</xsl:when>
        <xsl:when test="$ruleresult = 'unknown'">The testing tool encountered some problem and the result is unknown. For example, a result of 'unknown' might be given if the testing tool was unable to interpret the output of the checking engine (the output has no meaning to the testing tool).</xsl:when>

        <xsl:when test="$ruleresult = 'notchecked'">The Rule was not evaluated by the checking engine. This status is designed for Rule elements that have no check elements or that correspond to an unsupported checking system. It may also correspond to a status returned by a checking engine if the checking engine does not support the indicated check code.</xsl:when>
        <xsl:when test="$ruleresult = 'notselected'">The Rule was not selected in the evaluation. This may be caused by the rule not being selected by default in the benchmark or by the profile unselecting it.</xsl:when>
        <xsl:when test="$ruleresult = 'notapplicable'">The Rule was not applicable to the target of the test. For example, the Rule might have been specific to a different version of the target OS, or it might have been a test against a platform feature that was not installed.</xsl:when>
    </xsl:choose>
</xsl:template>

<!-- templates in mode "text", for processing text with
     markup and substitutions.
 -->

<!-- text nodes -->
<xsl:template match="text()" mode='sub'>
    <xsl:value-of select="."/>
</xsl:template>

<!-- substitute cdf:instance -->
<xsl:template match="cdf:fix//cdf:instance" mode='sub'>
    <xsl:variable name='ctx' select='@context'/>
  <xsl:choose>
    <xsl:when test='ancestor::cdf:rule-result'>
        <abbr class='replace' title='context: {$ctx}'><xsl:value-of select='ancestor::cdf:rule-result/cdf:instance[@context=$ctx]'/></abbr>
    </xsl:when>
    <xsl:otherwise><abbr class='cdf-sub-context' title='replace with actual {$ctx} context'><xsl:value-of select='$ctx'/></abbr></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- substitute cdf:sub -->
<xsl:template match="cdf:sub" mode='sub'>
    <xsl:variable name="subid" select="./@idref"/>
    <xsl:variable name="plain" select="ancestor::cdf:Benchmark/cdf:plain-text[@id=$subid]"/>
    <xsl:variable name="val" select="ancestor::cdf:Benchmark//cdf:Value[@id=$subid]"/>
  <xsl:choose>
    <xsl:when test='$plain'><abbr class='xccdf-plain-text' title="text: {$subid}"><xsl:value-of select="$plain"/></abbr></xsl:when>
    <xsl:otherwise><abbr class='cdf-sub-value' title="value: {$subid} ({$val/cdf:title})"><xsl:value-of select="$val/cdf:value"/></abbr></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- substitute object with #xccdf:value -->
<xsl:template match='object[starts-with(@data, "#xccdf:value:")]' mode='sub'>
    <xsl:variable name='id' value='substring(@data, 13)'/>
    <xsl:variable name='sub' select='ancestor::cdf:TestResult/cdf:target-facts/cdf:fact[@name=$id]/text()|ancestor::cdf:Benchmark//cdf:Value[@id=$id]/cdf:value[1]/text()|ancestor::cdf:Benchmark/cdf:plain-text[@id=$id][1]/text()'/>
  <xsl:choose>
    <xsl:when test='$sub'><xsl:value-of select='$sub[1]'/></xsl:when>
    <xsl:otherwise><xsl:value-of select='.'/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- substitute object with #xccdf:title -->
<xsl:template match='object[starts-with(@data, "#xccdf:title:")]' mode='sub'>
    <xsl:variable name='id' value='substring(@data, 13)'/>
    <xsl:variable name='sub' select='ancestor::cdf:Benchmark//cdf:Group/cdf:title|ancestor::cdf:Benchmark//cdf:Rule/cdf:title|ancestor::cdf:Benchmark//cdf:Value/cdf:title'/>
  <xsl:choose>
    <xsl:when test='$sub'><xsl:value-of select='$sub[1]'/></xsl:when>
    <xsl:otherwise><xsl:value-of select='.'/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- substitute a with #xccdf:link -->
<xsl:template match='a[starts-with(@href, "#xccdf:link:")]' mode='sub'>
    <xsl:variable name='id' value='substring(@href, 12)'/>
  <xsl:copy>
    <xsl:copy-of select='@*'/>
    <xsl:attribute name='href'><xsl:apply-templates select='@href' mode='sub'><xsl:with-param name='id' select='$id'/></xsl:apply-templates></xsl:attribute>
    <xsl:apply-templates select="node()" mode='sub'/>
  </xsl:copy>
</xsl:template>

<!-- default link-generating code -->
<xsl:template match='@href' mode='sub'><xsl:param name='id'/>#item-<xsl:value-of select='$id'/></xsl:template>

<!-- identity transform for the rest of HTML -->
<xsl:template match="*" mode='sub'>
    <xsl:element name="{local-name()}">
        <xsl:copy-of select="@*"/>
        <xsl:apply-templates select="./text() | ./*" mode='sub'/>
  </xsl:element>
</xsl:template>

<!-- support for direct call of the stylesheet -->
<xsl:template match='node()|@*' mode='sub'><xsl:copy><xsl:copy-of select='@*'/><xsl:apply-templates select='node()' mode='sub'/></xsl:copy></xsl:template>

<xsl:template match='cdf:description|cdf:fix|cdf:fixtext|cdf:front-matter|cdf:rear-matter|cdf:rationale|cdf:warning' mode='sub'>
  <xsl:copy><xsl:copy-of select='@*'/><xsl:apply-templates select='node()' mode='sub'/></xsl:copy>
</xsl:template>

</xsl:stylesheet>
