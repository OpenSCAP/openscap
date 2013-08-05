<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xsl:stylesheet [
<!-- check symbol -->
<!ENTITY resultgood "&#x2713;">
<!-- x symbol -->
<!ENTITY resultbad "&#x2715;">
]>
<!--

****************************************************************************************
 Copyright (c) 2002-2012, The MITRE Corporation
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

	Modified by David Rothenberg, The Mitre Corporation
		* Updated CSS style, updated groupings based on positive/negative implication rather than OVAL result enumeration
	DATE: 24 September 2012

	Modified by Simon Lukasik, Red Hat, Inc.
		* Removed overabundant whitespaces
	DATE: 05 August 2013

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
	xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:linux-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux"
	exclude-result-prefixes="oval oval-def oval-res oval-sc ind-def windows-def unix-def linux-def apache-def">
	<xsl:output method="xml" indent="yes" omit-xml-declaration="no" doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"/>
	<xsl:key name="definition-index" use="@id" match="//oval-def:oval_definitions/oval-def:definitions/oval-def:definition"/>

	<!-- Style declarations for pretty formatting -->
	<xsl:template name="oval_res_style">
		<style type="text/css">
            table { border: 1px solid #000000; width: 100%; border-spacing: 0px; margin: 2px 0px;}
            .noborder {border: none;}
            .nomargin {margin: 0px;}
            td { padding: 0px 4px 1px 4px;}
            .SmallLabel { font-family: Geneva, Arial, Helvetica, sans-serif; color: #000000; font-size: 9pt; font-weight: bold; white-space: nowrap;}
            .SmallText { font-family: Geneva, Arial, Helvetica, sans-serif; color: #000000; font-size: 9pt;}
            .Label { font-family: Geneva, Arial, Helvetica, sans-serif; color: #000000; font-size: 10pt; font-weight: bold; white-space: nowrap;}
            .TitleLabel { font-family: Geneva, Arial, Helvetica, sans-serif; color: #ffffff; font-size: 10pt; font-weight: bold; white-space: nowrap;}
            .Text { font-family: Geneva, Arial, Helvetica, sans-serif; color: #000000; font-size: 10pt;}
            .Title { color: #FFFFFF; background-color: #706c60; padding: 0px 4px 1px 4px; font-size: 10pt; border-bottom: 1px solid #000000;}
            .Center { text-align: center;}

            a { color:#676c63;}
            a.Hover:hover { color:#7b0e0e; text-decoration:underline;}

            .LightRow { background-color: #FFFFFF;}
            .DarkRow { background-color: #DDDDD8;}

            .resultbadA{background-color: #FFBC8F;}
            .resultbadB{background-color: #FFE0CC;}
            .resultgoodA{background-color: #ACD685;}
            .resultgoodB{background-color: #CBE6B3;}
            .unknownA{background-color: #AEC8E0;}
            .unknownB{background-color: #DAE6F1;}
            .errorA{background-color: #FFDD75;}
            .errorB{background-color: #FFECB3;}
            .otherA{background-color: #EEEEEE;}
            .otherB{background-color: #FFFFFF;}

            .Classcompliance{background-color: #93C572;}
            .Classinventory{background-color: #AEC6CF;}
            .Classmiscellaneous{background-color: #9966CC;}
            .Classpatch{background-color: #FFDD75;}
            .Classvulnerability{background-color: #FF9966;}
            .ColorBox{width: 2px;}
		</style>
	</xsl:template>

	<!-- Render the legend used to determine what row colors represent -->
	<xsl:template name="ResultColorTable">
		<table class="noborder nomargin" style="width:auto;">
			<tr>
				<td>
					<table border="1">
						<tr class="LightRow">
							<td class="resultbadA ColorBox"/>
							<td class="resultbadB ColorBox"/>
							<td class="Text" title="Non-Compliant/Vulnerable/Unpatched">&resultbad;</td>
						</tr>
					</table>
				</td>
				<td>
					<table border="1">
						<tr class="LightRow">
							<td class="resultgoodA ColorBox"/>
							<td class="resultgoodB ColorBox"/>
							<td class="Text" title="Compliant/Non-Vulnerable/Patched">&resultgood;</td>
						</tr>
					</table>
				</td>
				<td>
					<table border="1">
						<tr class="LightRow">
							<td class="errorA ColorBox"/>
							<td class="errorB ColorBox"/>
							<td class="Text">Error</td>
						</tr>
					</table>
				</td>
				<td>
					<table border="1">
						<tr class="LightRow">
							<td class="unknownA ColorBox"/>
							<td class="unknownB ColorBox"/>
							<td class="Text">Unknown</td>
						</tr>
					</table>
				</td>
				<td>
					<table border="1">
						<tr class="LightRow">
							<td class="DarkRow ColorBox"/>
							<td class="LightRow ColorBox"/>
							<td class="Text" title="Inventory/Miscellaneous class, or Not Applicable/Not Evaluated result">Other</td>
						</tr>
					</table>
				</td>
			</tr>
		</table>
	</xsl:template>

	<!-- Gets the top level node -->
	<xsl:template match="oval-res:oval_results">
		<!-- Put a break after the XML encoding declaration, then insert the color chart into the source -->
<xsl:text>
</xsl:text>
<xsl:comment>
		Color mapping
		Result enumeration
	Class       |t|f|u|e|na|ne|
	compliance  |G|R|B|Y|- |- |
	inventory   |-|-|B|Y|- |- |
	misc        |-|-|B|Y|- |- |
	patch       |R|G|B|Y|- |- |
	vuln        |R|G|B|Y|- |- |

	R = red
	G = green
	B = blue
	Y = yellow
	- = grey
</xsl:comment>
		<html>
			<head>
				<meta http-equiv="content-type" content="text/html; charset=UTF-8"/>
				<title>OVAL Results</title>
				<!-- Get CSS stylesheet -->
				<xsl:call-template name="oval_res_style"/>
			</head>
			<body>
				<!-- display results and definition generator information -->
				<table class="noborder nomargin">
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
				<hr />
				<!--
					create anchors to each system in the results file
					if only one system leave out the anchors
				-->
				<!-- 2007-08-20 Added aggregate result detail-->
				<xsl:if test="not(count(./oval-res:results/oval-res:system) = 1)">
					<table border="1">
						<tr class="Title">
							<td class="TitleLabel" align="center">Systems Analyzed</td>
							<td class="TitleLabel" align="center" title="Non-Compliant/Vulnerable/Unpatched">&resultbad;</td>
							<td class="TitleLabel" align="center" title="Compliant/Non-Vulnerable/Patched">&resultgood;</td>
							<td class="TitleLabel" align="center">Errors</td>
							<td class="TitleLabel" align="center">Unknown</td>
							<td class="TitleLabel" align="center" title="Inventory/Miscellaneous class, or Not Applicable/Not Evaluated result">Other</td>
						</tr>
						<xsl:for-each select="./oval-res:results/oval-res:system">
							<xsl:variable name="systemMod2">
								<xsl:choose>
									<xsl:when test="position() mod 2 = 1">A</xsl:when>
									<xsl:otherwise>B</xsl:otherwise>
								</xsl:choose>
							</xsl:variable>
							<tr>
								<xsl:choose>
									<xsl:when test="position() mod 2 = 1">
										<xsl:attribute name="class">DarkRow</xsl:attribute>
									</xsl:when>
									<xsl:when test="position() mod 2 = 0">
										<xsl:attribute name="class">LightRow</xsl:attribute>
									</xsl:when>
								</xsl:choose>
								<td class="Label">
									<a class="Hover" href="#{concat('a_',position())}">
										<xsl:value-of select="./oval-sc:oval_system_characteristics/oval-sc:system_info/oval-sc:primary_host_name"/>
									</a>
								</td>
								<td width="10">
									<xsl:attribute name="class"><xsl:value-of select="concat('resultbad',$systemMod2,' Text')"/></xsl:attribute>
									<xsl:value-of select="count(oval-res:definitions/oval-res:definition[@result='true'][key('definition-index', ./@definition_id)[@class='patch' or @class='vulnerability']]|oval-res:definitions/oval-res:definition[@result='false'][key('definition-index', ./@definition_id)[@class='compliance']])"/>
								</td>
								<td width="10">
									<xsl:attribute name="class"><xsl:value-of select="concat('resultgood',$systemMod2,' Text')"/></xsl:attribute>
									<xsl:value-of select="count(oval-res:definitions/oval-res:definition[@result='false'][key('definition-index', ./@definition_id)[@class='patch' or @class='vulnerability']]|oval-res:definitions/oval-res:definition[@result='true'][key('definition-index', ./@definition_id)[@class='compliance']])"/>
								</td>
								<td width="10">
									<xsl:attribute name="class"><xsl:value-of select="concat('error',$systemMod2,' Text')"/></xsl:attribute>
									<xsl:value-of select="count(./oval-res:definitions/oval-res:definition[@result='error'])"/>
								</td>
								<td width="10">
									<xsl:attribute name="class"><xsl:value-of select="concat('unknown',$systemMod2,' Text')"/></xsl:attribute>
									<xsl:value-of select="count(./oval-res:definitions/oval-res:definition[@result='unknown'])"/>
								</td>
								<td width="10">
									<xsl:attribute name="class"><xsl:value-of select="concat('other',$systemMod2,' Text')"/></xsl:attribute>
									<xsl:value-of select="count(oval-res:definitions/oval-res:definition[@result='not applicable' or @result='not evaluated']|oval-res:definitions/oval-res:definition[@result='true' or @result='false'][key('definition-index', ./@definition_id)[@class='inventory' or @class='miscellaneous']])"/>
								</td>
							</tr>
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
					<xsl:if test="not(position() = last())"><hr /></xsl:if>
				</xsl:for-each>
			</body>
		</html>
	</xsl:template>

	<!-- Get the system_info and put it into a table. -->
	<xsl:template name="SystemInfo">
		<xsl:param name="sysInfoElm"/>
		<!-- Create page anchor to this system, will be used if multiple systems present -->
		<a class="Hover" name="{concat('a_',position())}" id="{concat('a_',position())}" style="text-decoration:none;"/>
		<table border="1">
			<tr class="Title">
				<td class="TitleLabel" colspan="2">System Information</td>
			</tr>
			<tr class="DarkRow">
				<td class="Label" width="20%">Host Name</td>
				<td class="Text" width="80%"><xsl:value-of select="$sysInfoElm/child::oval-sc:primary_host_name"/></td>
			</tr>
			<tr class="LightRow">
				<td class="Label" width="20%">Operating System</td>
				<td class="Text" width="80%"><xsl:value-of select="$sysInfoElm/child::oval-sc:os_name"/></td>
			</tr>
			<tr class="DarkRow">
				<td class="Label" width="20%">Operating System Version</td>
				<td class="Text" width="80%"><xsl:value-of select="$sysInfoElm/child::oval-sc:os_version"/></td>
			</tr>
			<tr class="LightRow">
				<td class="Label" width="20%">Architecture</td>
				<td class="Text" width="80%"><xsl:value-of select="$sysInfoElm/child::oval-sc:architecture"/></td>
			</tr>
			<tr class="DarkRow">
				<td class="Label" width="20%">Interfaces</td>
				<td width="80%">
					<xsl:call-template name="Interfaces">
						<xsl:with-param name="iterfacesElm" select="$sysInfoElm/oval-sc:interfaces"/>
					</xsl:call-template>
				</td>
			</tr>
		</table>
	</xsl:template>

	<!-- Loop through all possible interfaces and put it into the table -->
	<xsl:template name="Interfaces">
		<xsl:param name="iterfacesElm"/>
		<xsl:for-each select="$iterfacesElm/oval-sc:interface">
			<xsl:variable name="interfaceMod2">
				<xsl:choose>
					<xsl:when test="position() mod 2 = 1">LightRow</xsl:when>
					<xsl:otherwise>DarkRow</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<table border="1">
				<tr class="{$interfaceMod2}">
					<td class="Label" width="20%">Interface Name</td>
					<td class="Text" width="80%"><xsl:value-of select="./oval-sc:interface_name"/></td>
				</tr>
				<tr class="{$interfaceMod2}">
					<td class="Label" width="20%">IP Address</td>
					<td class="Text" width="80%"><xsl:value-of select="./oval-sc:ip_address"/></td>
				</tr>
				<tr class="{$interfaceMod2}">
					<td class="Label" width="20%">MAC Address</td>
					<td class="Text" width="80%"><xsl:value-of select="./oval-sc:mac_address"/></td>
				</tr>
			</table>
		</xsl:for-each>
	</xsl:template>

	<!-- Create a table for the Result Generator and call its children. -->
	<xsl:template name="ResultGenerator">
		<xsl:param name="generatorElm"/>
		<table border="1">
			<tr class="Title">
			    <td class="TitleLabel" colspan="5">OVAL Results Generator Information</td>
			</tr>
			<xsl:call-template name="Generator">
      			<xsl:with-param name="generatorElm" select="$generatorElm"/>
      		</xsl:call-template>
            <xsl:call-template name="GeneratorResTotals">
                <xsl:with-param name="resultsElm" select="/oval-res:oval_results/oval-res:results"/>
            </xsl:call-template>
		</table>
	</xsl:template>

	<!-- Create a table for the System Characteristics Generator and call its children. -->
	<xsl:template name="SCGenerator">
		<xsl:param name="generatorElm"/>
		<table border="1">
			<tr class="Title">
				<td class="TitleLabel" colspan="5">OVAL System Characteristics Generator Information</td>
			</tr>
			<xsl:call-template name="Generator">
				<xsl:with-param name="generatorElm" select="$generatorElm"/>
			</xsl:call-template>
		</table>
	</xsl:template>

	<!-- Create a table for the Definitions Generator and call its children. -->
	<xsl:template name="DefGenerator">
		<xsl:param name="generatorElm"/>
		<table border="1">
		    <tr class="Title">
		        <td class="TitleLabel" colspan="5">OVAL Definition Generator Information</td>
			</tr>
		    <xsl:call-template name="Generator">
      			<xsl:with-param name="generatorElm" select="$generatorElm"/>
		    </xsl:call-template>
		    <xsl:call-template name="GeneratorDefTotals">
	            <xsl:with-param name="definitionsElm" select="/oval-res:oval_results/oval-def:oval_definitions"/>
	        </xsl:call-template>
		</table>
	</xsl:template>

	<!-- Each child of Generators (oval, system_characteristics, results) is the same, this template gets their children. -->
    <xsl:template name="Generator">
        <xsl:param name="generatorElm"/>
        <xsl:variable name="MessyNumber" select="string($generatorElm/oval:timestamp)"/>
        <tr class="DarkRow Center">
            <td class="SmallLabel">Schema Version</td>
        	<td class="SmallLabel">Product Name</td>
        	<td class="SmallLabel">Product Version</td>
        	<td class="SmallLabel">Date</td>
        	<td class="SmallLabel">Time</td>
        </tr>
        <tr class="LightRow">
            <td class="SmallText"><xsl:value-of select="$generatorElm/oval:schema_version"/></td>
        	<td class="SmallText"><xsl:value-of select="$generatorElm/oval:product_name"/></td>
        	<td class="SmallText"><xsl:value-of select="$generatorElm/oval:product_version"/></td>
        	<td class="SmallText">
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
        	<td class="SmallText">
                <xsl:value-of select="substring($MessyNumber, 12, 2)"/>
                <xsl:text>:</xsl:text>
                <!-- hour -->
                <xsl:value-of select="substring($MessyNumber, 15, 2)"/>
                <xsl:text>:</xsl:text>
                <!-- minute -->
                <xsl:value-of select="substring($MessyNumber, 18, 2)"/> <!-- second -->
            </td>
        </tr>
    </xsl:template>

	<!-- Add rows to the OVAL Definitions generator to supply aggregate data. -->
    <xsl:template name="GeneratorDefTotals">
        <xsl:param name="definitionsElm"/>
        <tr class="DarkRow Center">
        	<td class="SmallLabel" style="width: 20%;">#Definitions</td>
        	<td class="SmallLabel" style="width: 20%;">#Tests</td>
        	<td class="SmallLabel" style="width: 20%;">#Objects</td>
        	<td class="SmallLabel" style="width: 20%;">#States</td>
        	<td class="SmallLabel" style="width: 20%;">#Variables</td>
        </tr>
        <tr class="LightRow Center">
        	<td class="SmallText Center">
        		<xsl:value-of select="concat(count($definitionsElm/oval-def:definitions/oval-def:definition),' Total')"/><br />
                <xsl:if test="$definitionsElm/oval-def:definitions/oval-def:definition[@class]">
                	<table class="noborder">
                        <tr class="Center">
                        	<td class="SmallText Classcompliance" title="compliance" style="width:20%"><xsl:value-of select="count($definitionsElm/oval-def:definitions/oval-def:definition[@class='compliance'])"/></td>
                        	<td class="SmallText Classinventory" title="inventory" style="width:20%"><xsl:value-of select="count($definitionsElm/oval-def:definitions/oval-def:definition[@class='inventory'])"/></td>
                        	<td class="SmallText Classmiscellaneous" title="miscellaneous" style="width:20%"><xsl:value-of select="count($definitionsElm/oval-def:definitions/oval-def:definition[@class='miscellaneous'])"/></td>
                        	<td class="SmallText Classpatch" title="patch" style="width:20%"><xsl:value-of select="count($definitionsElm/oval-def:definitions/oval-def:definition[@class='patch'])"/></td>
                        	<td class="SmallText Classvulnerability" title="vulnerability" style="width:20%"><xsl:value-of select="count($definitionsElm/oval-def:definitions/oval-def:definition[@class='vulnerability'])"/></td>
                        </tr>
                    </table>
                </xsl:if>
            </td>
        	<td class="SmallText Center">
                <xsl:value-of select="count($definitionsElm/oval-def:tests/*)"/>
            </td>
        	<td class="SmallText Center">
                <xsl:value-of select="count($definitionsElm/oval-def:objects/*)"/>
            </td>
        	<td class="SmallText Center">
                <xsl:value-of select="count($definitionsElm/oval-def:states/*)"/>
            </td>
        	<td class="SmallText Center">
                <xsl:value-of select="count($definitionsElm/oval-def:variables/*)"/>
            </td>
        </tr>
    </xsl:template>

	<!-- Add rows to the OVAL Results generator to supply aggregate data. -->
    <xsl:template name="GeneratorResTotals">
        <xsl:param name="resultsElm"/>
        <tr class="DarkRow Center">
        	<td class="SmallLabel" style="width: 20%;" title="Non-Compliant/Vulnerable/Unpatched">#&resultbad;</td>
        	<td class="SmallLabel" style="width: 20%;" title="Compliant/Non-Vulnerable/Patched">#&resultgood;</td>
        	<td class="SmallLabel" style="width: 20%;" title="Error">#Error</td>
        	<td class="SmallLabel" style="width: 20%;" title="Unknown">#Unknown</td>
        	<td class="SmallLabel" style="width: 20%;" title="Inventory/Miscellaneous class, or Not Applicable/Not Evaluated result">#Other</td>
        </tr>
        <tr class="LightRow Center" style="height:auto;">
        	<td class="SmallText resultbadB" title="Non-Compliant/Vulnerable/Unpatched" style="width:20%"><xsl:value-of select="count($resultsElm/oval-res:system/oval-res:definitions/oval-res:definition[@result='true'][key('definition-index', ./@definition_id)[@class='patch' or @class='vulnerability']]|$resultsElm/oval-res:system/oval-res:definitions/oval-res:definition[@result='false'][key('definition-index', ./@definition_id)[@class='compliance']])"/></td>
        	<td class="SmallText resultgoodB" title="Compliant/Non-Vulnerable/Patched" style="width:20%"><xsl:value-of select="count($resultsElm/oval-res:system/oval-res:definitions/oval-res:definition[@result='false'][key('definition-index', ./@definition_id)[@class='patch' or @class='vulnerability']]|$resultsElm/oval-res:system/oval-res:definitions/oval-res:definition[@result='true'][key('definition-index', ./@definition_id)[@class='compliance']])"/></td>
        	<td class="SmallText errorB" title="Error" style="width:20%"><xsl:value-of select="count($resultsElm/oval-res:system/oval-res:definitions/oval-res:definition[@result='error'])"/></td>
        	<td class="SmallText unknownB" title="Unknown" style="width:20%"><xsl:value-of select="count($resultsElm/oval-res:system/oval-res:definitions/oval-res:definition[@result='unknown'])"/></td>
        	<td class="SmallText otherB" title="Inventory/Miscellaneous class, or Not Applicable/Not Evaluated result" style="width:20%"><xsl:value-of select="count($resultsElm/oval-res:system/oval-res:definitions/oval-res:definition[@result='not applicable' or @result='not evaluated']|$resultsElm/oval-res:system/oval-res:definitions/oval-res:definition[@result='true' or @result='false'][key('definition-index', ./@definition_id)[@class='inventory' or @class='miscellaneous']])"/></td>
        </tr>
    </xsl:template>

	<!-- Process a system's definition results in the specified order -->
	<xsl:template name="DefinitionsResults">
		<xsl:param name="definitionsElm"/>
		<table border="1">
			<tr class="Title">
				<td class="TitleLabel" colspan="5">OVAL Definition Results</td>
			</tr>
			<!-- display the result color coding table -->
			<tr class="DarkRow">
				<td colspan="5"><xsl:call-template name="ResultColorTable"/></td>
			</tr>

			<tr class="TitleLabel">
				<td class="Title" align="center">ID</td>
				<td class="Title" align="center">Result</td>
				<td class="Title" align="center">Class</td>
				<td class="Title" align="center">Reference ID</td>
				<td class="Title" align="center">Title</td>
			</tr>

			<!-- process Non-Compliant/Vulnerable/Unpatched results -->
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result='true'][key('definition-index', ./@definition_id)[@class='patch' or @class='vulnerability']]|$definitionsElm/oval-res:definition[@result='false'][key('definition-index', ./@definition_id)[@class='compliance']]">
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

			<!-- process other results -->
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result='not applicable' or @result='not evaluated']|$definitionsElm/oval-res:definition[@result='true' or @result='false'][key('definition-index', ./@definition_id)[@class='inventory' or @class='miscellaneous']]">
				<xsl:sort select="@id" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>

			<!-- process Compliant/Non-Vulnerable/Patched results -->
			<xsl:for-each select="$definitionsElm/oval-res:definition[@result='false'][key('definition-index', ./@definition_id)[@class='patch' or @class='vulnerability']]|$definitionsElm/oval-res:definition[@result='true'][key('definition-index', ./@definition_id)[@class='compliance']]">
				<xsl:sort select="@id" data-type="text" order="descending"/>
				<xsl:call-template name="Definition">
					<xsl:with-param name="definitionElm" select="."/>
				</xsl:call-template>
			</xsl:for-each>
		</table>
	</xsl:template>

	<!-- Add information about a single definition to a new row -->
	<xsl:template name="Definition">
		<xsl:param name="definitionElm"/>
		<xsl:variable name="defClass"><xsl:value-of select="key('definition-index', @definition_id)/@class"/></xsl:variable>
		<xsl:variable name="defResult"><xsl:value-of select="$definitionElm/@result"/></xsl:variable>
		<tr>
			<!-- set results to alternating colors -->
			<xsl:variable name="class_prefix">
				<xsl:choose>
					<xsl:when test="(($defResult='true') and (($defClass='patch') or ($defClass='vulnerability'))) or (($defResult='false') and ($defClass='compliance'))">resultbad</xsl:when>
					<xsl:when test="(($defResult='false') and (($defClass='patch') or ($defClass='vulnerability'))) or (($defResult='true') and ($defClass='compliance'))">resultgood</xsl:when>
					<xsl:when test="$defResult='unknown'">unknown</xsl:when>
					<xsl:when test="$defResult='error'">error</xsl:when>
					<xsl:otherwise>other</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="classMod2">
				<xsl:choose>
					<xsl:when test="position() mod 2 = 1">A</xsl:when>
					<xsl:otherwise>B</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>

			<xsl:attribute name="class"><xsl:value-of select="concat($class_prefix,$classMod2)"/></xsl:attribute>

			<!-- id -->
			<td class="Text" align="center">
				<xsl:choose>
					<!-- if the id is an oval repository id add a link otherwise don't -->
					<xsl:when test="starts-with($definitionElm/@definition_id, 'oval:org.mitre.oval:def:')">
						<xsl:variable name="idUrl" select="concat('http://oval.mitre.org/repository/data/getDef?id=', $definitionElm/@definition_id)"/>
						<a class="Hover" target="_blank" href="{$idUrl}"><xsl:value-of select="$definitionElm/@definition_id"/></a>
					</xsl:when>
					<xsl:otherwise><xsl:value-of select="$definitionElm/@definition_id"/></xsl:otherwise>
				</xsl:choose>
			</td>

			<!-- result -->
			<td class="Text" align="center">
				<xsl:value-of select="$defResult"/>
			</td>

			<!-- Class -->
			<td class="Text" align="center">
				<xsl:value-of select="$defClass"/>
			</td>

			<!-- reference id -->
			<td class="Text" align="center">
				<xsl:for-each select="key('definition-index', @definition_id)/oval-def:metadata/oval-def:reference">
					<xsl:text>[</xsl:text>
					<!-- only display as a link if reference has a URL -->
					<xsl:choose>
						<xsl:when test="@ref_url"><a class="Hover" target="_blank" href="{@ref_url}"><xsl:value-of select="@ref_id"/></a></xsl:when>
						<xsl:otherwise><xsl:value-of select="@ref_id"/></xsl:otherwise>
					</xsl:choose>
					<xsl:text>]</xsl:text>
					<xsl:if test="not(position() = last())"><xsl:text>, </xsl:text></xsl:if>
				</xsl:for-each>
			</td>

			<!-- title -->
			<td class="Text">
				<xsl:value-of select="key('definition-index', @definition_id)/oval-def:metadata/oval-def:title"/>
			</td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
