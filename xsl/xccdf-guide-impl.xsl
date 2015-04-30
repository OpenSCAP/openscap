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

<!-- TODO
  - do not display hidden and abstract items
  - profile info?
-->

<xsl:stylesheet version="1.1"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:cdf="http://checklists.nist.gov/xccdf/1.2"
    exclude-result-prefixes="xsl cdf">

<xsl:include href="xccdf-branding.xsl" />
<xsl:include href="xccdf-resources.xsl" />
<xsl:include href="xccdf-share.xsl" />

<xsl:output
    method="html"
    encoding="utf-8"
    indent="no"
    omit-xml-declaration="yes"/>

<xsl:template name="introduction">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <div id="introduction">
        <div class="row">
            <div class="col-md-8 well well-lg">
                <h2>
                    <xsl:choose>
                        <xsl:when test="$benchmark/cdf:title">
                            <xsl:apply-templates mode="sub-testresult" select="$benchmark/cdf:title[1]">
                                <xsl:with-param name="benchmark" select="$benchmark"/>
                                <xsl:with-param name="profile" select="$profile"/>
                            </xsl:apply-templates>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="$benchmark/@id"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </h2>

                <xsl:if test="$benchmark/cdf:front-matter">
                    <div class="front-matter">
                        <xsl:apply-templates mode="sub-testresult" select="$benchmark/cdf:front-matter[1]">
                            <xsl:with-param name="benchmark" select="$benchmark"/>
                            <xsl:with-param name="profile" select="$profile"/>
                        </xsl:apply-templates>
                    </div>
                </xsl:if>
                <xsl:if test="$benchmark/cdf:description">
                    <div class="description">
                        <xsl:apply-templates mode="sub-testresult" select="$benchmark/cdf:description[1]">
                            <xsl:with-param name="benchmark" select="$benchmark"/>
                            <xsl:with-param name="profile" select="$profile"/>
                        </xsl:apply-templates>
                    </div>
                </xsl:if>
                <xsl:if test="$benchmark/cdf:notice">
                    <div class="top-spacer-10">
                        <xsl:for-each select="$benchmark/cdf:notice">
                            <div class="alert alert-info">
                                <xsl:apply-templates mode="sub-testresult" select=".">
                                    <xsl:with-param name="benchmark" select="$benchmark"/>
                                    <xsl:with-param name="profile" select="$profile"/>
                                </xsl:apply-templates>
                            </div>
                        </xsl:for-each>
                    </div>
                </xsl:if>

                <table class="table table-bordered">
                    <xsl:if test="$profile/cdf:title">
                        <tr>
                            <th>Profile Title</th>
                            <td>
                                <xsl:apply-templates mode="sub-testresult" select="$profile/cdf:title[1]">
                                    <xsl:with-param name="benchmark" select="$benchmark"/>
                                    <xsl:with-param name="profile" select="$profile"/>
                                </xsl:apply-templates>
                            </td>
                        </tr>
                    </xsl:if>

                    <tr>
                        <th>Profile ID</th>
                        <td>
                            <xsl:choose>
                                <xsl:when test="$profile/@id">
                                    <xsl:value-of select="$profile/@id"/>
                                </xsl:when>
                                <xsl:otherwise>
                                    <abbr title="No profile was selected.">(default)</abbr>
                                </xsl:otherwise>
                            </xsl:choose>
                        </td>
                    </tr>
                </table>
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
    </div>
</xsl:template>

