<?xml version="1.0" encoding="utf-8" ?>

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
    Martin Preisler <mpreisle@redhat.com>
-->

<xsl:stylesheet version="1.1"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:ovalres="http://oval.mitre.org/XMLSchema/oval-results-5"
    xmlns:ovalsys="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5"
    xmlns:ovalunixsc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix"
    xmlns:ovalindsc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent"
    exclude-result-prefixes="xsl ovalres ovalsys ovalunixsc ovalindsc">

<xsl:key name='oval-definition' match='ovalres:definition'    use='@definition_id' />
<xsl:key name='oval-test'       match='ovalres:test'          use='@test_id'       />
<xsl:key name='oval-items'      match='ovalsys:system_data/*' use='@id'            />

<xsl:key name='oval-testdef' match='*[starts-with(namespace-uri(), "http://oval.mitre.org/XMLSchema/oval-definitions") and contains(local-name(), "_test")]' use='@id' />
<xsl:key name='oval-objectdef' match='*[starts-with(namespace-uri(), "http://oval.mitre.org/XMLSchema/oval-definitions") and contains(local-name(), "_object")]' use='@id' />
<xsl:key name='oval-statedef' match='*[starts-with(namespace-uri(), "http://oval.mitre.org/XMLSchema/oval-definitions") and contains(local-name(), "_state")]' use='@id' />
<xsl:key name='oval-variable' match='*[starts-with(namespace-uri(), "http://oval.mitre.org/XMLSchema/oval-results-5") and contains(local-name(), "tested_variable")]' use='@variable_id' />
<xsl:key name='ovalsys-object' match='*[starts-with(namespace-uri(), "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5") and contains(local-name(), "object")]' use='@id' />

<xsl:template mode='brief' match='ovalres:oval_results'>
    <xsl:param name='definition-id' />
    <xsl:param name='result'/>
    <xsl:apply-templates select='key("oval-definition", $definition-id)' mode='brief'>
        <xsl:with-param name='result' select='$result'/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template mode='brief' match='ovalres:definition|ovalres:criteria|ovalres:criterion|ovalres:extend_definition'>
    <xsl:param name='result'/>
    <xsl:apply-templates select='key("oval-test", @test_ref)' mode='brief'>
        <xsl:with-param name='title' select='key("oval-testdef", @test_ref)/@comment'/>
        <xsl:with-param name='result' select='$result'/>
    </xsl:apply-templates>
    <!-- descend deeper into the logic formula -->
    <xsl:apply-templates mode='brief'>
        <xsl:with-param name='result' select='$result'/>
    </xsl:apply-templates>
</xsl:template>

