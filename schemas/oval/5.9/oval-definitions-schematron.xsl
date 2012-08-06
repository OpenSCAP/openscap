<?xml version="1.0" standalone="yes"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:iso="http://purl.oclc.org/dsdl/schematron" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:aix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#aix" xmlns:apache-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#apache" xmlns:catos-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#catos" xmlns:esx-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#esx" xmlns:freebsd-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#freebsd" xmlns:hpux-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#hpux" xmlns:ios-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#ios" xmlns:linux-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns:macos-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#macos" xmlns:pixos-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#pixos" xmlns:sp-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#sharepoint" xmlns:sol-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#solaris" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:win-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#windows" version="1.0"><!--Implementers: please note that overriding process-prolog or process-root is
    the preferred method for meta-stylesheets to use where possible. -->
<axsl:param name="archiveDirParameter"/><axsl:param name="archiveNameParameter"/><axsl:param name="fileNameParameter"/><axsl:param name="fileDirParameter"/>

<!--PHASES-->


<!--PROLOG-->


<!--KEYS-->


<!--DEFAULT RULES-->


<!--MODE: SCHEMATRON-SELECT-FULL-PATH-->
<!--This mode can be used to generate an ugly though full XPath for locators-->
<axsl:template match="*" mode="schematron-select-full-path"><axsl:apply-templates select="." mode="schematron-get-full-path"/></axsl:template>

<!--MODE: SCHEMATRON-FULL-PATH-->
<!--This mode can be used to generate an ugly though full XPath for locators-->
<axsl:template match="*" mode="schematron-get-full-path"><axsl:apply-templates select="parent::*" mode="schematron-get-full-path"/><axsl:text>/</axsl:text><axsl:choose><axsl:when test="namespace-uri()=''"><axsl:value-of select="name()"/><axsl:variable name="p_1" select="1+    count(preceding-sibling::*[name()=name(current())])"/><axsl:if test="$p_1&gt;1 or following-sibling::*[name()=name(current())]">[<axsl:value-of select="$p_1"/>]</axsl:if></axsl:when><axsl:otherwise><axsl:text>*[local-name()='</axsl:text><axsl:value-of select="local-name()"/><axsl:text>' and namespace-uri()='</axsl:text><axsl:value-of select="namespace-uri()"/><axsl:text>']</axsl:text><axsl:variable name="p_2" select="1+   count(preceding-sibling::*[local-name()=local-name(current())])"/><axsl:if test="$p_2&gt;1 or following-sibling::*[local-name()=local-name(current())]">[<axsl:value-of select="$p_2"/>]</axsl:if></axsl:otherwise></axsl:choose></axsl:template><axsl:template match="@*" mode="schematron-get-full-path"><axsl:text>/</axsl:text><axsl:choose><axsl:when test="namespace-uri()=''">@<axsl:value-of select="name()"/></axsl:when><axsl:otherwise><axsl:text>@*[local-name()='</axsl:text><axsl:value-of select="local-name()"/><axsl:text>' and namespace-uri()='</axsl:text><axsl:value-of select="namespace-uri()"/><axsl:text>']</axsl:text></axsl:otherwise></axsl:choose></axsl:template>

<!--MODE: SCHEMATRON-FULL-PATH-2-->
<!--This mode can be used to generate prefixed XPath for humans-->
<axsl:template match="node() | @*" mode="schematron-get-full-path-2"><axsl:for-each select="ancestor-or-self::*"><axsl:text>/</axsl:text><axsl:value-of select="name(.)"/><axsl:if test="preceding-sibling::*[name(.)=name(current())]"><axsl:text>[</axsl:text><axsl:value-of select="count(preceding-sibling::*[name(.)=name(current())])+1"/><axsl:text>]</axsl:text></axsl:if></axsl:for-each><axsl:if test="not(self::*)"><axsl:text/>/@<axsl:value-of select="name(.)"/></axsl:if></axsl:template>

<!--MODE: GENERATE-ID-FROM-PATH -->
<axsl:template match="/" mode="generate-id-from-path"/><axsl:template match="text()" mode="generate-id-from-path"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:value-of select="concat('.text-', 1+count(preceding-sibling::text()), '-')"/></axsl:template><axsl:template match="comment()" mode="generate-id-from-path"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:value-of select="concat('.comment-', 1+count(preceding-sibling::comment()), '-')"/></axsl:template><axsl:template match="processing-instruction()" mode="generate-id-from-path"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:value-of select="concat('.processing-instruction-', 1+count(preceding-sibling::processing-instruction()), '-')"/></axsl:template><axsl:template match="@*" mode="generate-id-from-path"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:value-of select="concat('.@', name())"/></axsl:template><axsl:template match="*" mode="generate-id-from-path" priority="-0.5"><axsl:apply-templates select="parent::*" mode="generate-id-from-path"/><axsl:text>.</axsl:text><axsl:value-of select="concat('.',name(),'-',1+count(preceding-sibling::*[name()=name(current())]),'-')"/></axsl:template><!--MODE: SCHEMATRON-FULL-PATH-3-->
<!--This mode can be used to generate prefixed XPath for humans
	(Top-level element has index)-->
<axsl:template match="node() | @*" mode="schematron-get-full-path-3"><axsl:for-each select="ancestor-or-self::*"><axsl:text>/</axsl:text><axsl:value-of select="name(.)"/><axsl:if test="parent::*"><axsl:text>[</axsl:text><axsl:value-of select="count(preceding-sibling::*[name(.)=name(current())])+1"/><axsl:text>]</axsl:text></axsl:if></axsl:for-each><axsl:if test="not(self::*)"><axsl:text/>/@<axsl:value-of select="name(.)"/></axsl:if></axsl:template>

<!--MODE: GENERATE-ID-2 -->
<axsl:template match="/" mode="generate-id-2">U</axsl:template><axsl:template match="*" mode="generate-id-2" priority="2"><axsl:text>U</axsl:text><axsl:number level="multiple" count="*"/></axsl:template><axsl:template match="node()" mode="generate-id-2"><axsl:text>U.</axsl:text><axsl:number level="multiple" count="*"/><axsl:text>n</axsl:text><axsl:number count="node()"/></axsl:template><axsl:template match="@*" mode="generate-id-2"><axsl:text>U.</axsl:text><axsl:number level="multiple" count="*"/><axsl:text>_</axsl:text><axsl:value-of select="string-length(local-name(.))"/><axsl:text>_</axsl:text><axsl:value-of select="translate(name(),':','.')"/></axsl:template><!--Strip characters--><axsl:template match="text()" priority="-1"/>

<!--SCHEMA METADATA-->
<axsl:template match="/"><axsl:apply-templates select="/" mode="M36"/><axsl:apply-templates select="/" mode="M37"/><axsl:apply-templates select="/" mode="M38"/><axsl:apply-templates select="/" mode="M39"/><axsl:apply-templates select="/" mode="M40"/><axsl:apply-templates select="/" mode="M41"/><axsl:apply-templates select="/" mode="M42"/><axsl:apply-templates select="/" mode="M43"/><axsl:apply-templates select="/" mode="M44"/><axsl:apply-templates select="/" mode="M45"/><axsl:apply-templates select="/" mode="M46"/><axsl:apply-templates select="/" mode="M47"/><axsl:apply-templates select="/" mode="M48"/><axsl:apply-templates select="/" mode="M49"/><axsl:apply-templates select="/" mode="M50"/><axsl:apply-templates select="/" mode="M51"/><axsl:apply-templates select="/" mode="M52"/><axsl:apply-templates select="/" mode="M53"/><axsl:apply-templates select="/" mode="M54"/><axsl:apply-templates select="/" mode="M55"/><axsl:apply-templates select="/" mode="M56"/><axsl:apply-templates select="/" mode="M57"/><axsl:apply-templates select="/" mode="M58"/><axsl:apply-templates select="/" mode="M59"/><axsl:apply-templates select="/" mode="M60"/><axsl:apply-templates select="/" mode="M61"/><axsl:apply-templates select="/" mode="M62"/><axsl:apply-templates select="/" mode="M63"/><axsl:apply-templates select="/" mode="M64"/><axsl:apply-templates select="/" mode="M65"/><axsl:apply-templates select="/" mode="M66"/><axsl:apply-templates select="/" mode="M67"/><axsl:apply-templates select="/" mode="M68"/><axsl:apply-templates select="/" mode="M69"/><axsl:apply-templates select="/" mode="M70"/><axsl:apply-templates select="/" mode="M71"/><axsl:apply-templates select="/" mode="M72"/><axsl:apply-templates select="/" mode="M73"/><axsl:apply-templates select="/" mode="M74"/><axsl:apply-templates select="/" mode="M75"/><axsl:apply-templates select="/" mode="M76"/><axsl:apply-templates select="/" mode="M77"/><axsl:apply-templates select="/" mode="M78"/><axsl:apply-templates select="/" mode="M79"/><axsl:apply-templates select="/" mode="M80"/><axsl:apply-templates select="/" mode="M81"/><axsl:apply-templates select="/" mode="M82"/><axsl:apply-templates select="/" mode="M83"/><axsl:apply-templates select="/" mode="M84"/><axsl:apply-templates select="/" mode="M85"/><axsl:apply-templates select="/" mode="M86"/><axsl:apply-templates select="/" mode="M87"/><axsl:apply-templates select="/" mode="M88"/><axsl:apply-templates select="/" mode="M89"/><axsl:apply-templates select="/" mode="M90"/><axsl:apply-templates select="/" mode="M91"/><axsl:apply-templates select="/" mode="M92"/><axsl:apply-templates select="/" mode="M93"/><axsl:apply-templates select="/" mode="M94"/><axsl:apply-templates select="/" mode="M95"/><axsl:apply-templates select="/" mode="M96"/><axsl:apply-templates select="/" mode="M97"/><axsl:apply-templates select="/" mode="M98"/><axsl:apply-templates select="/" mode="M99"/><axsl:apply-templates select="/" mode="M100"/><axsl:apply-templates select="/" mode="M101"/><axsl:apply-templates select="/" mode="M102"/><axsl:apply-templates select="/" mode="M103"/><axsl:apply-templates select="/" mode="M104"/><axsl:apply-templates select="/" mode="M105"/><axsl:apply-templates select="/" mode="M106"/><axsl:apply-templates select="/" mode="M107"/><axsl:apply-templates select="/" mode="M108"/><axsl:apply-templates select="/" mode="M109"/><axsl:apply-templates select="/" mode="M110"/><axsl:apply-templates select="/" mode="M111"/><axsl:apply-templates select="/" mode="M112"/><axsl:apply-templates select="/" mode="M113"/><axsl:apply-templates select="/" mode="M114"/><axsl:apply-templates select="/" mode="M115"/><axsl:apply-templates select="/" mode="M116"/><axsl:apply-templates select="/" mode="M117"/><axsl:apply-templates select="/" mode="M118"/><axsl:apply-templates select="/" mode="M119"/><axsl:apply-templates select="/" mode="M120"/><axsl:apply-templates select="/" mode="M121"/><axsl:apply-templates select="/" mode="M122"/><axsl:apply-templates select="/" mode="M123"/><axsl:apply-templates select="/" mode="M124"/><axsl:apply-templates select="/" mode="M125"/><axsl:apply-templates select="/" mode="M126"/><axsl:apply-templates select="/" mode="M127"/><axsl:apply-templates select="/" mode="M128"/><axsl:apply-templates select="/" mode="M129"/><axsl:apply-templates select="/" mode="M130"/><axsl:apply-templates select="/" mode="M131"/><axsl:apply-templates select="/" mode="M132"/><axsl:apply-templates select="/" mode="M133"/><axsl:apply-templates select="/" mode="M134"/><axsl:apply-templates select="/" mode="M135"/><axsl:apply-templates select="/" mode="M136"/><axsl:apply-templates select="/" mode="M137"/><axsl:apply-templates select="/" mode="M138"/><axsl:apply-templates select="/" mode="M139"/><axsl:apply-templates select="/" mode="M140"/><axsl:apply-templates select="/" mode="M141"/><axsl:apply-templates select="/" mode="M142"/><axsl:apply-templates select="/" mode="M143"/><axsl:apply-templates select="/" mode="M144"/><axsl:apply-templates select="/" mode="M145"/><axsl:apply-templates select="/" mode="M146"/><axsl:apply-templates select="/" mode="M147"/><axsl:apply-templates select="/" mode="M148"/><axsl:apply-templates select="/" mode="M149"/><axsl:apply-templates select="/" mode="M150"/><axsl:apply-templates select="/" mode="M151"/><axsl:apply-templates select="/" mode="M152"/><axsl:apply-templates select="/" mode="M153"/><axsl:apply-templates select="/" mode="M154"/><axsl:apply-templates select="/" mode="M155"/><axsl:apply-templates select="/" mode="M156"/><axsl:apply-templates select="/" mode="M157"/><axsl:apply-templates select="/" mode="M158"/><axsl:apply-templates select="/" mode="M159"/><axsl:apply-templates select="/" mode="M160"/><axsl:apply-templates select="/" mode="M161"/><axsl:apply-templates select="/" mode="M162"/><axsl:apply-templates select="/" mode="M163"/><axsl:apply-templates select="/" mode="M164"/><axsl:apply-templates select="/" mode="M165"/><axsl:apply-templates select="/" mode="M166"/><axsl:apply-templates select="/" mode="M167"/><axsl:apply-templates select="/" mode="M168"/><axsl:apply-templates select="/" mode="M169"/><axsl:apply-templates select="/" mode="M170"/><axsl:apply-templates select="/" mode="M171"/><axsl:apply-templates select="/" mode="M172"/><axsl:apply-templates select="/" mode="M173"/><axsl:apply-templates select="/" mode="M174"/><axsl:apply-templates select="/" mode="M175"/><axsl:apply-templates select="/" mode="M176"/><axsl:apply-templates select="/" mode="M177"/><axsl:apply-templates select="/" mode="M178"/><axsl:apply-templates select="/" mode="M179"/><axsl:apply-templates select="/" mode="M180"/><axsl:apply-templates select="/" mode="M181"/><axsl:apply-templates select="/" mode="M182"/><axsl:apply-templates select="/" mode="M183"/><axsl:apply-templates select="/" mode="M184"/><axsl:apply-templates select="/" mode="M185"/><axsl:apply-templates select="/" mode="M186"/><axsl:apply-templates select="/" mode="M187"/><axsl:apply-templates select="/" mode="M188"/><axsl:apply-templates select="/" mode="M189"/><axsl:apply-templates select="/" mode="M190"/><axsl:apply-templates select="/" mode="M191"/><axsl:apply-templates select="/" mode="M192"/><axsl:apply-templates select="/" mode="M193"/><axsl:apply-templates select="/" mode="M194"/><axsl:apply-templates select="/" mode="M195"/><axsl:apply-templates select="/" mode="M196"/><axsl:apply-templates select="/" mode="M197"/><axsl:apply-templates select="/" mode="M198"/><axsl:apply-templates select="/" mode="M199"/><axsl:apply-templates select="/" mode="M200"/><axsl:apply-templates select="/" mode="M201"/><axsl:apply-templates select="/" mode="M202"/><axsl:apply-templates select="/" mode="M203"/><axsl:apply-templates select="/" mode="M204"/><axsl:apply-templates select="/" mode="M205"/><axsl:apply-templates select="/" mode="M206"/><axsl:apply-templates select="/" mode="M207"/><axsl:apply-templates select="/" mode="M208"/><axsl:apply-templates select="/" mode="M209"/><axsl:apply-templates select="/" mode="M210"/><axsl:apply-templates select="/" mode="M211"/><axsl:apply-templates select="/" mode="M212"/><axsl:apply-templates select="/" mode="M213"/><axsl:apply-templates select="/" mode="M214"/><axsl:apply-templates select="/" mode="M215"/><axsl:apply-templates select="/" mode="M216"/><axsl:apply-templates select="/" mode="M217"/><axsl:apply-templates select="/" mode="M218"/><axsl:apply-templates select="/" mode="M219"/><axsl:apply-templates select="/" mode="M220"/><axsl:apply-templates select="/" mode="M221"/><axsl:apply-templates select="/" mode="M222"/><axsl:apply-templates select="/" mode="M223"/><axsl:apply-templates select="/" mode="M224"/><axsl:apply-templates select="/" mode="M225"/><axsl:apply-templates select="/" mode="M226"/><axsl:apply-templates select="/" mode="M227"/><axsl:apply-templates select="/" mode="M228"/><axsl:apply-templates select="/" mode="M229"/><axsl:apply-templates select="/" mode="M230"/><axsl:apply-templates select="/" mode="M231"/><axsl:apply-templates select="/" mode="M232"/><axsl:apply-templates select="/" mode="M233"/><axsl:apply-templates select="/" mode="M234"/><axsl:apply-templates select="/" mode="M235"/><axsl:apply-templates select="/" mode="M236"/><axsl:apply-templates select="/" mode="M237"/><axsl:apply-templates select="/" mode="M238"/><axsl:apply-templates select="/" mode="M239"/><axsl:apply-templates select="/" mode="M240"/><axsl:apply-templates select="/" mode="M241"/><axsl:apply-templates select="/" mode="M242"/><axsl:apply-templates select="/" mode="M243"/><axsl:apply-templates select="/" mode="M244"/><axsl:apply-templates select="/" mode="M245"/><axsl:apply-templates select="/" mode="M246"/><axsl:apply-templates select="/" mode="M247"/><axsl:apply-templates select="/" mode="M248"/><axsl:apply-templates select="/" mode="M249"/><axsl:apply-templates select="/" mode="M250"/><axsl:apply-templates select="/" mode="M251"/><axsl:apply-templates select="/" mode="M252"/><axsl:apply-templates select="/" mode="M253"/><axsl:apply-templates select="/" mode="M254"/><axsl:apply-templates select="/" mode="M255"/><axsl:apply-templates select="/" mode="M256"/><axsl:apply-templates select="/" mode="M257"/><axsl:apply-templates select="/" mode="M258"/><axsl:apply-templates select="/" mode="M259"/><axsl:apply-templates select="/" mode="M260"/><axsl:apply-templates select="/" mode="M261"/><axsl:apply-templates select="/" mode="M262"/><axsl:apply-templates select="/" mode="M263"/><axsl:apply-templates select="/" mode="M264"/><axsl:apply-templates select="/" mode="M265"/><axsl:apply-templates select="/" mode="M266"/><axsl:apply-templates select="/" mode="M267"/><axsl:apply-templates select="/" mode="M268"/><axsl:apply-templates select="/" mode="M269"/><axsl:apply-templates select="/" mode="M270"/><axsl:apply-templates select="/" mode="M271"/><axsl:apply-templates select="/" mode="M272"/><axsl:apply-templates select="/" mode="M273"/><axsl:apply-templates select="/" mode="M274"/><axsl:apply-templates select="/" mode="M275"/><axsl:apply-templates select="/" mode="M276"/><axsl:apply-templates select="/" mode="M277"/><axsl:apply-templates select="/" mode="M278"/><axsl:apply-templates select="/" mode="M279"/><axsl:apply-templates select="/" mode="M280"/><axsl:apply-templates select="/" mode="M281"/><axsl:apply-templates select="/" mode="M282"/><axsl:apply-templates select="/" mode="M283"/><axsl:apply-templates select="/" mode="M284"/><axsl:apply-templates select="/" mode="M285"/><axsl:apply-templates select="/" mode="M286"/><axsl:apply-templates select="/" mode="M287"/><axsl:apply-templates select="/" mode="M288"/><axsl:apply-templates select="/" mode="M289"/><axsl:apply-templates select="/" mode="M290"/><axsl:apply-templates select="/" mode="M291"/><axsl:apply-templates select="/" mode="M292"/><axsl:apply-templates select="/" mode="M293"/><axsl:apply-templates select="/" mode="M294"/><axsl:apply-templates select="/" mode="M295"/><axsl:apply-templates select="/" mode="M296"/><axsl:apply-templates select="/" mode="M297"/><axsl:apply-templates select="/" mode="M298"/><axsl:apply-templates select="/" mode="M299"/><axsl:apply-templates select="/" mode="M300"/><axsl:apply-templates select="/" mode="M301"/><axsl:apply-templates select="/" mode="M302"/><axsl:apply-templates select="/" mode="M303"/><axsl:apply-templates select="/" mode="M304"/><axsl:apply-templates select="/" mode="M305"/><axsl:apply-templates select="/" mode="M306"/><axsl:apply-templates select="/" mode="M307"/><axsl:apply-templates select="/" mode="M308"/><axsl:apply-templates select="/" mode="M309"/><axsl:apply-templates select="/" mode="M310"/><axsl:apply-templates select="/" mode="M311"/><axsl:apply-templates select="/" mode="M312"/><axsl:apply-templates select="/" mode="M313"/><axsl:apply-templates select="/" mode="M314"/><axsl:apply-templates select="/" mode="M315"/><axsl:apply-templates select="/" mode="M316"/><axsl:apply-templates select="/" mode="M317"/><axsl:apply-templates select="/" mode="M318"/><axsl:apply-templates select="/" mode="M319"/><axsl:apply-templates select="/" mode="M320"/><axsl:apply-templates select="/" mode="M321"/><axsl:apply-templates select="/" mode="M322"/><axsl:apply-templates select="/" mode="M323"/><axsl:apply-templates select="/" mode="M324"/><axsl:apply-templates select="/" mode="M325"/><axsl:apply-templates select="/" mode="M326"/><axsl:apply-templates select="/" mode="M327"/><axsl:apply-templates select="/" mode="M328"/></axsl:template>

<!--SCHEMATRON PATTERNS-->


<!--PATTERN oval_none_exist_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:tests/child::*" priority="1000" mode="M36">

		<!--REPORT -->
