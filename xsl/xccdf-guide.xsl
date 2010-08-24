<?xml version="1.0" encoding="UTF-8" ?>

<!--
  XSLT Stylesheet for XCCDF - xccdf2html-0.12.4
  This stylesheet transforms a resolved XCCDF document into a nice
  XHTML document, with table of contents, cross-links, and section
  numbers.  This stylesheet basically assumes that the document is
  compliant with the XCCDF schema, so validation is probably
  advisable.  Note that this stylesheet completely ignores all
  TestResult elements: they do not appear in the generated
  XHTML document.  

  The XCCDF document MUST be resolved before applying this
  stylesheet.  This stylesheet cannot deal with extension/inheritance 
  at all.

  This stylesheet has been tested and found to work correctly with
  the following XSLT processors: 
      - Java 1.5 (Apache Xalan)
      - MSIE 6.0 (MSXML 3?)
      - Mozilla 1.7 (Transformiix)
      - Firebird 1.0 (Transformiix)

 This stylesheet is known to fail with the following XSLT
 implementations:
      - MSIE 5.0 (MSXML 2?)

 Known issues:
      - This stylesheet does not handle Dublin Core metadata
	in references nor for the document itself.
      - This stylesheet does not handle locale/language issues 
        at all.  It simply treats all item properties alike.
      - The CSS structure is poor.
      - Handling of tailoring Values that appear inside hidden
        Groups is (arguably) faulty.
      - This stylesheet uses the // shortcut a lot, which is
        quite inefficient and inelegant, but appears to be
	unavoidable.
      - XCCDF 0.12.4 employs Dave Waltermire's platform
        specification schema 0.2.3.  This stylesheet has
        a modest amount of code specifically for platform
	text, but it could use more.

  Author: Neal Ziring (nziring@thecouch.ncsc.mil)
  Version: 0.12.4 (for XCCDF schema version 1.0rc4 - 0.12.4)
  Date: 13 Nov 04

 -->

<!-- 
 THIS SOFTWARE WAS CREATED BY THE U.S. GOVERNMENT.

 SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
 EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 IN NO EVENT SHALL THE NATIONAL SECURITY AGENCY OR ANY AGENT OR
 REPRESENTATIVE THEREOF BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA OR PROFITS; OR BUSINESS INTERRUPTION), HOWEVER CAUSED, UNDER ANY
 THEORY OF LIABILITY, ARISING IN ANY WAY OUT OF THE USE OF OR INABILITY
 TO MAKE USE OF THIS SOFTWARE.
-->


<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:htm="http://www.w3.org/1999/xhtml"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.1"
	xmlns:cdfp="http://www.cisecurity.org/xccdf/platform/0.2.3">


<!-- Define variables about this stylesheet -->
<xsl:variable name="stylesheet-name" select="'XCCDF2XHTML Stylesheet'"/>
<xsl:variable name="stylesheet-author" select="'Neal Ziring'"/>
<xsl:variable name="stylesheet-author-contact" select="'nziring@thecouch.ncsc.mil'"/>
<xsl:variable name="stylesheet-version" select="'0.12.4'"/>
<xsl:variable name="stylesheet-version-date" select="'2004-11-13'"/>

<!-- Set output style: XHTML using xml output method. -->
<xsl:output method="xml" encoding="UTF-8" indent="yes"/>

<!-- Set up an id key to match on against all Items -->
<xsl:key name="items" match="cdf:Group | cdf:Rule | cdf:Value" 
         use="@id"/>

<!-- Set up an id key to match on Item clusters: Rule and Group, Value -->
<xsl:key name="clusters-rg" match="cdf:Group | cdf:Rule" 
         use="@cluster-id"/>
<xsl:key name="clusters-v" match="cdf:Value" 
         use="@cluster-id"/>

<!-- Set up an id key to match on all Profiles -->
<xsl:key name="profiles" match="cdf:Profile" 
         use="@id"/>

<!-- Set up an id key to match on all platform-definition elements -->
<xsl:key name="platforms" match="cdfp:platform-definition" use="@id"/>

<!-- TODO: Set desired output language as an XSLT global variable -->

<!-- TEMPLATE for cdf:Benchmark
  -  This template takes care of the top-level structure of the
  -  generated XHTML document.  It handles the Benchmark element
  -  and all the content of the benchmark.
  -->