<!-- OVAL items dump -->
<xsl:template mode='brief' match='ovalres:test'>
    <xsl:param name='title'/>
    <xsl:param name='result'/>
    <xsl:variable name='items' select='ovalres:tested_item'/>
    <xsl:choose>
        <!-- if there are items to display, go ahead -->
        <xsl:when test='$items'>
            <h4>
                <span class="label label-primary">
                    <xsl:choose>
                        <xsl:when test='$title'><xsl:value-of select='$title'/></xsl:when>
                        <xsl:otherwise>OVAL test <xsl:value-of select='@test_id'/></xsl:otherwise>
                    </xsl:choose>
                </span><!-- #160 is nbsp -->&#160;
                <xsl:choose>
                    <xsl:when test='$result="pass"'><span class="label label-success">passed</span> because of these items:</xsl:when>
                    <xsl:otherwise><span class="label label-danger">failed</span> because these items were missing:</xsl:otherwise>
                </xsl:choose>
            </h4>

            <table class="table table-striped table-bordered">
                <!-- table head (possibly item-type-specific) -->
                <thead>
                    <xsl:apply-templates mode='item-head' select='key("oval-items", $items[1]/@item_id)'/>
                </thead>

                <!-- table body (possibly item-type-specific) -->
                <!-- limited to 100 lines -->
                <tbody>
                    <xsl:for-each select='$items'>
                        <xsl:if test="not(position() > 100)">
                            <xsl:for-each select='key("oval-items", @item_id)'>
                                <xsl:apply-templates select='.' mode='item-body'/>
                            </xsl:for-each>
                        </xsl:if>
                    </xsl:for-each>
                </tbody>
            </table>
            <xsl:if test="count($items) > 100">
                ... and <xsl:value-of select="count($items)-100"/> more items.
            </xsl:if>
        </xsl:when>
        <xsl:otherwise>
            <!-- Applies when tested object doesn't exist or an error occured
                 while acessing object (permission denied etc.) -->
            <xsl:variable name='object_id' select='key("oval-testdef", @test_id)/*[local-name()="object"]/@object_ref'/>
            <xsl:variable name='object_info' select='key("oval-objectdef",$object_id)'/>
            <xsl:variable name='state_id' select='key("oval-testdef", @test_id)/*[local-name()="state"]/@state_ref'/>
            <xsl:variable name='state_info' select='key("oval-statedef",$state_id)'/>
            <xsl:variable name='comment' select='$object_info[1]/@comment'/>
            <xsl:if test="$object_info">
                <h4>
                    <span class="label label-primary"><xsl:value-of select="$title"/></span><!-- #160 is nbsp -->&#160;
                    <xsl:choose>
                        <xsl:when test='$result="pass"'><span class="label label-success">passed</span> because these items were not found:</xsl:when>
                        <xsl:otherwise><span class="label label-danger">failed</span> because these items were missing:</xsl:otherwise>
                    </xsl:choose>
                </h4>
                <h5>Object <strong><abbr>
                <xsl:if test='$comment'>
                    <xsl:attribute name='title'>
                        <xsl:value-of select='$object_info[1]/@comment'/>
                    </xsl:attribute>
                </xsl:if>
                <xsl:value-of select='$object_id'/></abbr></strong> of type
                <strong><xsl:value-of select='local-name($object_info)'/></strong></h5>
                <table class="table table-striped table-bordered">
                    <thead>
                        <xsl:apply-templates mode='item-head' select='$object_info[1]'/>
                    </thead>
                    <tbody>
                        <tr>
                            <xsl:variable name='variable_id' select='$object_info/*/@var_ref'/>
                            <xsl:if test='$variable_id'>
                                <td>
                                    <xsl:choose>
                                        <xsl:when test='count(ovalres:tested_variable)>1'>
                                            <table>
                                                <xsl:apply-templates mode='tableintable' select='ovalres:tested_variable'/>
                                            </table>
                                        </xsl:when>
                                        <xsl:when test='count(ovalres:tested_variable)=1'>
                                            <xsl:apply-templates mode='normal' select='ovalres:tested_variable'/>
                                        </xsl:when>
                                    </xsl:choose>
                                    <xsl:apply-templates mode='message' select='key("ovalsys-object",$object_id)'/>
                                </td>
                            </xsl:if>
                            <xsl:apply-templates mode='object' select='$object_info[1]'/>
                        </tr>
                    </tbody>
                </table>
                <xsl:if test="$state_info">
                    <h5>State <strong><xsl:value-of select='$state_id'/></strong> of type
                    <strong><xsl:value-of select='local-name($state_info)'/></strong></h5>
                    <table class="table table-striped table-bordered">
                        <thead>
                            <xsl:apply-templates mode='item-head' select='$state_info[1]'/>
                        </thead>
                        <tbody>
                            <tr>
                                <xsl:variable name='variable_id' select='$state_info/*/@var_ref'/>
                                <xsl:if test='$variable_id'>
                                    <td>
                                        <xsl:apply-templates mode='normal' select='ovalres:tested_variable'/>
                                        <xsl:apply-templates mode='message' select='key("ovalsys-object",$object_id)'/>
                                    </td>
                                </xsl:if>
                                <xsl:apply-templates mode='state' select='$state_info[1]'/>
                            </tr>
                       </tbody>
                    </table>
                </xsl:if>
            </xsl:if>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template mode='normal' match='ovalres:tested_variable'>
    <xsl:if test='* or normalize-space()'>
        <xsl:value-of select='.'/>
    </xsl:if>
</xsl:template>

