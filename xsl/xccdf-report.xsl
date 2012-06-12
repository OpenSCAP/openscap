<?xml version="1.0" encoding="UTF-8" ?>
<!--
Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
     Lukas Kuklinek <lkuklinek@redhat.com>
     Martin Preisler <mpreisle@redhat.com>
-->


<xsl:stylesheet version="1.1"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
    xmlns:exsl="http://exslt.org/common"
	xmlns:db="http://docbook.org/ns/docbook"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    xmlns:svg="http://www.w3.org/2000/svg"
	xmlns="http://docbook.org/ns/docbook"
    xmlns:s="http://open-scap.org/"
    exclude-result-prefixes="xsl cdf db s exsl"
    xmlns:ovalres="http://oval.mitre.org/XMLSchema/oval-results-5"
    xmlns:sceres="http://open-scap.org/page/SCE_result_file"
    >

<!--<xsl:include href="xccdf-common.xsl" />-->
<xsl:import href="security-guide.xsl" />
<xsl:import href="oval-report.xsl" />
<xsl:import href="sce-report.xsl" />

<xsl:output method="xml" encoding="UTF-8" indent="yes"/>

<!-- parameters -->
<xsl:param name="result-id"/>
<xsl:param name="with-target-facts"/>
<xsl:param name="show"/>

<!-- OVAL and SCE result parameters -->
<xsl:param name='pwd'/>
<xsl:param name='oval-template'/>
<xsl:param name='sce-template'/>

<xsl:variable name='oval-tmpl'>
  <xsl:choose>
    <xsl:when test='not($oval-template)' />
    <xsl:when test='substring($oval-template, 1, 1) = "/"'><xsl:value-of select='$oval-template'/></xsl:when>
    <xsl:otherwise><xsl:value-of select='concat($pwd, "/", $oval-template)'/></xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name='sce-tmpl'>
  <xsl:choose>
    <xsl:when test='not($sce-template)' />
    <xsl:when test='substring($sce-template, 1, 1) = "/"'><xsl:value-of select='$sce-template'/></xsl:when>
    <xsl:otherwise><xsl:value-of select='concat($pwd, "/", $sce-template)'/></xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name='toshow'>
  <xsl:choose>
    <xsl:when test='substring($show, 1, 1) = "="'>,<xsl:value-of select='substring($show, 2)'/>,</xsl:when>
    <xsl:otherwise>,pass,fixed,notchecked,informational,unknown,error,fail,<xsl:value-of select='$show'/>,</xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<!-- keys -->
<xsl:key name="items" match="cdf:Group|cdf:Rule|cdf:Value" use="@id"/>
<xsl:key name="profiles" match="cdf:Profile" use="@id"/>

<!-- top-level template -->
<xsl:template match='cdf:Benchmark'>
  <xsl:variable name='end-times'>
    <s:times>
    <xsl:for-each select='cdf:TestResult/@end-time'>
      <xsl:sort order='descending'/>
      <s:t t='{.}'/>
    </xsl:for-each>
    </s:times>
  </xsl:variable>

  <xsl:variable name='last-test-time' select='exsl:node-set($end-times)/s:times/s:t[1]/@t'/>

  <xsl:variable name='final-result-id'>
    <xsl:choose>
      <xsl:when test="$result-id">
        <xsl:value-of select='$result-id'/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select='cdf:TestResult[@end-time=$last-test-time][last()]/@id'/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name='result' select='cdf:TestResult[@id=$final-result-id]'/>

  <xsl:variable name='profile' select='cdf:TestResult[@id=$result-id][1]/cdf:profile/@idref'/>

  <xsl:choose>
    <xsl:when test='count(cdf:TestResult) = 0'>
      <xsl:message terminate='yes'>This benchmark does not contain any test results.</xsl:message>
    </xsl:when>
    <xsl:when test='$result'>
      <xsl:if test='$verbosity'>
        <xsl:message>TestResult ID: <xsl:value-of select='$final-result-id'/></xsl:message>
        <xsl:message>Profile: <xsl:choose><xsl:when test='$profile'><xsl:value-of select="$profile"/></xsl:when><xsl:otherwise>(Default)</xsl:otherwise></xsl:choose></xsl:message>
      </xsl:if>
      <xsl:apply-templates select='$result'/>
    </xsl:when>
    <xsl:when test='$result-id'>
      <xsl:message terminate='yes'>No such result exists.</xsl:message>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message terminate='yes'>No result ID specified.</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match='cdf:TestResult'>
  <book xmlns='http://docbook.org/ns/docbook' version='5.0'
        xmlns:xlink="http://www.w3.org/1999/xlink"
        id='{@id}'
    >
    <info>
        <title>XCCDF test result "<xsl:value-of select="@id"/>"</title>
        <xsl:call-template name='footerinfo'/>
    </info>

    <xsl:call-template name='summary'/>
    <xsl:call-template name='target-info'/>
    <xsl:call-template name='benchmark-info'/>
    <xsl:call-template name='score'/>
    <xsl:call-template name='rr'/>
  </book>