<axsl:if test="@check='none exist'">
                                             DEPRECATED ATTRIBUTE VALUE IN: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ATTRIBUTE VALUE:
                                        <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M36"/></axsl:template><axsl:template match="text()" priority="-1" mode="M36"/><axsl:template match="@*|node()" priority="-2" mode="M36"><axsl:apply-templates select="@*|*" mode="M36"/></axsl:template>

<!--PATTERN oval-def_empty_def_doc-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions" priority="1000" mode="M37">

		<!--ASSERT -->
<axsl:choose><axsl:when test="oval-def:definitions or oval-def:tests or oval-def:objects or oval-def:states or oval-def:variables"/><axsl:otherwise>A valid OVAL Definition document must contain at least one definitions, tests, objects, states, or variables element. The optional definitions, tests, objects, states, and variables sections define the specific characteristics that should be evaluated on a system to determine the truth values of the OVAL Definition Document. To be valid though, at least one definitions, tests, objects, states, or variables element must be present.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M37"/></axsl:template><axsl:template match="text()" priority="-1" mode="M37"/><axsl:template match="@*|node()" priority="-2" mode="M37"><axsl:apply-templates select="@*|*" mode="M37"/></axsl:template>

<!--PATTERN oval-def_required_criteria-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:definitions/oval-def:definition[@deprecated=false() or not(@deprecated)]" priority="1000" mode="M38">

		<!--ASSERT -->
<axsl:choose><axsl:when test="oval-def:criteria"/><axsl:otherwise>A valid OVAL Definition document must contain a criteria unless the definition is a deprecated definition.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M38"/></axsl:template><axsl:template match="text()" priority="-1" mode="M38"/><axsl:template match="@*|node()" priority="-2" mode="M38"><axsl:apply-templates select="@*|*" mode="M38"/></axsl:template>

<!--PATTERN oval-def_test_type-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:tests/*[@check_existence='none_exist']" priority="1000" mode="M39">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(*[local-name()='state'])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="@id"/><axsl:text/> - No state should be referenced when check_existence has a value of 'none_exist'.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M39"/></axsl:template><axsl:template match="text()" priority="-1" mode="M39"/><axsl:template match="@*|node()" priority="-2" mode="M39"><axsl:apply-templates select="@*|*" mode="M39"/></axsl:template>

<!--PATTERN oval-def_setobjref-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:object_reference" priority="1002" mode="M40">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../..) = name(ancestor::oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:object_reference" priority="1001" mode="M40">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../..) = name(ancestor::oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:set/oval-def:object_reference" priority="1000" mode="M40">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../../..) = name(ancestor::oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template><axsl:template match="text()" priority="-1" mode="M40"/><axsl:template match="@*|node()" priority="-2" mode="M40"><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template>

<!--PATTERN oval-def_literal_component-->


	<!--RULE -->
<axsl:template match="oval-def:literal_component" priority="1000" mode="M41">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype='record')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="ancestor::*/@id"/><axsl:text/> - The 'record' datatype is prohibited on variables.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M41"/></axsl:template><axsl:template match="text()" priority="-1" mode="M41"/><axsl:template match="@*|node()" priority="-2" mode="M41"><axsl:apply-templates select="@*|*" mode="M41"/></axsl:template>

<!--PATTERN oval-def_arithmeticfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:arithmetic/oval-def:literal_component" priority="1001" mode="M42">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='float' or @datatype='int'"/><axsl:otherwise>A literal_component used by an arithmetic function must have a datatype of float or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M42"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:arithmetic/oval-def:variable_component" priority="1000" mode="M42"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype='float' or ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype='int'"/><axsl:otherwise>The variable referenced by the arithmetic function must have a datatype of float or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M42"/></axsl:template><axsl:template match="text()" priority="-1" mode="M42"/><axsl:template match="@*|node()" priority="-2" mode="M42"><axsl:apply-templates select="@*|*" mode="M42"/></axsl:template>

<!--PATTERN oval-def_beginfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:begin/oval-def:literal_component" priority="1001" mode="M43">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the begin function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M43"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:begin/oval-def:variable_component" priority="1000" mode="M43"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the begin function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M43"/></axsl:template><axsl:template match="text()" priority="-1" mode="M43"/><axsl:template match="@*|node()" priority="-2" mode="M43"><axsl:apply-templates select="@*|*" mode="M43"/></axsl:template>

<!--PATTERN oval-def_concatfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:concat/oval-def:literal_component" priority="1001" mode="M44">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the concat function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:concat/oval-def:variable_component" priority="1000" mode="M44"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the concat function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template><axsl:template match="text()" priority="-1" mode="M44"/><axsl:template match="@*|node()" priority="-2" mode="M44"><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template>

<!--PATTERN oval-def_endfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:end/oval-def:literal_component" priority="1001" mode="M45">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the end function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M45"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:end/oval-def:variable_component" priority="1000" mode="M45"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the end function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M45"/></axsl:template><axsl:template match="text()" priority="-1" mode="M45"/><axsl:template match="@*|node()" priority="-2" mode="M45"><axsl:apply-templates select="@*|*" mode="M45"/></axsl:template>

<!--PATTERN oval-def_escaperegexfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:escape_regex/oval-def:literal_component" priority="1001" mode="M46">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the escape_regex function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M46"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:escape_regex/oval-def:variable_component" priority="1000" mode="M46"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the escape_regex function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M46"/></axsl:template><axsl:template match="text()" priority="-1" mode="M46"/><axsl:template match="@*|node()" priority="-2" mode="M46"><axsl:apply-templates select="@*|*" mode="M46"/></axsl:template>

<!--PATTERN oval-def_splitfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:split/oval-def:literal_component" priority="1001" mode="M47">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the split function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M47"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:split/oval-def:variable_component" priority="1000" mode="M47"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the split function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M47"/></axsl:template><axsl:template match="text()" priority="-1" mode="M47"/><axsl:template match="@*|node()" priority="-2" mode="M47"><axsl:apply-templates select="@*|*" mode="M47"/></axsl:template>

<!--PATTERN oval-def_substringfunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:substring/oval-def:literal_component" priority="1001" mode="M48">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the substring function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M48"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:substring/oval-def:variable_component" priority="1000" mode="M48"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the substring function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M48"/></axsl:template><axsl:template match="text()" priority="-1" mode="M48"/><axsl:template match="@*|node()" priority="-2" mode="M48"><axsl:apply-templates select="@*|*" mode="M48"/></axsl:template>

<!--PATTERN oval-def_timedifferencefunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:time_difference/oval-def:literal_component" priority="1001" mode="M49">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string' or @datatype='int'"/><axsl:otherwise>A literal_component used by the time_difference function must have a datatype of string or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M49"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:time_difference/oval-def:variable_component" priority="1000" mode="M49"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype='string' or ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype='int'"/><axsl:otherwise>The variable referenced by the time_difference function must have a datatype of string or int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M49"/></axsl:template><axsl:template match="text()" priority="-1" mode="M49"/><axsl:template match="@*|node()" priority="-2" mode="M49"><axsl:apply-templates select="@*|*" mode="M49"/></axsl:template>

<!--PATTERN oval-def_regexcapturefunctionrules-->


	<!--RULE -->
<axsl:template match="oval-def:regex_capture/oval-def:literal_component" priority="1001" mode="M50">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>A literal_component used by the regex_capture function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:regex_capture/oval-def:variable_component" priority="1000" mode="M50"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype = 'string'"/><axsl:otherwise>The variable referenced by the regex_capture function must have a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template><axsl:template match="text()" priority="-1" mode="M50"/><axsl:template match="@*|node()" priority="-2" mode="M50"><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template>

<!--PATTERN oval-def_definition_entity_rules-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@var_ref]|oval-def:states/*/*[@var_ref]|oval-def:states/*/*/*[@var_ref]" priority="1017" mode="M51"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test=".=''"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so no value should be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="( (not(@datatype)) and ('string' = ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype) ) or (@datatype = ancestor::oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype)"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="$var_ref"/><axsl:text/> - inconsistent datatype between the variable and an associated var_ref<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@var_ref]|oval-def:objects/*/*/*[@var_ref]" priority="1016" mode="M51">

		<!--REPORT -->
<axsl:if test="not(@var_check)">
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_check must also be provided<axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@var_check]|oval-def:objects/*/*/*[@var_check]" priority="1015" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@var_ref"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_check has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_ref must also be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:states/*/*[@var_ref]|oval-def:states/*/*/*[@var_ref]" priority="1014" mode="M51">

		<!--REPORT -->
<axsl:if test="not(@var_check)">
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_check must also be provided<axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:states/*/*[@var_check]|oval-def:states/*/*/*[@var_check]" priority="1013" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@var_ref"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var_check has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so a var_ref must also be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[not(@datatype)]|oval-def:states/*/*[not(@datatype)]|oval-def:states/*/*/*[not(@datatype)]" priority="1012" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='case insensitive equals' or @operation='case insensitive not equal' or @operation='pattern match'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given the lack of a declared datatype (hence a default datatype of string).<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='binary']|oval-def:states/*/*[@datatype='binary']|oval-def:states/*/*/*[@datatype='binary']" priority="1011" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of binary.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='boolean']|oval-def:states/*/*[@datatype='boolean']|oval-def:states/*/*/*[@datatype='boolean']" priority="1010" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of boolean.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='evr_string']|oval-def:states/*/*[@datatype='evr_string']|oval-def:states/*/*/*[@datatype='evr_string']" priority="1009" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or  @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of evr_string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='fileset_revision']|oval-def:states/*/*[@datatype='fileset_revision']|oval-def:states/*/*/*[@datatype='fileset_revision']" priority="1008" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or  @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of fileset_revision.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='float']|oval-def:states/*/*[@datatype='float']|oval-def:states/*/*/*[@datatype='float']" priority="1007" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of float.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='ios_version']|oval-def:states/*/*[@datatype='ios_version']|oval-def:states/*/*/*[@datatype='ios_version']" priority="1006" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of ios_version.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='int']|oval-def:states/*/*[@datatype='int']|oval-def:states/*/*/*[@datatype='int']" priority="1005" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='bitwise and' or @operation='bitwise or'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of int.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='ipv4_address']|oval-def:states/*/*[@datatype='ipv4_address']|oval-def:states/*/*/*[@datatype='ipv4_address']" priority="1004" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='subset of' or @operation='superset of'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of ipv4_address.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='ipv6_address']|oval-def:states/*/*[@datatype='ipv6_address']|oval-def:states/*/*/*[@datatype='ipv6_address']" priority="1003" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='subset of' or @operation='superset of'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of ipv6_address.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='string']|oval-def:states/*/*[@datatype='string']|oval-def:states/*/*/*[@datatype='string']" priority="1002" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='case insensitive equals' or @operation='case insensitive not equal' or @operation='pattern match'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of string.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='version']|oval-def:states/*/*[@datatype='version']|oval-def:states/*/*/*[@datatype='version']" priority="1001" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of version.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='record']|oval-def:states/*/*[@datatype='record']|oval-def:states/*/*/*[@datatype='record']" priority="1000" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of '<axsl:text/><axsl:value-of select="@operation"/><axsl:text/>' for the operation attribute of the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of record.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template><axsl:template match="text()" priority="-1" mode="M51"/><axsl:template match="@*|node()" priority="-2" mode="M51"><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

<!--PATTERN oval-def_no_var_ref_with_records-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[@datatype='record']|oval-def:states/*/*[@datatype='record']" priority="1000" mode="M52">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@var_ref)"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The use of var_ref is prohibited when the datatype is 'record'.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M52"/></axsl:template><axsl:template match="text()" priority="-1" mode="M52"/><axsl:template match="@*|node()" priority="-2" mode="M52"><axsl:apply-templates select="@*|*" mode="M52"/></axsl:template>

<!--PATTERN oval-def_definition_entity_type_check_rules-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*[not(@xsi:nil=true()) and not(@var_ref) and @datatype='int']|oval-def:states/*/*[not(@xsi:nil=true()) and not(@var_ref) and @datatype='int']" priority="1000" mode="M53">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'.'))) and (number(.) = floor(.))"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M53"/></axsl:template><axsl:template match="text()" priority="-1" mode="M53"/><axsl:template match="@*|node()" priority="-2" mode="M53"><axsl:apply-templates select="@*|*" mode="M53"/></axsl:template>

<!--PATTERN ind-def_famtst-->


	<!--RULE -->
<axsl:template match="ind-def:family_test/ind-def:object" priority="1001" mode="M54">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:family_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a family_test must reference a family_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:family_test/ind-def:state" priority="1000" mode="M54">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:family_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a family_test must reference a family_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template><axsl:template match="text()" priority="-1" mode="M54"/><axsl:template match="@*|node()" priority="-2" mode="M54"><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template>

<!--PATTERN ind-def_filehash_test_dep-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_test" priority="1000" mode="M55">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M55"/></axsl:template><axsl:template match="text()" priority="-1" mode="M55"/><axsl:template match="@*|node()" priority="-2" mode="M55"><axsl:apply-templates select="@*|*" mode="M55"/></axsl:template>

<!--PATTERN ind-def_hashtst-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_test/ind-def:object" priority="1001" mode="M56">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:filehash_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a filehash_test must reference a filesha1_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M56"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:filehash_test/ind-def:state" priority="1000" mode="M56">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:filehash_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a filehash_test must reference a filesha1_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M56"/></axsl:template><axsl:template match="text()" priority="-1" mode="M56"/><axsl:template match="@*|node()" priority="-2" mode="M56"><axsl:apply-templates select="@*|*" mode="M56"/></axsl:template>

<!--PATTERN ind-def_filehash_object_dep-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_object" priority="1000" mode="M57">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M57"/></axsl:template><axsl:template match="text()" priority="-1" mode="M57"/><axsl:template match="@*|node()" priority="-2" mode="M57"><axsl:apply-templates select="@*|*" mode="M57"/></axsl:template>

<!--PATTERN ind-def_hashobjfilepath-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_object/ind-def:filepath" priority="1000" mode="M58">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::ind-def:behaviors[@max_depth or @recurse_direction])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth and recurse_direction behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M58"/></axsl:template><axsl:template match="text()" priority="-1" mode="M58"/><axsl:template match="@*|node()" priority="-2" mode="M58"><axsl:apply-templates select="@*|*" mode="M58"/></axsl:template>

<!--PATTERN ind-def_hashobjfilename-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_object/ind-def:filename" priority="1000" mode="M59">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M59"/></axsl:template><axsl:template match="text()" priority="-1" mode="M59"/><axsl:template match="@*|node()" priority="-2" mode="M59"><axsl:apply-templates select="@*|*" mode="M59"/></axsl:template>

<!--PATTERN ind-def_filehash_state_dep-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state" priority="1000" mode="M60">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M60"/></axsl:template><axsl:template match="text()" priority="-1" mode="M60"/><axsl:template match="@*|node()" priority="-2" mode="M60"><axsl:apply-templates select="@*|*" mode="M60"/></axsl:template>

<!--PATTERN ind-def_filehash58_test-->


	<!--RULE -->
<axsl:template match="ind-def:filehash58_test/ind-def:object" priority="1001" mode="M61">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:filehash58_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a filehash58_test must reference a filehash58_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M61"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:filehash58_test/ind-def:state" priority="1000" mode="M61">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:filehash58_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a filehash58_test must reference a filehash58_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M61"/></axsl:template><axsl:template match="text()" priority="-1" mode="M61"/><axsl:template match="@*|node()" priority="-2" mode="M61"><axsl:apply-templates select="@*|*" mode="M61"/></axsl:template>

<!--PATTERN ind-def_hash58objfilepath-->


	<!--RULE -->
<axsl:template match="ind-def:filehash58_object/ind-def:filepath" priority="1000" mode="M62">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::ind-def:behaviors[@max_depth or @recurse_direction])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth and recurse_direction behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M62"/></axsl:template><axsl:template match="text()" priority="-1" mode="M62"/><axsl:template match="@*|node()" priority="-2" mode="M62"><axsl:apply-templates select="@*|*" mode="M62"/></axsl:template>

<!--PATTERN ind-def_hash58objfilename-->


	<!--RULE -->
<axsl:template match="ind-def:filehash58_object/ind-def:filename" priority="1000" mode="M63">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M63"/></axsl:template><axsl:template match="text()" priority="-1" mode="M63"/><axsl:template match="@*|node()" priority="-2" mode="M63"><axsl:apply-templates select="@*|*" mode="M63"/></axsl:template>

<!--PATTERN ind-def_environmentvariable_test_dep-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_test" priority="1000" mode="M64">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M64"/></axsl:template><axsl:template match="text()" priority="-1" mode="M64"/><axsl:template match="@*|node()" priority="-2" mode="M64"><axsl:apply-templates select="@*|*" mode="M64"/></axsl:template>

<!--PATTERN ind-def_envtst-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_test/ind-def:object" priority="1001" mode="M65">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:environmentvariable_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an environmentvariable_test must reference a environmentvariable_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M65"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_test/ind-def:state" priority="1000" mode="M65">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:environmentvariable_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an environmentvariable_test must reference a environmentvariable_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M65"/></axsl:template><axsl:template match="text()" priority="-1" mode="M65"/><axsl:template match="@*|node()" priority="-2" mode="M65"><axsl:apply-templates select="@*|*" mode="M65"/></axsl:template>

<!--PATTERN ind-def_environmentvariable_object_dep-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_object" priority="1000" mode="M66">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M66"/></axsl:template><axsl:template match="text()" priority="-1" mode="M66"/><axsl:template match="@*|node()" priority="-2" mode="M66"><axsl:apply-templates select="@*|*" mode="M66"/></axsl:template>

<!--PATTERN ind-def_environmentvariable_state_dep-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_state" priority="1000" mode="M67">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M67"/></axsl:template><axsl:template match="text()" priority="-1" mode="M67"/><axsl:template match="@*|node()" priority="-2" mode="M67"><axsl:apply-templates select="@*|*" mode="M67"/></axsl:template>

<!--PATTERN ind-def_env58tst-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable58_test/ind-def:object" priority="1001" mode="M68">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:environmentvariable58_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an environmentvariable58_test must reference a environmentvariable58_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M68"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:environmentvariable58_test/ind-def:state" priority="1000" mode="M68">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:environmentvariable58_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an environmentvariable58_test must reference a environmentvariable58_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M68"/></axsl:template><axsl:template match="text()" priority="-1" mode="M68"/><axsl:template match="@*|node()" priority="-2" mode="M68"><axsl:apply-templates select="@*|*" mode="M68"/></axsl:template>

<!--PATTERN ind-def_ldap_test_dep-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_test" priority="1000" mode="M69">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M69"/></axsl:template><axsl:template match="text()" priority="-1" mode="M69"/><axsl:template match="@*|node()" priority="-2" mode="M69"><axsl:apply-templates select="@*|*" mode="M69"/></axsl:template>

<!--PATTERN ind-def_ldaptst-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_test/ind-def:object" priority="1001" mode="M70">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:ldap_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an ldap_test must reference an ldap_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M70"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:ldap_test/ind-def:state" priority="1000" mode="M70">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:ldap_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an ldap_test must reference an ldap_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M70"/></axsl:template><axsl:template match="text()" priority="-1" mode="M70"/><axsl:template match="@*|node()" priority="-2" mode="M70"><axsl:apply-templates select="@*|*" mode="M70"/></axsl:template>

<!--PATTERN ind-def_ldap_object_dep-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_object" priority="1000" mode="M71">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M71"/></axsl:template><axsl:template match="text()" priority="-1" mode="M71"/><axsl:template match="@*|node()" priority="-2" mode="M71"><axsl:apply-templates select="@*|*" mode="M71"/></axsl:template>

<!--PATTERN ind-def_ldap_state_dep-->


	<!--RULE -->
<axsl:template match="ind-def:ldap_state" priority="1000" mode="M72">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M72"/></axsl:template><axsl:template match="text()" priority="-1" mode="M72"/><axsl:template match="@*|node()" priority="-2" mode="M72"><axsl:apply-templates select="@*|*" mode="M72"/></axsl:template>

<!--PATTERN ind-def_ldap57_test-->


	<!--RULE -->
<axsl:template match="ind-def:ldap57_test/ind-def:object" priority="1001" mode="M73">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:ldap57_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an ldap57_test must reference an ldap57_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M73"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:ldap57_test/ind-def:state" priority="1000" mode="M73">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:ldap57_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an ldap57_test must reference an ldap57_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M73"/></axsl:template><axsl:template match="text()" priority="-1" mode="M73"/><axsl:template match="@*|node()" priority="-2" mode="M73"><axsl:apply-templates select="@*|*" mode="M73"/></axsl:template>

<!--PATTERN ind-def_ldap57stevalue-->


	<!--RULE -->
<axsl:template match="ind-def:ldap57_state/ind-def:value" priority="1000" mode="M74">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='record'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the value entity of a ldap57_state must be 'record'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M74"/></axsl:template><axsl:template match="text()" priority="-1" mode="M74"/><axsl:template match="@*|node()" priority="-2" mode="M74"><axsl:apply-templates select="@*|*" mode="M74"/></axsl:template>

<!--PATTERN ind-def_sql_test_dep-->


	<!--RULE -->
<axsl:template match="ind-def:sql_test" priority="1000" mode="M75">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M75"/></axsl:template><axsl:template match="text()" priority="-1" mode="M75"/><axsl:template match="@*|node()" priority="-2" mode="M75"><axsl:apply-templates select="@*|*" mode="M75"/></axsl:template>

<!--PATTERN ind-def_sqltst-->


	<!--RULE -->
<axsl:template match="ind-def:sql_test/ind-def:object" priority="1001" mode="M76">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:sql_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sql_test must reference a sql_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M76"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:sql_test/ind-def:state" priority="1000" mode="M76">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:sql_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sql_test must reference a sql_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M76"/></axsl:template><axsl:template match="text()" priority="-1" mode="M76"/><axsl:template match="@*|node()" priority="-2" mode="M76"><axsl:apply-templates select="@*|*" mode="M76"/></axsl:template>