<xsl:template match="/cdf:Benchmark">

  <!-- First issue a warning if the Benchmark is not marked resolved. -->
  <xsl:if test="not(@resolved)">
     <xsl:message>
        Warning: benchmark <xsl:value-of select="@id"/> not resolved, formatted 
        output will be incomplete or corrupted.
     </xsl:message>
  </xsl:if>

  <!-- Define variables for section numbers. -->
  <xsl:variable name="introSecNum" select="1"/>
  <xsl:variable name="valSecNum" select="2"/>
  <xsl:variable name="ruleSecNum"
	        select="2 + number(count(//cdf:Value[not(number(@hidden)+number(@abstract))])!=0)"/>
  <xsl:variable name="profSecNum"
                select="2 + number(count(./cdf:Rule[not(number(@hidden)+number(@abstract))] | ./cdf:Group[not(number(@hidden)+number(@abstract))])!=0) + number(count(//cdf:Value[not(number(@hidden)+number(@abstract))])!=0)"/>
  <xsl:variable name="concSecNum" 
                select="2 + number(count(./cdf:Rule[not(number(@hidden)+number(@abstract))] | ./cdf:Group[not(number(@hidden)+number(@abstract))])!=0) + number(count(//cdf:Value[not(number(@hidden)+number(@abstract))])!=0) + number(count(./cdf:Profile)!=0)"/>
  <xsl:variable name="refSecNum" 
                select="2 + number(count(./cdf:Rule[not(number(@hidden)+number(@abstract))] | ./cdf:Group[not(number(@hidden)+number(@abstract))])!=0) + number(count(//cdf:Value[not(number(@hidden)+number(@abstract))])!=0) + number(count(./cdf:Profile)!=0) + number(count(./cdf:rear-matter)!=0)"/>