<xsl:template name="is-item-selected-final">
    <xsl:param name="item"/>
    <xsl:param name="profile"/>

    <!--
    assumptions of this template:
    1) The benchmark is fully resolved
    2) is-item-selected-final is called for Groups and only if it returns true it's called for child items
    -->

    <xsl:choose>
        <xsl:when test="$profile and $profile/cdf:select[@idref = $item/@id]">
            <xsl:value-of select="$profile/cdf:select[@idref = $item/@id][last()]/@selected"/>
        </xsl:when>
        <xsl:when test="$item/@selected">
            <xsl:value-of select="$item/@selected"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="'true'"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="guide-tree-leaf">
    <xsl:param name="item"/>
    <xsl:param name="profile"/>
    <xsl:param name="indent"/>

    <xsl:variable name="selected_final">
        <xsl:call-template name="is-item-selected-final">
            <xsl:with-param name="item" select="$item"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:call-template>
    </xsl:variable>

    <xsl:if test="$selected_final = 'true'">
        <tr data-tt-id="{$item/@id}" class="guide-tree-leaf guide-tree-leaf-id-{$item/@id}" id="guide-tree-leaf-{generate-id($item)}">
            <xsl:attribute name="data-tt-parent-id">
                <xsl:value-of select="concat('children-', $item/parent::cdf:*/@id)"/>
            </xsl:attribute>

            <td style="padding-left: {$indent * 19}px">
                <h4>
                    <xsl:call-template name="item-title">
                        <xsl:with-param name="item" select="$item"/>
                        <xsl:with-param name="profile" select="$profile"/>
                    </xsl:call-template>
                    <span class="label label-default pull-right">rule</span>
                </h4>

                <xsl:if test="$item/cdf:description">
                    <p>
                        <xsl:apply-templates mode="sub-testresult" select="$item/cdf:description">
                            <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                            <xsl:with-param name="profile" select="$profile"/>
                        </xsl:apply-templates>
                    </p>
                </xsl:if>

                <xsl:for-each select="$item/cdf:warning">
                    <div class="panel panel-warning">
                        <div class="panel-heading">
                            <span class="label label-warning">warning</span>&#160;
                            <xsl:apply-templates mode="sub-testresult" select=".">
                                <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                                <xsl:with-param name="profile" select="$profile"/>
                            </xsl:apply-templates>
                        </div>
                    </div>
                </xsl:for-each>

                <xsl:if test="$item/cdf:rationale">
                    <span class="label label-primary">Rationale:</span>
                    <p>
                        <xsl:apply-templates mode="sub-testresult" select="$item/cdf:rationale">
                            <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                            <xsl:with-param name="profile" select="$profile"/>
                        </xsl:apply-templates>
                    </p>
                </xsl:if>

                <xsl:call-template name="item-idents-refs">
                    <xsl:with-param name="item" select="$item"/>
                </xsl:call-template>

                <xsl:for-each select="$item/cdf:fixtext">
                    <span class="label label-success">Remediation description:</span>
                    <div class="panel panel-default"><div class="panel-body">
                        <xsl:call-template name="show-fixtext">
                            <xsl:with-param name="fixtext" select="."/>
                            <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                            <xsl:with-param name="profile" select="$profile"/>
                        </xsl:call-template>
                    </div></div>
                </xsl:for-each>

                <xsl:for-each select="$item/cdf:fix">
                    <span class="label label-success">Remediation script:</span>
                    <xsl:call-template name="show-fix">
                        <xsl:with-param name="fix" select="."/>
                        <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                        <xsl:with-param name="profile" select="$profile"/>
                    </xsl:call-template>
                </xsl:for-each>
            </td>
        </tr>
    </xsl:if>
</xsl:template>

<xsl:template name="substring-count">
    <xsl:param name="string"/>
    <xsl:param name="substr"/>
    <xsl:choose>
        <xsl:when test="contains($string, $substr) and $string and $substr">
            <xsl:variable name="rest">
                <xsl:call-template name="substring-count">
                    <xsl:with-param name="string" select="substring-after($string, $substr)"/>
                    <xsl:with-param name="substr" select="$substr"/>
                </xsl:call-template>
            </xsl:variable>
            <xsl:value-of select="$rest + 1"/>
        </xsl:when>
        <xsl:otherwise>0</xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="guide-count-contained-selected-rules-impl">
    <xsl:param name="item"/>
    <xsl:param name="profile"/>

    <xsl:variable name="selected_final">
        <xsl:call-template name="is-item-selected-final">
            <xsl:with-param name="item" select="$item"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:call-template>
    </xsl:variable>

    <xsl:if test="$selected_final = 'true'">
        <xsl:for-each select="$item/cdf:Group">
            <xsl:call-template name="guide-count-contained-selected-rules-impl">
                <xsl:with-param name="item" select="."/>
                <xsl:with-param name="profile" select="$profile"/>
            </xsl:call-template>
        </xsl:for-each>

        <xsl:for-each select="$item/cdf:Rule">
            <xsl:variable name="rule" select="."/>

            <xsl:variable name="rule_selected_final">
                <xsl:call-template name="is-item-selected-final">
                    <xsl:with-param name="item" select="$rule"/>
                    <xsl:with-param name="profile" select="$profile"/>
                </xsl:call-template>
            </xsl:variable>

            <xsl:if test="$rule_selected_final = 'true'">
                RULE
            </xsl:if>
        </xsl:for-each>
    </xsl:if>
