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

<!--
  Minimalist stylesheet to convert docbook to HTML.
  TODO:
-->

<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:htm="http://www.w3.org/1999/xhtml"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:db="http://docbook.org/ns/docbook"
    xmlns:exsl="http://exslt.org/common"
    xmlns:svg="http://www.w3.org/2000/svg"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    exclude-result-prefixes="xsl db exsl htm"
	>

<xsl:output method="xml" encoding="UTF-8" indent="no"
  doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
  doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
/>

<xsl:variable name='generator'/>

<xsl:template match='/'>
  <xsl:apply-templates select='/' mode='dbout.html'/>
</xsl:template>

<xsl:template mode='dbout.html' match='/'>
  <html xmlns="http://www.w3.org/1999/xhtml" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:svg="http://www.w3.org/2000/svg">
    <head>
      <xsl:apply-templates mode='dbout.html.head'/>
      <meta name="generator" content="{$generator}"/>
      <meta name="Content-Type" content="text/html;charset=utf-8"/>
      <xsl:call-template name='css'/>
    </head>
    <body>
      <xsl:apply-templates mode='dbout.html'/>
    </body>
  </html>
</xsl:template>

<xsl:template mode='dbout.html' match='db:book|db:article'>
  <div id='top'>
    <xsl:call-template name='dbout.html.attrs'/>
    <div id='header'><xsl:apply-templates mode='dbout.html.title' select='.'/></div>
    <!--<xsl:apply-templates mode='dbout.html.menu' select='.'/>-->
    <div id='content'><xsl:apply-templates mode='dbout.html'/></div>
    <div id='footer'><p>
      <xsl:if test='db:info/db:subtitle'><xsl:apply-templates mode='dbout.html' select='db:info/db:subtitle[1]/node()'/>. </xsl:if>
      <xsl:apply-templates mode='dbout.html' select='db:info/db:releaseinfo[@role="version"]/node()'/><xsl:text> </xsl:text>
      <xsl:apply-templates mode='dbout.html' select='db:info/db:releaseinfo[@role="footer"]/node()'/>
    </p></div>
  </div>
</xsl:template>

<!-- document structure -->
<xsl:template mode='dbout.html' match='db:chapter|db:section|db:colophon|db:preface|db:figure|db:note|db:warning'>
  <div>
    <xsl:call-template name='dbout.html.attrs'/>
    <xsl:apply-templates mode='dbout.html.title' select='.'/>
    <xsl:apply-templates mode='dbout.html' select='db:info/db:releaseinfo[@role="version"]|self::db:preface/../db:info/db:releaseinfo[@role="version"]'/>
    <xsl:apply-templates mode='dbout.html' select='node()[not(self::db:chapter|self::db:section|self::db:colophon|self::db:preface)]'/>
    <xsl:apply-templates mode='dbout.html.toc' select='.'/>
    <xsl:apply-templates mode='dbout.html.nav' select='.'/>
    <xsl:apply-templates mode='dbout.html' select='db:chapter|db:section|db:colophon|db:preface'/>
  </div>
</xsl:template>

<xsl:template mode='dbout.html' match='db:para|db:simpara|db:releaseinfo'>
  <p><xsl:call-template name='dbout.html.inline'/></p>
</xsl:template>

<xsl:template mode='dbout.html' match='db:programlisting'>
  <pre class='code'><xsl:call-template name='dbout.html.attrs'/><code><xsl:apply-templates mode='dbout.html'/></code></pre>
</xsl:template>

<xsl:template mode='dbout.html' match='db:itemizedlist'>
    <xsl:apply-templates mode='dbout.html.title' select='.'/>
    <ul class='{local-name()}'><xsl:call-template name='dbout.html.attrs'/><xsl:apply-templates mode='dbout.html' select='db:listitem'/></ul>
</xsl:template>

<xsl:template mode='dbout.html' match='db:revhistory'>
  <div>
    <xsl:call-template name='dbout.html.attrs'/>
    <xsl:apply-templates mode='dbout.html.title' select='.'/>
    <ul class='{local-name()}'>
      <xsl:apply-templates mode='dbout.html' select='db:revision'>
        <xsl:sort select='string(db:date)' order='descending'/>
      </xsl:apply-templates>
    </ul>
  </div>
