<?xml version="1.0" encoding="UTF-8"?>
<!-- This schematron has been created based on the original SCAP 1.3 schematron from NIST -->
<sch:schema xmlns:sch="http://purl.oclc.org/dsdl/schematron"
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform" queryBinding="xslt">
  <sch:ns prefix="ai" uri="http://scap.nist.gov/schema/asset-identification/1.1"/>
  <sch:ns prefix="arf" uri="http://scap.nist.gov/schema/asset-reporting-format/1.1"/>
  <sch:ns prefix="cat" uri="urn:oasis:names:tc:entity:xmlns:xml:catalog"/>
  <sch:ns prefix="cpe-dict" uri="http://cpe.mitre.org/dictionary/2.0"/>
  <sch:ns prefix="cpe-lang" uri="http://cpe.mitre.org/language/2.0"/>
  <sch:ns prefix="dc" uri="http://purl.org/dc/elements/1.1/"/>
  <sch:ns prefix="ds" uri="http://scap.nist.gov/schema/scap/source/1.2"/>
  <sch:ns prefix="dsig" uri="http://www.w3.org/2000/09/xmldsig#"/>
  <sch:ns prefix="java" uri="java:gov.nist.secauto.scap.validation.schematron"/>
  <sch:ns prefix="nvd-config" uri="http://scap.nist.gov/schema/feed/configuration/0.1"/>
  <sch:ns prefix="ocil" uri="http://scap.nist.gov/schema/ocil/2.0"/>
  <sch:ns prefix="oval-com" uri="http://oval.mitre.org/XMLSchema/oval-common-5"/>
  <sch:ns prefix="oval-def" uri="http://oval.mitre.org/XMLSchema/oval-definitions-5"/>
  <sch:ns prefix="oval-res" uri="http://oval.mitre.org/XMLSchema/oval-results-5"/>
  <sch:ns prefix="oval-sc" uri="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5"/>
  <sch:ns prefix="rc" uri="http://scap.nist.gov/schema/reporting-core/1.1"/>
  <sch:ns prefix="scap" uri="http://scap.nist.gov/schema/scap/source/1.2"/>
  <sch:ns prefix="scap-con" uri="http://scap.nist.gov/schema/scap/constructs/1.2"/>
  <sch:ns prefix="tmsad" uri="http://scap.nist.gov/schema/xml-dsig/1.0"/>
  <sch:ns prefix="xccdf" uri="http://checklists.nist.gov/xccdf/1.2"/>
  <sch:ns prefix="xcf" uri="nist:scap:xslt:function"/>
  <sch:ns prefix="xinclude" uri="http://www.w3.org/2001/XInclude"/>
  <sch:ns prefix="xlink" uri="http://www.w3.org/1999/xlink"/>
  <sch:ns prefix="xml" uri="http://www.w3.org/XML/1998/namespace"/>
  <sch:ns prefix="xsd" uri="http://www.w3.org/2001/XMLSchema"/>

  <sch:pattern id="scap-general">
    <sch:rule id="scap-general-xccdf-benchmark" context="xccdf:Benchmark">
      <sch:assert id="scap-general-xccdf-benchmark-description" test="xccdf:description">Error: SRC-10-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-lang-required" test="@xml:lang">Error: SRC-2-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-no-check-content-check" test="not(.//xccdf:check-content)">Error: SRC-25-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-val-forbid" test="not(.//xccdf:Value//xccdf:source) and not(.//xccdf:Value//xccdf:complex-value) and not(.//xccdf:Value//xccdf:complex-default) and not(.//xccdf:Value//xccdf:choices//xccdf:complex-choice)">Error: SRC-276-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-version-time-attribute-req" test="xccdf:version/@time">Warning: SRC-3-2|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-no-xinclude" test="not(.//xinclude:include)">Error: SRC-339-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-version-update-req" test=".//xccdf:version[string(@update)]">Warning: SRC-341-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-no-set-complex-value" test="not(.//xccdf:set-complex-value)">Error: SRC-343-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-style" test="@style = 'SCAP_1.3'">Warning: SRC-4-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-metadata-missing" test="xccdf:metadata">Error: SRC-8-2|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-title-1" test="xccdf:title">Error: SRC-9-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-title-3" test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']">Error: SRC-9-3|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-title-2" test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))">Error: SRC-9-3|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-profile" context="xccdf:Profile">
      <sch:assert id="scap-general-xccdf-profile-description" test="xccdf:description">Error: SRC-10-1|xccdf:Profile <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-profile-title-1" test="xccdf:title">Error: SRC-9-1|xccdf:Profile <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-profile-title-3" test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']">Error: SRC-9-3|xccdf:Profile <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-profile-title-2" test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))">Error: SRC-9-3|xccdf:Profile <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-value" context="xccdf:Value">
      <sch:assert id="scap-general-xccdf-value-description" test="xccdf:description">Error: SRC-10-1|xccdf:Value <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-value-title-1" test="xccdf:title">Error: SRC-9-1|xccdf:Value <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-value-title-3" test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']">Error: SRC-9-3|xccdf:Value <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-value-title-2" test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))">Error: SRC-9-3|xccdf:Value <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-group" context="xccdf:Group">
      <sch:assert id="scap-general-xccdf-group-description" test="xccdf:description">Error: SRC-10-1|xccdf:Group <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-no-group-extension" test="not(@extends)">SRC-354-1|xccdf:Group <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-group-title-1" test="xccdf:title">Error: SRC-9-1|xccdf:Group <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-group-title-3" test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']">Error: SRC-9-3|xccdf:Group <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-group-title-2" test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))">Error: SRC-9-3|xccdf:Group <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-rule" context="xccdf:Rule">
      <sch:assert id="scap-general-xccdf-rule-description" test="xccdf:description">Error: SRC-10-1|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-title-1" test="xccdf:title">Error: SRC-9-1|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-title-3" test="count(xccdf:title) &lt;= 1 or xccdf:title[@xml:lang = 'en-US']">Error: SRC-9-3|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-title-2" test="count(xccdf:title) &lt;= 1 or (count(xccdf:title) = count(xccdf:title[@xml:lang]))">Error: SRC-9-3|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-cve-cce-cpe" test="xccdf:ident[@system = 'http://cce.mitre.org' or @system = 'http://cve.mitre.org' or @system = 'http://cpe.mitre.org']">Warning: SRC-251-1|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-attributes-exists" test="@selected and @weight and @role and @severity">Warning: A-26-1|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-metadata" context="xccdf:Benchmark/xccdf:metadata">
      <sch:assert id="scap-general-xccdf-metadata-populated-creator" test="dc:creator/text()">Error: SRC-8-1|xccdf:Benchmark <sch:value-of select="parent::xccdf:Benchmark/@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-metadata-populated-publisher" test="dc:publisher/text()">Error: SRC-8-1|xccdf:Benchmark <sch:value-of select="parent::xccdf:Benchmark/@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-metadata-populated-contributor" test="dc:contributor/text()">Error: SRC-8-1|xccdf:Benchmark <sch:value-of select="parent::xccdf:Benchmark/@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-metadata-populated-source" test="dc:source/text()">Error: SRC-8-1|xccdf:Benchmark <sch:value-of select="parent::xccdf:Benchmark/@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-system-cpe-dict-check" context="cpe-dict:check">
      <sch:assert id="scap-general-system-cpe-dict-check2" test="@system = 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system = 'http://scap.nist.gov/schema/ocil/2'">Error: SRC-118-2|cpe-dict:cpe-item <sch:value-of select="ancestor::cpe-dict:cpe-item[1]/@name"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-system-cpe-lang-check" context="cpe-lang:check-fact-ref">
      <sch:assert id="scap-general-system-cpe-lang-check2" test="@system = 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system = 'http://scap.nist.gov/schema/ocil/2'">Error: SRC-118-3|cpe-lang:platform <sch:value-of select="ancestor::cpe-lang:platform[1]/@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-check" context="xccdf:Rule/xccdf:check">
      <sch:assert id="scap-general-xccdf-check-sys-req" test="@system = 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system = 'http://scap.nist.gov/schema/ocil/2'">Error: SRC-118-1|xccdf:Rule <sch:value-of select="ancestor::xccdf:Rule[1]/@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-check-patches-ref-oval-only" test="not(current()/parent::xccdf:Rule[substring(@id, string-length(@id) - string-length('security_patches_up_to_date') + 1) = 'security_patches_up_to_date']) or current()/@system = 'http://oval.mitre.org/XMLSchema/oval-definitions-5'">Error: SRC-169-2|xccdf:Rule <sch:value-of select="parent::xccdf:Rule/@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-check-content-ref-req" test="xccdf:check-content-ref">Error: SRC-175-1|xccdf:Rule <sch:value-of select="parent::xccdf:Rule/@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-scap-data-stream-collection" context="scap:data-stream-collection ">
      <sch:assert id="scap-general-latest-schematron-rules" test="current()[@schematron-version='1.3']">Error: SRC-330-2|scap:data-stream-collection</sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-scap-content" context="scap:data-stream">
      <sch:assert id="scap-general-scap-content-use-case" test="@use-case = 'CONFIGURATION' or @use-case = 'VULNERABILITY' or @use-case = 'INVENTORY' or @use-case = 'OTHER'">Error: SRC-324-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-oval-def" context="oval-def:definition">
      <sch:assert id="scap-general-oval-def-rule-compliance-cce" test="@class != 'compliance' or oval-def:metadata/oval-def:reference[@source='http://cce.mitre.org' or @source='CCE']">Warning: SRC-207-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-oval-def-vulnerability-cve-ref" test="@class != 'vulnerability' or oval-def:metadata/oval-def:reference[@source='http://cve.mitre.org' or @source='CVE']">Warning: SRC-214-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-oval-generator" context="oval-def:generator/oval-com:schema_version">
      <sch:assert id="scap-general-oval-version" test="@platform or text()='5.3' or text()='5.4' or text()='5.5' or text()='5.6' or text()='5.7' or text()='5.8' or text()='5.9' or text()='5.10' or text()='5.10.1' or text()='5.11' or text()='5.11.1' or text()='5.11.2'">Error: SRC-216-1</sch:assert>
      <sch:assert id="scap-general-oval-platform-version" test="not(@platform) or text()='5.11.1:1.0' or text()='5.11.1:1.1' or text()='5.11.1:1.2' or text()='5.11.2:1.0' or text()='5.11.2:1.1' or text()='5.11.2:1.2'">Error: SRC-216-2</sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-signature-sig" context="dsig:Signature">
      <sch:assert id="scap-general-signature-sig-manifest-req" test="dsig:Object/dsig:Manifest">Error: SRC-284-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-tmsad" test="dsig:Object/dsig:SignatureProperties/dsig:SignatureProperty/tmsad:signature-info">Error: SRC-285-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-first-ref-ds" test="ancestor::ds:data-stream-collection[1]/ds:data-stream[concat('#',@id) = current()/dsig:SignedInfo/dsig:Reference[1]/@URI]">Error: SRC-286-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-ref-sig-prop" test="dsig:Object/dsig:SignatureProperties[concat('#',@Id) = current()/dsig:SignedInfo/dsig:Reference[2]/@URI]">Error: SRC-287-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-third-ref-manifest" test="dsig:Object/dsig:Manifest[concat('#',@Id) = current()/dsig:SignedInfo/dsig:Reference[3]/@URI]">Error: SRC-288-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-key-info" test="dsig:KeyInfo">Warning: SRC-290-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-status-rule-value-date" context="xccdf:status">
      <sch:assert id="scap-general-xccdf-status-rule-value-date-1" test="text()='draft' or text()= 'accepted'">Error: SRC-5-1</sch:assert>
      <sch:assert id="scap-general-xccdf-status-rule-value-date-2" test="@date!=''">Error: SRC-5-2</sch:assert>
    </sch:rule>
    <sch:rule id="scap-cce-check-rule-1" context="xccdf:ident">
      <sch:assert flag="WARNING" test="not(@system='http://cce.mitre.org' or @system='CCE') or text() != ''" id="scap-cce-check-assert-1">Warning: A-16-1|xccdf:Rule <sch:value-of select="ancestor::xccdf:Rule/@id"/></sch:assert>
      <sch:assert flag="ERROR" test="not(@system='http://cce.mitre.org' or @system='CCE') or starts-with(text(), 'CCE-')" id="scap-cce-check-assert-2">Error: A-17-1|<sch:value-of select="."/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-cce-check-rule-2" context="oval-def:reference">
      <sch:assert flag="WARNING" test="not(@source='http://cce.mitre.org' or @source='CCE') or @ref_id!=''" id="scap-cce-check-assert-5">Warning: A-16-1|oval-def:definition <sch:value-of select="ancestor::oval-def:definition/@id"/></sch:assert>
      <sch:assert flag="ERROR" test="not(@source='http://cce.mitre.org' or @source='CCE') or starts-with(@ref_id, 'CCE-')" id="scap-cce-check-assert-6">Error: A-17-1|<sch:value-of select="@ref_id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-check-system-content-match-rule" context="scap:check-system-content">
      <sch:assert flag="ERROR" test="not(@content-type='OVAL_COMPLIANCE' or @content-type='OVAL_PATCH' or @content-type='CPE_INVENTORY' or @content-type='OVAL_VULNERABILITY') or oval-def:oval_definitions" id="scap-check-system-content-match-assert-1">Error: A-18-1</sch:assert>
      <sch:assert flag="ERROR" test="@content-type != 'OCIL_QUESTIONS' or ocil:ocil" id="scap-check-system-content-match-assert-2">Error: A-18-1</sch:assert>
    </sch:rule>
  </sch:pattern>

</sch:schema>