</xsl:template>

<xsl:template name='summary'>
  <table id="test-result-summary">
    <thead>
      <row>
        <entry>Result ID</entry>
        <entry>Profile</entry>
        <entry>Start time</entry>
        <entry>End time</entry>
        <entry>Benchmark version</entry>
      </row>
    </thead>
    <tbody>
      <row>
        <entry align="center"><xsl:value-of select="@id"/></entry>
        <entry align="center">
          <xsl:choose>
            <xsl:when test="cdf:profile">
              <xsl:value-of select="cdf:profile/text()"/>
            </xsl:when>
            <xsl:otherwise>
              (Default profile)
            </xsl:otherwise>
          </xsl:choose>
        </entry>
        <entry align="center"><date><xsl:value-of select="@start-time"/></date></entry>
        <entry align="center"><date><xsl:value-of select="@end-time"/></date></entry>
        <entry align="center">
          <xsl:choose>
            <xsl:when test="/cdf:Benchmark/cdf:version">
              <xsl:value-of select="/cdf:Benchmark/cdf:version/text()"/>
            </xsl:when>
            <xsl:otherwise>
              Unknown
            </xsl:otherwise>
          </xsl:choose>
        </entry>
      </row>
    </tbody>
  </table>
    
  <xsl:apply-templates select='cdf:identity'/>
</xsl:template>

<xsl:template name='target-info'>
  <chapter id='target-info'>
    <title>Target Information</title>

    <xsl:call-template name='list'>
      <xsl:with-param name='nodes' select='cdf:target' />
      <xsl:with-param name='title' select='"Target"' />
    </xsl:call-template>

    <xsl:call-template name='list'>
      <xsl:with-param name='nodes' select='cdf:target-address' />
      <xsl:with-param name='title' select='"Addresses"' />
    </xsl:call-template>

    <xsl:if test="$with-target-facts">
      <xsl:apply-templates select='cdf:target-facts' mode='result' />
    </xsl:if>
  </chapter>
</xsl:template>

<xsl:template name='benchmark-info'>
  <chapter id='benchmark-info'>
    <title>Benchmark Execution Information</title>

    <xsl:call-template name='list'>
      <xsl:with-param name='nodes' select='cdf:remark' />
      <xsl:with-param name='title' select='"Remarks"' />
    </xsl:call-template>

    <xsl:if test='/cdf:Benchmark/cdf:platform or cdf:platform'>
      <itemizedlist>
        <title>Platforms</title>
        <xsl:apply-templates select='/cdf:Benchmark/cdf:platform'/>
        <xsl:apply-templates select='cdf:platform'/>
      </itemizedlist>
    </xsl:if>

    <xsl:if test='cdf:set-value'>
      <table>
        <title>Values</title>
        <tgroup>
          <thead><row><entry>Name</entry><entry>Value</entry></row></thead>
          <tbody><xsl:apply-templates select='cdf:set-value'/></tbody>
        </tgroup>
      </table>
    </xsl:if>

    <xsl:call-template name='list'>
      <xsl:with-param name='nodes' select='cdf:organization' />
      <xsl:with-param name='title' select='"Organization"' />
    </xsl:call-template>
  </chapter>
