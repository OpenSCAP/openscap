<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
	            xmlns="http://docbook.org/ns/docbook"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
                xmlns:fo="http://www.w3.org/1999/XSL/Format" 
                xmlns:html="http://www.w3.org/1999/xhtml" 
                xmlns:exsl="http://exslt.org/common"
                exclude-result-prefixes="xsl fo html exsl">

<xsl:output method="xml" indent="yes"/>
<xsl:param name="filename"></xsl:param> 
<xsl:param name="prefix">db</xsl:param>
<xsl:param name="graphics_location">./</xsl:param>

<!-- wrap plaintext to <para> -->
<xsl:template name='h2db.para'>
  <xsl:param name='in'/>
  <xsl:variable name='delims' select='$in/html:p|$in/html:ul|$in/html:ol'/>
  <xsl:variable name='rest.r'>
    <xsl:choose>
      <xsl:when test='count($delims)!=0'><xsl:copy-of select='$delims[last()]/following-sibling::node()'/></xsl:when>
      <xsl:otherwise><xsl:copy-of select='$in/node()'/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name='start.r'>
    <xsl:if test='count($delims)!=0'><xsl:copy-of select='$delims[1]/preceding-sibling::node()'/></xsl:if>
  </xsl:variable>
  <xsl:variable name='rest' select='exsl:node-set($rest.r)'/>
  <xsl:variable name='start' select='exsl:node-set($start.r)'/>

  <xsl:apply-templates mode='h2db.para' select='$delims'/>
  <xsl:if test='normalize-space(string($rest))'><para><xsl:apply-templates mode='h2db' select='$rest'/></para></xsl:if>
</xsl:template>

<xsl:template mode='h2db.para' match='html:*'>
  <xsl:variable name='prev.delim' select='(preceding-sibling::html:p|preceding-sibling::html:ul|preceding-sibling::html:ol)[last()]'/>
  <xsl:variable name='chunk' select='preceding-sibling::node()[
           generate-id((preceding-sibling::html:p|preceding-sibling::html:ul|preceding-sibling::html:ol)[last()]) = generate-id($prev.delim)]'/>
  <xsl:if test='normalize-space(concat(string($chunk//text()), string($chunk))) != "" or count(*) != 0'><para><xsl:apply-templates mode='h2db' select='$chunk'/></para></xsl:if>
  <xsl:apply-templates select='.' mode='h2db'/>
</xsl:template>

<!-- Main block-level conversions -->
<xsl:template mode='h2db' match="html:html">
 <xsl:apply-templates mode='h2db' select="html:body"/>
</xsl:template>

<xsl:template match="/"><xsl:apply-templates mode='h2db' select="html:html"/></xsl:template>

<!-- This template converts each HTML file encountered into a DocBook 
     section.  For a title, it selects the first h1 element -->
<xsl:template mode='h2db' match="html:body">
 <section>
  <xsl:if test="$filename != ''">
   <xsl:attribute name="id">
    <xsl:value-of select="$prefix"/>
    <xsl:text>_</xsl:text>
    <xsl:value-of select="translate($filename,' ()','__')"/>
   </xsl:attribute>
  </xsl:if>
  <title>
   <xsl:value-of select=".//html:h1[1]
                         |.//html:h2[1]
                         |.//html:h3[1]"/>
  </title>
  <xsl:apply-templates mode='h2db' select="*"/>
 </section>
</xsl:template>

<!-- This template matches on all HTML header items and makes them into 
     bridgeheads. It attempts to assign an ID to each bridgehead by looking 
     for a named anchor as a child of the header or as the immediate preceding
     or following sibling -->
<xsl:template mode='h2db' match="html:h1
              |html:h2
              |html:h3
              |html:h4
              |html:h5
              |html:h6">
 <bridgehead>
  <xsl:choose>
   <xsl:when test="count(html:a/@name)">
    <xsl:attribute name="id">
     <xsl:value-of select="html:a/@name"/>
    </xsl:attribute>
   </xsl:when>
   <xsl:when test="preceding-sibling::* = preceding-sibling::html:a[@name != '']">
    <xsl:attribute name="id">
    <xsl:value-of select="concat($prefix,preceding-sibling::html:a[1]/@name)"/>
    </xsl:attribute>
   </xsl:when>
   <xsl:when test="following-sibling::* = following-sibling::html:a[@name != '']">
    <xsl:attribute name="id">
    <xsl:value-of select="concat($prefix,following-sibling::html:a[1]/@name)"/>
    </xsl:attribute>
   </xsl:when>
  </xsl:choose>
  <xsl:apply-templates mode='h2db'/>
 </bridgehead>
