<?xml version="1.0" encoding="utf-8" ?>

<!--
Copyright 2010 - 2014 Red Hat Inc., Durham, North Carolina.
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
    xmlns:cdf="http://checklists.nist.gov/xccdf/1.2">

<xsl:param name="verbosity"/>

<xsl:template name="rule-result-tooltip">
    <xsl:param name="ruleresult"/>
    <!-- The texts are sourced from XCCDF 1.2 specification with minor modifications -->
    <xsl:choose>
        <xsl:when test="$ruleresult = 'pass'">The target system or system component satisfied all the conditions of the rule.</xsl:when>
        <xsl:when test="$ruleresult = 'fixed'">The Rule had failed, but was then fixed (possibly by a tool that can automatically apply remediation, or possibly by the human auditor).</xsl:when>
        <xsl:when test="$ruleresult = 'informational'">The Rule was checked, but the output from the checking engine is simply information for auditors or administrators; it is not a compliance category. This status value is designed for Rule elements whose main purpose is to extract information from the target rather than test the target.</xsl:when>

        <xsl:when test="$ruleresult = 'fail'">The target system or system component did not satisfy at least one condition of the rule.</xsl:when>
        <xsl:when test="$ruleresult = 'error'">The checking engine could not complete the evaluation, therefore the status of the target's compliance with the rule is not certain. This could happen, for example, if a testing tool was run with insufficient privileges and could not gather all of the necessary information.</xsl:when>
        <xsl:when test="$ruleresult = 'unknown'">The testing tool encountered some problem and the result is unknown. For example, a result of 'unknown' might be given if the testing tool was unable to interpret the output of the checking engine (the output has no meaning to the testing tool).</xsl:when>

        <xsl:when test="$ruleresult = 'notchecked'">The Rule was not evaluated by the checking engine. This status is designed for Rule elements that have no check elements or that correspond to an unsupported checking system. It may also correspond to a status returned by a checking engine if the checking engine does not support the indicated check code.</xsl:when>
        <xsl:when test="$ruleresult = 'notselected'">The Rule was not selected in the evaluation. This may be caused by the rule not being selected by default in the benchmark or by the profile unselecting it.</xsl:when>
        <xsl:when test="$ruleresult = 'notapplicable'">The Rule was not applicable to the target of the test. For example, the Rule might have been specific to a different version of the target OS, or it might have been a test against a platform feature that was not installed.</xsl:when>
    </xsl:choose>
</xsl:template>

<xsl:template match="cdf:ident" mode="ident">
    <xsl:choose>
        <xsl:when test="starts-with(@system, 'http://cve.mitre.org')">
            <a href="{concat('https://cve.mitre.org/cgi-bin/cvename.cgi?name=', text())}"><abbr title="{concat(@system, concat(': ', text()))}"><xsl:value-of select="text()"/></abbr></a>
        </xsl:when>
        <xsl:otherwise>
            <abbr title="{concat(@system, concat(': ', text()))}"><xsl:value-of select="text()"/></abbr>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="cdf:reference" mode="reference">
    <xsl:choose>
        <xsl:when test="@href">
            <a href="{@href}">
                <xsl:choose>
                    <xsl:when test="text()">
                        <xsl:value-of select="text()"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="@href"/>
                    </xsl:otherwise>
                </xsl:choose>
            </a>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="text()"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="item-title">
    <xsl:param name="item"/>
    <xsl:param name="profile"/>

    <xsl:choose>
        <xsl:when test="$item/cdf:title">
            <xsl:apply-templates mode="sub-testresult" select="$item/cdf:title">
                <xsl:with-param name="benchmark" select="$item/ancestor::cdf:Benchmark"/>
                <xsl:with-param name="profile" select="$profile"/>
            </xsl:apply-templates>
        </xsl:when>
        <xsl:otherwise>
            ID: <xsl:value-of select="$item/@id"/>
        </xsl:otherwise>
    </xsl:choose>

</xsl:template>

