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
    xmlns:ovalres="http://oval.mitre.org/XMLSchema/oval-results-5"
    xmlns:sceres="http://open-scap.org/page/SCE_result_file"
    xmlns:exsl="http://exslt.org/common"
    xmlns:arf="http://scap.nist.gov/schema/asset-reporting-format/1.1"
    exclude-result-prefixes="xsl cdf ovalres sceres exsl">

<!-- This selects all the references, even if the SDS has multiple benchmarks.
     That is fine because we will go through just the benchmark references
     and only then we compare to this map. So this is correct.
     See template "get-all-references". -->
<xsl:key name="references" match="//cdf:reference" use="@href"/>

<xsl:template name="convert-reference-url-to-name">
    <xsl:param name="href"/>
    <xsl:choose>
        <xsl:when test="starts-with($href, 'http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-53')">
            <xsl:text>NIST SP 800-53</xsl:text>
        </xsl:when>
        <xsl:when test="starts-with($href, 'http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-171')">
            <xsl:text>NIST SP 800-171</xsl:text>
        </xsl:when>
        <xsl:when test="starts-with($href, 'http://iase.disa.mil/stigs/cci/')">
            <xsl:text>DISA CCI</xsl:text>
        </xsl:when>
        <!-- SRG weblinks can be subject to change. Keep the old ones for compatibility and add any new ones -->
        <xsl:when test="starts-with($href, 'http://iase.disa.mil/stigs/srgs/') or 
                starts-with($href, 'http://iase.disa.mil/stigs/os/general/Pages/index.aspx') or
                starts-with($href, 'http://iase.disa.mil/stigs/app-security/app-servers/Pages/general.aspx')">
            <xsl:text>DISA SRG</xsl:text>
        </xsl:when>
        <!-- STIG weblinks can be subject to change. Keep the old ones for compatibility and add any new ones -->
        <xsl:when test="starts-with($href, 'http://iase.disa.mil/stigs/os/') or
                starts-with($href, 'http://iase.disa.mil/stigs/app-security/')">
            <xsl:text>DISA STIG</xsl:text>
        </xsl:when>
        <xsl:when test="starts-with($href, 'https://www.pcisecuritystandards.org/')">
            <xsl:text>PCI-DSS Requirement</xsl:text>
        </xsl:when>
        <xsl:when test="starts-with($href, 'https://benchmarks.cisecurity.org/')">
            <xsl:text>CIS Recommendation</xsl:text>
        </xsl:when>
        <xsl:when test="starts-with($href, 'https://www.fbi.gov/file-repository/cjis-security-policy')">
            <xsl:text>FBI CJIS</xsl:text>
        </xsl:when>
        <xsl:when test="starts-with($href, 'http://www.ssi.gouv.fr/administration/bonnes-pratiques')">
            <xsl:text>ANSSI</xsl:text>
        </xsl:when>
        <xsl:when test="starts-with($href, 'https://www.gpo.gov/fdsys/pkg/CFR-2007-title45-vol1')">
            <xsl:text>HIPAA</xsl:text>
        </xsl:when>
        <xsl:when test="starts-with($href, 'https://www.iso.org/standard/54534.html')">
            <xsl:text>ISO 27001-2013</xsl:text>
        </xsl:when>
        <xsl:when test="starts-with($href, 'https://iase.disa.mil/stigs/pages/stig-viewing-guidance')">
            <xsl:text>STIG Viewer</xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$href"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="get-all-references">
    <xsl:param name="benchmark"/>
    <xsl:for-each select="$benchmark//cdf:reference[generate-id(.) = generate-id(key('references',@href)[1])]">
        <xsl:sort select="@href" />
        <xsl:if test="normalize-space(@href) and @href != 'https://github.com/OpenSCAP/scap-security-guide/wiki/Contributors'">
            <option>
                <xsl:variable name="reference">
                    <xsl:call-template name="convert-reference-url-to-name">
                        <xsl:with-param name="href" select="@href"/>
                    </xsl:call-template>
                </xsl:variable>
                <xsl:attribute name="value">
                    <xsl:value-of select="$reference"/>
                </xsl:attribute>
                <xsl:value-of select="$reference"/>
            </option>
        </xsl:if>
    </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