</xsl:template>
        
<!-- These templates perform one-to-one conversions of HTML elements into
     DocBook elements -->
<xsl:template mode='h2db' match="html:p">
<!-- if the paragraph has no text (perhaps only a child <img>), don't 
     make it a para -->
 <xsl:choose>
  <xsl:when test="normalize-space(.) = ''">
   <xsl:apply-templates mode='h2db'/>
  </xsl:when>
  <xsl:otherwise>
 <para>
  <xsl:apply-templates mode='h2db'/>
 </para>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>
<xsl:template mode='h2db' match="html:pre">
 <programlisting>
  <xsl:apply-templates mode='h2db'/>
 </programlisting>
</xsl:template>

<!-- typography -->
<xsl:template mode='h2db' match='html:br'><phrase role='br'/></xsl:template>
<xsl:template mode='h2db' match='html:code[ancestor::html:pre]|html:span'><xsl:apply-templates mode='h2db'/></xsl:template>
<xsl:template mode='h2db' match='html:code'><code><xsl:apply-templates mode='h2db'/></code></xsl:template>
<xsl:template mode='h2db' match='html:sub'><subscript><xsl:apply-templates mode='h2db'/></subscript></xsl:template>
<xsl:template mode='h2db' match='html:sup'><superscript><xsl:apply-templates mode='h2db'/></superscript></xsl:template>
<xsl:template mode='h2db' match='html:em|html:i'><emphasis><xsl:apply-templates mode='h2db'/></emphasis></xsl:template>
<xsl:template mode='h2db' match='html:strong|html:b'><emphasis role='strong'><xsl:apply-templates mode='h2db'/></emphasis></xsl:template>
<xsl:template mode='h2db' match='text()'><xsl:value-of select='.'/></xsl:template>
<xsl:template mode='h2db' match='html:abbr'><abbrev><alt><xsl:value-of select='@title'/></alt><xsl:apply-templates mode='h2db'/></abbrev></xsl:template>
<xsl:template mode='h2db' match='html:abbr[@class="replace"]'><replaceable><alt><xsl:value-of select='@title'/></alt><xsl:apply-templates mode='h2db'/></replaceable></xsl:template>

<!-- Hyperlinks -->
<xsl:template mode='h2db' match="html:a[contains(@href,'http://')]" priority="1.5">
 <ulink>
  <xsl:attribute name="url">
   <xsl:value-of select="normalize-space(@href)"/>
  </xsl:attribute>
  <xsl:apply-templates mode='h2db'/>
 </ulink>
</xsl:template>
<xsl:template mode='h2db' match="html:a[contains(@href,'ftp://')]" priority="1.5">
 <ulink>
  <xsl:attribute name="url">
   <xsl:value-of select="normalize-space(@href)"/>
  </xsl:attribute>
  <xsl:apply-templates mode='h2db'/>
 </ulink>
</xsl:template>

<xsl:template mode='h2db' match="html:a[contains(@href,'#')]" priority="0.6">
 <xref>
  <xsl:attribute name="linkend">
   <xsl:call-template mode='h2db' name="make_id">
    <xsl:with-param name="string" select="substring-after(@href,'#')"/>
   </xsl:call-template>
  </xsl:attribute>
 </xref>
</xsl:template>
<xsl:template mode='h2db' match="html:a[@name != '']" priority="0.6">
 <anchor>
  <xsl:attribute name="id">
   <xsl:call-template mode='h2db' name="make_id">
    <xsl:with-param name="string" select="@name"/>
   </xsl:call-template>
  </xsl:attribute>
  <xsl:apply-templates mode='h2db'/>
 </anchor>
</xsl:template>

<xsl:template mode='h2db' match="html:a[@href != '']">
 <xref>
  <xsl:attribute name="linkend">
   <xsl:value-of select="$prefix"/>
   <xsl:text>_</xsl:text>
   <xsl:call-template mode='h2db' name="make_id">
    <xsl:with-param name="string" select="@href"/>
   </xsl:call-template>
  </xsl:attribute>
 </xref>
</xsl:template>