<!--PATTERN ind-def_sql_object_dep-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object" priority="1000" mode="M77">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M77"/></axsl:template><axsl:template match="text()" priority="-1" mode="M77"/><axsl:template match="@*|node()" priority="-2" mode="M77"><axsl:apply-templates select="@*|*" mode="M77"/></axsl:template>

<!--PATTERN ind-def_sqlobjdengine-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:engine" priority="1000" mode="M78">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the engine entity of an sql_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M78"/></axsl:template><axsl:template match="text()" priority="-1" mode="M78"/><axsl:template match="@*|node()" priority="-2" mode="M78"><axsl:apply-templates select="@*|*" mode="M78"/></axsl:template>

<!--PATTERN ind-def_sqlobjversion-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:version" priority="1000" mode="M79">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the version entity of an sql_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M79"/></axsl:template><axsl:template match="text()" priority="-1" mode="M79"/><axsl:template match="@*|node()" priority="-2" mode="M79"><axsl:apply-templates select="@*|*" mode="M79"/></axsl:template>

<!--PATTERN ind-def_sqlobjconnection_string-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:connection_string" priority="1000" mode="M80">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the connection_string entity of an sql_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M80"/></axsl:template><axsl:template match="text()" priority="-1" mode="M80"/><axsl:template match="@*|node()" priority="-2" mode="M80"><axsl:apply-templates select="@*|*" mode="M80"/></axsl:template>

<!--PATTERN ind-def_sqlobjsql-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:sql" priority="1000" mode="M81">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the sql entity of an sql_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M81"/></axsl:template><axsl:template match="text()" priority="-1" mode="M81"/><axsl:template match="@*|node()" priority="-2" mode="M81"><axsl:apply-templates select="@*|*" mode="M81"/></axsl:template>

<!--PATTERN ind-def_sql_state_dep-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state" priority="1000" mode="M82">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M82"/></axsl:template><axsl:template match="text()" priority="-1" mode="M82"/><axsl:template match="@*|node()" priority="-2" mode="M82"><axsl:apply-templates select="@*|*" mode="M82"/></axsl:template>

<!--PATTERN ind-def_sql57_test-->


	<!--RULE -->
<axsl:template match="ind-def:sql57_test/ind-def:object" priority="1001" mode="M83">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:sql57_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sql57_test must reference a sql57_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M83"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:sql57_test/ind-def:state" priority="1000" mode="M83">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:sql57_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sql57_test must reference a sql57_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M83"/></axsl:template><axsl:template match="text()" priority="-1" mode="M83"/><axsl:template match="@*|node()" priority="-2" mode="M83"><axsl:apply-templates select="@*|*" mode="M83"/></axsl:template>

<!--PATTERN ind-def_sql57_object_dengine-->


	<!--RULE -->
<axsl:template match="ind-def:sql57_object/ind-def:engine" priority="1000" mode="M84">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the engine entity of an sql57_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M84"/></axsl:template><axsl:template match="text()" priority="-1" mode="M84"/><axsl:template match="@*|node()" priority="-2" mode="M84"><axsl:apply-templates select="@*|*" mode="M84"/></axsl:template>

<!--PATTERN ind-def_sql57_object_version-->


	<!--RULE -->
<axsl:template match="ind-def:sql57_object/ind-def:version" priority="1000" mode="M85">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the version entity of an sql57_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M85"/></axsl:template><axsl:template match="text()" priority="-1" mode="M85"/><axsl:template match="@*|node()" priority="-2" mode="M85"><axsl:apply-templates select="@*|*" mode="M85"/></axsl:template>

<!--PATTERN ind-def_sql57_object_connection_string-->


	<!--RULE -->
<axsl:template match="ind-def:sql57_object/ind-def:connection_string" priority="1000" mode="M86">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the connection_string entity of an sql57_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M86"/></axsl:template><axsl:template match="text()" priority="-1" mode="M86"/><axsl:template match="@*|node()" priority="-2" mode="M86"><axsl:apply-templates select="@*|*" mode="M86"/></axsl:template>

<!--PATTERN ind-def_sql57_object_sql-->


	<!--RULE -->
<axsl:template match="ind-def:sql57_object/ind-def:sql" priority="1000" mode="M87">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the sql entity of an sql57_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M87"/></axsl:template><axsl:template match="text()" priority="-1" mode="M87"/><axsl:template match="@*|node()" priority="-2" mode="M87"><axsl:apply-templates select="@*|*" mode="M87"/></axsl:template>

<!--PATTERN ind-def_sql57steresult-->


	<!--RULE -->
<axsl:template match="ind-def:sql57_state/ind-def:result" priority="1000" mode="M88">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='record'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the result entity of a sql57_state must be 'record'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M88"/></axsl:template><axsl:template match="text()" priority="-1" mode="M88"/><axsl:template match="@*|node()" priority="-2" mode="M88"><axsl:apply-templates select="@*|*" mode="M88"/></axsl:template>

<!--PATTERN ind-def_txt54tst-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_test/ind-def:object" priority="1001" mode="M89">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:textfilecontent54_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a textfilecontent54_test must reference a textfilecontent54_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M89"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_test/ind-def:state" priority="1000" mode="M89">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:textfilecontent54_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a textfilecontent54_test must reference a textfilecontent54_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M89"/></axsl:template><axsl:template match="text()" priority="-1" mode="M89"/><axsl:template match="@*|node()" priority="-2" mode="M89"><axsl:apply-templates select="@*|*" mode="M89"/></axsl:template>

<!--PATTERN ind-def_txt54objfilepath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:filepath" priority="1000" mode="M90">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::ind-def:behaviors[@max_depth or @recurse_direction])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth and recurse_direction behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M90"/></axsl:template><axsl:template match="text()" priority="-1" mode="M90"/><axsl:template match="@*|node()" priority="-2" mode="M90"><axsl:apply-templates select="@*|*" mode="M90"/></axsl:template>

<!--PATTERN ind-def_txt54objfilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:filename" priority="1000" mode="M91">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M91"/></axsl:template><axsl:template match="text()" priority="-1" mode="M91"/><axsl:template match="@*|node()" priority="-2" mode="M91"><axsl:apply-templates select="@*|*" mode="M91"/></axsl:template>

<!--PATTERN ind-def_txt54objpattern-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:pattern" priority="1000" mode="M92">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@operation='pattern match'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the pattern entity of a textfilecontent54_object should be 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M92"/></axsl:template><axsl:template match="text()" priority="-1" mode="M92"/><axsl:template match="@*|node()" priority="-2" mode="M92"><axsl:apply-templates select="@*|*" mode="M92"/></axsl:template>

<!--PATTERN ind-def_txttst_dep-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_test" priority="1000" mode="M93">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M93"/></axsl:template><axsl:template match="text()" priority="-1" mode="M93"/><axsl:template match="@*|node()" priority="-2" mode="M93"><axsl:apply-templates select="@*|*" mode="M93"/></axsl:template>

<!--PATTERN ind-def_txttst-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_test/ind-def:object" priority="1001" mode="M94">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:textfilecontent_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a textfilecontent_test must reference a textfilecontent_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M94"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_test/ind-def:state" priority="1000" mode="M94">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:textfilecontent_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a textfilecontent_test must reference a textfilecontent_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M94"/></axsl:template><axsl:template match="text()" priority="-1" mode="M94"/><axsl:template match="@*|node()" priority="-2" mode="M94"><axsl:apply-templates select="@*|*" mode="M94"/></axsl:template>

<!--PATTERN ind-def_txtobj_dep-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object" priority="1000" mode="M95">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M95"/></axsl:template><axsl:template match="text()" priority="-1" mode="M95"/><axsl:template match="@*|node()" priority="-2" mode="M95"><axsl:apply-templates select="@*|*" mode="M95"/></axsl:template>

<!--PATTERN ind-def_txtobjfilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object/ind-def:filename" priority="1000" mode="M96">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M96"/></axsl:template><axsl:template match="text()" priority="-1" mode="M96"/><axsl:template match="@*|node()" priority="-2" mode="M96"><axsl:apply-templates select="@*|*" mode="M96"/></axsl:template>

<!--PATTERN ind-def_txtobjline-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object/ind-def:line" priority="1000" mode="M97">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@operation='pattern match'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the line entity of a textfilecontent_object should be 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M97"/></axsl:template><axsl:template match="text()" priority="-1" mode="M97"/><axsl:template match="@*|node()" priority="-2" mode="M97"><axsl:apply-templates select="@*|*" mode="M97"/></axsl:template>

<!--PATTERN ind-def_txtste_dep-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state" priority="1000" mode="M98">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M98"/></axsl:template><axsl:template match="text()" priority="-1" mode="M98"/><axsl:template match="@*|node()" priority="-2" mode="M98"><axsl:apply-templates select="@*|*" mode="M98"/></axsl:template>

<!--PATTERN ind-def_vattst-->


	<!--RULE -->
<axsl:template match="ind-def:variable_test/ind-def:object" priority="1001" mode="M99">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:variable_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a variable_test must reference a variable_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M99"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:variable_test/ind-def:state" priority="1000" mode="M99">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:variable_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a variable_test must reference a variable_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M99"/></axsl:template><axsl:template match="text()" priority="-1" mode="M99"/><axsl:template match="@*|node()" priority="-2" mode="M99"><axsl:apply-templates select="@*|*" mode="M99"/></axsl:template>

<!--PATTERN ind-def_varobjvar_ref-->


	<!--RULE -->
<axsl:template match="ind-def:variable_object/ind-def:var_ref" priority="1000" mode="M100">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@var_ref)"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - var_ref attribute for the var_ref entity of a variable_object is prohibited.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M100"/></axsl:template><axsl:template match="text()" priority="-1" mode="M100"/><axsl:template match="@*|node()" priority="-2" mode="M100"><axsl:apply-templates select="@*|*" mode="M100"/></axsl:template>

<!--PATTERN ind-def_varobjvar_ref_exists-->


	<!--RULE -->
<axsl:template match="ind-def:variable_object/ind-def:var_ref" priority="1000" mode="M101"><axsl:variable name="varId" select="."/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id = $varId]"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - referenced variable <axsl:text/><axsl:value-of select="."/><axsl:text/> not found. The var_ref entity must hold a variable id that exists in the document.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M101"/></axsl:template><axsl:template match="text()" priority="-1" mode="M101"/><axsl:template match="@*|node()" priority="-2" mode="M101"><axsl:apply-templates select="@*|*" mode="M101"/></axsl:template>

<!--PATTERN ind-def_varstevar_ref-->


	<!--RULE -->
<axsl:template match="ind-def:variable_state/ind-def:var_ref" priority="1000" mode="M102">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@var_ref)"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - var_ref attribute for the var_ref entity of a variable_state is prohibited.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M102"/></axsl:template><axsl:template match="text()" priority="-1" mode="M102"/><axsl:template match="@*|node()" priority="-2" mode="M102"><axsl:apply-templates select="@*|*" mode="M102"/></axsl:template>

<!--PATTERN ind-def_varstevar_ref_exists-->


	<!--RULE -->
<axsl:template match="ind-def:variable_state/ind-def:var_ref" priority="1000" mode="M103"><axsl:variable name="varId" select="."/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="ancestor::oval-def:oval_definitions/oval-def:variables/*[@id =  $varId]"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - referenced variable <axsl:text/><axsl:value-of select="."/><axsl:text/> not found. The var_ref entity must hold a variable id that exists in the document.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M103"/></axsl:template><axsl:template match="text()" priority="-1" mode="M103"/><axsl:template match="@*|node()" priority="-2" mode="M103"><axsl:apply-templates select="@*|*" mode="M103"/></axsl:template>

<!--PATTERN ind-def_xmltst-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_test/ind-def:object" priority="1001" mode="M104">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ind-def:xmlfilecontent_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a xmlfilecontent_test must reference a xmlfilecontent_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M104"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_test/ind-def:state" priority="1000" mode="M104">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ind-def:xmlfilecontent_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a xmlfilecontent_test must reference a xmlfilecontent_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M104"/></axsl:template><axsl:template match="text()" priority="-1" mode="M104"/><axsl:template match="@*|node()" priority="-2" mode="M104"><axsl:apply-templates select="@*|*" mode="M104"/></axsl:template>

<!--PATTERN ind-def_xmlobjfilepath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:filepath" priority="1000" mode="M105">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::ind-def:behaviors[@max_depth or @recurse_direction])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth and recurse_direction behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M105"/></axsl:template><axsl:template match="text()" priority="-1" mode="M105"/><axsl:template match="@*|node()" priority="-2" mode="M105"><axsl:apply-templates select="@*|*" mode="M105"/></axsl:template>

<!--PATTERN ind-def_xmlobjfilename-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:filename" priority="1000" mode="M106">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M106"/></axsl:template><axsl:template match="text()" priority="-1" mode="M106"/><axsl:template match="@*|node()" priority="-2" mode="M106"><axsl:apply-templates select="@*|*" mode="M106"/></axsl:template>

<!--PATTERN ind-def_xmlobjxpath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:xpath" priority="1000" mode="M107">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the xpath entity of a xmlfilecontent_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M107"/></axsl:template><axsl:template match="text()" priority="-1" mode="M107"/><axsl:template match="@*|node()" priority="-2" mode="M107"><axsl:apply-templates select="@*|*" mode="M107"/></axsl:template>

<!--PATTERN ind-def_ldaptype_timestamp_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:states/ind-def:ldap_state/ind-def:ldaptype|oval-def:oval_definitions/oval-def:states/ind-def:ldap57_state/ind-def:ldaptype" priority="1000" mode="M108">

		<!--REPORT -->
<axsl:if test=".='LDAPTYPE_TIMESTAMP'">
                                                            DEPRECATED ELEMENT VALUE IN: ldap_state ELEMENT VALUE: <axsl:text/><axsl:value-of select="."/><axsl:text/>
                                                      <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M108"/></axsl:template><axsl:template match="text()" priority="-1" mode="M108"/><axsl:template match="@*|node()" priority="-2" mode="M108"><axsl:apply-templates select="@*|*" mode="M108"/></axsl:template>

<!--PATTERN ind-def_ldaptype_email_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:states/ind-def:ldap_state/ind-def:ldaptype|oval-def:oval_definitions/oval-def:states/ind-def:ldap57_state/ind-def:ldaptype" priority="1000" mode="M109">

		<!--REPORT -->
<axsl:if test=".='LDAPTYPE_EMAIL'">
                                                            DEPRECATED ELEMENT VALUE IN: ldap_state ELEMENT VALUE: <axsl:text/><axsl:value-of select="."/><axsl:text/>
                                                      <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M109"/></axsl:template><axsl:template match="text()" priority="-1" mode="M109"/><axsl:template match="@*|node()" priority="-2" mode="M109"><axsl:apply-templates select="@*|*" mode="M109"/></axsl:template>

<!--PATTERN aix-def_interimfixtst-->


	<!--RULE -->
<axsl:template match="aix-def:interim_fix_test/aix-def:object" priority="1001" mode="M110">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/aix-def:interim_fix_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a <axsl:text/><axsl:value-of select="name(..)"/><axsl:text/> must reference a interim_fix_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M110"/></axsl:template>

	<!--RULE -->
<axsl:template match="aix-def:interim_fix_test/aix-def:state" priority="1000" mode="M110">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/aix-def:interim_fix_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a <axsl:text/><axsl:value-of select="name(..)"/><axsl:text/> must reference a interim_fix_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M110"/></axsl:template><axsl:template match="text()" priority="-1" mode="M110"/><axsl:template match="@*|node()" priority="-2" mode="M110"><axsl:apply-templates select="@*|*" mode="M110"/></axsl:template>

<!--PATTERN aix-def_filesettst-->


	<!--RULE -->
<axsl:template match="aix-def:fileset_test/aix-def:object" priority="1001" mode="M111">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/aix-def:fileset_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileset_test must reference a fileset_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M111"/></axsl:template>

	<!--RULE -->
<axsl:template match="aix-def:fileset_test/aix-def:state" priority="1000" mode="M111">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/aix-def:fileset_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileset_test must reference a fileset_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M111"/></axsl:template><axsl:template match="text()" priority="-1" mode="M111"/><axsl:template match="@*|node()" priority="-2" mode="M111"><axsl:apply-templates select="@*|*" mode="M111"/></axsl:template>

<!--PATTERN aix-def_fixtst-->


	<!--RULE -->
<axsl:template match="aix-def:fix_test/aix-def:object" priority="1001" mode="M112">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/aix-def:fix_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fix_test must reference a fix_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M112"/></axsl:template>

	<!--RULE -->
<axsl:template match="aix-def:fix_test/aix-def:state" priority="1000" mode="M112">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/aix-def:fix_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fix_test must reference a fix_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M112"/></axsl:template><axsl:template match="text()" priority="-1" mode="M112"/><axsl:template match="@*|node()" priority="-2" mode="M112"><axsl:apply-templates select="@*|*" mode="M112"/></axsl:template>

<!--PATTERN aix-def_notst-->


	<!--RULE -->
<axsl:template match="aix-def:no_test/aix-def:object" priority="1001" mode="M113">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/aix-def:no_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a <axsl:text/><axsl:value-of select="name(..)"/><axsl:text/> must reference a no_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M113"/></axsl:template>

	<!--RULE -->
<axsl:template match="aix-def:no_test/aix-def:state" priority="1000" mode="M113">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/aix-def:no_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a <axsl:text/><axsl:value-of select="name(..)"/><axsl:text/> must reference a no_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M113"/></axsl:template><axsl:template match="text()" priority="-1" mode="M113"/><axsl:template match="@*|node()" priority="-2" mode="M113"><axsl:apply-templates select="@*|*" mode="M113"/></axsl:template>

<!--PATTERN aix-def_osleveltst-->


	<!--RULE -->
<axsl:template match="aix-def:oslevel_test/aix-def:object" priority="1001" mode="M114">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/aix-def:oslevel_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a oslevel_test must reference a oslevel_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M114"/></axsl:template>

	<!--RULE -->
<axsl:template match="aix-def:oslevel_test/aix-def:state" priority="1000" mode="M114">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/aix-def:oslevel_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a oslevel_test must reference a oslevel_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M114"/></axsl:template><axsl:template match="text()" priority="-1" mode="M114"/><axsl:template match="@*|node()" priority="-2" mode="M114"><axsl:apply-templates select="@*|*" mode="M114"/></axsl:template>

<!--PATTERN apache-def_httpd_test_dep-->


	<!--RULE -->
<axsl:template match="apache-def:httpd_test" priority="1000" mode="M115">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M115"/></axsl:template><axsl:template match="text()" priority="-1" mode="M115"/><axsl:template match="@*|node()" priority="-2" mode="M115"><axsl:apply-templates select="@*|*" mode="M115"/></axsl:template>

<!--PATTERN apache-def_httpdtst-->


	<!--RULE -->
<axsl:template match="apache-def:httpd_test/apache-def:object" priority="1001" mode="M116">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/apache-def:httpd_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a httpd_test must reference a httpd_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M116"/></axsl:template>

	<!--RULE -->
<axsl:template match="apache-def:httpd_test/apache-def:state" priority="1000" mode="M116">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/apache-def:httpd_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a httpd_test must reference a httpd_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M116"/></axsl:template><axsl:template match="text()" priority="-1" mode="M116"/><axsl:template match="@*|node()" priority="-2" mode="M116"><axsl:apply-templates select="@*|*" mode="M116"/></axsl:template>

<!--PATTERN apache-def_httpd_object_dep-->


	<!--RULE -->
<axsl:template match="apache-def:httpd_object" priority="1000" mode="M117">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M117"/></axsl:template><axsl:template match="text()" priority="-1" mode="M117"/><axsl:template match="@*|node()" priority="-2" mode="M117"><axsl:apply-templates select="@*|*" mode="M117"/></axsl:template>

<!--PATTERN apache-def_httpd_state_dep-->


	<!--RULE -->
<axsl:template match="apache-def:httpd_state" priority="1000" mode="M118">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M118"/></axsl:template><axsl:template match="text()" priority="-1" mode="M118"/><axsl:template match="@*|node()" priority="-2" mode="M118"><axsl:apply-templates select="@*|*" mode="M118"/></axsl:template>

<!--PATTERN catos-def_linetst-->


	<!--RULE -->
<axsl:template match="catos-def:line_test/catos-def:object" priority="1001" mode="M119">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/catos-def:line_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a line_test must reference a line_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M119"/></axsl:template>

	<!--RULE -->
<axsl:template match="catos-def:line_test/catos-def:state" priority="1000" mode="M119">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/catos-def:line_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a line_test must reference a line_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M119"/></axsl:template><axsl:template match="text()" priority="-1" mode="M119"/><axsl:template match="@*|node()" priority="-2" mode="M119"><axsl:apply-templates select="@*|*" mode="M119"/></axsl:template>

<!--PATTERN catos-def_moduletst-->


	<!--RULE -->
<axsl:template match="catos-def:module_test/catos-def:object" priority="1001" mode="M120">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/catos-def:module_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a module_test must reference a module_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M120"/></axsl:template>

	<!--RULE -->
<axsl:template match="catos-def:module_test/catos-def:state" priority="1000" mode="M120">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/catos-def:module_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a module_test must reference a module_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M120"/></axsl:template><axsl:template match="text()" priority="-1" mode="M120"/><axsl:template match="@*|node()" priority="-2" mode="M120"><axsl:apply-templates select="@*|*" mode="M120"/></axsl:template>

<!--PATTERN catos-def_version55_test-->


	<!--RULE -->
<axsl:template match="catos-def:version55_test/catos-def:object" priority="1001" mode="M121">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/catos-def:version55_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a version55_test must reference a version55_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M121"/></axsl:template>

	<!--RULE -->
