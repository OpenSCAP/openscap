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

<!-- TODO
  - do not display hidden and abstract items
  - profile info?
-->

<xsl:stylesheet version="1.1"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.2"
    xmlns:exsl="http://exslt.org/common"
    xmlns:s="http://open-scap.org/"
    xmlns:ovalres="http://oval.mitre.org/XMLSchema/oval-results-5"
    xmlns:sceres="http://open-scap.org/page/SCE_result_file"
    exclude-result-prefixes="xsl cdf s exsl ovalres sceres">

<xsl:include href="xccdf-branding.xsl" />
<xsl:include href="xccdf-resources.xsl" />
<xsl:include href="xccdf-share.xsl" />

<xsl:output
    method="html"
    encoding="utf-8"
    indent="no"
    omit-xml-declaration="yes"/>

<xsl:param name="benchmark_id"/>
<xsl:param name="profile_id"/>

<!-- main(..) -->
<xsl:template match="/">
    <xsl:choose>
        <xsl:when test="$benchmark_id">
            <xsl:variable name="benchmark" select="//cdf:Benchmark[@id = $benchmark_id][1]"/>
            <xsl:choose>
                <xsl:when test="not($benchmark)">
                    <xsl:message>Can't find benchmark of ID '<xsl:value-of select="$benchmark_id"/>'!</xsl:message>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:call-template name="generate-guide">
                        <xsl:with-param name="benchmark" select="$benchmark"/>
                    </xsl:call-template>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
            <xsl:message>The 'benchmark_id' parameter was not supplied. Using the first cdf:Benchmark found!</xsl:message>
            <xsl:variable name="benchmark" select="//cdf:Benchmark[1]"/>
            <xsl:choose>
                <xsl:when test="not($benchmark)">
                    <xsl:message>Can't find any cdf:Benchmark elements!</xsl:message>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:call-template name="generate-guide">
                        <xsl:with-param name="benchmark" select="$benchmark"/>
                    </xsl:call-template>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="introduction">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <div id="introduction"><a name="introduction"></a>
        <div class="row">
            <div class="col-md-8 well well-lg">
                <xsl:if test="$benchmark/cdf:front-matter">
                    <div class="front-matter">
                        <xsl:apply-templates mode="sub" select="$benchmark/cdf:front-matter[1]"/>
                    </div>
                </xsl:if>
                <xsl:if test="$benchmark/cdf:description">
                    <h2>Description</h2>
                    <div class="description">
                        <xsl:apply-templates mode="sub" select="$benchmark/cdf:description[1]"/>
                    </div>
                </xsl:if>
                <xsl:if test="$benchmark/cdf:notice">
                    <h2>Notices</h2>
                    <xsl:for-each select="$benchmark/cdf:notice">
                        <div class="alert alert-info">
                            <xsl:value-of select="text()"/>
                        </div>
                    </xsl:for-each>
                </xsl:if>
            </div>
            <div class="col-md-4">
                <h2>Revision History</h2>
                <!-- version is a required element -->
                <p>Current version: <strong><xsl:value-of select="$benchmark/cdf:version[1]"/></strong></p>
                <!-- at least one cdf:status is required -->
                <ul>
                <xsl:for-each select="$benchmark/cdf:status">
                    <xsl:sort select="@date"/>
                    <li>
                        <xsl:choose>
                            <xsl:when test="position() = 1">
                                <strong><xsl:value-of select="text()"/></strong>
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:value-of select="text()"/>
                            </xsl:otherwise>
                        </xsl:choose>
                        <xsl:if test="@date">
                            (as of <xsl:value-of select="@date"/>)
                        </xsl:if>
                    </li>
                </xsl:for-each>
                </ul>

                <h2>Platforms</h2>
                <xsl:choose>
                    <xsl:when test="$benchmark/cdf:platform">
                        <ul class="list-group">
                            <xsl:for-each select="$benchmark/cdf:platform">
                                <li class="list-group-item"><span class="label label-default"><xsl:value-of select="@idref"/></span></li>
                            </xsl:for-each>
                        </ul>
                    </xsl:when>
                    <xsl:otherwise>
                        The benchmark does not specify applicable platforms.
                    </xsl:otherwise>
                </xsl:choose>
            </div>
        </div>
        <xsl:if test="$benchmark/cdf:notice">
            <div class="row">
                <div class="col-md-12 well well-lg">
                </div>
            </div>
        </xsl:if>
    </div>
</xsl:template>