<!-- Begin the HTML/XHTML body -->
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>XCCDF Benchmark: <xsl:value-of select="./cdf:title/text()"/></title>
  <meta name="identifier" content="{@id}"/>
  <meta name="generator"  
   content="{$stylesheet-name} version {$stylesheet-version} by {$stylesheet-author}"/>

  <!-- Embed a cascading stylesheet; should we depend on one in a separate
       file instead?
  -->
  <style type="text/css">
      *	   { background-color: #FFFFCC; }
      body { margin-left: 8%; margin-right: 8%; foreground: black; }
      h1   { margin-left: -6%; font-size: 200%;  margin-bottom: 2em;
             font-family: verdana, arial, helvetica, sans-serif;  }
      h2   { margin-left: -2%; font-size: 150%; 
             border-bottom: solid 1px gray; margin-bottom: 1.0em; 
             margin-top: 2em; margin-bottom: 0.75em;
             font-family: verdana, arial, helvetica, sans-serif; }
      h3   { margin-left: 6%; font-size: 110%; margin-bottom: 0.25em; 
             font-family: verdana, arial, helvetica, sans-serif; }
      h4   { margin-left: 10%; font-size: 100%; margin-bottom: 0.75em; 
             font-family: verdana, arial, helvetica, sans-serif; }
      h5,h6 { margin-left: 10%; font-size: 90%; margin-bottom: 0.5em;  
              font-family: verdana, arial, helvetica, sans-serif; }
      p    { margin-bottom: 0.2em; margin-top: 0.2em; }
      pre  { margin-bottom: 0.5em; margin-top: 0.25em; margin-left: 3%;
             font-family: monospace; font-size: 90%; }
      ul   { margin-bottom: 0.5em; margin-top: 0.25em; }
      td   { vertical-align: top; }

      *.simpleText   { margin-left: 10%; }
      *.propertyText { margin-left: 10%; margin-top: 0.2em; margin-bottom: 0.2em }
      *.toc	     { background: #CCCCCC; }
      *.toc2	     { background: #CCCCCC; }
      div	     { margin-top: 1em; margin-bottom: 1em; }
      div.legal      { margin-left: 10%; font-family: monospace; font-size: 90%; text-transform: uppercase; }
      
      div.toc	     { margin-left: 6%; margin-bottom: 4em;
                       padding-bottom: 0.75em; padding-top: 1em; 
                       padding-left: 2em; padding-right: 2em; 
                     }
      h2.toc	     { border-bottom: none; margin-left: 0%; margin-top: 0em; }
      p.toc          { margin-left: 2em; margin-bottom: 0.2em; margin-top: 0.5em; }
      p.toc2         { margin-left: 5em; margin-bottom: 0.1em; margin-top: 0.1em; }
      ul.smallList   { margin-bottom: 0.1em; margin-top: 0.1em; font-size: 85%; }
      table.propertyTable { margin-left: 14%; width: 90%; margin-top: 0.5em; margin-bottom: 0.25em; }
      th.propertyTableHead { font-size: 80%; background-color: #CCCCCC; }

  </style>
</head>
<body bgcolor="#FFFFCC">
  <xsl:comment>Benchmark id = <xsl:value-of select="./@id"/></xsl:comment>
  <xsl:comment>
     This XHTML output file
     generated using the 
     <xsl:value-of select="system-property('xsl:vendor')"/>
     XSLT processor.
  </xsl:comment>

  <!-- BEGINNING OF BODY -->
  <h1><xsl:value-of select="./cdf:title"/></h1>
  <xsl:if test="./cdf:status | ./cdf:version | ./cdf:platform">
    <div class="simpleText">
      <p>Status: <b><xsl:value-of select="./cdf:status/text()"/></b>
	  <xsl:if test="./cdf:status/@date">
	     (as of <xsl:value-of select="./cdf:status/@date"/>)
	  </xsl:if>
      </p>
      <xsl:if test="./cdf:version">
	<p>Version: <xsl:value-of select="./cdf:version/text()"/></p>
      </xsl:if>
      <xsl:if test="./cdf:platform">
	<p>Applies to:<ul>
	  <xsl:apply-templates select="./cdf:platform" mode="list"/>
	</ul></p>
      </xsl:if>
    </div>
  </xsl:if>

  <!-- Build the Table of Contents -->
  <div class="toc">
     <h2 class="toc">Contents</h2>
     <p class="toc">
         <xsl:value-of select="$introSecNum"/>.
         <a class="toc" href="#section---intro">Introduction</a>
     </p>

     <!-- values TOC -->
     <xsl:if test=".//cdf:Value">
       <p class="toc">
         <xsl:value-of select="$valSecNum"/>. 
         <a class="toc" href="#section---values">Tailoring Values</a>
       </p>
       <xsl:apply-templates select=".//cdf:Value[not(number(@hidden)+number(@abstract))]" mode="toc">
	 <xsl:sort select="./cdf:title/text()" data-type="text" order="ascending"/>
	 <xsl:with-param name="section-prefix" select="concat($valSecNum,'.')"/>
       </xsl:apply-templates>
     </xsl:if>

     <!-- rules and groups TOC -->
     <xsl:if test="./cdf:Group[not(number(@hidden)+number(@abstract))] | ./cdf:Rule[not(number(@hidden)+number(@abstract))]">
        <p class="toc">
          <xsl:value-of select="$ruleSecNum"/>. 
          <a class="toc" href="#section---rules">Rules</a>
        </p>
	<xsl:apply-templates mode="toc"
         select="./cdf:Group[not(number(@hidden)+number(@abstract))] | ./cdf:Rule[not(number(@hidden)+number(@abstract))]">
	  <xsl:with-param name="section-prefix" select="concat($ruleSecNum,'.')"/>
	</xsl:apply-templates>
     </xsl:if>
	 
     <!-- Profiles TOC -->
     <xsl:if test="./cdf:Profile">
       <p class="toc">
         <xsl:value-of select="$profSecNum"/>. 
         <a class="toc" href="#section---profiles">Profiles</a>
       </p>
       <xsl:apply-templates select="./cdf:Profile" mode="toc">
         <xsl:sort select="./cdf:title/text()" data-type="text" order="ascending"/>
	 <xsl:with-param name="section-prefix" select="concat($profSecNum,'.')"/>
       </xsl:apply-templates>
     </xsl:if>

     <!-- rear-matter TOC -->
     <xsl:if test="./cdf:rear-matter">
       <p class="toc">
         <xsl:value-of select="$concSecNum"/>.
         <a class="toc" href="#section---conc">Conclusions</a>
       </p>
     </xsl:if>

     <!-- references TOC -->
     <xsl:if test="./cdf:reference">
       <p class="toc">
         <xsl:value-of select="$refSecNum"/>. <a class="toc" href="#section---references">References</a>
       </p>
     </xsl:if>
  </div>

  <!-- Begin the main page content in the HTML body -->

  <!-- 1. Build the introduction -->
  <h2><a name="section---intro"></a>1. Introduction</h2>
  <xsl:if test="./cdf:front-matter">
     <xsl:for-each select="./cdf:front-matter">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>
  <xsl:if test="./cdf:description">
     <h3>Description</h3>
     <xsl:for-each select="./cdf:description">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>
  <xsl:if test="./cdf:notice">
      <xsl:for-each select="./cdf:notice">
        <h3>Legal Notice</h3>       
          <div class="legal"><p><xsl:value-of select="text()"/></p></div>
      </xsl:for-each>
  </xsl:if>
   
  <!-- 2. Build the tailoring values section (Values) -->
  <xsl:if test=".//cdf:Value[not(number(@hidden)+number(@abstract))]">
     <h2><a name="section---values"></a>
           <xsl:value-of select="$valSecNum"/>. Tailoring Values
     </h2>
     <xsl:apply-templates select=".//cdf:Value[not(number(@hidden)+number(@abstract))]" mode="body">
	<xsl:sort select="./cdf:title/text()" data-type="text" order="ascending"/>
	<xsl:with-param name="section-prefix" select="concat($valSecNum,'.')"/>
     </xsl:apply-templates>
  </xsl:if>

  <!-- 3. Build the rules section (rules and groups) -->
  <xsl:if test="./cdf:Group[not(number(@hidden)+number(@abstract))] | ./cdf:Rule[not(number(@hidden)+number(@abstract))]">
     <h2><a name="section---rules"></a>
         <xsl:value-of select="$ruleSecNum"/>. Rules
     </h2>
     <xsl:apply-templates select="./cdf:Group[not(number(@hidden)+number(@abstract))] | ./cdf:Rule[not(number(@hidden)+number(@abstract))]" mode="body">
	  <xsl:with-param name="section-prefix" select="concat($ruleSecNum,'.')"/>
     </xsl:apply-templates>
  </xsl:if>

  <!-- 4. Build the tailored profiles section (Profiles) -->
  <xsl:if test="./cdf:Profile">
     <h2><a name="section---profiles"></a><xsl:value-of select="$profSecNum"/>. Profiles</h2>
     <xsl:apply-templates select=".//cdf:Profile" mode="body">
	<xsl:sort select="./cdf:title/text()" data-type="text" order="ascending"/>
	<xsl:with-param name="section-prefix" select="concat($profSecNum,'.')"/>
     </xsl:apply-templates>
  </xsl:if>

  <!-- 5. Build the conclusions section using Benchmark/rear-matter -->
  <xsl:if test="./cdf:rear-matter">
     <h2><a name="section---conc"></a>
         <xsl:value-of select="$concSecNum"/>. Conclusions
     </h2>
     <xsl:for-each select="./cdf:rear-matter">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <!-- 6. Build the references section using Benchmark/reference -->
  <xsl:if test="./cdf:reference">
     <h2><a name="section---references"></a>
        <xsl:value-of select="$refSecNum"/>. References
     </h2>
     <ol xmlns="http://www.w3.org/1999/xhtml" class="propertyText">
	<xsl:for-each select="./cdf:reference">
	  <li><xsl:value-of select="text()"/>
	    <xsl:if test="@href">
	      [<a href="{@href}">link</a>]
            </xsl:if>
	  </li>
	</xsl:for-each>
     </ol>
  </xsl:if>

  <!-- All done, close out the HTML -->
  </body>
</html>

</xsl:template>

<!-- Additional template for cdf:platform element; 
  -  this has changed to accomodate the CIS platform 
  -  schema 0.2.2.
  -->
<xsl:template match="cdf:platform" mode="list">
  <li xmlns="http://www.w3.org/1999/xhtml"><xsl:value-of select="key('platforms',@idref)/cdfp:title/text()"/>
  </li>
</xsl:template>

<!-- Additional templates for a Value element;
  -  For TOC, we present a line with number, for body
  -  we present a numbered section with title, and then
  -  the fields of the Value with a dl list.
  -->
<xsl:template match="cdf:Value" mode="toc">
  <xsl:param name="section-prefix"/>
  <xsl:param name="section-num" select="position()"/>

  <xsl:message>In toc template for Value, id=<xsl:value-of select="@id"/>.</xsl:message>
  <p xmlns="http://www.w3.org/1999/xhtml" class="toc2">
     <xsl:value-of select="$section-prefix"/>
     <xsl:value-of select="$section-num"/>
     <xsl:text>. </xsl:text>
     <a class="toc" href="#{@id}"><xsl:value-of select="./cdf:title/text()"/></a>
  </p>
</xsl:template>

<xsl:template match="cdf:Value" mode="body">
  <xsl:param name="section-prefix"/>
  <xsl:param name="section-num" select="position()"/>

  <xsl:message>In body template for Value, id=<xsl:value-of select="@id"/>.</xsl:message>
  <xsl:comment>Value id = <xsl:value-of select="./@id"/></xsl:comment>
  <div xmlns="http://www.w3.org/1999/xhtml">
  <h3><a name="{@id}"></a>
     <xsl:value-of select="$section-prefix"/>
     <xsl:value-of select="$section-num"/>
     <xsl:text>. Value: </xsl:text>
     <i><xsl:value-of select="./cdf:title/text()"/></i>
  </h3>

  <div class="simpleText">
    <xsl:if test="./cdf:status">
      <p>Status: <xsl:value-of select="./cdf:status/text()"/>
        <xsl:if test="./cdf:status/@date">
	   (as of <xsl:value-of select="./cdf:status/@date"/>)
	</xsl:if>
      </p>
    </xsl:if>
    <xsl:if test="./cdf:platform">
      <p>Applies only to:<ul>
        <xsl:apply-templates select="./cdf:platform" mode="list"/>
      </ul></p>
    </xsl:if>
    <p><xsl:text>Type: </xsl:text><xsl:value-of select="@type"/></p>
    <xsl:if test="@operator">
      <p><xsl:text>Operator: </xsl:text><xsl:value-of select="@operator"/></p>
    </xsl:if>


    <xsl:if test="/cdf:Benchmark/cdf:Profile">
        <p>Value and value contraints:
          <table class="propertyTable" border="1" cellpadding="1" cellspacing="1">
            <tr>
               <th class="propertyTableHead" style="width: 27%;">Property</th>
               <th class="propertyTableHead" style="width: 22%;">Selector</th>
               <th class="propertyTableHead">Value</th>
            </tr>
            <xsl:for-each select="./cdf:value | ./cdf:default | ./cdf:match | ./cdf:lower-bound | ./cdf:upper-bound">
	        <tr>
                   <td style="width: 27%;"><b><xsl:value-of select="local-name()"/></b></td>
		   <td style="width: 22%; text-align: center;"><xsl:choose>
                        <xsl:when test="not(string-length(./@selector) = 0)"><xsl:value-of select="./@selector"/></xsl:when>
	 	        <xsl:otherwise>*</xsl:otherwise>
	           </xsl:choose></td>
		   <td><xsl:value-of select="./text()"/></td>
		</tr>
	    </xsl:for-each>
            <xsl:for-each select="./cdf:choices">
	        <tr>
                   <td style="width: 27%;"><b><xsl:value-of select="local-name()"/></b>
		   </td>
		   <td style="width: 22%; text-align: center;"><xsl:choose>
                        <xsl:when test="./@selector"><xsl:value-of select="not(string-length(./@selector) = 0)"/></xsl:when>
	 	        <xsl:otherwise>*</xsl:otherwise>
	           </xsl:choose></td>
		   <td>
		      <xsl:if test="boolean(number(./@mustMatch))">
			<i>Exclusive values:</i>
 		      </xsl:if>
		      <xsl:if test="not(number(./@mustMatch))">
			<i>Suggested values:</i>
 		      </xsl:if>
                       <ul class="smallList">
			<xsl:for-each select="./cdf:choice">
			   <li><xsl:value-of select="./text()"/></li>
			</xsl:for-each>
		       </ul>
                   </td>
		</tr>
	    </xsl:for-each>
          </table>
        </p>
    </xsl:if>

    <xsl:if test="not(/cdf:Benchmark/cdf:Profile)">
	<p><xsl:text>Value: </xsl:text>
             <b><xsl:value-of select="./cdf:value/text()"/></b>
        </p>
	<xsl:if test="./cdf:default">
	  <p><xsl:text>Default value: </xsl:text>
              <xsl:value-of select="./cdf:default/text()"/>
          </p>
	</xsl:if>
	<xsl:if test="./@type = 'number'">
	  <xsl:if test="./cdf:lower-bound">
	    <p><xsl:text>Lower bound: </xsl:text><xsl:value-of select="./cdf:lower-bound/text()"/></p>
	  </xsl:if>
	  <xsl:if test="./cdf:upper-bound">
	    <p><xsl:text>Upper bound: </xsl:text><xsl:value-of select="./cdf:upper-bound/text()"/></p>
	  </xsl:if>
	</xsl:if>
        <xsl:if test="./@type = 'string'">
	  <xsl:if test="./cdf:match">
	    <p><xsl:text>Value pattern: </xsl:text><xsl:value-of select="./cdf:match/text()"/></p>
	  </xsl:if>
	</xsl:if>

	<xsl:if test="./cdf:choices">
	  <p><xsl:text>Choices: </xsl:text>
	    <xsl:if test="./cdf:choices/@mustMatch">
	       <xsl:text> (exclusive)</xsl:text>
	    </xsl:if>
	    <ul>
	       <xsl:for-each select="./cdf:choices/cdf:choice">
		  <li><xsl:value-of select="./text()"/></li>
	       </xsl:for-each>
	    </ul>
	  </p>
	</xsl:if>
    </xsl:if>

  </div>

  <xsl:if test="./cdf:description">
     <h4>Description</h4>
     <xsl:for-each select="./cdf:description">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <xsl:if test="./cdf:warning">
     <h4>Warning</h4>
     <xsl:for-each select="./cdf:warning">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <xsl:if test="./cdf:reference">
      <h4>References</h4>
      <ol>
	<xsl:for-each select="./cdf:reference">
	  <li><xsl:value-of select="text()"/>
	    <xsl:if test="@href">
	      [<a href="{@href}">link</a>]
           </xsl:if>
	  </li>
	</xsl:for-each>
     </ol>
  </xsl:if>
  </div>
</xsl:template>

<!-- Template for toc entries for both rules and groups -->
<xsl:template match="cdf:Group | cdf:Rule" mode="toc">
  <xsl:param name="section-prefix"/>
  <xsl:param name="section-num" select="position()"/>

  <xsl:message>In toc template for Group|Rule, id=<xsl:value-of select="@id"/>.</xsl:message>
  <p xmlns="http://www.w3.org/1999/xhtml" class="toc2">
     <xsl:value-of select="$section-prefix"/>
     <xsl:value-of select="$section-num"/>
     <xsl:text>. </xsl:text>
     <a class="toc" href="#{@id}"><xsl:value-of select="./cdf:title/text()"/></a>
  </p>
  <xsl:if test="./cdf:Group | ./cdf:Rule">
	<xsl:apply-templates mode="toc"
         select="./cdf:Group[not(number(@hidden)+number(@abstract))] | ./cdf:Rule[not(number(@hidden)+number(@abstract))]">
	    <xsl:with-param name="section-prefix" 
             select="concat($section-prefix,$section-num,'.')"/>
	</xsl:apply-templates>
  </xsl:if>
</xsl:template>


<!-- Template for toc entries for Profiles -->
<xsl:template match="cdf:Profile" mode="toc">
  <xsl:param name="section-prefix"/>
  <xsl:param name="section-num" select="position()"/>

  <xsl:message>In toc template for Profile, id=<xsl:value-of select="@id"/>.</xsl:message>
  <p xmlns="http://www.w3.org/1999/xhtml" class="toc2">
     <xsl:value-of select="$section-prefix"/>
     <xsl:value-of select="$section-num"/>
     <xsl:text>. </xsl:text>
     <a class="toc" href="#profile-{@id}"><xsl:value-of select="./cdf:title/text()"/></a>
  </p>

</xsl:template>

<!-- template for body elements for Profiles -->
<xsl:template match="cdf:Profile" mode="body">
  <xsl:param name="section-prefix"/>
  <xsl:param name="section-num" select="position()"/>

  <xsl:message>In body template for Profile, id=<xsl:value-of select="@id"/>.</xsl:message>
  <xsl:comment>Profile id = <xsl:value-of select="./@id"/></xsl:comment>
  <div xmlns="http://www.w3.org/1999/xhtml">
  <h3><a name="profile-{@id}"></a>
     <xsl:value-of select="$section-prefix"/>
     <xsl:value-of select="$section-num"/>
     <xsl:text>. Profile: </xsl:text>
     <i><xsl:value-of select="./cdf:title/text()"/></i>
  </h3>

  <div class="simpleText">
    <xsl:if test="@extends">
      <p>Extends: 
          <xsl:apply-templates select="key('profiles',@extends)" mode="prof-ref"/>
      </p>
    </xsl:if>
    <xsl:if test="./cdf:status">
      <p>Status: <xsl:value-of select="./cdf:status/text()"/>
        <xsl:if test="./cdf:status/@date">
	   (as of <xsl:value-of select="./cdf:status/@date"/>)
	</xsl:if>
      </p>
    </xsl:if>
    <xsl:if test="./cdf:platform">
      <p>Applies only to:<ul>
        <xsl:apply-templates select="./cdf:platform" mode="list"/>
      </ul></p>
    </xsl:if>
  </div>

  <xsl:if test="./cdf:description">
     <h4>Description</h4>
     <xsl:for-each select="./cdf:description">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <xsl:if test="./cdf:select">
   <h4>Item Selections</h4>
   <div class="propertyText">
    <p>Rules and Groups explicitly selected and deselected for this profile.</p>
       <ul>
         <xsl:apply-templates select="." mode="sel-list"/>
       </ul>
   </div>
  </xsl:if>

  <xsl:if test="./cdf:set-value | ./cdf:refine-value">
   <h4>Value Settings</h4>
   <div class="propertyText">
    <p>Tailoring value adjustments explicitly set for this profile:</p>
       <ul>
         <xsl:apply-templates select="." mode="set-list"/>
       </ul>
   </div>
  </xsl:if>

  <xsl:if test="./cdf:reference">
      <h4>References</h4>
      <ol>
	<xsl:for-each select="./cdf:reference">
	  <li><xsl:value-of select="text()"/>
	    <xsl:if test="@href">
	      [<a href="{@href}">link</a>]
           </xsl:if>
	  </li>
	</xsl:for-each>
     </ol>
  </xsl:if>
  </div>
</xsl:template>

<xsl:template match="cdf:Profile" mode="sel-list">
   <xsl:apply-templates select="./cdf:select" mode="sel-list"/>
</xsl:template>

<xsl:template match="cdf:Profile" mode="set-list">
   <xsl:apply-templates select="./cdf:set-value" mode="set-list"/>
   <xsl:apply-templates select="./cdf:refine-value" mode="set-list"/>
</xsl:template>

<xsl:template match="cdf:select" mode="sel-list">
   <li xmlns="http://www.w3.org/1999/xhtml">
       <xsl:if test="number(./@selected)">Included: </xsl:if>
       <xsl:if test="not(number(./@selected))">Excluded: </xsl:if>
       <xsl:if test="count(key('items',@idref))">
            <a href="#{@idref}">
                <xsl:value-of select="key('items', @idref)/cdf:title/text()"/>
            </a>
       </xsl:if>
       <xsl:if test="not(count(key('items',@idref)))">
            (cluster) 
            <xsl:for-each select="key('clusters-rg',@idref)">
              <a href="#{./@id}">
                <xsl:value-of select="./cdf:title/text()"/>
              </a> 
            </xsl:for-each>
       </xsl:if>
   </li>
</xsl:template>

<xsl:template match="cdf:set-value" mode="set-list">
   <li xmlns="http://www.w3.org/1999/xhtml">
     <a href="#{@idref}"><xsl:value-of select="key('items', @idref)/cdf:title/text()"/></a><br/><xsl:text> set to value: </xsl:text><b><xsl:value-of select="./text()"/></b>
   </li>
</xsl:template>

<xsl:template match="cdf:refine-value" mode="set-list">
   <li xmlns="http://www.w3.org/1999/xhtml">
     <a href="#{@idref}"><xsl:value-of select="key('items', @idref)/cdf:title/text()"/></a><br/><xsl:text> refinement selector: </xsl:text><b><xsl:value-of select="./@selector"/></b>
   </li>
</xsl:template>

<xsl:template match="cdf:Profile" mode="prof-ref">
   <a href="#profile-{@id}"><xsl:value-of select="./cdf:title/text()"/></a>
</xsl:template>


<!-- Additional template for a Group element;
  -  we present a numbered section with title, and then
  -  the fields of the Group with a dl list, then the
  -  enclosed items as subsections.
  -->
<xsl:template match="cdf:Group" mode="body">
  <xsl:param name="section-prefix"/>
  <xsl:param name="section-num" select="position()"/>

  <xsl:message>In body template for Group, id=<xsl:value-of select="@id"/>.</xsl:message>
  <xsl:comment>Group id = <xsl:value-of select="./@id"/></xsl:comment>
  <div  xmlns="http://www.w3.org/1999/xhtml">
  <h3><a name="{@id}"></a>
     <xsl:value-of select="$section-prefix"/>
     <xsl:value-of select="$section-num"/>
     <xsl:text>. Group: </xsl:text>
     <i><xsl:value-of select="./cdf:title/text()"/></i>
  </h3>

  <xsl:if test="./cdf:status | ./cdf:platform">
  <div class="simpleText">
    <xsl:if test="./cdf:status">
      <p>Status: <xsl:value-of select="./cdf:status/text()"/>
        <xsl:if test="./cdf:status/@date">
	   (as of <xsl:value-of select="./cdf:status/@date"/>)
	</xsl:if>
      </p>
    </xsl:if>
    <xsl:if test="./cdf:platform">
      <p>Applies only to:<ul>
        <xsl:apply-templates select="./cdf:platform" mode="list"/>
      </ul></p>
    </xsl:if>
  </div>
  </xsl:if>

  <xsl:if test="./cdf:description">
     <!-- <h4>Description</h4> -->
     <xsl:for-each select="./cdf:description">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <xsl:if test="./cdf:rationale">
     <h4>Rationale</h4>
     <xsl:for-each select="./cdf:rationale">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <xsl:if test="./cdf:warning">
     <h4>Warning</h4>
     <xsl:for-each select="./cdf:warning">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <xsl:if test="./cdf:requires | ./cdf:conflicts">
     <h4>Dependencies</h4>
     <div class="propertyText">
     <ul>
     <xsl:for-each select="./cdf:requires">
          <xsl:variable name="thisid" select="@idref"/>
          <li><xsl:text>Requires: </xsl:text>
	     <a href="#{@idref}">
	       <xsl:value-of select="key('items', @idref)/cdf:title/text()"/>
	     </a>
	  </li>
     </xsl:for-each>
     <xsl:for-each select="./cdf:conflicts">
          <xsl:variable name="thisid" select="@idref"/>
          <li><xsl:text>Conflicts with: </xsl:text>
	     <a href="#{@idref}">
	       <xsl:value-of select="key('items', @idref)/cdf:title/text()"/>
	     </a>
	  </li>
     </xsl:for-each>
     </ul>
     </div>
  </xsl:if>

  <xsl:if test="./cdf:reference">
     <h4>References</h4>
     <ol>
       <xsl:for-each select="./cdf:reference">
	   <li><xsl:value-of select="text()"/>
	     <xsl:if test="@href">
	       [<a href="{@href}">link</a>]
             </xsl:if>
	   </li>
        </xsl:for-each>
      </ol>
  </xsl:if>

  <xsl:if test="./cdf:Group | ./cdf:Rule">
      <xsl:apply-templates mode="body" select="./cdf:Group[not(number(@hidden)+number(@abstract))] | ./cdf:Rule[not(number(@hidden)+number(@abstract))]">
         <xsl:with-param name="section-prefix" 
                         select="concat($section-prefix,$section-num,'.')"/>
      </xsl:apply-templates>
  </xsl:if>
  </div>
</xsl:template>

<!-- Additional template for a Rule element in body;
  -  we present a numbered section with title, and then
  -  the fields of the Rule with a dl list.
  -->
<xsl:template match="cdf:Rule" mode="body">
  <xsl:param name="section-prefix"/>
  <xsl:param name="section-num" select="position()"/>

  <xsl:message>In body template for Rule, id=<xsl:value-of select="@id"/>.</xsl:message>
  <xsl:comment>Rule id = <xsl:value-of select="./@id"/></xsl:comment>
  <div xmlns="http://www.w3.org/1999/xhtml">
  <h3><a name="{@id}"></a>
     <xsl:value-of select="$section-prefix"/>
     <xsl:value-of select="$section-num"/>
     <xsl:text>. Rule: </xsl:text>
     <i><xsl:value-of select="./cdf:title/text()"/></i>
  </h3>

  <xsl:if test="./cdf:status | ./cdf:platform">
    <div class="simpleText">
      <xsl:if test="./cdf:status">
	<p>Status: <xsl:value-of select="./cdf:status/text()"/>
	  <xsl:if test="./cdf:status/@date">
	     (as of <xsl:value-of select="./cdf:status/@date"/>)
	  </xsl:if>
	</p>
      </xsl:if>
      <xsl:if test="./cdf:platform">
	<p>Applies only to:<ul>
	  <xsl:apply-templates select="./cdf:platform" mode="list"/>
	</ul></p>
      </xsl:if>
    </div>
  </xsl:if>
  
  <xsl:if test="./cdf:description">
     <!-- <h4>Description</h4> -->
     <xsl:for-each select="./cdf:description">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <xsl:if test="./cdf:rationale">
     <h4>Rationale</h4>
     <xsl:for-each select="./cdf:rationale">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <xsl:if test="./cdf:warning">
     <h4>Warning</h4>
     <xsl:for-each select="./cdf:warning">
       <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
       </div>
     </xsl:for-each>
  </xsl:if>

  <xsl:if test="./cdf:fixtext | ./cdf:fix">
     <h4>Remediation</h4>
     <xsl:for-each select="./cdf:fixtext">
        <div class="propertyText">
          <xsl:apply-templates select="./text() | ./*" mode="text"/>
        </div>
     </xsl:for-each>
     <xsl:if test="./cdf:fix">
        <div class="propertyText">
	   <p>Fix:
	     <pre>
	       <xsl:apply-templates select="./cdf:fix/text() | ./cdf:fix/*" mode="text"/>
	     </pre>
           </p>
        </div>
      </xsl:if>
  </xsl:if>

  <xsl:if test="./cdf:requires | ./cdf:conflicts">
     <h4>Dependencies</h4>
     <div class="propertyText"><ul>
     <xsl:for-each select="./cdf:requires">
          <xsl:variable name="thisid" select="@idref"/>
          <li><xsl:text>Requires: </xsl:text>
	     <a href="#{@idref}">
	       <xsl:value-of select="key('items', @idref)/cdf:title/text()"/>
	     </a>
	  </li>
     </xsl:for-each>
     <xsl:for-each select="./cdf:conflicts">
          <xsl:variable name="thisid" select="@idref"/>
          <li><xsl:text>Conflicts with: </xsl:text>
	     <a href="#{@idref}">
	       <xsl:value-of select="key('items', @idref)/cdf:title/text()"/>
	     </a>
	  </li>
     </xsl:for-each>
     </ul></div>
  </xsl:if>    

  <xsl:if test="./cdf:reference">
     <h4>References</h4>
     <ol>
	 <xsl:for-each select="./cdf:reference">
	   <li><xsl:value-of select="text()"/>
	     <xsl:if test="@href">
	       [<a href="{@href}">link</a>]
             </xsl:if>
	   </li>
	 </xsl:for-each>
    </ol>
  </xsl:if>
  </div>
</xsl:template>

<!-- templates in mode "text", for processing text with 
     markup and substitutions.
 -->
<xsl:template match="text()" mode="text">
  <xsl:value-of select="."/>
</xsl:template>
<xsl:template match="cdf:sub[@idref]" mode="text">
  <xsl:variable name="subid" select="./@idref"/><i>
  <a xmlns="http://www.w3.org/1999/xhtml" href="#{@idref}">
    <xsl:value-of select="//cdf:Value[@id = $subid]/cdf:value/text()"/>
  </a></i>
</xsl:template>
<xsl:template match="*" mode="text">
  <xsl:copy>
    <xsl:for-each select="./@*">
        <xsl:copy-of select="."/>
    </xsl:for-each>
    <xsl:apply-templates select="./text() | ./*" mode="text"/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