<!-- Need to come up with good template for converting filenames into ID's -->
<xsl:template mode='h2db' name="make_id">
 <xsl:param name="string" select="''"/>
 <xsl:variable name="fixedname">
  <xsl:call-template mode='h2db' name="get_filename">
   <xsl:with-param name="path" select="translate($string,' \()','_/_')"/>
  </xsl:call-template>
 </xsl:variable>
 <xsl:choose>
  <xsl:when test="contains($fixedname,'.htm')">
   <xsl:value-of select="substring-before($fixedname,'.htm')"/>
  </xsl:when>
  <xsl:otherwise>
   <xsl:value-of select="$fixedname"/>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template mode='h2db' name="string.subst">
 <xsl:param name="string" select="''"/>
 <xsl:param name="substitute" select="''"/>
 <xsl:param name="with" select="''"/>
 <xsl:choose>
  <xsl:when test="contains($string,$substitute)">
   <xsl:variable name="pre" select="substring-before($string,$substitute)"/>
   <xsl:variable name="post" select="substring-after($string,$substitute)"/>
   <xsl:call-template mode='h2db' name="string.subst">
    <xsl:with-param name="string" select="concat($pre,$with,$post)"/>
    <xsl:with-param name="substitute" select="$substitute"/>
    <xsl:with-param name="with" select="$with"/>
   </xsl:call-template>
  </xsl:when>
  <xsl:otherwise>
   <xsl:value-of select="$string"/>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<!-- Images -->
<!-- Images and image maps -->
<xsl:template mode='h2db' match="html:img">
 <xsl:variable name="tag_name">
  <xsl:choose>
   <xsl:when test="boolean(parent::html:p) and 
        boolean(normalize-space(parent::html:p/text()))">
    <xsl:text>inlinemediaobject</xsl:text>
   </xsl:when>
   <xsl:otherwise>mediaobject</xsl:otherwise>
  </xsl:choose>
 </xsl:variable>
 <xsl:element name="{$tag_name}">
  <imageobject>
   <xsl:call-template mode='h2db' name="process.image"/>
  </imageobject>
 </xsl:element>
</xsl:template>

<xsl:template mode='h2db' name="process.image">
 <imagedata>
<xsl:attribute name="fileref">
 <xsl:call-template mode='h2db' name="make_absolute">
  <xsl:with-param name="filename" select="@src"/>
 </xsl:call-template>
</xsl:attribute>
<xsl:if test="@height != ''">
 <xsl:attribute name="depth">
  <xsl:value-of select="@height"/>
 </xsl:attribute>
</xsl:if>
<xsl:if test="@width != ''">
 <xsl:attribute name="width">
  <xsl:value-of select="@width"/>
 </xsl:attribute>
</xsl:if>
 </imagedata>
</xsl:template>

<xsl:template mode='h2db' name="make_absolute">
 <xsl:param name="filename"/>
 <xsl:variable name="name_only">
  <xsl:call-template mode='h2db' name="get_filename">
   <xsl:with-param name="path" select="$filename"/>
  </xsl:call-template>
 </xsl:variable>
 <xsl:value-of select="$graphics_location"/><xsl:value-of select="$name_only"/>
</xsl:template>

<xsl:template mode='h2db' match="html:ul[count(*) = 0]">
 <xsl:message>Matched</xsl:message>
 <blockquote>
  <xsl:apply-templates mode='h2db'/>
 </blockquote>
</xsl:template>

<xsl:template mode='h2db' name="get_filename">
 <xsl:param name="path"/>
 <xsl:choose>
  <xsl:when test="contains($path,'/')">
   <xsl:call-template mode='h2db' name="get_filename">
    <xsl:with-param name="path" select="substring-after($path,'/')"/>
   </xsl:call-template>
  </xsl:when>
  <xsl:otherwise>
   <xsl:value-of select="$path"/>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<!-- LIST ELEMENTS -->
<xsl:template mode='h2db' match="html:ul">
 <itemizedlist>
  <xsl:apply-templates mode='h2db'/>
 </itemizedlist>
</xsl:template>

<xsl:template mode='h2db' match="html:ol">
 <orderedlist>
  <xsl:apply-templates mode='h2db'/>
 </orderedlist>
</xsl:template>
        
<!-- This template makes a DocBook variablelist out of an HTML definition list -->
<xsl:template mode='h2db' match="html:dl">
 <variablelist>
  <xsl:for-each select="html:dt">
   <varlistentry>
    <term>
     <xsl:apply-templates mode='h2db'/>
    </term>
    <listitem>
     <xsl:apply-templates mode='h2db' select="following-sibling::html:dd[1]"/>
    </listitem>
   </varlistentry>
  </xsl:for-each>
 </variablelist>
</xsl:template>

<xsl:template mode='h2db' match="html:dd">
 <xsl:choose>
  <xsl:when test="boolean(html:p)">
   <xsl:apply-templates mode='h2db'/>
  </xsl:when>
  <xsl:otherwise>
   <para>
    <xsl:apply-templates mode='h2db'/>
   </para>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template mode='h2db' match="html:li">
 <listitem>
  <xsl:choose>
   <xsl:when test="count(html:p) = 0">
    <para>
     <xsl:apply-templates mode='h2db'/>
    </para>
   </xsl:when>
   <xsl:otherwise>
    <xsl:apply-templates mode='h2db'/>
   </xsl:otherwise>
  </xsl:choose>
 </listitem>