<axsl:template match="catos-def:version55_test/catos-def:state" priority="1000" mode="M121">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/catos-def:version55_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a version55_test must reference a version55_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M121"/></axsl:template><axsl:template match="text()" priority="-1" mode="M121"/><axsl:template match="@*|node()" priority="-2" mode="M121"><axsl:apply-templates select="@*|*" mode="M121"/></axsl:template>

<!--PATTERN catos-def_version_test_dep-->


	<!--RULE -->
<axsl:template match="catos-def:version_test" priority="1000" mode="M122">

		<!--REPORT -->
<axsl:if test="true()">
                                      DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M122"/></axsl:template><axsl:template match="text()" priority="-1" mode="M122"/><axsl:template match="@*|node()" priority="-2" mode="M122"><axsl:apply-templates select="@*|*" mode="M122"/></axsl:template>

<!--PATTERN catos-def_version_test-->


	<!--RULE -->
<axsl:template match="catos-def:version_test/catos-def:object" priority="1001" mode="M123">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/catos-def:version_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a version_test must reference a version_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M123"/></axsl:template>

	<!--RULE -->
<axsl:template match="catos-def:version_test/catos-def:state" priority="1000" mode="M123">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/catos-def:version_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a version_test must reference a version_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M123"/></axsl:template><axsl:template match="text()" priority="-1" mode="M123"/><axsl:template match="@*|node()" priority="-2" mode="M123"><axsl:apply-templates select="@*|*" mode="M123"/></axsl:template>

<!--PATTERN catos-def_version_object_dep-->


	<!--RULE -->
<axsl:template match="catos-def:version_object" priority="1000" mode="M124">

		<!--REPORT -->
<axsl:if test="true()">
                                      DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M124"/></axsl:template><axsl:template match="text()" priority="-1" mode="M124"/><axsl:template match="@*|node()" priority="-2" mode="M124"><axsl:apply-templates select="@*|*" mode="M124"/></axsl:template>

<!--PATTERN catos-def_version_state_dep-->


	<!--RULE -->
<axsl:template match="catos-def:version_state" priority="1000" mode="M125">

		<!--REPORT -->
<axsl:if test="true()">
                                      DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M125"/></axsl:template><axsl:template match="text()" priority="-1" mode="M125"/><axsl:template match="@*|node()" priority="-2" mode="M125"><axsl:apply-templates select="@*|*" mode="M125"/></axsl:template>

<!--PATTERN esx-def_patch56tst-->


	<!--RULE -->
<axsl:template match="esx-def:patch56_test/esx-def:object" priority="1001" mode="M126">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/esx-def:patch56_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a patch56_test must reference a patch56_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M126"/></axsl:template>

	<!--RULE -->
<axsl:template match="esx-def:patch56_test/esx-def:state" priority="1000" mode="M126">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/esx-def:patch56_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a patch56_test must reference a patch56_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M126"/></axsl:template><axsl:template match="text()" priority="-1" mode="M126"/><axsl:template match="@*|node()" priority="-2" mode="M126"><axsl:apply-templates select="@*|*" mode="M126"/></axsl:template>

<!--PATTERN esx-def_patchtst_dep-->


	<!--RULE -->
<axsl:template match="esx-def:patch_test" priority="1000" mode="M127">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M127"/></axsl:template><axsl:template match="text()" priority="-1" mode="M127"/><axsl:template match="@*|node()" priority="-2" mode="M127"><axsl:apply-templates select="@*|*" mode="M127"/></axsl:template>

<!--PATTERN esx-def_patchtst-->


	<!--RULE -->
<axsl:template match="esx-def:patch_test/esx-def:object" priority="1001" mode="M128">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/esx-def:patch_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a patch_test must reference a patch_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M128"/></axsl:template>

	<!--RULE -->
<axsl:template match="esx-def:patch_test/esx-def:state" priority="1000" mode="M128">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/esx-def:patch_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a patch_test must reference a patch_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M128"/></axsl:template><axsl:template match="text()" priority="-1" mode="M128"/><axsl:template match="@*|node()" priority="-2" mode="M128"><axsl:apply-templates select="@*|*" mode="M128"/></axsl:template>

<!--PATTERN esx-def_patchobj_dep-->


	<!--RULE -->
<axsl:template match="esx-def:patch_object" priority="1000" mode="M129">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M129"/></axsl:template><axsl:template match="text()" priority="-1" mode="M129"/><axsl:template match="@*|node()" priority="-2" mode="M129"><axsl:apply-templates select="@*|*" mode="M129"/></axsl:template>

<!--PATTERN esx-def_patchste_dep-->


	<!--RULE -->
<axsl:template match="esx-def:patch_state" priority="1000" mode="M130">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M130"/></axsl:template><axsl:template match="text()" priority="-1" mode="M130"/><axsl:template match="@*|node()" priority="-2" mode="M130"><axsl:apply-templates select="@*|*" mode="M130"/></axsl:template>

<!--PATTERN esx-def_versiontst-->


	<!--RULE -->
<axsl:template match="esx-def:version_test/esx-def:object" priority="1001" mode="M131">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/esx-def:version_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a version_test must reference a version_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M131"/></axsl:template>

	<!--RULE -->
<axsl:template match="esx-def:version_test/esx-def:state" priority="1000" mode="M131">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/esx-def:version_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a version_test must reference a version_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M131"/></axsl:template><axsl:template match="text()" priority="-1" mode="M131"/><axsl:template match="@*|node()" priority="-2" mode="M131"><axsl:apply-templates select="@*|*" mode="M131"/></axsl:template>

<!--PATTERN esx-def_visdkmanagedobjecttst-->


	<!--RULE -->
<axsl:template match="esx-def:visdkmanagedobject_test/esx-def:object" priority="1001" mode="M132">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/esx-def:visdkmanagedobject_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a visdkmanagedobject_test must reference a visdkmanagedobject_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M132"/></axsl:template>

	<!--RULE -->
<axsl:template match="esx-def:visdkmanagedobject_test/esx-def:state" priority="1000" mode="M132">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/esx-def:visdkmanagedobject_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a visdkmanagedobject_test must reference a visdkmanagedobject_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M132"/></axsl:template><axsl:template match="text()" priority="-1" mode="M132"/><axsl:template match="@*|node()" priority="-2" mode="M132"><axsl:apply-templates select="@*|*" mode="M132"/></axsl:template>

<!--PATTERN freebsd-def_portinfotst-->


	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_test/freebsd-def:object" priority="1001" mode="M133">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/freebsd-def:portinfo_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a portinfo_test must reference an portinfo_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M133"/></axsl:template>

	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_test/freebsd-def:state" priority="1000" mode="M133">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/freebsd-def:portinfo_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a portinfo_test must reference an portinfo_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M133"/></axsl:template><axsl:template match="text()" priority="-1" mode="M133"/><axsl:template match="@*|node()" priority="-2" mode="M133"><axsl:apply-templates select="@*|*" mode="M133"/></axsl:template>

<!--PATTERN hpux-def_getconf_test-->


	<!--RULE -->
<axsl:template match="hpux-def:getconf_test/hpux-def:object" priority="1001" mode="M134">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/hpux-def:getconf_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an getconf_test must reference an getconf_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M134"/></axsl:template>

	<!--RULE -->
<axsl:template match="hpux-def:getconf_test/hpux-def:state" priority="1000" mode="M134">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/hpux-def:getconf_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an getconf_test must reference an getconf_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M134"/></axsl:template><axsl:template match="text()" priority="-1" mode="M134"/><axsl:template match="@*|node()" priority="-2" mode="M134"><axsl:apply-templates select="@*|*" mode="M134"/></axsl:template>

<!--PATTERN hpux-def_ndd_test-->


	<!--RULE -->
<axsl:template match="hpux-def:ndd_test/hpux-def:object" priority="1001" mode="M135">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/hpux-def:ndd_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an ndd_test must reference an ndd_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M135"/></axsl:template>

	<!--RULE -->
<axsl:template match="hpux-def:ndd_test/hpux-def:state" priority="1000" mode="M135">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/hpux-def:ndd_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an ndd_test must reference an ndd_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M135"/></axsl:template><axsl:template match="text()" priority="-1" mode="M135"/><axsl:template match="@*|node()" priority="-2" mode="M135"><axsl:apply-templates select="@*|*" mode="M135"/></axsl:template>

<!--PATTERN hpux-def_patch53_test-->


	<!--RULE -->
<axsl:template match="hpux-def:patch53_test/hpux-def:object" priority="1001" mode="M136">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/hpux-def:patch53_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an patch53_test must reference an patch53_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M136"/></axsl:template>

	<!--RULE -->
<axsl:template match="hpux-def:patch53_test/hpux-def:state" priority="1000" mode="M136">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/hpux-def:patch53_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an patch53_test must reference an patch53_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M136"/></axsl:template><axsl:template match="text()" priority="-1" mode="M136"/><axsl:template match="@*|node()" priority="-2" mode="M136"><axsl:apply-templates select="@*|*" mode="M136"/></axsl:template>

<!--PATTERN hpux-def_patch_test_dep-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_test" priority="1000" mode="M137">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M137"/></axsl:template><axsl:template match="text()" priority="-1" mode="M137"/><axsl:template match="@*|node()" priority="-2" mode="M137"><axsl:apply-templates select="@*|*" mode="M137"/></axsl:template>

<!--PATTERN hpux-def_patch_test-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_test/hpux-def:object" priority="1001" mode="M138">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/hpux-def:patch_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an patch_test must reference an patch_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M138"/></axsl:template>

	<!--RULE -->
<axsl:template match="hpux-def:patch_test/hpux-def:state" priority="1000" mode="M138">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/hpux-def:patch_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an patch_test must reference an patch_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M138"/></axsl:template><axsl:template match="text()" priority="-1" mode="M138"/><axsl:template match="@*|node()" priority="-2" mode="M138"><axsl:apply-templates select="@*|*" mode="M138"/></axsl:template>

<!--PATTERN hpux-def_patch_object_dep-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_object" priority="1000" mode="M139">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M139"/></axsl:template><axsl:template match="text()" priority="-1" mode="M139"/><axsl:template match="@*|node()" priority="-2" mode="M139"><axsl:apply-templates select="@*|*" mode="M139"/></axsl:template>

<!--PATTERN hpux-def_patch_state_dep-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_state" priority="1000" mode="M140">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M140"/></axsl:template><axsl:template match="text()" priority="-1" mode="M140"/><axsl:template match="@*|node()" priority="-2" mode="M140"><axsl:apply-templates select="@*|*" mode="M140"/></axsl:template>

<!--PATTERN hpux-def_swlist_test-->


	<!--RULE -->
<axsl:template match="hpux-def:swlist_test/hpux-def:object" priority="1001" mode="M141">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/hpux-def:swlist_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an swlist_test must reference an swlist_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M141"/></axsl:template>

	<!--RULE -->
<axsl:template match="hpux-def:swlist_test/hpux-def:state" priority="1000" mode="M141">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/hpux-def:swlist_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an swlist_test must reference an swlist_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M141"/></axsl:template><axsl:template match="text()" priority="-1" mode="M141"/><axsl:template match="@*|node()" priority="-2" mode="M141"><axsl:apply-templates select="@*|*" mode="M141"/></axsl:template>

<!--PATTERN hpux-def_trusted_test-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_test/hpux-def:object" priority="1001" mode="M142">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/hpux-def:trusted_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an trusted_test must reference an trusted_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M142"/></axsl:template>

	<!--RULE -->
<axsl:template match="hpux-def:trusted_test/hpux-def:state" priority="1000" mode="M142">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/hpux-def:trusted_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an trusted_test must reference an trusted_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M142"/></axsl:template><axsl:template match="text()" priority="-1" mode="M142"/><axsl:template match="@*|node()" priority="-2" mode="M142"><axsl:apply-templates select="@*|*" mode="M142"/></axsl:template>

<!--PATTERN ios-def_globaltst-->


	<!--RULE -->
<axsl:template match="ios-def:global_test/ios-def:object" priority="1001" mode="M143">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ios-def:global_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a global_test must reference a global_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M143"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:global_test/ios-def:state" priority="1000" mode="M143">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ios-def:global_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a global_test must reference a global_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M143"/></axsl:template><axsl:template match="text()" priority="-1" mode="M143"/><axsl:template match="@*|node()" priority="-2" mode="M143"><axsl:apply-templates select="@*|*" mode="M143"/></axsl:template>

<!--PATTERN ios-def_interfacetst-->


	<!--RULE -->
<axsl:template match="ios-def:interface_test/ios-def:object" priority="1001" mode="M144">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ios-def:interface_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an interface_test must reference an interface_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M144"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:interface_test/ios-def:state" priority="1000" mode="M144">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ios-def:interface_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an interface_test must reference an interface_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M144"/></axsl:template><axsl:template match="text()" priority="-1" mode="M144"/><axsl:template match="@*|node()" priority="-2" mode="M144"><axsl:apply-templates select="@*|*" mode="M144"/></axsl:template>

<!--PATTERN ios-def_linetst-->


	<!--RULE -->
<axsl:template match="ios-def:line_test/ios-def:object" priority="1001" mode="M145">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ios-def:line_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a line_test must reference a line_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M145"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:line_test/ios-def:state" priority="1000" mode="M145">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ios-def:line_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a line_test must reference a line_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M145"/></axsl:template><axsl:template match="text()" priority="-1" mode="M145"/><axsl:template match="@*|node()" priority="-2" mode="M145"><axsl:apply-templates select="@*|*" mode="M145"/></axsl:template>

<!--PATTERN ios-def_snmptst-->


	<!--RULE -->
<axsl:template match="ios-def:snmp_test/ios-def:object" priority="1001" mode="M146">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ios-def:snmp_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a snmp_test must reference a snmp_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M146"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:snmp_test/ios-def:state" priority="1000" mode="M146">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ios-def:snmp_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a snmp_test must reference a snmp_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M146"/></axsl:template><axsl:template match="text()" priority="-1" mode="M146"/><axsl:template match="@*|node()" priority="-2" mode="M146"><axsl:apply-templates select="@*|*" mode="M146"/></axsl:template>

<!--PATTERN ios-def_tclshtst-->


	<!--RULE -->
<axsl:template match="ios-def:tclsh_test/ios-def:object" priority="1001" mode="M147">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ios-def:tclsh_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a tclsh_test must reference a tclsh_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M147"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:tclsh_test/ios-def:state" priority="1000" mode="M147">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ios-def:tclsh_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a tclsh_test must reference a tclsh_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M147"/></axsl:template><axsl:template match="text()" priority="-1" mode="M147"/><axsl:template match="@*|node()" priority="-2" mode="M147"><axsl:apply-templates select="@*|*" mode="M147"/></axsl:template>

<!--PATTERN ios-def_ver55tst-->


	<!--RULE -->
<axsl:template match="ios-def:version55_test/ios-def:object" priority="1001" mode="M148">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ios-def:version55_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a version55_test must reference a version_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M148"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:version55_test/ios-def:state" priority="1000" mode="M148">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ios-def:version55_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a version55_test must reference a version_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M148"/></axsl:template><axsl:template match="text()" priority="-1" mode="M148"/><axsl:template match="@*|node()" priority="-2" mode="M148"><axsl:apply-templates select="@*|*" mode="M148"/></axsl:template>

<!--PATTERN ios-def_vertst_dep-->


	<!--RULE -->
<axsl:template match="ios-def:version_test" priority="1000" mode="M149">

		<!--REPORT -->
<axsl:if test="true()">
                                      DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M149"/></axsl:template><axsl:template match="text()" priority="-1" mode="M149"/><axsl:template match="@*|node()" priority="-2" mode="M149"><axsl:apply-templates select="@*|*" mode="M149"/></axsl:template>

<!--PATTERN ios-def_vertst-->


	<!--RULE -->
<axsl:template match="ios-def:version_test/ios-def:object" priority="1001" mode="M150">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/ios-def:version_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a version_test must reference a version_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M150"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:version_test/ios-def:state" priority="1000" mode="M150">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/ios-def:version_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a version_test must reference a version_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M150"/></axsl:template><axsl:template match="text()" priority="-1" mode="M150"/><axsl:template match="@*|node()" priority="-2" mode="M150"><axsl:apply-templates select="@*|*" mode="M150"/></axsl:template>

<!--PATTERN ios-def_verobj_dep-->


	<!--RULE -->
<axsl:template match="ios-def:version_object" priority="1000" mode="M151">

		<!--REPORT -->
<axsl:if test="true()">
                                    DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                              <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M151"/></axsl:template><axsl:template match="text()" priority="-1" mode="M151"/><axsl:template match="@*|node()" priority="-2" mode="M151"><axsl:apply-templates select="@*|*" mode="M151"/></axsl:template>

<!--PATTERN ios-def_verste_dep-->


	<!--RULE -->
<axsl:template match="ios-def:version_state" priority="1000" mode="M152">

		<!--REPORT -->
<axsl:if test="true()">
                                    DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                              <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M152"/></axsl:template><axsl:template match="text()" priority="-1" mode="M152"/><axsl:template match="@*|node()" priority="-2" mode="M152"><axsl:apply-templates select="@*|*" mode="M152"/></axsl:template>

<!--PATTERN linux-def_dpkginfo_test-->


	<!--RULE -->
<axsl:template match="linux-def:dpkginfo_test/linux-def:object" priority="1001" mode="M153">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/linux-def:dpkginfo_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an dpkginfo_test must reference an dpkginfo_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M153"/></axsl:template>

	<!--RULE -->
<axsl:template match="linux-def:dpkginfo_test/linux-def:state" priority="1000" mode="M153">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/linux-def:dpkginfo_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an dpkginfo_test must reference an dpkginfo_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M153"/></axsl:template><axsl:template match="text()" priority="-1" mode="M153"/><axsl:template match="@*|node()" priority="-2" mode="M153"><axsl:apply-templates select="@*|*" mode="M153"/></axsl:template>

<!--PATTERN linux-def_iflisteners_test-->


	<!--RULE -->
<axsl:template match="linux-def:iflisteners_test/linux-def:object" priority="1001" mode="M154">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/linux-def:iflisteners_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an iflisteners_test must reference an iflisteners_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M154"/></axsl:template>

	<!--RULE -->
<axsl:template match="linux-def:iflisteners_test/linux-def:state" priority="1000" mode="M154">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/linux-def:iflisteners_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an iflisteners_test must reference an iflisteners_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M154"/></axsl:template><axsl:template match="text()" priority="-1" mode="M154"/><axsl:template match="@*|node()" priority="-2" mode="M154"><axsl:apply-templates select="@*|*" mode="M154"/></axsl:template>

<!--PATTERN linux-def_inetlisteningservers_test-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_test/linux-def:object" priority="1001" mode="M155">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/linux-def:inetlisteningservers_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an inetlisteningservers_test must reference an inetlisteningservers_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M155"/></axsl:template>

	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_test/linux-def:state" priority="1000" mode="M155">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/linux-def:inetlisteningservers_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an inetlisteningservers_test must reference an inetlisteningservers_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M155"/></axsl:template><axsl:template match="text()" priority="-1" mode="M155"/><axsl:template match="@*|node()" priority="-2" mode="M155"><axsl:apply-templates select="@*|*" mode="M155"/></axsl:template>

<!--PATTERN linux-def_partitiontst-->


	<!--RULE -->
<axsl:template match="linux-def:partition_test/linux-def:object" priority="1001" mode="M156">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/linux-def:partition_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a partition_test must reference a partition_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M156"/></axsl:template>

	<!--RULE -->
<axsl:template match="linux-def:partition_test/linux-def:state" priority="1000" mode="M156">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/linux-def:partition_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a partition_test must reference a partition_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M156"/></axsl:template><axsl:template match="text()" priority="-1" mode="M156"/><axsl:template match="@*|node()" priority="-2" mode="M156"><axsl:apply-templates select="@*|*" mode="M156"/></axsl:template>

<!--PATTERN linux-def_rpminfo_test-->


	<!--RULE -->
<axsl:template match="linux-def:rpminfo_test/linux-def:object" priority="1001" mode="M157">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/linux-def:rpminfo_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an rpminfo_test must reference an rpminfo_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M157"/></axsl:template>

	<!--RULE -->
<axsl:template match="linux-def:rpminfo_test/linux-def:state" priority="1000" mode="M157">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/linux-def:rpminfo_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an rpminfo_test must reference an rpminfo_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M157"/></axsl:template><axsl:template match="text()" priority="-1" mode="M157"/><axsl:template match="@*|node()" priority="-2" mode="M157"><axsl:apply-templates select="@*|*" mode="M157"/></axsl:template>

<!--PATTERN linux-def_rpmverify_test-->


	<!--RULE -->
<axsl:template match="linux-def:rpmverify_test/linux-def:object" priority="1001" mode="M158">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/linux-def:rpmverify_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an rpmverify_test must reference an rpmverify_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M158"/></axsl:template>

	<!--RULE -->
<axsl:template match="linux-def:rpmverify_test/linux-def:state" priority="1000" mode="M158">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/linux-def:rpmverify_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an rpmverify_test must reference an rpmverify_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M158"/></axsl:template><axsl:template match="text()" priority="-1" mode="M158"/><axsl:template match="@*|node()" priority="-2" mode="M158"><axsl:apply-templates select="@*|*" mode="M158"/></axsl:template>

<!--PATTERN linux-def_selinuxbooleantst-->


	<!--RULE -->
<axsl:template match="linux-def:selinuxboolean_test/linux-def:object" priority="1001" mode="M159">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/linux-def:selinuxboolean_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an selinuxboolean_test must reference an selinuxboolean_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M159"/></axsl:template>

	<!--RULE -->
<axsl:template match="linux-def:selinuxboolean_test/linux-def:state" priority="1000" mode="M159">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/linux-def:selinuxboolean_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an selinuxboolean_test must reference an selinuxboolean_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M159"/></axsl:template><axsl:template match="text()" priority="-1" mode="M159"/><axsl:template match="@*|node()" priority="-2" mode="M159"><axsl:apply-templates select="@*|*" mode="M159"/></axsl:template>

