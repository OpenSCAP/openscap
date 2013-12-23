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
-->

<!-- TODO
  - do not display hidden and abstract items
  - profile info?
-->

<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:htm="http://www.w3.org/1999/xhtml"
	xmlns:db="http://docbook.org/ns/docbook"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xlink="http://www.w3.org/1999/xlink"
	xmlns="http://docbook.org/ns/docbook"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
    exclude-result-prefixes="xsl cdf db htm exsl"
	>

<xsl:import href='xccdf-share.xsl'/>
<xsl:import href='docbook-share.xsl'/>

<xsl:param name='clean-profile-notes' select='1'/>
<xsl:param name='hide-profile-info'/>
<xsl:param name='hide-rules' select='false'/>

<xsl:output method="xml" encoding="UTF-8" indent="yes"/>

<xsl:template match='cdf:Benchmark'>
  <book xmlns='http://docbook.org/ns/docbook' version='5.0'
        xmlns:xlink="http://www.w3.org/1999/xlink"
        id='{@id}'
    >
    <xsl:call-template name='info'>
      <xsl:with-param name='make-footer' select='1'/>
      <xsl:with-param name='more'>
        <subtitle><phrase xlink:href='http://scap.nist.gov/specifications/xccdf/'>XCCDF</phrase> Security Guide</subtitle>
        <titleabbrev>Security guide</titleabbrev>
      </xsl:with-param>
    </xsl:call-template>
    <preface id='preface'>
      <table role="raw" id="guide-top-table">
        <tbody>
          <row>
            <entry id="common-info" valign="top"><xsl:call-template name='common-info'/></entry>
            <entry id="versions-revisions" valign="top">
              <xsl:apply-templates select='cdf:version'/>
              <xsl:call-template name='revisions'/>
            </entry>
          </row>
        </tbody>
      </table>
    </preface>
    <xsl:apply-templates select='cdf:Rule'/>
    <xsl:apply-templates select='cdf:Group'/>
    <xsl:if test='not($hide-profile-info)'><xsl:call-template name='profile-info'/></xsl:if>
    <xsl:call-template name='references'><xsl:with-param name='elname' select='"bibliography"'/></xsl:call-template>
    <xsl:apply-templates select='cdf:rear-matter'/>
  </book>
</xsl:template>

<xsl:template match='cdf:Group|cdf:Rule'>
  <xsl:if test='self::cdf:Group or not($hide-rules)'>
    <xsl:variable name='elname'>
      <xsl:choose><xsl:when test='count(ancestor::cdf:Group)=0'>chapter</xsl:when><xsl:otherwise>section</xsl:otherwise></xsl:choose>
    </xsl:variable>
    <xsl:element name="{string($elname)}">
      <xsl:attribute name='id'>item-<xsl:value-of select='@id'/></xsl:attribute>
      <xsl:attribute name='role'><xsl:value-of select='concat("xccdf-", translate(local-name(), "GR", "gr"))'/></xsl:attribute>
      <xsl:call-template name='info'/>
      <xsl:call-template name='common-info'/>
      <xsl:call-template name='references'/>
      <xsl:apply-templates select='cdf:Rule'/>
      <xsl:apply-templates select='cdf:Group'/>
    </xsl:element>
  </xsl:if>
</xsl:template>

<xsl:template name='common-info'>
  <xsl:apply-templates select='cdf:title'/>
  <xsl:apply-templates select='cdf:front-matter'/>
  <xsl:apply-templates select='cdf:description'/>
  <xsl:call-template name='platforms'/>
  <xsl:apply-templates select='cdf:warning|cdf:rationale|cdf:profile-note'/>
  <!-- TODO better fix + fixtext structure -->
  <xsl:apply-templates select='cdf:fixtext'/>
  <xsl:apply-templates select='cdf:fix'/>
  <xsl:call-template name='idents'/>
</xsl:template>

<xsl:template name='references'>
  <xsl:param name='elname' select='"bibliolist"'/>
  <xsl:if test='cdf:reference'>
    <xsl:element name='{$elname}'>
      <xsl:attribute name='id'>references-<xsl:value-of select='@id'/></xsl:attribute>
      <title>References</title>
      <xsl:apply-templates select='cdf:reference'/>
    </xsl:element>
  </xsl:if>
</xsl:template>


<xsl:template name='revisions'>
  <xsl:if test='cdf:status'>
    <revhistory role='{translate(local-name(), "BRG", "brg")}-statuses'>
      <xsl:apply-templates select='cdf:status'/>
    </revhistory>
  </xsl:if>
</xsl:template>

<xsl:template name='platforms'>
  <xsl:if test='cdf:platform'>
    <itemizedlist>
      <title>Applicable platforms</title>
      <xsl:apply-templates select='cdf:platform'/>
    </itemizedlist>
  </xsl:if>
</xsl:template>

<xsl:template name='idents'>
  <xsl:if test='cdf:ident'>
    <itemizedlist>
      <title>Security identifiers</title>
      <xsl:apply-templates select='cdf:ident'/>
    </itemizedlist>
  </xsl:if>
</xsl:template>

<xsl:template match='cdf:status'>
  <revision>
    <xsl:if test='@date'><date><xsl:value-of select='@date'/></date></xsl:if>
    <revremark><xsl:value-of select='normalize-space(.)'/></revremark>
  </revision>
</xsl:template>

