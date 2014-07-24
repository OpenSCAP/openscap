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

<xsl:template name="xccdf-report-header">
    <nav class="navbar navbar-default" role="navigation">
        <div class="navbar-header">
            <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse-3">
                <span class="sr-only">Toggle navigation</span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
            </button>
            <a class="navbar-brand" href="#"><img src="img/brand.svg" alt="OpenSCAP" /></a>
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
