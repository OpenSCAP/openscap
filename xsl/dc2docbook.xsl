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
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:xlink="http://www.w3.org/1999/xlink"
                xmlns:dc="http://purl.org/dc/elements/1.1/"
                xmlns:db="http://docbook.org/ns/docbook"
                xmlns="http://docbook.org/ns/docbook"
                exclude-result-prefixes="xsl xsi dc db"
>

<xsl:output method="xml" encoding="UTF-8" indent="yes"/>

<xsl:template mode='dc2db' match='dc:title'>
  <title><xsl:copy-of select='@xml:lang'/><xsl:apply-templates mode='dc2db'/></title>
</xsl:template>

<xsl:template mode='dc2db' match='dc:title[preceding-sibling::dc:title]'>
  <subtitle><xsl:copy-of select='@xml:lang'/><xsl:apply-templates mode='dc2db'/></subtitle>
</xsl:template>

<xsl:template mode='dc2db' match='dc:creator'>
  <author><personname><xsl:apply-templates mode='dc2db'/></personname></author>
</xsl:template>

<xsl:template mode='dc2db.subject' match='dc:subject'>
  <subject><subjectterm><xsl:apply-templates mode='dc2db'/></subjectterm></subject>
</xsl:template>

<xsl:template mode='dc2db' match='dc:description'>
  <abstract><xsl:copy-of select='@xml:lang'/><xsl:apply-templates mode='dc2db'/></abstract>
</xsl:template>

<xsl:template mode='dc2db' match='dc:publisher'>
  <publishername><xsl:apply-templates mode='dc2db'/></publishername>
</xsl:template>

<xsl:template mode='dc2db' match='dc:contributor'>
  <collab><personname><xsl:apply-templates mode='dc2db'/></personname></collab>
</xsl:template>

<xsl:template mode='dc2db' match='dc:date'>
  <date><xsl:apply-templates mode='dc2db'/></date>
</xsl:template>

<xsl:template mode='dc2db' match='dc:identifier'>
  <!-- TODO: get class properly -->
  <biblioid><xsl:call-template name='dc2db.link'/></biblioid>
</xsl:template>

<xsl:template mode='dc2db' match='dc:source'>
  <!-- TODO: get class properly -->
  <bibliosource><xsl:call-template name='dc2db.link'/></bibliosource>
</xsl:template>

<xsl:template name='dc2db.link'>
  <xsl:attribute name='class'>other</xsl:attribute>
  <xsl:attribute name='otherclass'>unknown</xsl:attribute>
  <phrase>
    <xsl:if test='contains(string(.), "://")'><xsl:attribute name='xlink:href'><xsl:apply-templates mode='dc2db'/></xsl:attribute></xsl:if>
    <xsl:apply-templates mode='dc2db'/>
  </phrase>
</xsl:template>

<xsl:template mode='dc2db' match='dc:relation'>
  <!-- TODO: get class and type properly -->
  <bibliorelation class='uri' type='other' othertype='unknown'><xsl:apply-templates mode='dc2db'/></bibliorelation>
</xsl:template>

<xsl:template mode='dc2db' match='dc:rights'>
  <legalnotice><title><xsl:apply-templates mode='dc2db'/></title></legalnotice>
</xsl:template>

<xsl:template mode='dc2db' match='dc:*'/>
<xsl:template mode='dc2db' match='text()'><xsl:value-of select='normalize-space(.)'/></xsl:template>

<xsl:template name='dc2db' mode='dc2db' match='*'>
  <!-- TODO dc:coverage? -->
  <xsl:apply-templates mode='dc2db' select='dc:*'/>
  <xsl:if test='dc:subject'>
    <subjectset><xsl:apply-templates mode='dc2db.subject' select='dc:subject'/></subjectset>
  </xsl:if>
</xsl:template>

<xsl:template name='dc2db.entry' mode='dc2db' match='dc:record'>
  <biblioentry>
    <xsl:if test='dc:language'><xsl:attribute name='xml:lang'><xsl:value-of select='dc:language[1]'/></xsl:attribute></xsl:if>
    <xsl:call-template name='dc2db'/>
  </biblioentry>
</xsl:template>

<xsl:template mode='dc2db' match='*'><xsl:apply-templates mode='dc2db' select='*'/></xsl:template>

<xsl:template match='/'>
  <bibliography>
    <title>Bibliography</title>
    <xsl:choose>
      <xsl:when test='.//dc:record'><xsl:apply-templates mode='dc2db'/></xsl:when>
      <xsl:when test='.//dc:*'><xsl:call-template name='dc2db.entry'/></xsl:when>
    </xsl:choose>
  </bibliography>
</xsl:template>

</xsl:stylesheet>