</xsl:template>

<xsl:template mode='dbout.html' match='db:orderedlist|db:bibliolist|db:bibliography'>
  <div>
    <xsl:call-template name='dbout.html.attrs'/>
    <xsl:apply-templates mode='dbout.html.title' select='.'/>
    <ol class='{local-name()}'><xsl:apply-templates mode='dbout.html' select='db:listitem|db:biblioentry'/></ol>
  </div>
</xsl:template>

<xsl:template mode='dbout.html' match='db:listitem'>
  <xsl:choose>
    <xsl:when test='count(db:*)=1 and db:simpara[not(@*)]'><li><xsl:apply-templates name='dbout.html' select='db:simpara/node()'/></li></xsl:when>
    <xsl:otherwise><li><xsl:call-template name='dbout.html.inline'/></li></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode='dbout.html' match='db:revision'>
  <li><xsl:call-template name='dbout.html.attrs'/><strong><xsl:apply-templates name='dbout.html' select='db:revremark'/></strong>
  <xsl:if test='db:date'> (as of <xsl:apply-templates name='dbout.html' select='db:date'/>)</xsl:if></li>
</xsl:template>

<xsl:template mode='dbout.html' match='db:info|db:alt|db:title|db:subtitle|db:titleabbrev'/>
<xsl:template mode='dbout.html' match='db:title'/>

<!-- tables -->

<xsl:template mode='dbout.html' match='db:informaltable|db:table'>
  <div>
    <xsl:call-template name='dbout.html.attrs'/>
    <xsl:apply-templates mode='dbout.html.title' select='.'/>
    <table><xsl:apply-templates mode='dbout.html'/></table>
  </div>
</xsl:template>

<xsl:template mode='dbout.html' match='db:tgroup'><xsl:apply-templates mode='dbout.html'/></xsl:template>
<xsl:template mode='dbout.html' match='db:thead'><thead><xsl:call-template name='dbout.html.inline'/></thead></xsl:template>
<xsl:template mode='dbout.html' match='db:tbody'><tbody><xsl:call-template name='dbout.html.inline'/></tbody></xsl:template>
<xsl:template mode='dbout.html' match='db:row'><tr><xsl:call-template name='dbout.html.inline'/></tr></xsl:template>
<xsl:template mode='dbout.html' match='db:entry'>
  <td>
    <xsl:if test="@align">
      <xsl:attribute name="align"><xsl:value-of select="@align"/></xsl:attribute>  
    </xsl:if>
    <xsl:if test="@valign">
      <xsl:attribute name="valign"><xsl:value-of select="@valign"/></xsl:attribute>  
    </xsl:if>

    <xsl:call-template name='dbout.html.inline'/>
  </td>
</xsl:template>

<!-- inline elements -->

<xsl:template mode='dbout.html' match='db:abbrev|db:replaceable'>
  <abbr><xsl:call-template name='dbout.html.inline'/></abbr>
</xsl:template>

<xsl:template mode='dbout.html' match='db:phrase'>
  <span><xsl:call-template name='dbout.html.inline'/></span>
</xsl:template>

<xsl:template mode='dbout.html' match='db:phrase[@xlink:href]' name='dbout.html.link'>
  <a href='{@xlink:href}'><xsl:call-template name='dbout.html.inline'/></a>
</xsl:template>

<xsl:template mode='dbout.html' match='db:phrase[@role="br"]'><br/></xsl:template>

<xsl:template mode='dbout.html' match='db:emphasis'>
  <em><xsl:call-template name='dbout.html.inline'/></em>
</xsl:template>

<xsl:template mode='dbout.html' match='db:code'>
  <code><xsl:call-template name='dbout.html.inline'/></code>
</xsl:template>

<xsl:template mode='dbout.html' match='db:emphasis[@role="strong" or @role="bold"]'>
  <strong><xsl:call-template name='dbout.html.inline'/></strong>