</xsl:template>

<xsl:template name="guide-count-contained-selected-rules">
    <!-- XSLT is a functional language, you cannot "update"
         variables. That's why we recursivelly build a huge
         string containing 'RULE' for each selected RULE.
         We then cound the number of RULES to get the number
         we need. -->

    <xsl:param name="item"/>
    <xsl:param name="profile"/>

    <xsl:variable name="impl-ret">
        <xsl:call-template name="guide-count-contained-selected-rules-impl">
            <xsl:with-param name="item" select="$item"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:call-template>
    </xsl:variable>

    <xsl:call-template name="substring-count">
        <xsl:with-param name="string" select="$impl-ret"/>
        <xsl:with-param name="substr" select="'RULE'"/>
    </xsl:call-template>
</xsl:template>

<xsl:template name="guide-tree-inner-node">
    <xsl:param name="item"/>
    <xsl:param name="profile"/>
    <xsl:param name="indent"/>

    <xsl:variable name="selected_final">
        <xsl:call-template name="is-item-selected-final">
            <xsl:with-param name="item" select="$item"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:call-template>
    </xsl:variable>

    <xsl:if test="$selected_final = 'true'">
        <xsl:variable name="contained_rules">
            <xsl:call-template name="guide-count-contained-selected-rules">
                <xsl:with-param name="item" select="$item"/>
                <xsl:with-param name="profile" select="$profile"/>
            </xsl:call-template>
        </xsl:variable>

        <xsl:if test="not($item/self::cdf:Benchmark)">
            <tr data-tt-id="{$item/@id}" class="guide-tree-inner-node guide-tree-inner-node-id-{$item/@id}">
                <xsl:if test="$item/parent::cdf:Group or $item/parent::cdf:Benchmark">
                    <xsl:attribute name="data-tt-parent-id">
                        <xsl:value-of select="concat('children-', $item/parent::cdf:*/@id)"/>
                    </xsl:attribute>
                </xsl:if>

                <td style="padding-left: {$indent * 19}px">
                    <h3>
                        <xsl:if test="$indent=1 or $indent=2">
                            <xsl:attribute name="id">
                                <xsl:value-of select="$item/@id"/>
                            </xsl:attribute>
                        </xsl:if>
                        <xsl:call-template name="item-title">
                            <xsl:with-param name="item" select="$item"/>
                            <xsl:with-param name="profile" select="$profile"/>
                        </xsl:call-template>
                        <span class="label label-default pull-right">group</span>
                    </h3>

                    <p>
                        <xsl:apply-templates mode="sub-testresult" select="$item/cdf:description">
                            <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                            <xsl:with-param name="profile" select="$profile"/>
                        </xsl:apply-templates>
                    </p>

                    <xsl:for-each select="$item/cdf:warning">
                        <div class="panel panel-warning">
                            <div class="panel-heading">
                                <span class="label label-warning">warning</span>&#160;
                                <xsl:apply-templates mode="sub-testresult" select=".">
                                    <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                                    <xsl:with-param name="profile" select="$profile"/>
                                </xsl:apply-templates>
                            </div>
                        </div>
                    </xsl:for-each>

                    <xsl:call-template name="item-idents-refs">
                        <xsl:with-param name="item" select="$item"/>
                    </xsl:call-template>
                </td>
            </tr>
        </xsl:if>

        <tr data-tt-id="children-{$item/@id}">
            <xsl:if test="$item/parent::cdf:Group or $item/parent::cdf:Benchmark">
                <xsl:attribute name="data-tt-parent-id">
                    <xsl:value-of select="concat('children-', $item/parent::cdf:*/@id)"/>
                </xsl:attribute>
            </xsl:if>

            <td style="padding-left: {$indent * 19}px">
                <xsl:choose>
                    <xsl:when test="$contained_rules > 1">
                        <small>contains <xsl:value-of select="$contained_rules"/> rules</small>
                    </xsl:when>
                    <xsl:when test="$contained_rules = 1">
                        <small>contains <xsl:value-of select="$contained_rules"/> rule</small>
                    </xsl:when>
                    <xsl:otherwise>
                    </xsl:otherwise>
                </xsl:choose>
            </td>
        </tr>

        <xsl:for-each select="$item/cdf:Group">
            <xsl:call-template name="guide-tree-inner-node">
                <xsl:with-param name="item" select="."/>
                <xsl:with-param name="profile" select="$profile"/>
                <xsl:with-param name="indent" select="$indent + 1"/>
            </xsl:call-template>
        </xsl:for-each>

        <xsl:for-each select="$item/cdf:Rule">
            <xsl:call-template name="guide-tree-leaf">
                <xsl:with-param name="item" select="."/>
                <xsl:with-param name="profile" select="$profile"/>
                <xsl:with-param name="indent" select="$indent + 1"/>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:if>
