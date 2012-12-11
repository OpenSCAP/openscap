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

<!--

FIX TEMPLATE WRITING GUIDE
**************************

This XSL uses a simple template engine to generate fix files.
Take a look at fixtpl-bash.xml file for an example.
All elements in such a file reside in the 'http://open-scap.org/xml/fixtemplate#' namespace.

Element hierarchy:
+ fixtemplate    - The toplevel element.
                   Attribute 'system' is a fix system identifier this template accepts.
                   It can be a whitespace-separated list of systems.
  + linecomment  - String that introduces a single-line comment.
                   (e.g. <linecomment>// </linecomment> for C++)
  + before       - This will be instantiated before actual fixes are outputted.
                   Can include for example a shebang, library imports, etc.
  + fixentry     - Template for single fix entry.
                   Element <sub select='$val'/> will be substituted with actual fix value.
  + after        - This will be instantiated after actual fixes are outputted.

Substitution elements can be placed to any of the child elements of the root element (before, fixentry, ...).
There are two of them:
+ <sub select='xpath'/> will be substituted with a string value of the XPath expression.
+ <if test='xpath'>conditional content, can include other <if> and <sub> elements</if>

Note that the XCCDF elements have to be accessed using the 'cdf:' prefix in the XPath expressions.
To obtain XCCDF context, several variables can be used in the XPath expressions:

$benchmark  - XCCDF benchmark
$testresult - TestResult element being processed (can be unavailible)
$result     - rule result associated with the fix (<fixentry> only, can be unavailible)
$rule       - XCCDF rule associated with the fix (<fixentry> only)
$fix        - XCCDF fix element (<fixentry> only)
$val        - actual XCCDF fix text value (substitutions already performed, <fixentry> only)

-->

<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:htm="http://www.w3.org/1999/xhtml"
    xmlns:exsl="http://exslt.org/common"
    xmlns:tpl="http://open-scap.org/xml/fixtemplate#"
    xmlns:dyn="http://exslt.org/dynamic"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
	>

<xsl:import href="xccdf-share.xsl" />

<xsl:output method="text" encoding="UTF-8"/>

<!-- params -->
<xsl:param name='result-id'/>
<xsl:param name='profile' select='string(/cdf:Benchmark/cdf:TestResult[@id=$result-id]/cdf:profile/@idref)'/>
<xsl:param name='template' select='"bash"'/>

<!-- variables -->
<xsl:variable name='tplfile'>
  <xsl:choose>
    <xsl:when test='contains($template, ".")'>
      <xsl:if test='not(contains($template, "://") or starts-with($template, "/")) and $pwd'><xsl:value-of select='$pwd'/>/</xsl:if><xsl:value-of select='$template'/>
    </xsl:when>
    <xsl:otherwise>fixtpl-<xsl:value-of select='$template'/>.xml</xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name='tpl' select='document($tplfile)'/>
<xsl:variable name='sys' select='concat(" ", $tpl/tpl:fixtemplate/@system, " ")'/>

<xsl:variable name='dummy'>
  <xsl:if test='not($tpl)'><xsl:message terminate='yes'>Fix template '<xsl:value-of select='$template'/>' failed to load. Aborting.</xsl:message></xsl:if>
</xsl:variable>

<xsl:variable name='benchmark' select='$root'/>
<xsl:variable name='testresult' select='$root/cdf:TestResult[@id=$result-id]'/>
<xsl:variable name='dummy2'>
  <xsl:if test='$result-id and not($testresult)'><xsl:message terminate='yes'>No TestResult '<xsl:value-of select='$result-id'/>'. Aborting.</xsl:message></xsl:if>
</xsl:variable>

<!-- keys -->
<xsl:key name='item' match='cdf:Rule|cdf:Group|cdf:Value' use='@id'/>

<!-- templates -->
<xsl:template match='/'>
  <xsl:call-template name='tpl.elem'><xsl:with-param name='elem' select='"before"'/></xsl:call-template>
  <xsl:choose>
    <xsl:when test='$testresult'>
# Generating fixes for all failed rules in test result '<xsl:value-of select="$result-id"/>'.

<xsl:apply-templates select='$testresult/cdf:rule-result[cdf:result="fail"]'/></xsl:when>
    <xsl:otherwise>
# Generating fixes for all Rules that are selected in the default profile.

<xsl:apply-templates select='$root//cdf:Rule[@selected="true" or @selected="1"]'/></xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name='tpl.elem'><xsl:with-param name='elem' select='"after"'/></xsl:call-template>
</xsl:template>

<xsl:template match='cdf:Rule'>
  <xsl:variable name='fix' select='cdf:fix[contains($sys, concat(" ", @system, ""))][1]'/>
  <xsl:variable name='val'><xsl:apply-templates mode='text' select='$fix/node()'/></xsl:variable>

  <xsl:if test='$fix'>
    <xsl:choose>
      <xsl:when test='count($fix//cdf:instance) != 0'>
        <xsl:variable name='msg'>Skipping fix generation for rule <xsl:value-of select='@id'/> because it contains instance substitutions</xsl:variable>
        <xsl:message><xsl:value-of select='$msg'/></xsl:message>
        <xsl:call-template name='linecomment'><xsl:with-param name='content' select='$msg'/></xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name='tpl.elem'>
          <xsl:with-param name='elem' select='"fixentry"'/>
          <xsl:with-param name='val'  select='string($val)'/>
          <xsl:with-param name='fix'  select='$fix'/>
          <xsl:with-param name='rule' select='.'/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template match='cdf:rule-result'>
  <xsl:variable name='rule' select='key("item", @idref)'/>
  <xsl:variable name='fix' select='(cdf:fix|$rule/cdf:fix)[contains($sys, concat(" ", @system, ""))][last()]'/>
  <xsl:variable name='val'><xsl:apply-templates mode='text' select='$fix/node()'/></xsl:variable>

  <xsl:if test='$fix'>
    <xsl:call-template name='tpl.elem'>
      <xsl:with-param name='elem'   select='"fixentry"'/>
      <xsl:with-param name='val'    select='string($val)'/>
      <xsl:with-param name='fix'    select='$fix'/>
      <xsl:with-param name='rule'   select='$rule'/>
      <xsl:with-param name='result' select='.'/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name='linecomment'>
  <xsl:param name='content'/>
  <xsl:variable name='cnt' select='normalize-space($content)'/>
  <xsl:variable name='commentbeg' select='$tpl/tpl:fixtemplate/tpl:linecomment[1]'/>
  <xsl:if test='$cnt and $commentbeg'>
    <xsl:value-of select='$commentbeg'/><xsl:value-of select='$cnt'/><xsl:text>&#x0a;</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match='text()'><xsl:value-of select='normalize-space(.)'/></xsl:template>

<!-- fixing template execution engine -->
<xsl:template name='tpl.elem'>
  <xsl:param name='elem'/>
  <xsl:param name='val'/>
  <xsl:param name='fix'/>
  <xsl:param name='rule'/>
  <xsl:param name='result'/>

  <xsl:apply-templates mode='fixtpl' select='$tpl/tpl:fixtemplate/tpl:*[local-name()=$elem][1]'>
    <xsl:with-param name='val' select='$val'/>
    <xsl:with-param name='fix' select='$fix'/>
    <xsl:with-param name='rule' select='$rule'/>
    <xsl:with-param name='result' select='$result'/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template mode='fixtpl' match='tpl:*'>
  <xsl:param name='val'/>
  <xsl:param name='rule'/>
  <xsl:param name='fix'/>
  <xsl:param name='result'/>
  <xsl:apply-templates mode='fixtpl' select='node()'>
    <xsl:with-param name='val'       select='$val'/>
    <xsl:with-param name='rule'      select='$rule'/>
    <xsl:with-param name='fix'       select='$fix'/>
    <xsl:with-param name='result'    select='$result'/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template mode='fixtpl' match='tpl:sub'>
  <xsl:param name='val'/>
  <xsl:param name='rule'/>
  <xsl:param name='fix'/>
  <xsl:param name='result'/>
  <xsl:value-of select='dyn:evaluate(@select)'/>
</xsl:template>

<xsl:template mode='fixtpl' match='tpl:if'>
  <xsl:param name='val'/>
  <xsl:param name='rule'/>
  <xsl:param name='fix'/>
  <xsl:param name='result'/>
  <xsl:if test='dyn:evaluate(@test)'>
    <xsl:apply-templates mode='fixtpl' select='node()'>
      <xsl:with-param name='val'       select='$val'/>
      <xsl:with-param name='rule'      select='$rule'/>
      <xsl:with-param name='fix'       select='$fix'/>
      <xsl:with-param name='result'    select='$result'/>
    </xsl:apply-templates>
  </xsl:if>
</xsl:template>

<xsl:template mode='fixtpl' match='text()'><xsl:value-of select='.'/></xsl:template>


</xsl:stylesheet>