</xsl:template>

<xsl:template name='dbout.html.date' mode='dbout.html' match='db:date|db:phrase[@role="date"]'>
  <abbr title='{.}' class='date'><xsl:value-of select='translate(substring(.,1,16),"T"," ")'/></abbr>
</xsl:template>

<!-- bibliography -->

<xsl:template mode='dbout.html' match='db:biblioentry'>
  <li>
    <xsl:call-template name='dbout.html.attrs'/>
    <!-- author -->
    <xsl:if test='db:author'>
      <span class='bib-author'>
        <xsl:for-each select='db:author'>
          <xsl:choose>
            <xsl:when test='not(preceding-sibling::db:author)'><xsl:apply-templates mode='dbout.html'/></xsl:when>
            <xsl:when test='count(../db:author) &gt; 3 and generate-id(../db:author[2]) = generate-id(.)'> et&#160;al</xsl:when>
            <xsl:when test='count(../db:author) &gt; 3'></xsl:when>
            <xsl:when test='not(following-sibling::db:author)'>, and <xsl:apply-templates mode='dbout.html'/></xsl:when>
            <xsl:otherwise>, <xsl:apply-templates mode='dbout.html'/></xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
        <xsl:text>.</xsl:text>
      </span>
      <xsl:text> </xsl:text>
    </xsl:if>
    <!-- title -->
    <xsl:if test='db:title'>
      <em class='bib-title'><xsl:apply-templates mode='dbout.html' select='db:title[1]'/>
        <xsl:if test='db:subtitle'>: <xsl:apply-templates mode='dbout.html' select='db:subtitle'/></xsl:if>
      </em>
      <xsl:text>. </xsl:text>
    </xsl:if>
    <!-- publisher, year -->
    <xsl:if test='db:publishername|db:date'>
      <xsl:apply-templates mode='dbout.html' select='db:publishername[1]'/>
      <xsl:if test='db:date and db:publishername'>, </xsl:if>
      <xsl:apply-templates mode='dbout.html' select='db:date[1]'/>
      <xsl:text>. </xsl:text>
    </xsl:if>
    <xsl:for-each select='db:biblioid'><xsl:apply-templates mode='dbout.html'/>. </xsl:for-each>
  </li>
</xsl:template>

<xsl:template mode='dbout.html' match='db:personname|db:publishername'>
  <span class='{local-name()}'><xsl:call-template name='dbout.html.inline'/></span>
</xsl:template>

<xsl:template mode='dbout.html' match='db:biblioentry/db:title|db:biblioentry/db:subtitle'>
  <span class='bib-{local-name()}'><xsl:call-template name='dbout.html.inline'/></span>
</xsl:template>

<xsl:template mode='dbout.html' match='db:biblioid/db:phrase[@xlink:href]' priority='1'>
  <xsl:text>URL:&#160;&#60;</xsl:text><xsl:call-template name='dbout.html.link'/><xsl:text>&#62;</xsl:text>
</xsl:template>

<!-- headings (titles) -->

<xsl:template mode='dbout.html.title' match='db:*' priority='-2'>
  <xsl:apply-templates select='(db:info/db:title|db:title|db:titleabbrev[@role="short"]|db:subtitle[@role="prefer"])[last()]' mode='dbout.html.title'/>
</xsl:template>

<xsl:template mode='dbout.html.title' match='db:book/db:title|db:book/db:info/db:title|db:article/db:title|db:article/db:info/db:title'>
  <h1><xsl:call-template name='dbout.html.inline'/></h1>
</xsl:template>

<xsl:template mode='dbout.html.title' match='db:chapter/db:title|db:chapter/db:info/db:title|db:preface/db:title|db:preface/db:info/db:title|db:bibliography/db:title|db:bibliography/db:info/db:title'>
  <h2><xsl:apply-templates mode='dbout.html.num' select='ancestor::db:chapter[1]'/><xsl:call-template name='dbout.html.inline'/></h2>
</xsl:template>

