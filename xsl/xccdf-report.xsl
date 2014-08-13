<?xml version="1.0" encoding="utf-8" ?>

<!--
Copyright 2010-2014 Red Hat Inc., Durham, North Carolina.
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

<!--
This stylesheet is the entry point for HTML report generator.

The goal is to negotiate the right namespace and find the XPaths for TestResult
and Benchmark. All the logic to do that should be in this stylesheet and this
stylesheet only.
-->

<xsl:stylesheet version="1.1"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.2"
    xmlns:exsl="http://exslt.org/common"
    xmlns:s="http://open-scap.org/"
    exclude-result-prefixes="xsl cdf s exsl">

<xsl:include href="xccdf-report-impl.xsl" />

<xsl:output
    method="html"
    encoding="utf-8"
    indent="no"
    omit-xml-declaration="yes"/>

<!-- parameters -->
<xsl:param name="testresult_id"/>

<!-- OVAL and SCE result parameters -->
<xsl:param name='pwd'/>
<xsl:param name='oval-template'/>
<xsl:param name='sce-template'/>

<xsl:variable name='oval-tmpl'>
  <xsl:choose>
    <xsl:when test='not($oval-template)' />
    <xsl:when test='substring($oval-template, 1, 1) = "/"'><xsl:value-of select='$oval-template'/></xsl:when>
    <xsl:otherwise><xsl:value-of select='concat($pwd, "/", $oval-template)'/></xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name='sce-tmpl'>
  <xsl:choose>
    <xsl:when test='not($sce-template)' />
    <xsl:when test='substring($sce-template, 1, 1) = "/"'><xsl:value-of select='$sce-template'/></xsl:when>
    <xsl:otherwise><xsl:value-of select='concat($pwd, "/", $sce-template)'/></xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<!-- keys -->
<xsl:key name="items" match="cdf:Group|cdf:Rule|cdf:Value" use="@id"/>
<xsl:key name="profiles" match="cdf:Profile" use="@id"/>

<!-- main(..) -->
<xsl:template match="/">
    <xsl:variable name='end_times'>
        <s:times>
            <xsl:for-each select='//cdf:TestResult/@end-time'>
                <xsl:sort order='descending'/>
                <s:t t='{.}'/>
            </xsl:for-each>
        </s:times>
    </xsl:variable>

    <xsl:variable name='last_test_time' select='exsl:node-set($end_times)/s:times/s:t[1]/@t'/>

    <xsl:variable name='final_result_id'>
        <xsl:choose>
            <xsl:when test="$testresult_id">
                <xsl:value-of select="$testresult_id"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="//cdf:TestResult[@end-time=$last_test_time][last()]/@id"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:variable name="testresult" select="//cdf:TestResult[@id=$final_result_id]"/>
    <!-- TODO: TEMPORARY! Only works in trivial cases. -->
    <xsl:variable name="benchmark" select="/cdf:Benchmark"/>

    <xsl:choose>
        <xsl:when test="$testresult">
            <xsl:message>TestResult ID: <xsl:value-of select="$final_result_id"/></xsl:message>
            <xsl:call-template name="generate-report">
                <xsl:with-param name="testresult" select="$testresult"/>
                <xsl:with-param name="benchmark" select="$benchmark"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:when test="$testresult_id">
            <xsl:message terminate="yes">No such result exists.</xsl:message>
        </xsl:when>
        <xsl:otherwise>
            <xsl:message terminate="yes">No result ID specified.</xsl:message>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>