</xsl:template>
        
<xsl:template mode='h2db' match="html:*">
 <xsl:message>No template for <xsl:value-of select="name()"/>(<xsl:value-of select='namespace-uri()'/>)<xsl:for-each select='ancestor::*'>&lt;=<xsl:value-of select="name()"/>[<xsl:value-of select="position()"/>]</xsl:for-each>
 </xsl:message>
 <xsl:apply-templates mode='h2db'/>
</xsl:template>

<xsl:template mode='h2db' match="@*">
 <xsl:message>No template for @<xsl:value-of select="name()"/>
 </xsl:message>
 <xsl:apply-templates mode='h2db'/>
</xsl:template>
        
<!-- inline formatting -->
<xsl:template mode='h2db' match="html:b">
 <emphasis role="bold">
  <xsl:apply-templates mode='h2db'/>
 </emphasis>
</xsl:template>
<xsl:template mode='h2db' match="html:i">
 <emphasis>
  <xsl:apply-templates mode='h2db'/>
 </emphasis>
</xsl:template>
<xsl:template mode='h2db' match="html:u">
 <citetitle>
  <xsl:apply-templates mode='h2db'/>
 </citetitle>
</xsl:template>
        
<!-- Ignored elements -->
<xsl:template mode='h2db' match="html:hr"/>
<xsl:template mode='h2db' match="html:h1[1]|html:h2[1]|html:h3[1]" priority="1"/>
<xsl:template mode='h2db' match="html:p[normalize-space(.) = '' and count(*) = 0]"/>
<xsl:template mode='h2db' match="text()">
 <xsl:choose>
  <xsl:when test="normalize-space(.) = ''"></xsl:when>
  <xsl:otherwise><xsl:copy/></xsl:otherwise>
 </xsl:choose>
</xsl:template>

<!-- Workbench Hacks -->
<xsl:template mode='h2db' match="html:div[contains(@style,'margin-left: 2em')]">
 <blockquote><para>
  <xsl:apply-templates mode='h2db'/></para>
 </blockquote>
</xsl:template>

<xsl:template mode='h2db' match="html:a[@href != '' 
                      and not(boolean(ancestor::html:p|ancestor::html:li))]" 
              priority="1">
 <para>
 <xref>
  <xsl:attribute name="linkend">
   <xsl:value-of select="$prefix"/>
   <xsl:text>_</xsl:text>
   <xsl:call-template mode='h2db' name="make_id">
    <xsl:with-param name="string" select="@href"/>
   </xsl:call-template>
  </xsl:attribute>
 </xref>
 </para>
</xsl:template>

<xsl:template mode='h2db' match="html:a[contains(@href,'#') 
                    and not(boolean(ancestor::html:p|ancestor::html:li))]" 
              priority="1.1">
 <para>
 <xref>
  <xsl:attribute name="linkend">
   <xsl:value-of select="$prefix"/>
   <xsl:text>_</xsl:text>
   <xsl:call-template mode='h2db' name="make_id">
    <xsl:with-param name="string" select="substring-after(@href,'#')"/>
   </xsl:call-template>
  </xsl:attribute>
 </xref>
 </para>
</xsl:template>

<!-- Table conversion -->
<xsl:template mode='h2db' match="html:table">
 <xsl:variable name="column_count">
  <xsl:call-template mode='h2db' name="count_columns">
   <xsl:with-param name="table" select="."/>
  </xsl:call-template>
 </xsl:variable>
 <informaltable>
  <tgroup>
   <xsl:attribute name="cols">
    <xsl:value-of select="$column_count"/>
   </xsl:attribute>
   <xsl:call-template mode='h2db' name="generate-colspecs">
    <xsl:with-param name="count" select="$column_count"/>
   </xsl:call-template>
   <thead>
    <xsl:apply-templates mode='h2db' select="html:tr[1]|html:thead/html:tr"/>
   </thead>
   <tbody>
    <xsl:apply-templates mode='h2db' select="html:tr[position() != 1]|html:tbody/html:tr"/>
   </tbody>
  </tgroup>
 </informaltable>
</xsl:template>

