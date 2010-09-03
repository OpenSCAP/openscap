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


<xsl:stylesheet version="1.1"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:h="http://www.w3.org/1999/xhtml"
    xmlns:fn="http://www.w3.org/2005/xpath-functions"
    xmlns:exsl="http://exslt.org/common"
    xmlns:func="http://exslt.org/functions"
    xmlns:s="http://open-scap.org/"
    xmlns:edate="http://exslt.org/dates-and-times">


<!-- variable with current date/time -->
<xsl:variable name='now' select='edate:date-time()'/>
<xsl:variable name='generator' select="'OpenSCAP stylesheet'"/>
<xsl:param name="oscap-version"/>


<!--
    mode 'toc'
    replace <div id='toc'/> by table of contents generated from h2 elements
    (just a flat list with links, h2's must have an 'id' attribute)
-->
<xsl:template mode='toc' match="h:div[@id='toc']">
  <div id='toc'>
    <h2>Page contents</h2>
    
    <ul>
      <xsl:for-each select="//h:div[@id='content']//h:h2[@id]">
        <li><a href="#{@id}"><xsl:value-of select='.'/></a></li>
      </xsl:for-each>
    </ul>
  </div>
</xsl:template>

<xsl:template mode='toc' match='node()|@*'><xsl:copy><xsl:apply-templates mode='toc' select="node()|@*"/></xsl:copy></xsl:template>

<!-- list template -->
<xsl:template name='list'>
    <xsl:param name='nodes'/>
    <xsl:param name='title' select='name($nodes[1])'/>
    <xsl:param name='el' select='"ul"'/>
    <xsl:if test='$nodes'>
      <h3 id='list-{local-name($nodes[1])}'><xsl:value-of select='$title'/></h3>
      <xsl:element name='{$el}'>
        <xsl:for-each select='$nodes'>
          <li><xsl:value-of select='.'/></li>
        </xsl:for-each>
      </xsl:element>
    </xsl:if>
</xsl:template>

<!-- ifelse template -->
<xsl:template name='ifelse'>
    <xsl:param name='test'/>
    <xsl:param name='true'/>
    <xsl:param name='false'><em class='unknown'>unknown</em></xsl:param>
    <xsl:if test='$test'><xsl:copy-of select='$true'/></xsl:if>
    <xsl:if test='not($test)'><xsl:copy-of select='$false'/></xsl:if>
</xsl:template>

<!-- ifelse function (does not work with libxslt + libexslt) -->
<func:function name='s:if'>
    <xsl:param name='test'/>
    <xsl:param name='true'/>
    <xsl:param name='false'/>
    <xsl:if test='true()'>
        <func:result>
            <xsl:choose>
                <xsl:when test='$test'><xsl:copy-of select='$true'/></xsl:when>
                <xsl:otherwise><xsl:copy-of select='$false'/></xsl:otherwise>
            </xsl:choose>
        </func:result>
    </xsl:if>
</func:function>

<!-- date template -->
<xsl:template mode='date' match='text()|@*'>
  <abbr title='{.}' class='date'><xsl:value-of select='translate(substring(.,1,16),"T"," ")'/></abbr>
</xsl:template>
<xsl:template mode='date' match='node()'><xsl:copy><xsl:apply-templates mode='date' select="node()"/></xsl:copy></xsl:template>

<!-- CSS stylesheet -->
<xsl:template name='css'>
  <style type='text/css' media='all'>
    html, body { background-color: black; font-family:sans-serif; margin:0; padding:0; }
    abbr { text-transform:none; border:none; font-variant:normal; }
    div.score-outer { height: .8em; width:100%; min-width:100px; background-color: red; }
    div.score-inner { height: 100%; background-color: green; }
    .score-max, .score-val { text-align:right; }
    th, td { padding-left:.5em; padding-right:.5em; }
    .rule-selected, .result-pass strong, .result-fixed strong { color:green; }
    .rule-inactive, .unknown, .result-notselected strong, .result-notchecked strong, .result-notapplicable strong, .result-informational strong, .result-unknown strong { color:#555; }
    .rule-notselected, .result-error strong, .result-fail strong { color:red; }
    table { border-collapse: collapse; border: 1px black solid; width:100%; }
    table th { background-color:black; color:white; }
    table td { border-right: 1px black solid; }
    table td.result, table td.link { text-align:center; }
    div#content p { text-align:justify; }
    div.result-detail { border: 1px solid black; margin: 2em 0; padding: 0 1em; }
    div#content h2 { border-bottom:2px dashed; margin-top:2em; margin-bottom:1.5em; text-align:center; }
    div#content h2#summary { margin-top:1em; }
    h1 { margin:1em 0; }
    table.raw, table.raw td { border:none; width:auto; padding:0; }
    table.raw { margin-left: 2em; }
    table.raw td { padding: .1em .7em; }
    pre.code { background: #ccc; }
    ul.toc-struct li { list-style-type: none; }
    div.group-rules { margin-left: 10%; }
    div#footer, p.remark, .link { font-size:.8em; }
  </style>
  <style type='text/css' media='screen'>
    div#content, div#header, div#footer { margin-left:5%; margin-right:25%; }
    div#content { background-color: white; padding:2em; }
    div#footer, div#header { color:white; text-align:center; }
    a, a:visited { color:blue; text-decoration:underline; }
    div#content p.link { text-align:right; font-size:.8em; }
    div#toc { position:absolute; left:80%; top:6.5em; width:15%; background-color:white; padding:0; }
    div#toc[id="toc"] { position:fixed; } /* IE6 hack (won't be interpreted by IE6) */
    div#toc ul, div#toc ul li { margin:0; padding:0; list-style-type:none; font-size:.9em; }
    div#toc h2 { display:none; }
    div#toc a { display:block; text-align:center; padding:.3em 1em; text-decoration:none; }
    div#toc a:hover { background: #ccc; }
    div#footer a { color:white; }
    div.section { border-left: 3px solid white; padding-left:.5em; }
    div.section:target { border-left-color:#ccc; }
    .toc-struct li:target { background:#ddd; }
    abbr { border-bottom: 1px black dotted; }
    abbr.date { border-bottom:none; }
  </style>
  <style type='text/css' media='print'>
    @page { margin:3cm; }
    html, body { background-color:white; font-family:serif; }
    .link, div#toc { display:none; }
    a, a:visited { color:black; text-decoration:none; }
    div#header, div#footer { text-align:center; }
    div#header { padding-top:36%; }
    h1 { vertical-align:center; }
    h2 { page-break-before:always; }
    h3, h4, h5  { page-break-after:avoid; }
    pre.code { background: #ccc; }
    div#footer { margin-top:auto; }
    .toc-struct { page-break-after:always; }
  </style>
</xsl:template>

<!-- document structure -->
<xsl:template name='skelet'>
    <xsl:param name='title' />
    <xsl:param name='file' />
    <xsl:param name='content' />
    <xsl:param name='footer' />
    <xsl:variable name='cnt'>
      <html xmlns="http://www.w3.org/1999/xhtml"
            xmlns:h="http://www.w3.org/1999/xhtml"
            xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
            xmlns:fn="http://www.w3.org/2005/xpath-functions"
            xmlns:exsl="http://exslt.org/common"
            xmlns:edate="http://exslt.org/dates-and-times">
        <head>
          <xsl:if test='$title'><title><xsl:value-of select='$title'/></title></xsl:if>
          <meta name="generator" content="{$generator}"/>
          <meta name="Content-Type" content="text/html;charset=utf-8"/>
          <xsl:call-template name='css'/>
        </head>
        <body>
          <xsl:if test='$title'><div id='header'><h1><xsl:copy-of select='$title'/></h1></div></xsl:if>
          <div id='toc'/>
          <div id='content'>
            <xsl:copy-of select='$content'/>
          </div>
          <div id='footer'>
            <p><xsl:copy-of select='$footer'/>
            Generated by <a href="http://open-scap.org">OpenSCAP</a><xsl:if test='$oscap-version'> v<xsl:value-of select='$oscap-version'/></xsl:if>
            on <xsl:apply-templates mode='date' select='exsl:node-set($now)'/>.</p>
          </div>
        </body>
      </html>
    </xsl:variable>
    <xsl:apply-templates mode='toc' select='exsl:node-set($cnt)'/>
</xsl:template>


</xsl:stylesheet>
