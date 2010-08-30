<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:htm="http://www.w3.org/1999/xhtml"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
	>

<xsl:include href="xccdf-common.xsl" />

<xsl:param name='keep-profiles'/>
<xsl:param name='profile'/>

<!-- identity transform -->
<xsl:template mode='profile' match='node()|@*'>
  <xsl:param name='p'/>
  <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"><xsl:with-param name='p' select='$p'/></xsl:apply-templates></xsl:copy>
</xsl:template>

<!-- omit profiles if needed -->
<xsl:template mode='profile' match='cdf:Profile'>
  <xsl:param name='p'/>
  <xsl:if test='$keep-profiles'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"><xsl:with-param name='p' select='$p'/></xsl:apply-templates></xsl:copy>
  </xsl:if>
</xsl:template>

<!-- adjust 'selected' attribute and refine-rules -->
<xsl:template mode='profile' match='cdf:Group|cdf:Rule'>
  <xsl:param name='p'/>
  <xsl:variable name='id' select='@id'/>
  <xsl:variable name='cid' select='@cluster-id'/>
  <xsl:variable name='sel' select='$p/cdf:select[@idref=$id or @idref=$cid][1]'/>
  <xsl:variable name='rr' select='$p/cdf:refine-rule[@idref=$id or @idref=$cid][1]'/>
  <xsl:copy>
    <xsl:copy-of select='@*'/>
    <xsl:if test='self::cdf:Rule and $rr'><xsl:copy-of select='$rr/@role|$rr/@severity|$rr/@weight'/></xsl:if>
    <xsl:if test='$sel'><xsl:attribute name='selected'><xsl:value-of select='$sel/@selected'/></xsl:attribute></xsl:if>
    <xsl:apply-templates mode='profile' select="node()"><xsl:with-param name='p' select='$p'/></xsl:apply-templates>
  </xsl:copy>
</xsl:template>

<!-- apply refine-rule selectors -->
<xsl:template mode='profile' match='cdf:check'>
  <xsl:param name='p'/>
  <xsl:variable name='rule' select='ancestor::cdf:Rule[1]'/>
  <xsl:variable name='rr' select='$p/cdf:refine-rule[@idref=$rule/@id or @idref=$rule/@cluster-id][1]'/>
  <xsl:if test='@selector=$rr/@selector'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"><xsl:with-param name='p' select='$p'/></xsl:apply-templates></xsl:copy>
  </xsl:if>
</xsl:template>

<!-- implement refine-value -->
<xsl:template mode='profile' match='cdf:Value'>
  <xsl:param name='p'/>
  <xsl:variable name='id' select='@id'/>
  <xsl:variable name='cid' select='@cluster-id'/>
  <xsl:variable name='rv' select='$p/cdf:refine-value[@idref=$id or @idref=$cid][1]'/>
  <xsl:variable name='sv' select='$p/cdf:set-value[@idref=$id or @idref=$cid][1]'/>
  <xsl:copy>
    <xsl:copy-of select='@*'/>
    <xsl:if test='$rv'><xsl:copy-of select='$rv/@operator'/></xsl:if>
    <xsl:apply-templates mode='profile' select="cdf:status|cdf:version|cdf:title|cdf:description|cdf:warning|cdf:question|cdf:reference">
      <xsl:with-param name='p' select='$p'/>
    </xsl:apply-templates>
    <xsl:choose>
      <xsl:when test='$sv'><cdf:value><xsl:value-of select='$sv'/></cdf:value></xsl:when>
      <xsl:otherwise><xsl:apply-templates mode='profile' select="cdf:value"><xsl:with-param name='p' select='$p'/></xsl:apply-templates></xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates mode='profile' select="cdf:value|cdf:default|cdf:match|cdf:lower-bound|cdf:upper-bound|cdf:choices|cdf:source">
      <xsl:with-param name='p' select='$p'/>
    </xsl:apply-templates>
  </xsl:copy>
</xsl:template>

<!-- refine-value selector -->
<xsl:template mode='profile' match='cdf:value|cdf:default|cdf:match|cdf:lower-bound|cdf:upper-bound|cdf:choices'>
  <xsl:param name='p'/>
  <xsl:variable name='val' select='ancestor::cdf:Value[1]'/>
  <xsl:variable name='rv' select='$p/cdf:refine-value[@idref=$val/@id]'/>
  <xsl:if test='@selector=$rv/@selector'>
    <xsl:copy><xsl:apply-templates mode='profile' select="node()|@*"><xsl:with-param name='p' select='$p'/></xsl:apply-templates></xsl:copy>
  </xsl:if>
</xsl:template>

<!-- erase @selector -->
<xsl:template mode='profile' match='@selector[not(ancestor::cdf:Profile)]'/>

<!-- top-level template -->
<xsl:template match='/cdf:Benchmark'>
  <xsl:apply-templates select='.' mode='apply-profile'/>
</xsl:template>

<xsl:template mode='apply-profile' match='cdf:Benchmark'>
  <xsl:call-template name='warn-unresolved'/>
  <xsl:apply-templates select='.' mode='profile'>
    <xsl:with-param name='p' select='cdf:Profile[@id=$profile]'/>
  </xsl:apply-templates>
</xsl:template>

</xsl:stylesheet>

