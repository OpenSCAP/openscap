<?xml version="1.0" encoding="UTF-8" ?>
<!--
Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
     Martin Preisler <mpreisle@redhat.com>
-->


<xsl:stylesheet version="1.1"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns="http://docbook.org/ns/docbook"
    xmlns:sceres="http://open-scap.org/page/SCE_result_file"
    xmlns:xccdf="http://checklists.nist.gov/xccdf/1.1"
    >

<!-- the concat workaround deals with preserved excessive whitespace -->

<xsl:template mode='brief' match='xccdf:check-import'>
  <programlisting><xsl:value-of select='concat("&#10;", text())' /></programlisting>
</xsl:template>

<xsl:template mode='brief' match='sceres:sce_results'>
  <programlisting><xsl:value-of select='concat("&#10;", sceres:stdout/text())' /></programlisting>
</xsl:template>

</xsl:stylesheet>
