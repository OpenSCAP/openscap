<?xml version="1.0" encoding="UTF-8" ?>
<!--
Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
     Lukas Kuklinek <lkuklinek@redhat.com>
-->

<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:htm="http://www.w3.org/1999/xhtml"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
	>

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

<!-- substitute htm:object with #xccdf:value -->
<xsl:template match='htm:object[starts-with(@data, "#xccdf:value:")]' mode='sub'>
  <xsl:variable name='id' value='substring(@data, 13)'/>
  <xsl:variable name='sub' select='ancestor::cdf:TestResult/cdf:target-facts/cdf:fact[@name=$id]/text()|ancestor::cdf:Benchmark//cdf:Value[@id=$id]/cdf:value[1]/text()|ancestor::cdf:Benchmark/cdf:plain-text[@id=$id][1]/text()'/>
  <xsl:choose>
    <xsl:when test='$sub'><xsl:value-of select='$sub[1]'/></xsl:when>
    <xsl:otherwise><xsl:value-of select='.'/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- substitute htm:object with #xccdf:title -->
<xsl:template match='htm:object[starts-with(@data, "#xccdf:title:")]' mode='sub'>
  <xsl:variable name='id' value='substring(@data, 13)'/>
  <xsl:variable name='sub' select='ancestor::cdf:Benchmark//cdf:Group/cdf:title|ancestor::cdf:Benchmark//cdf:Rule/cdf:title|ancestor::cdf:Benchmark//cdf:Value/cdf:title'/>
  <xsl:choose>
    <xsl:when test='$sub'><xsl:value-of select='$sub[1]'/></xsl:when>
    <xsl:otherwise><xsl:value-of select='.'/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- substitute htm:a with #xccdf:link -->
<xsl:template match='htm:a[starts-with(@href, "#xccdf:link:")]' mode='sub'>
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
<xsl:template match="htm:*" mode='sub'>
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

