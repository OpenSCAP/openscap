<?xml version="1.0" encoding="UTF-8"?>
<!--
$Id: results_to_html.xsl 4295 2007-08-23 17:02:56Z bakerj $

****************************************************************************************
 Copyright (c) 2002-2007, The MITRE Corporation
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are
 permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice, this list
       of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice, this 
       list of conditions and the following disclaimer in the documentation and/or other
       materials provided with the distribution.
     * Neither the name of The MITRE Corporation nor the names of its contributors may be
       used to endorse or promote products derived from this software without specific 
       prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

****************************************************************************************
	
	AUTHOR:Matt Burton, The Mitre Corporation 
	DATE: 02 May 2005 
	
	Modified by Loren Bandiera, MMG Security
   	   * Updating for v5 results
	DATE: 10 May 2006
	
	Reimplemented by Jon Baker, The Mitre Corporation 
	DATE: 12 October 2006 

	Modified by Vladimir Giszpenc, DSCI Contractor Supporting CERDEC S&TCD IAD
   	   * Allowing for references other than CVE such as Red Hat patches
	DATE: 18 May 2007

	Modified by Vladimir Giszpenc, DSCI Contractor Supporting CERDEC S&TCD IAD
   	   * Added some aggregate data in the Systems Analysed section
	DATE: 20 Aug 2007

	The results_to_html stylesheet converts an OVAL Results document into a more readable html format.
	General information about the source of the OVAL Definitions being reported on, and the OVAL Results
	producer is displayed. Next general information about each system analyzed is presented including a 
	table or result information. The table displays true results then all other results sorted in
	descending order by result. If the OVAL Results document has results for multiple systems a set
	of links will be generated near the top of the resulting html to allow users to easily jump to the 
	each system's results.
	
-->
<xsl:stylesheet version="1.1" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5"
	xmlns:oval-res="http://oval.mitre.org/XMLSchema/oval-results-5" xmlns:oval-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5"
	xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:apache-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#apache"
	xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:windows-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#windows"
	xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:linux-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux">
	<xsl:output method="html" indent="yes" omit-xml-declaration="yes"/>

	<xsl:key name="definition-index" use="@id" match="/oval-res:oval_results/oval-def:oval_definitions/oval-def:definitions/oval-def:definition"/>

	<!-- Style declarations for pretty formatting -->
	<xsl:template name="oval_res_style">
		<style type="text/css">

