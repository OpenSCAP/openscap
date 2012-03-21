<?xml version="1.0" encoding="UTF-8"?>

<!--
Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
Authors:
      Martin Preisler <mpreisle@redhat.com>
-->

<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xccdf_11="http://checklists.nist.gov/xccdf/1.1"
  xmlns:xccdf_12="http://checklists.nist.gov/xccdf/1.2"

  version="1.0">
 
  <xsl:output method="xml" indent="yes"/>

  <xsl:param name="reverse_DNS"/>

  <!-- This is the generic matched template that by default copies everything
       verbatim, every template that is more specifically matched that this
       will take precedence -->  
  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

  <!-- Deals with namespace change (xccdf 1.1 -> xccdf 1.2) -->
  <xsl:template match="xccdf_11:*"> <!-- select just elements from xccdf 1.1 -->
    <xsl:choose>
      <!-- Remove deprecated elements -->
      <xsl:when test="local-name()='platform-definitions' or local-name()='Platform-Specification' or local-name()='cpe-list' or local-name()='impact-metric'">
        <!-- We will do a trick here to "comment" the deprecated elements,
             XSLT itself ignores all nodes inside <xsl:comment> except TEXT
             nodes, so we can't use that. -->
        <xsl:text disable-output-escaping="yes">&lt;--</xsl:text>
        This element has been deprecated and was removed from XCCDF 1.2!

        <xsl:element name="{local-name()}">
          <xsl:apply-templates select="@*"/>
          <xsl:apply-templates/>
        </xsl:element>
        <xsl:text disable-output-escaping="yes">--&gt;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:element name="{local-name()}" namespace="http://checklists.nist.gov/xccdf/1.2">
          <!-- We have basically added an element with the same local name but xccdf 1.2 namespace,
               now we want to apply templates to all attributes of it and all subelements -->
          <xsl:apply-templates select="@*"/>
          <xsl:apply-templates/>
        </xsl:element>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Deals with the new enforced id format of Benchmark -->
  <xsl:template match="@id[parent::xccdf_11:Benchmark]"> 
    <xsl:variable name="old_id" select="."/>

    <xsl:attribute name="id">
      <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_benchmark_', $old_id)"/>
    </xsl:attribute>
  </xsl:template>

  <!-- Deals with the new enforced id format of Rule -->
  <xsl:template match="@id[parent::xccdf_11:Profile]">
    <xsl:variable name="old_id" select="."/>

    <xsl:attribute name="id">
      <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_profile_', $old_id)"/>
    </xsl:attribute>
  </xsl:template>

  <!-- Deals with the new enforced id format of Rule -->
  <xsl:template match="@id[parent::xccdf_11:Rule]">
    <xsl:variable name="old_id" select="."/>

    <xsl:attribute name="id">
      <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_rule_', $old_id)"/>
    </xsl:attribute>
  </xsl:template>

  <!-- Deals with the new enforced id format of Group -->
  <xsl:template match="@id[parent::xccdf_11:Group]">
    <xsl:variable name="old_id" select="."/>

    <xsl:attribute name="id">
      <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_group_', $old_id)"/>
    </xsl:attribute>
  </xsl:template>

  <!-- Deals with the new enforced id format of Value -->
  <xsl:template match="@id[parent::xccdf_11:Value]">
    <xsl:variable name="old_id" select="."/>

    <xsl:attribute name="id">
      <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_value_', $old_id)"/>
    </xsl:attribute>
  </xsl:template>

  <!-- Deals with the new enforced id format of TestResult -->
  <xsl:template match="@id[parent::xccdf_11:TestResult]">
    <xsl:variable name="old_id" select="."/>

    <xsl:attribute name="id">
      <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_testresult_', $old_id)"/>
    </xsl:attribute>
  </xsl:template>

  <!-- idrefs -->
  <!-- we want to migrate all @idrefs except platform's, we don't migrate requires/@idref here, it's handled elsewhere -->
  <xsl:template match="@idref[parent::xccdf_11:*][not(parent::xccdf_11:platform)][not(parent::xccdf_11:requires)]">
    <xsl:variable name="old_idref" select="."/>

    <xsl:attribute name="idref">
      <!-- Our goal here is to figure out which item the idref references and
           change its format accordingly -->

      <xsl:choose>
        <xsl:when test="//xccdf_11:Profile[@id = $old_idref]">
          <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_profile_', $old_idref)"/>
        </xsl:when>

        <xsl:when test="//xccdf_11:Rule[@id = $old_idref]">
          <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_rule_', $old_idref)"/>
        </xsl:when>

        <xsl:when test="//xccdf_11:Group[@id = $old_idref]">
          <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_group_', $old_idref)"/>
        </xsl:when>

        <xsl:when test="//xccdf_11:Value[@id = $old_idref]">
          <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_value_', $old_idref)"/>
        </xsl:when>
        
        <xsl:otherwise>
          <xsl:value-of select="concat('dangling reference to ', $old_idref, '!')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </xsl:template>
 
  <xsl:template name="migrate-requires">
    <!-- The string parameter represents text content of the requires attribute
         It needs to have ' ' appended for this template to work! -->

    <xsl:param name="string"/>
    <xsl:choose>
      <!-- requires contain space delimited references, we will strip them one by one -->
      <xsl:when test="$string=''">
        <!-- we are done -->
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="//xccdf_11:Group[@id = substring-before($string, ' ')]">
           <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_group_', substring-before($string, ' '))"/>
          </xsl:when>
          <xsl:when test="//xccdf_11:Rule[@id = substring-before($string, ' ')]">
            <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_rule_', substring-before($string, ' '))"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="concat('dangling reference to ', substring-before($string, ' '), '!')"/>
          </xsl:otherwise>
        </xsl:choose>
        
        <!-- we test whether the tail is empty to avoid inserting one extra space -->
        <xsl:variable name="tail" select="substring-after($string, ' ')"/>
        <xsl:if test="$tail!=''">
          <xsl:text> </xsl:text>
          <xsl:call-template name="migrate-requires">
            <xsl:with-param name="string" select="$tail"/>
          </xsl:call-template>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="@idref[parent::xccdf_11:requires]">
    <!-- We intentionally concat @idref and a space, this is required for the called template to work properly -->
    <xsl:variable name="old_idref" select="."/>
    <xsl:attribute name="idref"><xsl:call-template name="migrate-requires"><xsl:with-param name="string" select="concat($old_idref, ' ')"/></xsl:call-template></xsl:attribute>
  </xsl:template>

  <!-- value-ids -->
  <xsl:template match="@value-id[parent::xccdf_11:*]">
    <xsl:variable name="old_value_id" select="."/>

    <xsl:attribute name="value-id">
      <!-- Our goal here is to figure out which item the value-id references and
           change its format accordingly -->

      <xsl:choose>
        <xsl:when test="//xccdf_11:Value[@id = $old_value_id]">
          <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_value_', $old_value_id)"/>
        </xsl:when>

        <xsl:otherwise>
          <xsl:value-of select="concat('dangling reference to ', $old_value_id, '!')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </xsl:template>

  <!-- Group/@extends -->
  <!-- If present, update extends attribute -->
  <xsl:template match="@extends[parent::xccdf_11:Group]">
    <xsl:variable name="old_extends" select="."/>

    <xsl:attribute name="extends">
      <xsl:choose>
       <xsl:when test="//xccdf_11:Group[@id = $old_extends]">
          <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_group_', $old_extends)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat('dangling reference to ', $old_extends, '!')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </xsl:template>

  <!-- Rule/@extends -->
  <!-- If present, update extends attribute -->
  <xsl:template match="@extends[parent::xccdf_11:Rule]">
    <xsl:variable name="old_extends" select="."/>

    <xsl:attribute name="extends">
      <xsl:choose>
       <xsl:when test="//xccdf_11:Rule[@id = $old_extends]">
          <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_rule_', $old_extends)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat('dangling reference to ', $old_extends, '!')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </xsl:template>

  <!-- Value/@extends -->
  <!-- If present, update extends attribute -->
  <xsl:template match="@extends[parent::xccdf_11:Value]">
    <xsl:variable name="old_extends" select="."/>

    <xsl:attribute name="extends">
      <xsl:choose>
       <xsl:when test="//xccdf_11:Value[@id = $old_extends]">
          <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_value_', $old_extends)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat('dangling reference to ', $old_extends, '!')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </xsl:template>

  <!-- Profile/@extends -->
  <!-- If present, update extends attribute -->
  <xsl:template match="@extends[parent::xccdf_11:Profile]">
    <xsl:variable name="old_extends" select="."/>

    <xsl:attribute name="extends">
      <xsl:choose>
       <xsl:when test="//xccdf_11:Profile[@id = $old_extends]">
          <xsl:value-of select="concat('xccdf_', $reverse_DNS, '_profile_', $old_extends)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat('dangling reference to ', $old_extends, '!')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </xsl:template>

  <!-- add the @legacy attribute to <sub> elements -->
  <xsl:template match="xccdf_11:sub">
    <xsl:element name="{local-name()}" namespace="http://checklists.nist.gov/xccdf/1.2">
      <xsl:apply-templates select="@*"/>
      <xsl:apply-templates/>
      <xsl:attribute name="use">legacy</xsl:attribute>
    </xsl:element>
  </xsl:template>
</xsl:stylesheet>