<xsl:template name="item-idents-refs">
    <xsl:param name="item"/>

    <xsl:if test="$item/cdf:ident">
        <p>
            <span class="label label-info" title="A globally meaningful identifiers for this rule. MAY be the name or identifier of a security configuration issue or vulnerability that the rule remediates. By setting an identifier on a rule, the benchmark author effectively declares that the rule instantiates, implements, or remediates the issue for which the name was assigned.">identifiers:</span>&#160;
            <xsl:for-each select="$item/cdf:ident">
                <xsl:apply-templates mode="ident" select="."/>
                <xsl:if test="position() != last()">, </xsl:if>
            </xsl:for-each>
        </p>
    </xsl:if>
    <xsl:if test="$item/cdf:reference">
        <p>
            <span class="label label-default" title="Provide a reference to a document or resource where the user can learn more about the subject of the Rule or Group.">references:</span>&#160;
            <xsl:for-each select="$item/cdf:reference">
                <xsl:apply-templates mode="reference" select="."/>
                <xsl:if test="position() != last()">, </xsl:if>
            </xsl:for-each>
        </p>
    </xsl:if>
</xsl:template>

<!-- substitution for testresults, used in HTML report -->
<xsl:template mode="sub-testresult" match="text()">
    <xsl:param name="testresult"/>
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <xsl:value-of select="."/>
</xsl:template>

<xsl:template mode="sub-testresult" match="cdf:fix//cdf:instance">
    <xsl:param name="testresult"/>
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <xsl:variable name='ctx' select='@context'/>

    <xsl:choose>
        <xsl:when test="$testresult/cdf:rule-result/cdf:instance[@context=$ctx]">
            <abbr title="context: {$ctx}"><xsl:value-of select="$testresult/cdf:rule-result/cdf:instance[@context=$ctx]"/></abbr>
        </xsl:when>
        <xsl:otherwise>
            <abbr class="cdf-sub-context" title="replace with actual {$ctx} context"><xsl:value-of select="$ctx"/></abbr>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template mode="sub-testresult" match="cdf:sub">
    <xsl:param name="testresult"/>
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <xsl:variable name="subid" select="./@idref"/>

    <xsl:choose>
        <xsl:when test="$testresult and $testresult/cdf:set-value[@idref = $subid]">
            <abbr title="from TestResult: {$subid}"><xsl:value-of select="$testresult/cdf:set-value[@idref = $subid][last()]"/></abbr>
        </xsl:when>
        <xsl:when test="$profile and $profile/cdf:refine-value[@idref = $subid]">
            <xsl:variable name="selector" select="$profile/cdf:refine-value[@idref = $subid][last()]/@selector"/>
            <abbr title="from Profile/refine-value: {$subid}"><xsl:value-of select="$benchmark//cdf:Value/cdf:value[@selector = $selector][last()]/text()"/></abbr>
        </xsl:when>
        <xsl:when test="$profile and $profile/cdf:set-value[@idref = $subid]">
            <abbr title="from Profile/set-value: {$subid}"><xsl:value-of select="$profile/cdf:set-value[@idref = $subid][last()]/text()"/></abbr>
        </xsl:when>
        <xsl:when test="$benchmark//cdf:Value[@id = $subid]/cdf:value[not(@selector)]">
            <abbr title="from Benchmark/Value: {$subid}"><xsl:value-of select="$benchmark//cdf:Value[@id = $subid]/cdf:value[not(@selector)][last()]"/></abbr>
        </xsl:when>
        <xsl:otherwise>
            <abbr title="Substitution failed: {$subid}">(N/A)</abbr>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template mode="sub-testresult" match="*[namespace-uri()='http://www.w3.org/1999/xhtml']">
    <xsl:param name="testresult"/>
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <xsl:choose>
        <xsl:when test="local-name() = 'br'">
            <!-- <br></br> shows up as 2 <br> elements in HTML5, this horrible hack prevents that -->
            <xsl:text disable-output-escaping="yes">&#60;br&#62;</xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:element name="{local-name()}">
                <xsl:copy-of select="@*"/>
                <xsl:apply-templates select="./text() | ./*" mode="sub-testresult">
                    <xsl:with-param name="testresult" select="$testresult"/>
                    <xsl:with-param name="benchmark" select="$benchmark"/>
                    <xsl:with-param name="profile" select="$profile"/>
                </xsl:apply-templates>
            </xsl:element>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template mode="sub-testresult" match="node() | @*">
    <xsl:param name="testresult"/>
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <xsl:copy>
        <xsl:copy-of select="@*"/>

        <xsl:apply-templates select="node()" mode="sub-testresult">
            <xsl:with-param name="testresult" select="$testresult"/>
            <xsl:with-param name="benchmark" select="$benchmark"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:apply-templates>
    </xsl:copy>
