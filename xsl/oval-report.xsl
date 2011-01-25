<?xml version="1.0" encoding="UTF-8" ?>
<!--
Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
    xmlns:exsl="http://exslt.org/common"
	xmlns:db="http://docbook.org/ns/docbook"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    xmlns:svg="http://www.w3.org/2000/svg"
	xmlns="http://docbook.org/ns/docbook"
    xmlns:s="http://open-scap.org/"
    xmlns:ovalres="http://oval.mitre.org/XMLSchema/oval-results-5"
    xmlns:ovalsys="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5"
    xmlns:ovalunixsc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix"
	xmlns:ovalindsc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent"
    exclude-result-prefixes="xsl cdf db s exsl"
    >

<xsl:key name='oval-definition' match='ovalres:definition'    use='@definition_id' />
<xsl:key name='oval-test'       match='ovalres:test'          use='@test_id'       />
<xsl:key name='oval-items'      match='ovalsys:system_data/*' use='@id'            />

<xsl:key name='oval-testdef' match='*[starts-with(namespace-uri(), "http://oval.mitre.org/XMLSchema/oval-definitions") and contains(local-name(), "_test")]' use='@id' />

<xsl:template mode='brief' match='ovalres:oval_results'>
  <xsl:param name='definition-id' />
  <xsl:apply-templates select='key("oval-definition", $definition-id)' mode='brief'/>
</xsl:template>

<xsl:template mode='brief' match='ovalres:definition'>
  <!--
    Take just critera satisfying all of the following conditions:
       1) do not nest more than one level deep
       2) are not negated
       3) Its children are logically combined using AND or OR operator
       4) evaluated to "false"
    From the criteria select criterions that:
       1) are not negated
       2) evaluated to "false"
  -->
  <xsl:for-each select='ovalres:criteria[
                            (not(@negate) or @negate="false" or @negate="0") and
                            (not(@operator) or @operator="AND" or @operator="OR") and
                            (@result="false")
                        ]/ovalres:criterion[
                            (not(@negate) or @negate="false" or @negate="0") and
                            (@result="false")
                        ]'>
    <xsl:apply-templates select='key("oval-test", @test_ref)' mode='brief'>
      <xsl:with-param name='title' select='key("oval-testdef", @test_ref)/@comment'/>
    </xsl:apply-templates>
  </xsl:for-each>
</xsl:template>

<!-- OVAL items dump -->
<xsl:template mode='brief' match='ovalres:test'>
  <xsl:param name='title'/>
  <xsl:variable name='items' select='ovalres:tested_item[@result="true"]'/>

  <xsl:if test='$items'>

    <table role='oval-results'>
      <title>
	    <xsl:choose>
		  <xsl:when test='$title'><xsl:value-of select='$title'/></xsl:when>
	      <xsl:otherwise>OVAL test <xsl:value-of select='@test_id'/></xsl:otherwise>
		</xsl:choose>
	  </title>

      <!-- table head (possibly item-type-specific) -->
      <thead>
        <xsl:apply-templates mode='item-head' select='key("oval-items", $items[1]/@item_id)'/>
      </thead>

      <!-- table body (possibly item-type-specific) -->
      <tbody>
        <xsl:for-each select='$items'>
          <xsl:for-each select='key("oval-items", @item_id)'>
            <xsl:apply-templates select='.' mode='item-body'/>
          </xsl:for-each>
        </xsl:for-each>
      </tbody>
    </table>

  </xsl:if>
</xsl:template>

<!-- unmatched node visualisation (i.e. not displayed) -->

<xsl:template mode='item-head' match='node()' priority='-5'/>
<xsl:template mode='item-body' match='node()' priority='-5'/>

<!-- generic item visualisation -->

<xsl:template mode='item-head' match='*'>
  <row><xsl:for-each select='*'><entry><xsl:value-of select='translate(local-name(), "_", " ")'/></entry></xsl:for-each></row>
</xsl:template>

<xsl:template mode='item-body' match='*'>
  <row>
    <xsl:for-each select='*'>
      <entry>
        <xsl:if test='@datatype="int" or @datatype="boolean"'><xsl:attribute name='role'>num</xsl:attribute></xsl:if>
        <xsl:value-of select='.'/>
      </entry>
    </xsl:for-each>
  </row>
</xsl:template>

<!-- UNIX file item visualisation -->

<xsl:template mode='item-head' match='ovalunixsc:file_item'>
  <row><entry>path</entry><entry>type</entry><entry>UID</entry><entry>GID</entry><entry>size</entry><entry>permissions</entry></row>
</xsl:template>

<xsl:template mode='item-body' match='ovalunixsc:file_item'>
  <xsl:variable name='path' select='concat(ovalunixsc:path, "/", ovalunixsc:filename)'/>
  <row>
    <entry><xsl:value-of select='$path'/></entry>
    <entry><xsl:value-of select='ovalunixsc:type'/></entry>
    <entry role='num'><xsl:value-of select='ovalunixsc:user_id'/></entry>
    <entry role='num'><xsl:value-of select='ovalunixsc:group_id'/></entry>
    <entry role='num'><xsl:value-of select='ovalunixsc:size'/></entry>
    <!-- permissions output -->
    <entry role='num'>
      <code>
      <xsl:apply-templates mode='permission' select='ovalunixsc:uread'/>
      <xsl:apply-templates mode='permission' select='ovalunixsc:uwrite'/>
      <xsl:choose>
        <xsl:when test='string(ovalunixsc:suid)="true"'>s</xsl:when>
        <xsl:otherwise><xsl:apply-templates mode='permission' select='ovalunixsc:uexec'/></xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates mode='permission' select='ovalunixsc:gread'/>
      <xsl:apply-templates mode='permission' select='ovalunixsc:gwrite'/>
      <xsl:choose>
        <xsl:when test='string(ovalunixsc:sgid)="true"'>s</xsl:when>
        <xsl:otherwise><xsl:apply-templates mode='permission' select='ovalunixsc:gexec'/></xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates mode='permission' select='ovalunixsc:oread'/>
      <xsl:apply-templates mode='permission' select='ovalunixsc:owrite'/>
      <xsl:apply-templates mode='permission' select='ovalunixsc:oexec'/>
      <xsl:choose>
        <xsl:when test='string(ovalunixsc:sticky)="true"'>t</xsl:when>
        <xsl:otherwise><xsl:text>&#160;</xsl:text></xsl:otherwise>
      </xsl:choose>
      </code>
    </entry>
  </row>
</xsl:template>

<xsl:template mode='permission' match='*'>
    <xsl:choose>
      <xsl:when test='string(.)="true"'><xsl:value-of select='translate(substring(local-name(),2,1), "e", "x")'/></xsl:when>
      <xsl:otherwise>-</xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- textfilecontent visualisation -->

<xsl:template mode='item-head' match='ovalindsc:textfilecontent_item'>
  <row><entry>path</entry><entry>content</entry></row>
</xsl:template>

<xsl:template mode='item-body' match='ovalindsc:textfilecontent_item'>
  <xsl:variable name='path' select='concat(ovalindsc:path, "/", ovalindsc:filename)'/>
  <row><entry><xsl:value-of select='$path'/></entry><entry><xsl:value-of select='ovalindsc:text'/></entry></row>
</xsl:template>

</xsl:stylesheet>