<xsl:template mode='dbout.html.title' match='db:section/db:title|db:section/db:info/db:title'>
  <h3><xsl:apply-templates mode='dbout.html.num' select='ancestor::db:section[1]'/><xsl:call-template name='dbout.html.inline'/></h3>
</xsl:template>

<xsl:template mode='dbout.html.title' match='db:title|db:titleabbrev' priority='-1'>
  <h4><xsl:call-template name='dbout.html.inline'/></h4>
</xsl:template>

<!-- defaults -->
<xsl:template mode='dbout.html.title' match='db:revhistory[not(db:title)]'><h4>Revision history</h4></xsl:template>
<xsl:template mode='dbout.html.title' match='db:colophon[not(db:title)]'><h2 id='colophon'>Colophon</h2></xsl:template>
<xsl:template mode='dbout.html.title' match='text()'/>

<!-- numbering -->

<xsl:template name='dbout.html.num.section'>
  <xsl:number level='multiple' count='db:section|db:chapter' format='1.'/>
</xsl:template>

<xsl:template mode='dbout.html.num' match='db:section[@role="xccdf-group"]|db:chapter[@role="xccdf-group"]'>
  <xsl:call-template name='dbout.html.num.section'/><xsl:text> </xsl:text>
</xsl:template>

<xsl:template mode='dbout.html.num' match='db:section[@role="xccdf-rule"]'>
  <xsl:for-each select='..'><xsl:call-template name='dbout.html.num.section'/></xsl:for-each><xsl:number format='a. '/>
</xsl:template>

<xsl:template mode='dbout.html.num' match='node()'/>

<!-- Default template -->
<xsl:template mode='dbout.html' match='*'>
  <xsl:message>HTML output filter warning: unhandeled element '<xsl:value-of select='name()'/>'.</xsl:message>
  <xsl:apply-templates mode='dbout.html'/>
</xsl:template>

<!-- TOC templates -->

<xsl:template mode='dbout.html.toc' match='db:preface'><xsl:apply-templates mode='dbout.html.toc' select='..'/></xsl:template>

<xsl:template mode='dbout.html.toc' match='db:chapter|db:book|db:article'>
  <xsl:variable name='sub'><xsl:apply-templates mode='dbout.html.tocdo'/></xsl:variable>
  <xsl:if test='normalize-space($sub)'>
    <div id='toc-of-{@id}'>
      <h4>Table of Contents</h4>
      <ul class='toc-struct'><xsl:copy-of select='$sub'/></ul>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template mode='dbout.html.tocdo' match='db:section|db:chapter'>
  <xsl:variable name='sub'><xsl:apply-templates mode='dbout.html.tocdo'/></xsl:variable>
  <li id='toc-{@id}'><xsl:call-template name='dbout.html.nlink'/>
    <xsl:if test='normalize-space($sub) and not(self::db:chapter)'><ul><xsl:copy-of select='$sub'/></ul></xsl:if>
  </li>
</xsl:template>

<!--<xsl:template mode='dbout.html.tocdo' match='db:section[@role="xccdf-rule"]'/>-->

<xsl:template mode='dbout.html.toc' match='node()'/>
<xsl:template mode='dbout.html.tocdo' match='node()'/>
<xsl:template mode='dbout.html.toctitle' match='node()'/>

<!-- navigation templates -->

<xsl:template mode='dbout.html.nav' match='db:chapter[contains(@role, "xccdf-")]|db:section[contains(@role, "xccdf-")]'>
  <p class='link'>
      <xsl:call-template name='dbout.html.nlink'>
        <xsl:with-param name='text'   select='"link"'/>
      </xsl:call-template>
    | <xsl:call-template name='dbout.html.nlink'>
        <xsl:with-param name='tgt'  select='(preceding-sibling::db:chapter|preceding-sibling::db:section)[last()]'/>
        <xsl:with-param name='text' select='"previous"'/>
      </xsl:call-template>
    | <xsl:call-template name='dbout.html.nlink'>
        <xsl:with-param name='tgt'  select='(following-sibling::db:chapter|following-sibling::db:section)[1]'/>
        <xsl:with-param name='text' select='"next"'/>
      </xsl:call-template>
    | <xsl:call-template name='dbout.html.nlink'>
        <xsl:with-param name='tgt'  select='(ancestor::db:book|ancestor::db:chapter|ancestor::db:section)[last()]'/>
        <xsl:with-param name='text' select='"up"'/>
      </xsl:call-template>
    | <xsl:call-template name='dbout.html.nlink'>
        <xsl:with-param name='text'   select='"toc"'/>
        <xsl:with-param name='prefix' select='"toc-"'/>
      </xsl:call-template>
    | <xsl:call-template name='dbout.html.nlink'>
        <xsl:with-param name='tgt'  select='(ancestor::db:chapter)[last()]'/>
        <xsl:with-param name='text' select='"home"'/>
      </xsl:call-template>
  </p>
