<?xml version="1.0" encoding="UTF-8"?>
<!--These rules are for informational purposes only and DO NOT supersede the requirements in NIST SP 800-126 Rev 3.-->
<!--These rules may be revised at anytime. Comments/feedback on these rules are welcome.-->
<!--Private comments may be sent to scap@nist.gov.  Public comments may be sent to scap-dev@nist.gov.-->
<sch:schema xmlns:sch="http://purl.oclc.org/dsdl/schematron"
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform" queryBinding="xslt2">
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
  
  <sch:let name="datafiles_directory" value="'datafiles'"/>

  <sch:pattern id="scap-general">
    <sch:rule id="scap-general-xccdf-benchmark" context="xccdf:Benchmark">
      <sch:assert id="scap-general-xccdf-description" test="every $m in (. union .//xccdf:Profile union .//xccdf:Value union .//xccdf:Group union .//xccdf:Rule) satisfies exists($m/xccdf:description)">SRC-10-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-lang-required" test="exists(@xml:lang)">SRC-2-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-no-check-content-check" test="not(exists(.//xccdf:check-content))">SRC-25-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-val-forbid" test="not(exists(.//xccdf:Value//xccdf:source)) and not(exists(.//xccdf:Value//xccdf:complex-value)) and not(exists(.//xccdf:Value//xccdf:complex-default)) and not(exists(.//xccdf:Value//xccdf:choices//xccdf:complex-choice))">SRC-276-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-version-time-attribute-req" test="exists(xccdf:version/@time)">SRC-3-2</sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-no-xinclude" test="not(exists(.//xinclude:include))">SRC-339-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-version-update-req" test="exists(//xccdf:version[string(@update)])">SRC-341-1</sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-no-set-complex-value" test="not(exists(.//xccdf:set-complex-value))">SRC-343-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-no-group-extension" test="every $m in .//xccdf:Group satisfies not(exists($m/@extends))">SRC-354-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-style" test="@style eq 'SCAP_1.3'">SRC-4-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-metadata-missing" test="exists(xccdf:metadata)">SRC-8-2|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-metadata-populated" test="if( exists(xccdf:metadata) ) then exists(xccdf:metadata/dc:creator/text()) and exists(xccdf:metadata/dc:publisher/text()) and exists(xccdf:metadata/dc:contributor/text()) and exists(xccdf:metadata/dc:source/text()) else false()">SRC-8-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-title-1" test="every $m in (. union .//xccdf:Profile union .//xccdf:Value union .//xccdf:Group union .//xccdf:Rule) satisfies exists($m/xccdf:title)">SRC-9-1|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-title-3" test="every $m in (. union .//xccdf:Profile union .//xccdf:Value union .//xccdf:Group union .//xccdf:Rule) satisfies if( count($m/xccdf:title) gt 1 ) then exists($m/xccdf:title[@xml:lang eq 'en-US']) else true()">SRC-9-3|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-title-2" test="every $m in (. union .//xccdf:Profile union .//xccdf:Value union .//xccdf:Group union .//xccdf:Rule) satisfies (if( count($m/xccdf:title) gt 1 ) then count($m/xccdf:title) eq count($m/xccdf:title[@xml:lang]) else true())">SRC-9-2|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-benchmark-unique" test="every $m in ancestor::scap:data-stream-collection//xccdf:Benchmark[@id eq current()/@id] satisfies (generate-id($m) eq generate-id(current()) or $m/xccdf:version[1] ne current()/xccdf:version[1])">SRC-3-3|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert></sch:rule>
    <sch:rule id="scap-general-xccdf-profile" context="xccdf:Profile">
      <sch:assert id="scap-general-xccdf-profile-unique" test="if (count(ancestor::scap:data-stream-collection//xccdf:Benchmark/xccdf:Profile[@id eq current()/@id]) gt 1) then false() else true()">A-25-1|xccdf:Profile <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-system-cpe-dict-check" context="cpe-dict:check">
      <sch:assert id="scap-general-system-cpe-dict-check2" test="@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2'">SRC-118-2|cpe-dict:cpe-item <sch:value-of select="ancestor::cpe-dict:cpe-item[1]/@name"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-system-cpe-lang-check" context="cpe-lang:check-fact-ref">
      <sch:assert id="scap-general-system-cpe-lang-check2" test="@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2'">SRC-118-3|cpe-lang:platform <sch:value-of select="ancestor::cpe-lang:platform[1]/@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-cpe-ref-check-fact" test="some $m in current()/ancestor::scap:data-stream-collection//ds:checklists/ds:component-ref satisfies(exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, current()/@href))//oval-def:definition[@id eq current()/@id-ref]) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, current()/@href))//ocil:questionnaire[@id eq current()/@id-ref]))">A-27-1|cpe-lang:check-fact-ref <sch:value-of select="@id-ref"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-check" context="xccdf:Rule/xccdf:check">
      <sch:assert id="scap-general-xccdf-check-sys-req" test="@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2'">SRC-118-1|xccdf:Rule <sch:value-of select="ancestor::xccdf:Rule[1]/@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-check-patches-ref-oval-only" test="if(current()/parent::xccdf:Rule[ends-with(@id,'security_patches_up_to_date')]) then current()/@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' else true()">SRC-169-2|xccdf:Rule <sch:value-of select="parent::xccdf:Rule/@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-scap-data-stream-collection" context="scap:data-stream-collection ">
      <sch:assert id="scap-general-latest-schematron-rules" test="exists(current()[@schematron-version='1.3'])">SRC-330-2|scap:data-stream-collection</sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-scap-content" context="scap:data-stream">
      <sch:assert id="scap-general-scap-content-every-cpe-equal-subset2" test="if( function-available('java:isEqualOrSuperset') ) then (every $m in ds:checklists/ds:component-ref satisfies ((every $n in xcf:get-component($m)//xccdf:platform[not(starts-with(@idref,'#'))] satisfies some $o in ds:dictionaries/ds:component-ref satisfies some $p in xcf:get-component($o)//cpe-dict:cpe-item satisfies java:isEqualOrSuperset($n/@idref,$p/@name)) and (every $q in xcf:get-component($m)//cpe-lang:fact-ref satisfies some $r in ds:dictionaries/ds:component-ref satisfies some $s in xcf:get-component($r)//cpe-dict:cpe-item satisfies java:isEqualOrSuperset($q/@name,$s/@name)))) else true()">SRC-15-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-check-content-ref-multiple-patch-id-req" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule[starts-with(@id,'xccdf_') and ends-with(@id,'_rule_security_patches_up_to_date')]//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies if ($n/parent::node()[@multi-check eq 'true']) then count(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@class eq 'patch']) gt 0 else true()">SRC-169-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-check-content-ref-multiple-patch-name-omit" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule[starts-with(@id,'xccdf_') and ends-with(@id,'_rule_security_patches_up_to_date')]//xccdf:check-content-ref satisfies if ($n/parent::node()[@multi-check eq 'true']) then not(exists($n[@name])) else true()">SRC-171-1|xccdf:Rule <value-of select="ancestor::xccdf:Rule/@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-check-content-ref-single-patch-id-req" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule[starts-with(@id,'xccdf_') and ends-with(@id,'_rule_security_patches_up_to_date')]//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies if ($n/parent::node()[@multi-check eq 'false' or not(exists(@multi-check))]) then ((not(exists($n/preceding-sibling::*:check-content-ref)) and not(exists($n/following-sibling::*:check-content-ref))) and count(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name and @class eq 'patch']) eq 1) else true()">SRC-377-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-check-content-ref-single-patch-name-req" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule[starts-with(@id,'xccdf_') and ends-with(@id,'_rule_security_patches_up_to_date')]//xccdf:check-content-ref satisfies if ($n/parent::node()[@multi-check eq 'false' or not(exists(@multi-check))]) then exists($n[@name]) else true()">SRC-379-1|xccdf:Rule <value-of select="ancestor::xccdf:Rule/@id"/></sch:assert>
      <sch:assert id="scap-use-case-conf-verification-rule-ref-oval" test="if( @use-case eq 'CONFIGURATION' ) then every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule satisfies (if(exists($n/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2']/xccdf:check-content-ref[exists(@name) and not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)])) then some $o in $n/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2']/xccdf:check-content-ref[exists(@name) and not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies (exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//oval-def:definition[@id eq $o/@name and matches(@class,'^(compliance|patch)$')]) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//ocil:questionnaire[@id eq $o/@name])) else true()) else true()">SRC-236-2|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-use-case-conf-verification-content-xccdf-req" test="if( @use-case eq 'CONFIGURATION' ) then (count(//*:checklists/ds:component-ref) gt 0 and (some $m in ds:checklists/ds:component-ref satisfies exists(xcf:get-component($m)/xccdf:Benchmark))) else true()">SRC-236-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-use-case-vul-assess-xccdf-oval-content-xccdf-req" test="if( @use-case eq 'VULNERABILITY' ) then (count(//*:checklists/ds:component-ref) gt 0 and (some $m in ds:checklists/ds:component-ref satisfies exists(xcf:get-component($m)/xccdf:Benchmark))) else true()">SRC-242-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-use-case-vul-assess-xccdf-oval-rule-xccdf-oval-ref-req" test="if( @use-case eq 'VULNERABILITY' ) then every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule satisfies some $o in $n/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2']/xccdf:check-content-ref[exists(@name) and not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies (exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//oval-def:definition[@id eq $o/@name and matches(@class,'^(vulnerability|patch)$')]) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//ocil:questionnaire[@id eq $o/@name])) else true()">SRC-242-2|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-use-case-inventory-collection-content-xccdf-req" test="if( @use-case eq 'INVENTORY' ) then (count(//*:checklists/ds:component-ref) gt 0 and (some $m in ds:checklists/ds:component-ref satisfies exists(xcf:get-component($m)/xccdf:Benchmark))) else true()">SRC-248-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-use-case-inventory-collection-xccdf-rule-ref-oval-def-req" test="if( @use-case eq 'INVENTORY' ) then every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule satisfies some $o in $n/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2']/xccdf:check-content-ref[exists(@name) and not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies (exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//oval-def:definition[@id eq $o/@name and matches(@class,'^(inventory)$')]) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//ocil:questionnaire[@id eq $o/@name])) else true()">SRC-248-2|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-xccdf-oval-ref-match" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:ident[matches(@system,'^(http://cce.mitre.org|http://cve.mitre.org|http://cpe.mitre.org)$')] satisfies every $o in $n/ancestor::xccdf:Rule[1]/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']/xccdf:check-content-ref[exists(@name)] satisfies exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//oval-def:definition[@id eq $o/@name]//oval-def:reference[matches(@source,xcf:ident-mapping($n/@system)) and $n eq @ref_id])">SRC-251-2|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-use-case-conf-verification-benchmark-one-rule-ref-oval-ocil" test="if( @use-case eq 'CONFIGURATION' ) then ( every $m in ds:checklists/ds:component-ref satisfies if( exists(xcf:get-component($m)/xccdf:Benchmark)) then (some $n in xcf:get-component($m)//xccdf:check[matches(@system,'^(http://oval\.mitre\.org/XMLSchema/oval-definitions-5|http://scap\.nist\.gov/schema/ocil/2)$')]/xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog,@href) cast as xsd:boolean)] satisfies (exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//ocil:questionnaire[@id eq $n/@name]) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name and @class eq 'compliance']))) else true()) else true()">SRC-262-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-use-case-vul-assess-xccdf-benchmark-one-rule-ref-oval-ocil" test="if( @use-case eq 'VULNERABILITY' ) then( every $m in ds:checklists/ds:component-ref satisfies if( exists(xcf:get-component($m)/xccdf:Benchmark)) then( some $n in xcf:get-component($m)//xccdf:check[matches(@system,'^(http://oval\.mitre\.org/XMLSchema/oval-definitions-5|http://scap\.nist\.gov/schema/ocil/2)$')]/xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog,@href) cast as xsd:boolean)] satisfies (exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//ocil:questionnaire[@id eq $n/@name]) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name and @class eq 'vulnerability']))) else true()) else true()">SRC-265-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-dict-and-oval-req" test="if( @use-case eq 'CONFIGURATION' ) then every $m in ds:checklists/ds:component-ref satisfies( if( exists(xcf:get-component($m)//xccdf:platform)  or exists(xcf:get-component($m)//cpe-dict:cpe-item)  ) then (exists(ds:dictionaries/ds:component-ref) and (some $n in ds:checks/ds:component-ref satisfies exists(xcf:get-component($n)//oval-def:definition[@class eq 'inventory'])) )else true()) else true()">SRC-30-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-resolve-to-comp-ref" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:check//xccdf:check-content-ref satisfies exists(xcf:get-component-ref($m/cat:catalog, $n/@href))">SRC-31-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-use-case" test="matches(@use-case,'^(CONFIGURATION|VULNERABILITY|INVENTORY|OTHER)$')">SRC-324-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-vuln-cpe-inv-req" test="if( @use-case eq 'VULNERABILITY' ) then every $m in ds:checklists/ds:component-ref satisfies (if( exists(xcf:get-component($m)//xccdf:platform) or exists(xcf:get-component($m)//cpe-dict:cpe-item) ) then (exists(ds:dictionaries/ds:component-ref) and (some $n in ds:checks/ds:component-ref satisfies exists(xcf:get-component($n)//oval-def:definition[@class eq 'inventory'])) ) else true()) else true()">SRC-33-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-indent-match-to-oval-def-inv" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule[exists(xccdf:ident[@system eq 'http://cpe.mitre.org'])]/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean) and exists(@name)] satisfies exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name and @class eq 'inventory'])">SRC-331-3|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-indent-match-to-oval-def-comp" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule[exists(xccdf:ident[@system eq 'http://cce.mitre.org'])]/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean) and exists(@name)] satisfies exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name and @class eq 'compliance'])">SRC-331-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-indent-match-to-oval-def-vuln" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule[exists(xccdf:ident[@system eq 'http://cve.mitre.org'])]/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean) and exists(@name)] satisfies exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name and @class eq 'vulnerability'])">SRC-331-2|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-ref-comp-once" test="every $m in .//scap:component-ref satisfies count($m/ancestor::scap:data-stream//scap:component-ref[@xlink:href eq $m/@xlink:href]) eq 1">SRC-333-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-oval-ref-check-content-ref-oval" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:oval_definitions)">SRC-345-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-xccdf-check-content-ref-name-not-req" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies (not(exists($n/@name)) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name]))">SRC-346-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-ocil-check-content-ref-ocil" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:check[@system eq 'http://scap.nist.gov/schema/ocil/2']//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//ocil:ocil)">SRC-348-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-scap-content-ocil-check-content-ref-with-without-name" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:check[@system eq 'http://scap.nist.gov/schema/ocil/2']//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies (not(exists($n/@name)) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//ocil:questionnaire[@id eq $n/@name]))">SRC-349-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-value-oval-binding-type-1" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']/xccdf:check-export satisfies ( not(matches($n/ancestor::xccdf:Benchmark//xccdf:Value[@id eq $n/@value-id]/@type, '^number$')) or (every $o in $n/following-sibling::xccdf:check-content-ref satisfies matches(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//*[@id eq $n/@export-name]/@datatype, '^(int|float)$')))">SRC-38-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-value-oval-binding-type-2" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']/xccdf:check-export satisfies ( not(matches($n/ancestor::xccdf:Benchmark//xccdf:Value[@id eq $n/@value-id]/@type, '^boolean$')) or (every $o in $n/following-sibling::xccdf:check-content-ref satisfies matches(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//*[@id eq $n/@export-name]/@datatype, '^boolean$')))">SRC-38-2|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-value-oval-binding-type-3" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']/xccdf:check-export satisfies ( not(matches($n/ancestor::xccdf:Benchmark//xccdf:Value[@id eq $n/@value-id]/@type, '^string$')) or (every $o in $n/following-sibling::xccdf:check-content-ref satisfies matches(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//*[@id eq $n/@export-name]/@datatype, '^(string|evr_string|version|ios_version|fileset_revision|binary)$')))">SRC-38-3|scap:data-stream <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-oval-cpe-inventory" test="every $m in ds:dictionaries/ds:component-ref satisfies every $n in xcf:get-component($m)//cpe-dict:cpe-list/cpe-dict:cpe-item/cpe-dict:check satisfies ($n/@system = 'http://oval.mitre.org/XMLSchema/oval-definitions-5' and exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id = $n]))">SRC-72-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-rule" context="xccdf:Rule">
      <sch:assert id="scap-general-xccdf-rule-check-content-ref-req" test="every $m in .//xccdf:check satisfies exists($m/xccdf:check-content-ref)">SRC-175-1|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-cve-cce-cpe" test="exists(xccdf:ident[matches(@system,'^(http://cce.mitre.org|http://cve.mitre.org|http://cpe.mitre.org)$')])">SRC-251-1|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-ident-cve-cce-cpe-order" test="every $m in xccdf:ident satisfies matches($m/@system,'^(http://cpe.mitre.org|http://cve.mitre.org|http://cce.mitre.org)$') or not(exists($m/following-sibling::xccdf:ident[matches(@system,'^(http://cpe.mitre.org|http://cve.mitre.org|http://cce.mitre.org)$')]))">SRC-257-1|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-cce-exists" test="every $m in xccdf:ident[matches(@system, '^(http://cce.mitre.org)$')] satisfies exists(document(concat($datafiles_directory,'/nvdcce-0.1-feed.xml'))/nvd-config:nvd/nvd-config:entry[@id eq $m])">SRC-74-1|xccdf:Rule <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-xccdf-rule-attributes-exists" test="exists(@selected) and exists(@weight) and exists(@role) and exists(@severity)">A-26-1</sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-oval-def" context="oval-def:definition">
      <sch:assert id="scap-general-oval-def-rule-compliance-cce" test="if(@class eq 'compliance') then exists(oval-def:metadata/oval-def:reference[matches(@source,'^(http://cce.mitre.org|CCE)$')]) else true()">SRC-207-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-oval-def-compliance-extension" test="if(@class eq 'compliance') then every $m in xcf:get-all-parents(ancestor::oval-def:definitions,.) satisfies matches($m/@class,'^(compliance|inventory)$') else true()">SRC-208-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-oval-def-ref-cpe-inventory" test="if(@class eq 'inventory') then exists(oval-def:metadata/oval-def:reference[matches(@source,'^(http://cpe.mitre.org|CPE)$') and (matches(@ref_id,'^[c][pP][eE]:/[AHOaho]?(:[A-Za-z0-9\._\-~%]*){0,6}$') or matches(@ref_id,'^cpe:2\.3:[aho\*-](:(((\?*|\*?)([a-zA-Z0-9\-\._]|(\\[\\\*\?!&quot;#$$%&amp;\(\)\+,/:;&lt;=&gt;@\[\]\^`\{{\|}}~]))+(\?*|\*?))|[\*-])){5}(:(([a-zA-Z]{2,3}(-([a-zA-Z]{2}|[0-9]{3}))?)|[\*-]))(:(((\?*|\*?)([a-zA-Z0-9\-\._]|(\\[\\\*\?!&quot;#$$%&amp;\(\)\+,/:;&lt;=&gt;@\[\]\^`\{{\|}}~]))+(\?*|\*?))|[\*-])){4}$'))]) else true()">SRC-209-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-oval-def-inventory-extension" test="if(@class eq 'inventory') then every $m in xcf:get-all-parents(ancestor::oval-def:definitions,.) satisfies matches($m/@class,'^(inventory)$') else true()">SRC-210-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-oval-def-patch-extensions" test="if(@class eq 'patch') then every $m in xcf:get-all-parents(ancestor::oval-def:definitions,.) satisfies matches($m/@class,'^(patch|inventory)$') else true()">SRC-213-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-oval-def-vulnerability-cve-ref" test="if(@class eq 'vulnerability') then exists(oval-def:metadata/oval-def:reference[matches(@source,'^(http://cve.mitre.org|CVE)$')]) else true()">SRC-214-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
      <sch:assert id="scap-general-oval-def-vulnerability-extension" test="if(@class eq 'vulnerability') then every $m in xcf:get-all-parents(ancestor::oval-def:definitions,.) satisfies (if( generate-id(current()) ne generate-id($m) ) then matches($m/@class,'^(inventory|vulnerability)$') else true() ) else true()">SRC-215-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-oval-generator" context="oval-def:generator">
      <sch:assert id="scap-general-oval-version" test="every $m in oval-com:schema_version satisfies if (not(exists($m/@platform))) then matches($m,'^5\.(3|4|5|6|7|8|9|10(\.1)?|11(\.1|\.2)?)$') else true()">SRC-216-1</sch:assert>
      <sch:assert id="scap-general-oval-platform-version" test="every $m in oval-com:schema_version satisfies if (exists($m/@platform)) then matches($m,'^5\.11(\.1|\.2):1(\.0|\.1|\.2)$') else true()">SRC-216-2</sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-signature-sig" context="dsig:Signature">
      <sch:assert id="scap-general-signature-sig-one-ds-ref" test="every $m in dsig:SignedInfo/dsig:Reference satisfies ((not(exists(ancestor::ds:data-stream-collection[1]/ds:data-stream[concat('#',@id) eq $m/@URI]))) or (every $n in $m/preceding-sibling::dsig:Reference satisfies not(exists(ancestor::ds:data-stream-collection[1]/ds:data-stream[concat('#',@id) eq $n/@URI]))))">SRC-282-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-local-refs-req" test="every $m in ancestor::ds:data-stream-collection[1]/ds:data-stream[concat('#',@id) eq current()/dsig:SignedInfo/dsig:Reference[1]/@URI]//ds:component-ref[starts-with(@xlink:href,'#')] satisfies exists(dsig:Object/dsig:Manifest/dsig:Reference[@URI eq $m/@xlink:href])">SRC-284-2|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-manifest-req" test="exists(dsig:Object/dsig:Manifest)">SRC-284-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-tmsad" test="exists(dsig:Object/dsig:SignatureProperties/dsig:SignatureProperty/tmsad:signature-info)">SRC-285-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-first-ref-ds" test="exists(ancestor::ds:data-stream-collection[1]/ds:data-stream[concat('#',@id) eq current()/dsig:SignedInfo/dsig:Reference[1]/@URI])">SRC-286-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-ref-sig-prop" test="exists(dsig:Object/dsig:SignatureProperties[concat('#',@Id) eq current()/dsig:SignedInfo/dsig:Reference[2]/@URI])">SRC-287-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-third-ref-manifest" test="exists(dsig:Object/dsig:Manifest[concat('#',@Id) eq current()/dsig:SignedInfo/dsig:Reference[3]/@URI])">SRC-288-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
      <sch:assert id="scap-general-signature-sig-key-info" test="exists(dsig:KeyInfo)">SRC-290-1|dsig:Signature <sch:value-of select="@Id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-general-xccdf-status-rule-value-date" context="xccdf:status">
      <sch:assert id="scap-general-xccdf-status-rule-value-date-1" test=". eq 'draft' or . eq 'accepted'">SRC-5-1</sch:assert>
      <sch:assert id="scap-general-xccdf-status-rule-value-date-2" test="@date ne ''">SRC-5-2</sch:assert>
    </sch:rule>
  </sch:pattern>
  <sch:pattern id="scap-additional-rules">
    <sch:rule id="scap-unused-oval-rule" context="oval-def:definition">
      <sch:assert flag="WARNING" test="if(exists(//scap:xccdf-content)) then exists(ancestor::scap:check-system-content[@content-type eq 'OVAL_PATCH']) or exists(//xccdf:check-content-ref[@href eq current()/ancestor::scap:check-system-content/@id and (not(@name) or @name eq current()/@id)]) or exists(current()/ancestor::oval-def:oval_definitions//oval-def:extend_definition[@definition_ref eq current()/@id]) or exists(//cpe-dict:check[@href eq current()/ancestor::scap:check-system-content/@id and . eq current()/@id]) else true()" id="scap-unused-oval-assert">A-15-1|oval-def:definition <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-cce-check-rule-1" context="xccdf:ident">
      <sch:assert flag="WARNING" test="if(@system eq 'http://cce.mitre.org' or @system eq 'CCE') then . ne '' else true()" id="scap-cce-check-assert-1">A-16-1|xccdf:Rule <sch:value-of select="ancestor::xccdf:Rule/@id"/></sch:assert>
      <sch:assert flag="ERROR" test="if(@system eq 'http://cce.mitre.org' or @system eq 'CCE') then matches(., '^CCE-\d+-\d$') else true()" id="scap-cce-check-assert-2">A-17-1|<sch:value-of select="."/></sch:assert>
      <sch:assert flag="ERROR" test="if((@system eq 'http://cce.mitre.org' or @system eq 'CCE') and matches(., '^CCE-\d+-\d$')) then (sum(for $j in (for $i in reverse(string-to-codepoints(concat(substring(.,5,string-length(.)-6),substring(.,string-length(.),1))))[position() mod 2 = 0] return ($i - 48) * 2, for $i in reverse(string-to-codepoints(concat(substring(.,5,string-length(.)-6),substring(.,string-length(.),1))))[position() mod 2 = 1] return ($i - 48)) return ($j mod 10, $j idiv 10)) mod 10) eq 0 else true()" id="scap-cce-check-assert-3">A-17-1|<sch:value-of select="."/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-cce-check-rule-2" context="oval-def:reference">
      <sch:assert flag="WARNING" test="if(@source eq 'http://cce.mitre.org' or @source eq 'CCE') then @ref_id ne '' else true()" id="scap-cce-check-assert-5">A-16-1|oval-def:definition <sch:value-of select="ancestor::oval-def:definition/@id"/></sch:assert>
      <sch:assert flag="ERROR" test="if(@source eq 'http://cce.mitre.org' or @source eq 'CCE') then matches(@ref_id, '^CCE-\d+-\d$') else true()" id="scap-cce-check-assert-6">A-17-1|<sch:value-of select="@ref_id"/></sch:assert>
      <sch:assert flag="ERROR" test="if((@source eq 'http://cce.mitre.org' or @source eq 'CCE') and matches(@ref_id, '^CCE-\d+-\d$')) then (sum(for $j in (for $i in reverse(string-to-codepoints(concat(substring(@ref_id,5,string-length(@ref_id)-6),substring(@ref_id,string-length(@ref_id),1))))[position() mod 2 = 0] return ($i - 48) * 2, for $i in reverse(string-to-codepoints(concat(substring(@ref_id,5,string-length(@ref_id)-6),substring(@ref_id,string-length(@ref_id),1))))[position() mod 2 = 1] return ($i - 48)) return ($j mod 10, $j idiv 10)) mod 10) eq 0 else true()" id="scap-cce-check-assert-7">A-17-1|<sch:value-of select="@ref_id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-check-system-content-match-rule" context="scap:check-system-content">
      <sch:assert flag="ERROR" test="if(matches(@content-type,'^(OVAL_COMPLIANCE|OVAL_PATCH|CPE_INVENTORY|OVAL_VULNERABILITY)$')) then exists(oval-def:oval_definitions) else true()" id="scap-check-system-content-match-assert-1">A-18-1</sch:assert>
      <sch:assert flag="ERROR" test="if(matches(@content-type,'^(OCIL_QUESTIONS)$')) then exists(ocil:ocil) else true()" id="scap-check-system-content-match-assert-2">A-18-1</sch:assert>
    </sch:rule>
    <!--        
        <sch:rule id="scap-xccdf-profile-check-rule" context="xccdf:Benchmark">
        <sch:assert flag="ERROR"
        test="if($profile ne '') then exists(current()//xccdf:Profile[@id eq $profile]) else true()"
        id="scap-xccdf-profile-check-asset">A-20|xccdf:Benchmark <sch:value-of select="@id"/></sch:assert>
        </sch:rule>
    -->
    <sch:rule id="scap-oval-tests" context="oval-def:tests/*">
      <sch:assert flag="INFO" test="exists(document(concat($datafiles_directory,'/validation_program_oval_test_types.xml'))/test-types-collection/test_types[@style='SCAP_1.3']/test_type[@namespace eq namespace-uri(current()) and @name eq local-name(current())])" id="scap-oval-tests-validation-program-test-types">A-21-1|OVAL test <sch:value-of select="@id"/></sch:assert>
    </sch:rule>
    <sch:rule id="scap-collection" context="scap:data-stream-collection">
      <sch:assert flag="WARNING" test="function-available('java:isEqualOrSuperset')" id="scap-collection-java-functions">A-22-1</sch:assert>
    </sch:rule>
  </sch:pattern>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:get-all-profile-parents">
    <xsl:param name="doc"/>
    <xsl:param name="node"/>
    <xsl:sequence select="$node"/>
    <xsl:if test="exists($node/@extends)">
      <xsl:sequence select="xcf:get-all-profile-parents($doc,$doc//xccdf:Benchmark//xccdf:Profile[@id eq $node/@extends])"/>
    </xsl:if>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:get-component">
    <xsl:param name="component-ref"/>
    <xsl:sequence select="$component-ref/ancestor::ds:data-stream-collection//ds:component[@id eq substring($component-ref/@xlink:href,2)]"/>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/XSL/Transform" name="xcf:get-all-def-children">
    <xsl:param name="doc"/>
    <xsl:param name="node"/>
    <xsl:sequence select="$node"/>
    <xsl:for-each select="$doc//oval-def:extend_definition[@definition_ref eq $node/@id]/ancestor::oval-def:definition">
      <xsl:sequence select="xcf:get-all-def-children($doc,current())"/>
    </xsl:for-each>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:ident-mapping">
    <xsl:param name="in-string"/>
    <xsl:choose>
      <xsl:when test="$in-string eq 'http://cce.mitre.org'">
        <xsl:value-of select="string('^(CCE|http://cce.mitre.org)$')"/>
      </xsl:when>
      <xsl:when test="$in-string eq 'http://cve.mitre.org'">
        <xsl:value-of select="string('^(CVE|http://cve.mitre.org)$')"/>
      </xsl:when>
      <xsl:when test="$in-string eq 'http://cpe.mitre.org'">
        <xsl:value-of select="string('^(CPE|http://cpe.mitre.org)$')"/>
      </xsl:when>
    </xsl:choose>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:get-all-parents">
    <xsl:param name="doc"/>
    <xsl:param name="node"/>
    <xsl:sequence select="$node"/>
    <xsl:for-each select="$node//oval-def:extend_definition">
      <xsl:sequence select="xcf:get-all-parents($doc,ancestor::oval-def:oval_definitions//*[@id eq current()/@definition_ref])"/>
    </xsl:for-each>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:get-ocil-var-ref">
    <xsl:param name="ocil_questionnaire"/>
    <xsl:variable name="initialSet">
      <xsl:for-each select="$ocil_questionnaire/ocil:actions/ocil:test_action_ref">
        <xsl:sequence select="ancestor::ocil:ocil/ocil:test_actions/ocil:numeric_question_test_action[@id eq current()]/ocil:when_equals[@var_ref]"/>
        <xsl:sequence select="ancestor::ocil:ocil/ocil:test_actions/ocil:string_question_test_action[@id eq current()]/ocil:when_pattern/ocil:pattern[@var_ref]"/>
        <xsl:sequence select="ancestor::ocil:ocil/ocil:test_actions/ocil:numeric_question_test_action[@id eq current()]/ocil:when_range/ocil:range/ocil:min[@var_ref]"/>
        <xsl:sequence select="ancestor::ocil:ocil/ocil:test_actions/ocil:numeric_question_test_action[@id eq current()]/ocil:when_range/ocil:range/ocil:max[@var_ref]"/>
        <xsl:for-each select="ancestor::ocil:ocil/ocil:test_actions/*[@id eq current()]">
          <xsl:sequence select="ancestor::ocil:ocil/ocil:questions/ocil:choice_question[@id eq current()/@question_ref]/ocil:choice[@var_ref]"/>
          <xsl:for-each select="ancestor::ocil:ocil/ocil:questions/ocil:choice_question[@id eq current()/@question_ref]/ocil:choice_group_ref">
            <xsl:sequence select="ancestor::ocil:questions/ocil:choice_group[@id eq current()]/ocil:choice[@var_ref]"/>
          </xsl:for-each>
        </xsl:for-each>
      </xsl:for-each>
    </xsl:variable>
    <xsl:for-each select="$initialSet/*">
      <xsl:sequence select="xcf:pass-ocil-var-ref($ocil_questionnaire,current())"/>
    </xsl:for-each>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:get-component-ref">
    <xsl:param name="catalog"/>
    <xsl:param name="uri"/>
    <xsl:variable name="component-ref-uri" select="xcf:resolve-in-catalog($catalog/*[1],$uri)"/>
    <xsl:if test="$component-ref-uri ne ''">
      <xsl:sequence select="$catalog/ancestor::ds:data-stream//ds:component-ref[@id eq substring($component-ref-uri,2)]"/>
    </xsl:if>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:pass-ocil-var-ref">
    <xsl:param name="ocil_questionnaire"/>
    <xsl:param name="var_ref"/>
    <xsl:sequence select="$var_ref"/>
    <xsl:for-each select="$ocil_questionnaire/ancestor::ocil:ocil/ocil:variables/ocil:local_variable[@id eq $var_ref/@var_ref]/ocil:set/ocil:when_range/ocil:min[@var_ref]">
      <xsl:sequence select="xcf:pass-ocil-var-ref($ocil_questionnaire,current())"/>
    </xsl:for-each>
    <xsl:for-each select="$ocil_questionnaire/ancestor::ocil:ocil/ocil:variables/ocil:local_variable[@id eq $var_ref/@var_ref]/ocil:set/ocil:when_range/ocil:max[@var_ref]">
      <xsl:sequence select="xcf:pass-ocil-var-ref($ocil_questionnaire,current())"/>
    </xsl:for-each>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:resolve-in-catalog">
    <xsl:param name="resolver-node"/>
    <xsl:param name="uri"/>
    <xsl:choose>
      <xsl:when test="starts-with($uri,'#')">
        <xsl:value-of select="$uri"/>
      </xsl:when>
      <xsl:when test="exists($resolver-node)">
        <xsl:choose>
          <xsl:when test="$resolver-node/local-name() eq 'uri'">
            <xsl:choose>
              <xsl:when test="$resolver-node/@name eq $uri">
                <xsl:value-of select="$resolver-node/@uri"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="xcf:resolve-in-catalog($resolver-node/following-sibling::*[1], $uri)"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="$resolver-node/local-name() eq 'rewriteURI'">
            <xsl:choose>
              <xsl:when test="starts-with($uri,$resolver-node/@uriStartString)">
                <xsl:value-of select="concat($resolver-node/@rewritePrefix,substring($uri,string-length($resolver-node/@uriStartString)+1))"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="xcf:resolve-in-catalog($resolver-node/following-sibling::*[1], $uri)"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
        </xsl:choose>
      </xsl:when>
    </xsl:choose>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/XSL/Transform" name="xcf:get-all-external-vars">
    <xsl:param name="doc"/>
    <xsl:param name="node"/>
    <xsl:sequence select="$doc//oval-def:external_variable[@id eq $node/@var_ref]"/>
    <xsl:for-each select="$doc//*[@id eq $node/@var_ref]//*[@var_ref]">
      <xsl:sequence select="xcf:get-all-external-vars($doc,current())"/>
    </xsl:for-each>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:is-external-ref">
    <xsl:param name="catalog"/>
    <xsl:param name="uri"/>
    <xsl:variable name="comp-ref" select="xcf:get-component-ref($catalog,$uri)"/>
    <xsl:choose>
      <xsl:when test="exists($comp-ref)">
        <xsl:value-of select="not(starts-with($comp-ref/@xlink:href,'#'))"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="false()"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:get-all-group-parents">
    <xsl:param name="doc"/>
    <xsl:param name="node"/>
    <xsl:sequence select="$node"/>
    <xsl:if test="exists($node/@extends)">
      <xsl:sequence select="xcf:get-all-group-parents($doc,$doc//xccdf:Benchmark//xccdf:Group[@id eq $node/@extends])"/>
    </xsl:if>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:get-locator-prefix">
    <xsl:param name="name"/>
    <xsl:variable name="subName" select="substring($name,1,string-length($name) - string-length(tokenize($name,'-')[last()]) - 1)"/>
    <xsl:choose>
      <xsl:when test="ends-with($subName,'cpe')">
        <xsl:value-of select="xcf:get-locator-prefix($subName)"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="tokenize($subName,'/')[last()]"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:function>
  <xsl:function xmlns:xcf="nist:scap:xslt:function" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" name="xcf:get-locator-prefix-res">
    <xsl:param name="name"/>
    <xsl:variable name="subName" select="substring($name,1,string-length($name) - string-length(tokenize($name,'-')[last()]) - 1)"/>
    <xsl:value-of select="xcf:get-locator-prefix($subName)"/>
  </xsl:function>
</sch:schema>