<!--PATTERN linux-def_selinuxsecuritycontexttst-->


	<!--RULE -->
<axsl:template match="linux-def:selinuxsecuritycontext_test/linux-def:object" priority="1001" mode="M160">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/linux-def:selinuxsecuritycontext_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an selinuxsecuritycontext_test must reference an selinuxsecuritycontext_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M160"/></axsl:template>

	<!--RULE -->
<axsl:template match="linux-def:selinuxsecuritycontext_test/linux-def:state" priority="1000" mode="M160">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/linux-def:selinuxsecuritycontext_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an selinuxsecuritycontext_test must reference an selinuxsecuritycontext_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M160"/></axsl:template><axsl:template match="text()" priority="-1" mode="M160"/><axsl:template match="@*|node()" priority="-2" mode="M160"><axsl:apply-templates select="@*|*" mode="M160"/></axsl:template>

<!--PATTERN linux-def_selinuxsecuritycontext_objectfilename-->


	<!--RULE -->
<axsl:template match="linux-def:selinuxsecuritycontext_object/linux-def:filename" priority="1000" mode="M161">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or (@xsi:nil='true' and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M161"/></axsl:template><axsl:template match="text()" priority="-1" mode="M161"/><axsl:template match="@*|node()" priority="-2" mode="M161"><axsl:apply-templates select="@*|*" mode="M161"/></axsl:template>

<!--PATTERN linux-def_slackwarepkginfo_test-->


	<!--RULE -->
<axsl:template match="linux-def:slackwarepkginfo_test/linux-def:object" priority="1001" mode="M162">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/linux-def:slackwarepkginfo_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an slackwarepkginfo_test must reference an slackwarepkginfo_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M162"/></axsl:template>

	<!--RULE -->
<axsl:template match="linux-def:slackwarepkginfo_test/linux-def:state" priority="1000" mode="M162">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/linux-def:slackwarepkginfo_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an slackwarepkginfo_test must reference an slackwarepkginfo_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M162"/></axsl:template><axsl:template match="text()" priority="-1" mode="M162"/><axsl:template match="@*|node()" priority="-2" mode="M162"><axsl:apply-templates select="@*|*" mode="M162"/></axsl:template>

<!--PATTERN macos-def_accountinfo_test-->


	<!--RULE -->
<axsl:template match="macos-def:accountinfo_test/macos-def:object" priority="1001" mode="M163">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/macos-def:accountinfo_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an accountinfo_test must reference an accountinfo_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M163"/></axsl:template>

	<!--RULE -->
<axsl:template match="macos-def:accountinfo_test/macos-def:state" priority="1000" mode="M163">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/macos-def:accountinfo_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an accountinfo_test must reference an accountinfo_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M163"/></axsl:template><axsl:template match="text()" priority="-1" mode="M163"/><axsl:template match="@*|node()" priority="-2" mode="M163"><axsl:apply-templates select="@*|*" mode="M163"/></axsl:template>

<!--PATTERN macos-def_diskutiltst-->


	<!--RULE -->
<axsl:template match="macos-def:diskutil_test/macos-def:object" priority="1001" mode="M164">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/macos-def:diskutil_object/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a diskutil_test must reference a diskutil_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M164"/></axsl:template>

	<!--RULE -->
<axsl:template match="macos-def:diskutil_test/macos-def:state" priority="1000" mode="M164">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/macos-def:diskutil_state/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a diskutil_test must reference a diskutil_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M164"/></axsl:template><axsl:template match="text()" priority="-1" mode="M164"/><axsl:template match="@*|node()" priority="-2" mode="M164"><axsl:apply-templates select="@*|*" mode="M164"/></axsl:template>

<!--PATTERN macos-def_inetlisteningservers_test-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_test/macos-def:object" priority="1001" mode="M165">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/macos-def:inetlisteningservers_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an inetlisteningservers_test must reference an inetlisteningservers_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M165"/></axsl:template>

	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_test/macos-def:state" priority="1000" mode="M165">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/macos-def:inetlisteningservers_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an inetlisteningservers_test must reference an inetlisteningservers_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M165"/></axsl:template><axsl:template match="text()" priority="-1" mode="M165"/><axsl:template match="@*|node()" priority="-2" mode="M165"><axsl:apply-templates select="@*|*" mode="M165"/></axsl:template>

<!--PATTERN macos-def_nvram_test-->


	<!--RULE -->
<axsl:template match="macos-def:nvram_test/macos-def:object" priority="1001" mode="M166">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/macos-def:nvram_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an nvram_test must reference an nvram_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M166"/></axsl:template>

	<!--RULE -->
<axsl:template match="macos-def:nvram_test/macos-def:state" priority="1000" mode="M166">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/macos-def:nvram_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an nvram_test must reference an nvram_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M166"/></axsl:template><axsl:template match="text()" priority="-1" mode="M166"/><axsl:template match="@*|node()" priority="-2" mode="M166"><axsl:apply-templates select="@*|*" mode="M166"/></axsl:template>

<!--PATTERN macos-def_plist_test-->


	<!--RULE -->
<axsl:template match="macos-def:plist_test/macos-def:object" priority="1001" mode="M167">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/macos-def:plist_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a plist_test must reference a plist_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M167"/></axsl:template>

	<!--RULE -->
<axsl:template match="macos-def:plist_test/macos-def:state" priority="1000" mode="M167">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/macos-def:plist_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a plist_test must reference a plist_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M167"/></axsl:template><axsl:template match="text()" priority="-1" mode="M167"/><axsl:template match="@*|node()" priority="-2" mode="M167"><axsl:apply-templates select="@*|*" mode="M167"/></axsl:template>

<!--PATTERN macos-def_plistobjfilepath-->


	<!--RULE -->
<axsl:template match="macos-def:plist_object/macos-def:filepath" priority="1000" mode="M168">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
                                                                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the filepath entity of a plist_object should be 'equals'
                                                                                    <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M168"/></axsl:template><axsl:template match="text()" priority="-1" mode="M168"/><axsl:template match="@*|node()" priority="-2" mode="M168"><axsl:apply-templates select="@*|*" mode="M168"/></axsl:template>

<!--PATTERN macos-def_pwpolicy_test_dep-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_test" priority="1000" mode="M169">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M169"/></axsl:template><axsl:template match="text()" priority="-1" mode="M169"/><axsl:template match="@*|node()" priority="-2" mode="M169"><axsl:apply-templates select="@*|*" mode="M169"/></axsl:template>

<!--PATTERN macos-def_pwpolicy_test-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_test/macos-def:object" priority="1001" mode="M170">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/macos-def:pwpolicy_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an pwpolicy_test must reference an pwpolicy_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M170"/></axsl:template>

	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_test/macos-def:state" priority="1000" mode="M170">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/macos-def:pwpolicy_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an pwpolicy_test must reference an pwpolicy_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M170"/></axsl:template><axsl:template match="text()" priority="-1" mode="M170"/><axsl:template match="@*|node()" priority="-2" mode="M170"><axsl:apply-templates select="@*|*" mode="M170"/></axsl:template>

<!--PATTERN macos-def_pwpolicy_object_dep-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_object" priority="1000" mode="M171">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M171"/></axsl:template><axsl:template match="text()" priority="-1" mode="M171"/><axsl:template match="@*|node()" priority="-2" mode="M171"><axsl:apply-templates select="@*|*" mode="M171"/></axsl:template>

<!--PATTERN macos-def_pwpobjuserpass-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_object/macos-def:userpass" priority="1000" mode="M172">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the userpass entity of a pwpolicy_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M172"/></axsl:template><axsl:template match="text()" priority="-1" mode="M172"/><axsl:template match="@*|node()" priority="-2" mode="M172"><axsl:apply-templates select="@*|*" mode="M172"/></axsl:template>

<!--PATTERN macos-def_pwpolicy_state_dep-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state" priority="1000" mode="M173">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M173"/></axsl:template><axsl:template match="text()" priority="-1" mode="M173"/><axsl:template match="@*|node()" priority="-2" mode="M173"><axsl:apply-templates select="@*|*" mode="M173"/></axsl:template>

<!--PATTERN macos-def_pwpolicy59_test-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy59_test/macos-def:object" priority="1001" mode="M174">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/macos-def:pwpolicy59_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an pwpolicy59_test must reference an pwpolicy59_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M174"/></axsl:template>

	<!--RULE -->
<axsl:template match="macos-def:pwpolicy59_test/macos-def:state" priority="1000" mode="M174">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/macos-def:pwpolicy59_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an pwpolicy59_test must reference an pwpolicy59_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M174"/></axsl:template><axsl:template match="text()" priority="-1" mode="M174"/><axsl:template match="@*|node()" priority="-2" mode="M174"><axsl:apply-templates select="@*|*" mode="M174"/></axsl:template>

<!--PATTERN macos-def_pwp59objusername-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy59_object/macos-def:username" priority="1000" mode="M175">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@xsi:nil=true()) or ../macos-def:userpass/@xsi:nil=true()"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - userpass entity must be nil when username entity is nil<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M175"/></axsl:template><axsl:template match="text()" priority="-1" mode="M175"/><axsl:template match="@*|node()" priority="-2" mode="M175"><axsl:apply-templates select="@*|*" mode="M175"/></axsl:template>

<!--PATTERN macos-def_pwp59objuserpass-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy59_object/macos-def:userpass" priority="1000" mode="M176">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the userpass entity of a pwpolicy59_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@xsi:nil=true()) or ../macos-def:username/@xsi:nil=true()"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - username entity must be nil when userpass entity is nil<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M176"/></axsl:template><axsl:template match="text()" priority="-1" mode="M176"/><axsl:template match="@*|node()" priority="-2" mode="M176"><axsl:apply-templates select="@*|*" mode="M176"/></axsl:template>

<!--PATTERN pixos-def_linetst-->


	<!--RULE -->
<axsl:template match="pixos-def:line_test/pixos-def:object" priority="1001" mode="M177">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/pixos-def:line_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a line_test must reference a line_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M177"/></axsl:template>

	<!--RULE -->
<axsl:template match="pixos-def:line_test/pixos-def:state" priority="1000" mode="M177">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/pixos-def:line_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a line_test must reference a line_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M177"/></axsl:template><axsl:template match="text()" priority="-1" mode="M177"/><axsl:template match="@*|node()" priority="-2" mode="M177"><axsl:apply-templates select="@*|*" mode="M177"/></axsl:template>

<!--PATTERN pixos-def_vertst-->


	<!--RULE -->
<axsl:template match="pixos-def:version_test/pixos-def:object" priority="1001" mode="M178">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/pixos-def:version_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a version_test must reference a version_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M178"/></axsl:template>

	<!--RULE -->
<axsl:template match="pixos-def:version_test/pixos-def:state" priority="1000" mode="M178">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/pixos-def:version_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a version_test must reference a version_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M178"/></axsl:template><axsl:template match="text()" priority="-1" mode="M178"/><axsl:template match="@*|node()" priority="-2" mode="M178"><axsl:apply-templates select="@*|*" mode="M178"/></axsl:template>

<!--PATTERN sp-def_webapptst-->


	<!--RULE -->
<axsl:template match="sp-def:spwebapplication_test/sp-def:object" priority="1001" mode="M179">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spwebapplication_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a spwebapplication_test must reference an spwebapplication_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M179"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spwebapplication_test/sp-def:state" priority="1000" mode="M179">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spwebapplication_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a spwebapplication_test must reference an spwebapplication_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M179"/></axsl:template><axsl:template match="text()" priority="-1" mode="M179"/><axsl:template match="@*|node()" priority="-2" mode="M179"><axsl:apply-templates select="@*|*" mode="M179"/></axsl:template>

<!--PATTERN sp-def_grptst-->


	<!--RULE -->
<axsl:template match="sp-def:spgroup_test/sp-def:object" priority="1001" mode="M180">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spgroup_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a spgroup_test must reference a spgroup_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M180"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spgroup_test/sp-def:state" priority="1000" mode="M180">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spgroup_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a spgroup_test must reference a spgroup_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M180"/></axsl:template><axsl:template match="text()" priority="-1" mode="M180"/><axsl:template match="@*|node()" priority="-2" mode="M180"><axsl:apply-templates select="@*|*" mode="M180"/></axsl:template>

<!--PATTERN sp-def_webtst-->


	<!--RULE -->
<axsl:template match="sp-def:spweb_test/sp-def:object" priority="1001" mode="M181">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spweb_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a spweb_test must reference an spweb_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M181"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spweb_test/sp-def:state" priority="1000" mode="M181">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spweb_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a spweb_test must reference an spweb_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M181"/></axsl:template><axsl:template match="text()" priority="-1" mode="M181"/><axsl:template match="@*|node()" priority="-2" mode="M181"><axsl:apply-templates select="@*|*" mode="M181"/></axsl:template>

<!--PATTERN sp-def_listtst-->


	<!--RULE -->
<axsl:template match="sp-def:splist_test/sp-def:object" priority="1001" mode="M182">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:splist_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a splist_test must reference an splist_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M182"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:splist_test/sp-def:state" priority="1000" mode="M182">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:splist_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a splist_test must reference an splist_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M182"/></axsl:template><axsl:template match="text()" priority="-1" mode="M182"/><axsl:template match="@*|node()" priority="-2" mode="M182"><axsl:apply-templates select="@*|*" mode="M182"/></axsl:template>

<!--PATTERN sp-def_avstst-->


	<!--RULE -->
<axsl:template match="sp-def:spantivirussettings_test/sp-def:object" priority="1001" mode="M183">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spantivirussettings_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a spantivirussettings_test must reference an spantivirussettings_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M183"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spantivirussettings_test/sp-def:state" priority="1000" mode="M183">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spantivirussettings_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a spantivirussettings_test must reference an spantivirussettings_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M183"/></axsl:template><axsl:template match="text()" priority="-1" mode="M183"/><axsl:template match="@*|node()" priority="-2" mode="M183"><axsl:apply-templates select="@*|*" mode="M183"/></axsl:template>

<!--PATTERN sp-def_siteadmintst-->


	<!--RULE -->
<axsl:template match="sp-def:spsiteadministration_test/sp-def:object" priority="1001" mode="M184">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spsiteadministration_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a spsiteadministration_test must reference an spsiteadministration_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M184"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spsiteadministration_test/sp-def:state" priority="1000" mode="M184">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spsiteadministration_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a spsiteadministration_test must reference an spsiteadministration_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M184"/></axsl:template><axsl:template match="text()" priority="-1" mode="M184"/><axsl:template match="@*|node()" priority="-2" mode="M184"><axsl:apply-templates select="@*|*" mode="M184"/></axsl:template>

<!--PATTERN sp-def_sitetst-->


	<!--RULE -->
<axsl:template match="sp-def:spsite_test/sp-def:object" priority="1001" mode="M185">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spsite_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a spsite_test must reference an spsite_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M185"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spsite_test/sp-def:state" priority="1000" mode="M185">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spsite_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a spsite_test must reference an spsite_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M185"/></axsl:template><axsl:template match="text()" priority="-1" mode="M185"/><axsl:template match="@*|node()" priority="-2" mode="M185"><axsl:apply-templates select="@*|*" mode="M185"/></axsl:template>

<!--PATTERN sp-def_crtst-->


	<!--RULE -->
<axsl:template match="sp-def:spcrawlrule_test/sp-def:object" priority="1001" mode="M186">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spcrawlrule_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a spcrawlrule_test must reference an spcrawlrule_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M186"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spcrawlrule_test/sp-def:state" priority="1000" mode="M186">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spcrawlrule_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a spcrawlrule_test must reference an spcrawlrule_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M186"/></axsl:template><axsl:template match="text()" priority="-1" mode="M186"/><axsl:template match="@*|node()" priority="-2" mode="M186"><axsl:apply-templates select="@*|*" mode="M186"/></axsl:template>

<!--PATTERN sp-def_jobdeftst-->


	<!--RULE -->
<axsl:template match="sp-def:spjobdefinition_test/sp-def:object" priority="1001" mode="M187">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spjobdefinition_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a spjobdefinition_test must reference an spjobdefinition_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M187"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spjobdefinition_test/sp-def:state" priority="1000" mode="M187">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spjobdefinition_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a spjobdefinition_test must reference an spjobdefinition_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M187"/></axsl:template><axsl:template match="text()" priority="-1" mode="M187"/><axsl:template match="@*|node()" priority="-2" mode="M187"><axsl:apply-templates select="@*|*" mode="M187"/></axsl:template>

<!--PATTERN sp-def_bbtst-->


	<!--RULE -->
<axsl:template match="sp-def:bestbet_test/sp-def:object" priority="1001" mode="M188">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:bestbet_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a bestbet_test must reference an bestbet_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M188"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:bestbet_test/sp-def:state" priority="1000" mode="M188">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:bestbet_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a bestbet_test must reference an bestbet_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M188"/></axsl:template><axsl:template match="text()" priority="-1" mode="M188"/><axsl:template match="@*|node()" priority="-2" mode="M188"><axsl:apply-templates select="@*|*" mode="M188"/></axsl:template>

<!--PATTERN sp-def_infopolicycolltst-->


	<!--RULE -->
<axsl:template match="sp-def:policycoll_test/sp-def:object" priority="1001" mode="M189">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:policycoll_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a policycoll_test must reference an policycoll_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M189"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:policycoll_test/sp-def:state" priority="1000" mode="M189">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:policycoll_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a policycoll_test must reference an policycoll_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M189"/></axsl:template><axsl:template match="text()" priority="-1" mode="M189"/><axsl:template match="@*|node()" priority="-2" mode="M189"><axsl:apply-templates select="@*|*" mode="M189"/></axsl:template>

<!--PATTERN sp-def_diagnosticsservicetest-->


	<!--RULE -->
<axsl:template match="sp-def:spdiagnosticsservice_test/sp-def:object" priority="1001" mode="M190">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spdiagnosticsservice_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an spdiagnosticsservice_test must reference an spdiagnosticsservice_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M190"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spdiagnosticsservice_test/sp-def:state" priority="1000" mode="M190">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spdiagnosticsservice_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an spdiagnosticsservice_test must reference an spdiagnosticsservice_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M190"/></axsl:template><axsl:template match="text()" priority="-1" mode="M190"/><axsl:template match="@*|node()" priority="-2" mode="M190"><axsl:apply-templates select="@*|*" mode="M190"/></axsl:template>

<!--PATTERN sp-def_diagnostics_level_test-->


	<!--RULE -->
<axsl:template match="sp-def:spdiagnosticslevel_test/sp-def:object" priority="1001" mode="M191">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:spdiagnosticslevel_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an spdiagnosticslevel_test must reference an spdiagnosticslevel_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M191"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:spdiagnosticslevel_test/sp-def:state" priority="1000" mode="M191">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:spdiagnosticslevel_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an spdiagnosticslevel_test must reference an spdiagnosticslevel_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M191"/></axsl:template><axsl:template match="text()" priority="-1" mode="M191"/><axsl:template match="@*|node()" priority="-2" mode="M191"><axsl:apply-templates select="@*|*" mode="M191"/></axsl:template>

<!--PATTERN sp-def_policyfeature_test-->


	<!--RULE -->
<axsl:template match="sp-def:sppolicyfeature_test/sp-def:object" priority="1001" mode="M192">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:sppolicyfeature_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an sppolicyfeature_test must reference an sppolicyfeature_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M192"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:sppolicyfeature_test/sp-def:state" priority="1000" mode="M192">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:sppolicyfeature_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an sppolicyfeature_test must reference an sppolicyfeature_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M192"/></axsl:template><axsl:template match="text()" priority="-1" mode="M192"/><axsl:template match="@*|node()" priority="-2" mode="M192"><axsl:apply-templates select="@*|*" mode="M192"/></axsl:template>

<!--PATTERN sp-def_policy_test-->


	<!--RULE -->
<axsl:template match="sp-def:sppolicy_test/sp-def:object" priority="1001" mode="M193">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sp-def:sppolicy_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an sppolicy_test must reference an sppolicy_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M193"/></axsl:template>

	<!--RULE -->
<axsl:template match="sp-def:sppolicy_test/sp-def:state" priority="1000" mode="M193">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sp-def:sppolicy_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an sppolicy_test must reference an sppolicy_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M193"/></axsl:template><axsl:template match="text()" priority="-1" mode="M193"/><axsl:template match="@*|node()" priority="-2" mode="M193"><axsl:apply-templates select="@*|*" mode="M193"/></axsl:template>

<!--PATTERN sol-def_isainfotst-->


	<!--RULE -->
<axsl:template match="sol-def:isainfo_test/sol-def:object" priority="1001" mode="M194">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sol-def:isainfo_object/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an isainfo_test must reference an isainfo_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M194"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:isainfo_test/sol-def:state" priority="1000" mode="M194">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sol-def:isainfo_state/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an isainfo_test must reference an isainfo_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M194"/></axsl:template><axsl:template match="text()" priority="-1" mode="M194"/><axsl:template match="@*|node()" priority="-2" mode="M194"><axsl:apply-templates select="@*|*" mode="M194"/></axsl:template>

<!--PATTERN sol-def_ndd_test-->


	<!--RULE -->
<axsl:template match="sol-def:ndd_test/sol-def:object" priority="1001" mode="M195">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sol-def:ndd_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an ndd_test must reference an ndd_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M195"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:ndd_test/sol-def:state" priority="1000" mode="M195">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sol-def:ndd_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an ndd_test must reference an ndd_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M195"/></axsl:template><axsl:template match="text()" priority="-1" mode="M195"/><axsl:template match="@*|node()" priority="-2" mode="M195"><axsl:apply-templates select="@*|*" mode="M195"/></axsl:template>

<!--PATTERN sol-def_packagetst-->


	<!--RULE -->
<axsl:template match="sol-def:package_test/sol-def:object" priority="1001" mode="M196">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sol-def:package_object/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a package_test must reference a package_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M196"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:package_test/sol-def:state" priority="1000" mode="M196">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sol-def:package_state/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a package_test must reference a package_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M196"/></axsl:template><axsl:template match="text()" priority="-1" mode="M196"/><axsl:template match="@*|node()" priority="-2" mode="M196"><axsl:apply-templates select="@*|*" mode="M196"/></axsl:template>

<!--PATTERN sol-def_packagechecktst-->


	<!--RULE -->
<axsl:template match="sol-def:packagecheck_test/sol-def:object" priority="1001" mode="M197">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sol-def:packagecheck_object/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a packagecheck_test must reference a packagecheck_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M197"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:packagecheck_test/sol-def:state" priority="1000" mode="M197">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sol-def:packagecheck_state/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a packagecheck_test must reference a packagecheck_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M197"/></axsl:template><axsl:template match="text()" priority="-1" mode="M197"/><axsl:template match="@*|node()" priority="-2" mode="M197"><axsl:apply-templates select="@*|*" mode="M197"/></axsl:template>

<!--PATTERN sol-def_patch54tst-->


	<!--RULE -->
<axsl:template match="sol-def:patch54_test/sol-def:object" priority="1001" mode="M198">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sol-def:patch54_object/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a patch54_test must reference a patch54_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M198"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:patch54_test/sol-def:state" priority="1000" mode="M198">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sol-def:patch_state/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a patch54_test must reference a patch_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M198"/></axsl:template><axsl:template match="text()" priority="-1" mode="M198"/><axsl:template match="@*|node()" priority="-2" mode="M198"><axsl:apply-templates select="@*|*" mode="M198"/></axsl:template>

<!--PATTERN sol-def_patchtst_dep-->


	<!--RULE -->
<axsl:template match="sol-def:patch_test" priority="1000" mode="M199">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M199"/></axsl:template><axsl:template match="text()" priority="-1" mode="M199"/><axsl:template match="@*|node()" priority="-2" mode="M199"><axsl:apply-templates select="@*|*" mode="M199"/></axsl:template>

<!--PATTERN sol-def_patchtst-->


	<!--RULE -->
<axsl:template match="sol-def:patch_test/sol-def:object" priority="1001" mode="M200">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sol-def:patch_object/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a patch_test must reference a patch_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M200"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:patch_test/sol-def:state" priority="1000" mode="M200">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sol-def:patch_state/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a patch_test must reference a patch_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M200"/></axsl:template><axsl:template match="text()" priority="-1" mode="M200"/><axsl:template match="@*|node()" priority="-2" mode="M200"><axsl:apply-templates select="@*|*" mode="M200"/></axsl:template>

<!--PATTERN sol-def_patchobj_dep-->


	<!--RULE -->
<axsl:template match="sol-def:patch_object" priority="1000" mode="M201">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M201"/></axsl:template><axsl:template match="text()" priority="-1" mode="M201"/><axsl:template match="@*|node()" priority="-2" mode="M201"><axsl:apply-templates select="@*|*" mode="M201"/></axsl:template>

<!--PATTERN sol-def_smftst-->


	<!--RULE -->
<axsl:template match="sol-def:smf_test/sol-def:object" priority="1001" mode="M202">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/sol-def:smf_object/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a smf_test must reference a smf_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M202"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:smf_test/sol-def:state" priority="1000" mode="M202">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/sol-def:smf_state/@id"/><axsl:otherwise>
                                          <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a smf_test must reference a smf_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M202"/></axsl:template><axsl:template match="text()" priority="-1" mode="M202"/><axsl:template match="@*|node()" priority="-2" mode="M202"><axsl:apply-templates select="@*|*" mode="M202"/></axsl:template>

<!--PATTERN unix-def_dnscachetst-->


	<!--RULE -->
<axsl:template match="unix-def:dnscache_test/unix-def:object" priority="1001" mode="M203">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:dnscache_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a dnscache_test must reference a dnscache_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M203"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:dnscache_test/unix-def:state" priority="1000" mode="M203">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:dnscache_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a dnscache_test must reference a dnscache_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M203"/></axsl:template><axsl:template match="text()" priority="-1" mode="M203"/><axsl:template match="@*|node()" priority="-2" mode="M203"><axsl:apply-templates select="@*|*" mode="M203"/></axsl:template>

<!--PATTERN unix-def_filetst-->


	<!--RULE -->
<axsl:template match="unix-def:file_test/unix-def:object" priority="1001" mode="M204">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:file_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a file_test must reference a file_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M204"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:file_test/unix-def:state" priority="1000" mode="M204">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:file_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a file_test must reference a file_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M204"/></axsl:template><axsl:template match="text()" priority="-1" mode="M204"/><axsl:template match="@*|node()" priority="-2" mode="M204"><axsl:apply-templates select="@*|*" mode="M204"/></axsl:template>

<!--PATTERN unix-def_fileobjfilepath-->


	<!--RULE -->
<axsl:template match="unix-def:file_object/unix-def:filepath" priority="1000" mode="M205">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::unix-def:behaviors[@max_depth or @recurse or @recurse_direction or @recurse_file_system])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth, recurse, recurse_direction and recurse_file_system behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M205"/></axsl:template><axsl:template match="text()" priority="-1" mode="M205"/><axsl:template match="@*|node()" priority="-2" mode="M205"><axsl:apply-templates select="@*|*" mode="M205"/></axsl:template>