<xsl:template name="rule-overview-leaf">
    <xsl:param name="item"/>
    <xsl:param name="indent"/>

    <tr data-tt-id="{$item/@id}" class="rule-overview-leaf" id="rule-overview-leaf-{generate-id($item)}">
        <xsl:attribute name="data-tt-parent-id">
            <xsl:value-of select="$item/parent::cdf:*/@id"/>
        </xsl:attribute>

        <td style="padding-left: {$indent * 19}px">
            <h4><xsl:value-of select="$item/cdf:title/text()"/></h4>

            <p>
                <xsl:apply-templates mode="sub" select="$item/cdf:description"/>
            </p>

            <xsl:call-template name="item-idents-refs">
                <xsl:with-param name="item" select="$item"/>
            </xsl:call-template>

            <xsl:if test="$item/cdf:fix">
                <span class="label label-success">Remediation script:</span>
                <pre><code>
                    <xsl:apply-templates mode="sub" select="$item/cdf:fix"/>
                </code></pre>
            </xsl:if>
        </td>
    </tr>
</xsl:template>

<xsl:template name="rule-overview-inner-node">
    <xsl:param name="item"/>
    <xsl:param name="indent"/>

    <tr data-tt-id="{$item/@id}">
        <xsl:if test="$item/parent::cdf:Group or $item/parent::cdf:Benchmark">
            <xsl:attribute name="data-tt-parent-id">
                <xsl:value-of select="$item/parent::cdf:*/@id"/>
            </xsl:attribute>
        </xsl:if>

        <td style="padding-left: {$indent * 19}px">
            <h3><xsl:value-of select="$item/cdf:title/text()"/></h3>

            <p>
                <xsl:apply-templates mode="sub" select="$item/cdf:description"/>
            </p>

            <xsl:call-template name="item-idents-refs">
                <xsl:with-param name="item" select="$item"/>
            </xsl:call-template>
        </td>
    </tr>

    <xsl:for-each select="$item/cdf:Group">
        <xsl:call-template name="rule-overview-inner-node">
            <xsl:with-param name="item" select="."/>
            <xsl:with-param name="indent" select="$indent + 1"/>
        </xsl:call-template>
    </xsl:for-each>

    <xsl:for-each select="$item/cdf:Rule">
        <xsl:call-template name="rule-overview-leaf">
            <xsl:with-param name="item" select="."/>
            <xsl:with-param name="indent" select="$indent + 1"/>
        </xsl:call-template>
    </xsl:for-each>
</xsl:template>

<xsl:template name="rule-overview">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <div id="rule-overview"><a name="rule-overview"></a>
        <h2>Rule Overview</h2>

        <div class="form-group js-only">
            <div class="row">
                <div class="col-sm-6">
                    <div class="input-group">
                        <input type="text" class="form-control" placeholder="Search through XCCDF rules" id="search-input" oninput="ruleSearch()"/>

                        <div class="input-group-btn">
                            <button class="btn btn-default" onclick="ruleSearch()">Search</button>
                        </div>
                    </div>
                    <p id="search-matches"></p>
                </div>
            </div>
        </div>

        <table class="treetable table table-striped table-bordered">
            <tbody>
                <xsl:call-template name="rule-overview-inner-node">
                    <xsl:with-param name="item" select="$benchmark"/>
                    <xsl:with-param name="indent" select="0"/>
                </xsl:call-template>
            </tbody>
        </table>
    </div>
</xsl:template>

<xsl:template name="rear-matter">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <div id="rear-matter"><a name="rear-matter"></a>
        <div class="row">
            <div class="col-md-12 well well-lg">
                <xsl:if test="$benchmark/cdf:rear-matter">
                    <div class="rear-matter">
                        <xsl:apply-templates mode="sub" select="$benchmark/cdf:rear-matter[1]"/>
                    </div>
                </xsl:if>
            </div>
        </div>
    </div>
</xsl:template>

<xsl:template name="generate-guide">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile_id"/>

    <xsl:variable name="profile" select="$benchmark/cdf:Profile[@id = $profile_id][1]"/>

    <xsl:text disable-output-escaping='yes'>&lt;!DOCTYPE html></xsl:text>
    <html lang="en">
    <head>
        <meta charset="utf-8"/>
        <meta http-equiv="X-UA-Compatible" content="IE=edge"/>
        <meta name="viewport" content="width=device-width, initial-scale=1"/>
        <title><xsl:value-of select="$benchmark/@id"/> | OpenSCAP Security Guide</title>

        <style><xsl:call-template name="css-sources"/></style>
        <script><xsl:call-template name="js-sources"/></script>
    </head>

    <body>
    <xsl:call-template name="xccdf-guide-header"/>

    <div class="container"><div id="content">

    <xsl:call-template name="introduction">
        <xsl:with-param name="benchmark" select="$benchmark"/>
        <xsl:with-param name="profile" select="$profile"/>
    </xsl:call-template>
    <xsl:call-template name="rule-overview">
        <xsl:with-param name="benchmark" select="$benchmark"/>
        <xsl:with-param name="profile" select="$profile"/>
    </xsl:call-template>
    <xsl:call-template name="rear-matter">
        <xsl:with-param name="benchmark" select="$benchmark"/>
        <xsl:with-param name="profile" select="$profile"/>
    </xsl:call-template>

    </div></div>

    <xsl:call-template name="xccdf-guide-footer"/>

    </body>
    </html>
</xsl:template>

</xsl:stylesheet>