</xsl:template>

<xsl:template mode="sub-testresult" match="cdf:title | cdf:description | cdf:fix | cdf:fixtext | cdf:front-matter | cdf:rear-matter | cdf:rationale | cdf:warning | cdf:notice">
    <xsl:param name="testresult"/>
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <xsl:apply-templates select="node()" mode="sub-testresult">
        <xsl:with-param name="testresult" select="$testresult"/>
        <xsl:with-param name="benchmark" select="$benchmark"/>
        <xsl:with-param name="profile" select="$profile"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template name="show-fixtext">
    <xsl:param name="fixtext"/>
    <xsl:param name="testresult"/>
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <xsl:apply-templates mode="sub-testresult" select="$fixtext">
        <xsl:with-param name="testresult" select="$testresult"/>
        <xsl:with-param name="benchmark" select="$benchmark"/>
        <xsl:with-param name="profile" select="$profile"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template name="show-fix">
    <xsl:param name="fix"/>
    <xsl:param name="testresult"/>
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <pre><code>
        <xsl:apply-templates mode="sub-testresult" select="$fix">
            <xsl:with-param name="testresult" select="$testresult"/>
            <xsl:with-param name="benchmark" select="$benchmark"/>
            <xsl:with-param name="profile" select="$profile"/>
        </xsl:apply-templates>
    </code></pre>
</xsl:template>

<xsl:template name="show-title-front-matter-description-notices">
    <!-- TODO: please invent a better name for this template -->

    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

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
    <xsl:if test="$profile">
        <blockquote>with profile <mark>
            <xsl:choose>
                <xsl:when test="$profile/cdf:title">
                    <xsl:apply-templates mode="sub-testresult" select="$profile/cdf:title[1]">
                        <xsl:with-param name="benchmark" select="$benchmark"/>
                        <xsl:with-param name="profile" select="$profile"/>
                    </xsl:apply-templates>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$profile/@id"/>
                </xsl:otherwise>
            </xsl:choose>
        </mark></blockquote>
    </xsl:if>

    <div class="col-md-12 well well-lg horizontal-scroll">
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
    </div>
</xsl:template>

<xsl:template name="rear-matter">
    <xsl:param name="benchmark"/>
    <xsl:param name="profile"/>

    <div id="rear-matter">
        <div class="row top-spacer-10">
            <div class="col-md-12 well well-lg">
                <xsl:if test="$benchmark/cdf:rear-matter">
                    <div class="rear-matter">
                        <xsl:apply-templates mode="sub-testresult" select="$benchmark/cdf:rear-matter[1]">
                            <xsl:with-param name="benchmark" select="$benchmark"/>
                            <xsl:with-param name="profile" select="$profile"/>
                        </xsl:apply-templates>
                    </div>
                </xsl:if>
            </div>
        </div>
    </div>
</xsl:template>

<xsl:template name="warn-unresolved">
    <xsl:param name="benchmark"/>

    <xsl:if test="$benchmark[not(@resolved=1)][not(@resolved='true')]">
        <xsl:message>WARNING: Processing an unresolved XCCDF document. This may have unexpected results.</xsl:message>
    </xsl:if>
</xsl:template>

</xsl:stylesheet>