<xsl:template mode='tableintable' match='ovalres:tested_variable'>
    <xsl:if test='* or normalize-space()'>
        <tr>
            <td>
                <xsl:value-of select='.'/>
            </td>
        </tr>
    </xsl:if>
</xsl:template>

<xsl:template mode='object' match='*[starts-with(namespace-uri(), "http://oval.mitre.org/XMLSchema/oval-definitions") and contains(local-name(), "_object")]'>
    <xsl:for-each select='*'>
        <xsl:if test='not(*) and not(normalize-space()) and not(@var_ref)'><td>no value</td></xsl:if>
        <xsl:if test='* or normalize-space()'>
            <td>
                <xsl:value-of select='.'/>
            </td>
        </xsl:if>
    </xsl:for-each>
</xsl:template>

<xsl:template mode='state' match='*[starts-with(namespace-uri(), "http://oval.mitre.org/XMLSchema/oval-definitions") and contains(local-name(), "_state")]'>
    <xsl:for-each select='*'>
        <xsl:if test='* or normalize-space()'>
            <td>
                <xsl:value-of select='.'/>
            </td>
        </xsl:if>
    </xsl:for-each>
</xsl:template>

<xsl:template mode='message' match='ovalsys:object'>
    <xsl:if test='ovalsys:message'>
        <xsl:value-of select='ovalsys:message'/>
    </xsl:if>
</xsl:template>

<!-- unmatched node visualisation (i.e. not displayed) -->

<xsl:template mode='item-head' match='node()' priority='-5'/>
<xsl:template mode='item-body' match='node()' priority='-5'/>

<!-- generic item visualisation -->

<xsl:template mode='item-head' match='*'>
    <tr>
        <xsl:for-each select='*'>
            <xsl:variable name='label' select='translate(local-name(), "_", " ")'/>
            <xsl:variable name='first_letter' select='translate(substring($label,1,1), "abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZ")'/>
            <xsl:variable name='rest' select='substring($label,2)'/>
            <th><xsl:value-of select='concat($first_letter, $rest)'/></th>
        </xsl:for-each>
    </tr>
</xsl:template>

<xsl:template mode='item-body' match='*'>
    <tr>
        <xsl:for-each select='*'>
            <td>
                <xsl:if test='@datatype="int" or @datatype="boolean"'><xsl:attribute name='role'>num</xsl:attribute></xsl:if>
                <xsl:value-of select='.'/>
            </td>
        </xsl:for-each>
    </tr>
</xsl:template>

<!-- UNIX file item visualisation -->

<xsl:template mode='item-head' match='ovalunixsc:file_item'>
    <tr><th>Path</th><th>Type</th><th>UID</th><th>GID</th><th>Size (B)</th><th>Permissions</th></tr>
</xsl:template>

<xsl:template mode='item-body' match='ovalunixsc:file_item'>
    <xsl:variable name='path' select='concat(ovalunixsc:path, "/", ovalunixsc:filename)'/>
    <tr>
        <td><xsl:value-of select='$path'/></td>
        <td><xsl:value-of select='ovalunixsc:type'/></td>
        <td><xsl:value-of select='ovalunixsc:user_id'/></td>
        <td><xsl:value-of select='ovalunixsc:group_id'/></td>
        <td><xsl:value-of select='ovalunixsc:size'/></td>
        <!-- permissions output -->
        <td>
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
        </td>
    </tr>
</xsl:template>

<xsl:template mode='permission' match='*'>
    <xsl:choose>
        <xsl:when test='string(.)="true"'><xsl:value-of select='translate(substring(local-name(),2,1), "e", "x")'/></xsl:when>
        <xsl:otherwise>-</xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- textfilecontent visualisation -->

<xsl:template mode='item-head' match='ovalindsc:textfilecontent_item'>
    <tr><th>Path</th><th>Content</th></tr>
</xsl:template>

<xsl:template mode='item-body' match='ovalindsc:textfilecontent_item'>
    <xsl:variable name='path' select='concat(ovalindsc:path, "/", ovalindsc:filename)'/>
    <tr><td><xsl:value-of select='$path'/></td><td><xsl:value-of select='ovalindsc:text'/></td></tr>
</xsl:template>

</xsl:stylesheet>