<!--PATTERN unix-def_file_objectfilename-->


	<!--RULE -->
<axsl:template match="unix-def:file_object/unix-def:filename" priority="1000" mode="M206">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or (@xsi:nil='true' and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M206"/></axsl:template><axsl:template match="text()" priority="-1" mode="M206"/><axsl:template match="@*|node()" priority="-2" mode="M206"><axsl:apply-templates select="@*|*" mode="M206"/></axsl:template>

<!--PATTERN unix-def_recurse_value_file_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/unix-def:file_object/unix-def:behaviors" priority="1000" mode="M207">

		<!--REPORT -->
<axsl:if test="@recurse='files'">DEPRECATED ATTRIBUTE VALUE IN: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ATTRIBUTE VALUE: <axsl:text/><axsl:value-of select="@recurse"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if>

		<!--REPORT -->
<axsl:if test="@recurse='files and directories'">DEPRECATED ATTRIBUTE VALUE IN: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ATTRIBUTE VALUE: <axsl:text/><axsl:value-of select="@recurse"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if>

		<!--REPORT -->
<axsl:if test="@recurse='none'"> DEPRECATED ATTRIBUTE VALUE IN: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ATTRIBUTE VALUE: <axsl:text/><axsl:value-of select="@recurse"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M207"/></axsl:template><axsl:template match="text()" priority="-1" mode="M207"/><axsl:template match="@*|node()" priority="-2" mode="M207"><axsl:apply-templates select="@*|*" mode="M207"/></axsl:template>

<!--PATTERN unix-def_file_ea_tst-->


	<!--RULE -->
<axsl:template match="unix-def:filextendedattribute_test/unix-def:object" priority="1001" mode="M208">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:fileextendedattribute_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileextendedattribute_test must reference a fileextendedattribute_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M208"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:fileextendedattribute_test/unix-def:state" priority="1000" mode="M208">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:fileextendedattribute_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileextendedattribute_test must reference a fileextendedattribute_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M208"/></axsl:template><axsl:template match="text()" priority="-1" mode="M208"/><axsl:template match="@*|node()" priority="-2" mode="M208"><axsl:apply-templates select="@*|*" mode="M208"/></axsl:template>

<!--PATTERN unix-def_file_ea_objfilepath-->


	<!--RULE -->
<axsl:template match="unix-def:fileextendedattribute_object/unix-def:filepath" priority="1000" mode="M209">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::unix-def:behaviors[@max_depth or @recurse or @recurse_direction or @recurse_file_system])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth, recurse, recurse_direction and recurse_file_system behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M209"/></axsl:template><axsl:template match="text()" priority="-1" mode="M209"/><axsl:template match="@*|node()" priority="-2" mode="M209"><axsl:apply-templates select="@*|*" mode="M209"/></axsl:template>

<!--PATTERN unix-def_fileextendedattribute_objectfilename-->


	<!--RULE -->
<axsl:template match="unix-def:fileextendedattribute_object/unix-def:filename" priority="1000" mode="M210">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or (@xsi:nil='true' and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M210"/></axsl:template><axsl:template match="text()" priority="-1" mode="M210"/><axsl:template match="@*|node()" priority="-2" mode="M210"><axsl:apply-templates select="@*|*" mode="M210"/></axsl:template>

<!--PATTERN unix-def_gconf_test-->


	<!--RULE -->
<axsl:template match="unix-def:gconf_test/unix-def:object" priority="1001" mode="M211">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:gconf_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a gconf_test must reference an gconf_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M211"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:gconf_test/unix-def:state" priority="1000" mode="M211">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:gconf_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a gconf_test must reference an gconf_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M211"/></axsl:template><axsl:template match="text()" priority="-1" mode="M211"/><axsl:template match="@*|node()" priority="-2" mode="M211"><axsl:apply-templates select="@*|*" mode="M211"/></axsl:template>

<!--PATTERN unix-def_gconfobjsource-->


	<!--RULE -->
<axsl:template match="unix-def:gconf_object/unix-def:source" priority="1000" mode="M212">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
                                                                                    <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the source entity of a gconf_object should be 'equals'
                                                                              <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M212"/></axsl:template><axsl:template match="text()" priority="-1" mode="M212"/><axsl:template match="@*|node()" priority="-2" mode="M212"><axsl:apply-templates select="@*|*" mode="M212"/></axsl:template>

<!--PATTERN unix-def_inetdtst-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_test/unix-def:object" priority="1001" mode="M213">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:inetd_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an inetd_test must reference an inetd_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M213"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:inetd_test/unix-def:state" priority="1000" mode="M213">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:inetd_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an inetd_test must reference an inetd_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M213"/></axsl:template><axsl:template match="text()" priority="-1" mode="M213"/><axsl:template match="@*|node()" priority="-2" mode="M213"><axsl:apply-templates select="@*|*" mode="M213"/></axsl:template>

<!--PATTERN unix-def_interfacetst-->


	<!--RULE -->
<axsl:template match="unix-def:interface_test/unix-def:object" priority="1001" mode="M214">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:interface_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an interface_test must reference an interface_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M214"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:interface_test/unix-def:state" priority="1000" mode="M214">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:interface_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an interface_test must reference an interface_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M214"/></axsl:template><axsl:template match="text()" priority="-1" mode="M214"/><axsl:template match="@*|node()" priority="-2" mode="M214"><axsl:apply-templates select="@*|*" mode="M214"/></axsl:template>

<!--PATTERN unix-def_passwordtst-->


	<!--RULE -->
<axsl:template match="unix-def:password_test/unix-def:object" priority="1001" mode="M215">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:password_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a password_test must reference a password_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M215"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:password_test/unix-def:state" priority="1000" mode="M215">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:password_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a password_test must reference a password_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M215"/></axsl:template><axsl:template match="text()" priority="-1" mode="M215"/><axsl:template match="@*|node()" priority="-2" mode="M215"><axsl:apply-templates select="@*|*" mode="M215"/></axsl:template>

<!--PATTERN unix-def_processtst_dep-->


	<!--RULE -->
<axsl:template match="unix-def:process_test" priority="1000" mode="M216">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M216"/></axsl:template><axsl:template match="text()" priority="-1" mode="M216"/><axsl:template match="@*|node()" priority="-2" mode="M216"><axsl:apply-templates select="@*|*" mode="M216"/></axsl:template>

<!--PATTERN unix-def_processtst-->


	<!--RULE -->
<axsl:template match="unix-def:process_test/unix-def:object" priority="1001" mode="M217">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:process_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a process_test must reference a process_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M217"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:process_test/unix-def:state" priority="1000" mode="M217">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:process_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a process_test must reference a process_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M217"/></axsl:template><axsl:template match="text()" priority="-1" mode="M217"/><axsl:template match="@*|node()" priority="-2" mode="M217"><axsl:apply-templates select="@*|*" mode="M217"/></axsl:template>

<!--PATTERN unix-def_processobj_dep-->


	<!--RULE -->
<axsl:template match="unix-def:process_object" priority="1000" mode="M218">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M218"/></axsl:template><axsl:template match="text()" priority="-1" mode="M218"/><axsl:template match="@*|node()" priority="-2" mode="M218"><axsl:apply-templates select="@*|*" mode="M218"/></axsl:template>

<!--PATTERN unix-def_processste_dep-->


	<!--RULE -->
<axsl:template match="unix-def:process_state" priority="1000" mode="M219">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M219"/></axsl:template><axsl:template match="text()" priority="-1" mode="M219"/><axsl:template match="@*|node()" priority="-2" mode="M219"><axsl:apply-templates select="@*|*" mode="M219"/></axsl:template>

<!--PATTERN unix-def_process58tst-->


	<!--RULE -->
<axsl:template match="unix-def:process58_test/unix-def:object" priority="1001" mode="M220">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:process58_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a process58_test must reference a process58_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M220"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:process58_test/unix-def:state" priority="1000" mode="M220">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:process58_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a process58_test must reference a process58_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M220"/></axsl:template><axsl:template match="text()" priority="-1" mode="M220"/><axsl:template match="@*|node()" priority="-2" mode="M220"><axsl:apply-templates select="@*|*" mode="M220"/></axsl:template>

<!--PATTERN unix-def_routingtable_test-->


	<!--RULE -->
<axsl:template match="unix-def:routingtable_test/unix-def:object" priority="1001" mode="M221">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:routingtable_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a routingtable_test must reference an routingtable_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M221"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:routingtable_test/unix-def:state" priority="1000" mode="M221">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:routingtable_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a routingtable_test must reference an routingtable_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M221"/></axsl:template><axsl:template match="text()" priority="-1" mode="M221"/><axsl:template match="@*|node()" priority="-2" mode="M221"><axsl:apply-templates select="@*|*" mode="M221"/></axsl:template>

<!--PATTERN unix-def_runleveltst-->


	<!--RULE -->
<axsl:template match="unix-def:runlevel_test/unix-def:object" priority="1001" mode="M222">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:runlevel_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a runlevel_test must reference a runlevel_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M222"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:runlevel_test/unix-def:state" priority="1000" mode="M222">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:runlevel_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a runlevel_test must reference a runlevel_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M222"/></axsl:template><axsl:template match="text()" priority="-1" mode="M222"/><axsl:template match="@*|node()" priority="-2" mode="M222"><axsl:apply-templates select="@*|*" mode="M222"/></axsl:template>

<!--PATTERN unix-def_sccstst-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_test/unix-def:object" priority="1001" mode="M223">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:sccs_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sccs_test must reference a sccs_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M223"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:sccs_test/unix-def:state" priority="1000" mode="M223">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:sccs_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sccs_test must reference a sccs_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M223"/></axsl:template><axsl:template match="text()" priority="-1" mode="M223"/><axsl:template match="@*|node()" priority="-2" mode="M223"><axsl:apply-templates select="@*|*" mode="M223"/></axsl:template>

<!--PATTERN unix-def_sccsobjfilepath-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_object/unix-def:filepath" priority="1000" mode="M224">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::unix-def:behaviors[@max_depth or @recurse or @recurse_direction or @recurse_file_system])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth, recurse, recurse_direction and recurse_file_system behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M224"/></axsl:template><axsl:template match="text()" priority="-1" mode="M224"/><axsl:template match="@*|node()" priority="-2" mode="M224"><axsl:apply-templates select="@*|*" mode="M224"/></axsl:template>

<!--PATTERN unix-def_sccs_objectfilename-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_object/unix-def:filename" priority="1000" mode="M225">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M225"/></axsl:template><axsl:template match="text()" priority="-1" mode="M225"/><axsl:template match="@*|node()" priority="-2" mode="M225"><axsl:apply-templates select="@*|*" mode="M225"/></axsl:template>

<!--PATTERN unix-def_shadowtst-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_test/unix-def:object" priority="1001" mode="M226">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:shadow_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a shadow_test must reference a shadow_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M226"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:shadow_test/unix-def:state" priority="1000" mode="M226">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:shadow_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a shadow_test must reference a shadow_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M226"/></axsl:template><axsl:template match="text()" priority="-1" mode="M226"/><axsl:template match="@*|node()" priority="-2" mode="M226"><axsl:apply-templates select="@*|*" mode="M226"/></axsl:template>

<!--PATTERN unix-def_sysctltst-->


	<!--RULE -->
<axsl:template match="unix-def:sysctl_test/unix-def:object" priority="1001" mode="M227">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:sysctl_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sysctl_test must reference a sysctl_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M227"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:sysctl_test/unix-def:state" priority="1000" mode="M227">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:sysctl_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sysctl_test must reference a sysctl_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M227"/></axsl:template><axsl:template match="text()" priority="-1" mode="M227"/><axsl:template match="@*|node()" priority="-2" mode="M227"><axsl:apply-templates select="@*|*" mode="M227"/></axsl:template>

<!--PATTERN unix-def_unametst-->


	<!--RULE -->
<axsl:template match="unix-def:uname_test/unix-def:object" priority="1001" mode="M228">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:uname_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a uname_test must reference a uname_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M228"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:uname_test/unix-def:state" priority="1000" mode="M228">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:uname_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a uname_test must reference a uname_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M228"/></axsl:template><axsl:template match="text()" priority="-1" mode="M228"/><axsl:template match="@*|node()" priority="-2" mode="M228"><axsl:apply-templates select="@*|*" mode="M228"/></axsl:template>

<!--PATTERN unix-def_xinetdtst-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_test/unix-def:object" priority="1001" mode="M229">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/unix-def:xinetd_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a xinetd_test must reference a xinetd_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M229"/></axsl:template>

	<!--RULE -->
<axsl:template match="unix-def:xinetd_test/unix-def:state" priority="1000" mode="M229">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/unix-def:xinetd_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a xinetd_test must reference a xinetd_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M229"/></axsl:template><axsl:template match="text()" priority="-1" mode="M229"/><axsl:template match="@*|node()" priority="-2" mode="M229"><axsl:apply-templates select="@*|*" mode="M229"/></axsl:template>

<!--PATTERN win-def_affected_platform-->


	<!--RULE -->
<axsl:template match="oval-def:affected[@family='windows']" priority="1000" mode="M230">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(oval-def:platform) or oval-def:platform='Microsoft Windows 95' or oval-def:platform='Microsoft Windows 98' or oval-def:platform='Microsoft Windows ME' or oval-def:platform='Microsoft Windows NT' or oval-def:platform='Microsoft Windows 2000' or oval-def:platform='Microsoft Windows XP' or oval-def:platform='Microsoft Windows Server 2003' or oval-def:platform='Microsoft Windows Vista' or oval-def:platform='Microsoft Windows Server 2008' or oval-def:platform='Microsoft Windows 7'"/><axsl:otherwise>
                                    <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> - the value "<axsl:text/><axsl:value-of select="oval-def:platform"/><axsl:text/>" found in platform element as part of the affected element is not a valid windows platform.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M230"/></axsl:template><axsl:template match="text()" priority="-1" mode="M230"/><axsl:template match="@*|node()" priority="-2" mode="M230"><axsl:apply-templates select="@*|*" mode="M230"/></axsl:template>

<!--PATTERN win-def_attst-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_test/win-def:object" priority="1001" mode="M231">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:accesstoken_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an accesstoken_test must reference an accesstoken_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M231"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:accesstoken_test/win-def:state" priority="1000" mode="M231">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:accesstoken_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an accesstoken_test must reference an accesstoken_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M231"/></axsl:template><axsl:template match="text()" priority="-1" mode="M231"/><axsl:template match="@*|node()" priority="-2" mode="M231"><axsl:apply-templates select="@*|*" mode="M231"/></axsl:template>

<!--PATTERN win-def_at_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:accesstoken_object/win-def:behaviors" priority="1000" mode="M232">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: win-def:accesstoken_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M232"/></axsl:template><axsl:template match="text()" priority="-1" mode="M232"/><axsl:template match="@*|node()" priority="-2" mode="M232"><axsl:apply-templates select="@*|*" mode="M232"/></axsl:template>

<!--PATTERN win-def_activedirectory_test_dep-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_test" priority="1000" mode="M233">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M233"/></axsl:template><axsl:template match="text()" priority="-1" mode="M233"/><axsl:template match="@*|node()" priority="-2" mode="M233"><axsl:apply-templates select="@*|*" mode="M233"/></axsl:template>

<!--PATTERN win-def_adtst-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_test/win-def:object" priority="1001" mode="M234">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:activedirectory_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an activedirectory_test must reference an activedirectory_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M234"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:activedirectory_test/win-def:state" priority="1000" mode="M234">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:activedirectory_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an activedirectory_test must reference an activedirectory_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M234"/></axsl:template><axsl:template match="text()" priority="-1" mode="M234"/><axsl:template match="@*|node()" priority="-2" mode="M234"><axsl:apply-templates select="@*|*" mode="M234"/></axsl:template>

<!--PATTERN win-def_activedirectory_object_dep-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_object" priority="1000" mode="M235">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M235"/></axsl:template><axsl:template match="text()" priority="-1" mode="M235"/><axsl:template match="@*|node()" priority="-2" mode="M235"><axsl:apply-templates select="@*|*" mode="M235"/></axsl:template>

<!--PATTERN win-def_activedirectory_state_dep-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_state" priority="1000" mode="M236">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M236"/></axsl:template><axsl:template match="text()" priority="-1" mode="M236"/><axsl:template match="@*|node()" priority="-2" mode="M236"><axsl:apply-templates select="@*|*" mode="M236"/></axsl:template>

<!--PATTERN win-def_activedirectory57_test-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory57_test/win-def:object" priority="1001" mode="M237">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:activedirectory57_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an activedirectory57_test must reference an activedirectory57_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M237"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:activedirectory57_test/win-def:state" priority="1000" mode="M237">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:activedirectory57_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an activedirectory57_test must reference an activedirectory57_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M237"/></axsl:template><axsl:template match="text()" priority="-1" mode="M237"/><axsl:template match="@*|node()" priority="-2" mode="M237"><axsl:apply-templates select="@*|*" mode="M237"/></axsl:template>

<!--PATTERN win-def_ad57stevalue-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory57_state/win-def:value" priority="1000" mode="M238">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='record'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the value entity of a activedirectory57_state must be 'record'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M238"/></axsl:template><axsl:template match="text()" priority="-1" mode="M238"/><axsl:template match="@*|node()" priority="-2" mode="M238"><axsl:apply-templates select="@*|*" mode="M238"/></axsl:template>

<!--PATTERN win-def_aeptst-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_test/win-def:object" priority="1001" mode="M239">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:auditeventpolicy_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an auditeventpolicy_test must reference an auditeventpolicy_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M239"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_test/win-def:state" priority="1000" mode="M239">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:auditeventpolicy_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an auditeventpolicy_test must reference an auditeventpolicy_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M239"/></axsl:template><axsl:template match="text()" priority="-1" mode="M239"/><axsl:template match="@*|node()" priority="-2" mode="M239"><axsl:apply-templates select="@*|*" mode="M239"/></axsl:template>

<!--PATTERN win-def_aepstst-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_test/win-def:object" priority="1001" mode="M240">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:auditeventpolicysubcategories_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an auditeventpolicysubcategories_test must reference an auditeventpolicysubcategories_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M240"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_test/win-def:state" priority="1000" mode="M240">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:auditeventpolicysubcategories_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an auditeventpolicysubcategories_test must reference an auditeventpolicysubcategories_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M240"/></axsl:template><axsl:template match="text()" priority="-1" mode="M240"/><axsl:template match="@*|node()" priority="-2" mode="M240"><axsl:apply-templates select="@*|*" mode="M240"/></axsl:template>

<!--PATTERN win-def_dnscachetst-->


	<!--RULE -->
<axsl:template match="win-def:dnscache_test/win-def:object" priority="1001" mode="M241">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:dnscache_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a dnscache_test must reference a dnscache_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M241"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:dnscache_test/win-def:state" priority="1000" mode="M241">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:dnscache_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a dnscache_test must reference a dnscache_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M241"/></axsl:template><axsl:template match="text()" priority="-1" mode="M241"/><axsl:template match="@*|node()" priority="-2" mode="M241"><axsl:apply-templates select="@*|*" mode="M241"/></axsl:template>

<!--PATTERN win-def_filetst-->


	<!--RULE -->
<axsl:template match="win-def:file_test/win-def:object" priority="1001" mode="M242">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:file_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a file_test must reference a file_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M242"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:file_test/win-def:state" priority="1000" mode="M242">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:file_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a file_test must reference a file_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M242"/></axsl:template><axsl:template match="text()" priority="-1" mode="M242"/><axsl:template match="@*|node()" priority="-2" mode="M242"><axsl:apply-templates select="@*|*" mode="M242"/></axsl:template>

<!--PATTERN win-def_fileobjfilepath-->


	<!--RULE -->
<axsl:template match="win-def:file_object/win-def:filepath" priority="1000" mode="M243">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::win-def:behaviors[@max_depth or @recurse_direction])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth and recurse_direction behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M243"/></axsl:template><axsl:template match="text()" priority="-1" mode="M243"/><axsl:template match="@*|node()" priority="-2" mode="M243"><axsl:apply-templates select="@*|*" mode="M243"/></axsl:template>

