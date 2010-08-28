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
<xsl:template match="cdf:sub[@idref]" mode="text">
  <xsl:variable name="subid" select="./@idref"/><i>
  <a href="#{@idref}">
    <xsl:value-of select="//cdf:Value[@id = $subid]/cdf:value/text()"/>
  </a></i>
</xsl:template>
<xsl:template match="htm:*" mode="text">
  <xsl:element name="{local-name()}">
    <xsl:for-each select="./@*">
      <xsl:copy-of select="."/>
    </xsl:for-each>
    <xsl:apply-templates select="./text() | ./*" mode="text"/>
  </xsl:element>
</xsl:template>

<!-- Print warning when processing an unresolved XCCDF, usage:
  <xsl:call-template name='warn-unresolved'/>
-->
<xsl:template name='warn-unresolved'>
  <xsl:if test='ancestor::cdf:Benchmark[not(number(@resolved)=1)]'>
    <xsl:message>WARNING: Processing an unresolved XCCDF document. This may have unexpected results.</xsl:message>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
