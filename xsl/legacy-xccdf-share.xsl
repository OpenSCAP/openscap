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

<xsl:stylesheet version="1.1"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:db="http://docbook.org/ns/docbook"
    xmlns:htm="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
	xmlns:h="http://www.w3.org/1999/xhtml"
    xmlns:fn="http://www.w3.org/2005/xpath-functions"
    xmlns:exsl="http://exslt.org/common"
    xmlns:func="http://exslt.org/functions"
    xmlns:s="http://open-scap.org/"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.2"
    xmlns:dc="http://purl.org/dc/elements/1.1/"
    xmlns:edate="http://exslt.org/dates-and-times">

<!-- parametres -->
<xsl:param name='oscap-version'/>
<xsl:param name='verbosity'/>
<xsl:param name='pwd'/>

<!-- variable with current date/time -->
<xsl:variable name='now' select='edate:date-time()'/>
<!-- veriable w/ generator info -->
<xsl:variable name='generator' select="'OpenSCAP stylesheet'"/>

<!-- begin of xccdf-substitute.xsl -->

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
<!-- end of xccdf-substitute.xsl -->

<!-- begin of xccdf-tailor.xsl -->
<xsl:param name='profile'/>
<xsl:param name='keep-profiles'/>
<xsl:param name='clean-profile-notes'/>

<xsl:variable name='theprofile' select='/cdf:Benchmark/cdf:Profile[@id=$profile]'/>

<!-- identity transform -->
<xsl:template mode='profile' match='node()|@*'>
  <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
</xsl:template>

<!-- omit profiles if needed -->
<xsl:template mode='profile' match='cdf:Profile'>
  <xsl:if test='$keep-profiles'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
  </xsl:if>
</xsl:template>

<!-- adjust 'selected' attribute and refine-rules -->
<xsl:template mode='profile' match='cdf:Group|cdf:Rule'>
  <xsl:variable name='id' select='@id'/>
  <xsl:variable name='cid' select='@cluster-id'/>
  <xsl:variable name='sel' select='$theprofile/cdf:select[@idref=$id or @idref=$cid][1]'/>
  <xsl:variable name='rr' select='$theprofile/cdf:refine-rule[@idref=$id or @idref=$cid][1]'/>
  <xsl:copy>
    <xsl:apply-templates select='@*' mode='profile'/>
    <xsl:if test='self::cdf:Rule and $rr'><xsl:copy-of select='$rr/@role|$rr/@severity|$rr/@weight'/></xsl:if>
    <xsl:if test='$sel'><xsl:attribute name='selected'><xsl:value-of select='$sel/@selected'/></xsl:attribute></xsl:if>
    <xsl:apply-templates mode='profile' select="node()"/>
  </xsl:copy>
</xsl:template>

<!-- apply refine-rule selectors -->
<xsl:template mode='profile' match='cdf:check'>
  <xsl:variable name='name' select='name()'/>
  <xsl:variable name='rule' select='ancestor::cdf:Rule[1]'/>
  <xsl:variable name='rr' select='$theprofile/cdf:refine-rule[@idref=$rule/@id or @idref=$rule/@cluster-id][1]'/>
  <xsl:for-each select='self::*[@selector=$rr/@selector or ((not(@selector) or @selector="") and count($rule/*[name()=$name][@selector=$rr/@selector])=0)]'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
  </xsl:for-each>
</xsl:template>

<!-- remove irelevant profile-notes -->
<xsl:template mode='profile' match='cdf:profile-note'>
  <xsl:if test='not($clean-profile-notes) or (@tag = $theprofile/@note-tag)'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
  </xsl:if>
</xsl:template>

<!-- implement refine-value -->
<xsl:template mode='profile' match='cdf:Value'>
  <xsl:variable name='id' select='@id'/>
  <xsl:variable name='cid' select='@cluster-id'/>
  <xsl:variable name='rv' select='$theprofile/cdf:refine-value[@idref=$id or @idref=$cid][1]'/>
  <xsl:variable name='sv' select='$theprofile/cdf:set-value[@idref=$id or @idref=$cid][1]'/>
  <xsl:copy>
    <xsl:apply-templates select='@*' mode='profile'/>
    <xsl:if test='$rv'><xsl:copy-of select='$rv/@operator'/></xsl:if>
    <xsl:apply-templates mode='profile' select="cdf:status|cdf:version|cdf:title|cdf:description|cdf:warning|cdf:question|cdf:reference"/>
    <xsl:choose>
      <xsl:when test='$sv'><cdf:value><xsl:value-of select='$sv'/></cdf:value></xsl:when>
      <xsl:otherwise><xsl:apply-templates mode='profile' select="cdf:value"/></xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates mode='profile' select="cdf:default|cdf:match|cdf:lower-bound|cdf:upper-bound|cdf:choices|cdf:source"/>
  </xsl:copy>
</xsl:template>

<!-- refine-value selector -->
<xsl:template mode='profile' match='cdf:value|cdf:default|cdf:match|cdf:lower-bound|cdf:upper-bound|cdf:choices'>
  <xsl:variable name='val' select='ancestor::cdf:Value'/>
  <xsl:variable name='name' select='name()'/>
  <xsl:variable name='rv' select='$theprofile/cdf:refine-value[@idref=$val/@id or @idref=$val/@cluster-id][1]'/>
  <xsl:for-each select='self::*[@selector=$rv/@selector or ((not(@selector) or @selector="") and count($val/*[name()=$name][@selector=$rv/@selector])=0)]'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
  </xsl:for-each>
</xsl:template>

<!-- erase @selector -->
<xsl:template mode='profile' match='@selector[not(ancestor::cdf:Profile)]'/>

<!-- top-level template -->
<xsl:template match='/'>
  <xsl:apply-templates mode='apply-profile'/>
</xsl:template>

<xsl:template mode='apply-profile' match='cdf:Benchmark'>
  <xsl:call-template name='warn-unresolved'/>
  <xsl:apply-templates select='.' mode='profile'/>
</xsl:template>
<!-- end of xccdf-tailor.xsl -->

<xsl:param name='profile'/>
<xsl:param name='substitute' select='1'/>

<xsl:variable name='warn-unresolved' select='1'/>

<!-- stage1: apply profile -->
<xsl:variable name='benchmark.s1'>
  <xsl:call-template name='warn-unresolved'/>
  <xsl:apply-templates select='/cdf:Benchmark' mode='profile'>
    <xsl:with-param name='p' select='/cdf:Benchmark/cdf:Profile[@id=$profile]'/>
  </xsl:apply-templates>
</xsl:variable>
<!-- stage2: text substitutions -->
<xsl:variable name='benchmark.s2'>
  <xsl:choose>
    <xsl:when test='$substitute'><xsl:apply-templates select='exsl:node-set($benchmark.s1)/cdf:Benchmark' mode='sub'/></xsl:when>
    <xsl:otherwise><xsl:copy-of select='$benchmark.s1'/></xsl:otherwise>
  </xsl:choose>
</xsl:variable>
<!-- create root element from the last stage -->
<xsl:variable name='root' select='exsl:node-set($benchmark.s2)/cdf:Benchmark'/>

<xsl:template name='warn-unresolved'>
  <xsl:if test='$warn-unresolved and cdf:Benchmark[not(@resolved=1)][not(@resolved="true")]'>
    <xsl:message>WARNING: Processing an unresolved XCCDF document. This may have unexpected results.</xsl:message>
  </xsl:if>
</xsl:template>


</xsl:stylesheet>
