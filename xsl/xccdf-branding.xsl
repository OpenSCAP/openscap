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

<xsl:template name="xccdf-branding-logo">
    <!--
    This is the openscap logo SVG.

    These are the steps I used to generate this embedded SVG snippet
    1) Round all positions to 1 decimal place
    2) Remove inkscape, RDF, DC and other metadata
    3) Move repeated style declarations into CSS classes
    -->
    <svg xmlns="http://www.w3.org/2000/svg" version="1.1" x="0px" y="0px" width="240" height="55"><style>.style0{fill:#38393f;}.style1{fill:#12497f;}</style><defs/><g transform="matrix(0.75266991,0,0,0.75266991,-16.573637,-103.09218)"><g><g><path d="m 112.6 173.8 c 0 -11.6 3 -18.2 13.9 -18.2 10.9 0 13.9 6.6 13.9 18.2 0 12.1 -3 17.9 -13.9 17.9 -10.9 0 -13.9 -6.2 -13.9 -17.9 z m 23.7 0 c 0 -9.5 -1.8 -14.8 -9.9 -14.8 -8.1 0 -9.9 5.2 -9.9 14.8 0 9.6 1.7 14.5 9.9 14.5 8.2 0 9.9 -4.6 9.9 -14.5 z" class="style0"/><path d="m 146.9 165.8 h 3.8 v 1.8 c 0 0 3.9 -2.3 7.6 -2.3 6.4 0 9.2 3.9 9.2 12.9 0 10 -3.3 13.5 -11.1 13.5 -2.4 0 -5 -0.4 -5.7 -0.6 v 11.3 h -3.8 v -36.7 z m 3.8 4.9 v 17.1 c 0.7 0.1 3.3 0.6 5.5 0.6 5.6 0 7.4 -2.9 7.4 -10.2 0 -7 -2.2 -9.5 -5.9 -9.5 -3.5 0 -7 2 -7 2 z" class="style0"/><path d="m 192.5 187.9 0.1 3 c 0 0 -5.8 0.8 -9.9 0.8 -7.6 0 -10.1 -4.4 -10.1 -13.1 0 -9.7 4.2 -13.4 10.6 -13.4 6.8 0 10.2 3.6 10.2 11.9 l -0.2 2.9 h -16.8 c 0 5.4 1.7 8.3 6.7 8.3 3.8 -0 9.4 -0.5 9.4 -0.5 z m -2.8 -10.9 c 0 -6.3 -1.9 -8.5 -6.4 -8.5 -4.3 0 -6.8 2.4 -6.8 8.5 h 13.2 z" class="style0"/><path d="m 199.4 191.2 v -25.4 h 3.8 v 1.8 c 0 0 4.1 -2.3 7.9 -2.3 7 0 8.6 3.5 8.6 12.5 v 13.4 h -3.8 v -13.3 c 0 -6.7 -0.8 -9.2 -5.5 -9.2 -3.7 0 -7.2 1.9 -7.2 1.9 v 20.6 l -3.8 0 0 0 z" class="style0"/></g><g><path d="m 246.7 159.8 c 0 0 -7.1 -0.9 -10 -0.9 -5 0 -7.6 1.8 -7.6 5.7 0 4.6 2.4 5.3 8.6 6.7 7 1.6 10.1 3.3 10.1 9.5 0 7.8 -4.3 10.8 -11.2 10.8 -4.2 0 -11.1 -1.1 -11.1 -1.1 l 0.4 -3.3 c 0 0 6.9 0.9 10.5 0.9 5 0 7.5 -2.2 7.5 -7.1 0 -4 -2.1 -5.1 -7.8 -6.2 -7.3 -1.6 -10.9 -3.3 -10.9 -10 0 -6.8 4.5 -9.5 11.3 -9.5 4.2 0 10.6 1 10.6 1 l -0.4 3.3 z" class="style0"/><path d="m 275.8 190.7 c -2.5 0.5 -6.4 1.1 -9.7 1.1 -10.5 0 -12.9 -6.4 -12.9 -18 0 -11.9 2.3 -18.2 12.9 -18.2 3.6 0 7.5 0.7 9.7 1.1 l -0.2 3.3 c -2.3 -0.4 -6.4 -0.9 -9.1 -0.9 -7.8 0 -9.2 4.7 -9.2 14.7 0 9.8 1.3 14.5 9.3 14.5 2.8 0 6.6 -0.5 9 -0.9 l 0.2 3.4 z" class="style0"/><path d="m 288.8 156.1 h 8.5 l 9.7 35.2 h -3.9 l -2.5 -9.1 h -15 l -2.5 9.1 h -3.9 l 9.7 -35.2 z m -2.4 22.6 h 13.3 l -5.3 -19.3 h -2.6 l -5.3 19.3 z" class="style0"/><path d="m 316.4 179.4 v 11.8 h -3.9 V 156.1 h 12.9 c 7.7 0 11.2 3.7 11.2 11.3 0 7.7 -3.6 12 -11.2 12 h -8.9 z m 8.9 -3.5 c 5.1 0 7.3 -3.1 7.3 -8.5 0 -5.4 -2.1 -7.9 -7.3 -7.9 h -8.9 v 16.4 h 8.9 z" class="style0"/></g></g><g><path d="m 24.7 173.5 c 0 -9 3.5 -17.5 9.9 -23.9 6.8 -6.8 15.7 -10.4 25 -10 8.6 0.3 16.9 3.9 22.9 9.8 6.4 6.4 9.9 14.9 10 23.8 0.1 9.1 -3.5 17.8 -10 24.3 -13.2 13.2 -34.7 13.1 -48 -0.1 -1.5 -1.5 -1.9 -4.2 0.2 -6.2 l 9 -9 c -2 -3.6 -4.9 -13.1 2.6 -20.7 7.6 -7.6 18.6 -6 24.4 -0.2 3.3 3.3 5.1 7.6 5.1 12.1 0.1 4.6 -1.8 9.1 -5.3 12.5 -4.2 4.2 -10.2 5.8 -16.1 4.4 -1.5 -0.4 -2.4 -1.9 -2.1 -3.4 0.4 -1.5 1.9 -2.4 3.4 -2.1 4.1 1 8 -0.1 10.9 -2.9 2.3 -2.3 3.6 -5.3 3.6 -8.4 0 -0 0 -0.1 0 -0.1 -0 -3 -1.3 -5.9 -3.5 -8.2 -3.9 -3.9 -11.3 -4.9 -16.5 0.2 -6.3 6.3 -1.6 14.1 -1.6 14.2 1.5 2.4 0.7 5 -0.9 6.3 l -8.4 8.4 c 9.9 8.9 27.2 11.2 39.1 -0.8 5.4 -5.4 8.4 -12.5 8.4 -20 0 -0.1 0 -0.2 -0 -0.3 -0.1 -7.5 -3 -14.6 -8.4 -19.9 -5 -5 -11.9 -8 -19.1 -8.2 -7.8 -0.3 -15.2 2.7 -20.9 8.4 -8.7 8.7 -8.7 19 -7.9 24.3 0.3 2.4 1.1 4.9 2.2 7.3 0.6 1.4 0 3.1 -1.4 3.7 -1.4 0.6 -3.1 0 -3.7 -1.4 -1.3 -2.9 -2.2 -5.8 -2.6 -8.7 -0.3 -1.7 -0.4 -3.5 -0.4 -5.2 z" class="style1"/></g></g></svg>
</xsl:template>

<xsl:template name="xccdf-report-header">
    <nav class="navbar navbar-default" role="navigation">
        <div class="navbar-header">
            <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse-3">
                <span class="sr-only">Toggle navigation</span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
            </button>
            <a class="navbar-brand" href="#">
                <xsl:call-template name="xccdf-branding-logo"/>
            </a>
            <div style="padding-left: 270px !important"><h1>Evaluation Report</h1></div>
        </div>
        <div class="collapse navbar-collapse navbar-collapse-3">
            <ul class="nav navbar-nav">
                <li>
                    <a href="#characteristics">Characteristics</a>
                </li>
                <li>
                    <a href="#compliance-and-scoring">Compliance and Scoring</a>
                </li>
                <li>
                    <a href="#rule-overview">Rule Overview</a>
                </li>
                <li>
                    <a href="#result-details">Result Details</a>
                </li>
            </ul>
        </div>
    </nav>
</xsl:template>

<xsl:template name="xccdf-report-footer">
    <footer id="footer">
        <div class="container">
            <p class="muted credit">
                <p>Generated using <a href="http://open-scap.org">OpenSCAP</a></p>
            </p>
        </div>
    </footer>
</xsl:template>

</xsl:stylesheet>
