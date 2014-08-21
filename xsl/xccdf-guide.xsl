<?xml version="1.0" encoding="UTF-8" ?>
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

<xsl:stylesheet version="1.1"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.2"
    exclude-result-prefixes="xsl cdf">

<xsl:include href="xccdf-guide-impl.xsl" />

<xsl:param name="benchmark_id"/>
<xsl:param name="profile_id"/>

<!-- main(..) -->
<xsl:template match="/">
    <xsl:choose>
        <xsl:when test="$benchmark_id">
            <xsl:variable name="benchmark" select="//cdf:Benchmark[@id = $benchmark_id][1]"/>
            <xsl:choose>
                <xsl:when test="not($benchmark)">
                    <xsl:message terminate="yes">Can't find benchmark of ID '<xsl:value-of select="$benchmark_id"/>'!</xsl:message>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:call-template name="generate-guide">
                        <xsl:with-param name="benchmark" select="$benchmark"/>
                        <xsl:with-param name="profile_id" select="$profile_id"/>
                    </xsl:call-template>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
            <xsl:message>The 'benchmark_id' parameter was not supplied. Using the first cdf:Benchmark found!</xsl:message>
            <xsl:variable name="benchmark" select="//cdf:Benchmark[1]"/>
            <xsl:choose>
                <xsl:when test="not($benchmark)">
                    <xsl:message terminate="yes">Can't find any cdf:Benchmark elements!</xsl:message>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:call-template name="generate-guide">
                        <xsl:with-param name="benchmark" select="$benchmark"/>
                        <xsl:with-param name="profile_id" select="$profile_id"/>
                    </xsl:call-template>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>
