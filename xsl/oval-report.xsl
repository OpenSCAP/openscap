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
    exclude-result-prefixes="xsl cdf db s exsl ovalres ovalsys ovalunixsc ovalindsc"
    >

<xsl:key name='oval-definition' match='ovalres:definition'    use='@definition_id' />
<xsl:key name='oval-test'       match='ovalres:test'          use='@test_id'       />
<xsl:key name='oval-items'      match='ovalsys:system_data/*' use='@id'            />

<xsl:key name='oval-testdef' match='*[starts-with(namespace-uri(), "http://oval.mitre.org/XMLSchema/oval-definitions") and contains(local-name(), "_test")]' use='@id' />

<xsl:template mode='brief' match='ovalres:oval_results'>
  <xsl:param name='definition-id' />
  <xsl:apply-templates select='key("oval-definition", $definition-id)' mode='brief'/>
</xsl:template>

<xsl:template mode='brief' match='ovalres:definition|ovalres:criteria|ovalres:criterion|ovalres:extend_definition'>
  <!-- expression "higher" in syntax tree is negated -->
  <xsl:param name='neg' select='false()'/>
  <!-- this expression is negated -->
  <xsl:variable name='neg1' select='@negate="TRUE" or @negate="true" or @negate="1"'/>
  <!-- negation inference form the above -->
  <xsl:variable name='cur-neg' select='($neg and not($neg1)) or (not($neg) and $neg1)'/>

  <!-- which result types to display -->
  <xsl:variable name='disp'>
    <xsl:text>:unknown:error:not evaluated:not applicable:</xsl:text>
    <xsl:if test='not($neg)'>false:</xsl:if>
    <xsl:if test='$neg'     >true:</xsl:if>
    <xsl:if test='@operator="XOR" or @operator="ONE"'>false:true:</xsl:if>
  </xsl:variable>

  <!-- is this relevant? -->
  <xsl:if test='contains($disp, concat(":", @result, ":"))'>

    <!-- if this atom references a test, display it -->
    <xsl:apply-templates select='key("oval-test", @test_ref)' mode='brief'>
      <!-- suggested test title (will be replaced by test ID if empty) -->
      <xsl:with-param name='title' select='key("oval-testdef", @test_ref)/@comment'/>
      <!-- negate results iif overall number of negations is odd -->
      <xsl:with-param name='neg' select='$cur-neg'/>
    </xsl:apply-templates>

    <!-- descend deeper into the logic formula -->
    <xsl:apply-templates mode='brief'>
      <xsl:with-param name='neg' select='$cur-neg'/>
    </xsl:apply-templates>

  </xsl:if>

</xsl:template>

<!-- OVAL items dump -->
<xsl:template mode='brief' match='ovalres:test'>
  <xsl:param name='title'/>
  <xsl:param name='neg' select='false()'/>

  <!-- existence status of items to be displayed -->
  <xsl:variable name='disp.status'>:<xsl:apply-templates select='@check_existence' mode='display-mapping'>
    <xsl:with-param name='neg' select='$neg' />
  </xsl:apply-templates>:</xsl:variable>

  <!-- result status of items to be displayed -->
  <xsl:variable name='disp.result'>:<xsl:apply-templates select='@check' mode='display-mapping'>
    <xsl:with-param name='neg' select='$neg' />
  </xsl:apply-templates>:</xsl:variable>

  <!-- items to be displayed -->
  <xsl:variable name='items' select='ovalres:tested_item[
                                         contains($disp.result, concat(":", @result, ":")) or
                                         contains($disp.status, concat(":", key("oval-items", @item_id)/@status, ":"))
                                     ]'/>
  
  <!-- if there are items to display, go ahead -->
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

<!--
  Define a mapping from @check or @check_existence attribute values
  to type of items to be displayed, i.e. to their existence or complience status.
  This is used to filter out items that do not cause the failure of a test.
  It also performs possible negation.
-->
<xsl:template mode='display-mapping' match='@*'>
  <!-- negation param -->
  <xsl:param name='neg' select='false()'/>

  <!-- simplified check representation -->
  <xsl:variable name='c1' select='substring-before(translate(concat(., "_"), " ALNYTNOE", "_alnytnoe"), "_")'/>

  <!-- negation -->
  <xsl:variable name='c'>
    <xsl:choose>
      <xsl:when test='not($neg)' ><xsl:value-of select='$c1'/></xsl:when> <!-- not negated -->
      <xsl:when test='$c1="all"' >none</xsl:when>
      <xsl:when test='$c1="none"'>at</xsl:when>   <!-- at = at least one -->
      <xsl:when test='$c1="any"' >any</xsl:when>
      <xsl:when test='$c1="at"'  >none</xsl:when>
      <xsl:when test='$c1="only"'>only</xsl:when> <!-- only = only one exists -->
      <xsl:otherwise><xsl:message>WARNING: unknown value of @<xsl:value-of select='name()'/></xsl:message></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <!-- which item types to display (by existence or complience status) -->
  <xsl:variable name='disp'>
    <!-- dispaly error items every time -->
    <xsl:text>error</xsl:text>
    <xsl:if test='not($c="any")'>
      <xsl:text>:not collected:not evaluated:not applicable</xsl:text>
      <xsl:choose>
        <xsl:when test='$c="only"'>:true:false::exists:does not exist</xsl:when>
        <xsl:when test='$c="at" or $c="all"'>:false:does not exist</xsl:when>
        <xsl:when test='$c="none"'>:true::exists</xsl:when>
        <xsl:otherwise><xsl:message>WARNING: unknown value of @<xsl:value-of select='name()'/></xsl:message></xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:variable>

  <!-- write the result out -->
  <xsl:value-of select='$disp'/>

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