<!--PATTERN win-def_fileobjfilename-->


	<!--RULE -->
<axsl:template match="win-def:file_object/win-def:filename" priority="1000" mode="M244">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or (@xsi:nil='true' and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M244"/></axsl:template><axsl:template match="text()" priority="-1" mode="M244"/><axsl:template match="@*|node()" priority="-2" mode="M244"><axsl:apply-templates select="@*|*" mode="M244"/></axsl:template>

<!--PATTERN win-def_filestefilename-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:filename" priority="1000" mode="M245">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M245"/></axsl:template><axsl:template match="text()" priority="-1" mode="M245"/><axsl:template match="@*|node()" priority="-2" mode="M245"><axsl:apply-templates select="@*|*" mode="M245"/></axsl:template>

<!--PATTERN win-def_fap53tst-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_test/win-def:object" priority="1001" mode="M246">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:fileauditedpermissions53_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileauditedpermissions53_test must reference a fileauditedpermissions53_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M246"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_test/win-def:state" priority="1000" mode="M246">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:fileauditedpermissions53_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileauditedpermissions53_test must reference a fileauditedpermissions53_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M246"/></axsl:template><axsl:template match="text()" priority="-1" mode="M246"/><axsl:template match="@*|node()" priority="-2" mode="M246"><axsl:apply-templates select="@*|*" mode="M246"/></axsl:template>

<!--PATTERN win-def_fap53objfilepath-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_object/win-def:filepath" priority="1000" mode="M247">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::win-def:behaviors[@max_depth or @recurse_direction])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth and recurse_direction behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M247"/></axsl:template><axsl:template match="text()" priority="-1" mode="M247"/><axsl:template match="@*|node()" priority="-2" mode="M247"><axsl:apply-templates select="@*|*" mode="M247"/></axsl:template>

<!--PATTERN win-def_fap53objfilename-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_object/win-def:filename" priority="1000" mode="M248">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or (@xsi:nil='true' and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M248"/></axsl:template><axsl:template match="text()" priority="-1" mode="M248"/><axsl:template match="@*|node()" priority="-2" mode="M248"><axsl:apply-templates select="@*|*" mode="M248"/></axsl:template>

<!--PATTERN win-def_fap53stefilename-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:filename" priority="1000" mode="M249">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M249"/></axsl:template><axsl:template match="text()" priority="-1" mode="M249"/><axsl:template match="@*|node()" priority="-2" mode="M249"><axsl:apply-templates select="@*|*" mode="M249"/></axsl:template>

<!--PATTERN win-def_fap53_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:fileauditedpermissions53_object/win-def:behaviors" priority="1000" mode="M250">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: fileauditedpermissions53_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M250"/></axsl:template><axsl:template match="text()" priority="-1" mode="M250"/><axsl:template match="@*|node()" priority="-2" mode="M250"><axsl:apply-templates select="@*|*" mode="M250"/></axsl:template>

<!--PATTERN win-def_faptst_dep-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_test" priority="1000" mode="M251">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M251"/></axsl:template><axsl:template match="text()" priority="-1" mode="M251"/><axsl:template match="@*|node()" priority="-2" mode="M251"><axsl:apply-templates select="@*|*" mode="M251"/></axsl:template>

<!--PATTERN win-def_faptst-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_test/win-def:object" priority="1001" mode="M252">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:fileauditedpermissions_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileauditedpermissions_test must reference a fileauditedpermissions_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M252"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_test/win-def:state" priority="1000" mode="M252">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:fileauditedpermissions_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileauditedpermissions_test must reference a fileauditedpermissions_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M252"/></axsl:template><axsl:template match="text()" priority="-1" mode="M252"/><axsl:template match="@*|node()" priority="-2" mode="M252"><axsl:apply-templates select="@*|*" mode="M252"/></axsl:template>

<!--PATTERN win-def_fapobj_dep-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_object" priority="1000" mode="M253">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M253"/></axsl:template><axsl:template match="text()" priority="-1" mode="M253"/><axsl:template match="@*|node()" priority="-2" mode="M253"><axsl:apply-templates select="@*|*" mode="M253"/></axsl:template>

<!--PATTERN win-def_fapobjfilename-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_object/win-def:filename" priority="1000" mode="M254">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or (@xsi:nil='true' and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M254"/></axsl:template><axsl:template match="text()" priority="-1" mode="M254"/><axsl:template match="@*|node()" priority="-2" mode="M254"><axsl:apply-templates select="@*|*" mode="M254"/></axsl:template>

<!--PATTERN win-def_fapste_dep-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state" priority="1000" mode="M255">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M255"/></axsl:template><axsl:template match="text()" priority="-1" mode="M255"/><axsl:template match="@*|node()" priority="-2" mode="M255"><axsl:apply-templates select="@*|*" mode="M255"/></axsl:template>

<!--PATTERN win-def_fapstefilename-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:filename" priority="1000" mode="M256">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M256"/></axsl:template><axsl:template match="text()" priority="-1" mode="M256"/><axsl:template match="@*|node()" priority="-2" mode="M256"><axsl:apply-templates select="@*|*" mode="M256"/></axsl:template>

<!--PATTERN win-def_fap_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:fileauditedpermissions_object/win-def:behaviors" priority="1000" mode="M257">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: fileauditedpermissions_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M257"/></axsl:template><axsl:template match="text()" priority="-1" mode="M257"/><axsl:template match="@*|node()" priority="-2" mode="M257"><axsl:apply-templates select="@*|*" mode="M257"/></axsl:template>

<!--PATTERN win-def_fer53tst-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_test/win-def:object" priority="1001" mode="M258">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:fileeffectiverights53_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileeffectiverights53_test must reference a fileeffectiverights53_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M258"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_test/win-def:state" priority="1000" mode="M258">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:fileeffectiverights53_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileeffectiverights53_test must reference a fileeffectiverights53_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M258"/></axsl:template><axsl:template match="text()" priority="-1" mode="M258"/><axsl:template match="@*|node()" priority="-2" mode="M258"><axsl:apply-templates select="@*|*" mode="M258"/></axsl:template>

<!--PATTERN win-def_fer53objfilepath-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_object/win-def:filepath" priority="1000" mode="M259">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(preceding-sibling::win-def:behaviors[@max_depth or @recurse_direction])"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the max_depth and recurse_direction behaviors are not allowed with a filepath entity<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M259"/></axsl:template><axsl:template match="text()" priority="-1" mode="M259"/><axsl:template match="@*|node()" priority="-2" mode="M259"><axsl:apply-templates select="@*|*" mode="M259"/></axsl:template>

<!--PATTERN win-def_fer53objfilename-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_object/win-def:filename" priority="1000" mode="M260">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or (@xsi:nil='true' and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M260"/></axsl:template><axsl:template match="text()" priority="-1" mode="M260"/><axsl:template match="@*|node()" priority="-2" mode="M260"><axsl:apply-templates select="@*|*" mode="M260"/></axsl:template>

<!--PATTERN win-def_fer53stefilename-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:filename" priority="1000" mode="M261">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M261"/></axsl:template><axsl:template match="text()" priority="-1" mode="M261"/><axsl:template match="@*|node()" priority="-2" mode="M261"><axsl:apply-templates select="@*|*" mode="M261"/></axsl:template>

<!--PATTERN win-def_ffr53_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:fileeffectiverights53_object/win-def:behaviors" priority="1000" mode="M262">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: fileeffectiverights53_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M262"/></axsl:template><axsl:template match="text()" priority="-1" mode="M262"/><axsl:template match="@*|node()" priority="-2" mode="M262"><axsl:apply-templates select="@*|*" mode="M262"/></axsl:template>

<!--PATTERN win-def_fertst_dep-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_test" priority="1000" mode="M263">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M263"/></axsl:template><axsl:template match="text()" priority="-1" mode="M263"/><axsl:template match="@*|node()" priority="-2" mode="M263"><axsl:apply-templates select="@*|*" mode="M263"/></axsl:template>

<!--PATTERN win-def_fertst-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_test/win-def:object" priority="1001" mode="M264">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:fileeffectiverights_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileeffectiverights_test must reference a fileeffectiverights_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M264"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_test/win-def:state" priority="1000" mode="M264">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:fileeffectiverights_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileeffectiverights_test must reference a fileeffectiverights_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M264"/></axsl:template><axsl:template match="text()" priority="-1" mode="M264"/><axsl:template match="@*|node()" priority="-2" mode="M264"><axsl:apply-templates select="@*|*" mode="M264"/></axsl:template>

<!--PATTERN win-def_ferobj_dep-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_object" priority="1000" mode="M265">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M265"/></axsl:template><axsl:template match="text()" priority="-1" mode="M265"/><axsl:template match="@*|node()" priority="-2" mode="M265"><axsl:apply-templates select="@*|*" mode="M265"/></axsl:template>

<!--PATTERN win-def_fefobjfilename-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_object/win-def:filename" priority="1000" mode="M266">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@var_ref and .='') or (@xsi:nil='true' and .='') or not(.='')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot be empty unless the xsi:nil attribute is set to true or a var_ref is used<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M266"/></axsl:template><axsl:template match="text()" priority="-1" mode="M266"/><axsl:template match="@*|node()" priority="-2" mode="M266"><axsl:apply-templates select="@*|*" mode="M266"/></axsl:template>

<!--PATTERN win-def_ferste_dep-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state" priority="1000" mode="M267">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M267"/></axsl:template><axsl:template match="text()" priority="-1" mode="M267"/><axsl:template match="@*|node()" priority="-2" mode="M267"><axsl:apply-templates select="@*|*" mode="M267"/></axsl:template>

<!--PATTERN win-def_ferstefilename-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:filename" priority="1000" mode="M268">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) or (@operation='pattern match')"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - filename entity cannot contain the characters / \ : * ? &gt; | <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M268"/></axsl:template><axsl:template match="text()" priority="-1" mode="M268"/><axsl:template match="@*|node()" priority="-2" mode="M268"><axsl:apply-templates select="@*|*" mode="M268"/></axsl:template>

<!--PATTERN win-def_ffr_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:fileeffectiverights_object/win-def:behaviors" priority="1000" mode="M269">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: fileeffectiverights_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M269"/></axsl:template><axsl:template match="text()" priority="-1" mode="M269"/><axsl:template match="@*|node()" priority="-2" mode="M269"><axsl:apply-templates select="@*|*" mode="M269"/></axsl:template>

<!--PATTERN win-def_grouptst-->


	<!--RULE -->
<axsl:template match="win-def:group_test/win-def:object" priority="1001" mode="M270">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:group_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a group_test must reference a group_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M270"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:group_test/win-def:state" priority="1000" mode="M270">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:group_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a group_test must reference a group_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M270"/></axsl:template><axsl:template match="text()" priority="-1" mode="M270"/><axsl:template match="@*|node()" priority="-2" mode="M270"><axsl:apply-templates select="@*|*" mode="M270"/></axsl:template>

<!--PATTERN win-def_groupsidtst-->


	<!--RULE -->
<axsl:template match="win-def:group_sid_test/win-def:object" priority="1001" mode="M271">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:group_sid_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a group_sid_test must reference a group_sid_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M271"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:group_sid_test/win-def:state" priority="1000" mode="M271">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:group_sid_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a group_sid_test must reference a group_sid_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M271"/></axsl:template><axsl:template match="text()" priority="-1" mode="M271"/><axsl:template match="@*|node()" priority="-2" mode="M271"><axsl:apply-templates select="@*|*" mode="M271"/></axsl:template>

<!--PATTERN win-def_wininterfacetst-->


	<!--RULE -->
<axsl:template match="win-def:interface_test/win-def:object" priority="1001" mode="M272">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:interface_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an interface_test must reference an interface_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M272"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:interface_test/win-def:state" priority="1000" mode="M272">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:interface_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an interface_test must reference an interface_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M272"/></axsl:template><axsl:template match="text()" priority="-1" mode="M272"/><axsl:template match="@*|node()" priority="-2" mode="M272"><axsl:apply-templates select="@*|*" mode="M272"/></axsl:template>

<!--PATTERN win-def_lptst-->


	<!--RULE -->
<axsl:template match="win-def:lockoutpolicy_test/win-def:object" priority="1001" mode="M273">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:lockoutpolicy_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a lockoutpolicy_test must reference a lockoutpolicy_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M273"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:lockoutpolicy_test/win-def:state" priority="1000" mode="M273">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:lockoutpolicy_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a lockoutpolicy_test must reference a lockoutpolicy_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M273"/></axsl:template><axsl:template match="text()" priority="-1" mode="M273"/><axsl:template match="@*|node()" priority="-2" mode="M273"><axsl:apply-templates select="@*|*" mode="M273"/></axsl:template>

<!--PATTERN win-def_metabasetst-->


	<!--RULE -->
<axsl:template match="win-def:metabase_test/win-def:object" priority="1001" mode="M274">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:metabase_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a metabase_test must reference a metabase_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M274"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:metabase_test/win-def:state" priority="1000" mode="M274">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:metabase_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a metabase_test must reference a metabase_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M274"/></axsl:template><axsl:template match="text()" priority="-1" mode="M274"/><axsl:template match="@*|node()" priority="-2" mode="M274"><axsl:apply-templates select="@*|*" mode="M274"/></axsl:template>

<!--PATTERN win-def_pptst-->


	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_test/win-def:object" priority="1001" mode="M275">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:passwordpolicy_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a passwordpolicy_test must reference a passwordpolicy_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M275"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_test/win-def:state" priority="1000" mode="M275">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:passwordpolicy_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a passwordpolicy_test must reference a passwordpolicy_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M275"/></axsl:template><axsl:template match="text()" priority="-1" mode="M275"/><axsl:template match="@*|node()" priority="-2" mode="M275"><axsl:apply-templates select="@*|*" mode="M275"/></axsl:template>

<!--PATTERN win-def_winporttst-->


	<!--RULE -->
<axsl:template match="win-def:port_test/win-def:object" priority="1001" mode="M276">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:port_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a port_test must reference a port_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M276"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:port_test/win-def:state" priority="1000" mode="M276">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:port_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a port_test must reference a port_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M276"/></axsl:template><axsl:template match="text()" priority="-1" mode="M276"/><axsl:template match="@*|node()" priority="-2" mode="M276"><axsl:apply-templates select="@*|*" mode="M276"/></axsl:template>

<!--PATTERN win-def_pertst-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_test/win-def:object" priority="1001" mode="M277">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:printereffectiverights_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a printereffectiverights_test must reference a printereffectiverights_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M277"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_test/win-def:state" priority="1000" mode="M277">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:printereffectiverights_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a printereffectiverights_test must reference a printereffectiverights_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M277"/></axsl:template><axsl:template match="text()" priority="-1" mode="M277"/><axsl:template match="@*|node()" priority="-2" mode="M277"><axsl:apply-templates select="@*|*" mode="M277"/></axsl:template>

<!--PATTERN win-def_pef_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:printereffectiverights_object/win-def:behaviors" priority="1000" mode="M278">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: printereffectiverights_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M278"/></axsl:template><axsl:template match="text()" priority="-1" mode="M278"/><axsl:template match="@*|node()" priority="-2" mode="M278"><axsl:apply-templates select="@*|*" mode="M278"/></axsl:template>

<!--PATTERN win-def_processtst_dep-->


	<!--RULE -->
<axsl:template match="win-def:process_test" priority="1000" mode="M279">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M279"/></axsl:template><axsl:template match="text()" priority="-1" mode="M279"/><axsl:template match="@*|node()" priority="-2" mode="M279"><axsl:apply-templates select="@*|*" mode="M279"/></axsl:template>

<!--PATTERN win-def_processtst-->


	<!--RULE -->
<axsl:template match="win-def:process_test/win-def:object" priority="1001" mode="M280">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:process_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a process_test must reference a process_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M280"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:process_test/win-def:state" priority="1000" mode="M280">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:process_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a process_test must reference a process_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M280"/></axsl:template><axsl:template match="text()" priority="-1" mode="M280"/><axsl:template match="@*|node()" priority="-2" mode="M280"><axsl:apply-templates select="@*|*" mode="M280"/></axsl:template>

<!--PATTERN win-def_processobj_dep-->


	<!--RULE -->
<axsl:template match="win-def:process_object" priority="1000" mode="M281">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M281"/></axsl:template><axsl:template match="text()" priority="-1" mode="M281"/><axsl:template match="@*|node()" priority="-2" mode="M281"><axsl:apply-templates select="@*|*" mode="M281"/></axsl:template>

<!--PATTERN win-def_processste_dep-->


	<!--RULE -->
<axsl:template match="win-def:process_state" priority="1000" mode="M282">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M282"/></axsl:template><axsl:template match="text()" priority="-1" mode="M282"/><axsl:template match="@*|node()" priority="-2" mode="M282"><axsl:apply-templates select="@*|*" mode="M282"/></axsl:template>

<!--PATTERN win-def_process58tst-->


	<!--RULE -->
<axsl:template match="win-def:process58_test/win-def:object" priority="1001" mode="M283">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:process58_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a process58_test must reference a process58_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M283"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:process58_test/win-def:state" priority="1000" mode="M283">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:process58_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a process58_test must reference a process58_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M283"/></axsl:template><axsl:template match="text()" priority="-1" mode="M283"/><axsl:template match="@*|node()" priority="-2" mode="M283"><axsl:apply-templates select="@*|*" mode="M283"/></axsl:template>

<!--PATTERN win-def_regtst-->


	<!--RULE -->
<axsl:template match="win-def:registry_test/win-def:object" priority="1001" mode="M284">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:registry_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a registry_test must reference a registry_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M284"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:registry_test/win-def:state" priority="1000" mode="M284">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:registry_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a registry_test must reference a registry_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M284"/></axsl:template><axsl:template match="text()" priority="-1" mode="M284"/><axsl:template match="@*|node()" priority="-2" mode="M284"><axsl:apply-templates select="@*|*" mode="M284"/></axsl:template>

<!--PATTERN win-def_regobjkey-->


	<!--RULE -->
<axsl:template match="win-def:registry_object/win-def:key" priority="1000" mode="M285">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@xsi:nil=true()) or ../win-def:name/@xsi:nil=true()"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - name entity must be nil when key is nil<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M285"/></axsl:template><axsl:template match="text()" priority="-1" mode="M285"/><axsl:template match="@*|node()" priority="-2" mode="M285"><axsl:apply-templates select="@*|*" mode="M285"/></axsl:template>

