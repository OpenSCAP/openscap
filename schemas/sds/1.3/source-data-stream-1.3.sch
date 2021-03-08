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
      <sch:assert id="scap-general-xccdf-benchmark-description" test="xccdf:description">SRC-10-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-lang-required" test="@xml:lang">SRC-2-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-no-check-content-check" test="not(.//xccdf:check-content)">SRC-25-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-profile" context="xccdf:Profile">
      <sch:assert id="scap-general-xccdf-profile-description" test="xccdf:description">SRC-10-1|xccdf:Profile <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-value" context="xccdf:Value">
      <sch:assert id="scap-general-xccdf-value-description" test="xccdf:description">SRC-10-1|xccdf:Value <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-group" context="xccdf:Group">
      <sch:assert id="scap-general-xccdf-group-description" test="xccdf:description">SRC-10-1|xccdf:Group <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-rule" context="xccdf:Rule">
      <sch:assert id="scap-general-xccdf-rule-description" test="xccdf:description">SRC-10-1|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
  </sch:pattern>

</sch:schema>
