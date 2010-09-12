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
	xmlns="http://docbook.org/ns/docbook"
	xmlns:h="http://www.w3.org/1999/xhtml"
    xmlns:fn="http://www.w3.org/2005/xpath-functions"
    xmlns:exsl="http://exslt.org/common"
    xmlns:func="http://exslt.org/functions"
    xmlns:s="http://open-scap.org/"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
    xmlns:dc="http://purl.org/dc/elements/1.1/"
    xmlns:edate="http://exslt.org/dates-and-times">

<xsl:import href='xccdf-substitute.xsl'/>
<xsl:import href='html2docbook.xsl'/>
<xsl:import href='dc2docbook.xsl'/>
<xsl:import href='xccdf-tailor.xsl'/>

<xsl:param name='profile'/>
<xsl:param name='substitute' select='1'/>

<xsl:variable name='warn-unresolved' select='1'/>

<!-- stage1: apply profile -->
<xsl:variable name='benchmark.s1'>
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

<!-- call default template on the processed root element -->
<xsl:template match='/'>
  <xsl:call-template name='warn-unresolved'/>
  <!-- TODO: postprocess to output format -->
  <xsl:apply-templates select='$root'/>
</xsl:template>

<!-- call the html to docbook transform -->
<xsl:template mode='db' match='cdf:*'>
  <xsl:call-template name='h2db.para'><xsl:with-param name='in' select='.'/></xsl:call-template>
</xsl:template>
<!-- call the dublin-core to docbook transform -->
<xsl:template mode='db' match='cdf:reference|cdf:metadata'>
  <xsl:variable name='dc.input'>
    <xsl:choose>
      <xsl:when test='dc:*'><xsl:copy-of select='.'/></xsl:when>
      <xsl:otherwise>
        <dc:title><xsl:value-of select='.'/></dc:title>
        <xsl:if test='@href'><dc:identifier><xsl:value-of select='@href'/></dc:identifier></xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:apply-templates mode='dc2db' select='exsl:node-set($dc.input)'/>
</xsl:template>

<xsl:template name='warn-unresolved'>
  <xsl:if test='$warn-unresolved and cdf:Benchmark[not(number(@resolved)=1)]'>
    <xsl:message>WARNING: Processing an unresolved XCCDF document. This may have unexpected results.</xsl:message>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