<xsl:template mode='h2db' name="generate-colspecs">
 <xsl:param name="count" select="0"/>
 <xsl:param name="number" select="1"/>
 <!--
 <xsl:choose>
  <xsl:when test="$count &lt; $number"/>
  <xsl:otherwise>
   <colspec>
    <xsl:attribute name="colnum">
     <xsl:value-of select="$number"/>
    </xsl:attribute>
    <xsl:attribute name="colname">
     <xsl:value-of select="concat('col',$number)"/>
    </xsl:attribute>
   </colspec>
   <xsl:call-template mode='h2db' name="generate-colspecs">
    <xsl:with-param name="count" select="$count"/>
    <xsl:with-param name="number" select="$number + 1"/>
   </xsl:call-template>
  </xsl:otherwise>
 </xsl:choose>
 -->
</xsl:template>

<xsl:template mode='h2db' match="html:tr">
 <row>
  <xsl:apply-templates mode='h2db'/>
 </row>
</xsl:template>

<xsl:template mode='h2db' match="html:th|html:td">
 <xsl:variable name="position" select="count(preceding-sibling::*) + 1"/>
 <entry>
  <xsl:if test="@colspan &gt; 1">
   <xsl:attribute name="namest">
    <xsl:value-of select="concat('col',$position)"/>
   </xsl:attribute>
   <xsl:attribute name="nameend">
    <xsl:value-of select="concat('col',$position + number(@colspan) - 1)"/>
   </xsl:attribute>
  </xsl:if>
  <xsl:if test="@rowspan &gt; 1">
   <xsl:attribute name="morerows">
    <xsl:value-of select="number(@rowspan) - 1"/>
   </xsl:attribute>
  </xsl:if>
  <xsl:apply-templates mode='h2db'/>
 </entry>
</xsl:template>

<xsl:template mode='h2db' match="html:td_null">
 <xsl:apply-templates mode='h2db'/>
</xsl:template>

<xsl:template mode='h2db' name="count_columns">
 <xsl:param name="table" select="."/>
 <xsl:param name="row" select="$table//html:tr[1]"/>
 <xsl:param name="max" select="0"/>
 <xsl:choose> 
  <xsl:when test="local-name($table) != 'table'">
   <xsl:message>Attempting to count columns on a non-table element</xsl:message>
  </xsl:when>
  <xsl:when test="local-name($row) != 'tr'">
   <xsl:message>Row parameter is not a valid row</xsl:message>
  </xsl:when>
  <xsl:otherwise>
   <!-- Count cells in the current row -->
   <xsl:variable name="current_count">
    <xsl:call-template mode='h2db' name="count_cells">
     <xsl:with-param name="cell" select="$row/html:td[1]|$row/html:th[1]"/>
    </xsl:call-template>
   </xsl:variable>
   <!-- Check for the maximum value of $current_count and $max -->
   <xsl:variable name="new_max">
    <xsl:choose>
     <xsl:when test="$current_count &gt; $max">
      <xsl:value-of select="number($current_count)"/>
     </xsl:when>
     <xsl:otherwise>
      <xsl:value-of select="number($max)"/>
     </xsl:otherwise>
    </xsl:choose>
   </xsl:variable>
   <!-- If this is the last row, return $max, otherwise continue -->
   <xsl:choose>
    <xsl:when test="count($row/following-sibling::html:tr) = 0">
     <xsl:value-of select="$new_max"/>
    </xsl:when>
    <xsl:otherwise>
     <xsl:call-template mode='h2db' name="count_columns">
      <xsl:with-param name="table" select="$table"/>
      <xsl:with-param name="row" select="$row/following-sibling::html:tr"/>
      <xsl:with-param name="max" select="$new_max"/>
     </xsl:call-template>
    </xsl:otherwise>
   </xsl:choose>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template mode='h2db' name="count_cells">
 <xsl:param name="cell"/>
 <xsl:param name="count" select="0"/>
 <xsl:variable name="new_count">
  <xsl:choose>
   <xsl:when test="$cell/@colspan &gt; 1">
    <xsl:value-of select="number($cell/@colspan) + number($count)"/>
   </xsl:when>
   <xsl:otherwise>
    <xsl:value-of select="number('1') + number($count)"/>
   </xsl:otherwise>
  </xsl:choose>
 </xsl:variable>
 <xsl:choose>
  <xsl:when test="count($cell/following-sibling::*) &gt; 0">
   <xsl:call-template mode='h2db' name="count_cells">
    <xsl:with-param name="cell"
                    select="$cell/following-sibling::*[1]"/>
    <xsl:with-param name="count" select="$new_count"/>
   </xsl:call-template>
  </xsl:when>
  <xsl:otherwise>
   <xsl:value-of select="$new_count"/>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

</xsl:stylesheet>
