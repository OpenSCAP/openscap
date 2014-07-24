<?xml version="1.0" encoding="utf-8" ?>

<!--
Copyright 2010 - 2014 Red Hat Inc., Durham, North Carolina.
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
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:cdf="http://checklists.nist.gov/xccdf/1.2">

<xsl:template name="rule-result-tooltip">
    <xsl:param name="ruleresult"/>
    <!-- The texts are sourced from XCCDF 1.2 specification with minor modifications -->
    <xsl:choose>
        <xsl:when test="$ruleresult = 'pass'">The target system or system component satisfied all the conditions of the rule.</xsl:when>
        <xsl:when test="$ruleresult = 'fixed'">The Rule had failed, but was then fixed (possibly by a tool that can automatically apply remediation, or possibly by the human auditor).</xsl:when>
        <xsl:when test="$ruleresult = 'informational'">The Rule was checked, but the output from the checking engine is simply information for auditors or administrators; it is not a compliance category. This status value is designed for Rule elements whose main purpose is to extract information from the target rather than test the target.</xsl:when>

        <xsl:when test="$ruleresult = 'fail'">The target system or system component did not satisfy at least one condition of the rule.</xsl:when>
        <xsl:when test="$ruleresult = 'error'">The checking engine could not complete the evaluation, therefore the status of the target's compliance with the rule is not certain. This could happen, for example, if a testing tool was run with insufficient privileges and could not gather all of the necessary information.</xsl:when>
        <xsl:when test="$ruleresult = 'unknown'">The testing tool encountered some problem and the result is unknown. For example, a result of 'unknown' might be given if the testing tool was unable to interpret the output of the checking engine (the output has no meaning to the testing tool).</xsl:when>

        <xsl:when test="$ruleresult = 'notchecked'">The Rule was not evaluated by the checking engine. This status is designed for Rule elements that have no check elements or that correspond to an unsupported checking system. It may also correspond to a status returned by a checking engine if the checking engine does not support the indicated check code.</xsl:when>
        <xsl:when test="$ruleresult = 'notselected'">The Rule was not selected in the evaluation. This may be caused by the rule not being selected by default in the benchmark or by the profile unselecting it.</xsl:when>
        <xsl:when test="$ruleresult = 'notapplicable'">The Rule was not applicable to the target of the test. For example, the Rule might have been specific to a different version of the target OS, or it might have been a test against a platform feature that was not installed.</xsl:when>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>