</xsl:template>

<xsl:template name='score'>
  <chapter id='score'>
    <title>Score</title>
    <xsl:choose>
      <xsl:when test='cdf:score'><xsl:call-template name='score.table'/></xsl:when>
      <xsl:otherwise><para role='unknown'>No score results.</para></xsl:otherwise>
    </xsl:choose>
  </chapter>
</xsl:template>

<xsl:template name='score.table'>
  <table>
    <title role='hidden'>Security Score</title>
    <tgroup>
      <thead><row><entry>system</entry><entry>score</entry><entry>max</entry><entry>bar</entry></row></thead>
      <tbody><xsl:apply-templates select='cdf:score'/></tbody>
    </tgroup>
  </table>
</xsl:template>

<xsl:template name='rr'>
  <xsl:variable name='results' select='cdf:rule-result[contains($toshow, concat(",",cdf:result,",")) and not(contains($toshow, concat(",-",cdf:result,",")))]'/>

  <chapter id='results'>
    <title>Results</title>
    <xsl:choose>
      <xsl:when test='$results'>
        <table role='raw'>
          <title>Rule Results Summary</title>
          <tgroup>
            <tbody>
              <row><entry>pass</entry><entry><xsl:value-of select='count(cdf:rule-result[cdf:result="pass"])'/></entry></row>
              <row><entry>fixed</entry><entry><xsl:value-of select='count(cdf:rule-result[cdf:result="fixed"])'/></entry></row>
              <row><entry>fail</entry><entry><xsl:value-of select='count(cdf:rule-result[cdf:result="fail"])'/></entry></row>
              <row><entry>error</entry><entry><xsl:value-of select='count(cdf:rule-result[cdf:result="error"])'/></entry></row>
              <row><entry>not selected</entry><entry><xsl:value-of select='count(cdf:rule-result[cdf:result="notselected"])'/></entry></row>
              <row><entry>not checked</entry><entry><xsl:value-of select='count(cdf:rule-result[cdf:result="notchecked"])'/></entry></row>
              <row><entry>not applicable</entry><entry><xsl:value-of select='count(cdf:rule-result[cdf:result="notapplicable"])'/></entry></row>
              <row><entry>informational</entry><entry><xsl:value-of select='count(cdf:rule-result[cdf:result="informational"])'/></entry></row>
              <row><entry>unknown</entry><entry><xsl:value-of select='count(cdf:rule-result[cdf:result="unknown"])'/></entry></row>
              <row><entry><emphasis role='strong'>total</emphasis></entry><entry><xsl:value-of select='count(cdf:rule-result)'/></entry></row>
            </tbody>
          </tgroup>
        </table>

        <xsl:apply-templates select='.' mode='rr.table'><xsl:with-param name='results' select='$results'/></xsl:apply-templates>
      </xsl:when>
      <xsl:otherwise><para role='unknown'>No rule results.</para></xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates select='$results'/>
  </chapter>
</xsl:template>

<xsl:template match='cdf:rule-result'>
  <xsl:variable name='rule' select="key('items',@idref)"/>

  <section id='ruleresult-{generate-id(.)}' role='result-detail'>
    <title>Result for <xsl:value-of select='($rule/cdf:title[1]|@idref)[1]'/></title>
    <para role="result-{normalize-space(cdf:result)}">Result: <emphasis role='strong'><xsl:value-of select="cdf:result"/></emphasis></para>
    <para>Rule ID: <emphasis role='strong'><xsl:value-of select="@idref"/></emphasis></para>

    <xsl:apply-templates select='@time'/>
    <xsl:apply-templates select='@severity'/>
    <xsl:call-template name='rr.instance'/>
    <xsl:apply-templates select='$rule/cdf:description[1]'/>
    <xsl:apply-templates select='$rule/cdf:warning[1]'/>
    <xsl:apply-templates select='$rule/cdf:rationale[1]'/>
    <xsl:call-template name='idents'/>
    <!-- overrides (n) -->
    <!-- messages (n) -->
    <xsl:apply-templates select='$rule/cdf:fixtext[1]'/>
    <xsl:apply-templates select='($rule/cdf:fix|cdf:fix)[last()]'/>
    <xsl:apply-templates select='.' mode='engine-results'/>
    <xsl:call-template name='references'/>
  </section>
