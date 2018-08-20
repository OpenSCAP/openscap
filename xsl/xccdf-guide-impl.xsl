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
  - do not display abstract items
-->

<xsl:stylesheet version="1.1"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:cdf="http://checklists.nist.gov/xccdf/1.2"
    exclude-result-prefixes="xsl cdf">

<xsl:include href="xccdf-branding.xsl" />
<xsl:include href="xccdf-resources.xsl" />
<xsl:include href="xccdf-share.xsl" />
<xsl:include href="xccdf-references.xsl" />

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
            <xsl:call-template name="show-title-front-matter-description-notices">
                <xsl:with-param name="benchmark" select="$benchmark"/>
                <xsl:with-param name="profile" select="$profile"/>
            </xsl:call-template>
        </div>
    </div>
</xsl:template>

<xsl:template name="profileinfo">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <div id="profileinfo">
        <h2>Profile Information</h2>
        <div class="row">
            <div class="col-md-5 well well-lg horizontal-scroll">
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
            <div class="col-md-3">
                <h4>CPE Platforms</h4>
                <xsl:choose>
                    <xsl:when test="$benchmark/cdf:platform">
                        <ul class="list-group">
                            <xsl:for-each select="$benchmark/cdf:platform">
                                <xsl:variable name="idref" select="@idref"/>
                                <xsl:if test="$benchmark/cdf:platform[@idref=$idref]">
                                  <li class="list-group-item">
                                    <span class="label label-default" title="CPE platform {@idref} is applicable to this Benchmark"><xsl:value-of select="@idref"/></span>
                                  </li>
                                </xsl:if>
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

<xsl:template name="revisionhistory">
    <xsl:param name="benchmark"/>

    <div id="revisionhistory">
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
    </div>
</xsl:template>

<xsl:key name="profile_selects" match="//cdf:select" use="concat(generate-id(ancestor::cdf:Profile), '|', @idref)"/>

<xsl:template name="is-item-selected-final">
    <xsl:param name="item"/>
    <xsl:param name="profile"/>

    <!--
    assumptions of this template:
    1) The benchmark is fully resolved
    2) is-item-selected-final is called for Groups and only if it returns true it's called for child items
    -->

    <xsl:variable name="profile_select" select="key('profile_selects', concat(generate-id($profile), '|', $item/@id))[last()]/@selected"/>

    <xsl:choose>
        <xsl:when test="$profile_select">
            <xsl:value-of select="$profile_select"/>
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
                <table class="table table-striped table-bordered">
                    <tbody>
                        <tr><td colspan="2">
                            <h4>
                                <xsl:attribute name="id">
                                    <xsl:value-of select="$item/@id"/>
                                </xsl:attribute>
                                <span class="label label-default">Rule</span>&#160;&#160;
                                <xsl:call-template name="item-title">
                                    <xsl:with-param name="item" select="$item"/>
                                    <xsl:with-param name="profile" select="$profile"/>
                                </xsl:call-template>
                                &#160;&#160;<a class="small" href="{concat('#', $item/@id)}">[ref]</a>
                            </h4>
                        </td></tr>

                        <tr><td colspan="2">
                            <xsl:if test="$item/cdf:description">
                                <div class="description"><p>
                                    <xsl:apply-templates mode="sub-testresult" select="$item/cdf:description">
                                        <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                                        <xsl:with-param name="profile" select="$profile"/>
                                    </xsl:apply-templates>
                                 </p></div>
                            </xsl:if>

                            <xsl:for-each select="$item/cdf:warning">
                                <div class="panel panel-warning">
                                    <div class="panel-heading">
                                        <span class="label label-warning">Warning:</span>&#160;
                                        <xsl:apply-templates mode="sub-testresult" select=".">
                                            <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                                            <xsl:with-param name="profile" select="$profile"/>
                                        </xsl:apply-templates>
                                    </div>
                                </div>
                            </xsl:for-each>
                        </td></tr>

                        <xsl:if test="$item/cdf:rationale">
                            <tr><td><span class="label label-primary">Rationale:</span></td><td><div class="rationale">
                                <p>
                                    <xsl:apply-templates mode="sub-testresult" select="$item/cdf:rationale">
                                        <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                                        <xsl:with-param name="profile" select="$profile"/>
                                    </xsl:apply-templates>
                                </p>
                            </div></td></tr>
                        </xsl:if>

                        <tr><td><span class="label label-warning">Severity:</span>&#160;</td><td><div class="severity">
                            <xsl:call-template name="item-severity">
                                <xsl:with-param name="item" select="$item" />
                            </xsl:call-template>
                        </div></td></tr>

                        <tr><td>Identifiers and References</td><td class="identifiers">
                            <xsl:call-template name="item-idents-refs">
                                <xsl:with-param name="item" select="$item"/>
                            </xsl:call-template>
                        </td></tr>

                        <tr><td colspan="2"><div class="remediation-description">
                            <xsl:for-each select="$item/cdf:fixtext">
                    
                                <xsl:call-template name="show-fixtext">
                                    <xsl:with-param name="fixtext" select="."/>
                                    <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                                    <xsl:with-param name="profile" select="$profile"/>
                                </xsl:call-template>
                    
                            </xsl:for-each>

                            <xsl:for-each select="$item/cdf:fix">
                    
                                <xsl:call-template name="show-fix">
                                    <xsl:with-param name="fix" select="."/>
                                    <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                                    <xsl:with-param name="profile" select="$profile"/>
                                </xsl:call-template>
                    
                            </xsl:for-each>
                        </div></td></tr>
                    </tbody>
                </table>
            </td>
        </tr>
    </xsl:if>
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
            <xsl:call-template name="guide-count-contained-selected-groups-impl">
                <xsl:with-param name="item" select="."/>
                <xsl:with-param name="profile" select="$profile"/>
            </xsl:call-template>
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
            <xsl:if test="$rule_selected_final = 'true'">R</xsl:if>
        </xsl:for-each>
    </xsl:if>