TD.title {BACKGROUND-COLOR: #000000; COLOR: #ffc; TEXT-ALIGN: left; font: bold 12pt/14pt "Arial"} 
TD.label {BACKGROUND-COLOR: #99cc99; font: 10pt/12pt "Arial"}
TD.label2 {font: bold 10pt/14pt "Arial"}
TD.text {font: 10pt/12pt "Arial"}

.trueA{background-color: #FFBC8F; font: 10pt/12pt "Arial"}
.trueB{background-color: #FFE0CC; font: 10pt/12pt "Arial"}
				
.falseA{background-color: #ACD685; font: 10pt/12pt "Arial"}
.falseB{background-color: #CBE6B3; font: 10pt/12pt "Arial"}
				
.unknownA{background-color: #AEC8E0; font: 10pt/12pt "Arial"}
.unknownB{background-color: #DAE6F1; font: 10pt/12pt "Arial"}
				
.errorA{background-color: #FFDD75; font: 10pt/12pt "Arial"}
.errorB{background-color: #FFECB3; font: 10pt/12pt "Arial"}
				
.naA{background-color: #EEEEEE; font: 10pt/12pt "Arial"}
.naB{background-color: #FFFFFF; font: 10pt/12pt "Arial"}
				
.neA{background-color: #EEEEEE; font: 10pt/12pt "Arial"}
.neB{background-color: #FFFFFF; font: 10pt/12pt "Arial"}

		</style>
	</xsl:template>
	
	<xsl:template name="ResultColorTable">
		<table border="0" cellspacing="0" cellpadding="0">
			<tr>
				<td>&#160;&#160;&#160;</td>
				<td>
					<!--<td class="label2">Result Color Key:&#160;&#160;</td>-->
					<table border="1" cellpadding="0" cellspacing="0">
						<tr>
							<td class="trueA" width="10">&#160;</td>
							<td class="trueB" width="10">&#160;</td>
							<td class="text">&#160;True&#160;&#160;</td>
						</tr>
					</table>
				</td>
				<td>&#160;&#160;&#160;</td>
				<td>
				<table border="1" cellpadding="0" cellspacing="0">
					<tr>
						<td class="falseA" width="10">&#160;</td>
						<td class="falseB" width="10">&#160;</td>
						<td class="text">&#160;False&#160;&#160;</td>
					</tr>
				</table>
				</td>
				<td>&#160;&#160;&#160;</td>
				<td>
					<table border="1" cellpadding="0" cellspacing="0">
						<tr>
							<td class="errorA" width="10">&#160;</td>
							<td class="errorB" width="10">&#160;</td>
							<td class="text">&#160;Error&#160;&#160;</td>
						</tr>
					</table>
				</td>
				<td>&#160;&#160;&#160;</td>
				<td>
					<table border="1" cellpadding="0" cellspacing="0">
						<tr>
							<td class="unknownA" width="10">&#160;</td>
							<td class="unknownB" width="10">&#160;</td>
							<td class="text">&#160;Unknown&#160;&#160;</td>
						</tr>
					</table>
				</td>
				<td>&#160;&#160;&#160;</td>
				<td>
					<table border="1" cellpadding="0" cellspacing="0">
						<tr>
							<td class="naA" width="10">&#160;</td>
							<td class="naB" width="10">&#160;</td>
							<td class="text">&#160;Not Applicable&#160;&#160;</td>
						</tr>
					</table>
				</td>
				<td>&#160;&#160;&#160;</td>
				<td>
					<table border="1" cellpadding="0" cellspacing="0">
						<tr>
							<td class="neA" width="10">&#160;</td>
							<td class="neB" width="10">&#160;</td>
							<td class="text">&#160;Not Evaluated&#160;&#160;</td>	
						</tr>
					</table>
				</td>
			</tr>
		</table>
	</xsl:template>

	<!-- Gets the top level node -->
	<xsl:template match="oval-res:oval_results">
		<html>
			<head>
				<title>OVAL Results</title>
				<!-- Get stylesheet -->
				<xsl:call-template name="oval_res_style"/>
			</head>
			<body>
								
				<!-- display results and definition generator information -->
				<table border="1" cellpadding="0" cellspacing="0" width="100%">
					<tr>
						<td width="50%">
							<xsl:call-template name="ResultGenerator">
								<xsl:with-param name="generatorElm" select="./oval-res:generator"/>
							</xsl:call-template>
						</td>
						<td width="50%">
							<xsl:call-template name="DefGenerator">
								<xsl:with-param name="generatorElm" select="./oval-def:oval_definitions/oval-def:generator"/>
							</xsl:call-template>
						</td>
					</tr>
				</table>
				<br/>

				<!-- 
					create anchors to each system in the results file
					if only one systen leave out the anchors
				-->
				<!-- 2007-08-20 Added aggregate result detail-->
				<xsl:if test="not(count(./oval-res:results/oval-res:system) = 1)">
					<table border="1" cellpadding="0" cellspacing="0">
						<tr>
							<td class="title" colspan="7">Systems Analyzed</td>
						</tr>
						<!-- display the result color coding table -->
						<tr><td colspan="7"><xsl:call-template name="ResultColorTable"/></td></tr>
						
						<tr>
							<td class="label" align="center">System</td>
							<td class="label" align="center">Trues</td>
							<td class="label" align="center">Falses</td>
							<td class="label" align="center">Errors</td>
							<td class="label" align="center">Unknown</td>
							<td class="label" align="center">Not Applicables</td>
							<td class="label" align="center">Not Evaluateds</td>
						</tr>
						<xsl:for-each select="./oval-res:results/oval-res:system">
							<tr>
								<td class="label">
									<a href="#{position()}">
										<xsl:value-of select="./oval-sc:oval_system_characteristics/oval-sc:system_info/oval-sc:primary_host_name"/>
									</a>
								</td>
								<td width="10">
									<xsl:choose>
										<xsl:when test="position() mod 2 = 1">
											<xsl:attribute name="class">trueA</xsl:attribute>	
										</xsl:when>
										<xsl:when test="position() mod 2 = 0">
											<xsl:attribute name="class">trueB</xsl:attribute>
										</xsl:when>
									</xsl:choose>
									<xsl:value-of select="count(./oval-res:definitions/oval-res:definition[@result='true'])"/>
								</td>
								<td width="10">
									<xsl:choose>
										<xsl:when test="position() mod 2 = 1">
											<xsl:attribute name="class">falseA</xsl:attribute>	
										</xsl:when>
										<xsl:when test="position() mod 2 = 0">
											<xsl:attribute name="class">falseB</xsl:attribute>
										</xsl:when>
									</xsl:choose>
									<xsl:value-of select="count(./oval-res:definitions/oval-res:definition[@result='false'])"/>
								</td>
								<td width="10">
									<xsl:choose>
										<xsl:when test="position() mod 2 = 1">
											<xsl:attribute name="class">errorA</xsl:attribute>	
										</xsl:when>
										<xsl:when test="position() mod 2 = 0">
											<xsl:attribute name="class">errorB</xsl:attribute>
										</xsl:when>
									</xsl:choose>
									<xsl:value-of select="count(./oval-res:definitions/oval-res:definition[@result='error'])"/>
								</td>
								<td width="10">
									<xsl:choose>
										<xsl:when test="position() mod 2 = 1">
											<xsl:attribute name="class">unknownA</xsl:attribute>	
										</xsl:when>
										<xsl:when test="position() mod 2 = 0">
											<xsl:attribute name="class">unknownB</xsl:attribute>
										</xsl:when>
									</xsl:choose>
									<xsl:value-of select="count(./oval-res:definitions/oval-res:definition[@result='unknown'])"/>
								</td>
								<td width="10">
									<xsl:choose>
										<xsl:when test="position() mod 2 = 1">
											<xsl:attribute name="class">naA</xsl:attribute>	
										</xsl:when>
										<xsl:when test="position() mod 2 = 0">
											<xsl:attribute name="class">naB</xsl:attribute>
										</xsl:when>
									</xsl:choose>
									<xsl:value-of select="count(./oval-res:definitions/oval-res:definition[@result='not applicable'])"/>
								</td>
								<td width="10">
									<xsl:choose>
										<xsl:when test="position() mod 2 = 1">
											<xsl:attribute name="class">neA</xsl:attribute>	
										</xsl:when>
										<xsl:when test="position() mod 2 = 0">
											<xsl:attribute name="class">neB</xsl:attribute>
										</xsl:when>
									</xsl:choose>
									<xsl:value-of select="count(./oval-res:definitions/oval-res:definition[@result='not evaluated'])"/>
								</td>
							</tr>
							<!-- 2007-08-20 Added aggregate result detail-->
						</xsl:for-each>
					</table>
					<br/>
				</xsl:if>

				<!-- 
					for each system in the results file
					 - display system info
					 - display the sc generator
					 - display results table
				-->
				<xsl:for-each select="./oval-res:results/oval-res:system">
					<!-- display the system info data -->
					<xsl:call-template name="SystemInfo">
						<xsl:with-param name="sysInfoElm" select="./oval-sc:oval_system_characteristics/oval-sc:system_info"/>
					</xsl:call-template>

					<!-- display the generator info for the sc data -->
					<xsl:call-template name="SCGenerator">
						<xsl:with-param name="generatorElm" select="./oval-sc:oval_system_characteristics/oval-sc:generator"/>
					</xsl:call-template>

					<!-- display definition results -->
					<xsl:call-template name="DefinitionsResults">
						<xsl:with-param name="definitionsElm" select="./oval-res:definitions"/>
					</xsl:call-template>
				</xsl:for-each>
			</body>
		</html>
	</xsl:template>

	<!-- Get the system_info and put it into a table. -->
	<xsl:template name="SystemInfo">
		<xsl:param name="sysInfoElm"/>
		<table border="1" cellspacing="0" cellpaddign="2" width="100%" bgcolor="#cccccc">
			<tr>
				<td class="title" colspan="2">
					<a name="{position()}">System Information</a>
				</td>
			</tr>
			<tr>
				<td class="label2" width="20%">Host Name</td>
				<td class="text" width="80%"><xsl:value-of select="$sysInfoElm/child::oval-sc:primary_host_name/text()"/>&#160;</td>
				<!-- $#160; is used to keep empty cells in the table clean -->
			</tr>
			<tr>
				<td class="label2" width="20%">Operating System</td>
				<td class="text" width="80%"><xsl:value-of select="$sysInfoElm/child::oval-sc:os_name/text()"/>&#160;</td>
			</tr>
			<tr>
				<td class="label2" width="20%">Operating System Version</td>
				<td class="text" width="80%"><xsl:value-of select="$sysInfoElm/child::oval-sc:os_version/text()"/>&#160;</td>
			</tr>
			<tr>
				<td class="label2" width="20%">Architecture</td>
				<td class="text" width="80%"><xsl:value-of select="$sysInfoElm/child::oval-sc:architecture/text()"/>&#160;</td>
			</tr>
			<tr>
				<td class="label2" width="20%">Interfaces</td>
				<td width="80%">
					<xsl:call-template name="Interfaces">
						<xsl:with-param name="iterfacesElm" select="$sysInfoElm[1]/oval-sc:interfaces"/>
					</xsl:call-template>
				</td>
			</tr>
		</table>
	</xsl:template>

	<!-- Get this interface and put it into the table, using templates to loop through all possible interfaces -->
	<xsl:template name="Interfaces">
		<xsl:param name="iterfacesElm"/>

		<xsl:for-each select="$iterfacesElm/oval-sc:interface">
			<xsl:if test="position() mod 2 = 1">
				<table border="1" cellpadding="1" cellspacing="0" width="100%" bgcolor="ffffff">
					<tr>
						<td class="label2" width="20%">Interface Name</td>
						<td class="text" width="80%"><xsl:value-of select="./oval-sc:interface_name/text()"/>&#160;</td>
					</tr>
					<tr>
						<td class="label2" width="20%">IP Address</td>
						<td class="text" width="80%"><xsl:value-of select="./oval-sc:ip_address/text()"/>&#160;</td>
					</tr>
					<tr>
						<td class="label2" width="20%">MAC Address</td>
						<td class="text" width="80%"><xsl:value-of select="./oval-sc:mac_address/text()"/>&#160;</td>
					</tr>
				</table>
			</xsl:if>
			<xsl:if test="position() mod 2 = 0">
				<table border="1" cellpadding="1" cellspacing="0" width="100%" bgcolor="eeeeee">
					<tr>
						<td class="label2" width="20%">Interface Name</td>
						<td class="text" width="80%"><xsl:value-of select="./oval-sc:interface_name/text()"/>&#160;</td>
					</tr>
					<tr>
						<td class="label2" width="20%">IP Address</td>
						<td class="text" width="80%"><xsl:value-of select="./oval-sc:ip_address/text()"/>&#160;</td>
					</tr>
					<tr>
						<td class="label2" width="20%">MAC Address</td>
						<td class="text" width="80%"><xsl:value-of select="./oval-sc:mac_address/text()"/>&#160;</td>
					</tr>
				</table>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>

	<!-- Create a table for the Result Generator and call its children. -->
	<xsl:template name="ResultGenerator">
		<xsl:param name="generatorElm"/>
		<table border="1" cellspacing="0" cellpaddign="2" width="100%" bgcolor="#cccccc">
			<tr>
				<td class="title" colspan="5">OVAL Results Generator Information</td>
			</tr>
			<tr>
				<td class="label" nowrap="nowrap">Schema Version</td>
				<td class="label" nowrap="nowrap">Product Name</td>
				<td class="label" nowrap="nowrap">Product Version</td>
				<td class="label">Date</td>
				<td class="label">Time</td>
			</tr>
			<xsl:call-template name="Generator">
				<xsl:with-param name="generatorElm" select="$generatorElm"/>
			</xsl:call-template>
		</table>
	</xsl:template>

	<!-- Create a table for the System Characteristics Generator and call its children. -->
	<xsl:template name="SCGenerator">
		<xsl:param name="generatorElm"/>
		<table border="1" cellspacing="0" cellpaddign="2" width="100%" bgcolor="#cccccc">
			<tr>
				<td class="title" colspan="5">OVAL System Characteristics Generator Information</td>
			</tr>
			<tr>
				<td class="label">Schema Version</td>
				<td class="label">Product Name</td>
				<td class="label">Product Version</td>
				<td class="label">Date</td>
				<td class="label">Time</td>
			</tr>
			<xsl:call-template name="Generator">
				<xsl:with-param name="generatorElm" select="$generatorElm"/>
			</xsl:call-template>
		</table>
	</xsl:template>

	<!-- Create a table for the Definitions Generator and call its children. -->
	<xsl:template name="DefGenerator">
		<xsl:param name="generatorElm"/>
		<table border="1" cellspacing="0" cellpaddign="2" width="100%" bgcolor="#cccccc">
			<tr>
				<td class="title" colspan="5">OVAL Definition Generator Information</td>
			</tr>
			<tr>
				<td class="label" nowrap="nowrap">Schema Version</td>
				<td class="label" nowrap="nowrap">Product Name</td>
				<td class="label" nowrap="nowrap">Product Version</td>
				<td class="label">Date</td>
				<td class="label">Time</td>
			</tr>
			<xsl:call-template name="Generator">
				<xsl:with-param name="generatorElm" select="$generatorElm"/>
			</xsl:call-template>
		</table>
	</xsl:template>

	<!-- Each child of Generators (oval, system_characteristics, results) is the same, this template gets their children. -->
	<xsl:template name="Generator">
		<xsl:param name="generatorElm"/>
		<xsl:variable name="MessyNumber" select="string($generatorElm/oval:timestamp)"/>
		<td class="text"><xsl:value-of select="$generatorElm/oval:schema_version"/>&#160;</td>
		<td class="text"><xsl:value-of select="$generatorElm/oval:product_name"/>&#160;</td>
		<td class="text"><xsl:value-of select="$generatorElm/oval:product_version"/>&#160;</td>
		<td class="text">
			<!--Create variable "MessyNumber" to make time stamp a string and then print it out in a readable version -->
			<xsl:value-of select="substring($MessyNumber, 1, 4)"/>
			<!-- year -->
			<xsl:text>-</xsl:text>
			<xsl:value-of select="substring($MessyNumber, 6, 2)"/>
			<!-- month -->
			<xsl:text>-</xsl:text>
			<xsl:value-of select="substring($MessyNumber, 9, 2)"/>
			<!-- day -->
		</td>
		<td class="text">
			<xsl:value-of select="substring($MessyNumber, 12, 2)"/>
			<xsl:text>:</xsl:text>
			<!-- hour -->
			<xsl:value-of select="substring($MessyNumber, 15, 2)"/>
			<xsl:text>:</xsl:text>
			<!-- minute -->
			<xsl:value-of select="substring($MessyNumber, 18, 2)"/>&#160; <!-- second -->
		</td>
	</xsl:template>

	<xsl:template name="DefinitionsResults">
		<xsl:param name="definitionsElm"/>
		<table border="1" cellspacing="0" cellpaddign="2" width="100%">
			<tr>
				<td class="title" colspan="7">Oval Definition Results</td>
			</tr>
			<!-- display the result color coding table -->
			<tr><td colspan="7"><xsl:call-template name="ResultColorTable"/></td></tr>
			
			<tr>
				<td class="label" align="center">OVAL ID</td>
				<td class="label" align="center">Result</td>
				<td class="label" align="center">Class</td>
				<td class="label" align="center">Reference ID</td>
				<td class="label" align="center">Title</td>
			</tr>

			<!-- 
				Process true results then all others
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result = 'true']">
				<xsl:sort select="@result" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result != 'true']">
				<xsl:sort select="@result" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>
			-->
			
			<!-- 
				Process all results in descending order by result attribute then definition id 
			<xsl:for-each select="$definitionsElm/oval-res:definition">
				<xsl:sort select="@result" data-type="text" order="descending"/>
				<xsl:sort select="@id" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>
			-->
			
			
			<!-- 
				select the definitions with the desired result
				sort them by their class
				display them 
			-->
			
			<!-- process true results -->
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result='true']">
				<xsl:sort select="@id" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>
			
			<!-- process unknown results -->
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result='unknown']">
				<xsl:sort select="@id" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>
			
			<!-- process error results -->
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result='error']">
				<xsl:sort select="@id" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>
			
			<!-- process not evaluated results -->
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result='not evaluated']">
				<xsl:sort select="@id" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>
			
			<!-- process false results -->
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result='false']">
				<xsl:sort select="@id" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>
			
			<!-- process not applicable results -->
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result='not applicable']">
				<xsl:sort select="@id" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>

		</table>
	</xsl:template>

	<xsl:template name="Definition">
		<xsl:param name="definitionElm"/>
		<tr>
			<!-- set results to alternating colors -->
			<xsl:choose>
				<xsl:when test="$definitionElm/@result  = 'true' and position() mod 2 = 1">
					<xsl:attribute name="class">trueA</xsl:attribute>	
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'true' and position() mod 2 = 0">
					<xsl:attribute name="class">trueB</xsl:attribute>
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'false' and position() mod 2 = 1">
					<xsl:attribute name="class">falseA</xsl:attribute>
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'false' and position() mod 2 = 0">
					<xsl:attribute name="class">falseB</xsl:attribute>
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'unknown' and position() mod 2 = 1">
					<xsl:attribute name="class">unknownA</xsl:attribute>
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'unknown' and position() mod 2 = 0">
					<xsl:attribute name="class">unknownB</xsl:attribute>
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'error' and position() mod 2 = 1">
					<xsl:attribute name="class">errorA</xsl:attribute>				
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'error' and position() mod 2 = 0">
					<xsl:attribute name="class">errorB</xsl:attribute>
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'not applicable' and position() mod 2 = 1">
					<xsl:attribute name="class">naA</xsl:attribute>				
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'not applicable' and position() mod 2 = 0">
					<xsl:attribute name="class">naB</xsl:attribute>
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'not evaluated' and position() mod 2 = 1">
					<xsl:attribute name="class">neA</xsl:attribute>				
				</xsl:when>
				<xsl:when test="$definitionElm/@result  = 'not evaluated' and position() mod 2 = 0">
					<xsl:attribute name="class">neB</xsl:attribute>
				</xsl:when>
			</xsl:choose>

			<!-- id -->
			<xsl:variable name="idUrl" select="concat('http://oval.mitre.org/repository/data/getDef?id=', $definitionElm/@definition_id)"/>
			<td class="text" align="center">
				<!--
				<a>
					<xsl:attribute name="target">_blank</xsl:attribute>
					<xsl:attribute name="href">
						<xsl:value-of select="$idUrl"/>
					</xsl:attribute>
					<xsl:value-of select="$definitionElm/@definition_id"/>
				</a>
				-->
				<xsl:value-of select="$definitionElm/@definition_id"/>
			</td>

			<!-- result -->
			<td class="text" align="center">
				<xsl:value-of select="$definitionElm/@result"/>
			</td>

			<!-- Class -->
			<td class="text" align="center">
				<xsl:for-each select="key('definition-index', @definition_id)">
					<xsl:value-of select="@class"/>
				</xsl:for-each>
			</td>

			<!-- reference id -->
			<td class="text" align="center">
				<xsl:for-each select="key('definition-index', @definition_id)">
					<xsl:for-each select="oval-def:metadata/oval-def:reference">
						<!--
						<a>
							<xsl:attribute name="target">_blank</xsl:attribute>
							<xsl:attribute name="href"><xsl:value-of select="@ref_url"/></xsl:attribute>
							<xsl:value-of select="@ref_id"/>
						</a>
						-->
						<xsl:value-of select="@ref_id"/>
					</xsl:for-each>&#160;
				</xsl:for-each>&#160;
			</td>

			<!-- title -->
			<td class="text">
				<xsl:for-each select="key('definition-index', @definition_id)">
					<xsl:value-of select="oval-def:metadata/oval-def:title"/>
				</xsl:for-each>&#160;
			</td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