</xsl:template>

<xsl:template mode='rr.table' match='cdf:TestResult'>
  <xsl:param name='results'/>
  <table>
    <title role='hidden'>Rule results summary</title>
    <tgroup>
      <thead><row><entry>Title</entry><entry>Result</entry><entry>more</entry></row></thead>
      <tbody><xsl:apply-templates select='$results' mode='rr.table'/></tbody>
    </tgroup>
  </table>
</xsl:template>

<xsl:template mode='rr.table' match='cdf:rule-result'>
  <row role='result-{normalize-space(cdf:result)}'>
    <entry role='id'    ><xsl:value-of select='(key("items",@idref)/cdf:title[1]|@idref)[1]'/></entry>
    <entry role='result'><emphasis role='strong'><xsl:value-of select='normalize-space(cdf:result)'/></emphasis></entry>
    <entry role='link'  ><phrase xlink:href='#ruleresult-{generate-id(.)}'>view</phrase></entry>
  </row>
</xsl:template>

<xsl:template name='rr.instance'>
  <xsl:if test='cdf:instance'>
    <itemizedlist><title>Instance</title><xsl:apply-templates select='cdf:instance'/></itemizedlist>
  </xsl:if>
</xsl:template>

<xsl:template match='cdf:instance'>
  <listitem><simpara><xsl:value-of select='.'/>
         <xsl:if test='@context'> [context: <xsl:value-of select='@context'/>]</xsl:if>
         <xsl:if test='@parentContext'> [parent context: <xsl:value-of select='@parentContext'/>]</xsl:if>
  </simpara></listitem>
</xsl:template>

<xsl:template match='cdf:benchmark'>
    <para>Used XCDF benchmark URI: <emphasis role='strong'><xsl:value-of select='@href'/></emphasis></para>
</xsl:template>

<xsl:template match='cdf:test-result/cdf:status'>
  <para>
    <xsl:text>Status: </xsl:text><emphasis role='strong'><xsl:value-of select='normalize-space(.)'/></emphasis>
    <xsl:if test='@date'>(as of <phrase role='date'><xsl:value-of select='@date'/></phrase>)</xsl:if>
  </para>
</xsl:template>

<xsl:template match='@severity'>
    <para>Severity: <emphasis role='strong'><xsl:value-of select='.'/></emphasis></para>
</xsl:template>

<xsl:template match='@time'>
    <para>Time: <emphasis role='strong'><phrase role='date'><xsl:apply-templates mode='date' select='.'/></phrase></emphasis></para>
</xsl:template>

<xsl:template match='cdf:profile'>
    <para>Profile: <emphasis role='strong'><xsl:value-of select='@idref'/></emphasis></para>
</xsl:template>

<xsl:template match='cdf:target-facts'>
  <table>
    <title>Target facts</title>
    <tgroup>
      <thead><row><entry>Fact</entry><entry>Value</entry></row></thead>
      <tbody>
        <xsl:apply-templates select='cdf:fact'/>
      </tbody>
    </tgroup>
  </table>
</xsl:template>

<xsl:template match='cdf:fact'>
  <row><entry><xsl:value-of select='@name'/></entry><entry><xsl:value-of select='.'/></entry></row>
</xsl:template>

<xsl:template match='cdf:set-value'>
  <row><entry><xsl:value-of select='(key("items",@idref)/cdf:title[1]|@idref)[1]'/></entry><entry><xsl:value-of select='.'/></entry></row>
</xsl:template>

