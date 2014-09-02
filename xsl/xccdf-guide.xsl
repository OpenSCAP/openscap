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
    <xsl:variable name="final_benchmark_id">
        <xsl:choose>
            <xsl:when test="$benchmark_id">
                <xsl:value-of select="$benchmark_id"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:if test="$verbosity">
                    <xsl:message>The 'benchmark_id' parameter was not supplied. Using the first cdf:Benchmark found!</xsl:message>
                </xsl:if>
                <xsl:value-of select="//cdf:Benchmark[1]/@id"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:variable name="benchmark" select="//cdf:Benchmark[@id = $final_benchmark_id][1]"/>
    <!-- Empty $profile_id is a valid use case! It's the default profile. -->

    <xsl:if test="not($benchmark)">
        <xsl:choose>
            <xsl:when test="$benchmark_id">
                <xsl:message terminate="yes">No such cdf:Benchmark exists (with @id = "<xsl:value-of select="$benchmark_id"/>")</xsl:message>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">No cdf:Benchmark ID specified and no suitable candidate has been autodetected.</xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:if>

    <xsl:if test="$verbosity">
        <xsl:message>Benchmark ID: <xsl:value-of select="$final_benchmark_id"/></xsl:message>
        <xsl:choose>
            <xsl:when test="$profile_id">
                <xsl:message>Profile ID: <xsl:value-of select="$profile_id"/></xsl:message>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message>Profile: (default)</xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:if>

    <xsl:call-template name="generate-guide">
        <xsl:with-param name="benchmark" select="$benchmark"/>
        <xsl:with-param name="profile_id" select="$profile_id"/>
    </xsl:call-template>
</xsl:template>

</xsl:stylesheet>