</xsl:template>

<xsl:template mode='dbout.html.nav' match='db:section[@role="result-detail"]'>
  <p class='link'><a href='#results-overview'>results overview</a></p>
</xsl:template>

<xsl:template mode='dbout.html.nav' match='node()'/>

<xsl:template name='dbout.html.nlink'>
  <xsl:param name='tgt' select='.'/>
  <xsl:param name='text'>
    <xsl:apply-templates select='$tgt' mode='dbout.html.num'/>
    <xsl:value-of select='$tgt/db:title'/>
  </xsl:param>
  <xsl:param name='prefix'/>
  <xsl:param name='title' select='string(concat($tgt/db:title, " (", $tgt/@id, ")"))'/>
  <xsl:choose>
    <xsl:when test='$tgt'><a title='{$title}' href='#{$prefix}{$tgt/@id}'><xsl:value-of select='$text'/></a></xsl:when>
    <xsl:when test='$text'><span class='unknown'><xsl:value-of select='$text'/></span></xsl:when>
  </xsl:choose>
</xsl:template>

<!-- HEAD templates -->

<xsl:template mode='dbout.html.head' match='db:article|db:book'>
  <title><xsl:value-of select='(db:title|db:info/db:title)[1]'/><xsl:for-each select='db:info/db:subtitle[1]'> – <xsl:value-of select='.'/></xsl:for-each></title>
</xsl:template>

<xsl:template mode='dbout.html.head' match='node()'/>

<!-- images & media -->

<xsl:template mode='dbout.html' match='db:inlinemediaobject'>
  <xsl:for-each select='(db:audioobject|db:imageobject|db:imageobjectco|db:textobject|db:videoobject)[1]'>
    <span class='media'><xsl:call-template name='dbout.html.inline'/></span>
  </xsl:for-each>
</xsl:template>

<xsl:template mode='dbout.html' match='db:imagedata'>
  <xsl:apply-templates mode='dbout.html'/>
</xsl:template>

<xsl:template mode='dbout.html' match='svg:*'><xsl:copy-of select='.'/></xsl:template>

<!-- functions -->

<xsl:template name='dbout.html.attrs'>
  <xsl:if test='@role'><xsl:attribute name='class'><xsl:value-of select='@role'/></xsl:attribute></xsl:if>
  <xsl:if test='@id'><xsl:attribute name='id'><xsl:value-of select='@id'/></xsl:attribute></xsl:if>
  <xsl:if test='@xlink:href'><xsl:attribute name='href'><xsl:value-of select='@xlink:href'/></xsl:attribute></xsl:if>
  <xsl:if test='db:alt'><xsl:attribute name='title'><xsl:value-of select='normalize-space(db:alt[1])'/></xsl:attribute></xsl:if>
</xsl:template>

<xsl:template name='dbout.html.inline'>
  <xsl:call-template name='dbout.html.attrs'/>
  <xsl:apply-templates mode='dbout.html'/>
</xsl:template>


<!-- menu -->
<xsl:template mode='dbout.html.menu' match='db:book|db:article'>
  <div id='menu'>
    <h2>Page contents</h2>
    <ul>
      <xsl:apply-templates mode='dbout.html.menu'/>
    </ul>
  </div>
</xsl:template>