<xsl:template match='cdf:score'>
  <xsl:variable name='max'>
    <xsl:choose>
      <xsl:when test='@maximum'><xsl:value-of select='@maximum'/></xsl:when>
      <xsl:otherwise>100</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name='percent' select='number(.) div number($max)'/>
  <xsl:variable name='format' select="'#.00'"/>

  <row id='score-{translate(@system, ":", "-")}'>
    <entry role='score-sys'><xsl:value-of select='@system' /></entry>
    <entry role='score-val'><xsl:value-of select='format-number(string(.), $format)' /></entry>
    <entry role='score-max'><xsl:value-of select='format-number($max, $format)' /></entry>
    <entry role='score-bar'>
      <inlinemediaobject role='score-bar'>
        <imageobject>
          <imagedata format='SVG'>
            <svg:svg width="100%" height="100%" version="1.1" baseProfile="full">
              <svg:rect width="100%" height="100%" fill="red"/>
              <svg:rect height="100%" width="{format-number($percent, '#.00%')}" fill="green"/>
            </svg:svg>
          </imagedata>
        </imageobject>
        <textobject><phrase><xsl:value-of select='format-number($percent, "#.0%")'/></phrase></textobject>
      </inlinemediaobject>
    </entry>
  </row>
</xsl:template>

<!-- TOC adjustment (switch off) -->
<xsl:template mode='dbout.html.toc' match='db:chapter'/>


<!-- checking engine results related templates -->
<xsl:template match='cdf:rule-result' mode='engine-results'>
  <xsl:if test='contains(",fail,error,unknown,informational,", concat(",", normalize-space(cdf:result), ","))'>
    <xsl:apply-templates mode='engine-results' select='key("items", @idref)'/>
  </xsl:if>
</xsl:template>

<xsl:template match='cdf:Rule' mode='engine-results'>
    <xsl:apply-templates mode='engine-results' select='cdf:check'/>
</xsl:template>

<xsl:template match='cdf:check[starts-with(@system, "http://oval.mitre.org/XMLSchema/oval")]' mode='engine-results'>
  <xsl:apply-templates mode='oval-engine-results' select='cdf:check-content-ref[1]'/>
</xsl:template>

<xsl:template match='cdf:check-content-ref' mode='oval-engine-results'>
  <xsl:variable name='filename'>
    <xsl:choose>
      <xsl:when test='contains($oval-tmpl, "%")'><xsl:value-of select='concat(substring-before($oval-tmpl, "%"), @href, substring-after($oval-tmpl, "%"))'/></xsl:when>
      <xsl:otherwise><xsl:value-of select='$oval-tmpl'/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:if test='$oval-tmpl'>
    <xsl:apply-templates select='document($filename)/ovalres:oval_results' mode='brief'>
      <xsl:with-param name='definition-id' select='@name'/>
    </xsl:apply-templates>
  </xsl:if>
</xsl:template>

<xsl:template match='cdf:check[starts-with(@system, "http://open-scap.org/page/SCE")]' mode='engine-results'>
   <xsl:apply-templates mode='sce-engine-results' select='cdf:check-content-ref[1]'/>
</xsl:template>

<xsl:template match='cdf:check-content-ref' mode='sce-engine-results'>
  <xsl:variable name='rule-id' select='../../@id'/>
  <xsl:variable name='stdout-check-imports' select='//cdf:rule-result[@idref=$rule-id]/descendant::cdf:check-import[@import-name="stdout"]'/>
  
  <xsl:apply-templates select='$stdout-check-imports' mode='brief' />
  
<xsl:if test='not($stdout-check-imports)'>
  <!-- fallback that looks for SCE result files -->  
  <xsl:variable name='filename'>
    <xsl:choose>
      <xsl:when test='contains($sce-tmpl, "%")'><xsl:value-of select='concat(substring-before($sce-tmpl, "%"), @href, substring-after($sce-tmpl, "%"))'/></xsl:when>
      <xsl:otherwise><xsl:value-of select='$sce-tmpl'/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:if test='$sce-tmpl'>
    <xsl:apply-templates select='document($filename)/sceres:sce_results' mode='brief' />
  </xsl:if>
</xsl:if>
</xsl:template>

<xsl:template match='node()' mode='engine-results'/>

</xsl:stylesheet>
