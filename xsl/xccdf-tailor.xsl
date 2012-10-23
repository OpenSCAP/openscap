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

<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:htm="http://www.w3.org/1999/xhtml"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
	>

<xsl:param name='profile'/>
<xsl:param name='keep-profiles'/>
<xsl:param name='clean-profile-notes'/>

<xsl:variable name='theprofile' select='/cdf:Benchmark/cdf:Profile[@id=$profile]'/>

<!-- identity transform -->
<xsl:template mode='profile' match='node()|@*'>
  <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
</xsl:template>

<!-- omit profiles if needed -->
<xsl:template mode='profile' match='cdf:Profile'>
  <xsl:if test='$keep-profiles'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
  </xsl:if>
</xsl:template>

<!-- adjust 'selected' attribute and refine-rules -->
<xsl:template mode='profile' match='cdf:Group|cdf:Rule'>
  <xsl:variable name='id' select='@id'/>
  <xsl:variable name='cid' select='@cluster-id'/>
  <xsl:variable name='sel' select='$theprofile/cdf:select[@idref=$id or @idref=$cid][1]'/>
  <xsl:variable name='rr' select='$theprofile/cdf:refine-rule[@idref=$id or @idref=$cid][1]'/>
  <xsl:copy>
    <xsl:apply-templates select='@*' mode='profile'/>
    <xsl:if test='self::cdf:Rule and $rr'><xsl:copy-of select='$rr/@role|$rr/@severity|$rr/@weight'/></xsl:if>
    <xsl:if test='$sel'><xsl:attribute name='selected'><xsl:value-of select='$sel/@selected'/></xsl:attribute></xsl:if>
    <xsl:apply-templates mode='profile' select="node()"/>
  </xsl:copy>
</xsl:template>

<!-- apply refine-rule selectors -->
<xsl:template mode='profile' match='cdf:check'>
  <xsl:variable name='name' select='name()'/>
  <xsl:variable name='rule' select='ancestor::cdf:Rule[1]'/>
  <xsl:variable name='rr' select='$theprofile/cdf:refine-rule[@idref=$rule/@id or @idref=$rule/@cluster-id][1]'/>
  <xsl:for-each select='self::*[@selector=$rr/@selector or ((not(@selector) or @selector="") and count($rule/*[name()=$name][@selector=$rr/@selector])=0)]'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
  </xsl:for-each>
</xsl:template>

<!-- remove irelevant profile-notes -->
<xsl:template mode='profile' match='cdf:profile-note'>
  <xsl:if test='not($clean-profile-notes) or (@tag = $theprofile/@note-tag)'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
  </xsl:if>
</xsl:template>

<!-- implement refine-value -->
<xsl:template mode='profile' match='cdf:Value'>
  <xsl:variable name='id' select='@id'/>
  <xsl:variable name='cid' select='@cluster-id'/>
  <xsl:variable name='rv' select='$theprofile/cdf:refine-value[@idref=$id or @idref=$cid][1]'/>
  <xsl:variable name='sv' select='$theprofile/cdf:set-value[@idref=$id or @idref=$cid][1]'/>
  <xsl:copy>
    <xsl:apply-templates select='@*' mode='profile'/>
    <xsl:if test='$rv'><xsl:copy-of select='$rv/@operator'/></xsl:if>
    <xsl:apply-templates mode='profile' select="cdf:status|cdf:version|cdf:title|cdf:description|cdf:warning|cdf:question|cdf:reference"/>
    <xsl:choose>
      <xsl:when test='$sv'><cdf:value><xsl:value-of select='$sv'/></cdf:value></xsl:when>
      <xsl:otherwise><xsl:apply-templates mode='profile' select="cdf:value"/></xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates mode='profile' select="cdf:default|cdf:match|cdf:lower-bound|cdf:upper-bound|cdf:choices|cdf:source"/>
  </xsl:copy>
</xsl:template>

<!-- refine-value selector -->
<xsl:template mode='profile' match='cdf:value|cdf:default|cdf:match|cdf:lower-bound|cdf:upper-bound|cdf:choices'>
  <xsl:variable name='val' select='ancestor::cdf:Value'/>
  <xsl:variable name='name' select='name()'/>
  <xsl:variable name='rv' select='$theprofile/cdf:refine-value[@idref=$val/@id or @idref=$val/@cluster-id][1]'/>
  <xsl:for-each select='self::*[@selector=$rv/@selector or ((not(@selector) or @selector="") and count($val/*[name()=$name][@selector=$rv/@selector])=0)]'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"/></xsl:copy>
  </xsl:for-each>
</xsl:template>

<!-- erase @selector -->
<xsl:template mode='profile' match='@selector[not(ancestor::cdf:Profile)]'/>

<!-- top-level template -->
<xsl:template match='/'>
  <xsl:apply-templates mode='apply-profile'/>
</xsl:template>

<xsl:template mode='apply-profile' match='cdf:Benchmark'>
  <xsl:call-template name='warn-unresolved'/>
  <xsl:apply-templates select='.' mode='profile'/>
</xsl:template>

</xsl:stylesheet>