</xsl:template>

<xsl:template name="guide-count-contained-selected-groups-impl">
    <xsl:param name="item"/>
    <xsl:param name="profile"/>

    <xsl:variable name="selected_group_final">
        <xsl:call-template name="is-item-selected-final">
            <xsl:with-param name="item" select="$item"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:call-template>
    </xsl:variable>

    <xsl:if test="$selected_group_final = 'true'">
        <xsl:for-each select="$item/cdf:Group">
            <xsl:call-template name="guide-count-contained-selected-groups-impl">
                <xsl:with-param name="item" select="."/>
                <xsl:with-param name="profile" select="$profile"/>
            </xsl:call-template>
        </xsl:for-each>

        <xsl:for-each select="$item/cdf:Group">
            <xsl:variable name="group" select="."/>
            <xsl:variable name="group_selected_final">
                <xsl:call-template name="is-item-selected-final">
                    <xsl:with-param name="item" select="$group"/>
                    <xsl:with-param name="profile" select="$profile"/>
                </xsl:call-template>
            </xsl:variable>
            <xsl:if test="$group_selected_final = 'true'">G</xsl:if>
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

    <xsl:value-of select="string-length($impl-ret)"/>
</xsl:template>

<xsl:template name="guide-count-contained-selected-groups">
    <!-- XSLT is a functional language, you cannot "update"
         variables. That's why we recursivelly build a huge
         string containing 'Group' for each selected RULE.
         We then cound the number of Groups to get the number
         we need. -->

    <xsl:param name="item"/>
    <xsl:param name="profile"/>

    <xsl:variable name="impl-ret-group">
        <xsl:call-template name="guide-count-contained-selected-groups-impl">
            <xsl:with-param name="item" select="$item"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:call-template>
    </xsl:variable>

    <xsl:value-of select="string-length($impl-ret-group)"/>
</xsl:template>