<xsl:template mode='dbout.html.menu' match='db:chapter|db:preface|db:bibliography'>
  <li><a href="#{@id}"><xsl:value-of select='(self::db:preface/../db:info/db:titleabbrev|db:title)[1]'/></a></li>
</xsl:template>

<xsl:template mode='dbout.html.menu' match='db:colophon'>
  <li><a href="#{@id}">Colophon</a></li>
</xsl:template>

<xsl:template mode='dbout.html.menu' match="node()"/>

<!-- CSS stylesheet -->
<xsl:template name='css'>
  <style type='text/css' media='all'>
    html, body { background-color: black; font-family:sans-serif; margin:0; padding:0; }
    abbr { text-transform:none; border:none; font-variant:normal; }
    div.score-outer { height: .8em; width:100%; min-width:100px; background-color: red; }
    div.score-inner { height: 100%; background-color: green; }
    .score-max, .score-val, .score-percent { text-align:right; }
    .score-percent { font-weight: bold; }
    th, td { padding-left:.5em; padding-right:.5em; }
    .rule-selected, .result-pass strong, .result-fixed strong { color:green; }
    .rule-inactive, .unknown, .result-notselected strong, .result-notchecked strong, .result-notapplicable strong, .result-informational strong, .result-unknown strong { color:#555; }
    .rule-notselected, .result-error strong, .result-fail strong { color:red; }
    table { border-collapse: collapse; border: 1px black solid; width:100%; }
    table th, thead tr { background-color:black; color:white; }
    table td { border-right: 1px black solid; }
    table td.result, table td.link { text-align:center; }
    table td.num { text-align:right; }
    div#rule-results-summary { margin-bottom: 1em; }
    table tr.result-legend td { width: 10%; }
    div#content p { text-align:justify; }
    div.result-detail { border: 1px solid black; margin: 2em 0; padding: 0 1em; }
    div#content h2 { border-bottom:2px dashed; margin-top:1em; margin-bottom:0.5em; text-align:center; }
    div#content h2#summary { margin-top:0; }
    h1 { margin:1em 0; }
    div.raw table, div.raw table td { border:none; width:auto; padding:0; }
    div.raw table { margin-left: 2em; }
    div.raw table td { padding: .1em .7em; }
    table tr { border-bottom: 1px dotted #000; }
    dir.raw table tr { border-bottom: 0 !important; }
    pre.code { background: #ccc; padding:.2em; }
    ul.toc-struct li { list-style-type: none; }
    div.xccdf-rule { margin-left: 10%; }
    div#footer, p.remark, .link { font-size:.8em; }
    thead tr td { font-weight:bold; text-align:center; }
    .hidden { display:none; }
    td.score-bar { text-align:center; }
    td.score-bar span.media { width:100%; min-width:7em; height:.8em; display:block; margin:0; padding:0; }
    .oval-results { font-size:.8em; overflow:auto; }
    div#guide-top-table table { width: 100%; }
    td#common-info { min-width: 25.0em; border-right: 1px solid #000; }
    td#versions-revisions { width: 25.0em; }
  </style>
  <style type='text/css' media='screen'>
    div#content, div#header, div#footer { margin-left:1em; margin-right:1em; }
    div#content { background-color: white; padding:2em; }
    div#footer, div#header { color:white; text-align:center; }
    a, a:visited { color:blue; text-decoration:underline; }
    div#content p.link { text-align:right; font-size:.8em; }
    div#footer a { color:white; }
    div.xccdf-group, div.xccdf-rule { border-left: 3px solid white; padding-left:.3em; }
    div.xccdf-group:target, div.xccdf-rule:target { border-left-color:#ccc; }
    .toc-struct li:target { background:#ddd; }
    abbr { border-bottom: 1px black dotted; }
    abbr.date { border-bottom:none; }
    pre.code { overflow:auto; }
    table tbody tr:hover { background: #ccc; }
    div.raw table tbody tr:hover { background: transparent !important; }
  </style>
  <style type='text/css' media='print'>
    @page { margin:3cm; }
    html, body { background-color:white; font-family:serif; }
    .link { display:none; }
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

</xsl:stylesheet>
