<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:htm="http://www.w3.org/1999/xhtml"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
	>

<xsl:include href="oscap-common.xsl" />


<!-- templates in mode "text", for processing text with 
     markup and substitutions.
 -->

<xsl:template match="text()" mode="text">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="cdf:instance" mode="text">
  <xsl:variable name='ctx' select='@context'/>
  <xsl:choose>
    <xsl:when test='ancestor::cdf:rule-result'>
      <abbr title='context: {$ctx}'><xsl:value-of select='ancestor::cdf:rule-result/cdf:instance[@context=$ctx]'/></abbr>
    </xsl:when>
    <xsl:otherwise><abbr title='replace with actual {$ctx} context'><em><xsl:value-of select='$ctx'/></em></abbr></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="cdf:sub[@idref]" mode="text">
  <xsl:variable name="subid" select="./@idref"/>
  <xsl:variable name="plain" select="/cdf:Benchmark/cdf:plain-text[@id=$subid]"/>
  <xsl:variable name="val" select="//cdf:Value[@id=$subid]"/>
  <xsl:choose>
    <xsl:when test='$plain'><abbr title="text: {$subid}"><xsl:value-of select="$plain"/></abbr></xsl:when>
    <xsl:otherwise><abbr title="value: {$subid} ({$val/cdf:title})"><xsl:value-of select="$val/cdf:value/text()"/></abbr></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="htm:*" mode="text">
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates select="./text() | ./*" mode="text"/>
  </xsl:element>
</xsl:template>


<!-- Print warning when processing an unresolved XCCDF, usage:
  <xsl:call-template name='warn-unresolved'/>
-->
<xsl:template name='warn-unresolved'>
  <xsl:if test='ancestor-or-self::cdf:Benchmark[not(number(@resolved)=1)]'>
    <xsl:message>WARNING: Processing an unresolved XCCDF document. This may have unexpected results.</xsl:message>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