<xsl:template name="contained-groups-wording">
    <xsl:param name="item"/>
    <xsl:param name="profile"/>

    <xsl:variable name="contained_groups">
        <xsl:call-template name="guide-count-contained-selected-groups">
            <xsl:with-param name="item" select="$item"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
        <xsl:when test="$contained_groups > 1">
            <small> <xsl:value-of select="$contained_groups"/> groups and </small>
        </xsl:when>
        <xsl:when test="$contained_groups = 1">
            <small><xsl:value-of select="$contained_groups"/> group and </small>
        </xsl:when>
        <xsl:otherwise>                
        </xsl:otherwise>
    </xsl:choose>
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

        <xsl:variable name="contained_groups">
            <xsl:call-template name="contained-groups-wording">
                <xsl:with-param name="item" select="$item"/>
                <xsl:with-param name="profile" select="$profile"/>
            </xsl:call-template>
        </xsl:variable>

        <tr data-tt-id="children-{$item/@id}">
            <xsl:if test="$item/parent::cdf:Group or $item/parent::cdf:Benchmark">
                <xsl:attribute name="data-tt-parent-id">
                    <xsl:value-of select="concat('children-', $item/parent::cdf:*/@id)"/>
                </xsl:attribute>
            </xsl:if>

            <td style="padding-left: {$indent * 19}px" colspan="2">
                <xsl:attribute name="id">
                    <xsl:value-of select="$item/@id"/>
                </xsl:attribute>
                <span class="label label-default">Group</span>&#160;&#160;
                <xsl:call-template name="item-title">
                    <xsl:with-param name="item" select="$item"/>
                    <xsl:with-param name="profile" select="$profile"/>
                </xsl:call-template>
                <xsl:choose>
                    <xsl:when test="$contained_rules > 1">
                        &#160;&#160;<small>Group contains <xsl:value-of select="$contained_groups"/><xsl:value-of select="$contained_rules"/> rules</small>
                    </xsl:when>
                    <xsl:when test="$contained_rules = 1">
                        &#160;&#160;<small>Group contains  <xsl:value-of select="$contained_groups"/><xsl:value-of select="$contained_rules"/> rule</small>
                    </xsl:when>
                    <xsl:otherwise>
                    </xsl:otherwise>
                </xsl:choose>
            </td>
        </tr>

        <xsl:if test="not($item/self::cdf:Benchmark)">
            <tr data-tt-id="{$item/@id}" class="guide-tree-inner-node guide-tree-inner-node-id-{$item/@id}">
                <xsl:if test="$item/parent::cdf:Group or $item/parent::cdf:Benchmark">
                    <xsl:attribute name="data-tt-parent-id">
                        <xsl:value-of select="concat('children-', $item/parent::cdf:*/@id)"/>
                    </xsl:attribute>
                </xsl:if>

                <td style="padding-left: {$indent * 19}px" colspan="2">
                    <p>
                        <a class="small" href="{concat('#', $item/@id)}">[ref]</a>&#160;&#160;
                        <xsl:apply-templates mode="sub-testresult" select="$item/cdf:description">
                            <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                            <xsl:with-param name="profile" select="$profile"/>
                        </xsl:apply-templates>
                    </p>

                    <xsl:for-each select="$item/cdf:warning">
                        <div class="panel panel-warning">
                            <div class="panel-heading">
                                <span class="label label-warning">Warning:</span>&#160;
                                <xsl:apply-templates mode="sub-testresult" select=".">
                                    <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                                    <xsl:with-param name="profile" select="$profile"/>
                                </xsl:apply-templates>
                            </div>
                        </div>
                    </xsl:for-each>

                    <xsl:if test="$item/cdf:reference">
                        <table class="table table-striped table-bordered">
                            <tbody>
                                <tr><td>Identifiers and References</td><td class="identifiers">
                                    <xsl:call-template name="item-idents-refs">
                                        <xsl:with-param name="item" select="$item"/>
                                    </xsl:call-template>
                                </td></tr>
                            </tbody>
                        </table>
                    </xsl:if>
                </td>
            </tr>
        </xsl:if>

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
    <xsl:param name="profile"/>
    <ol>
        <xsl:for-each select="$item/cdf:Group">
            <xsl:variable name="selected_final">
                <xsl:call-template name="is-item-selected-final">
                    <xsl:with-param name="item" select="."/>
                    <xsl:with-param name="profile" select="$profile"/>
                </xsl:call-template>
            </xsl:variable>
            <xsl:if test="$selected_final = 'true'">
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
                        <xsl:with-param name="profile" select="$profile"/>
                    </xsl:call-template>
                </xsl:if>
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
        <xsl:with-param name="profile" select="$profile"/>
    </xsl:call-template>
</xsl:template>

<xsl:template name="guide-tree">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <div id="guide-tree">
        <h2>Checklist</h2>
     
        <div class="row">
            <div class="col-sm-6">
                <div class="input-group">
                    <input type="text" class="form-control" placeholder="Search through the guide" id="search-input" oninput="ruleSearch()"/>

                    <div class="input-group-btn">
                         <button class="btn btn-default" onclick="ruleSearch()">Search</button>
                    </div>
                </div>
                <p id="search-matches"></p>
            </div>
        </div>

        <table class="treetable table table-bordered">
            <tbody>
                <xsl:call-template name="guide-tree-inner-node">
                    <xsl:with-param name="item" select="$benchmark"/>
                    <xsl:with-param name="profile" select="$profile"/>
                    <xsl:with-param name="indent" select="0"/>
                </xsl:call-template>
            </tbody>
        </table>

        <a href="#guide-tree"><button type="button" class="btn btn-secondary">Scroll back to the start of the checklist</button></a>
    </div>
</xsl:template>

<xsl:template name="generate-guide">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile_id"/>

    <xsl:call-template name="warn-unresolved">
        <xsl:with-param name="benchmark" select="$benchmark"/>
    </xsl:call-template>

    <xsl:variable name="profile" select="$benchmark/cdf:Profile[@id = $profile_id]"/>
    <xsl:if test="$profile_id != '' and not($profile)">
        <xsl:message terminate="yes">Profile "<xsl:value-of select="$profile_id"/>" was not found. Get available profiles using "oscap info".</xsl:message>
    </xsl:if>

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
        <xsl:call-template name="profileinfo">
            <xsl:with-param name="benchmark" select="$benchmark"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:call-template>
        <xsl:call-template name="revisionhistory">
            <xsl:with-param name="benchmark" select="$benchmark"/>
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
