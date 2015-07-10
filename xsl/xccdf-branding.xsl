<?xml version="1.0" encoding="utf-8" ?>

<!--
Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
-->

<!--
The purpose of this file is to allow downstreams to patch it to provide
their own branding. For example a distribution might want to add their
logo to the HTML report / guide header.
-->

<xsl:stylesheet version="1.1"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.2">

<xsl:param name="oscap-version"/>

<xsl:template name="xccdf-branding-logo">
    <!--
    This is the openscap logo SVG.

    These are the steps I used to generate this embedded SVG snippet
    1) Round all positions to 1 decimal place
    2) Remove inkscape, RDF, DC and other metadata
    -->

<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="52" height="52" id="svg2"><g transform="matrix(0.75266991,0,0,0.75266991,-17.752968,-104.57468)" id="g32"><path d="m 24.7,173.5 c 0,-9 3.5,-17.5 9.9,-23.9 6.8,-6.8 15.7,-10.4 25,-10 8.6,0.3 16.9,3.9 22.9,9.8 6.4,6.4 9.9,14.9 10,23.8 0.1,9.1 -3.5,17.8 -10,24.3 -13.2,13.2 -34.7,13.1 -48,-0.1 -1.5,-1.5 -1.9,-4.2 0.2,-6.2 l 9,-9 c -2,-3.6 -4.9,-13.1 2.6,-20.7 7.6,-7.6 18.6,-6 24.4,-0.2 3.3,3.3 5.1,7.6 5.1,12.1 0.1,4.6 -1.8,9.1 -5.3,12.5 -4.2,4.2 -10.2,5.8 -16.1,4.4 -1.5,-0.4 -2.4,-1.9 -2.1,-3.4 0.4,-1.5 1.9,-2.4 3.4,-2.1 4.1,1 8,-0.1 10.9,-2.9 2.3,-2.3 3.6,-5.3 3.6,-8.4 0,0 0,-0.1 0,-0.1 0,-3 -1.3,-5.9 -3.5,-8.2 -3.9,-3.9 -11.3,-4.9 -16.5,0.2 -6.3,6.3 -1.6,14.1 -1.6,14.2 1.5,2.4 0.7,5 -0.9,6.3 l -8.4,8.4 c 9.9,8.9 27.2,11.2 39.1,-0.8 5.4,-5.4 8.4,-12.5 8.4,-20 0,-0.1 0,-0.2 0,-0.3 -0.1,-7.5 -3,-14.6 -8.4,-19.9 -5,-5 -11.9,-8 -19.1,-8.2 -7.8,-0.3 -15.2,2.7 -20.9,8.4 -8.7,8.7 -8.7,19 -7.9,24.3 0.3,2.4 1.1,4.9 2.2,7.3 0.6,1.4 0,3.1 -1.4,3.7 -1.4,0.6 -3.1,0 -3.7,-1.4 -1.3,-2.9 -2.2,-5.8 -2.6,-8.7 -0.3,-1.7 -0.4,-3.5 -0.4,-5.2 z" id="path34" style="fill:#12497f"/></g></svg>
</xsl:template>

<xsl:template name="xccdf-report-header">
    <nav class="navbar navbar-default" role="navigation">
        <div class="navbar-header" style="float: none">
            <a class="navbar-brand" href="#">
                <xsl:call-template name="xccdf-branding-logo"/>
            </a>
            <div><h1>OpenSCAP Evaluation Report</h1></div>
        </div>
    </nav>
</xsl:template>

<xsl:template name="xccdf-report-footer">
    <footer id="footer">
        <div class="container">
            <p class="muted credit">
                Generated using <a href="http://open-scap.org">OpenSCAP</a>
                <xsl:if test="$oscap-version">
                    <xsl:value-of select="concat(' ', $oscap-version)"/>
                </xsl:if>
            </p>
        </div>
    </footer>
</xsl:template>

<xsl:template name="xccdf-guide-header">
    <nav class="navbar navbar-default" role="navigation">
        <div class="navbar-header" style="float: none">
            <a class="navbar-brand" href="#">
                <xsl:call-template name="xccdf-branding-logo"/>
            </a>
            <div><h1>OpenSCAP Security Guide</h1></div>
        </div>
    </nav>
</xsl:template>

<xsl:template name="xccdf-guide-footer">
    <footer id="footer">
        <div class="container">
            <p class="muted credit">
                Generated using <a href="http://open-scap.org">OpenSCAP</a>
                <xsl:if test="$oscap-version">
                    <xsl:value-of select="concat(' ', $oscap-version)"/>
                </xsl:if>
            </p>
        </div>
    </footer>
</xsl:template>

</xsl:stylesheet>