<xsl:template match='cdf:version'>
  <releaseinfo role='version'>
    <emphasis role='strong'>Version: </emphasis> <xsl:value-of select='normalize-space(.)'/>
    <xsl:if test='@time'> (as of <phrase role='date'><xsl:value-of select='@time'/></phrase>)</xsl:if>
    <xsl:if test='@update'> [<phrase xlink:href='{@update}'>updates</phrase>]</xsl:if>
  </releaseinfo>
</xsl:template>

<xsl:template match='cdf:notice'>
  <legalnotice id='notice-{@id}'>
    <title>Legal notice</title>
    <xsl:apply-templates select='.' mode='db'/>
  </legalnotice>
</xsl:template>

<xsl:template match='cdf:rationale'>
  <note role='rationale'>
    <title>Rationale</title>
    <xsl:apply-templates select='.' mode='db'/>
  </note>
</xsl:template>

<xsl:template match='cdf:profile-note'>
  <note role='profile-note'>
    <title>Profile note</title>
    <xsl:apply-templates select='.' mode='db'/>
  </note>
</xsl:template>

<xsl:template match='cdf:fixtext'>
  <figure role='xccdf-fixtext'>
    <title><xsl:value-of select='normalize-space(../cdf:title[1])'/> – remediation instructions</title>
    <titleabbrev role='short'>Remediation instructions</titleabbrev>
    <xsl:apply-templates select='.' mode='db'/>
    <!-- TODO process fixtext attributes -->
  </figure>
</xsl:template>

<xsl:template match='cdf:fix'>
  <figure role='xccdf-fix'>
    <title><xsl:value-of select='normalize-space(../cdf:title[1])'/> – remediation script</title>
    <titleabbrev role='short'>Remediation script</titleabbrev>
    <programlisting>
      <xsl:apply-templates select='.' mode='h2db'/>
      <!-- TODO process fixtext attributes -->
    </programlisting>
  </figure>
</xsl:template>

<xsl:template match='cdf:title'><title><xsl:value-of select='normalize-space(.)'/></title></xsl:template>
<xsl:template match='cdf:description|cdf:rationale|cdf:front-matter'><xsl:apply-templates select='.' mode='db'/></xsl:template>
<xsl:template match='cdf:warning'><warning role='xccdf-warning'><xsl:apply-templates select='.' mode='db'/></warning></xsl:template>
<xsl:template match='cdf:metadata'><info><xsl:apply-templates select='.' mode='db'/></info></xsl:template>
<xsl:template match='cdf:reference'><biblioentry><xsl:apply-templates select='.' mode='db'/></biblioentry></xsl:template>
<xsl:template match='cdf:platform'><listitem><simpara><xsl:value-of select='normalize-space(@idref)'/></simpara></listitem></xsl:template>
<xsl:template match='cdf:ident'>
  <listitem>
    <xsl:variable name="ident"><xsl:value-of select='normalize-space(.)'/></xsl:variable>
    <xsl:choose>
      <xsl:when test="starts-with(@system, 'http://cve.mitre.org')">
        <phrase xlink:href='http://web.nvd.nist.gov/view/vuln/detail?vulnId={$ident}'><xsl:value-of select="$ident"/></phrase>
      </xsl:when>
      <!--<xsl:when test="starts-with(@system, 'http://cce.mitre.org')">
        <phrase xlink:href='http://open-scap.org/cce_view/?id={$ident}'><xsl:value-of select="$ident"/></phrase>
      </xsl:when>-->
      <xsl:when test="starts-with(@system, 'https://rhn.redhat.com/errata')">
        <phrase xlink:href='https://rhn.redhat.com/errata/{$ident}.html'><xsl:value-of select="$ident"/></phrase>
      </xsl:when>
      <xsl:otherwise>
        <phrase><xsl:value-of select="$ident"/></phrase>
      </xsl:otherwise>
    </xsl:choose>
  </listitem>
</xsl:template>
<xsl:template match='cdf:rear-matter'><colophon id='rear-matter'><xsl:apply-templates select='.' mode='db'/></colophon></xsl:template>

<xsl:template match='cdf:*'/>


<xsl:template name='profile-info'>
  <xsl:if test='$theprofile'>
    <chapter id='rule-selection'>
      <title>Rule Selection</title>
      <para>Based on profile: <emphasis role='strong'><xsl:value-of select='$theprofile/cdf:title[1]'/></emphasis> (<xsl:value-of select='$theprofile/@id'/>)</para>
      <xsl:apply-templates select='$theprofile/cdf:description[1]' mode='db'/>
      <informaltable>
        <tgroup>
          <thead><row><entry>Rule</entry><entry>selection</entry></row></thead>
          <tbody><xsl:apply-templates mode='profile-info' select='.//cdf:Rule[not(@hidden) or @hidden!="1" and @hidden!="true"]'/></tbody>
        </tgroup>
      </informaltable>
    </chapter>
  </xsl:if>
</xsl:template>

<xsl:template mode='profile-info' match='cdf:Rule'>
  <row>
    <entry><phrase xlink:href='#item-{@id}'><xsl:value-of select='cdf:title'/></phrase></entry>
    <xsl:choose>
      <xsl:when test='@selected!="1" and @selected!="true"'><entry role='rule-notselected'>not&#160;selected</entry></xsl:when>
      <xsl:when test='ancestor::cdf:Group[@selected="0" or @selected="false"]'><entry role='rule-inactive'>inactive</entry></xsl:when>
      <xsl:otherwise><entry role='rule-selected'>selected</entry></xsl:otherwise>
    </xsl:choose>
  </row>
</xsl:template>


</xsl:stylesheet>
