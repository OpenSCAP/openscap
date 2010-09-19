<?xml version="1.0" encoding="UTF-8" ?>
<!--
Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
	xmlns:h="http://www.w3.org/1999/xhtml"
    xmlns:exsl="http://exslt.org/common"
    xmlns:edate="http://exslt.org/dates-and-times">

<!-- parametres -->
<xsl:param name='oscap-version'/>
<xsl:param name='verbosity'/>
<xsl:param name='pwd'/>

<!-- variable with current date/time -->
<xsl:variable name='now' select='edate:date-time()'/>
<!-- veriable w/ generator info -->
<xsl:variable name='generator' select="'OpenSCAP stylesheet'"/>

</xsl:stylesheet>