</xsl:template>

<xsl:template name="table-of-contents-items">
    <xsl:param name="item"/>
    <xsl:param name="levels"/>
    <ol>
        <xsl:for-each select="$item/cdf:Group">
            <li>
                <a>
                    <xsl:attribute name="href">
                        <xsl:text>#</xsl:text>
                        <xsl:value-of select="@id"/>
                    </xsl:attribute>
                    <xsl:choose>
                        <xsl:when test="cdf:title">
                            <xsl:value-of select="cdf:title"/>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="@id"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </a>
            </li>
            <xsl:if test="cdf:Group and $levels&gt;1">
                <xsl:call-template name="table-of-contents-items">
                    <xsl:with-param name="item" select="."/>
                    <xsl:with-param name="levels" select="$levels - 1"/>
                </xsl:call-template>
            </xsl:if>
        </xsl:for-each>
    </ol>
</xsl:template>

<xsl:template name="table-of-contents">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>
    <h2>Table of Contents</h2>
    <xsl:call-template name="table-of-contents-items">
        <xsl:with-param name="item" select="$benchmark"/>
        <xsl:with-param name="levels" select="2"/>
    </xsl:call-template>
</xsl:template>

<xsl:template name="guide-tree">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <div id="guide-tree">
        <h2>Checklist</h2>

        <table class="treetable table table-bordered">
            <tbody>
                <xsl:call-template name="guide-tree-inner-node">
                    <xsl:with-param name="item" select="$benchmark"/>
                    <xsl:with-param name="profile" select="$profile"/>
                    <xsl:with-param name="indent" select="0"/>
                </xsl:call-template>
            </tbody>
        </table>
    </div>
</xsl:template>

<xsl:template name="generate-guide">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile_id"/>

    <xsl:variable name="profile" select="$benchmark/cdf:Profile[@id = $profile_id]"/>

    <xsl:text disable-output-escaping='yes'>&lt;!DOCTYPE html></xsl:text>
    <html lang="en">
    <head>
        <meta charset="utf-8"/>
        <meta http-equiv="X-UA-Compatible" content="IE=edge"/>
        <meta name="viewport" content="width=device-width, initial-scale=1"/>
        <title>
            <xsl:call-template name="item-title">
                <xsl:with-param name="item" select="$benchmark"/>
                <xsl:with-param name="profile" select="$profile"/>
            </xsl:call-template> | OpenSCAP Security Guide
        </title>

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
    <xsl:call-template name="table-of-contents">
        <xsl:with-param name="benchmark" select="$benchmark"/>
        <xsl:with-param name="profile" select="$profile"/>
    </xsl:call-template>
    <xsl:call-template name="guide-tree">
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