<!--PATTERN win-def_rap53tst-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_test/win-def:object" priority="1001" mode="M286">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:regkeyauditedpermissions53_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a regkeyauditedpermissions53_test must reference a regkeyauditedpermissions53_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M286"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_test/win-def:state" priority="1000" mode="M286">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:regkeyauditedpermissions53_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a regkeyauditedpermissions53_test must reference a regkeyauditedpermissions53_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M286"/></axsl:template><axsl:template match="text()" priority="-1" mode="M286"/><axsl:template match="@*|node()" priority="-2" mode="M286"><axsl:apply-templates select="@*|*" mode="M286"/></axsl:template>

<!--PATTERN win-def_rap53stestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_synchronize" priority="1000" mode="M287">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED ELEMENT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M287"/></axsl:template><axsl:template match="text()" priority="-1" mode="M287"/><axsl:template match="@*|node()" priority="-2" mode="M287"><axsl:apply-templates select="@*|*" mode="M287"/></axsl:template>

<!--PATTERN win-def_rka53_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:regkeyauditedpermissions53_object/win-def:behaviors" priority="1000" mode="M288">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: regkeyauditedpermissions53_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M288"/></axsl:template><axsl:template match="text()" priority="-1" mode="M288"/><axsl:template match="@*|node()" priority="-2" mode="M288"><axsl:apply-templates select="@*|*" mode="M288"/></axsl:template>

<!--PATTERN win-def_raptst_dep-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_test" priority="1000" mode="M289">

		<!--REPORT -->
<axsl:if test="true()">
                                                DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                          <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M289"/></axsl:template><axsl:template match="text()" priority="-1" mode="M289"/><axsl:template match="@*|node()" priority="-2" mode="M289"><axsl:apply-templates select="@*|*" mode="M289"/></axsl:template>

<!--PATTERN win-def_raptst-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_test/win-def:object" priority="1001" mode="M290">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:regkeyauditedpermissions_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a regkeyauditedpermissions_test must reference a regkeyauditedpermissions_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M290"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_test/win-def:state" priority="1000" mode="M290">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:regkeyauditedpermissions_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a regkeyauditedpermissions_test must reference a regkeyauditedpermissions_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M290"/></axsl:template><axsl:template match="text()" priority="-1" mode="M290"/><axsl:template match="@*|node()" priority="-2" mode="M290"><axsl:apply-templates select="@*|*" mode="M290"/></axsl:template>

<!--PATTERN win-def_rapobj_dep-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_object" priority="1000" mode="M291">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M291"/></axsl:template><axsl:template match="text()" priority="-1" mode="M291"/><axsl:template match="@*|node()" priority="-2" mode="M291"><axsl:apply-templates select="@*|*" mode="M291"/></axsl:template>

<!--PATTERN win-def_rapste_dep-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state" priority="1000" mode="M292">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M292"/></axsl:template><axsl:template match="text()" priority="-1" mode="M292"/><axsl:template match="@*|node()" priority="-2" mode="M292"><axsl:apply-templates select="@*|*" mode="M292"/></axsl:template>

<!--PATTERN win-def_rka_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:regkeyauditedpermissions_object/win-def:behaviors" priority="1000" mode="M293">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: regkeyauditedpermissions_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M293"/></axsl:template><axsl:template match="text()" priority="-1" mode="M293"/><axsl:template match="@*|node()" priority="-2" mode="M293"><axsl:apply-templates select="@*|*" mode="M293"/></axsl:template>

<!--PATTERN win-def_rer53tst-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_test/win-def:object" priority="1001" mode="M294">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:regkeyeffectiverights53_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a regkeyeffectiverights53_test must reference a regkeyeffectiverights53_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M294"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_test/win-def:state" priority="1000" mode="M294">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:regkeyeffectiverights53_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a regkeyeffectiverights53_test must reference a regkeyeffectiverights53_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M294"/></axsl:template><axsl:template match="text()" priority="-1" mode="M294"/><axsl:template match="@*|node()" priority="-2" mode="M294"><axsl:apply-templates select="@*|*" mode="M294"/></axsl:template>

<!--PATTERN win-def_rer53stestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_synchronize" priority="1000" mode="M295">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED ELEMENT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M295"/></axsl:template><axsl:template match="text()" priority="-1" mode="M295"/><axsl:template match="@*|node()" priority="-2" mode="M295"><axsl:apply-templates select="@*|*" mode="M295"/></axsl:template>

<!--PATTERN win-def_rke53_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:regkeyeffectiverights53_object/win-def:behaviors" priority="1000" mode="M296">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: regkeyeffectiverights53_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M296"/></axsl:template><axsl:template match="text()" priority="-1" mode="M296"/><axsl:template match="@*|node()" priority="-2" mode="M296"><axsl:apply-templates select="@*|*" mode="M296"/></axsl:template>

<!--PATTERN win-def_rertst_dep-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_test" priority="1000" mode="M297">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M297"/></axsl:template><axsl:template match="text()" priority="-1" mode="M297"/><axsl:template match="@*|node()" priority="-2" mode="M297"><axsl:apply-templates select="@*|*" mode="M297"/></axsl:template>

<!--PATTERN win-def_rertst-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_test/win-def:object" priority="1001" mode="M298">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:regkeyeffectiverights_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a regkeyeffectiverights_test must reference a regkeyeffectiverights_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M298"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_test/win-def:state" priority="1000" mode="M298">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:regkeyeffectiverights_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a regkeyeffectiverights_test must reference a regkeyeffectiverights_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M298"/></axsl:template><axsl:template match="text()" priority="-1" mode="M298"/><axsl:template match="@*|node()" priority="-2" mode="M298"><axsl:apply-templates select="@*|*" mode="M298"/></axsl:template>

<!--PATTERN win-def_rerobj_dep-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_object" priority="1000" mode="M299">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M299"/></axsl:template><axsl:template match="text()" priority="-1" mode="M299"/><axsl:template match="@*|node()" priority="-2" mode="M299"><axsl:apply-templates select="@*|*" mode="M299"/></axsl:template>

<!--PATTERN win-def_rerste_dep-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state" priority="1000" mode="M300">

		<!--REPORT -->
<axsl:if test="true()">
                                          DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                    <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M300"/></axsl:template><axsl:template match="text()" priority="-1" mode="M300"/><axsl:template match="@*|node()" priority="-2" mode="M300"><axsl:apply-templates select="@*|*" mode="M300"/></axsl:template>

<!--PATTERN win-def_rke_resolve_group_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:objects/win-def:regkeyeffectiverights_object/win-def:behaviors" priority="1000" mode="M301">

		<!--REPORT -->
<axsl:if test="@resolve_group">DEPRECATED BEHAVIOR IN: regkeyeffectiverights_object <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M301"/></axsl:template><axsl:template match="text()" priority="-1" mode="M301"/><axsl:template match="@*|node()" priority="-2" mode="M301"><axsl:apply-templates select="@*|*" mode="M301"/></axsl:template>

<!--PATTERN win-def_servicetst-->


	<!--RULE -->
<axsl:template match="win-def:service_test/win-def:object" priority="1001" mode="M302">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:service_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a service_test must reference a service_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M302"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:service_test/win-def:state" priority="1000" mode="M302">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:service_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a service_test must reference a service_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M302"/></axsl:template><axsl:template match="text()" priority="-1" mode="M302"/><axsl:template match="@*|node()" priority="-2" mode="M302"><axsl:apply-templates select="@*|*" mode="M302"/></axsl:template>

<!--PATTERN win-def_svcertst-->


	<!--RULE -->
<axsl:template match="win-def:serviceeffectiverights_test/win-def:object" priority="1001" mode="M303">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:serviceeffectiverights_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a serviceeffectiverights_test must reference a serviceeffectiverights_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M303"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:serviceeffectiverights_test/win-def:state" priority="1000" mode="M303">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:serviceeffectiverights_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a serviceeffectiverights_test must reference a serviceeffectiverights_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M303"/></axsl:template><axsl:template match="text()" priority="-1" mode="M303"/><axsl:template match="@*|node()" priority="-2" mode="M303"><axsl:apply-templates select="@*|*" mode="M303"/></axsl:template>

<!--PATTERN win-def_srtst-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_test/win-def:object" priority="1001" mode="M304">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:sharedresource_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sharedresource_test must reference a sharedresource_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M304"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:sharedresource_test/win-def:state" priority="1000" mode="M304">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:sharedresource_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sharedresource_test must reference a sharedresource_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M304"/></axsl:template><axsl:template match="text()" priority="-1" mode="M304"/><axsl:template match="@*|node()" priority="-2" mode="M304"><axsl:apply-templates select="@*|*" mode="M304"/></axsl:template>

<!--PATTERN win-def_sidtst-->


	<!--RULE -->
<axsl:template match="win-def:sid_test/win-def:object" priority="1001" mode="M305">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:sid_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sid_test must reference a sid_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M305"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:sid_test/win-def:state" priority="1000" mode="M305">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:sid_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sid_test must reference a sid_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M305"/></axsl:template><axsl:template match="text()" priority="-1" mode="M305"/><axsl:template match="@*|node()" priority="-2" mode="M305"><axsl:apply-templates select="@*|*" mode="M305"/></axsl:template>

<!--PATTERN win-def_sidsidtst-->


	<!--RULE -->
<axsl:template match="win-def:sid_sid_test/win-def:object" priority="1001" mode="M306">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:sid_sid_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sid_sid_test must reference a sid_sid_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M306"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:sid_sid_test/win-def:state" priority="1000" mode="M306">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:sid_sid_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sid_sid_test must reference a sid_sid_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M306"/></axsl:template><axsl:template match="text()" priority="-1" mode="M306"/><axsl:template match="@*|node()" priority="-2" mode="M306"><axsl:apply-templates select="@*|*" mode="M306"/></axsl:template>

<!--PATTERN win-def_uactst-->


	<!--RULE -->
<axsl:template match="win-def:uac_test/win-def:object" priority="1001" mode="M307">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:uac_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a uac_test must reference a uac_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M307"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:uac_test/win-def:state" priority="1000" mode="M307">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:uac_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a uac_test must reference a uac_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M307"/></axsl:template><axsl:template match="text()" priority="-1" mode="M307"/><axsl:template match="@*|node()" priority="-2" mode="M307"><axsl:apply-templates select="@*|*" mode="M307"/></axsl:template>

<!--PATTERN win-def_usertst-->


	<!--RULE -->
<axsl:template match="win-def:user_test/win-def:object" priority="1001" mode="M308">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:user_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a user_test must reference a user_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M308"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:user_test/win-def:state" priority="1000" mode="M308">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:user_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a user_test must reference a user_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M308"/></axsl:template><axsl:template match="text()" priority="-1" mode="M308"/><axsl:template match="@*|node()" priority="-2" mode="M308"><axsl:apply-templates select="@*|*" mode="M308"/></axsl:template>

<!--PATTERN win-def_usersid55tst-->


	<!--RULE -->
<axsl:template match="win-def:user_sid55_test/win-def:object" priority="1001" mode="M309">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:user_sid55_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a user_sid55_test must reference a user_sid55_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M309"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:user_sid55_test/win-def:state" priority="1000" mode="M309">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:user_sid55_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a user_sid55_test must reference a user_sid55_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M309"/></axsl:template><axsl:template match="text()" priority="-1" mode="M309"/><axsl:template match="@*|node()" priority="-2" mode="M309"><axsl:apply-templates select="@*|*" mode="M309"/></axsl:template>

<!--PATTERN win-def_usersidtst_dep-->


	<!--RULE -->
<axsl:template match="win-def:user_sid_test" priority="1000" mode="M310">

		<!--REPORT -->
<axsl:if test="true()">
                                        DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                  <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M310"/></axsl:template><axsl:template match="text()" priority="-1" mode="M310"/><axsl:template match="@*|node()" priority="-2" mode="M310"><axsl:apply-templates select="@*|*" mode="M310"/></axsl:template>

<!--PATTERN win-def_usersidtst-->


	<!--RULE -->
<axsl:template match="win-def:user_sid_test/win-def:object" priority="1001" mode="M311">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:user_sid_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a user_sid_test must reference a user_sid_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M311"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:user_sid_test/win-def:state" priority="1000" mode="M311">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:user_sid_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a user_sid_test must reference a user_sid_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M311"/></axsl:template><axsl:template match="text()" priority="-1" mode="M311"/><axsl:template match="@*|node()" priority="-2" mode="M311"><axsl:apply-templates select="@*|*" mode="M311"/></axsl:template>

<!--PATTERN win-def_usersidobj_dep-->


	<!--RULE -->
<axsl:template match="win-def:user_sid_object" priority="1000" mode="M312">

		<!--REPORT -->
<axsl:if test="true()">
                                        DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                  <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M312"/></axsl:template><axsl:template match="text()" priority="-1" mode="M312"/><axsl:template match="@*|node()" priority="-2" mode="M312"><axsl:apply-templates select="@*|*" mode="M312"/></axsl:template>

<!--PATTERN win-def_usersidste_dep-->


	<!--RULE -->
<axsl:template match="win-def:user_sid_state" priority="1000" mode="M313">

		<!--REPORT -->
<axsl:if test="true()">
                                        DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
                                  <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M313"/></axsl:template><axsl:template match="text()" priority="-1" mode="M313"/><axsl:template match="@*|node()" priority="-2" mode="M313"><axsl:apply-templates select="@*|*" mode="M313"/></axsl:template>

<!--PATTERN win-def_volumetst-->


	<!--RULE -->
<axsl:template match="win-def:volume_test/win-def:object" priority="1001" mode="M314">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:volume_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a volume_test must reference a volume_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M314"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:volume_test/win-def:state" priority="1000" mode="M314">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:volume_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a volume_test must reference a volume_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M314"/></axsl:template><axsl:template match="text()" priority="-1" mode="M314"/><axsl:template match="@*|node()" priority="-2" mode="M314"><axsl:apply-templates select="@*|*" mode="M314"/></axsl:template>

<!--PATTERN win-def_wmitst_dep-->


	<!--RULE -->
<axsl:template match="win-def:wmi_test" priority="1000" mode="M315">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED TEST: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M315"/></axsl:template><axsl:template match="text()" priority="-1" mode="M315"/><axsl:template match="@*|node()" priority="-2" mode="M315"><axsl:apply-templates select="@*|*" mode="M315"/></axsl:template>

<!--PATTERN win-def_wmitst-->


	<!--RULE -->
<axsl:template match="win-def:wmi_test/win-def:object" priority="1001" mode="M316">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:wmi_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a wmi_test must reference a wmi_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M316"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:wmi_test/win-def:state" priority="1000" mode="M316">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:wmi_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a wmi_test must reference a wmi_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M316"/></axsl:template><axsl:template match="text()" priority="-1" mode="M316"/><axsl:template match="@*|node()" priority="-2" mode="M316"><axsl:apply-templates select="@*|*" mode="M316"/></axsl:template>

<!--PATTERN win-def_wmiobj_dep-->


	<!--RULE -->
<axsl:template match="win-def:wmi_object" priority="1000" mode="M317">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED OBJECT: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M317"/></axsl:template><axsl:template match="text()" priority="-1" mode="M317"/><axsl:template match="@*|node()" priority="-2" mode="M317"><axsl:apply-templates select="@*|*" mode="M317"/></axsl:template>

<!--PATTERN win-def_wmiobjnamespace-->


	<!--RULE -->
<axsl:template match="win-def:wmi_object/win-def:namespace" priority="1000" mode="M318">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the namespace entity of a wmi_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M318"/></axsl:template><axsl:template match="text()" priority="-1" mode="M318"/><axsl:template match="@*|node()" priority="-2" mode="M318"><axsl:apply-templates select="@*|*" mode="M318"/></axsl:template>

<!--PATTERN win-def_wmiobjwql-->


	<!--RULE -->
<axsl:template match="win-def:wmi_object/win-def:wql" priority="1000" mode="M319">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the wql entity of a wmi_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M319"/></axsl:template><axsl:template match="text()" priority="-1" mode="M319"/><axsl:template match="@*|node()" priority="-2" mode="M319"><axsl:apply-templates select="@*|*" mode="M319"/></axsl:template>

<!--PATTERN win-def_wmiste_dep-->


	<!--RULE -->
<axsl:template match="win-def:wmi_state" priority="1000" mode="M320">

		<!--REPORT -->
<axsl:if test="true()">DEPRECATED STATE: <axsl:text/><axsl:value-of select="name()"/><axsl:text/> ID: <axsl:text/><axsl:value-of select="@id"/><axsl:text/>
         <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M320"/></axsl:template><axsl:template match="text()" priority="-1" mode="M320"/><axsl:template match="@*|node()" priority="-2" mode="M320"><axsl:apply-templates select="@*|*" mode="M320"/></axsl:template>

<!--PATTERN win-def_wmi57tst-->


	<!--RULE -->
<axsl:template match="win-def:wmi57_test/win-def:object" priority="1001" mode="M321">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:wmi57_object/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a wmi57_test must reference a wmi57_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M321"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:wmi57_test/win-def:state" priority="1000" mode="M321">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:wmi57_state/@id"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a wmi57_test must reference a wmi57_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M321"/></axsl:template><axsl:template match="text()" priority="-1" mode="M321"/><axsl:template match="@*|node()" priority="-2" mode="M321"><axsl:apply-templates select="@*|*" mode="M321"/></axsl:template>

<!--PATTERN win-def_wmi57objnamespace-->


	<!--RULE -->
<axsl:template match="win-def:wmi57_object/win-def:namespace" priority="1000" mode="M322">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the namespace entity of a wmi57_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M322"/></axsl:template><axsl:template match="text()" priority="-1" mode="M322"/><axsl:template match="@*|node()" priority="-2" mode="M322"><axsl:apply-templates select="@*|*" mode="M322"/></axsl:template>

<!--PATTERN win-def_wmi57objwql-->


	<!--RULE -->
<axsl:template match="win-def:wmi57_object/win-def:wql" priority="1000" mode="M323">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the wql entity of a wmi57_object should be 'equals', note that this overrules the general operation attribute validation (i.e. follow this one)<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M323"/></axsl:template><axsl:template match="text()" priority="-1" mode="M323"/><axsl:template match="@*|node()" priority="-2" mode="M323"><axsl:apply-templates select="@*|*" mode="M323"/></axsl:template>

<!--PATTERN win-def_wmi57steresult-->


	<!--RULE -->
<axsl:template match="win-def:wmi57_state/win-def:result" priority="1000" mode="M324">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='record'"/><axsl:otherwise>
            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the result entity of a wmi57_object must be 'record'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M324"/></axsl:template><axsl:template match="text()" priority="-1" mode="M324"/><axsl:template match="@*|node()" priority="-2" mode="M324"><axsl:apply-templates select="@*|*" mode="M324"/></axsl:template>

<!--PATTERN win-def_wuaupdatesearchertst-->


	<!--RULE -->
<axsl:template match="win-def:wuaupdatesearcher_test/win-def:object" priority="1001" mode="M325">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=ancestor::oval-def:oval_definitions/oval-def:objects/win-def:wuaupdatesearcher_object/@id"/><axsl:otherwise>
                            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a wuaupdatesearcher_test must reference a wuaupdatesearcher_object
                        <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M325"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:wuaupdatesearcher_test/win-def:state" priority="1000" mode="M325">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=ancestor::oval-def:oval_definitions/oval-def:states/win-def:wuaupdatesearcher_state/@id"/><axsl:otherwise>
                            <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a wuaupdatesearcher_test must reference a wuaupdatesearcher_state
                        <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M325"/></axsl:template><axsl:template match="text()" priority="-1" mode="M325"/><axsl:template match="@*|node()" priority="-2" mode="M325"><axsl:apply-templates select="@*|*" mode="M325"/></axsl:template>

<!--PATTERN win-def_wuaupdatesearcherobjsearchcriteria-->


	<!--RULE -->
<axsl:template match="win-def:wuaupdatesearcher_object/win-def:search_criteria" priority="1000" mode="M326">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals'"/><axsl:otherwise>
                                                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the search_criteria entity of a wuaupdatesearcher_object should be 'equals'
                                                    <axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M326"/></axsl:template><axsl:template match="text()" priority="-1" mode="M326"/><axsl:template match="@*|node()" priority="-2" mode="M326"><axsl:apply-templates select="@*|*" mode="M326"/></axsl:template>

<!--PATTERN win-def_ssr_stype_special_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:states/win-def:sharedresource_state/win-def:shared_type" priority="1000" mode="M327">

		<!--REPORT -->
<axsl:if test=".='STYPE_SPECIAL'">
                                                            DEPRECATED ELEMENT VALUE IN: sharedresource_state ELEMENT VALUE: <axsl:text/><axsl:value-of select="."/><axsl:text/>
                                                      <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M327"/></axsl:template><axsl:template match="text()" priority="-1" mode="M327"/><axsl:template match="@*|node()" priority="-2" mode="M327"><axsl:apply-templates select="@*|*" mode="M327"/></axsl:template>

<!--PATTERN win-def_ssr_stype_temporary_value_dep-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions/oval-def:states/win-def:sharedresource_state/win-def:shared_type" priority="1000" mode="M328">

		<!--REPORT -->
<axsl:if test=".='STYPE_TEMPORARY'">
                                                            DEPRECATED ELEMENT VALUE IN: sharedresource_state ELEMENT VALUE: <axsl:text/><axsl:value-of select="."/><axsl:text/>
                                                      <axsl:value-of select="string('&#10;')"/></axsl:if><axsl:apply-templates select="@*|*" mode="M328"/></axsl:template><axsl:template match="text()" priority="-1" mode="M328"/><axsl:template match="@*|node()" priority="-2" mode="M328"><axsl:apply-templates select="@*|*" mode="M328"/></axsl:template></axsl:stylesheet>
