<?xml version="1.0" standalone="yes"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:iso="http://purl.oclc.org/dsdl/schematron" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:aix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#aix" xmlns:apache-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#apache" xmlns:catos-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#catos" xmlns:esx-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#esx" xmlns:freebsd-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#freebsd" xmlns:hpux-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#hpux" xmlns:ios-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#ios" xmlns:linux-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns:macos-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#macos" xmlns:sol-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#solaris" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:win-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#windows" version="1.0"><!--Implementers: please note that overriding process-prolog or process-root is 
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
<axsl:template match="/"><axsl:apply-templates select="/" mode="M17"/><axsl:apply-templates select="/" mode="M18"/><axsl:apply-templates select="/" mode="M19"/><axsl:apply-templates select="/" mode="M20"/><axsl:apply-templates select="/" mode="M21"/><axsl:apply-templates select="/" mode="M22"/><axsl:apply-templates select="/" mode="M23"/><axsl:apply-templates select="/" mode="M24"/><axsl:apply-templates select="/" mode="M25"/><axsl:apply-templates select="/" mode="M26"/><axsl:apply-templates select="/" mode="M27"/><axsl:apply-templates select="/" mode="M28"/><axsl:apply-templates select="/" mode="M29"/><axsl:apply-templates select="/" mode="M30"/><axsl:apply-templates select="/" mode="M31"/><axsl:apply-templates select="/" mode="M32"/><axsl:apply-templates select="/" mode="M33"/><axsl:apply-templates select="/" mode="M34"/><axsl:apply-templates select="/" mode="M35"/><axsl:apply-templates select="/" mode="M36"/><axsl:apply-templates select="/" mode="M37"/><axsl:apply-templates select="/" mode="M38"/><axsl:apply-templates select="/" mode="M39"/><axsl:apply-templates select="/" mode="M40"/><axsl:apply-templates select="/" mode="M41"/><axsl:apply-templates select="/" mode="M42"/><axsl:apply-templates select="/" mode="M43"/><axsl:apply-templates select="/" mode="M44"/><axsl:apply-templates select="/" mode="M45"/><axsl:apply-templates select="/" mode="M46"/><axsl:apply-templates select="/" mode="M47"/><axsl:apply-templates select="/" mode="M48"/><axsl:apply-templates select="/" mode="M49"/><axsl:apply-templates select="/" mode="M50"/><axsl:apply-templates select="/" mode="M51"/><axsl:apply-templates select="/" mode="M52"/><axsl:apply-templates select="/" mode="M53"/><axsl:apply-templates select="/" mode="M54"/><axsl:apply-templates select="/" mode="M55"/><axsl:apply-templates select="/" mode="M56"/><axsl:apply-templates select="/" mode="M57"/><axsl:apply-templates select="/" mode="M58"/><axsl:apply-templates select="/" mode="M59"/><axsl:apply-templates select="/" mode="M60"/><axsl:apply-templates select="/" mode="M61"/><axsl:apply-templates select="/" mode="M62"/><axsl:apply-templates select="/" mode="M63"/><axsl:apply-templates select="/" mode="M64"/><axsl:apply-templates select="/" mode="M65"/><axsl:apply-templates select="/" mode="M66"/><axsl:apply-templates select="/" mode="M67"/><axsl:apply-templates select="/" mode="M68"/><axsl:apply-templates select="/" mode="M69"/><axsl:apply-templates select="/" mode="M70"/><axsl:apply-templates select="/" mode="M71"/><axsl:apply-templates select="/" mode="M72"/><axsl:apply-templates select="/" mode="M73"/><axsl:apply-templates select="/" mode="M74"/><axsl:apply-templates select="/" mode="M75"/><axsl:apply-templates select="/" mode="M76"/><axsl:apply-templates select="/" mode="M77"/><axsl:apply-templates select="/" mode="M78"/><axsl:apply-templates select="/" mode="M79"/><axsl:apply-templates select="/" mode="M80"/><axsl:apply-templates select="/" mode="M81"/><axsl:apply-templates select="/" mode="M82"/><axsl:apply-templates select="/" mode="M83"/><axsl:apply-templates select="/" mode="M84"/><axsl:apply-templates select="/" mode="M85"/><axsl:apply-templates select="/" mode="M86"/><axsl:apply-templates select="/" mode="M87"/><axsl:apply-templates select="/" mode="M88"/><axsl:apply-templates select="/" mode="M89"/><axsl:apply-templates select="/" mode="M90"/><axsl:apply-templates select="/" mode="M91"/><axsl:apply-templates select="/" mode="M92"/><axsl:apply-templates select="/" mode="M93"/><axsl:apply-templates select="/" mode="M94"/><axsl:apply-templates select="/" mode="M95"/><axsl:apply-templates select="/" mode="M96"/><axsl:apply-templates select="/" mode="M97"/><axsl:apply-templates select="/" mode="M98"/><axsl:apply-templates select="/" mode="M99"/><axsl:apply-templates select="/" mode="M100"/><axsl:apply-templates select="/" mode="M101"/><axsl:apply-templates select="/" mode="M102"/><axsl:apply-templates select="/" mode="M103"/><axsl:apply-templates select="/" mode="M104"/><axsl:apply-templates select="/" mode="M105"/><axsl:apply-templates select="/" mode="M106"/><axsl:apply-templates select="/" mode="M107"/><axsl:apply-templates select="/" mode="M108"/><axsl:apply-templates select="/" mode="M109"/><axsl:apply-templates select="/" mode="M110"/><axsl:apply-templates select="/" mode="M111"/><axsl:apply-templates select="/" mode="M112"/><axsl:apply-templates select="/" mode="M113"/><axsl:apply-templates select="/" mode="M114"/><axsl:apply-templates select="/" mode="M115"/><axsl:apply-templates select="/" mode="M116"/><axsl:apply-templates select="/" mode="M117"/><axsl:apply-templates select="/" mode="M118"/><axsl:apply-templates select="/" mode="M119"/><axsl:apply-templates select="/" mode="M120"/><axsl:apply-templates select="/" mode="M121"/><axsl:apply-templates select="/" mode="M122"/><axsl:apply-templates select="/" mode="M123"/><axsl:apply-templates select="/" mode="M124"/><axsl:apply-templates select="/" mode="M125"/><axsl:apply-templates select="/" mode="M126"/><axsl:apply-templates select="/" mode="M127"/><axsl:apply-templates select="/" mode="M128"/><axsl:apply-templates select="/" mode="M129"/><axsl:apply-templates select="/" mode="M130"/><axsl:apply-templates select="/" mode="M131"/><axsl:apply-templates select="/" mode="M132"/><axsl:apply-templates select="/" mode="M133"/><axsl:apply-templates select="/" mode="M134"/><axsl:apply-templates select="/" mode="M135"/><axsl:apply-templates select="/" mode="M136"/><axsl:apply-templates select="/" mode="M137"/><axsl:apply-templates select="/" mode="M138"/><axsl:apply-templates select="/" mode="M139"/><axsl:apply-templates select="/" mode="M140"/><axsl:apply-templates select="/" mode="M141"/><axsl:apply-templates select="/" mode="M142"/><axsl:apply-templates select="/" mode="M143"/><axsl:apply-templates select="/" mode="M144"/><axsl:apply-templates select="/" mode="M145"/><axsl:apply-templates select="/" mode="M146"/><axsl:apply-templates select="/" mode="M147"/><axsl:apply-templates select="/" mode="M148"/><axsl:apply-templates select="/" mode="M149"/><axsl:apply-templates select="/" mode="M150"/><axsl:apply-templates select="/" mode="M151"/><axsl:apply-templates select="/" mode="M152"/><axsl:apply-templates select="/" mode="M153"/><axsl:apply-templates select="/" mode="M154"/><axsl:apply-templates select="/" mode="M155"/><axsl:apply-templates select="/" mode="M156"/><axsl:apply-templates select="/" mode="M157"/><axsl:apply-templates select="/" mode="M158"/><axsl:apply-templates select="/" mode="M159"/><axsl:apply-templates select="/" mode="M160"/><axsl:apply-templates select="/" mode="M161"/><axsl:apply-templates select="/" mode="M162"/><axsl:apply-templates select="/" mode="M163"/><axsl:apply-templates select="/" mode="M164"/><axsl:apply-templates select="/" mode="M165"/><axsl:apply-templates select="/" mode="M166"/><axsl:apply-templates select="/" mode="M167"/><axsl:apply-templates select="/" mode="M168"/><axsl:apply-templates select="/" mode="M169"/><axsl:apply-templates select="/" mode="M170"/><axsl:apply-templates select="/" mode="M171"/><axsl:apply-templates select="/" mode="M172"/><axsl:apply-templates select="/" mode="M173"/><axsl:apply-templates select="/" mode="M174"/><axsl:apply-templates select="/" mode="M175"/><axsl:apply-templates select="/" mode="M176"/><axsl:apply-templates select="/" mode="M177"/><axsl:apply-templates select="/" mode="M178"/><axsl:apply-templates select="/" mode="M179"/><axsl:apply-templates select="/" mode="M180"/><axsl:apply-templates select="/" mode="M181"/><axsl:apply-templates select="/" mode="M182"/><axsl:apply-templates select="/" mode="M183"/><axsl:apply-templates select="/" mode="M184"/><axsl:apply-templates select="/" mode="M185"/><axsl:apply-templates select="/" mode="M186"/><axsl:apply-templates select="/" mode="M187"/><axsl:apply-templates select="/" mode="M188"/><axsl:apply-templates select="/" mode="M189"/><axsl:apply-templates select="/" mode="M190"/><axsl:apply-templates select="/" mode="M191"/><axsl:apply-templates select="/" mode="M192"/><axsl:apply-templates select="/" mode="M193"/><axsl:apply-templates select="/" mode="M194"/><axsl:apply-templates select="/" mode="M195"/><axsl:apply-templates select="/" mode="M196"/><axsl:apply-templates select="/" mode="M197"/><axsl:apply-templates select="/" mode="M198"/><axsl:apply-templates select="/" mode="M199"/><axsl:apply-templates select="/" mode="M200"/><axsl:apply-templates select="/" mode="M201"/><axsl:apply-templates select="/" mode="M202"/><axsl:apply-templates select="/" mode="M203"/><axsl:apply-templates select="/" mode="M204"/><axsl:apply-templates select="/" mode="M205"/><axsl:apply-templates select="/" mode="M206"/><axsl:apply-templates select="/" mode="M207"/><axsl:apply-templates select="/" mode="M208"/><axsl:apply-templates select="/" mode="M209"/><axsl:apply-templates select="/" mode="M210"/><axsl:apply-templates select="/" mode="M211"/><axsl:apply-templates select="/" mode="M212"/><axsl:apply-templates select="/" mode="M213"/><axsl:apply-templates select="/" mode="M214"/><axsl:apply-templates select="/" mode="M215"/><axsl:apply-templates select="/" mode="M216"/><axsl:apply-templates select="/" mode="M217"/><axsl:apply-templates select="/" mode="M218"/><axsl:apply-templates select="/" mode="M219"/><axsl:apply-templates select="/" mode="M220"/><axsl:apply-templates select="/" mode="M221"/><axsl:apply-templates select="/" mode="M222"/><axsl:apply-templates select="/" mode="M223"/><axsl:apply-templates select="/" mode="M224"/><axsl:apply-templates select="/" mode="M225"/><axsl:apply-templates select="/" mode="M226"/><axsl:apply-templates select="/" mode="M227"/><axsl:apply-templates select="/" mode="M228"/><axsl:apply-templates select="/" mode="M229"/><axsl:apply-templates select="/" mode="M230"/><axsl:apply-templates select="/" mode="M231"/><axsl:apply-templates select="/" mode="M232"/><axsl:apply-templates select="/" mode="M233"/><axsl:apply-templates select="/" mode="M234"/><axsl:apply-templates select="/" mode="M235"/><axsl:apply-templates select="/" mode="M236"/><axsl:apply-templates select="/" mode="M237"/><axsl:apply-templates select="/" mode="M238"/><axsl:apply-templates select="/" mode="M239"/><axsl:apply-templates select="/" mode="M240"/><axsl:apply-templates select="/" mode="M241"/><axsl:apply-templates select="/" mode="M242"/><axsl:apply-templates select="/" mode="M243"/><axsl:apply-templates select="/" mode="M244"/><axsl:apply-templates select="/" mode="M245"/><axsl:apply-templates select="/" mode="M246"/><axsl:apply-templates select="/" mode="M247"/><axsl:apply-templates select="/" mode="M248"/><axsl:apply-templates select="/" mode="M249"/><axsl:apply-templates select="/" mode="M250"/><axsl:apply-templates select="/" mode="M251"/><axsl:apply-templates select="/" mode="M252"/><axsl:apply-templates select="/" mode="M253"/><axsl:apply-templates select="/" mode="M254"/><axsl:apply-templates select="/" mode="M255"/><axsl:apply-templates select="/" mode="M256"/><axsl:apply-templates select="/" mode="M257"/><axsl:apply-templates select="/" mode="M258"/><axsl:apply-templates select="/" mode="M259"/><axsl:apply-templates select="/" mode="M260"/><axsl:apply-templates select="/" mode="M261"/><axsl:apply-templates select="/" mode="M262"/><axsl:apply-templates select="/" mode="M263"/><axsl:apply-templates select="/" mode="M264"/><axsl:apply-templates select="/" mode="M265"/><axsl:apply-templates select="/" mode="M266"/><axsl:apply-templates select="/" mode="M267"/><axsl:apply-templates select="/" mode="M268"/><axsl:apply-templates select="/" mode="M269"/><axsl:apply-templates select="/" mode="M270"/><axsl:apply-templates select="/" mode="M271"/><axsl:apply-templates select="/" mode="M272"/><axsl:apply-templates select="/" mode="M273"/><axsl:apply-templates select="/" mode="M274"/><axsl:apply-templates select="/" mode="M275"/><axsl:apply-templates select="/" mode="M276"/><axsl:apply-templates select="/" mode="M277"/><axsl:apply-templates select="/" mode="M278"/><axsl:apply-templates select="/" mode="M279"/><axsl:apply-templates select="/" mode="M280"/><axsl:apply-templates select="/" mode="M281"/><axsl:apply-templates select="/" mode="M282"/><axsl:apply-templates select="/" mode="M283"/><axsl:apply-templates select="/" mode="M284"/><axsl:apply-templates select="/" mode="M285"/><axsl:apply-templates select="/" mode="M286"/><axsl:apply-templates select="/" mode="M287"/><axsl:apply-templates select="/" mode="M288"/><axsl:apply-templates select="/" mode="M289"/><axsl:apply-templates select="/" mode="M290"/><axsl:apply-templates select="/" mode="M291"/><axsl:apply-templates select="/" mode="M292"/><axsl:apply-templates select="/" mode="M293"/><axsl:apply-templates select="/" mode="M294"/><axsl:apply-templates select="/" mode="M295"/><axsl:apply-templates select="/" mode="M296"/><axsl:apply-templates select="/" mode="M297"/><axsl:apply-templates select="/" mode="M298"/><axsl:apply-templates select="/" mode="M299"/><axsl:apply-templates select="/" mode="M300"/><axsl:apply-templates select="/" mode="M301"/><axsl:apply-templates select="/" mode="M302"/><axsl:apply-templates select="/" mode="M303"/><axsl:apply-templates select="/" mode="M304"/><axsl:apply-templates select="/" mode="M305"/><axsl:apply-templates select="/" mode="M306"/><axsl:apply-templates select="/" mode="M307"/><axsl:apply-templates select="/" mode="M308"/><axsl:apply-templates select="/" mode="M309"/><axsl:apply-templates select="/" mode="M310"/><axsl:apply-templates select="/" mode="M311"/><axsl:apply-templates select="/" mode="M312"/><axsl:apply-templates select="/" mode="M313"/><axsl:apply-templates select="/" mode="M314"/><axsl:apply-templates select="/" mode="M315"/><axsl:apply-templates select="/" mode="M316"/><axsl:apply-templates select="/" mode="M317"/><axsl:apply-templates select="/" mode="M318"/><axsl:apply-templates select="/" mode="M319"/><axsl:apply-templates select="/" mode="M320"/><axsl:apply-templates select="/" mode="M321"/><axsl:apply-templates select="/" mode="M322"/><axsl:apply-templates select="/" mode="M323"/><axsl:apply-templates select="/" mode="M324"/><axsl:apply-templates select="/" mode="M325"/><axsl:apply-templates select="/" mode="M326"/><axsl:apply-templates select="/" mode="M327"/><axsl:apply-templates select="/" mode="M328"/><axsl:apply-templates select="/" mode="M329"/><axsl:apply-templates select="/" mode="M330"/><axsl:apply-templates select="/" mode="M331"/><axsl:apply-templates select="/" mode="M332"/><axsl:apply-templates select="/" mode="M333"/><axsl:apply-templates select="/" mode="M334"/><axsl:apply-templates select="/" mode="M335"/><axsl:apply-templates select="/" mode="M336"/><axsl:apply-templates select="/" mode="M337"/><axsl:apply-templates select="/" mode="M338"/><axsl:apply-templates select="/" mode="M339"/><axsl:apply-templates select="/" mode="M340"/><axsl:apply-templates select="/" mode="M341"/><axsl:apply-templates select="/" mode="M342"/><axsl:apply-templates select="/" mode="M343"/><axsl:apply-templates select="/" mode="M344"/><axsl:apply-templates select="/" mode="M345"/><axsl:apply-templates select="/" mode="M346"/><axsl:apply-templates select="/" mode="M347"/><axsl:apply-templates select="/" mode="M348"/><axsl:apply-templates select="/" mode="M349"/><axsl:apply-templates select="/" mode="M350"/><axsl:apply-templates select="/" mode="M351"/><axsl:apply-templates select="/" mode="M352"/><axsl:apply-templates select="/" mode="M353"/><axsl:apply-templates select="/" mode="M354"/><axsl:apply-templates select="/" mode="M355"/><axsl:apply-templates select="/" mode="M356"/><axsl:apply-templates select="/" mode="M357"/><axsl:apply-templates select="/" mode="M358"/><axsl:apply-templates select="/" mode="M359"/><axsl:apply-templates select="/" mode="M360"/><axsl:apply-templates select="/" mode="M361"/><axsl:apply-templates select="/" mode="M362"/><axsl:apply-templates select="/" mode="M363"/><axsl:apply-templates select="/" mode="M364"/><axsl:apply-templates select="/" mode="M365"/><axsl:apply-templates select="/" mode="M366"/><axsl:apply-templates select="/" mode="M367"/><axsl:apply-templates select="/" mode="M368"/><axsl:apply-templates select="/" mode="M369"/><axsl:apply-templates select="/" mode="M370"/><axsl:apply-templates select="/" mode="M371"/><axsl:apply-templates select="/" mode="M372"/><axsl:apply-templates select="/" mode="M373"/><axsl:apply-templates select="/" mode="M374"/><axsl:apply-templates select="/" mode="M375"/><axsl:apply-templates select="/" mode="M376"/><axsl:apply-templates select="/" mode="M377"/><axsl:apply-templates select="/" mode="M378"/><axsl:apply-templates select="/" mode="M379"/><axsl:apply-templates select="/" mode="M380"/><axsl:apply-templates select="/" mode="M381"/><axsl:apply-templates select="/" mode="M382"/><axsl:apply-templates select="/" mode="M383"/><axsl:apply-templates select="/" mode="M384"/><axsl:apply-templates select="/" mode="M385"/><axsl:apply-templates select="/" mode="M386"/><axsl:apply-templates select="/" mode="M387"/><axsl:apply-templates select="/" mode="M388"/><axsl:apply-templates select="/" mode="M389"/><axsl:apply-templates select="/" mode="M390"/><axsl:apply-templates select="/" mode="M391"/><axsl:apply-templates select="/" mode="M392"/><axsl:apply-templates select="/" mode="M393"/><axsl:apply-templates select="/" mode="M394"/><axsl:apply-templates select="/" mode="M395"/><axsl:apply-templates select="/" mode="M396"/><axsl:apply-templates select="/" mode="M397"/><axsl:apply-templates select="/" mode="M398"/><axsl:apply-templates select="/" mode="M399"/><axsl:apply-templates select="/" mode="M400"/><axsl:apply-templates select="/" mode="M401"/><axsl:apply-templates select="/" mode="M402"/><axsl:apply-templates select="/" mode="M403"/><axsl:apply-templates select="/" mode="M404"/><axsl:apply-templates select="/" mode="M405"/><axsl:apply-templates select="/" mode="M406"/><axsl:apply-templates select="/" mode="M407"/><axsl:apply-templates select="/" mode="M408"/><axsl:apply-templates select="/" mode="M409"/><axsl:apply-templates select="/" mode="M410"/><axsl:apply-templates select="/" mode="M411"/><axsl:apply-templates select="/" mode="M412"/><axsl:apply-templates select="/" mode="M413"/><axsl:apply-templates select="/" mode="M414"/><axsl:apply-templates select="/" mode="M415"/><axsl:apply-templates select="/" mode="M416"/><axsl:apply-templates select="/" mode="M417"/><axsl:apply-templates select="/" mode="M418"/><axsl:apply-templates select="/" mode="M419"/><axsl:apply-templates select="/" mode="M420"/><axsl:apply-templates select="/" mode="M421"/><axsl:apply-templates select="/" mode="M422"/><axsl:apply-templates select="/" mode="M423"/><axsl:apply-templates select="/" mode="M424"/><axsl:apply-templates select="/" mode="M425"/><axsl:apply-templates select="/" mode="M426"/><axsl:apply-templates select="/" mode="M427"/><axsl:apply-templates select="/" mode="M428"/><axsl:apply-templates select="/" mode="M429"/><axsl:apply-templates select="/" mode="M430"/><axsl:apply-templates select="/" mode="M431"/><axsl:apply-templates select="/" mode="M432"/><axsl:apply-templates select="/" mode="M433"/><axsl:apply-templates select="/" mode="M434"/><axsl:apply-templates select="/" mode="M435"/><axsl:apply-templates select="/" mode="M436"/><axsl:apply-templates select="/" mode="M437"/><axsl:apply-templates select="/" mode="M438"/><axsl:apply-templates select="/" mode="M439"/><axsl:apply-templates select="/" mode="M440"/><axsl:apply-templates select="/" mode="M441"/><axsl:apply-templates select="/" mode="M442"/><axsl:apply-templates select="/" mode="M443"/><axsl:apply-templates select="/" mode="M444"/><axsl:apply-templates select="/" mode="M445"/><axsl:apply-templates select="/" mode="M446"/><axsl:apply-templates select="/" mode="M447"/><axsl:apply-templates select="/" mode="M448"/><axsl:apply-templates select="/" mode="M449"/><axsl:apply-templates select="/" mode="M450"/><axsl:apply-templates select="/" mode="M451"/><axsl:apply-templates select="/" mode="M452"/><axsl:apply-templates select="/" mode="M453"/><axsl:apply-templates select="/" mode="M454"/><axsl:apply-templates select="/" mode="M455"/><axsl:apply-templates select="/" mode="M456"/><axsl:apply-templates select="/" mode="M457"/><axsl:apply-templates select="/" mode="M458"/><axsl:apply-templates select="/" mode="M459"/><axsl:apply-templates select="/" mode="M460"/><axsl:apply-templates select="/" mode="M461"/><axsl:apply-templates select="/" mode="M462"/><axsl:apply-templates select="/" mode="M463"/><axsl:apply-templates select="/" mode="M464"/><axsl:apply-templates select="/" mode="M465"/><axsl:apply-templates select="/" mode="M466"/><axsl:apply-templates select="/" mode="M467"/><axsl:apply-templates select="/" mode="M468"/><axsl:apply-templates select="/" mode="M469"/><axsl:apply-templates select="/" mode="M470"/><axsl:apply-templates select="/" mode="M471"/><axsl:apply-templates select="/" mode="M472"/><axsl:apply-templates select="/" mode="M473"/><axsl:apply-templates select="/" mode="M474"/><axsl:apply-templates select="/" mode="M475"/><axsl:apply-templates select="/" mode="M476"/><axsl:apply-templates select="/" mode="M477"/><axsl:apply-templates select="/" mode="M478"/><axsl:apply-templates select="/" mode="M479"/><axsl:apply-templates select="/" mode="M480"/><axsl:apply-templates select="/" mode="M481"/><axsl:apply-templates select="/" mode="M482"/><axsl:apply-templates select="/" mode="M483"/><axsl:apply-templates select="/" mode="M484"/><axsl:apply-templates select="/" mode="M485"/><axsl:apply-templates select="/" mode="M486"/><axsl:apply-templates select="/" mode="M487"/><axsl:apply-templates select="/" mode="M488"/><axsl:apply-templates select="/" mode="M489"/><axsl:apply-templates select="/" mode="M490"/><axsl:apply-templates select="/" mode="M491"/><axsl:apply-templates select="/" mode="M492"/><axsl:apply-templates select="/" mode="M493"/><axsl:apply-templates select="/" mode="M494"/><axsl:apply-templates select="/" mode="M495"/><axsl:apply-templates select="/" mode="M496"/><axsl:apply-templates select="/" mode="M497"/><axsl:apply-templates select="/" mode="M498"/><axsl:apply-templates select="/" mode="M499"/><axsl:apply-templates select="/" mode="M500"/><axsl:apply-templates select="/" mode="M501"/><axsl:apply-templates select="/" mode="M502"/><axsl:apply-templates select="/" mode="M503"/><axsl:apply-templates select="/" mode="M504"/><axsl:apply-templates select="/" mode="M505"/><axsl:apply-templates select="/" mode="M506"/><axsl:apply-templates select="/" mode="M507"/><axsl:apply-templates select="/" mode="M508"/><axsl:apply-templates select="/" mode="M509"/><axsl:apply-templates select="/" mode="M510"/><axsl:apply-templates select="/" mode="M511"/><axsl:apply-templates select="/" mode="M512"/><axsl:apply-templates select="/" mode="M513"/><axsl:apply-templates select="/" mode="M514"/><axsl:apply-templates select="/" mode="M515"/><axsl:apply-templates select="/" mode="M516"/><axsl:apply-templates select="/" mode="M517"/><axsl:apply-templates select="/" mode="M518"/><axsl:apply-templates select="/" mode="M519"/><axsl:apply-templates select="/" mode="M520"/><axsl:apply-templates select="/" mode="M521"/><axsl:apply-templates select="/" mode="M522"/><axsl:apply-templates select="/" mode="M523"/><axsl:apply-templates select="/" mode="M524"/><axsl:apply-templates select="/" mode="M525"/><axsl:apply-templates select="/" mode="M526"/><axsl:apply-templates select="/" mode="M527"/><axsl:apply-templates select="/" mode="M528"/><axsl:apply-templates select="/" mode="M529"/><axsl:apply-templates select="/" mode="M530"/><axsl:apply-templates select="/" mode="M531"/><axsl:apply-templates select="/" mode="M532"/><axsl:apply-templates select="/" mode="M533"/><axsl:apply-templates select="/" mode="M534"/><axsl:apply-templates select="/" mode="M535"/><axsl:apply-templates select="/" mode="M536"/><axsl:apply-templates select="/" mode="M537"/><axsl:apply-templates select="/" mode="M538"/><axsl:apply-templates select="/" mode="M539"/><axsl:apply-templates select="/" mode="M540"/><axsl:apply-templates select="/" mode="M541"/><axsl:apply-templates select="/" mode="M542"/><axsl:apply-templates select="/" mode="M543"/><axsl:apply-templates select="/" mode="M544"/><axsl:apply-templates select="/" mode="M545"/><axsl:apply-templates select="/" mode="M546"/><axsl:apply-templates select="/" mode="M547"/><axsl:apply-templates select="/" mode="M548"/><axsl:apply-templates select="/" mode="M549"/><axsl:apply-templates select="/" mode="M550"/><axsl:apply-templates select="/" mode="M551"/><axsl:apply-templates select="/" mode="M552"/><axsl:apply-templates select="/" mode="M553"/><axsl:apply-templates select="/" mode="M554"/><axsl:apply-templates select="/" mode="M555"/><axsl:apply-templates select="/" mode="M556"/><axsl:apply-templates select="/" mode="M557"/><axsl:apply-templates select="/" mode="M558"/><axsl:apply-templates select="/" mode="M559"/><axsl:apply-templates select="/" mode="M560"/><axsl:apply-templates select="/" mode="M561"/><axsl:apply-templates select="/" mode="M562"/><axsl:apply-templates select="/" mode="M563"/><axsl:apply-templates select="/" mode="M564"/><axsl:apply-templates select="/" mode="M565"/><axsl:apply-templates select="/" mode="M566"/><axsl:apply-templates select="/" mode="M567"/><axsl:apply-templates select="/" mode="M568"/><axsl:apply-templates select="/" mode="M569"/><axsl:apply-templates select="/" mode="M570"/><axsl:apply-templates select="/" mode="M571"/><axsl:apply-templates select="/" mode="M572"/><axsl:apply-templates select="/" mode="M573"/><axsl:apply-templates select="/" mode="M574"/><axsl:apply-templates select="/" mode="M575"/><axsl:apply-templates select="/" mode="M576"/><axsl:apply-templates select="/" mode="M577"/><axsl:apply-templates select="/" mode="M578"/><axsl:apply-templates select="/" mode="M579"/><axsl:apply-templates select="/" mode="M580"/><axsl:apply-templates select="/" mode="M581"/><axsl:apply-templates select="/" mode="M582"/><axsl:apply-templates select="/" mode="M583"/><axsl:apply-templates select="/" mode="M584"/><axsl:apply-templates select="/" mode="M585"/><axsl:apply-templates select="/" mode="M586"/><axsl:apply-templates select="/" mode="M587"/><axsl:apply-templates select="/" mode="M588"/><axsl:apply-templates select="/" mode="M589"/><axsl:apply-templates select="/" mode="M590"/><axsl:apply-templates select="/" mode="M591"/><axsl:apply-templates select="/" mode="M592"/><axsl:apply-templates select="/" mode="M593"/><axsl:apply-templates select="/" mode="M594"/><axsl:apply-templates select="/" mode="M595"/><axsl:apply-templates select="/" mode="M596"/><axsl:apply-templates select="/" mode="M597"/><axsl:apply-templates select="/" mode="M598"/><axsl:apply-templates select="/" mode="M599"/><axsl:apply-templates select="/" mode="M600"/><axsl:apply-templates select="/" mode="M601"/><axsl:apply-templates select="/" mode="M602"/><axsl:apply-templates select="/" mode="M603"/><axsl:apply-templates select="/" mode="M604"/><axsl:apply-templates select="/" mode="M605"/><axsl:apply-templates select="/" mode="M606"/><axsl:apply-templates select="/" mode="M607"/><axsl:apply-templates select="/" mode="M608"/><axsl:apply-templates select="/" mode="M609"/><axsl:apply-templates select="/" mode="M610"/><axsl:apply-templates select="/" mode="M611"/><axsl:apply-templates select="/" mode="M612"/><axsl:apply-templates select="/" mode="M613"/><axsl:apply-templates select="/" mode="M614"/><axsl:apply-templates select="/" mode="M615"/><axsl:apply-templates select="/" mode="M616"/><axsl:apply-templates select="/" mode="M617"/><axsl:apply-templates select="/" mode="M618"/><axsl:apply-templates select="/" mode="M619"/><axsl:apply-templates select="/" mode="M620"/><axsl:apply-templates select="/" mode="M621"/><axsl:apply-templates select="/" mode="M622"/><axsl:apply-templates select="/" mode="M623"/><axsl:apply-templates select="/" mode="M624"/><axsl:apply-templates select="/" mode="M625"/><axsl:apply-templates select="/" mode="M626"/><axsl:apply-templates select="/" mode="M627"/><axsl:apply-templates select="/" mode="M628"/><axsl:apply-templates select="/" mode="M629"/><axsl:apply-templates select="/" mode="M630"/><axsl:apply-templates select="/" mode="M631"/><axsl:apply-templates select="/" mode="M632"/><axsl:apply-templates select="/" mode="M633"/><axsl:apply-templates select="/" mode="M634"/><axsl:apply-templates select="/" mode="M635"/><axsl:apply-templates select="/" mode="M636"/><axsl:apply-templates select="/" mode="M637"/><axsl:apply-templates select="/" mode="M638"/><axsl:apply-templates select="/" mode="M639"/><axsl:apply-templates select="/" mode="M640"/><axsl:apply-templates select="/" mode="M641"/><axsl:apply-templates select="/" mode="M642"/><axsl:apply-templates select="/" mode="M643"/><axsl:apply-templates select="/" mode="M644"/><axsl:apply-templates select="/" mode="M645"/><axsl:apply-templates select="/" mode="M646"/><axsl:apply-templates select="/" mode="M647"/><axsl:apply-templates select="/" mode="M648"/><axsl:apply-templates select="/" mode="M649"/><axsl:apply-templates select="/" mode="M650"/><axsl:apply-templates select="/" mode="M651"/><axsl:apply-templates select="/" mode="M652"/><axsl:apply-templates select="/" mode="M653"/><axsl:apply-templates select="/" mode="M654"/><axsl:apply-templates select="/" mode="M655"/><axsl:apply-templates select="/" mode="M656"/><axsl:apply-templates select="/" mode="M657"/><axsl:apply-templates select="/" mode="M658"/><axsl:apply-templates select="/" mode="M659"/><axsl:apply-templates select="/" mode="M660"/><axsl:apply-templates select="/" mode="M661"/><axsl:apply-templates select="/" mode="M662"/><axsl:apply-templates select="/" mode="M663"/><axsl:apply-templates select="/" mode="M664"/><axsl:apply-templates select="/" mode="M665"/><axsl:apply-templates select="/" mode="M666"/><axsl:apply-templates select="/" mode="M667"/><axsl:apply-templates select="/" mode="M668"/><axsl:apply-templates select="/" mode="M669"/><axsl:apply-templates select="/" mode="M670"/><axsl:apply-templates select="/" mode="M671"/><axsl:apply-templates select="/" mode="M672"/><axsl:apply-templates select="/" mode="M673"/><axsl:apply-templates select="/" mode="M674"/><axsl:apply-templates select="/" mode="M675"/><axsl:apply-templates select="/" mode="M676"/><axsl:apply-templates select="/" mode="M677"/><axsl:apply-templates select="/" mode="M678"/><axsl:apply-templates select="/" mode="M679"/><axsl:apply-templates select="/" mode="M680"/><axsl:apply-templates select="/" mode="M681"/><axsl:apply-templates select="/" mode="M682"/><axsl:apply-templates select="/" mode="M683"/><axsl:apply-templates select="/" mode="M684"/><axsl:apply-templates select="/" mode="M685"/><axsl:apply-templates select="/" mode="M686"/><axsl:apply-templates select="/" mode="M687"/><axsl:apply-templates select="/" mode="M688"/><axsl:apply-templates select="/" mode="M689"/><axsl:apply-templates select="/" mode="M690"/><axsl:apply-templates select="/" mode="M691"/><axsl:apply-templates select="/" mode="M692"/><axsl:apply-templates select="/" mode="M693"/><axsl:apply-templates select="/" mode="M694"/><axsl:apply-templates select="/" mode="M695"/><axsl:apply-templates select="/" mode="M696"/><axsl:apply-templates select="/" mode="M697"/><axsl:apply-templates select="/" mode="M698"/><axsl:apply-templates select="/" mode="M699"/><axsl:apply-templates select="/" mode="M700"/><axsl:apply-templates select="/" mode="M701"/><axsl:apply-templates select="/" mode="M702"/><axsl:apply-templates select="/" mode="M703"/><axsl:apply-templates select="/" mode="M704"/><axsl:apply-templates select="/" mode="M705"/><axsl:apply-templates select="/" mode="M706"/><axsl:apply-templates select="/" mode="M707"/><axsl:apply-templates select="/" mode="M708"/><axsl:apply-templates select="/" mode="M709"/><axsl:apply-templates select="/" mode="M710"/><axsl:apply-templates select="/" mode="M711"/><axsl:apply-templates select="/" mode="M712"/><axsl:apply-templates select="/" mode="M713"/><axsl:apply-templates select="/" mode="M714"/><axsl:apply-templates select="/" mode="M715"/><axsl:apply-templates select="/" mode="M716"/><axsl:apply-templates select="/" mode="M717"/><axsl:apply-templates select="/" mode="M718"/><axsl:apply-templates select="/" mode="M719"/><axsl:apply-templates select="/" mode="M720"/><axsl:apply-templates select="/" mode="M721"/><axsl:apply-templates select="/" mode="M722"/><axsl:apply-templates select="/" mode="M723"/><axsl:apply-templates select="/" mode="M724"/><axsl:apply-templates select="/" mode="M725"/><axsl:apply-templates select="/" mode="M726"/><axsl:apply-templates select="/" mode="M727"/><axsl:apply-templates select="/" mode="M728"/><axsl:apply-templates select="/" mode="M729"/><axsl:apply-templates select="/" mode="M730"/><axsl:apply-templates select="/" mode="M731"/><axsl:apply-templates select="/" mode="M732"/><axsl:apply-templates select="/" mode="M733"/><axsl:apply-templates select="/" mode="M734"/><axsl:apply-templates select="/" mode="M735"/><axsl:apply-templates select="/" mode="M736"/><axsl:apply-templates select="/" mode="M737"/><axsl:apply-templates select="/" mode="M738"/><axsl:apply-templates select="/" mode="M739"/><axsl:apply-templates select="/" mode="M740"/><axsl:apply-templates select="/" mode="M741"/><axsl:apply-templates select="/" mode="M742"/><axsl:apply-templates select="/" mode="M743"/><axsl:apply-templates select="/" mode="M744"/><axsl:apply-templates select="/" mode="M745"/><axsl:apply-templates select="/" mode="M746"/><axsl:apply-templates select="/" mode="M747"/><axsl:apply-templates select="/" mode="M748"/><axsl:apply-templates select="/" mode="M749"/><axsl:apply-templates select="/" mode="M750"/><axsl:apply-templates select="/" mode="M751"/><axsl:apply-templates select="/" mode="M752"/><axsl:apply-templates select="/" mode="M753"/><axsl:apply-templates select="/" mode="M754"/><axsl:apply-templates select="/" mode="M755"/><axsl:apply-templates select="/" mode="M756"/><axsl:apply-templates select="/" mode="M757"/><axsl:apply-templates select="/" mode="M758"/><axsl:apply-templates select="/" mode="M759"/><axsl:apply-templates select="/" mode="M760"/><axsl:apply-templates select="/" mode="M761"/><axsl:apply-templates select="/" mode="M762"/><axsl:apply-templates select="/" mode="M763"/><axsl:apply-templates select="/" mode="M764"/><axsl:apply-templates select="/" mode="M765"/><axsl:apply-templates select="/" mode="M766"/><axsl:apply-templates select="/" mode="M767"/><axsl:apply-templates select="/" mode="M768"/><axsl:apply-templates select="/" mode="M769"/><axsl:apply-templates select="/" mode="M770"/><axsl:apply-templates select="/" mode="M771"/><axsl:apply-templates select="/" mode="M772"/><axsl:apply-templates select="/" mode="M773"/><axsl:apply-templates select="/" mode="M774"/><axsl:apply-templates select="/" mode="M775"/><axsl:apply-templates select="/" mode="M776"/><axsl:apply-templates select="/" mode="M777"/><axsl:apply-templates select="/" mode="M778"/><axsl:apply-templates select="/" mode="M779"/><axsl:apply-templates select="/" mode="M780"/><axsl:apply-templates select="/" mode="M781"/><axsl:apply-templates select="/" mode="M782"/><axsl:apply-templates select="/" mode="M783"/><axsl:apply-templates select="/" mode="M784"/><axsl:apply-templates select="/" mode="M785"/><axsl:apply-templates select="/" mode="M786"/><axsl:apply-templates select="/" mode="M787"/><axsl:apply-templates select="/" mode="M788"/><axsl:apply-templates select="/" mode="M789"/><axsl:apply-templates select="/" mode="M790"/><axsl:apply-templates select="/" mode="M791"/><axsl:apply-templates select="/" mode="M792"/><axsl:apply-templates select="/" mode="M793"/><axsl:apply-templates select="/" mode="M794"/><axsl:apply-templates select="/" mode="M795"/><axsl:apply-templates select="/" mode="M796"/><axsl:apply-templates select="/" mode="M797"/><axsl:apply-templates select="/" mode="M798"/><axsl:apply-templates select="/" mode="M799"/><axsl:apply-templates select="/" mode="M800"/><axsl:apply-templates select="/" mode="M801"/><axsl:apply-templates select="/" mode="M802"/><axsl:apply-templates select="/" mode="M803"/><axsl:apply-templates select="/" mode="M804"/><axsl:apply-templates select="/" mode="M805"/><axsl:apply-templates select="/" mode="M806"/><axsl:apply-templates select="/" mode="M807"/><axsl:apply-templates select="/" mode="M808"/><axsl:apply-templates select="/" mode="M809"/><axsl:apply-templates select="/" mode="M810"/><axsl:apply-templates select="/" mode="M811"/><axsl:apply-templates select="/" mode="M812"/><axsl:apply-templates select="/" mode="M813"/><axsl:apply-templates select="/" mode="M814"/><axsl:apply-templates select="/" mode="M815"/><axsl:apply-templates select="/" mode="M816"/><axsl:apply-templates select="/" mode="M817"/><axsl:apply-templates select="/" mode="M818"/><axsl:apply-templates select="/" mode="M819"/><axsl:apply-templates select="/" mode="M820"/><axsl:apply-templates select="/" mode="M821"/><axsl:apply-templates select="/" mode="M822"/><axsl:apply-templates select="/" mode="M823"/><axsl:apply-templates select="/" mode="M824"/><axsl:apply-templates select="/" mode="M825"/><axsl:apply-templates select="/" mode="M826"/><axsl:apply-templates select="/" mode="M827"/><axsl:apply-templates select="/" mode="M828"/><axsl:apply-templates select="/" mode="M829"/><axsl:apply-templates select="/" mode="M830"/><axsl:apply-templates select="/" mode="M831"/><axsl:apply-templates select="/" mode="M832"/><axsl:apply-templates select="/" mode="M833"/><axsl:apply-templates select="/" mode="M834"/><axsl:apply-templates select="/" mode="M835"/><axsl:apply-templates select="/" mode="M836"/><axsl:apply-templates select="/" mode="M837"/><axsl:apply-templates select="/" mode="M838"/><axsl:apply-templates select="/" mode="M839"/><axsl:apply-templates select="/" mode="M840"/><axsl:apply-templates select="/" mode="M841"/><axsl:apply-templates select="/" mode="M842"/><axsl:apply-templates select="/" mode="M843"/><axsl:apply-templates select="/" mode="M844"/><axsl:apply-templates select="/" mode="M845"/><axsl:apply-templates select="/" mode="M846"/><axsl:apply-templates select="/" mode="M847"/><axsl:apply-templates select="/" mode="M848"/><axsl:apply-templates select="/" mode="M849"/><axsl:apply-templates select="/" mode="M850"/><axsl:apply-templates select="/" mode="M851"/><axsl:apply-templates select="/" mode="M852"/><axsl:apply-templates select="/" mode="M853"/><axsl:apply-templates select="/" mode="M854"/><axsl:apply-templates select="/" mode="M855"/><axsl:apply-templates select="/" mode="M856"/><axsl:apply-templates select="/" mode="M857"/><axsl:apply-templates select="/" mode="M858"/><axsl:apply-templates select="/" mode="M859"/><axsl:apply-templates select="/" mode="M860"/><axsl:apply-templates select="/" mode="M861"/><axsl:apply-templates select="/" mode="M862"/><axsl:apply-templates select="/" mode="M863"/><axsl:apply-templates select="/" mode="M864"/><axsl:apply-templates select="/" mode="M865"/><axsl:apply-templates select="/" mode="M866"/><axsl:apply-templates select="/" mode="M867"/><axsl:apply-templates select="/" mode="M868"/><axsl:apply-templates select="/" mode="M869"/><axsl:apply-templates select="/" mode="M870"/><axsl:apply-templates select="/" mode="M871"/><axsl:apply-templates select="/" mode="M872"/><axsl:apply-templates select="/" mode="M873"/><axsl:apply-templates select="/" mode="M874"/><axsl:apply-templates select="/" mode="M875"/><axsl:apply-templates select="/" mode="M876"/><axsl:apply-templates select="/" mode="M877"/><axsl:apply-templates select="/" mode="M878"/><axsl:apply-templates select="/" mode="M879"/><axsl:apply-templates select="/" mode="M880"/><axsl:apply-templates select="/" mode="M881"/><axsl:apply-templates select="/" mode="M882"/><axsl:apply-templates select="/" mode="M883"/><axsl:apply-templates select="/" mode="M884"/><axsl:apply-templates select="/" mode="M885"/><axsl:apply-templates select="/" mode="M886"/><axsl:apply-templates select="/" mode="M887"/><axsl:apply-templates select="/" mode="M888"/><axsl:apply-templates select="/" mode="M889"/><axsl:apply-templates select="/" mode="M890"/><axsl:apply-templates select="/" mode="M891"/><axsl:apply-templates select="/" mode="M892"/><axsl:apply-templates select="/" mode="M893"/><axsl:apply-templates select="/" mode="M894"/><axsl:apply-templates select="/" mode="M895"/><axsl:apply-templates select="/" mode="M896"/><axsl:apply-templates select="/" mode="M897"/><axsl:apply-templates select="/" mode="M898"/><axsl:apply-templates select="/" mode="M899"/><axsl:apply-templates select="/" mode="M900"/><axsl:apply-templates select="/" mode="M901"/><axsl:apply-templates select="/" mode="M902"/><axsl:apply-templates select="/" mode="M903"/><axsl:apply-templates select="/" mode="M904"/><axsl:apply-templates select="/" mode="M905"/><axsl:apply-templates select="/" mode="M906"/><axsl:apply-templates select="/" mode="M907"/><axsl:apply-templates select="/" mode="M908"/><axsl:apply-templates select="/" mode="M909"/><axsl:apply-templates select="/" mode="M910"/><axsl:apply-templates select="/" mode="M911"/></axsl:template>

<!--SCHEMATRON PATTERNS-->


<!--PATTERN empty_def_doc-->


	<!--RULE -->
<axsl:template match="oval-def:oval_definitions" priority="1000" mode="M17">

		<!--ASSERT -->
<axsl:choose><axsl:when test="oval-def:definitions or oval-def:tests or oval-def:objects or oval-def:states or oval-def:variables"/><axsl:otherwise>A valid OVAL Definition document must contain at least one definitions, tests, objects, states, or variables element. The optional definitions, tests, objects, states, and variables sections define the specific characteristics that should be evaluated on a system to determine the truth values of the OVAL Definition Document. To be valid though, at least one definitions, tests, objects, states, or variables element must be present.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M17"/></axsl:template><axsl:template match="text()" priority="-1" mode="M17"/><axsl:template match="@*|node()" priority="-2" mode="M17"><axsl:apply-templates select="@*|*" mode="M17"/></axsl:template>

<!--PATTERN required_criteria-->


	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:definitions/oval-def:definition[@deprecated='false' or not(@deprecated)]" priority="1000" mode="M18">

		<!--ASSERT -->
<axsl:choose><axsl:when test="oval-def:criteria"/><axsl:otherwise>A valid OVAL Definition document must contain a criteria unless the definition is a deprecated definition.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M18"/></axsl:template><axsl:template match="text()" priority="-1" mode="M18"/><axsl:template match="@*|node()" priority="-2" mode="M18"><axsl:apply-templates select="@*|*" mode="M18"/></axsl:template>

<!--PATTERN test_type-->


	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:tests/*[@check_existence='none_exist']" priority="1000" mode="M19">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(*[name()='state'])"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="@id"/><axsl:text/> - No state should be referenced when check_existence has a value of 'none_exist'.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M19"/></axsl:template><axsl:template match="text()" priority="-1" mode="M19"/><axsl:template match="@*|node()" priority="-2" mode="M19"><axsl:apply-templates select="@*|*" mode="M19"/></axsl:template>

<!--PATTERN setobjref-->


	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:object_reference" priority="1002" mode="M20">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template>

	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:object_reference" priority="1001" mode="M20">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template>

	<!--RULE -->
<axsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:set/oval-def:object_reference" priority="1000" mode="M20">

		<!--ASSERT -->
<axsl:choose><axsl:when test="name(./../../../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../../../../@id"/><axsl:text/> - Each object referenced by the set must be of the same type as parent object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template><axsl:template match="text()" priority="-1" mode="M20"/><axsl:template match="@*|node()" priority="-2" mode="M20"><axsl:apply-templates select="@*|*" mode="M20"/></axsl:template>

<!--PATTERN value-->


	<!--RULE -->
<axsl:template match="oval-def:constant_variable/oval-def:value" priority="1000" mode="M21">

		<!--ASSERT -->
<axsl:choose><axsl:when test=".!=''"/><axsl:otherwise>The value element of the constant_variable <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> can not be empty.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M21"/></axsl:template><axsl:template match="text()" priority="-1" mode="M21"/><axsl:template match="@*|node()" priority="-2" mode="M21"><axsl:apply-templates select="@*|*" mode="M21"/></axsl:template>

<!--PATTERN literal_component-->


	<!--RULE -->
<axsl:template match="oval-def:literal_component" priority="1000" mode="M22">

		<!--ASSERT -->
<axsl:choose><axsl:when test=".!=''"/><axsl:otherwise>The literal_component of variable <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> can not be empty.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template><axsl:template match="text()" priority="-1" mode="M22"/><axsl:template match="@*|node()" priority="-2" mode="M22"><axsl:apply-templates select="@*|*" mode="M22"/></axsl:template>

<!--PATTERN item_field-->


	<!--RULE -->
<axsl:template match="oval-def:object_component" priority="1000" mode="M23">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@item_field!=''"/><axsl:otherwise>The item_field attribute of an object_component of variable <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> can not be empty.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template><axsl:template match="text()" priority="-1" mode="M23"/><axsl:template match="@*|node()" priority="-2" mode="M23"><axsl:apply-templates select="@*|*" mode="M23"/></axsl:template>

<!--PATTERN entityrules-->


	<!--RULE -->
<axsl:template match="oval-def:objects/*/*|oval-def:states/*/*" priority="1000" mode="M24"><axsl:variable name="var_ref" select="@var_ref"/>

		<!--ASSERT -->
<axsl:choose><axsl:when test="((not(@datatype) and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='case insensitive equals' or @operation='case insensitive not equal' or @operation='pattern match')) or (@datatype='binary' and (not(@operation) or @operation='equals' or @operation='not equal')) or (@datatype='boolean' and (not(@operation) or @operation='equals' or @operation='not equal')) or (@datatype='evr_string' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')) or (@datatype='fileset_revision' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')) or (@datatype='float' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')) or (@datatype='ios_version' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')) or (@datatype='int' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='bitwise and' or @operation='bitwise or')) or (@datatype='string' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match')) or (@datatype='version' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')))"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the supplied operation attribute for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity is not valid given a datatype of '<axsl:text/><axsl:value-of select="@datatype"/><axsl:text/>'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@var_ref) or .=''"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a var-ref has been supplied for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity so no value should be provided<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template><axsl:template match="text()" priority="-1" mode="M24"/><axsl:template match="@*|node()" priority="-2" mode="M24"><axsl:apply-templates select="@*|*" mode="M24"/></axsl:template>

<!--PATTERN famtst-->


	<!--RULE -->
<axsl:template match="ind-def:family_test/ind-def:object" priority="1001" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:family_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a family_test must reference a family_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:family_test/ind-def:state" priority="1000" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:family_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a family_test must reference a family_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template><axsl:template match="text()" priority="-1" mode="M25"/><axsl:template match="@*|node()" priority="-2" mode="M25"><axsl:apply-templates select="@*|*" mode="M25"/></axsl:template>

<!--PATTERN famstefamily-->


	<!--RULE -->
<axsl:template match="ind-def:family_state/ind-def:family" priority="1000" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the family entity of a family_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template><axsl:template match="text()" priority="-1" mode="M26"/><axsl:template match="@*|node()" priority="-2" mode="M26"><axsl:apply-templates select="@*|*" mode="M26"/></axsl:template>

<!--PATTERN md5tst-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_test/ind-def:object" priority="1001" mode="M27">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:filemd5_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a filemd5_test must reference a filemd5_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:filemd5_test/ind-def:state" priority="1000" mode="M27">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:filemd5_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a filemd5_test must reference a filemd5_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template><axsl:template match="text()" priority="-1" mode="M27"/><axsl:template match="@*|node()" priority="-2" mode="M27"><axsl:apply-templates select="@*|*" mode="M27"/></axsl:template>

<!--PATTERN md5objpath-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_object/ind-def:path" priority="1000" mode="M28">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filemd5_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template><axsl:template match="text()" priority="-1" mode="M28"/><axsl:template match="@*|node()" priority="-2" mode="M28"><axsl:apply-templates select="@*|*" mode="M28"/></axsl:template>

<!--PATTERN md5objfilename-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_object/ind-def:filename" priority="1000" mode="M29">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filemd5_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M29"/></axsl:template><axsl:template match="text()" priority="-1" mode="M29"/><axsl:template match="@*|node()" priority="-2" mode="M29"><axsl:apply-templates select="@*|*" mode="M29"/></axsl:template>

<!--PATTERN md5stepath-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_state/ind-def:path" priority="1000" mode="M30">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filemd5_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template><axsl:template match="text()" priority="-1" mode="M30"/><axsl:template match="@*|node()" priority="-2" mode="M30"><axsl:apply-templates select="@*|*" mode="M30"/></axsl:template>

<!--PATTERN md5stefilename-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_state/ind-def:filename" priority="1000" mode="M31">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filemd5_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M31"/></axsl:template><axsl:template match="text()" priority="-1" mode="M31"/><axsl:template match="@*|node()" priority="-2" mode="M31"><axsl:apply-templates select="@*|*" mode="M31"/></axsl:template>

<!--PATTERN md5stemd5-->


	<!--RULE -->
<axsl:template match="ind-def:filemd5_state/ind-def:md5" priority="1000" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the md5 entity of a filemd5_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template><axsl:template match="text()" priority="-1" mode="M32"/><axsl:template match="@*|node()" priority="-2" mode="M32"><axsl:apply-templates select="@*|*" mode="M32"/></axsl:template>

<!--PATTERN hashtst-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_test/ind-def:object" priority="1001" mode="M33">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:filehash_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a filehash_test must reference a filesha1_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:filehash_test/ind-def:state" priority="1000" mode="M33">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:filehash_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a filehash_test must reference a filesha1_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template><axsl:template match="text()" priority="-1" mode="M33"/><axsl:template match="@*|node()" priority="-2" mode="M33"><axsl:apply-templates select="@*|*" mode="M33"/></axsl:template>

<!--PATTERN hashobjpath-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_object/ind-def:path" priority="1000" mode="M34">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filehash_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M34"/></axsl:template><axsl:template match="text()" priority="-1" mode="M34"/><axsl:template match="@*|node()" priority="-2" mode="M34"><axsl:apply-templates select="@*|*" mode="M34"/></axsl:template>

<!--PATTERN hashobjfilename-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_object/ind-def:filename" priority="1000" mode="M35">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filehash_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M35"/></axsl:template><axsl:template match="text()" priority="-1" mode="M35"/><axsl:template match="@*|node()" priority="-2" mode="M35"><axsl:apply-templates select="@*|*" mode="M35"/></axsl:template>

<!--PATTERN hashstepath-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state/ind-def:path" priority="1000" mode="M36">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filehash_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M36"/></axsl:template><axsl:template match="text()" priority="-1" mode="M36"/><axsl:template match="@*|node()" priority="-2" mode="M36"><axsl:apply-templates select="@*|*" mode="M36"/></axsl:template>

<!--PATTERN hashstefilename-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state/ind-def:filename" priority="1000" mode="M37">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filehash_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M37"/></axsl:template><axsl:template match="text()" priority="-1" mode="M37"/><axsl:template match="@*|node()" priority="-2" mode="M37"><axsl:apply-templates select="@*|*" mode="M37"/></axsl:template>

<!--PATTERN hashstemd5-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state/ind-def:md5" priority="1000" mode="M38">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the md5 entity of a filehash_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M38"/></axsl:template><axsl:template match="text()" priority="-1" mode="M38"/><axsl:template match="@*|node()" priority="-2" mode="M38"><axsl:apply-templates select="@*|*" mode="M38"/></axsl:template>

<!--PATTERN hashstesha1-->


	<!--RULE -->
<axsl:template match="ind-def:filehash_state/ind-def:sha1" priority="1000" mode="M39">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sha1 entity of a filehash_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M39"/></axsl:template><axsl:template match="text()" priority="-1" mode="M39"/><axsl:template match="@*|node()" priority="-2" mode="M39"><axsl:apply-templates select="@*|*" mode="M39"/></axsl:template>

<!--PATTERN envtst-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_test/ind-def:object" priority="1001" mode="M40">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:environmentvariable_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an environmentvariable_test must reference a environmentvariable_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_test/ind-def:state" priority="1000" mode="M40">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:environmentvariable_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an environmentvariable_test must reference a environmentvariable_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template><axsl:template match="text()" priority="-1" mode="M40"/><axsl:template match="@*|node()" priority="-2" mode="M40"><axsl:apply-templates select="@*|*" mode="M40"/></axsl:template>

<!--PATTERN envobjname-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_object/ind-def:name" priority="1000" mode="M41">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an environmentvariable_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M41"/></axsl:template><axsl:template match="text()" priority="-1" mode="M41"/><axsl:template match="@*|node()" priority="-2" mode="M41"><axsl:apply-templates select="@*|*" mode="M41"/></axsl:template>

<!--PATTERN envstename-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_state/ind-def:name" priority="1000" mode="M42">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an environmentvariable_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M42"/></axsl:template><axsl:template match="text()" priority="-1" mode="M42"/><axsl:template match="@*|node()" priority="-2" mode="M42"><axsl:apply-templates select="@*|*" mode="M42"/></axsl:template>

<!--PATTERN envstevalue-->


	<!--RULE -->
<axsl:template match="ind-def:environmentvariable_state/ind-def:value" priority="1000" mode="M43">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M43"/></axsl:template><axsl:template match="text()" priority="-1" mode="M43"/><axsl:template match="@*|node()" priority="-2" mode="M43"><axsl:apply-templates select="@*|*" mode="M43"/></axsl:template>

<!--PATTERN sqltst-->


	<!--RULE -->
<axsl:template match="ind-def:sql_test/ind-def:object" priority="1001" mode="M44">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:sql_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sql_test must reference a sql_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:sql_test/ind-def:state" priority="1000" mode="M44">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:sql_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sql_test must reference a sql_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template><axsl:template match="text()" priority="-1" mode="M44"/><axsl:template match="@*|node()" priority="-2" mode="M44"><axsl:apply-templates select="@*|*" mode="M44"/></axsl:template>

<!--PATTERN sqlobjdengine-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:engine" priority="1000" mode="M45">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the engine entity of an sql_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M45"/></axsl:template><axsl:template match="text()" priority="-1" mode="M45"/><axsl:template match="@*|node()" priority="-2" mode="M45"><axsl:apply-templates select="@*|*" mode="M45"/></axsl:template>

<!--PATTERN sqlobjversion-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:version" priority="1000" mode="M46">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of an sql_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M46"/></axsl:template><axsl:template match="text()" priority="-1" mode="M46"/><axsl:template match="@*|node()" priority="-2" mode="M46"><axsl:apply-templates select="@*|*" mode="M46"/></axsl:template>

<!--PATTERN sqlobjconnection_string-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:connection_string" priority="1000" mode="M47">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the connection_string entity of an sql_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M47"/></axsl:template><axsl:template match="text()" priority="-1" mode="M47"/><axsl:template match="@*|node()" priority="-2" mode="M47"><axsl:apply-templates select="@*|*" mode="M47"/></axsl:template>

<!--PATTERN sqlobjsql-->


	<!--RULE -->
<axsl:template match="ind-def:sql_object/ind-def:sql" priority="1000" mode="M48">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sql entity of a sql_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M48"/></axsl:template><axsl:template match="text()" priority="-1" mode="M48"/><axsl:template match="@*|node()" priority="-2" mode="M48"><axsl:apply-templates select="@*|*" mode="M48"/></axsl:template>

<!--PATTERN sqlsteengine-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:engine" priority="1000" mode="M49">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the engine entity of an sql_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M49"/></axsl:template><axsl:template match="text()" priority="-1" mode="M49"/><axsl:template match="@*|node()" priority="-2" mode="M49"><axsl:apply-templates select="@*|*" mode="M49"/></axsl:template>

<!--PATTERN sqlsteversion-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:version" priority="1000" mode="M50">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of an sql_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template><axsl:template match="text()" priority="-1" mode="M50"/><axsl:template match="@*|node()" priority="-2" mode="M50"><axsl:apply-templates select="@*|*" mode="M50"/></axsl:template>

<!--PATTERN sqlsteconnection_string-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:connection_string" priority="1000" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the connection_string entity of an sql_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template><axsl:template match="text()" priority="-1" mode="M51"/><axsl:template match="@*|node()" priority="-2" mode="M51"><axsl:apply-templates select="@*|*" mode="M51"/></axsl:template>

<!--PATTERN sqlstesql-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:sql" priority="1000" mode="M52">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sql entity of a sql_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M52"/></axsl:template><axsl:template match="text()" priority="-1" mode="M52"/><axsl:template match="@*|node()" priority="-2" mode="M52"><axsl:apply-templates select="@*|*" mode="M52"/></axsl:template>

<!--PATTERN sqlsteresult-->


	<!--RULE -->
<axsl:template match="ind-def:sql_state/ind-def:result" priority="1000" mode="M53">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M53"/></axsl:template><axsl:template match="text()" priority="-1" mode="M53"/><axsl:template match="@*|node()" priority="-2" mode="M53"><axsl:apply-templates select="@*|*" mode="M53"/></axsl:template>

<!--PATTERN txt54tst-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_test/ind-def:object" priority="1001" mode="M54">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:textfilecontent54_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a textfilecontent54_test must reference a textfilecontent54_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_test/ind-def:state" priority="1000" mode="M54">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:textfilecontent54_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a textfilecontent54_test must reference a textfilecontent54_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template><axsl:template match="text()" priority="-1" mode="M54"/><axsl:template match="@*|node()" priority="-2" mode="M54"><axsl:apply-templates select="@*|*" mode="M54"/></axsl:template>

<!--PATTERN txt54objpath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:path" priority="1000" mode="M55">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a textfilecontent54_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M55"/></axsl:template><axsl:template match="text()" priority="-1" mode="M55"/><axsl:template match="@*|node()" priority="-2" mode="M55"><axsl:apply-templates select="@*|*" mode="M55"/></axsl:template>

<!--PATTERN txt54objfilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:filename" priority="1000" mode="M56">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a textfilecontent54_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M56"/></axsl:template><axsl:template match="text()" priority="-1" mode="M56"/><axsl:template match="@*|node()" priority="-2" mode="M56"><axsl:apply-templates select="@*|*" mode="M56"/></axsl:template>

<!--PATTERN txt54objpattern-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:pattern" priority="1000" mode="M57">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pattern entity of a textfilecontent54_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M57"/></axsl:template><axsl:template match="text()" priority="-1" mode="M57"/><axsl:template match="@*|node()" priority="-2" mode="M57"><axsl:apply-templates select="@*|*" mode="M57"/></axsl:template>

<!--PATTERN txt54objinstance-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_object/ind-def:instance" priority="1000" mode="M58">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the instance entity of a textfilecontent54_object should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M58"/></axsl:template><axsl:template match="text()" priority="-1" mode="M58"/><axsl:template match="@*|node()" priority="-2" mode="M58"><axsl:apply-templates select="@*|*" mode="M58"/></axsl:template>

<!--PATTERN txt54stepath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:path" priority="1000" mode="M59">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a textfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M59"/></axsl:template><axsl:template match="text()" priority="-1" mode="M59"/><axsl:template match="@*|node()" priority="-2" mode="M59"><axsl:apply-templates select="@*|*" mode="M59"/></axsl:template>

<!--PATTERN txt54stefilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:filename" priority="1000" mode="M60">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a textfilecontent54_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M60"/></axsl:template><axsl:template match="text()" priority="-1" mode="M60"/><axsl:template match="@*|node()" priority="-2" mode="M60"><axsl:apply-templates select="@*|*" mode="M60"/></axsl:template>

<!--PATTERN txt54stepattern-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:pattern" priority="1000" mode="M61">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pattern entity of a textfilecontent54_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M61"/></axsl:template><axsl:template match="text()" priority="-1" mode="M61"/><axsl:template match="@*|node()" priority="-2" mode="M61"><axsl:apply-templates select="@*|*" mode="M61"/></axsl:template>

<!--PATTERN txt54steinstance-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:instance" priority="1000" mode="M62">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the instance entity of a textfilecontent54_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M62"/></axsl:template><axsl:template match="text()" priority="-1" mode="M62"/><axsl:template match="@*|node()" priority="-2" mode="M62"><axsl:apply-templates select="@*|*" mode="M62"/></axsl:template>

<!--PATTERN txt54stesubexpression-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent54_state/ind-def:subexpression" priority="1000" mode="M63">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M63"/></axsl:template><axsl:template match="text()" priority="-1" mode="M63"/><axsl:template match="@*|node()" priority="-2" mode="M63"><axsl:apply-templates select="@*|*" mode="M63"/></axsl:template>

<!--PATTERN txttst-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_test/ind-def:object" priority="1001" mode="M64">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:textfilecontent_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a textfilecontent_test must reference a textfilecontent_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M64"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_test/ind-def:state" priority="1000" mode="M64">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:textfilecontent_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a textfilecontent_test must reference a textfilecontent_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M64"/></axsl:template><axsl:template match="text()" priority="-1" mode="M64"/><axsl:template match="@*|node()" priority="-2" mode="M64"><axsl:apply-templates select="@*|*" mode="M64"/></axsl:template>

<!--PATTERN txtobjpath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object/ind-def:path" priority="1000" mode="M65">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a textfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M65"/></axsl:template><axsl:template match="text()" priority="-1" mode="M65"/><axsl:template match="@*|node()" priority="-2" mode="M65"><axsl:apply-templates select="@*|*" mode="M65"/></axsl:template>

<!--PATTERN txtobjfilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object/ind-def:filename" priority="1000" mode="M66">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a textfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M66"/></axsl:template><axsl:template match="text()" priority="-1" mode="M66"/><axsl:template match="@*|node()" priority="-2" mode="M66"><axsl:apply-templates select="@*|*" mode="M66"/></axsl:template>

<!--PATTERN txtobjline-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_object/ind-def:line" priority="1000" mode="M67">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the line entity of a textfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M67"/></axsl:template><axsl:template match="text()" priority="-1" mode="M67"/><axsl:template match="@*|node()" priority="-2" mode="M67"><axsl:apply-templates select="@*|*" mode="M67"/></axsl:template>

<!--PATTERN txtstepath-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state/ind-def:path" priority="1000" mode="M68">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a textfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M68"/></axsl:template><axsl:template match="text()" priority="-1" mode="M68"/><axsl:template match="@*|node()" priority="-2" mode="M68"><axsl:apply-templates select="@*|*" mode="M68"/></axsl:template>

<!--PATTERN txtstefilename-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state/ind-def:filename" priority="1000" mode="M69">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a textfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M69"/></axsl:template><axsl:template match="text()" priority="-1" mode="M69"/><axsl:template match="@*|node()" priority="-2" mode="M69"><axsl:apply-templates select="@*|*" mode="M69"/></axsl:template>

<!--PATTERN txtsteline-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state/ind-def:line" priority="1000" mode="M70">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the line entity of a textfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M70"/></axsl:template><axsl:template match="text()" priority="-1" mode="M70"/><axsl:template match="@*|node()" priority="-2" mode="M70"><axsl:apply-templates select="@*|*" mode="M70"/></axsl:template>

<!--PATTERN txtstesubexpression-->


	<!--RULE -->
<axsl:template match="ind-def:textfilecontent_state/ind-def:subexpression" priority="1000" mode="M71">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M71"/></axsl:template><axsl:template match="text()" priority="-1" mode="M71"/><axsl:template match="@*|node()" priority="-2" mode="M71"><axsl:apply-templates select="@*|*" mode="M71"/></axsl:template>

<!--PATTERN vattst-->


	<!--RULE -->
<axsl:template match="ind-def:variable_test/ind-def:object" priority="1001" mode="M72">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:variable_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a variable_test must reference a variable_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M72"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:variable_test/ind-def:state" priority="1000" mode="M72">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:variable_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a variable_test must reference a variable_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M72"/></axsl:template><axsl:template match="text()" priority="-1" mode="M72"/><axsl:template match="@*|node()" priority="-2" mode="M72"><axsl:apply-templates select="@*|*" mode="M72"/></axsl:template>

<!--PATTERN varobjvar_ref-->


	<!--RULE -->
<axsl:template match="ind-def:variable_object/ind-def:var_ref" priority="1000" mode="M73">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the var_ref entity of a variable_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M73"/></axsl:template><axsl:template match="text()" priority="-1" mode="M73"/><axsl:template match="@*|node()" priority="-2" mode="M73"><axsl:apply-templates select="@*|*" mode="M73"/></axsl:template>

<!--PATTERN varstevar_ref-->


	<!--RULE -->
<axsl:template match="ind-def:variable_state/ind-def:var_ref" priority="1000" mode="M74">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the var_ref entity of a variable_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M74"/></axsl:template><axsl:template match="text()" priority="-1" mode="M74"/><axsl:template match="@*|node()" priority="-2" mode="M74"><axsl:apply-templates select="@*|*" mode="M74"/></axsl:template>

<!--PATTERN varstevalue-->


	<!--RULE -->
<axsl:template match="ind-def:variable_state/ind-def:value" priority="1000" mode="M75">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M75"/></axsl:template><axsl:template match="text()" priority="-1" mode="M75"/><axsl:template match="@*|node()" priority="-2" mode="M75"><axsl:apply-templates select="@*|*" mode="M75"/></axsl:template>

<!--PATTERN xmltst-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_test/ind-def:object" priority="1001" mode="M76">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:xmlfilecontent_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a xmlfilecontent_test must reference a xmlfilecontent_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M76"/></axsl:template>

	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_test/ind-def:state" priority="1000" mode="M76">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:xmlfilecontent_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a xmlfilecontent_test must reference a xmlfilecontent_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M76"/></axsl:template><axsl:template match="text()" priority="-1" mode="M76"/><axsl:template match="@*|node()" priority="-2" mode="M76"><axsl:apply-templates select="@*|*" mode="M76"/></axsl:template>

<!--PATTERN xmlobjpath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:path" priority="1000" mode="M77">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a xmlfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M77"/></axsl:template><axsl:template match="text()" priority="-1" mode="M77"/><axsl:template match="@*|node()" priority="-2" mode="M77"><axsl:apply-templates select="@*|*" mode="M77"/></axsl:template>

<!--PATTERN xmlobjfilename-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:filename" priority="1000" mode="M78">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a xmlfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M78"/></axsl:template><axsl:template match="text()" priority="-1" mode="M78"/><axsl:template match="@*|node()" priority="-2" mode="M78"><axsl:apply-templates select="@*|*" mode="M78"/></axsl:template>

<!--PATTERN xmlobjxpath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_object/ind-def:xpath" priority="1000" mode="M79">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the xpath entity of a xmlfilecontent_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M79"/></axsl:template><axsl:template match="text()" priority="-1" mode="M79"/><axsl:template match="@*|node()" priority="-2" mode="M79"><axsl:apply-templates select="@*|*" mode="M79"/></axsl:template>

<!--PATTERN xmlstepath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_state/ind-def:path" priority="1000" mode="M80">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a xmlfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M80"/></axsl:template><axsl:template match="text()" priority="-1" mode="M80"/><axsl:template match="@*|node()" priority="-2" mode="M80"><axsl:apply-templates select="@*|*" mode="M80"/></axsl:template>

<!--PATTERN xmlstefilename-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_state/ind-def:filename" priority="1000" mode="M81">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a xmlfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M81"/></axsl:template><axsl:template match="text()" priority="-1" mode="M81"/><axsl:template match="@*|node()" priority="-2" mode="M81"><axsl:apply-templates select="@*|*" mode="M81"/></axsl:template>

<!--PATTERN xmlstexpath-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_state/ind-def:xpath" priority="1000" mode="M82">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the xpath entity of a xmlfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M82"/></axsl:template><axsl:template match="text()" priority="-1" mode="M82"/><axsl:template match="@*|node()" priority="-2" mode="M82"><axsl:apply-templates select="@*|*" mode="M82"/></axsl:template>

<!--PATTERN xmlstevalue_of-->


	<!--RULE -->
<axsl:template match="ind-def:xmlfilecontent_state/ind-def:value_of" priority="1000" mode="M83">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the value_of entity of a xmlfilecontent_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M83"/></axsl:template><axsl:template match="text()" priority="-1" mode="M83"/><axsl:template match="@*|node()" priority="-2" mode="M83"><axsl:apply-templates select="@*|*" mode="M83"/></axsl:template>

<!--PATTERN filesetobjflstinst-->


	<!--RULE -->
<axsl:template match="aix-def:fileset_object/aix-def:flstinst" priority="1000" mode="M84">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the flstinst entity of a fileset_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M84"/></axsl:template><axsl:template match="text()" priority="-1" mode="M84"/><axsl:template match="@*|node()" priority="-2" mode="M84"><axsl:apply-templates select="@*|*" mode="M84"/></axsl:template>

<!--PATTERN filesetsteflstinst-->


	<!--RULE -->
<axsl:template match="aix-def:fileset_state/aix-def:flstinst" priority="1000" mode="M85">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the flstinst entity of a fileset_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M85"/></axsl:template><axsl:template match="text()" priority="-1" mode="M85"/><axsl:template match="@*|node()" priority="-2" mode="M85"><axsl:apply-templates select="@*|*" mode="M85"/></axsl:template>

<!--PATTERN filesetstelevel-->


	<!--RULE -->
<axsl:template match="aix-def:fileset_state/aix-def:level" priority="1000" mode="M86">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the level entity of a fileset_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M86"/></axsl:template><axsl:template match="text()" priority="-1" mode="M86"/><axsl:template match="@*|node()" priority="-2" mode="M86"><axsl:apply-templates select="@*|*" mode="M86"/></axsl:template>

<!--PATTERN filesetstestate-->


	<!--RULE -->
<axsl:template match="aix-def:fileset_state/aix-def:state" priority="1000" mode="M87">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the state entity of a fileset_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M87"/></axsl:template><axsl:template match="text()" priority="-1" mode="M87"/><axsl:template match="@*|node()" priority="-2" mode="M87"><axsl:apply-templates select="@*|*" mode="M87"/></axsl:template>

<!--PATTERN filesetstedescription-->


	<!--RULE -->
<axsl:template match="aix-def:fileset_state/aix-def:description" priority="1000" mode="M88">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the description entity of a fileset_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M88"/></axsl:template><axsl:template match="text()" priority="-1" mode="M88"/><axsl:template match="@*|node()" priority="-2" mode="M88"><axsl:apply-templates select="@*|*" mode="M88"/></axsl:template>

<!--PATTERN fixobjapar_number-->


	<!--RULE -->
<axsl:template match="aix-def:fix_object/aix-def:apar_number" priority="1000" mode="M89">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the apar_number entity of a fix_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M89"/></axsl:template><axsl:template match="text()" priority="-1" mode="M89"/><axsl:template match="@*|node()" priority="-2" mode="M89"><axsl:apply-templates select="@*|*" mode="M89"/></axsl:template>

<!--PATTERN fixsteapar_number-->


	<!--RULE -->
<axsl:template match="aix-def:fix_state/aix-def:apar_number" priority="1000" mode="M90">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the apar_number entity of a fix_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M90"/></axsl:template><axsl:template match="text()" priority="-1" mode="M90"/><axsl:template match="@*|node()" priority="-2" mode="M90"><axsl:apply-templates select="@*|*" mode="M90"/></axsl:template>

<!--PATTERN fixsteabstract-->


	<!--RULE -->
<axsl:template match="aix-def:fix_state/aix-def:abstract" priority="1000" mode="M91">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the abstract entity of a fix_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M91"/></axsl:template><axsl:template match="text()" priority="-1" mode="M91"/><axsl:template match="@*|node()" priority="-2" mode="M91"><axsl:apply-templates select="@*|*" mode="M91"/></axsl:template>

<!--PATTERN fixstesymptom-->


	<!--RULE -->
<axsl:template match="aix-def:fix_state/aix-def:symptom" priority="1000" mode="M92">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the symptom entity of a fix_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M92"/></axsl:template><axsl:template match="text()" priority="-1" mode="M92"/><axsl:template match="@*|node()" priority="-2" mode="M92"><axsl:apply-templates select="@*|*" mode="M92"/></axsl:template>

<!--PATTERN fixsteinstallation_status-->


	<!--RULE -->
<axsl:template match="aix-def:fix_state/aix-def:installation_status" priority="1000" mode="M93">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the installation_status entity of a fix_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M93"/></axsl:template><axsl:template match="text()" priority="-1" mode="M93"/><axsl:template match="@*|node()" priority="-2" mode="M93"><axsl:apply-templates select="@*|*" mode="M93"/></axsl:template>

<!--PATTERN oslevelstemaintenance_level-->


	<!--RULE -->
<axsl:template match="aix-def:oslevel_state/aix-def:maintenance_level" priority="1000" mode="M94">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the maintenance_level entity of an oslevel_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M94"/></axsl:template><axsl:template match="text()" priority="-1" mode="M94"/><axsl:template match="@*|node()" priority="-2" mode="M94"><axsl:apply-templates select="@*|*" mode="M94"/></axsl:template>

<!--PATTERN httpdtst-->


	<!--RULE -->
<axsl:template match="apache-def:httpd_test/apache-def:object" priority="1001" mode="M95">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/apache-def:httpd_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a httpd_test must reference a httpd_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M95"/></axsl:template>

	<!--RULE -->
<axsl:template match="apache-def:httpd_test/apache-def:state" priority="1000" mode="M95">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/apache-def:httpd_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a httpd_test must reference a httpd_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M95"/></axsl:template><axsl:template match="text()" priority="-1" mode="M95"/><axsl:template match="@*|node()" priority="-2" mode="M95"><axsl:apply-templates select="@*|*" mode="M95"/></axsl:template>

<!--PATTERN httpdstepath-->


	<!--RULE -->
<axsl:template match="apache-def:httpd_state/apache-def:path" priority="1000" mode="M96">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a httpd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M96"/></axsl:template><axsl:template match="text()" priority="-1" mode="M96"/><axsl:template match="@*|node()" priority="-2" mode="M96"><axsl:apply-templates select="@*|*" mode="M96"/></axsl:template>

<!--PATTERN httpdstebinaryname-->


	<!--RULE -->
<axsl:template match="apache-def:httpd_state/apache-def:binary_name" priority="1000" mode="M97">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the binary_name entity of a httpd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M97"/></axsl:template><axsl:template match="text()" priority="-1" mode="M97"/><axsl:template match="@*|node()" priority="-2" mode="M97"><axsl:apply-templates select="@*|*" mode="M97"/></axsl:template>

<!--PATTERN httpdsteversion-->


	<!--RULE -->
<axsl:template match="apache-def:httpd_state/apache-def:version" priority="1000" mode="M98">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a httpd_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M98"/></axsl:template><axsl:template match="text()" priority="-1" mode="M98"/><axsl:template match="@*|node()" priority="-2" mode="M98"><axsl:apply-templates select="@*|*" mode="M98"/></axsl:template>

<!--PATTERN apachevertst-->


	<!--RULE -->
<axsl:template match="apache-def:version_test/apache-def:object" priority="1001" mode="M99">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/apache-def:version_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a version_test must reference a version_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M99"/></axsl:template>

	<!--RULE -->
<axsl:template match="apache-def:version_test/apache-def:state" priority="1000" mode="M99">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/apache-def:version_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a version_test must reference a version_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M99"/></axsl:template><axsl:template match="text()" priority="-1" mode="M99"/><axsl:template match="@*|node()" priority="-2" mode="M99"><axsl:apply-templates select="@*|*" mode="M99"/></axsl:template>

<!--PATTERN apcheversteversion-->


	<!--RULE -->
<axsl:template match="apache-def:version_state/apache-def:version" priority="1000" mode="M100">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of an version_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M100"/></axsl:template><axsl:template match="text()" priority="-1" mode="M100"/><axsl:template match="@*|node()" priority="-2" mode="M100"><axsl:apply-templates select="@*|*" mode="M100"/></axsl:template>

<!--PATTERN catosmoduletst-->


	<!--RULE -->
<axsl:template match="catos-def:module_test/catos-def:object" priority="1001" mode="M101">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/catos-def:module_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a module_test must reference a module_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M101"/></axsl:template>

	<!--RULE -->
<axsl:template match="catos-def:module_test/catos-def:state" priority="1000" mode="M101">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/catos-def:module_number/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a module_test must reference a module_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M101"/></axsl:template><axsl:template match="text()" priority="-1" mode="M101"/><axsl:template match="@*|node()" priority="-2" mode="M101"><axsl:apply-templates select="@*|*" mode="M101"/></axsl:template>

<!--PATTERN moduleobjmodule_number-->


	<!--RULE -->
<axsl:template match="catos-def:module_object/catos-def:module_number" priority="1000" mode="M102">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the module_number entity of a module_object should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the module_number entity of a module_object should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', or 'less than or equal'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M102"/></axsl:template><axsl:template match="text()" priority="-1" mode="M102"/><axsl:template match="@*|node()" priority="-2" mode="M102"><axsl:apply-templates select="@*|*" mode="M102"/></axsl:template>

<!--PATTERN catosmodulestemodule_number-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:module_number" priority="1000" mode="M103">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the module_number entity of a module_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M103"/></axsl:template><axsl:template match="text()" priority="-1" mode="M103"/><axsl:template match="@*|node()" priority="-2" mode="M103"><axsl:apply-templates select="@*|*" mode="M103"/></axsl:template>

<!--PATTERN catosmodulestetype-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:type" priority="1000" mode="M104">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of a module_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M104"/></axsl:template><axsl:template match="text()" priority="-1" mode="M104"/><axsl:template match="@*|node()" priority="-2" mode="M104"><axsl:apply-templates select="@*|*" mode="M104"/></axsl:template>

<!--PATTERN catosmodulestemodel-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:model" priority="1000" mode="M105">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the model entity of a module_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M105"/></axsl:template><axsl:template match="text()" priority="-1" mode="M105"/><axsl:template match="@*|node()" priority="-2" mode="M105"><axsl:apply-templates select="@*|*" mode="M105"/></axsl:template>

<!--PATTERN catosmodulestesoftware_major_release-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:software_major_release" priority="1000" mode="M106">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the software_major_release entity of a module_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M106"/></axsl:template><axsl:template match="text()" priority="-1" mode="M106"/><axsl:template match="@*|node()" priority="-2" mode="M106"><axsl:apply-templates select="@*|*" mode="M106"/></axsl:template>

<!--PATTERN catosmodulestesoftware_individual_release-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:software_individual_release" priority="1000" mode="M107">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the software_individual_release entity of a module_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M107"/></axsl:template><axsl:template match="text()" priority="-1" mode="M107"/><axsl:template match="@*|node()" priority="-2" mode="M107"><axsl:apply-templates select="@*|*" mode="M107"/></axsl:template>

<!--PATTERN catosmodulestesoftware_version_id-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:software_version_id" priority="1000" mode="M108">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the software_version_id entity of a module_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M108"/></axsl:template><axsl:template match="text()" priority="-1" mode="M108"/><axsl:template match="@*|node()" priority="-2" mode="M108"><axsl:apply-templates select="@*|*" mode="M108"/></axsl:template>

<!--PATTERN catosmodulestehardware_major_release-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:hardware_major_release" priority="1000" mode="M109">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hardware_major_release entity of a module_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M109"/></axsl:template><axsl:template match="text()" priority="-1" mode="M109"/><axsl:template match="@*|node()" priority="-2" mode="M109"><axsl:apply-templates select="@*|*" mode="M109"/></axsl:template>

<!--PATTERN catosmodulestehardware_individual_release-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:hardware_individual_release" priority="1000" mode="M110">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hardware_individual_release entity of a module_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M110"/></axsl:template><axsl:template match="text()" priority="-1" mode="M110"/><axsl:template match="@*|node()" priority="-2" mode="M110"><axsl:apply-templates select="@*|*" mode="M110"/></axsl:template>

<!--PATTERN catosmodulestefirmware_major_release-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:firmware_major_release" priority="1000" mode="M111">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the firmware_major_release entity of a module_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M111"/></axsl:template><axsl:template match="text()" priority="-1" mode="M111"/><axsl:template match="@*|node()" priority="-2" mode="M111"><axsl:apply-templates select="@*|*" mode="M111"/></axsl:template>

<!--PATTERN catosmodulestefirmware_individual_release-->


	<!--RULE -->
<axsl:template match="catos-def:module_state/catos-def:firmware_individual_release" priority="1000" mode="M112">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the firmware_individual_release entity of a module_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M112"/></axsl:template><axsl:template match="text()" priority="-1" mode="M112"/><axsl:template match="@*|node()" priority="-2" mode="M112"><axsl:apply-templates select="@*|*" mode="M112"/></axsl:template>

<!--PATTERN catosverstedevice_series-->


	<!--RULE -->
<axsl:template match="catos-def:version_state/catos-def:swtich_series" priority="1000" mode="M113">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the swtich_series entity of a version_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M113"/></axsl:template><axsl:template match="text()" priority="-1" mode="M113"/><axsl:template match="@*|node()" priority="-2" mode="M113"><axsl:apply-templates select="@*|*" mode="M113"/></axsl:template>

<!--PATTERN catosversteimage_name-->


	<!--RULE -->
<axsl:template match="catos-def:version_state/catos-def:image_name" priority="1000" mode="M114">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the image_name entity of a version_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M114"/></axsl:template><axsl:template match="text()" priority="-1" mode="M114"/><axsl:template match="@*|node()" priority="-2" mode="M114"><axsl:apply-templates select="@*|*" mode="M114"/></axsl:template>

<!--PATTERN catosverstecatos_major_release-->


	<!--RULE -->
<axsl:template match="catos-def:version_state/catos-def:catos_major_release" priority="1000" mode="M115">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the catos_major_release entity of a version_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M115"/></axsl:template><axsl:template match="text()" priority="-1" mode="M115"/><axsl:template match="@*|node()" priority="-2" mode="M115"><axsl:apply-templates select="@*|*" mode="M115"/></axsl:template>

<!--PATTERN catosverstecatos_individual_release-->


	<!--RULE -->
<axsl:template match="catos-def:version_state/catos-def:catos_individual_release" priority="1000" mode="M116">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the catos_individual_release entity of a version_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M116"/></axsl:template><axsl:template match="text()" priority="-1" mode="M116"/><axsl:template match="@*|node()" priority="-2" mode="M116"><axsl:apply-templates select="@*|*" mode="M116"/></axsl:template>

<!--PATTERN catosverstecatos_version_id-->


	<!--RULE -->
<axsl:template match="catos-def:version_state/catos-def:catos_version_id" priority="1000" mode="M117">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the catos_version_id entity of a version_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M117"/></axsl:template><axsl:template match="text()" priority="-1" mode="M117"/><axsl:template match="@*|node()" priority="-2" mode="M117"><axsl:apply-templates select="@*|*" mode="M117"/></axsl:template>

<!--PATTERN esxpatchobjpatch_number-->


	<!--RULE -->
<axsl:template match="esx-def:patch_object/esx-def:patch_number" priority="1000" mode="M118">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the patch_number entity of a patch_object should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M118"/></axsl:template><axsl:template match="text()" priority="-1" mode="M118"/><axsl:template match="@*|node()" priority="-2" mode="M118"><axsl:apply-templates select="@*|*" mode="M118"/></axsl:template>

<!--PATTERN esxpatchstepatch_number-->


	<!--RULE -->
<axsl:template match="esx-def:patch_state/esx-def:patch_number" priority="1000" mode="M119">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the patch_number entity of a patch_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M119"/></axsl:template><axsl:template match="text()" priority="-1" mode="M119"/><axsl:template match="@*|node()" priority="-2" mode="M119"><axsl:apply-templates select="@*|*" mode="M119"/></axsl:template>

<!--PATTERN esxpatchstestatus-->


	<!--RULE -->
<axsl:template match="esx-def:patch_state/esx-def:status" priority="1000" mode="M120">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the status entity of a patch_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M120"/></axsl:template><axsl:template match="text()" priority="-1" mode="M120"/><axsl:template match="@*|node()" priority="-2" mode="M120"><axsl:apply-templates select="@*|*" mode="M120"/></axsl:template>

<!--PATTERN versionsterelease-->


	<!--RULE -->
<axsl:template match="esx-def:version_state/esx-def:release" priority="1000" mode="M121">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the release entity of an version_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M121"/></axsl:template><axsl:template match="text()" priority="-1" mode="M121"/><axsl:template match="@*|node()" priority="-2" mode="M121"><axsl:apply-templates select="@*|*" mode="M121"/></axsl:template>

<!--PATTERN versionstebuild-->


	<!--RULE -->
<axsl:template match="esx-def:version_state/esx-def:build" priority="1000" mode="M122">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the build entity of an version_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M122"/></axsl:template><axsl:template match="text()" priority="-1" mode="M122"/><axsl:template match="@*|node()" priority="-2" mode="M122"><axsl:apply-templates select="@*|*" mode="M122"/></axsl:template>

<!--PATTERN portinfotst-->


	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_test/freebsd-def:object" priority="1001" mode="M123">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/freebsd-def:portinfo_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a portinfo_test must reference an portinfo_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M123"/></axsl:template>

	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_test/freebsd-def:state" priority="1000" mode="M123">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/freebsd-def:portinfo_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a portinfo_test must reference an portinfo_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M123"/></axsl:template><axsl:template match="text()" priority="-1" mode="M123"/><axsl:template match="@*|node()" priority="-2" mode="M123"><axsl:apply-templates select="@*|*" mode="M123"/></axsl:template>

<!--PATTERN portinfoobjpkginst-->


	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_object/freebsd-def:pkginst" priority="1000" mode="M124">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pkginst entity of a portinfo_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M124"/></axsl:template><axsl:template match="text()" priority="-1" mode="M124"/><axsl:template match="@*|node()" priority="-2" mode="M124"><axsl:apply-templates select="@*|*" mode="M124"/></axsl:template>

<!--PATTERN portinfostepkginst-->


	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_state/freebsd-def:pkginst" priority="1000" mode="M125">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pkginst entity of a portinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M125"/></axsl:template><axsl:template match="text()" priority="-1" mode="M125"/><axsl:template match="@*|node()" priority="-2" mode="M125"><axsl:apply-templates select="@*|*" mode="M125"/></axsl:template>

<!--PATTERN portinfostename-->


	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_state/freebsd-def:name" priority="1000" mode="M126">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a portinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M126"/></axsl:template><axsl:template match="text()" priority="-1" mode="M126"/><axsl:template match="@*|node()" priority="-2" mode="M126"><axsl:apply-templates select="@*|*" mode="M126"/></axsl:template>

<!--PATTERN portinfostecategory-->


	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_state/freebsd-def:category" priority="1000" mode="M127">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the category entity of a portinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M127"/></axsl:template><axsl:template match="text()" priority="-1" mode="M127"/><axsl:template match="@*|node()" priority="-2" mode="M127"><axsl:apply-templates select="@*|*" mode="M127"/></axsl:template>

<!--PATTERN portinfosteversion-->


	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_state/freebsd-def:version" priority="1000" mode="M128">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a portinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M128"/></axsl:template><axsl:template match="text()" priority="-1" mode="M128"/><axsl:template match="@*|node()" priority="-2" mode="M128"><axsl:apply-templates select="@*|*" mode="M128"/></axsl:template>

<!--PATTERN portinfostevendor-->


	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_state/freebsd-def:vendor" priority="1000" mode="M129">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the vendor entity of a portinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M129"/></axsl:template><axsl:template match="text()" priority="-1" mode="M129"/><axsl:template match="@*|node()" priority="-2" mode="M129"><axsl:apply-templates select="@*|*" mode="M129"/></axsl:template>

<!--PATTERN portinfostedescription-->


	<!--RULE -->
<axsl:template match="freebsd-def:portinfo_state/freebsd-def:description" priority="1000" mode="M130">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the description entity of a portinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M130"/></axsl:template><axsl:template match="text()" priority="-1" mode="M130"/><axsl:template match="@*|node()" priority="-2" mode="M130"><axsl:apply-templates select="@*|*" mode="M130"/></axsl:template>

<!--PATTERN gcobjparameter_name-->


	<!--RULE -->
<axsl:template match="hpux-def:getconf_object/hpux-def:parameter_name" priority="1000" mode="M131">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the parameter_name entity of a getconf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M131"/></axsl:template><axsl:template match="text()" priority="-1" mode="M131"/><axsl:template match="@*|node()" priority="-2" mode="M131"><axsl:apply-templates select="@*|*" mode="M131"/></axsl:template>

<!--PATTERN gcobjpathname-->


	<!--RULE -->
<axsl:template match="hpux-def:getconf_object/hpux-def:pathname" priority="1000" mode="M132">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pathname entity of a getconf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M132"/></axsl:template><axsl:template match="text()" priority="-1" mode="M132"/><axsl:template match="@*|node()" priority="-2" mode="M132"><axsl:apply-templates select="@*|*" mode="M132"/></axsl:template>

<!--PATTERN gcsteparameter_name-->


	<!--RULE -->
<axsl:template match="hpux-def:getconf_state/hpux-def:parameter_name" priority="1000" mode="M133">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the parameter_name entity of a getconf_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M133"/></axsl:template><axsl:template match="text()" priority="-1" mode="M133"/><axsl:template match="@*|node()" priority="-2" mode="M133"><axsl:apply-templates select="@*|*" mode="M133"/></axsl:template>

<!--PATTERN gcstepathname-->


	<!--RULE -->
<axsl:template match="hpux-def:getconf_state/hpux-def:pathname" priority="1000" mode="M134">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pathname entity of a getconf_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M134"/></axsl:template><axsl:template match="text()" priority="-1" mode="M134"/><axsl:template match="@*|node()" priority="-2" mode="M134"><axsl:apply-templates select="@*|*" mode="M134"/></axsl:template>

<!--PATTERN gcsteoutput-->


	<!--RULE -->
<axsl:template match="hpux-def:getconf_state/hpux-def:output" priority="1000" mode="M135">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the output entity of a getconf_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M135"/></axsl:template><axsl:template match="text()" priority="-1" mode="M135"/><axsl:template match="@*|node()" priority="-2" mode="M135"><axsl:apply-templates select="@*|*" mode="M135"/></axsl:template>

<!--PATTERN patch53objswtype-->


	<!--RULE -->
<axsl:template match="hpux-def:patch53_object/hpux-def:swtype" priority="1000" mode="M136">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the swtype entity of a patch53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M136"/></axsl:template><axsl:template match="text()" priority="-1" mode="M136"/><axsl:template match="@*|node()" priority="-2" mode="M136"><axsl:apply-templates select="@*|*" mode="M136"/></axsl:template>

<!--PATTERN patch53objarea_patched-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_object/hpux-def:area_patched" priority="1000" mode="M137">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the area_patched entity of a patch53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M137"/></axsl:template><axsl:template match="text()" priority="-1" mode="M137"/><axsl:template match="@*|node()" priority="-2" mode="M137"><axsl:apply-templates select="@*|*" mode="M137"/></axsl:template>

<!--PATTERN patch53objpatch_base-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_object/hpux-def:patch_base" priority="1000" mode="M138">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the patch_base entity of a patch53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M138"/></axsl:template><axsl:template match="text()" priority="-1" mode="M138"/><axsl:template match="@*|node()" priority="-2" mode="M138"><axsl:apply-templates select="@*|*" mode="M138"/></axsl:template>

<!--PATTERN patch53steswtype-->


	<!--RULE -->
<axsl:template match="hpux-def:patch53_state/hpux-def:swtype" priority="1000" mode="M139">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the swtype entity of a patch53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M139"/></axsl:template><axsl:template match="text()" priority="-1" mode="M139"/><axsl:template match="@*|node()" priority="-2" mode="M139"><axsl:apply-templates select="@*|*" mode="M139"/></axsl:template>

<!--PATTERN patch53stearea_patched-->


	<!--RULE -->
<axsl:template match="hpux-def:patch53_state/hpux-def:area_patched" priority="1000" mode="M140">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the area_patched entity of a patch53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M140"/></axsl:template><axsl:template match="text()" priority="-1" mode="M140"/><axsl:template match="@*|node()" priority="-2" mode="M140"><axsl:apply-templates select="@*|*" mode="M140"/></axsl:template>

<!--PATTERN patch53stepatch_base-->


	<!--RULE -->
<axsl:template match="hpux-def:patch53_state/hpux-def:patch_base" priority="1000" mode="M141">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the patch_base entity of a patch53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M141"/></axsl:template><axsl:template match="text()" priority="-1" mode="M141"/><axsl:template match="@*|node()" priority="-2" mode="M141"><axsl:apply-templates select="@*|*" mode="M141"/></axsl:template>

<!--PATTERN patchobjpatch_name-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_object/hpux-def:patch_name" priority="1000" mode="M142">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the patch_name entity of a patch_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M142"/></axsl:template><axsl:template match="text()" priority="-1" mode="M142"/><axsl:template match="@*|node()" priority="-2" mode="M142"><axsl:apply-templates select="@*|*" mode="M142"/></axsl:template>

<!--PATTERN patchstepatch_name-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_state/hpux-def:patch_name" priority="1000" mode="M143">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the patch_name entity of a patch_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M143"/></axsl:template><axsl:template match="text()" priority="-1" mode="M143"/><axsl:template match="@*|node()" priority="-2" mode="M143"><axsl:apply-templates select="@*|*" mode="M143"/></axsl:template>

<!--PATTERN patchsteswtype-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_state/hpux-def:swtype" priority="1000" mode="M144">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the swtype entity of a patch_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M144"/></axsl:template><axsl:template match="text()" priority="-1" mode="M144"/><axsl:template match="@*|node()" priority="-2" mode="M144"><axsl:apply-templates select="@*|*" mode="M144"/></axsl:template>

<!--PATTERN patchstearea_patched-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_state/hpux-def:area_patched" priority="1000" mode="M145">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the area_patched entity of a patch_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M145"/></axsl:template><axsl:template match="text()" priority="-1" mode="M145"/><axsl:template match="@*|node()" priority="-2" mode="M145"><axsl:apply-templates select="@*|*" mode="M145"/></axsl:template>

<!--PATTERN patchstepatch_base-->


	<!--RULE -->
<axsl:template match="hpux-def:patch_state/hpux-def:patch_base" priority="1000" mode="M146">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the patch_base entity of a patch_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M146"/></axsl:template><axsl:template match="text()" priority="-1" mode="M146"/><axsl:template match="@*|node()" priority="-2" mode="M146"><axsl:apply-templates select="@*|*" mode="M146"/></axsl:template>

<!--PATTERN swlistobjswlist-->


	<!--RULE -->
<axsl:template match="hpux-def:swlist_object/hpux-def:swlist" priority="1000" mode="M147">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the swlist entity of a swlist_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M147"/></axsl:template><axsl:template match="text()" priority="-1" mode="M147"/><axsl:template match="@*|node()" priority="-2" mode="M147"><axsl:apply-templates select="@*|*" mode="M147"/></axsl:template>

<!--PATTERN swliststeswlist-->


	<!--RULE -->
<axsl:template match="hpux-def:swlist_state/hpux-def:swlist" priority="1000" mode="M148">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the swlist entity of a swlist_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M148"/></axsl:template><axsl:template match="text()" priority="-1" mode="M148"/><axsl:template match="@*|node()" priority="-2" mode="M148"><axsl:apply-templates select="@*|*" mode="M148"/></axsl:template>

<!--PATTERN swliststebundle-->


	<!--RULE -->
<axsl:template match="hpux-def:swlist_state/hpux-def:bundle" priority="1000" mode="M149">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the bundle entity of a swlist_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M149"/></axsl:template><axsl:template match="text()" priority="-1" mode="M149"/><axsl:template match="@*|node()" priority="-2" mode="M149"><axsl:apply-templates select="@*|*" mode="M149"/></axsl:template>

<!--PATTERN swliststefileset-->


	<!--RULE -->
<axsl:template match="hpux-def:swlist_state/hpux-def:fileset" priority="1000" mode="M150">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the fileset entity of a swlist_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M150"/></axsl:template><axsl:template match="text()" priority="-1" mode="M150"/><axsl:template match="@*|node()" priority="-2" mode="M150"><axsl:apply-templates select="@*|*" mode="M150"/></axsl:template>

<!--PATTERN swliststeversion-->


	<!--RULE -->
<axsl:template match="hpux-def:swlist_state/hpux-def:version" priority="1000" mode="M151">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='fileset_revision'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a swlist_state should be 'fileset_revision'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M151"/></axsl:template><axsl:template match="text()" priority="-1" mode="M151"/><axsl:template match="@*|node()" priority="-2" mode="M151"><axsl:apply-templates select="@*|*" mode="M151"/></axsl:template>

<!--PATTERN swliststetitle-->


	<!--RULE -->
<axsl:template match="hpux-def:swlist_state/hpux-def:title" priority="1000" mode="M152">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the title entity of a swlist_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M152"/></axsl:template><axsl:template match="text()" priority="-1" mode="M152"/><axsl:template match="@*|node()" priority="-2" mode="M152"><axsl:apply-templates select="@*|*" mode="M152"/></axsl:template>

<!--PATTERN swliststevendor-->


	<!--RULE -->
<axsl:template match="hpux-def:swlist_state/hpux-def:vendor" priority="1000" mode="M153">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the vendor entity of a swlist_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M153"/></axsl:template><axsl:template match="text()" priority="-1" mode="M153"/><axsl:template match="@*|node()" priority="-2" mode="M153"><axsl:apply-templates select="@*|*" mode="M153"/></axsl:template>

<!--PATTERN trustobjusername-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_object/hpux-def:username" priority="1000" mode="M154">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a trusted_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M154"/></axsl:template><axsl:template match="text()" priority="-1" mode="M154"/><axsl:template match="@*|node()" priority="-2" mode="M154"><axsl:apply-templates select="@*|*" mode="M154"/></axsl:template>

<!--PATTERN truststeusername-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:username" priority="1000" mode="M155">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M155"/></axsl:template><axsl:template match="text()" priority="-1" mode="M155"/><axsl:template match="@*|node()" priority="-2" mode="M155"><axsl:apply-templates select="@*|*" mode="M155"/></axsl:template>

<!--PATTERN truststeuid-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:uid" priority="1000" mode="M156">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uid entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M156"/></axsl:template><axsl:template match="text()" priority="-1" mode="M156"/><axsl:template match="@*|node()" priority="-2" mode="M156"><axsl:apply-templates select="@*|*" mode="M156"/></axsl:template>

<!--PATTERN truststepassword-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:password" priority="1000" mode="M157">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M157"/></axsl:template><axsl:template match="text()" priority="-1" mode="M157"/><axsl:template match="@*|node()" priority="-2" mode="M157"><axsl:apply-templates select="@*|*" mode="M157"/></axsl:template>

<!--PATTERN truststeaccount_owner-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:account_owner" priority="1000" mode="M158">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the account_owner entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M158"/></axsl:template><axsl:template match="text()" priority="-1" mode="M158"/><axsl:template match="@*|node()" priority="-2" mode="M158"><axsl:apply-templates select="@*|*" mode="M158"/></axsl:template>

<!--PATTERN truststeboot_auth-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:boot_auth" priority="1000" mode="M159">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the boot_auth entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M159"/></axsl:template><axsl:template match="text()" priority="-1" mode="M159"/><axsl:template match="@*|node()" priority="-2" mode="M159"><axsl:apply-templates select="@*|*" mode="M159"/></axsl:template>

<!--PATTERN truststeaudit_id-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:audit_id" priority="1000" mode="M160">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the audit_id entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M160"/></axsl:template><axsl:template match="text()" priority="-1" mode="M160"/><axsl:template match="@*|node()" priority="-2" mode="M160"><axsl:apply-templates select="@*|*" mode="M160"/></axsl:template>

<!--PATTERN truststeaudit_flag-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:audit_flag" priority="1000" mode="M161">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the audit_flag entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M161"/></axsl:template><axsl:template match="text()" priority="-1" mode="M161"/><axsl:template match="@*|node()" priority="-2" mode="M161"><axsl:apply-templates select="@*|*" mode="M161"/></axsl:template>

<!--PATTERN truststepw_change_min-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_change_min" priority="1000" mode="M162">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_change_min entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M162"/></axsl:template><axsl:template match="text()" priority="-1" mode="M162"/><axsl:template match="@*|node()" priority="-2" mode="M162"><axsl:apply-templates select="@*|*" mode="M162"/></axsl:template>

<!--PATTERN truststepw_max_size-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_max_size" priority="1000" mode="M163">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_max_size entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M163"/></axsl:template><axsl:template match="text()" priority="-1" mode="M163"/><axsl:template match="@*|node()" priority="-2" mode="M163"><axsl:apply-templates select="@*|*" mode="M163"/></axsl:template>

<!--PATTERN truststepw_expiration-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_expiration" priority="1000" mode="M164">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_expiration entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M164"/></axsl:template><axsl:template match="text()" priority="-1" mode="M164"/><axsl:template match="@*|node()" priority="-2" mode="M164"><axsl:apply-templates select="@*|*" mode="M164"/></axsl:template>

<!--PATTERN truststepw_life-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_life" priority="1000" mode="M165">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_life entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M165"/></axsl:template><axsl:template match="text()" priority="-1" mode="M165"/><axsl:template match="@*|node()" priority="-2" mode="M165"><axsl:apply-templates select="@*|*" mode="M165"/></axsl:template>

<!--PATTERN truststepw_change_s-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_change_s" priority="1000" mode="M166">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_change_s entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M166"/></axsl:template><axsl:template match="text()" priority="-1" mode="M166"/><axsl:template match="@*|node()" priority="-2" mode="M166"><axsl:apply-templates select="@*|*" mode="M166"/></axsl:template>

<!--PATTERN truststepw_change_u-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_change_u" priority="1000" mode="M167">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_change_u entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M167"/></axsl:template><axsl:template match="text()" priority="-1" mode="M167"/><axsl:template match="@*|node()" priority="-2" mode="M167"><axsl:apply-templates select="@*|*" mode="M167"/></axsl:template>

<!--PATTERN truststeacct_expire-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:acct_expire" priority="1000" mode="M168">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the acct_expire entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M168"/></axsl:template><axsl:template match="text()" priority="-1" mode="M168"/><axsl:template match="@*|node()" priority="-2" mode="M168"><axsl:apply-templates select="@*|*" mode="M168"/></axsl:template>

<!--PATTERN truststemax_llogin-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:max_llogin" priority="1000" mode="M169">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the max_llogin entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M169"/></axsl:template><axsl:template match="text()" priority="-1" mode="M169"/><axsl:template match="@*|node()" priority="-2" mode="M169"><axsl:apply-templates select="@*|*" mode="M169"/></axsl:template>

<!--PATTERN truststeexp_warning-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:exp_warning" priority="1000" mode="M170">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exp_warning entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M170"/></axsl:template><axsl:template match="text()" priority="-1" mode="M170"/><axsl:template match="@*|node()" priority="-2" mode="M170"><axsl:apply-templates select="@*|*" mode="M170"/></axsl:template>

<!--PATTERN truststeusr_chg_pw-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:usr_chg_pw" priority="1000" mode="M171">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the usr_chg_pw entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M171"/></axsl:template><axsl:template match="text()" priority="-1" mode="M171"/><axsl:template match="@*|node()" priority="-2" mode="M171"><axsl:apply-templates select="@*|*" mode="M171"/></axsl:template>

<!--PATTERN truststegen_pw-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:gen_pw" priority="1000" mode="M172">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gen_pw entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M172"/></axsl:template><axsl:template match="text()" priority="-1" mode="M172"/><axsl:template match="@*|node()" priority="-2" mode="M172"><axsl:apply-templates select="@*|*" mode="M172"/></axsl:template>

<!--PATTERN truststepw_restrict-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_restrict" priority="1000" mode="M173">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_restrict entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M173"/></axsl:template><axsl:template match="text()" priority="-1" mode="M173"/><axsl:template match="@*|node()" priority="-2" mode="M173"><axsl:apply-templates select="@*|*" mode="M173"/></axsl:template>

<!--PATTERN truststepw_null-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_null" priority="1000" mode="M174">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_null entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M174"/></axsl:template><axsl:template match="text()" priority="-1" mode="M174"/><axsl:template match="@*|node()" priority="-2" mode="M174"><axsl:apply-templates select="@*|*" mode="M174"/></axsl:template>

<!--PATTERN truststepw_gen_char-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_gen_char" priority="1000" mode="M175">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_gen_char entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M175"/></axsl:template><axsl:template match="text()" priority="-1" mode="M175"/><axsl:template match="@*|node()" priority="-2" mode="M175"><axsl:apply-templates select="@*|*" mode="M175"/></axsl:template>

<!--PATTERN truststepw_gen_let-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_gen_let" priority="1000" mode="M176">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_gen_let entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M176"/></axsl:template><axsl:template match="text()" priority="-1" mode="M176"/><axsl:template match="@*|node()" priority="-2" mode="M176"><axsl:apply-templates select="@*|*" mode="M176"/></axsl:template>

<!--PATTERN truststelogin_time-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:login_time" priority="1000" mode="M177">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_time entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M177"/></axsl:template><axsl:template match="text()" priority="-1" mode="M177"/><axsl:template match="@*|node()" priority="-2" mode="M177"><axsl:apply-templates select="@*|*" mode="M177"/></axsl:template>

<!--PATTERN truststepw_changer-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:pw_changer" priority="1000" mode="M178">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_changer entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M178"/></axsl:template><axsl:template match="text()" priority="-1" mode="M178"/><axsl:template match="@*|node()" priority="-2" mode="M178"><axsl:apply-templates select="@*|*" mode="M178"/></axsl:template>

<!--PATTERN truststelogin_time_s-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:login_time_s" priority="1000" mode="M179">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_time_s entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M179"/></axsl:template><axsl:template match="text()" priority="-1" mode="M179"/><axsl:template match="@*|node()" priority="-2" mode="M179"><axsl:apply-templates select="@*|*" mode="M179"/></axsl:template>

<!--PATTERN truststelogin_time_u-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:login_time_u" priority="1000" mode="M180">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_time_u entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M180"/></axsl:template><axsl:template match="text()" priority="-1" mode="M180"/><axsl:template match="@*|node()" priority="-2" mode="M180"><axsl:apply-templates select="@*|*" mode="M180"/></axsl:template>

<!--PATTERN truststelogin_tty_s-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:login_tty_s" priority="1000" mode="M181">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_tty_s entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M181"/></axsl:template><axsl:template match="text()" priority="-1" mode="M181"/><axsl:template match="@*|node()" priority="-2" mode="M181"><axsl:apply-templates select="@*|*" mode="M181"/></axsl:template>

<!--PATTERN truststelogin_tty_u-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:login_tty_u" priority="1000" mode="M182">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_tty_u entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M182"/></axsl:template><axsl:template match="text()" priority="-1" mode="M182"/><axsl:template match="@*|node()" priority="-2" mode="M182"><axsl:apply-templates select="@*|*" mode="M182"/></axsl:template>

<!--PATTERN truststenum_u_logins-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:num_u_logins" priority="1000" mode="M183">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the num_u_logins entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M183"/></axsl:template><axsl:template match="text()" priority="-1" mode="M183"/><axsl:template match="@*|node()" priority="-2" mode="M183"><axsl:apply-templates select="@*|*" mode="M183"/></axsl:template>

<!--PATTERN truststemax_u_logins-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:max_u_logins" priority="1000" mode="M184">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the max_u_logins entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M184"/></axsl:template><axsl:template match="text()" priority="-1" mode="M184"/><axsl:template match="@*|node()" priority="-2" mode="M184"><axsl:apply-templates select="@*|*" mode="M184"/></axsl:template>

<!--PATTERN truststelock_flag-->


	<!--RULE -->
<axsl:template match="hpux-def:trusted_state/hpux-def:lock_flag" priority="1000" mode="M185">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the lock_flag entity of a trusted_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M185"/></axsl:template><axsl:template match="text()" priority="-1" mode="M185"/><axsl:template match="@*|node()" priority="-2" mode="M185"><axsl:apply-templates select="@*|*" mode="M185"/></axsl:template>

<!--PATTERN globaltst-->


	<!--RULE -->
<axsl:template match="ios-def:global_test/ios-def:object" priority="1001" mode="M186">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:global_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a global_test must reference a global_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M186"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:global_test/ios-def:state" priority="1000" mode="M186">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:global_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a global_test must reference a global_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M186"/></axsl:template><axsl:template match="text()" priority="-1" mode="M186"/><axsl:template match="@*|node()" priority="-2" mode="M186"><axsl:apply-templates select="@*|*" mode="M186"/></axsl:template>

<!--PATTERN globalobjglobal_command-->


	<!--RULE -->
<axsl:template match="ios-def:global_object/ios-def:global_command" priority="1000" mode="M187">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the global_command entity of a global_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M187"/></axsl:template><axsl:template match="text()" priority="-1" mode="M187"/><axsl:template match="@*|node()" priority="-2" mode="M187"><axsl:apply-templates select="@*|*" mode="M187"/></axsl:template>

<!--PATTERN globalsteglobal_command-->


	<!--RULE -->
<axsl:template match="ios-def:global_state/ios-def:global_command" priority="1000" mode="M188">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the global_command entity of a global_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M188"/></axsl:template><axsl:template match="text()" priority="-1" mode="M188"/><axsl:template match="@*|node()" priority="-2" mode="M188"><axsl:apply-templates select="@*|*" mode="M188"/></axsl:template>

<!--PATTERN iosinterfacetst-->


	<!--RULE -->
<axsl:template match="ios-def:interface_test/ios-def:object" priority="1001" mode="M189">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:interface_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an interface_test must reference an interface_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M189"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:interface_test/ios-def:state" priority="1000" mode="M189">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:interface_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an interface_test must reference an interface_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M189"/></axsl:template><axsl:template match="text()" priority="-1" mode="M189"/><axsl:template match="@*|node()" priority="-2" mode="M189"><axsl:apply-templates select="@*|*" mode="M189"/></axsl:template>

<!--PATTERN iosinterfaceobjname-->


	<!--RULE -->
<axsl:template match="ios-def:interface_object/ios-def:name" priority="1000" mode="M190">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an interface_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M190"/></axsl:template><axsl:template match="text()" priority="-1" mode="M190"/><axsl:template match="@*|node()" priority="-2" mode="M190"><axsl:apply-templates select="@*|*" mode="M190"/></axsl:template>

<!--PATTERN iosinterfacestename-->


	<!--RULE -->
<axsl:template match="ios-def:interface_state/ios-def:name" priority="1000" mode="M191">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M191"/></axsl:template><axsl:template match="text()" priority="-1" mode="M191"/><axsl:template match="@*|node()" priority="-2" mode="M191"><axsl:apply-templates select="@*|*" mode="M191"/></axsl:template>

<!--PATTERN iosinterfacesteip_directed_broadcast_command-->


	<!--RULE -->
<axsl:template match="ios-def:interface_state/ios-def:ip_directed_broadcast_command" priority="1000" mode="M192">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ip_directed_broadcast_command entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M192"/></axsl:template><axsl:template match="text()" priority="-1" mode="M192"/><axsl:template match="@*|node()" priority="-2" mode="M192"><axsl:apply-templates select="@*|*" mode="M192"/></axsl:template>

<!--PATTERN iosinterfacesteno_ip_directed_broadcast_command-->


	<!--RULE -->
<axsl:template match="ios-def:interface_state/ios-def:no_ip_directed_broadcast_command" priority="1000" mode="M193">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the no_ip_directed_broadcast_command entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M193"/></axsl:template><axsl:template match="text()" priority="-1" mode="M193"/><axsl:template match="@*|node()" priority="-2" mode="M193"><axsl:apply-templates select="@*|*" mode="M193"/></axsl:template>

<!--PATTERN iosinterfacesteproxy_arp_command-->


	<!--RULE -->
<axsl:template match="ios-def:interface_state/ios-def:proxy_arp_command" priority="1000" mode="M194">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the proxy_arp_command entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M194"/></axsl:template><axsl:template match="text()" priority="-1" mode="M194"/><axsl:template match="@*|node()" priority="-2" mode="M194"><axsl:apply-templates select="@*|*" mode="M194"/></axsl:template>

<!--PATTERN iosinterfacesteshutdown_command-->


	<!--RULE -->
<axsl:template match="ios-def:interface_state/ios-def:shutdown_command" priority="1000" mode="M195">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the shutdown_command entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M195"/></axsl:template><axsl:template match="text()" priority="-1" mode="M195"/><axsl:template match="@*|node()" priority="-2" mode="M195"><axsl:apply-templates select="@*|*" mode="M195"/></axsl:template>

<!--PATTERN linetst-->


	<!--RULE -->
<axsl:template match="ios-def:line_test/ios-def:object" priority="1001" mode="M196">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:line_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a line_test must reference a line_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M196"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:line_test/ios-def:state" priority="1000" mode="M196">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:line_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a line_test must reference a line_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M196"/></axsl:template><axsl:template match="text()" priority="-1" mode="M196"/><axsl:template match="@*|node()" priority="-2" mode="M196"><axsl:apply-templates select="@*|*" mode="M196"/></axsl:template>

<!--PATTERN lineobjshow_subcommand-->


	<!--RULE -->
<axsl:template match="ios-def:line_object/ios-def:show_subcommand" priority="1000" mode="M197">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the show_subcommand entity of a line_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M197"/></axsl:template><axsl:template match="text()" priority="-1" mode="M197"/><axsl:template match="@*|node()" priority="-2" mode="M197"><axsl:apply-templates select="@*|*" mode="M197"/></axsl:template>

<!--PATTERN linesteshow_subcommand-->


	<!--RULE -->
<axsl:template match="ios-def:line_state/ios-def:show_subcommand" priority="1000" mode="M198">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the show_subcommand entity of a line_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M198"/></axsl:template><axsl:template match="text()" priority="-1" mode="M198"/><axsl:template match="@*|node()" priority="-2" mode="M198"><axsl:apply-templates select="@*|*" mode="M198"/></axsl:template>

<!--PATTERN linesteconfig_line-->


	<!--RULE -->
<axsl:template match="ios-def:line_state/ios-def:config_line" priority="1000" mode="M199">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the config_line entity of a line_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M199"/></axsl:template><axsl:template match="text()" priority="-1" mode="M199"/><axsl:template match="@*|node()" priority="-2" mode="M199"><axsl:apply-templates select="@*|*" mode="M199"/></axsl:template>

<!--PATTERN snmptst-->


	<!--RULE -->
<axsl:template match="ios-def:snmp_test/ios-def:object" priority="1001" mode="M200">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:snmp_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a snmp_test must reference a snmp_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M200"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:snmp_test/ios-def:state" priority="1000" mode="M200">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:snmp_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a snmp_test must reference a snmp_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M200"/></axsl:template><axsl:template match="text()" priority="-1" mode="M200"/><axsl:template match="@*|node()" priority="-2" mode="M200"><axsl:apply-templates select="@*|*" mode="M200"/></axsl:template>

<!--PATTERN snmpsteaccess_list-->


	<!--RULE -->
<axsl:template match="ios-def:snmp_state/ios-def:access_list" priority="1000" mode="M201">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_list entity of a snmp_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M201"/></axsl:template><axsl:template match="text()" priority="-1" mode="M201"/><axsl:template match="@*|node()" priority="-2" mode="M201"><axsl:apply-templates select="@*|*" mode="M201"/></axsl:template>

<!--PATTERN snmpstecommunity_name-->


	<!--RULE -->
<axsl:template match="ios-def:snmp_state/ios-def:community_name" priority="1000" mode="M202">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the community_name entity of a snmp_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M202"/></axsl:template><axsl:template match="text()" priority="-1" mode="M202"/><axsl:template match="@*|node()" priority="-2" mode="M202"><axsl:apply-templates select="@*|*" mode="M202"/></axsl:template>

<!--PATTERN tclshtst-->


	<!--RULE -->
<axsl:template match="ios-def:tclsh_test/ios-def:object" priority="1001" mode="M203">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:tclsh_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a tclsh_test must reference a tclsh_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M203"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:tclsh_test/ios-def:state" priority="1000" mode="M203">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:tclsh_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a tclsh_test must reference a tclsh_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M203"/></axsl:template><axsl:template match="text()" priority="-1" mode="M203"/><axsl:template match="@*|node()" priority="-2" mode="M203"><axsl:apply-templates select="@*|*" mode="M203"/></axsl:template>

<!--PATTERN tclshsteavailable-->


	<!--RULE -->
<axsl:template match="ios-def:tclsh_state/ios-def:available" priority="1000" mode="M204">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the available entity of a tclsh_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M204"/></axsl:template><axsl:template match="text()" priority="-1" mode="M204"/><axsl:template match="@*|node()" priority="-2" mode="M204"><axsl:apply-templates select="@*|*" mode="M204"/></axsl:template>

<!--PATTERN iosvertst-->


	<!--RULE -->
<axsl:template match="ios-def:version_test/ios-def:object" priority="1001" mode="M205">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:version_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a version_test must reference a version_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M205"/></axsl:template>

	<!--RULE -->
<axsl:template match="ios-def:version_test/ios-def:state" priority="1000" mode="M205">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:version_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a version_test must reference a version_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M205"/></axsl:template><axsl:template match="text()" priority="-1" mode="M205"/><axsl:template match="@*|node()" priority="-2" mode="M205"><axsl:apply-templates select="@*|*" mode="M205"/></axsl:template>

<!--PATTERN iosverstemajor_release-->


	<!--RULE -->
<axsl:template match="ios-def:version_state/ios-def:major_release" priority="1000" mode="M206">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the major_release entity of a version_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M206"/></axsl:template><axsl:template match="text()" priority="-1" mode="M206"/><axsl:template match="@*|node()" priority="-2" mode="M206"><axsl:apply-templates select="@*|*" mode="M206"/></axsl:template>

<!--PATTERN iosverstetrain_number-->


	<!--RULE -->
<axsl:template match="ios-def:version_state/ios-def:train_number" priority="1000" mode="M207">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the train_number entity of a version_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M207"/></axsl:template><axsl:template match="text()" priority="-1" mode="M207"/><axsl:template match="@*|node()" priority="-2" mode="M207"><axsl:apply-templates select="@*|*" mode="M207"/></axsl:template>

<!--PATTERN iosverstetrain_identifier-->


	<!--RULE -->
<axsl:template match="ios-def:version_state/ios-def:train_identifier" priority="1000" mode="M208">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the train_identifier entity of a version_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M208"/></axsl:template><axsl:template match="text()" priority="-1" mode="M208"/><axsl:template match="@*|node()" priority="-2" mode="M208"><axsl:apply-templates select="@*|*" mode="M208"/></axsl:template>

<!--PATTERN iosversteversion_string-->


	<!--RULE -->
<axsl:template match="ios-def:version_state/ios-def:version_string" priority="1000" mode="M209">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='ios_version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version_string entity of a version_state should be 'ios_version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M209"/></axsl:template><axsl:template match="text()" priority="-1" mode="M209"/><axsl:template match="@*|node()" priority="-2" mode="M209"><axsl:apply-templates select="@*|*" mode="M209"/></axsl:template>

<!--PATTERN dpkgobjname-->


	<!--RULE -->
<axsl:template match="linux-def:dpkginfo_object/linux-def:name" priority="1000" mode="M210">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a dpkginfo_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the name entity of a dpkginfo_object should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M210"/></axsl:template><axsl:template match="text()" priority="-1" mode="M210"/><axsl:template match="@*|node()" priority="-2" mode="M210"><axsl:apply-templates select="@*|*" mode="M210"/></axsl:template>

<!--PATTERN dpkgstename-->


	<!--RULE -->
<axsl:template match="linux-def:dpkginfo_state/linux-def:name" priority="1000" mode="M211">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a dpkginfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the name entity of a dpkginfo_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M211"/></axsl:template><axsl:template match="text()" priority="-1" mode="M211"/><axsl:template match="@*|node()" priority="-2" mode="M211"><axsl:apply-templates select="@*|*" mode="M211"/></axsl:template>

<!--PATTERN dpkgstearch-->


	<!--RULE -->
<axsl:template match="linux-def:dpkginfo_state/linux-def:arch" priority="1000" mode="M212">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the arch entity of a dpkginfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the arch entity of a dpkginfo_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M212"/></axsl:template><axsl:template match="text()" priority="-1" mode="M212"/><axsl:template match="@*|node()" priority="-2" mode="M212"><axsl:apply-templates select="@*|*" mode="M212"/></axsl:template>

<!--PATTERN dpkgsteepoch-->


	<!--RULE -->
<axsl:template match="linux-def:dpkginfo_state/linux-def:epoch" priority="1000" mode="M213">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the epoch entity of a dpkginfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or 'pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the epoch entity of a dpkginfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M213"/></axsl:template><axsl:template match="text()" priority="-1" mode="M213"/><axsl:template match="@*|node()" priority="-2" mode="M213"><axsl:apply-templates select="@*|*" mode="M213"/></axsl:template>

<!--PATTERN dpkgsterelease-->


	<!--RULE -->
<axsl:template match="linux-def:dpkginfo_state/linux-def:release" priority="1000" mode="M214">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the release entity of a dpkginfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or 'pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the release entity of a dpkginfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M214"/></axsl:template><axsl:template match="text()" priority="-1" mode="M214"/><axsl:template match="@*|node()" priority="-2" mode="M214"><axsl:apply-templates select="@*|*" mode="M214"/></axsl:template>

<!--PATTERN dpkgsteversion-->


	<!--RULE -->
<axsl:template match="linux-def:dpkginfo_state/linux-def:version" priority="1000" mode="M215">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a dpkginfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or 'pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the version entity of a dpkginfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M215"/></axsl:template><axsl:template match="text()" priority="-1" mode="M215"/><axsl:template match="@*|node()" priority="-2" mode="M215"><axsl:apply-templates select="@*|*" mode="M215"/></axsl:template>

<!--PATTERN dpkgsteevr-->


	<!--RULE -->
<axsl:template match="linux-def:dpkginfo_state/linux-def:evr" priority="1000" mode="M216">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='evr_string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the evr entity of a dpkginfo_state should be 'evr_string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or 'pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the evr entity of a dpkginfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M216"/></axsl:template><axsl:template match="text()" priority="-1" mode="M216"/><axsl:template match="@*|node()" priority="-2" mode="M216"><axsl:apply-templates select="@*|*" mode="M216"/></axsl:template>

<!--PATTERN ilsobjprotocol-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_object/linux-def:protocol" priority="1000" mode="M217">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an inetlisteningservers_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the protocol entity of an inetlisteningservers_object should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M217"/></axsl:template><axsl:template match="text()" priority="-1" mode="M217"/><axsl:template match="@*|node()" priority="-2" mode="M217"><axsl:apply-templates select="@*|*" mode="M217"/></axsl:template>

<!--PATTERN ilsobjlocal_address-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_object/linux-def:local_address" priority="1000" mode="M218">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_address entity of an inetlisteningservers_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the local_address entity of an inetlisteningservers_object should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M218"/></axsl:template><axsl:template match="text()" priority="-1" mode="M218"/><axsl:template match="@*|node()" priority="-2" mode="M218"><axsl:apply-templates select="@*|*" mode="M218"/></axsl:template>

<!--PATTERN ilsobjlocal_port-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_object/linux-def:local_port" priority="1000" mode="M219">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_port entity of an inetlisteningservers_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the local_port entity of an inetlisteningservers_object should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M219"/></axsl:template><axsl:template match="text()" priority="-1" mode="M219"/><axsl:template match="@*|node()" priority="-2" mode="M219"><axsl:apply-templates select="@*|*" mode="M219"/></axsl:template>

<!--PATTERN ilssteprotocol-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:protocol" priority="1000" mode="M220">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the protocol entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M220"/></axsl:template><axsl:template match="text()" priority="-1" mode="M220"/><axsl:template match="@*|node()" priority="-2" mode="M220"><axsl:apply-templates select="@*|*" mode="M220"/></axsl:template>

<!--PATTERN ilsstelocal_address-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:local_address" priority="1000" mode="M221">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_address entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the local_address entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M221"/></axsl:template><axsl:template match="text()" priority="-1" mode="M221"/><axsl:template match="@*|node()" priority="-2" mode="M221"><axsl:apply-templates select="@*|*" mode="M221"/></axsl:template>

<!--PATTERN ilsstelocal_port-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:local_port" priority="1000" mode="M222">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_port entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the local_port entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M222"/></axsl:template><axsl:template match="text()" priority="-1" mode="M222"/><axsl:template match="@*|node()" priority="-2" mode="M222"><axsl:apply-templates select="@*|*" mode="M222"/></axsl:template>

<!--PATTERN ilsstelocal_full_address-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:local_full_address" priority="1000" mode="M223">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_full_address entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the local_full_address entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M223"/></axsl:template><axsl:template match="text()" priority="-1" mode="M223"/><axsl:template match="@*|node()" priority="-2" mode="M223"><axsl:apply-templates select="@*|*" mode="M223"/></axsl:template>

<!--PATTERN ilssteprogram_name-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:program_name" priority="1000" mode="M224">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the program_name entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the program_name entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M224"/></axsl:template><axsl:template match="text()" priority="-1" mode="M224"/><axsl:template match="@*|node()" priority="-2" mode="M224"><axsl:apply-templates select="@*|*" mode="M224"/></axsl:template>

<!--PATTERN ilssteforeign_address-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:foreign_address" priority="1000" mode="M225">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_address entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the foreign_address entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M225"/></axsl:template><axsl:template match="text()" priority="-1" mode="M225"/><axsl:template match="@*|node()" priority="-2" mode="M225"><axsl:apply-templates select="@*|*" mode="M225"/></axsl:template>

<!--PATTERN ilssteforeign_port-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:foreign_port" priority="1000" mode="M226">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_port entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the foreign_port entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M226"/></axsl:template><axsl:template match="text()" priority="-1" mode="M226"/><axsl:template match="@*|node()" priority="-2" mode="M226"><axsl:apply-templates select="@*|*" mode="M226"/></axsl:template>

<!--PATTERN ilssteforeign_full_address-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:foreign_full_address" priority="1000" mode="M227">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_full_address entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the foreign_full_address entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M227"/></axsl:template><axsl:template match="text()" priority="-1" mode="M227"/><axsl:template match="@*|node()" priority="-2" mode="M227"><axsl:apply-templates select="@*|*" mode="M227"/></axsl:template>

<!--PATTERN ilsstepid-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:pid" priority="1000" mode="M228">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of an inetlisteningservers_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the pid entity of an inetlisteningservers_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', or 'less than or equal'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M228"/></axsl:template><axsl:template match="text()" priority="-1" mode="M228"/><axsl:template match="@*|node()" priority="-2" mode="M228"><axsl:apply-templates select="@*|*" mode="M228"/></axsl:template>

<!--PATTERN ilssteuser_id-->


	<!--RULE -->
<axsl:template match="linux-def:inetlisteningservers_state/linux-def:user_id" priority="1000" mode="M229">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the user_id entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M229"/></axsl:template><axsl:template match="text()" priority="-1" mode="M229"/><axsl:template match="@*|node()" priority="-2" mode="M229"><axsl:apply-templates select="@*|*" mode="M229"/></axsl:template>

<!--PATTERN rpmobjname-->


	<!--RULE -->
<axsl:template match="linux-def:rpminfo_object/linux-def:name" priority="1000" mode="M230">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a rpminfo_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the name entity of a rpminfo_object should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M230"/></axsl:template><axsl:template match="text()" priority="-1" mode="M230"/><axsl:template match="@*|node()" priority="-2" mode="M230"><axsl:apply-templates select="@*|*" mode="M230"/></axsl:template>

<!--PATTERN rpmstename-->


	<!--RULE -->
<axsl:template match="linux-def:rpminfo_state/linux-def:name" priority="1000" mode="M231">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a rpminfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the name entity of a rpminfo_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M231"/></axsl:template><axsl:template match="text()" priority="-1" mode="M231"/><axsl:template match="@*|node()" priority="-2" mode="M231"><axsl:apply-templates select="@*|*" mode="M231"/></axsl:template>

<!--PATTERN rpmstearch-->


	<!--RULE -->
<axsl:template match="linux-def:rpminfo_state/linux-def:arch" priority="1000" mode="M232">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the arch entity of a rpminfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the arch entity of a rpminfo_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M232"/></axsl:template><axsl:template match="text()" priority="-1" mode="M232"/><axsl:template match="@*|node()" priority="-2" mode="M232"><axsl:apply-templates select="@*|*" mode="M232"/></axsl:template>

<!--PATTERN rpmsteepoch-->


	<!--RULE -->
<axsl:template match="linux-def:rpminfo_state/linux-def:epoch" priority="1000" mode="M233">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the epoch entity of a rpminfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the epoch entity of a rpminfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M233"/></axsl:template><axsl:template match="text()" priority="-1" mode="M233"/><axsl:template match="@*|node()" priority="-2" mode="M233"><axsl:apply-templates select="@*|*" mode="M233"/></axsl:template>

<!--PATTERN rpmsterelease-->


	<!--RULE -->
<axsl:template match="linux-def:rpminfo_state/linux-def:release" priority="1000" mode="M234">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string' or @datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the release entity of a rpminfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the release entity of a rpminfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M234"/></axsl:template><axsl:template match="text()" priority="-1" mode="M234"/><axsl:template match="@*|node()" priority="-2" mode="M234"><axsl:apply-templates select="@*|*" mode="M234"/></axsl:template>

<!--PATTERN rpmsteversion-->


	<!--RULE -->
<axsl:template match="linux-def:rpminfo_state/linux-def:version" priority="1000" mode="M235">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string' or @datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a rpminfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the version entity of a rpminfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M235"/></axsl:template><axsl:template match="text()" priority="-1" mode="M235"/><axsl:template match="@*|node()" priority="-2" mode="M235"><axsl:apply-templates select="@*|*" mode="M235"/></axsl:template>

<!--PATTERN rpmsteevr-->


	<!--RULE -->
<axsl:template match="linux-def:rpminfo_state/linux-def:evr" priority="1000" mode="M236">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='evr_string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the evr entity of a rpminfo_state should be 'evr_string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the evr entity of a rpminfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M236"/></axsl:template><axsl:template match="text()" priority="-1" mode="M236"/><axsl:template match="@*|node()" priority="-2" mode="M236"><axsl:apply-templates select="@*|*" mode="M236"/></axsl:template>

<!--PATTERN rpmstesignaturekeyid-->


	<!--RULE -->
<axsl:template match="linux-def:rpminfo_state/linux-def:signature_keyid" priority="1000" mode="M237">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the signature_keyid entity of a rpminfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the signature_keyid entity of a rpminfo_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M237"/></axsl:template><axsl:template match="text()" priority="-1" mode="M237"/><axsl:template match="@*|node()" priority="-2" mode="M237"><axsl:apply-templates select="@*|*" mode="M237"/></axsl:template>

<!--PATTERN spkginfoobjname-->


	<!--RULE -->
<axsl:template match="linux-def:slackwarepkginfo_object/linux-def:name" priority="1000" mode="M238">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a slackwarepkginfo_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the name entity of a slackwarepkginfo_object should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M238"/></axsl:template><axsl:template match="text()" priority="-1" mode="M238"/><axsl:template match="@*|node()" priority="-2" mode="M238"><axsl:apply-templates select="@*|*" mode="M238"/></axsl:template>

<!--PATTERN spkginfostename-->


	<!--RULE -->
<axsl:template match="linux-def:slackwarepkginfo_state/linux-def:name" priority="1000" mode="M239">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a slackwarepkginfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the name entity of a slackwarepkginfo_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M239"/></axsl:template><axsl:template match="text()" priority="-1" mode="M239"/><axsl:template match="@*|node()" priority="-2" mode="M239"><axsl:apply-templates select="@*|*" mode="M239"/></axsl:template>

<!--PATTERN spkginfosteversion-->


	<!--RULE -->
<axsl:template match="linux-def:slackwarepkginfo_state/linux-def:version" priority="1000" mode="M240">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a slackwarepkginfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the version entity of a slackwarepkginfo_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M240"/></axsl:template><axsl:template match="text()" priority="-1" mode="M240"/><axsl:template match="@*|node()" priority="-2" mode="M240"><axsl:apply-templates select="@*|*" mode="M240"/></axsl:template>

<!--PATTERN spkginfostearch-->


	<!--RULE -->
<axsl:template match="linux-def:slackwarepkginfo_state/linux-def:architecture" priority="1000" mode="M241">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the architecture entity of a slackwarepkginfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the architecture entity of a slackwarepkginfo_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M241"/></axsl:template><axsl:template match="text()" priority="-1" mode="M241"/><axsl:template match="@*|node()" priority="-2" mode="M241"><axsl:apply-templates select="@*|*" mode="M241"/></axsl:template>

<!--PATTERN spkginfosterevision-->


	<!--RULE -->
<axsl:template match="linux-def:slackwarepkginfo_state/linux-def:revision" priority="1000" mode="M242">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the revision entity of a slackwarepkginfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - operation attribute for the revision entity of a slackwarepkginfo_state should be 'equals', 'not equal', or 'pattern match'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M242"/></axsl:template><axsl:template match="text()" priority="-1" mode="M242"/><axsl:template match="@*|node()" priority="-2" mode="M242"><axsl:apply-templates select="@*|*" mode="M242"/></axsl:template>

<!--PATTERN accountobjusername-->


	<!--RULE -->
<axsl:template match="macos-def:accountinfo_object/macos-def:username" priority="1000" mode="M243">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of an accountinfo_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M243"/></axsl:template><axsl:template match="text()" priority="-1" mode="M243"/><axsl:template match="@*|node()" priority="-2" mode="M243"><axsl:apply-templates select="@*|*" mode="M243"/></axsl:template>

<!--PATTERN accountsteusername-->


	<!--RULE -->
<axsl:template match="macos-def:accountinfo_state/macos-def:username" priority="1000" mode="M244">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of an accountinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M244"/></axsl:template><axsl:template match="text()" priority="-1" mode="M244"/><axsl:template match="@*|node()" priority="-2" mode="M244"><axsl:apply-templates select="@*|*" mode="M244"/></axsl:template>

<!--PATTERN accountstepassword-->


	<!--RULE -->
<axsl:template match="macos-def:accountinfo_state/macos-def:password" priority="1000" mode="M245">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password entity of an accountinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M245"/></axsl:template><axsl:template match="text()" priority="-1" mode="M245"/><axsl:template match="@*|node()" priority="-2" mode="M245"><axsl:apply-templates select="@*|*" mode="M245"/></axsl:template>

<!--PATTERN accountsteuid-->


	<!--RULE -->
<axsl:template match="macos-def:accountinfo_state/macos-def:uid" priority="1000" mode="M246">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uid entity of an accountinfo_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M246"/></axsl:template><axsl:template match="text()" priority="-1" mode="M246"/><axsl:template match="@*|node()" priority="-2" mode="M246"><axsl:apply-templates select="@*|*" mode="M246"/></axsl:template>

<!--PATTERN accountstegid-->


	<!--RULE -->
<axsl:template match="macos-def:accountinfo_state/macos-def:gid" priority="1000" mode="M247">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gid entity of an accountinfo_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M247"/></axsl:template><axsl:template match="text()" priority="-1" mode="M247"/><axsl:template match="@*|node()" priority="-2" mode="M247"><axsl:apply-templates select="@*|*" mode="M247"/></axsl:template>

<!--PATTERN accountsterealname-->


	<!--RULE -->
<axsl:template match="macos-def:accountinfo_state/macos-def:realname" priority="1000" mode="M248">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the realname entity of an accountinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M248"/></axsl:template><axsl:template match="text()" priority="-1" mode="M248"/><axsl:template match="@*|node()" priority="-2" mode="M248"><axsl:apply-templates select="@*|*" mode="M248"/></axsl:template>

<!--PATTERN accountstehome_dir-->


	<!--RULE -->
<axsl:template match="macos-def:accountinfo_state/macos-def:home_dir" priority="1000" mode="M249">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the home_dir entity of an accountinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M249"/></axsl:template><axsl:template match="text()" priority="-1" mode="M249"/><axsl:template match="@*|node()" priority="-2" mode="M249"><axsl:apply-templates select="@*|*" mode="M249"/></axsl:template>

<!--PATTERN accountstelogin_shell-->


	<!--RULE -->
<axsl:template match="macos-def:accountinfo_state/macos-def:login_shell" priority="1000" mode="M250">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_shell entity of an accountinfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M250"/></axsl:template><axsl:template match="text()" priority="-1" mode="M250"/><axsl:template match="@*|node()" priority="-2" mode="M250"><axsl:apply-templates select="@*|*" mode="M250"/></axsl:template>

<!--PATTERN macosilsobjprogram_name-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_object/macos-def:program_name" priority="1000" mode="M251">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the program_name entity of an inetlisteningservers_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M251"/></axsl:template><axsl:template match="text()" priority="-1" mode="M251"/><axsl:template match="@*|node()" priority="-2" mode="M251"><axsl:apply-templates select="@*|*" mode="M251"/></axsl:template>

<!--PATTERN macosilssteprogram_name-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:program_name" priority="1000" mode="M252">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the program_name entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M252"/></axsl:template><axsl:template match="text()" priority="-1" mode="M252"/><axsl:template match="@*|node()" priority="-2" mode="M252"><axsl:apply-templates select="@*|*" mode="M252"/></axsl:template>

<!--PATTERN macosilsstelocal_address-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:local_address" priority="1000" mode="M253">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_address entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M253"/></axsl:template><axsl:template match="text()" priority="-1" mode="M253"/><axsl:template match="@*|node()" priority="-2" mode="M253"><axsl:apply-templates select="@*|*" mode="M253"/></axsl:template>

<!--PATTERN macosilsstelocal_full_address-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:local_full_address" priority="1000" mode="M254">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_full_address entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M254"/></axsl:template><axsl:template match="text()" priority="-1" mode="M254"/><axsl:template match="@*|node()" priority="-2" mode="M254"><axsl:apply-templates select="@*|*" mode="M254"/></axsl:template>

<!--PATTERN macosilsstelocal_port-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:local_port" priority="1000" mode="M255">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_port entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M255"/></axsl:template><axsl:template match="text()" priority="-1" mode="M255"/><axsl:template match="@*|node()" priority="-2" mode="M255"><axsl:apply-templates select="@*|*" mode="M255"/></axsl:template>

<!--PATTERN macosilssteforeign_address-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:foreign_address" priority="1000" mode="M256">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_address entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M256"/></axsl:template><axsl:template match="text()" priority="-1" mode="M256"/><axsl:template match="@*|node()" priority="-2" mode="M256"><axsl:apply-templates select="@*|*" mode="M256"/></axsl:template>

<!--PATTERN macosilssteforeign_full_address-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:foreign_full_address" priority="1000" mode="M257">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_full_address entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M257"/></axsl:template><axsl:template match="text()" priority="-1" mode="M257"/><axsl:template match="@*|node()" priority="-2" mode="M257"><axsl:apply-templates select="@*|*" mode="M257"/></axsl:template>

<!--PATTERN macosilssteforeign_port-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:foreign_port" priority="1000" mode="M258">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_port entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M258"/></axsl:template><axsl:template match="text()" priority="-1" mode="M258"/><axsl:template match="@*|node()" priority="-2" mode="M258"><axsl:apply-templates select="@*|*" mode="M258"/></axsl:template>

<!--PATTERN macosilsstepid-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:pid" priority="1000" mode="M259">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of an inetlisteningservers_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M259"/></axsl:template><axsl:template match="text()" priority="-1" mode="M259"/><axsl:template match="@*|node()" priority="-2" mode="M259"><axsl:apply-templates select="@*|*" mode="M259"/></axsl:template>

<!--PATTERN macosilssteprotocol-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:protocol" priority="1000" mode="M260">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M260"/></axsl:template><axsl:template match="text()" priority="-1" mode="M260"/><axsl:template match="@*|node()" priority="-2" mode="M260"><axsl:apply-templates select="@*|*" mode="M260"/></axsl:template>

<!--PATTERN macosilssteuser_id-->


	<!--RULE -->
<axsl:template match="macos-def:inetlisteningservers_state/macos-def:user_id" priority="1000" mode="M261">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of an inetlisteningservers_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M261"/></axsl:template><axsl:template match="text()" priority="-1" mode="M261"/><axsl:template match="@*|node()" priority="-2" mode="M261"><axsl:apply-templates select="@*|*" mode="M261"/></axsl:template>

<!--PATTERN nvramobjnvram_var-->


	<!--RULE -->
<axsl:template match="macos-def:nvram_object/macos-def:nvram_var" priority="1000" mode="M262">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the nvram_var entity of a nvram_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M262"/></axsl:template><axsl:template match="text()" priority="-1" mode="M262"/><axsl:template match="@*|node()" priority="-2" mode="M262"><axsl:apply-templates select="@*|*" mode="M262"/></axsl:template>

<!--PATTERN nvramstenvram_var-->


	<!--RULE -->
<axsl:template match="macos-def:nvram_state/macos-def:nvram_var" priority="1000" mode="M263">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the nvram_var entity of a nvram_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M263"/></axsl:template><axsl:template match="text()" priority="-1" mode="M263"/><axsl:template match="@*|node()" priority="-2" mode="M263"><axsl:apply-templates select="@*|*" mode="M263"/></axsl:template>

<!--PATTERN nvramstenvram_value-->


	<!--RULE -->
<axsl:template match="macos-def:nvram_state/macos-def:nvram_value" priority="1000" mode="M264">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the nvram_value entity of a nvram_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M264"/></axsl:template><axsl:template match="text()" priority="-1" mode="M264"/><axsl:template match="@*|node()" priority="-2" mode="M264"><axsl:apply-templates select="@*|*" mode="M264"/></axsl:template>

<!--PATTERN pwpobjusername-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_object/macos-def:username" priority="1000" mode="M265">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a pwpolicy_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M265"/></axsl:template><axsl:template match="text()" priority="-1" mode="M265"/><axsl:template match="@*|node()" priority="-2" mode="M265"><axsl:apply-templates select="@*|*" mode="M265"/></axsl:template>

<!--PATTERN pwpobjuserpass-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_object/macos-def:userpass" priority="1000" mode="M266">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the userpass entity of a pwpolicy_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M266"/></axsl:template><axsl:template match="text()" priority="-1" mode="M266"/><axsl:template match="@*|node()" priority="-2" mode="M266"><axsl:apply-templates select="@*|*" mode="M266"/></axsl:template>

<!--PATTERN pwpobjdirectory_node-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_object/macos-def:directory_node" priority="1000" mode="M267">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_node entity of a pwpolicy_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M267"/></axsl:template><axsl:template match="text()" priority="-1" mode="M267"/><axsl:template match="@*|node()" priority="-2" mode="M267"><axsl:apply-templates select="@*|*" mode="M267"/></axsl:template>

<!--PATTERN pwpsteusername-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state/macos-def:username" priority="1000" mode="M268">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a pwpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M268"/></axsl:template><axsl:template match="text()" priority="-1" mode="M268"/><axsl:template match="@*|node()" priority="-2" mode="M268"><axsl:apply-templates select="@*|*" mode="M268"/></axsl:template>

<!--PATTERN pwpsteuserpass-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state/macos-def:userpass" priority="1000" mode="M269">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the userpass entity of a pwpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M269"/></axsl:template><axsl:template match="text()" priority="-1" mode="M269"/><axsl:template match="@*|node()" priority="-2" mode="M269"><axsl:apply-templates select="@*|*" mode="M269"/></axsl:template>

<!--PATTERN pwpstedirectory_node-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state/macos-def:directory_node" priority="1000" mode="M270">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_node entity of a pwpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M270"/></axsl:template><axsl:template match="text()" priority="-1" mode="M270"/><axsl:template match="@*|node()" priority="-2" mode="M270"><axsl:apply-templates select="@*|*" mode="M270"/></axsl:template>

<!--PATTERN pwpstemaxChars-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state/macos-def:maxChars" priority="1000" mode="M271">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the maxChars entity of a pwpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M271"/></axsl:template><axsl:template match="text()" priority="-1" mode="M271"/><axsl:template match="@*|node()" priority="-2" mode="M271"><axsl:apply-templates select="@*|*" mode="M271"/></axsl:template>

<!--PATTERN pwpstemaxFailedLoginAttempts-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state/macos-def:maxFailedLoginAttempts" priority="1000" mode="M272">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the maxFailedLoginAttempts entity of a pwpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M272"/></axsl:template><axsl:template match="text()" priority="-1" mode="M272"/><axsl:template match="@*|node()" priority="-2" mode="M272"><axsl:apply-templates select="@*|*" mode="M272"/></axsl:template>

<!--PATTERN pwpsteminChars-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state/macos-def:minChars" priority="1000" mode="M273">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the minChars entity of a pwpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M273"/></axsl:template><axsl:template match="text()" priority="-1" mode="M273"/><axsl:template match="@*|node()" priority="-2" mode="M273"><axsl:apply-templates select="@*|*" mode="M273"/></axsl:template>

<!--PATTERN pwpstepasswordCannotBeName-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state/macos-def:passwordCannotBeName" priority="1000" mode="M274">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the passwordCannotBeName entity of a pwpolicy_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M274"/></axsl:template><axsl:template match="text()" priority="-1" mode="M274"/><axsl:template match="@*|node()" priority="-2" mode="M274"><axsl:apply-templates select="@*|*" mode="M274"/></axsl:template>

<!--PATTERN pwpsterequiresAlpha-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state/macos-def:requiresAlpha" priority="1000" mode="M275">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the requiresAlpha entity of a pwpolicy_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M275"/></axsl:template><axsl:template match="text()" priority="-1" mode="M275"/><axsl:template match="@*|node()" priority="-2" mode="M275"><axsl:apply-templates select="@*|*" mode="M275"/></axsl:template>

<!--PATTERN pwpsterequiresNumeric-->


	<!--RULE -->
<axsl:template match="macos-def:pwpolicy_state/macos-def:requiresNumeric" priority="1000" mode="M276">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the requiresNumeric entity of a pwpolicy_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M276"/></axsl:template><axsl:template match="text()" priority="-1" mode="M276"/><axsl:template match="@*|node()" priority="-2" mode="M276"><axsl:apply-templates select="@*|*" mode="M276"/></axsl:template>

<!--PATTERN isainfotst-->


	<!--RULE -->
<axsl:template match="sol-def:isainfo_test/sol-def:object" priority="1001" mode="M277">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:smf_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an isainfo_test must reference an isainfo_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M277"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:isainfo_test/sol-def:state" priority="1000" mode="M277">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:smf_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an isainfo_test must reference an isainfo_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M277"/></axsl:template><axsl:template match="text()" priority="-1" mode="M277"/><axsl:template match="@*|node()" priority="-2" mode="M277"><axsl:apply-templates select="@*|*" mode="M277"/></axsl:template>

<!--PATTERN isastebits-->


	<!--RULE -->
<axsl:template match="sol-def:isainfo_state/sol-def:bits" priority="1000" mode="M278">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the bits entity of an isainfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M278"/></axsl:template><axsl:template match="text()" priority="-1" mode="M278"/><axsl:template match="@*|node()" priority="-2" mode="M278"><axsl:apply-templates select="@*|*" mode="M278"/></axsl:template>

<!--PATTERN isastekernel_isa-->


	<!--RULE -->
<axsl:template match="sol-def:isainfo_state/sol-def:kernel_isa" priority="1000" mode="M279">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the kernel_isa entity of an isainfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M279"/></axsl:template><axsl:template match="text()" priority="-1" mode="M279"/><axsl:template match="@*|node()" priority="-2" mode="M279"><axsl:apply-templates select="@*|*" mode="M279"/></axsl:template>

<!--PATTERN isasteapplication_isa-->


	<!--RULE -->
<axsl:template match="sol-def:isainfo_state/sol-def:application_isa" priority="1000" mode="M280">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the application_isa entity of an isainfo_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M280"/></axsl:template><axsl:template match="text()" priority="-1" mode="M280"/><axsl:template match="@*|node()" priority="-2" mode="M280"><axsl:apply-templates select="@*|*" mode="M280"/></axsl:template>

<!--PATTERN packagetst-->


	<!--RULE -->
<axsl:template match="sol-def:package_test/sol-def:object" priority="1001" mode="M281">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:package_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a package_test must reference a package_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M281"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:package_test/sol-def:state" priority="1000" mode="M281">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:package_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a package_test must reference a package_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M281"/></axsl:template><axsl:template match="text()" priority="-1" mode="M281"/><axsl:template match="@*|node()" priority="-2" mode="M281"><axsl:apply-templates select="@*|*" mode="M281"/></axsl:template>

<!--PATTERN packageobjpkginst-->


	<!--RULE -->
<axsl:template match="sol-def:package_object/sol-def:pkginst" priority="1000" mode="M282">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pkginst entity of a package_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M282"/></axsl:template><axsl:template match="text()" priority="-1" mode="M282"/><axsl:template match="@*|node()" priority="-2" mode="M282"><axsl:apply-templates select="@*|*" mode="M282"/></axsl:template>

<!--PATTERN packagestepkginst-->


	<!--RULE -->
<axsl:template match="sol-def:package_state/sol-def:pkginst" priority="1000" mode="M283">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pkginst entity of a package_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M283"/></axsl:template><axsl:template match="text()" priority="-1" mode="M283"/><axsl:template match="@*|node()" priority="-2" mode="M283"><axsl:apply-templates select="@*|*" mode="M283"/></axsl:template>

<!--PATTERN packagestename-->


	<!--RULE -->
<axsl:template match="sol-def:package_state/sol-def:name" priority="1000" mode="M284">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a package_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M284"/></axsl:template><axsl:template match="text()" priority="-1" mode="M284"/><axsl:template match="@*|node()" priority="-2" mode="M284"><axsl:apply-templates select="@*|*" mode="M284"/></axsl:template>

<!--PATTERN packagestecategory-->


	<!--RULE -->
<axsl:template match="sol-def:package_state/sol-def:category" priority="1000" mode="M285">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the category entity of a package_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M285"/></axsl:template><axsl:template match="text()" priority="-1" mode="M285"/><axsl:template match="@*|node()" priority="-2" mode="M285"><axsl:apply-templates select="@*|*" mode="M285"/></axsl:template>

<!--PATTERN packagesteversion-->


	<!--RULE -->
<axsl:template match="sol-def:package_state/sol-def:version" priority="1000" mode="M286">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a package_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M286"/></axsl:template><axsl:template match="text()" priority="-1" mode="M286"/><axsl:template match="@*|node()" priority="-2" mode="M286"><axsl:apply-templates select="@*|*" mode="M286"/></axsl:template>

<!--PATTERN packagestevendor-->


	<!--RULE -->
<axsl:template match="sol-def:package_state/sol-def:vendor" priority="1000" mode="M287">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the vendor entity of a package_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M287"/></axsl:template><axsl:template match="text()" priority="-1" mode="M287"/><axsl:template match="@*|node()" priority="-2" mode="M287"><axsl:apply-templates select="@*|*" mode="M287"/></axsl:template>

<!--PATTERN packagestedescription-->


	<!--RULE -->
<axsl:template match="sol-def:package_state/sol-def:description" priority="1000" mode="M288">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the description entity of a package_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M288"/></axsl:template><axsl:template match="text()" priority="-1" mode="M288"/><axsl:template match="@*|node()" priority="-2" mode="M288"><axsl:apply-templates select="@*|*" mode="M288"/></axsl:template>

<!--PATTERN patch54tst-->


	<!--RULE -->
<axsl:template match="sol-def:patch54_test/sol-def:object" priority="1001" mode="M289">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:patch54_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a patch54_test must reference a patch54_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M289"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:patch54_test/sol-def:state" priority="1000" mode="M289">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:patch_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a patch54_test must reference a patch_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M289"/></axsl:template><axsl:template match="text()" priority="-1" mode="M289"/><axsl:template match="@*|node()" priority="-2" mode="M289"><axsl:apply-templates select="@*|*" mode="M289"/></axsl:template>

<!--PATTERN patchtst-->


	<!--RULE -->
<axsl:template match="sol-def:patch_test/sol-def:object" priority="1001" mode="M290">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:patch_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a patch_test must reference a patch_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M290"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:patch_test/sol-def:state" priority="1000" mode="M290">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:patch_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a patch_test must reference a patch_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M290"/></axsl:template><axsl:template match="text()" priority="-1" mode="M290"/><axsl:template match="@*|node()" priority="-2" mode="M290"><axsl:apply-templates select="@*|*" mode="M290"/></axsl:template>

<!--PATTERN patch54objbase-->


	<!--RULE -->
<axsl:template match="sol-def:patch54_object/sol-def:base" priority="1000" mode="M291">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the base entity of a patch54_object should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M291"/></axsl:template><axsl:template match="text()" priority="-1" mode="M291"/><axsl:template match="@*|node()" priority="-2" mode="M291"><axsl:apply-templates select="@*|*" mode="M291"/></axsl:template>

<!--PATTERN patch54objversion-->


	<!--RULE -->
<axsl:template match="sol-def:patch54_object/sol-def:version" priority="1000" mode="M292">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a patch54_object should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M292"/></axsl:template><axsl:template match="text()" priority="-1" mode="M292"/><axsl:template match="@*|node()" priority="-2" mode="M292"><axsl:apply-templates select="@*|*" mode="M292"/></axsl:template>

<!--PATTERN patchobjbase-->


	<!--RULE -->
<axsl:template match="sol-def:patch_object/sol-def:base" priority="1000" mode="M293">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the base entity of a patch_object should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M293"/></axsl:template><axsl:template match="text()" priority="-1" mode="M293"/><axsl:template match="@*|node()" priority="-2" mode="M293"><axsl:apply-templates select="@*|*" mode="M293"/></axsl:template>

<!--PATTERN patchstebase-->


	<!--RULE -->
<axsl:template match="sol-def:patch_state/sol-def:base" priority="1000" mode="M294">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the base entity of a patch_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M294"/></axsl:template><axsl:template match="text()" priority="-1" mode="M294"/><axsl:template match="@*|node()" priority="-2" mode="M294"><axsl:apply-templates select="@*|*" mode="M294"/></axsl:template>

<!--PATTERN patchsteversion-->


	<!--RULE -->
<axsl:template match="sol-def:patch_state/sol-def:version" priority="1000" mode="M295">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a patch_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M295"/></axsl:template><axsl:template match="text()" priority="-1" mode="M295"/><axsl:template match="@*|node()" priority="-2" mode="M295"><axsl:apply-templates select="@*|*" mode="M295"/></axsl:template>

<!--PATTERN smftst-->


	<!--RULE -->
<axsl:template match="sol-def:smf_test/sol-def:object" priority="1001" mode="M296">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:smf_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a smf_test must reference a smf_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M296"/></axsl:template>

	<!--RULE -->
<axsl:template match="sol-def:smf_test/sol-def:state" priority="1000" mode="M296">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:smf_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a smf_test must reference a smf_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M296"/></axsl:template><axsl:template match="text()" priority="-1" mode="M296"/><axsl:template match="@*|node()" priority="-2" mode="M296"><axsl:apply-templates select="@*|*" mode="M296"/></axsl:template>

<!--PATTERN fmriobjbase-->


	<!--RULE -->
<axsl:template match="sol-def:smf_object/sol-def:fmri" priority="1000" mode="M297">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the fmri entity of a smf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M297"/></axsl:template><axsl:template match="text()" priority="-1" mode="M297"/><axsl:template match="@*|node()" priority="-2" mode="M297"><axsl:apply-templates select="@*|*" mode="M297"/></axsl:template>

<!--PATTERN fmristebase-->


	<!--RULE -->
<axsl:template match="sol-def:smf_state/sol-def:fmri" priority="1000" mode="M298">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the fmri entity of a smf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M298"/></axsl:template><axsl:template match="text()" priority="-1" mode="M298"/><axsl:template match="@*|node()" priority="-2" mode="M298"><axsl:apply-templates select="@*|*" mode="M298"/></axsl:template>

<!--PATTERN srvnamestebase-->


	<!--RULE -->
<axsl:template match="sol-def:smf_state/sol-def:service_name" priority="1000" mode="M299">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of a smf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M299"/></axsl:template><axsl:template match="text()" priority="-1" mode="M299"/><axsl:template match="@*|node()" priority="-2" mode="M299"><axsl:apply-templates select="@*|*" mode="M299"/></axsl:template>

<!--PATTERN srvstatestebase-->


	<!--RULE -->
<axsl:template match="sol-def:smf_state/sol-def:service_state" priority="1000" mode="M300">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_state entity of a smf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M300"/></axsl:template><axsl:template match="text()" priority="-1" mode="M300"/><axsl:template match="@*|node()" priority="-2" mode="M300"><axsl:apply-templates select="@*|*" mode="M300"/></axsl:template>

<!--PATTERN protocolstebase-->


	<!--RULE -->
<axsl:template match="sol-def:smf_state/sol-def:protocol" priority="1000" mode="M301">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of a smf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M301"/></axsl:template><axsl:template match="text()" priority="-1" mode="M301"/><axsl:template match="@*|node()" priority="-2" mode="M301"><axsl:apply-templates select="@*|*" mode="M301"/></axsl:template>

<!--PATTERN srvexestebase-->


	<!--RULE -->
<axsl:template match="sol-def:smf_state/sol-def:server_executable" priority="1000" mode="M302">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_executable entity of a smf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M302"/></axsl:template><axsl:template match="text()" priority="-1" mode="M302"/><axsl:template match="@*|node()" priority="-2" mode="M302"><axsl:apply-templates select="@*|*" mode="M302"/></axsl:template>

<!--PATTERN srvargsstebase-->


	<!--RULE -->
<axsl:template match="sol-def:smf_state/sol-def:server_arguements" priority="1000" mode="M303">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_arguements entity of a smf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M303"/></axsl:template><axsl:template match="text()" priority="-1" mode="M303"/><axsl:template match="@*|node()" priority="-2" mode="M303"><axsl:apply-templates select="@*|*" mode="M303"/></axsl:template>

<!--PATTERN execasuserstebase-->


	<!--RULE -->
<axsl:template match="sol-def:smf_state/sol-def:exec_as_user" priority="1000" mode="M304">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exec_as_user entity of a smf_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M304"/></axsl:template><axsl:template match="text()" priority="-1" mode="M304"/><axsl:template match="@*|node()" priority="-2" mode="M304"><axsl:apply-templates select="@*|*" mode="M304"/></axsl:template>

<!--PATTERN unixfileobjpath-->


	<!--RULE -->
<axsl:template match="unix-def:file_object/unix-def:path" priority="1000" mode="M305">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a file_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M305"/></axsl:template><axsl:template match="text()" priority="-1" mode="M305"/><axsl:template match="@*|node()" priority="-2" mode="M305"><axsl:apply-templates select="@*|*" mode="M305"/></axsl:template>

<!--PATTERN unixfileobjfilename-->


	<!--RULE -->
<axsl:template match="unix-def:file_object/unix-def:filename" priority="1000" mode="M306">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a file_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M306"/></axsl:template><axsl:template match="text()" priority="-1" mode="M306"/><axsl:template match="@*|node()" priority="-2" mode="M306"><axsl:apply-templates select="@*|*" mode="M306"/></axsl:template>

<!--PATTERN unixfilestepath-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:path" priority="1000" mode="M307">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M307"/></axsl:template><axsl:template match="text()" priority="-1" mode="M307"/><axsl:template match="@*|node()" priority="-2" mode="M307"><axsl:apply-templates select="@*|*" mode="M307"/></axsl:template>

<!--PATTERN unixfilestefilename-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:filename" priority="1000" mode="M308">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M308"/></axsl:template><axsl:template match="text()" priority="-1" mode="M308"/><axsl:template match="@*|node()" priority="-2" mode="M308"><axsl:apply-templates select="@*|*" mode="M308"/></axsl:template>

<!--PATTERN unixfilestetype-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:type" priority="1000" mode="M309">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M309"/></axsl:template><axsl:template match="text()" priority="-1" mode="M309"/><axsl:template match="@*|node()" priority="-2" mode="M309"><axsl:apply-templates select="@*|*" mode="M309"/></axsl:template>

<!--PATTERN unixfilestegroup_id-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:group_id" priority="1000" mode="M310">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group_id entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M310"/></axsl:template><axsl:template match="text()" priority="-1" mode="M310"/><axsl:template match="@*|node()" priority="-2" mode="M310"><axsl:apply-templates select="@*|*" mode="M310"/></axsl:template>

<!--PATTERN unixfilesteuser_id-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:user_id" priority="1000" mode="M311">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M311"/></axsl:template><axsl:template match="text()" priority="-1" mode="M311"/><axsl:template match="@*|node()" priority="-2" mode="M311"><axsl:apply-templates select="@*|*" mode="M311"/></axsl:template>

<!--PATTERN unixfilestea_time-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:a_time" priority="1000" mode="M312">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the a_time entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M312"/></axsl:template><axsl:template match="text()" priority="-1" mode="M312"/><axsl:template match="@*|node()" priority="-2" mode="M312"><axsl:apply-templates select="@*|*" mode="M312"/></axsl:template>

<!--PATTERN unixfilestec_time-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:c_time" priority="1000" mode="M313">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the c_time entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M313"/></axsl:template><axsl:template match="text()" priority="-1" mode="M313"/><axsl:template match="@*|node()" priority="-2" mode="M313"><axsl:apply-templates select="@*|*" mode="M313"/></axsl:template>

<!--PATTERN unixfilestem_time-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:m_time" priority="1000" mode="M314">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the m_time entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M314"/></axsl:template><axsl:template match="text()" priority="-1" mode="M314"/><axsl:template match="@*|node()" priority="-2" mode="M314"><axsl:apply-templates select="@*|*" mode="M314"/></axsl:template>

<!--PATTERN unixfilestesize-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:size" priority="1000" mode="M315">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the size entity of a file_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M315"/></axsl:template><axsl:template match="text()" priority="-1" mode="M315"/><axsl:template match="@*|node()" priority="-2" mode="M315"><axsl:apply-templates select="@*|*" mode="M315"/></axsl:template>

<!--PATTERN unixfilestesuid-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:suid" priority="1000" mode="M316">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the suid entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M316"/></axsl:template><axsl:template match="text()" priority="-1" mode="M316"/><axsl:template match="@*|node()" priority="-2" mode="M316"><axsl:apply-templates select="@*|*" mode="M316"/></axsl:template>

<!--PATTERN unixfilestesgid-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:sgid" priority="1000" mode="M317">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sgid entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M317"/></axsl:template><axsl:template match="text()" priority="-1" mode="M317"/><axsl:template match="@*|node()" priority="-2" mode="M317"><axsl:apply-templates select="@*|*" mode="M317"/></axsl:template>

<!--PATTERN unixfilestesticky-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:sticky" priority="1000" mode="M318">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sticky entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M318"/></axsl:template><axsl:template match="text()" priority="-1" mode="M318"/><axsl:template match="@*|node()" priority="-2" mode="M318"><axsl:apply-templates select="@*|*" mode="M318"/></axsl:template>

<!--PATTERN unixfilesteuread-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:uread" priority="1000" mode="M319">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uread entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M319"/></axsl:template><axsl:template match="text()" priority="-1" mode="M319"/><axsl:template match="@*|node()" priority="-2" mode="M319"><axsl:apply-templates select="@*|*" mode="M319"/></axsl:template>

<!--PATTERN unixfilesteuwrite-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:uwrite" priority="1000" mode="M320">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uwrite entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M320"/></axsl:template><axsl:template match="text()" priority="-1" mode="M320"/><axsl:template match="@*|node()" priority="-2" mode="M320"><axsl:apply-templates select="@*|*" mode="M320"/></axsl:template>

<!--PATTERN unixfilesteuexec-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:uexec" priority="1000" mode="M321">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uexec entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M321"/></axsl:template><axsl:template match="text()" priority="-1" mode="M321"/><axsl:template match="@*|node()" priority="-2" mode="M321"><axsl:apply-templates select="@*|*" mode="M321"/></axsl:template>

<!--PATTERN unixfilestegread-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:gread" priority="1000" mode="M322">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gread entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M322"/></axsl:template><axsl:template match="text()" priority="-1" mode="M322"/><axsl:template match="@*|node()" priority="-2" mode="M322"><axsl:apply-templates select="@*|*" mode="M322"/></axsl:template>

<!--PATTERN unixfilestegwrite-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:gwrite" priority="1000" mode="M323">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gwrite entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M323"/></axsl:template><axsl:template match="text()" priority="-1" mode="M323"/><axsl:template match="@*|node()" priority="-2" mode="M323"><axsl:apply-templates select="@*|*" mode="M323"/></axsl:template>

<!--PATTERN unixfilestegexec-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:gexec" priority="1000" mode="M324">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gexec entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M324"/></axsl:template><axsl:template match="text()" priority="-1" mode="M324"/><axsl:template match="@*|node()" priority="-2" mode="M324"><axsl:apply-templates select="@*|*" mode="M324"/></axsl:template>

<!--PATTERN unixfilesteoread-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:oread" priority="1000" mode="M325">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the oread entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M325"/></axsl:template><axsl:template match="text()" priority="-1" mode="M325"/><axsl:template match="@*|node()" priority="-2" mode="M325"><axsl:apply-templates select="@*|*" mode="M325"/></axsl:template>

<!--PATTERN unixfilesteowrite-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:owrite" priority="1000" mode="M326">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the owrite entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M326"/></axsl:template><axsl:template match="text()" priority="-1" mode="M326"/><axsl:template match="@*|node()" priority="-2" mode="M326"><axsl:apply-templates select="@*|*" mode="M326"/></axsl:template>

<!--PATTERN unixfilesteoexec-->


	<!--RULE -->
<axsl:template match="unix-def:file_state/unix-def:oexec" priority="1000" mode="M327">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the oexec entity of a file_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M327"/></axsl:template><axsl:template match="text()" priority="-1" mode="M327"/><axsl:template match="@*|node()" priority="-2" mode="M327"><axsl:apply-templates select="@*|*" mode="M327"/></axsl:template>

<!--PATTERN inetdobjprotocol-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_object/unix-def:protocol" priority="1000" mode="M328">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an inetd_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M328"/></axsl:template><axsl:template match="text()" priority="-1" mode="M328"/><axsl:template match="@*|node()" priority="-2" mode="M328"><axsl:apply-templates select="@*|*" mode="M328"/></axsl:template>

<!--PATTERN inetdobjservice_name-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_object/unix-def:service_name" priority="1000" mode="M329">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of an inetd_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M329"/></axsl:template><axsl:template match="text()" priority="-1" mode="M329"/><axsl:template match="@*|node()" priority="-2" mode="M329"><axsl:apply-templates select="@*|*" mode="M329"/></axsl:template>

<!--PATTERN inetdsteprotocol-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_state/unix-def:protocol" priority="1000" mode="M330">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an inetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M330"/></axsl:template><axsl:template match="text()" priority="-1" mode="M330"/><axsl:template match="@*|node()" priority="-2" mode="M330"><axsl:apply-templates select="@*|*" mode="M330"/></axsl:template>

<!--PATTERN inetdsteservice_name-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_state/unix-def:service_name" priority="1000" mode="M331">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of an inetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M331"/></axsl:template><axsl:template match="text()" priority="-1" mode="M331"/><axsl:template match="@*|node()" priority="-2" mode="M331"><axsl:apply-templates select="@*|*" mode="M331"/></axsl:template>

<!--PATTERN inetdsteserver_program-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_state/unix-def:server_program" priority="1000" mode="M332">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_program entity of an inetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M332"/></axsl:template><axsl:template match="text()" priority="-1" mode="M332"/><axsl:template match="@*|node()" priority="-2" mode="M332"><axsl:apply-templates select="@*|*" mode="M332"/></axsl:template>

<!--PATTERN inetdsteserver_arguments-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_state/unix-def:server_arguments" priority="1000" mode="M333">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_arguments entity of an inetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M333"/></axsl:template><axsl:template match="text()" priority="-1" mode="M333"/><axsl:template match="@*|node()" priority="-2" mode="M333"><axsl:apply-templates select="@*|*" mode="M333"/></axsl:template>

<!--PATTERN inetdsteendpoint_type-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_state/unix-def:endpoint_type" priority="1000" mode="M334">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the endpoint_type entity of an inetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M334"/></axsl:template><axsl:template match="text()" priority="-1" mode="M334"/><axsl:template match="@*|node()" priority="-2" mode="M334"><axsl:apply-templates select="@*|*" mode="M334"/></axsl:template>

<!--PATTERN inetdsteexec_as_user-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_state/unix-def:exec_as_user" priority="1000" mode="M335">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exec_as_user entity of an inetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M335"/></axsl:template><axsl:template match="text()" priority="-1" mode="M335"/><axsl:template match="@*|node()" priority="-2" mode="M335"><axsl:apply-templates select="@*|*" mode="M335"/></axsl:template>

<!--PATTERN inetdstewait_status-->


	<!--RULE -->
<axsl:template match="unix-def:inetd_state/unix-def:wait_status" priority="1000" mode="M336">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the wait_status entity of an inetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M336"/></axsl:template><axsl:template match="text()" priority="-1" mode="M336"/><axsl:template match="@*|node()" priority="-2" mode="M336"><axsl:apply-templates select="@*|*" mode="M336"/></axsl:template>

<!--PATTERN unixinterfaceobjname-->


	<!--RULE -->
<axsl:template match="unix-def:interface_object/unix-def:name" priority="1000" mode="M337">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an interface_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M337"/></axsl:template><axsl:template match="text()" priority="-1" mode="M337"/><axsl:template match="@*|node()" priority="-2" mode="M337"><axsl:apply-templates select="@*|*" mode="M337"/></axsl:template>

<!--PATTERN unixinterfacestename-->


	<!--RULE -->
<axsl:template match="unix-def:interface_state/unix-def:name" priority="1000" mode="M338">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M338"/></axsl:template><axsl:template match="text()" priority="-1" mode="M338"/><axsl:template match="@*|node()" priority="-2" mode="M338"><axsl:apply-templates select="@*|*" mode="M338"/></axsl:template>

<!--PATTERN unixinterfacestehardware_addr-->


	<!--RULE -->
<axsl:template match="unix-def:interface_state/unix-def:hardware_addr" priority="1000" mode="M339">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hardware_addr entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M339"/></axsl:template><axsl:template match="text()" priority="-1" mode="M339"/><axsl:template match="@*|node()" priority="-2" mode="M339"><axsl:apply-templates select="@*|*" mode="M339"/></axsl:template>

<!--PATTERN unixinterfacesteinet_addr-->


	<!--RULE -->
<axsl:template match="unix-def:interface_state/unix-def:inet_addr" priority="1000" mode="M340">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the inet_addr entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M340"/></axsl:template><axsl:template match="text()" priority="-1" mode="M340"/><axsl:template match="@*|node()" priority="-2" mode="M340"><axsl:apply-templates select="@*|*" mode="M340"/></axsl:template>

<!--PATTERN unixinterfacestebroadcast_addr-->


	<!--RULE -->
<axsl:template match="unix-def:interface_state/unix-def:broadcast_addr" priority="1000" mode="M341">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the broadcast_addr entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M341"/></axsl:template><axsl:template match="text()" priority="-1" mode="M341"/><axsl:template match="@*|node()" priority="-2" mode="M341"><axsl:apply-templates select="@*|*" mode="M341"/></axsl:template>

<!--PATTERN unixinterfacestenetmask-->


	<!--RULE -->
<axsl:template match="unix-def:interface_state/unix-def:netmask" priority="1000" mode="M342">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the netmask entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M342"/></axsl:template><axsl:template match="text()" priority="-1" mode="M342"/><axsl:template match="@*|node()" priority="-2" mode="M342"><axsl:apply-templates select="@*|*" mode="M342"/></axsl:template>

<!--PATTERN unixinterfacesteflag-->


	<!--RULE -->
<axsl:template match="unix-def:interface_state/unix-def:flag" priority="1000" mode="M343">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for a flag entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M343"/></axsl:template><axsl:template match="text()" priority="-1" mode="M343"/><axsl:template match="@*|node()" priority="-2" mode="M343"><axsl:apply-templates select="@*|*" mode="M343"/></axsl:template>

<!--PATTERN passwordobjusername-->


	<!--RULE -->
<axsl:template match="unix-def:password_object/unix-def:username" priority="1000" mode="M344">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a password_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M344"/></axsl:template><axsl:template match="text()" priority="-1" mode="M344"/><axsl:template match="@*|node()" priority="-2" mode="M344"><axsl:apply-templates select="@*|*" mode="M344"/></axsl:template>

<!--PATTERN passwordsteusername-->


	<!--RULE -->
<axsl:template match="unix-def:package_state/unix-def:username" priority="1000" mode="M345">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a password_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M345"/></axsl:template><axsl:template match="text()" priority="-1" mode="M345"/><axsl:template match="@*|node()" priority="-2" mode="M345"><axsl:apply-templates select="@*|*" mode="M345"/></axsl:template>

<!--PATTERN passwordstepassword-->


	<!--RULE -->
<axsl:template match="unix-def:package_state/unix-def:password" priority="1000" mode="M346">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password entity of a password_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M346"/></axsl:template><axsl:template match="text()" priority="-1" mode="M346"/><axsl:template match="@*|node()" priority="-2" mode="M346"><axsl:apply-templates select="@*|*" mode="M346"/></axsl:template>

<!--PATTERN passwordsteuser_id-->


	<!--RULE -->
<axsl:template match="unix-def:package_state/unix-def:user_id" priority="1000" mode="M347">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of a password_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M347"/></axsl:template><axsl:template match="text()" priority="-1" mode="M347"/><axsl:template match="@*|node()" priority="-2" mode="M347"><axsl:apply-templates select="@*|*" mode="M347"/></axsl:template>

<!--PATTERN passwordstegroup_id-->


	<!--RULE -->
<axsl:template match="unix-def:package_state/unix-def:group_id" priority="1000" mode="M348">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group_id entity of a password_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M348"/></axsl:template><axsl:template match="text()" priority="-1" mode="M348"/><axsl:template match="@*|node()" priority="-2" mode="M348"><axsl:apply-templates select="@*|*" mode="M348"/></axsl:template>

<!--PATTERN passwordstegcos-->


	<!--RULE -->
<axsl:template match="unix-def:package_state/unix-def:gcos" priority="1000" mode="M349">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gcos entity of a password_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M349"/></axsl:template><axsl:template match="text()" priority="-1" mode="M349"/><axsl:template match="@*|node()" priority="-2" mode="M349"><axsl:apply-templates select="@*|*" mode="M349"/></axsl:template>

<!--PATTERN passwordstehome_dir-->


	<!--RULE -->
<axsl:template match="unix-def:package_state/unix-def:home_dir" priority="1000" mode="M350">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the home_dir entity of a password_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M350"/></axsl:template><axsl:template match="text()" priority="-1" mode="M350"/><axsl:template match="@*|node()" priority="-2" mode="M350"><axsl:apply-templates select="@*|*" mode="M350"/></axsl:template>

<!--PATTERN passwordstelogin_shell-->


	<!--RULE -->
<axsl:template match="unix-def:package_state/unix-def:login_shell" priority="1000" mode="M351">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_shell entity of a password_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M351"/></axsl:template><axsl:template match="text()" priority="-1" mode="M351"/><axsl:template match="@*|node()" priority="-2" mode="M351"><axsl:apply-templates select="@*|*" mode="M351"/></axsl:template>

<!--PATTERN unixprocessobjcommand-->


	<!--RULE -->
<axsl:template match="unix-def:process_object/unix-def:command" priority="1000" mode="M352">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the command entity of a process_object be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M352"/></axsl:template><axsl:template match="text()" priority="-1" mode="M352"/><axsl:template match="@*|node()" priority="-2" mode="M352"><axsl:apply-templates select="@*|*" mode="M352"/></axsl:template>

<!--PATTERN unixprocessstecommand-->


	<!--RULE -->
<axsl:template match="unix-def:process_state/unix-def:command" priority="1000" mode="M353">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the command entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M353"/></axsl:template><axsl:template match="text()" priority="-1" mode="M353"/><axsl:template match="@*|node()" priority="-2" mode="M353"><axsl:apply-templates select="@*|*" mode="M353"/></axsl:template>

<!--PATTERN unixprocesssteexec_time-->


	<!--RULE -->
<axsl:template match="unix-def:process_state/unix-def:exec_time" priority="1000" mode="M354">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exec_time entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M354"/></axsl:template><axsl:template match="text()" priority="-1" mode="M354"/><axsl:template match="@*|node()" priority="-2" mode="M354"><axsl:apply-templates select="@*|*" mode="M354"/></axsl:template>

<!--PATTERN unixprocessstepid-->


	<!--RULE -->
<axsl:template match="unix-def:process_state/unix-def:pid" priority="1000" mode="M355">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of a process_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M355"/></axsl:template><axsl:template match="text()" priority="-1" mode="M355"/><axsl:template match="@*|node()" priority="-2" mode="M355"><axsl:apply-templates select="@*|*" mode="M355"/></axsl:template>

<!--PATTERN unixprocesssteppid-->


	<!--RULE -->
<axsl:template match="unix-def:process_state/unix-def:ppid" priority="1000" mode="M356">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ppid entity of a process_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M356"/></axsl:template><axsl:template match="text()" priority="-1" mode="M356"/><axsl:template match="@*|node()" priority="-2" mode="M356"><axsl:apply-templates select="@*|*" mode="M356"/></axsl:template>

<!--PATTERN unixprocessstepriority-->


	<!--RULE -->
<axsl:template match="unix-def:process_state/unix-def:priority" priority="1000" mode="M357">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the priority entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M357"/></axsl:template><axsl:template match="text()" priority="-1" mode="M357"/><axsl:template match="@*|node()" priority="-2" mode="M357"><axsl:apply-templates select="@*|*" mode="M357"/></axsl:template>

<!--PATTERN unixprocessstescheduling_class-->


	<!--RULE -->
<axsl:template match="unix-def:process_state/unix-def:scheduling_class" priority="1000" mode="M358">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the scheduling_class entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M358"/></axsl:template><axsl:template match="text()" priority="-1" mode="M358"/><axsl:template match="@*|node()" priority="-2" mode="M358"><axsl:apply-templates select="@*|*" mode="M358"/></axsl:template>

<!--PATTERN unixprocessstestart_time-->


	<!--RULE -->
<axsl:template match="unix-def:process_state/unix-def:start_time" priority="1000" mode="M359">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the start_time entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M359"/></axsl:template><axsl:template match="text()" priority="-1" mode="M359"/><axsl:template match="@*|node()" priority="-2" mode="M359"><axsl:apply-templates select="@*|*" mode="M359"/></axsl:template>

<!--PATTERN unixprocessstetty-->


	<!--RULE -->
<axsl:template match="unix-def:process_state/unix-def:tty" priority="1000" mode="M360">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the tty entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M360"/></axsl:template><axsl:template match="text()" priority="-1" mode="M360"/><axsl:template match="@*|node()" priority="-2" mode="M360"><axsl:apply-templates select="@*|*" mode="M360"/></axsl:template>

<!--PATTERN unixprocesssteuser_id-->


	<!--RULE -->
<axsl:template match="unix-def:process_state/unix-def:user_id" priority="1000" mode="M361">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M361"/></axsl:template><axsl:template match="text()" priority="-1" mode="M361"/><axsl:template match="@*|node()" priority="-2" mode="M361"><axsl:apply-templates select="@*|*" mode="M361"/></axsl:template>

<!--PATTERN unixrlobjservice_name-->


	<!--RULE -->
<axsl:template match="unix-def:runlevel_object/unix-def:service_name" priority="1000" mode="M362">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of a runlevel_object be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M362"/></axsl:template><axsl:template match="text()" priority="-1" mode="M362"/><axsl:template match="@*|node()" priority="-2" mode="M362"><axsl:apply-templates select="@*|*" mode="M362"/></axsl:template>

<!--PATTERN unixrlobjrunlevel-->


	<!--RULE -->
<axsl:template match="unix-def:runlevel_object/unix-def:runlevel" priority="1000" mode="M363">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the runlevel entity of a runlevel_object be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M363"/></axsl:template><axsl:template match="text()" priority="-1" mode="M363"/><axsl:template match="@*|node()" priority="-2" mode="M363"><axsl:apply-templates select="@*|*" mode="M363"/></axsl:template>

<!--PATTERN unixrlsteservice_name-->


	<!--RULE -->
<axsl:template match="unix-def:runlevel_state/unix-def:service_name" priority="1000" mode="M364">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of a runlevel_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M364"/></axsl:template><axsl:template match="text()" priority="-1" mode="M364"/><axsl:template match="@*|node()" priority="-2" mode="M364"><axsl:apply-templates select="@*|*" mode="M364"/></axsl:template>

<!--PATTERN unixrlsterunlevel-->


	<!--RULE -->
<axsl:template match="unix-def:runlevel_state/unix-def:runlevel" priority="1000" mode="M365">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the runlevel entity of a runlevel_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M365"/></axsl:template><axsl:template match="text()" priority="-1" mode="M365"/><axsl:template match="@*|node()" priority="-2" mode="M365"><axsl:apply-templates select="@*|*" mode="M365"/></axsl:template>

<!--PATTERN unixrlstestart-->


	<!--RULE -->
<axsl:template match="unix-def:runlevel_state/unix-def:start" priority="1000" mode="M366">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the start entity of a runlevel_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M366"/></axsl:template><axsl:template match="text()" priority="-1" mode="M366"/><axsl:template match="@*|node()" priority="-2" mode="M366"><axsl:apply-templates select="@*|*" mode="M366"/></axsl:template>

<!--PATTERN unixrlstekill-->


	<!--RULE -->
<axsl:template match="unix-def:runlevel_state/unix-def:kill" priority="1000" mode="M367">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the kill entity of a runlevel_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M367"/></axsl:template><axsl:template match="text()" priority="-1" mode="M367"/><axsl:template match="@*|node()" priority="-2" mode="M367"><axsl:apply-templates select="@*|*" mode="M367"/></axsl:template>

<!--PATTERN sccsobjpath-->


	<!--RULE -->
<axsl:template match="unix-def:file_object/unix-def:path" priority="1000" mode="M368">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a sccs_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M368"/></axsl:template><axsl:template match="text()" priority="-1" mode="M368"/><axsl:template match="@*|node()" priority="-2" mode="M368"><axsl:apply-templates select="@*|*" mode="M368"/></axsl:template>

<!--PATTERN sccsobjfilename-->


	<!--RULE -->
<axsl:template match="unix-def:file_object/unix-def:filename" priority="1000" mode="M369">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a sccs_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M369"/></axsl:template><axsl:template match="text()" priority="-1" mode="M369"/><axsl:template match="@*|node()" priority="-2" mode="M369"><axsl:apply-templates select="@*|*" mode="M369"/></axsl:template>

<!--PATTERN sccsstepath-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_state/unix-def:path" priority="1000" mode="M370">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a sccs_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M370"/></axsl:template><axsl:template match="text()" priority="-1" mode="M370"/><axsl:template match="@*|node()" priority="-2" mode="M370"><axsl:apply-templates select="@*|*" mode="M370"/></axsl:template>

<!--PATTERN sccsstefilename-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_state/unix-def:filename" priority="1000" mode="M371">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a sccs_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M371"/></axsl:template><axsl:template match="text()" priority="-1" mode="M371"/><axsl:template match="@*|node()" priority="-2" mode="M371"><axsl:apply-templates select="@*|*" mode="M371"/></axsl:template>

<!--PATTERN sccsstemodule_name-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_state/unix-def:module_name" priority="1000" mode="M372">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the module_name entity of a sccs_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M372"/></axsl:template><axsl:template match="text()" priority="-1" mode="M372"/><axsl:template match="@*|node()" priority="-2" mode="M372"><axsl:apply-templates select="@*|*" mode="M372"/></axsl:template>

<!--PATTERN sccsstemodule_type-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_state/unix-def:module_type" priority="1000" mode="M373">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the module_type entity of a sccs_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M373"/></axsl:template><axsl:template match="text()" priority="-1" mode="M373"/><axsl:template match="@*|node()" priority="-2" mode="M373"><axsl:apply-templates select="@*|*" mode="M373"/></axsl:template>

<!--PATTERN sccssterelease-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_state/unix-def:release" priority="1000" mode="M374">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the release entity of a sccs_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M374"/></axsl:template><axsl:template match="text()" priority="-1" mode="M374"/><axsl:template match="@*|node()" priority="-2" mode="M374"><axsl:apply-templates select="@*|*" mode="M374"/></axsl:template>

<!--PATTERN sccsstelevel-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_state/unix-def:level" priority="1000" mode="M375">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the level entity of a sccs_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M375"/></axsl:template><axsl:template match="text()" priority="-1" mode="M375"/><axsl:template match="@*|node()" priority="-2" mode="M375"><axsl:apply-templates select="@*|*" mode="M375"/></axsl:template>

<!--PATTERN sccsstebranch-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_state/unix-def:branch" priority="1000" mode="M376">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the branch entity of a sccs_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M376"/></axsl:template><axsl:template match="text()" priority="-1" mode="M376"/><axsl:template match="@*|node()" priority="-2" mode="M376"><axsl:apply-templates select="@*|*" mode="M376"/></axsl:template>

<!--PATTERN sccsstesequence-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_state/unix-def:sequence" priority="1000" mode="M377">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sequence entity of a sccs_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M377"/></axsl:template><axsl:template match="text()" priority="-1" mode="M377"/><axsl:template match="@*|node()" priority="-2" mode="M377"><axsl:apply-templates select="@*|*" mode="M377"/></axsl:template>

<!--PATTERN sccsstewhat_string-->


	<!--RULE -->
<axsl:template match="unix-def:sccs_state/unix-def:what_string" priority="1000" mode="M378">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the what_string entity of a sccs_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M378"/></axsl:template><axsl:template match="text()" priority="-1" mode="M378"/><axsl:template match="@*|node()" priority="-2" mode="M378"><axsl:apply-templates select="@*|*" mode="M378"/></axsl:template>

<!--PATTERN shadowobjusername-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_object/unix-def:username" priority="1000" mode="M379">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a shadow_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M379"/></axsl:template><axsl:template match="text()" priority="-1" mode="M379"/><axsl:template match="@*|node()" priority="-2" mode="M379"><axsl:apply-templates select="@*|*" mode="M379"/></axsl:template>

<!--PATTERN shadowsteusername-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_state/unix-def:username" priority="1000" mode="M380">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a shadow_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M380"/></axsl:template><axsl:template match="text()" priority="-1" mode="M380"/><axsl:template match="@*|node()" priority="-2" mode="M380"><axsl:apply-templates select="@*|*" mode="M380"/></axsl:template>

<!--PATTERN shadowstepassword-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_state/unix-def:password" priority="1000" mode="M381">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password entity of a shadow_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M381"/></axsl:template><axsl:template match="text()" priority="-1" mode="M381"/><axsl:template match="@*|node()" priority="-2" mode="M381"><axsl:apply-templates select="@*|*" mode="M381"/></axsl:template>

<!--PATTERN shadowstechg_lst-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_state/unix-def:chg_lst" priority="1000" mode="M382">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the chg_lst entity of a shadow_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M382"/></axsl:template><axsl:template match="text()" priority="-1" mode="M382"/><axsl:template match="@*|node()" priority="-2" mode="M382"><axsl:apply-templates select="@*|*" mode="M382"/></axsl:template>

<!--PATTERN shadowstechg_allow-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_state/unix-def:chg_allow" priority="1000" mode="M383">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the chg_allow entity of a shadow_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M383"/></axsl:template><axsl:template match="text()" priority="-1" mode="M383"/><axsl:template match="@*|node()" priority="-2" mode="M383"><axsl:apply-templates select="@*|*" mode="M383"/></axsl:template>

<!--PATTERN shadowstechg_req-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_state/unix-def:chg_req" priority="1000" mode="M384">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the chg_req entity of a shadow_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M384"/></axsl:template><axsl:template match="text()" priority="-1" mode="M384"/><axsl:template match="@*|node()" priority="-2" mode="M384"><axsl:apply-templates select="@*|*" mode="M384"/></axsl:template>

<!--PATTERN shadowsteexp_warn-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_state/unix-def:exp_warn" priority="1000" mode="M385">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exp_warn entity of a shadow_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M385"/></axsl:template><axsl:template match="text()" priority="-1" mode="M385"/><axsl:template match="@*|node()" priority="-2" mode="M385"><axsl:apply-templates select="@*|*" mode="M385"/></axsl:template>

<!--PATTERN shadowsteexp_inact-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_state/unix-def:exp_inact" priority="1000" mode="M386">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exp_inact entity of a shadow_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M386"/></axsl:template><axsl:template match="text()" priority="-1" mode="M386"/><axsl:template match="@*|node()" priority="-2" mode="M386"><axsl:apply-templates select="@*|*" mode="M386"/></axsl:template>

<!--PATTERN shadowsteexp_date-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_state/unix-def:exp_date" priority="1000" mode="M387">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exp_date entity of a shadow_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M387"/></axsl:template><axsl:template match="text()" priority="-1" mode="M387"/><axsl:template match="@*|node()" priority="-2" mode="M387"><axsl:apply-templates select="@*|*" mode="M387"/></axsl:template>

<!--PATTERN shadowsteflag-->


	<!--RULE -->
<axsl:template match="unix-def:shadow_state/unix-def:flag" priority="1000" mode="M388">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the flag entity of a shadow_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M388"/></axsl:template><axsl:template match="text()" priority="-1" mode="M388"/><axsl:template match="@*|node()" priority="-2" mode="M388"><axsl:apply-templates select="@*|*" mode="M388"/></axsl:template>

<!--PATTERN unamestemachine_class-->


	<!--RULE -->
<axsl:template match="unix-def:uname_state/unix-def:machine_class" priority="1000" mode="M389">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the machine_class entity of a uname_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M389"/></axsl:template><axsl:template match="text()" priority="-1" mode="M389"/><axsl:template match="@*|node()" priority="-2" mode="M389"><axsl:apply-templates select="@*|*" mode="M389"/></axsl:template>

<!--PATTERN unamestenode_name-->


	<!--RULE -->
<axsl:template match="unix-def:uname_state/unix-def:node_name" priority="1000" mode="M390">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the node_name entity of a uname_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M390"/></axsl:template><axsl:template match="text()" priority="-1" mode="M390"/><axsl:template match="@*|node()" priority="-2" mode="M390"><axsl:apply-templates select="@*|*" mode="M390"/></axsl:template>

<!--PATTERN unamesteos_name-->


	<!--RULE -->
<axsl:template match="unix-def:uname_state/unix-def:os_name" priority="1000" mode="M391">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the os_name entity of a uname_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M391"/></axsl:template><axsl:template match="text()" priority="-1" mode="M391"/><axsl:template match="@*|node()" priority="-2" mode="M391"><axsl:apply-templates select="@*|*" mode="M391"/></axsl:template>

<!--PATTERN unamesteos_release-->


	<!--RULE -->
<axsl:template match="unix-def:uname_state/unix-def:os_release" priority="1000" mode="M392">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the os_release entity of a uname_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M392"/></axsl:template><axsl:template match="text()" priority="-1" mode="M392"/><axsl:template match="@*|node()" priority="-2" mode="M392"><axsl:apply-templates select="@*|*" mode="M392"/></axsl:template>

<!--PATTERN unamesteos_version-->


	<!--RULE -->
<axsl:template match="unix-def:uname_state/unix-def:os_version" priority="1000" mode="M393">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the os_version entity of a uname_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M393"/></axsl:template><axsl:template match="text()" priority="-1" mode="M393"/><axsl:template match="@*|node()" priority="-2" mode="M393"><axsl:apply-templates select="@*|*" mode="M393"/></axsl:template>

<!--PATTERN unamesteprocessor_type-->


	<!--RULE -->
<axsl:template match="unix-def:uname_state/unix-def:processor_type" priority="1000" mode="M394">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the processor_type entity of a uname_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M394"/></axsl:template><axsl:template match="text()" priority="-1" mode="M394"/><axsl:template match="@*|node()" priority="-2" mode="M394"><axsl:apply-templates select="@*|*" mode="M394"/></axsl:template>

<!--PATTERN xinetdobjprotocol-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_object/unix-def:protocol" priority="1000" mode="M395">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an xinetd_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M395"/></axsl:template><axsl:template match="text()" priority="-1" mode="M395"/><axsl:template match="@*|node()" priority="-2" mode="M395"><axsl:apply-templates select="@*|*" mode="M395"/></axsl:template>

<!--PATTERN xinetdobjservice_name-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_object/unix-def:service_name" priority="1000" mode="M396">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of an xinetd_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M396"/></axsl:template><axsl:template match="text()" priority="-1" mode="M396"/><axsl:template match="@*|node()" priority="-2" mode="M396"><axsl:apply-templates select="@*|*" mode="M396"/></axsl:template>

<!--PATTERN xinetdsteprotocol-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:protocol" priority="1000" mode="M397">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M397"/></axsl:template><axsl:template match="text()" priority="-1" mode="M397"/><axsl:template match="@*|node()" priority="-2" mode="M397"><axsl:apply-templates select="@*|*" mode="M397"/></axsl:template>

<!--PATTERN xinetdsteservice_name-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:service_name" priority="1000" mode="M398">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M398"/></axsl:template><axsl:template match="text()" priority="-1" mode="M398"/><axsl:template match="@*|node()" priority="-2" mode="M398"><axsl:apply-templates select="@*|*" mode="M398"/></axsl:template>

<!--PATTERN xinetdsteflags-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:flags" priority="1000" mode="M399">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the flags entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M399"/></axsl:template><axsl:template match="text()" priority="-1" mode="M399"/><axsl:template match="@*|node()" priority="-2" mode="M399"><axsl:apply-templates select="@*|*" mode="M399"/></axsl:template>

<!--PATTERN xinetdstenoaccess-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:no_access" priority="1000" mode="M400">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the no_access entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M400"/></axsl:template><axsl:template match="text()" priority="-1" mode="M400"/><axsl:template match="@*|node()" priority="-2" mode="M400"><axsl:apply-templates select="@*|*" mode="M400"/></axsl:template>

<!--PATTERN xinetdsteonlyfrom-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:only_from" priority="1000" mode="M401">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the only_from entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M401"/></axsl:template><axsl:template match="text()" priority="-1" mode="M401"/><axsl:template match="@*|node()" priority="-2" mode="M401"><axsl:apply-templates select="@*|*" mode="M401"/></axsl:template>

<!--PATTERN xinetdsteport-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:port" priority="1000" mode="M402">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the port entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M402"/></axsl:template><axsl:template match="text()" priority="-1" mode="M402"/><axsl:template match="@*|node()" priority="-2" mode="M402"><axsl:apply-templates select="@*|*" mode="M402"/></axsl:template>

<!--PATTERN xinetdsteserver-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:server" priority="1000" mode="M403">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M403"/></axsl:template><axsl:template match="text()" priority="-1" mode="M403"/><axsl:template match="@*|node()" priority="-2" mode="M403"><axsl:apply-templates select="@*|*" mode="M403"/></axsl:template>

<!--PATTERN xinetdsteserverarguments-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:server_arguments" priority="1000" mode="M404">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_arguments entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M404"/></axsl:template><axsl:template match="text()" priority="-1" mode="M404"/><axsl:template match="@*|node()" priority="-2" mode="M404"><axsl:apply-templates select="@*|*" mode="M404"/></axsl:template>

<!--PATTERN xinetdstesockettype-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:socket_type" priority="1000" mode="M405">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the socket_type entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M405"/></axsl:template><axsl:template match="text()" priority="-1" mode="M405"/><axsl:template match="@*|node()" priority="-2" mode="M405"><axsl:apply-templates select="@*|*" mode="M405"/></axsl:template>

<!--PATTERN xinetdstetype-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:type" priority="1000" mode="M406">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M406"/></axsl:template><axsl:template match="text()" priority="-1" mode="M406"/><axsl:template match="@*|node()" priority="-2" mode="M406"><axsl:apply-templates select="@*|*" mode="M406"/></axsl:template>

<!--PATTERN xinetdsteuser-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:user" priority="1000" mode="M407">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user entity of an xinetd_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M407"/></axsl:template><axsl:template match="text()" priority="-1" mode="M407"/><axsl:template match="@*|node()" priority="-2" mode="M407"><axsl:apply-templates select="@*|*" mode="M407"/></axsl:template>

<!--PATTERN xinetdstewait-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:wait" priority="1000" mode="M408">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the wait entity of an xinetd_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M408"/></axsl:template><axsl:template match="text()" priority="-1" mode="M408"/><axsl:template match="@*|node()" priority="-2" mode="M408"><axsl:apply-templates select="@*|*" mode="M408"/></axsl:template>

<!--PATTERN xinetdstedisabled-->


	<!--RULE -->
<axsl:template match="unix-def:xinetd_state/unix-def:disabled" priority="1000" mode="M409">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the disabled entity of an xinetd_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M409"/></axsl:template><axsl:template match="text()" priority="-1" mode="M409"/><axsl:template match="@*|node()" priority="-2" mode="M409"><axsl:apply-templates select="@*|*" mode="M409"/></axsl:template>

<!--PATTERN affected_platform-->


	<!--RULE -->
<axsl:template match="oval-def:affected[@family='windows']" priority="1000" mode="M410">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(oval-def:platform) or oval-def:platform='Microsoft Windows 95' or oval-def:platform='Microsoft Windows 98' or oval-def:platform='Microsoft Windows ME' or oval-def:platform='Microsoft Windows NT' or oval-def:platform='Microsoft Windows 2000' or oval-def:platform='Microsoft Windows XP' or oval-def:platform='Microsoft Windows Server 2003' or oval-def:platform='Microsoft Windows Vista' or oval-def:platform='Microsoft Windows Server 2008'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../../@id"/><axsl:text/> - the value "<axsl:text/><axsl:value-of select="oval-def:platform"/><axsl:text/>" found in platform element as part of the affected element is not a valid windows platform.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M410"/></axsl:template><axsl:template match="text()" priority="-1" mode="M410"/><axsl:template match="@*|node()" priority="-2" mode="M410"><axsl:apply-templates select="@*|*" mode="M410"/></axsl:template>

<!--PATTERN attst-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_test/win-def:object" priority="1001" mode="M411">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:accesstoken_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an accesstoken_test must reference an accesstoken_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M411"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:accesstoken_test/win-def:state" priority="1000" mode="M411">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:accesstoken_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an accesstoken_test must reference an accesstoken_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M411"/></axsl:template><axsl:template match="text()" priority="-1" mode="M411"/><axsl:template match="@*|node()" priority="-2" mode="M411"><axsl:apply-templates select="@*|*" mode="M411"/></axsl:template>

<!--PATTERN atobjsecurity_principle-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_object/win-def:security_principle" priority="1000" mode="M412">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the security_principle entity of an accesstoken_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M412"/></axsl:template><axsl:template match="text()" priority="-1" mode="M412"/><axsl:template match="@*|node()" priority="-2" mode="M412"><axsl:apply-templates select="@*|*" mode="M412"/></axsl:template>

<!--PATTERN atstesecurity_principle-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:security_principle" priority="1000" mode="M413">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the security_principle entity of an accesstoken_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M413"/></axsl:template><axsl:template match="text()" priority="-1" mode="M413"/><axsl:template match="@*|node()" priority="-2" mode="M413"><axsl:apply-templates select="@*|*" mode="M413"/></axsl:template>

<!--PATTERN atsteseassignprimarytokenprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seassignprimarytokenprivilege" priority="1000" mode="M414">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seassignprimarytokenprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M414"/></axsl:template><axsl:template match="text()" priority="-1" mode="M414"/><axsl:template match="@*|node()" priority="-2" mode="M414"><axsl:apply-templates select="@*|*" mode="M414"/></axsl:template>

<!--PATTERN atsteseauditprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seauditprivilege" priority="1000" mode="M415">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seauditprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M415"/></axsl:template><axsl:template match="text()" priority="-1" mode="M415"/><axsl:template match="@*|node()" priority="-2" mode="M415"><axsl:apply-templates select="@*|*" mode="M415"/></axsl:template>

<!--PATTERN atstesebackupprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sebackupprivilege" priority="1000" mode="M416">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sebackupprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M416"/></axsl:template><axsl:template match="text()" priority="-1" mode="M416"/><axsl:template match="@*|node()" priority="-2" mode="M416"><axsl:apply-templates select="@*|*" mode="M416"/></axsl:template>

<!--PATTERN atstesechangenotifyprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sechangenotifyprivilege" priority="1000" mode="M417">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sechangenotifyprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M417"/></axsl:template><axsl:template match="text()" priority="-1" mode="M417"/><axsl:template match="@*|node()" priority="-2" mode="M417"><axsl:apply-templates select="@*|*" mode="M417"/></axsl:template>

<!--PATTERN atstesecreateglobalprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:secreateglobalprivilege" priority="1000" mode="M418">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreateglobalprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M418"/></axsl:template><axsl:template match="text()" priority="-1" mode="M418"/><axsl:template match="@*|node()" priority="-2" mode="M418"><axsl:apply-templates select="@*|*" mode="M418"/></axsl:template>

<!--PATTERN atstesecreatepagefileprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:secreatepagefileprivilege" priority="1000" mode="M419">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreatepagefileprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M419"/></axsl:template><axsl:template match="text()" priority="-1" mode="M419"/><axsl:template match="@*|node()" priority="-2" mode="M419"><axsl:apply-templates select="@*|*" mode="M419"/></axsl:template>

<!--PATTERN atstesecreatepermanentprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:secreatepermanentprivilege" priority="1000" mode="M420">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreatepermanentprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M420"/></axsl:template><axsl:template match="text()" priority="-1" mode="M420"/><axsl:template match="@*|node()" priority="-2" mode="M420"><axsl:apply-templates select="@*|*" mode="M420"/></axsl:template>

<!--PATTERN atstesecreatesymboliclinkprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:secreatesymboliclinkprivilege" priority="1000" mode="M421">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreatesymboliclinkprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M421"/></axsl:template><axsl:template match="text()" priority="-1" mode="M421"/><axsl:template match="@*|node()" priority="-2" mode="M421"><axsl:apply-templates select="@*|*" mode="M421"/></axsl:template>

<!--PATTERN atstesecreatetokenprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:secreatetokenprivilege" priority="1000" mode="M422">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreatetokenprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M422"/></axsl:template><axsl:template match="text()" priority="-1" mode="M422"/><axsl:template match="@*|node()" priority="-2" mode="M422"><axsl:apply-templates select="@*|*" mode="M422"/></axsl:template>

<!--PATTERN atstesedebugprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sedebugprivilege" priority="1000" mode="M423">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedebugprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M423"/></axsl:template><axsl:template match="text()" priority="-1" mode="M423"/><axsl:template match="@*|node()" priority="-2" mode="M423"><axsl:apply-templates select="@*|*" mode="M423"/></axsl:template>

<!--PATTERN atsteseenabledelegationprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seenabledelegationprivilege" priority="1000" mode="M424">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seenabledelegationprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M424"/></axsl:template><axsl:template match="text()" priority="-1" mode="M424"/><axsl:template match="@*|node()" priority="-2" mode="M424"><axsl:apply-templates select="@*|*" mode="M424"/></axsl:template>

<!--PATTERN atsteseimpersonateprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seimpersonateprivilege" priority="1000" mode="M425">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seimpersonateprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M425"/></axsl:template><axsl:template match="text()" priority="-1" mode="M425"/><axsl:template match="@*|node()" priority="-2" mode="M425"><axsl:apply-templates select="@*|*" mode="M425"/></axsl:template>

<!--PATTERN atsteseincreasebasepriorityprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seincreasebasepriorityprivilege" priority="1000" mode="M426">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seincreasebasepriorityprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M426"/></axsl:template><axsl:template match="text()" priority="-1" mode="M426"/><axsl:template match="@*|node()" priority="-2" mode="M426"><axsl:apply-templates select="@*|*" mode="M426"/></axsl:template>

<!--PATTERN atsteseincreasequotaprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seincreasequotaprivilege" priority="1000" mode="M427">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seincreasequotaprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M427"/></axsl:template><axsl:template match="text()" priority="-1" mode="M427"/><axsl:template match="@*|node()" priority="-2" mode="M427"><axsl:apply-templates select="@*|*" mode="M427"/></axsl:template>

<!--PATTERN atsteseincreaseworkingsetprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seincreaseworkingsetprivilege" priority="1000" mode="M428">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seincreaseworkingsetprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M428"/></axsl:template><axsl:template match="text()" priority="-1" mode="M428"/><axsl:template match="@*|node()" priority="-2" mode="M428"><axsl:apply-templates select="@*|*" mode="M428"/></axsl:template>

<!--PATTERN atsteseloaddriverprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seloaddriverprivilege" priority="1000" mode="M429">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seloaddriverprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M429"/></axsl:template><axsl:template match="text()" priority="-1" mode="M429"/><axsl:template match="@*|node()" priority="-2" mode="M429"><axsl:apply-templates select="@*|*" mode="M429"/></axsl:template>

<!--PATTERN atsteselockmemoryprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:selockmemoryprivilege" priority="1000" mode="M430">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the selockmemoryprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M430"/></axsl:template><axsl:template match="text()" priority="-1" mode="M430"/><axsl:template match="@*|node()" priority="-2" mode="M430"><axsl:apply-templates select="@*|*" mode="M430"/></axsl:template>

<!--PATTERN atstesemachineaccountprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:semachineaccountprivilege" priority="1000" mode="M431">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the semachineaccountprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M431"/></axsl:template><axsl:template match="text()" priority="-1" mode="M431"/><axsl:template match="@*|node()" priority="-2" mode="M431"><axsl:apply-templates select="@*|*" mode="M431"/></axsl:template>

<!--PATTERN atstesemanagevolumeprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:semanagevolumeprivilege" priority="1000" mode="M432">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the semanagevolumeprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M432"/></axsl:template><axsl:template match="text()" priority="-1" mode="M432"/><axsl:template match="@*|node()" priority="-2" mode="M432"><axsl:apply-templates select="@*|*" mode="M432"/></axsl:template>

<!--PATTERN atsteseprofilesingleprocessprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seprofilesingleprocessprivilege" priority="1000" mode="M433">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seprofilesingleprocessprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M433"/></axsl:template><axsl:template match="text()" priority="-1" mode="M433"/><axsl:template match="@*|node()" priority="-2" mode="M433"><axsl:apply-templates select="@*|*" mode="M433"/></axsl:template>

<!--PATTERN atsteserelabelprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:serelabelprivilege" priority="1000" mode="M434">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the serelabelprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M434"/></axsl:template><axsl:template match="text()" priority="-1" mode="M434"/><axsl:template match="@*|node()" priority="-2" mode="M434"><axsl:apply-templates select="@*|*" mode="M434"/></axsl:template>

<!--PATTERN atsteseremoteshutdownprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seremoteshutdownprivilege" priority="1000" mode="M435">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seremoteshutdownprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M435"/></axsl:template><axsl:template match="text()" priority="-1" mode="M435"/><axsl:template match="@*|node()" priority="-2" mode="M435"><axsl:apply-templates select="@*|*" mode="M435"/></axsl:template>

<!--PATTERN atsteserestoreprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:serestoreprivilege" priority="1000" mode="M436">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the serestoreprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M436"/></axsl:template><axsl:template match="text()" priority="-1" mode="M436"/><axsl:template match="@*|node()" priority="-2" mode="M436"><axsl:apply-templates select="@*|*" mode="M436"/></axsl:template>

<!--PATTERN atstesesecurityprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sesecurityprivilege" priority="1000" mode="M437">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesecurityprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M437"/></axsl:template><axsl:template match="text()" priority="-1" mode="M437"/><axsl:template match="@*|node()" priority="-2" mode="M437"><axsl:apply-templates select="@*|*" mode="M437"/></axsl:template>

<!--PATTERN atsteseshutdownprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seshutdownprivilege" priority="1000" mode="M438">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seshutdownprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M438"/></axsl:template><axsl:template match="text()" priority="-1" mode="M438"/><axsl:template match="@*|node()" priority="-2" mode="M438"><axsl:apply-templates select="@*|*" mode="M438"/></axsl:template>

<!--PATTERN atstesesyncagentprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sesyncagentprivilege" priority="1000" mode="M439">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesyncagentprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M439"/></axsl:template><axsl:template match="text()" priority="-1" mode="M439"/><axsl:template match="@*|node()" priority="-2" mode="M439"><axsl:apply-templates select="@*|*" mode="M439"/></axsl:template>

<!--PATTERN atstesesystemenvironmentprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sesystemenvironmentprivilege" priority="1000" mode="M440">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesystemenvironmentprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M440"/></axsl:template><axsl:template match="text()" priority="-1" mode="M440"/><axsl:template match="@*|node()" priority="-2" mode="M440"><axsl:apply-templates select="@*|*" mode="M440"/></axsl:template>

<!--PATTERN atstesesystemprofileprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sesystemprofileprivilege" priority="1000" mode="M441">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesystemprofileprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M441"/></axsl:template><axsl:template match="text()" priority="-1" mode="M441"/><axsl:template match="@*|node()" priority="-2" mode="M441"><axsl:apply-templates select="@*|*" mode="M441"/></axsl:template>

<!--PATTERN atstesesystemtimeprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sesystemtimeprivilege" priority="1000" mode="M442">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesystemtimeprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M442"/></axsl:template><axsl:template match="text()" priority="-1" mode="M442"/><axsl:template match="@*|node()" priority="-2" mode="M442"><axsl:apply-templates select="@*|*" mode="M442"/></axsl:template>

<!--PATTERN atstesetakeownershipprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:setakeownershipprivilege" priority="1000" mode="M443">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the setakeownershipprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M443"/></axsl:template><axsl:template match="text()" priority="-1" mode="M443"/><axsl:template match="@*|node()" priority="-2" mode="M443"><axsl:apply-templates select="@*|*" mode="M443"/></axsl:template>

<!--PATTERN atstesetcbprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:setcbprivilege" priority="1000" mode="M444">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the setcbprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M444"/></axsl:template><axsl:template match="text()" priority="-1" mode="M444"/><axsl:template match="@*|node()" priority="-2" mode="M444"><axsl:apply-templates select="@*|*" mode="M444"/></axsl:template>

<!--PATTERN atstesetimezoneprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:setimezoneprivilege" priority="1000" mode="M445">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the setimezoneprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M445"/></axsl:template><axsl:template match="text()" priority="-1" mode="M445"/><axsl:template match="@*|node()" priority="-2" mode="M445"><axsl:apply-templates select="@*|*" mode="M445"/></axsl:template>

<!--PATTERN atsteseundockprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seundockprivilege" priority="1000" mode="M446">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seundockprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M446"/></axsl:template><axsl:template match="text()" priority="-1" mode="M446"/><axsl:template match="@*|node()" priority="-2" mode="M446"><axsl:apply-templates select="@*|*" mode="M446"/></axsl:template>

<!--PATTERN atsteseunsolicitedinputprivilege-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seunsolicitedinputprivilege" priority="1000" mode="M447">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seunsolicitedinputprivilege entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M447"/></axsl:template><axsl:template match="text()" priority="-1" mode="M447"/><axsl:template match="@*|node()" priority="-2" mode="M447"><axsl:apply-templates select="@*|*" mode="M447"/></axsl:template>

<!--PATTERN atstesebatchlogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sebatchlogonright" priority="1000" mode="M448">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sebatchlogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M448"/></axsl:template><axsl:template match="text()" priority="-1" mode="M448"/><axsl:template match="@*|node()" priority="-2" mode="M448"><axsl:apply-templates select="@*|*" mode="M448"/></axsl:template>

<!--PATTERN atsteseinteractivelogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seinteractivelogonright" priority="1000" mode="M449">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seinteractivelogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M449"/></axsl:template><axsl:template match="text()" priority="-1" mode="M449"/><axsl:template match="@*|node()" priority="-2" mode="M449"><axsl:apply-templates select="@*|*" mode="M449"/></axsl:template>

<!--PATTERN atstesenetworklogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:senetworklogonright" priority="1000" mode="M450">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the senetworklogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M450"/></axsl:template><axsl:template match="text()" priority="-1" mode="M450"/><axsl:template match="@*|node()" priority="-2" mode="M450"><axsl:apply-templates select="@*|*" mode="M450"/></axsl:template>

<!--PATTERN atsteseremoteinteractivelogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seremoteinteractivelogonright" priority="1000" mode="M451">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seremoteinteractivelogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M451"/></axsl:template><axsl:template match="text()" priority="-1" mode="M451"/><axsl:template match="@*|node()" priority="-2" mode="M451"><axsl:apply-templates select="@*|*" mode="M451"/></axsl:template>

<!--PATTERN atsteseservicelogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:seservicelogonright" priority="1000" mode="M452">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seservicelogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M452"/></axsl:template><axsl:template match="text()" priority="-1" mode="M452"/><axsl:template match="@*|node()" priority="-2" mode="M452"><axsl:apply-templates select="@*|*" mode="M452"/></axsl:template>

<!--PATTERN atstesedenybatchLogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sedenybatchLogonright" priority="1000" mode="M453">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenybatchLogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M453"/></axsl:template><axsl:template match="text()" priority="-1" mode="M453"/><axsl:template match="@*|node()" priority="-2" mode="M453"><axsl:apply-templates select="@*|*" mode="M453"/></axsl:template>

<!--PATTERN atstesedenyinteractivelogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sedenyinteractivelogonright" priority="1000" mode="M454">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenyinteractivelogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M454"/></axsl:template><axsl:template match="text()" priority="-1" mode="M454"/><axsl:template match="@*|node()" priority="-2" mode="M454"><axsl:apply-templates select="@*|*" mode="M454"/></axsl:template>

<!--PATTERN atstesedenynetworklogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sedenynetworklogonright" priority="1000" mode="M455">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenynetworklogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M455"/></axsl:template><axsl:template match="text()" priority="-1" mode="M455"/><axsl:template match="@*|node()" priority="-2" mode="M455"><axsl:apply-templates select="@*|*" mode="M455"/></axsl:template>

<!--PATTERN atstesedenyremoteInteractivelogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sedenyremoteInteractivelogonright" priority="1000" mode="M456">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenyremoteInteractivelogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M456"/></axsl:template><axsl:template match="text()" priority="-1" mode="M456"/><axsl:template match="@*|node()" priority="-2" mode="M456"><axsl:apply-templates select="@*|*" mode="M456"/></axsl:template>

<!--PATTERN atstesedenyservicelogonright-->


	<!--RULE -->
<axsl:template match="win-def:accesstoken_state/win-def:sedenyservicelogonright" priority="1000" mode="M457">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenyservicelogonright entity of an accesstoken_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M457"/></axsl:template><axsl:template match="text()" priority="-1" mode="M457"/><axsl:template match="@*|node()" priority="-2" mode="M457"><axsl:apply-templates select="@*|*" mode="M457"/></axsl:template>

<!--PATTERN adtst-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_test/win-def:object" priority="1001" mode="M458">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:activedirectory_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an activedirectory_test must reference an activedirectory_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M458"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:activedirectory_test/win-def:state" priority="1000" mode="M458">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:activedirectory_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an activedirectory_test must reference an activedirectory_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M458"/></axsl:template><axsl:template match="text()" priority="-1" mode="M458"/><axsl:template match="@*|node()" priority="-2" mode="M458"><axsl:apply-templates select="@*|*" mode="M458"/></axsl:template>

<!--PATTERN adobjnaming_context-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_object/win-def:naming_context" priority="1000" mode="M459">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the naming_context entity of an activedirectory_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M459"/></axsl:template><axsl:template match="text()" priority="-1" mode="M459"/><axsl:template match="@*|node()" priority="-2" mode="M459"><axsl:apply-templates select="@*|*" mode="M459"/></axsl:template>

<!--PATTERN adobjrelative_dn-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_object/win-def:relative_dn" priority="1000" mode="M460">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the relative_dn entity of an activedirectory_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@xsi:nil='true') or ../win-def:attribute/@xsi:nil='true'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - attribute entity must be nil when relative_dn is nil<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M460"/></axsl:template><axsl:template match="text()" priority="-1" mode="M460"/><axsl:template match="@*|node()" priority="-2" mode="M460"><axsl:apply-templates select="@*|*" mode="M460"/></axsl:template>

<!--PATTERN adobjattribute-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_object/win-def:attribute" priority="1000" mode="M461">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the attribute entity of an activedirectory_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M461"/></axsl:template><axsl:template match="text()" priority="-1" mode="M461"/><axsl:template match="@*|node()" priority="-2" mode="M461"><axsl:apply-templates select="@*|*" mode="M461"/></axsl:template>

<!--PATTERN adstenaming_context-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_state/win-def:naming_context" priority="1000" mode="M462">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the naming_context entity of an activedirectory_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M462"/></axsl:template><axsl:template match="text()" priority="-1" mode="M462"/><axsl:template match="@*|node()" priority="-2" mode="M462"><axsl:apply-templates select="@*|*" mode="M462"/></axsl:template>

<!--PATTERN adsterelative_dn-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_state/win-def:relative_dn" priority="1000" mode="M463">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the relative_dn entity of an activedirectory_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M463"/></axsl:template><axsl:template match="text()" priority="-1" mode="M463"/><axsl:template match="@*|node()" priority="-2" mode="M463"><axsl:apply-templates select="@*|*" mode="M463"/></axsl:template>

<!--PATTERN adsteattribute-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_state/win-def:attribute" priority="1000" mode="M464">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the attribute entity of an activedirectory_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M464"/></axsl:template><axsl:template match="text()" priority="-1" mode="M464"/><axsl:template match="@*|node()" priority="-2" mode="M464"><axsl:apply-templates select="@*|*" mode="M464"/></axsl:template>

<!--PATTERN adsteobject_class-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_state/win-def:object_class" priority="1000" mode="M465">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the object_class entity of an activedirectory_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M465"/></axsl:template><axsl:template match="text()" priority="-1" mode="M465"/><axsl:template match="@*|node()" priority="-2" mode="M465"><axsl:apply-templates select="@*|*" mode="M465"/></axsl:template>

<!--PATTERN adsteadstype-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_state/win-def:adstype" priority="1000" mode="M466">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the adstype entity of an activedirectory_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M466"/></axsl:template><axsl:template match="text()" priority="-1" mode="M466"/><axsl:template match="@*|node()" priority="-2" mode="M466"><axsl:apply-templates select="@*|*" mode="M466"/></axsl:template>

<!--PATTERN adstevalue-->


	<!--RULE -->
<axsl:template match="win-def:activedirectory_state/win-def:value" priority="1000" mode="M467">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M467"/></axsl:template><axsl:template match="text()" priority="-1" mode="M467"/><axsl:template match="@*|node()" priority="-2" mode="M467"><axsl:apply-templates select="@*|*" mode="M467"/></axsl:template>

<!--PATTERN aeptst-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_test/win-def:object" priority="1001" mode="M468">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:auditeventpolicy_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an auditeventpolicy_test must reference an auditeventpolicy_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M468"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_test/win-def:state" priority="1000" mode="M468">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:auditeventpolicy_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an auditeventpolicy_test must reference an auditeventpolicy_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M468"/></axsl:template><axsl:template match="text()" priority="-1" mode="M468"/><axsl:template match="@*|node()" priority="-2" mode="M468"><axsl:apply-templates select="@*|*" mode="M468"/></axsl:template>

<!--PATTERN aepsteaccount_logon-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_state/win-def:account_logon" priority="1000" mode="M469">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the account_logon entity of an auditeventpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M469"/></axsl:template><axsl:template match="text()" priority="-1" mode="M469"/><axsl:template match="@*|node()" priority="-2" mode="M469"><axsl:apply-templates select="@*|*" mode="M469"/></axsl:template>

<!--PATTERN aepsteaccount_management-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_state/win-def:account_management" priority="1000" mode="M470">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the account_management entity of an auditeventpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M470"/></axsl:template><axsl:template match="text()" priority="-1" mode="M470"/><axsl:template match="@*|node()" priority="-2" mode="M470"><axsl:apply-templates select="@*|*" mode="M470"/></axsl:template>

<!--PATTERN aepstedetailed_tracking-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_state/win-def:detailed_tracking" priority="1000" mode="M471">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the detailed_tracking entity of an auditeventpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M471"/></axsl:template><axsl:template match="text()" priority="-1" mode="M471"/><axsl:template match="@*|node()" priority="-2" mode="M471"><axsl:apply-templates select="@*|*" mode="M471"/></axsl:template>

<!--PATTERN aepstedirectory_service_access-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_state/win-def:directory_service_access" priority="1000" mode="M472">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_service_access entity of an auditeventpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M472"/></axsl:template><axsl:template match="text()" priority="-1" mode="M472"/><axsl:template match="@*|node()" priority="-2" mode="M472"><axsl:apply-templates select="@*|*" mode="M472"/></axsl:template>

<!--PATTERN aepstelogon-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_state/win-def:logon" priority="1000" mode="M473">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the logon entity of an auditeventpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M473"/></axsl:template><axsl:template match="text()" priority="-1" mode="M473"/><axsl:template match="@*|node()" priority="-2" mode="M473"><axsl:apply-templates select="@*|*" mode="M473"/></axsl:template>

<!--PATTERN aepsteobject_access-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_state/win-def:object_access" priority="1000" mode="M474">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the object_access entity of an auditeventpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M474"/></axsl:template><axsl:template match="text()" priority="-1" mode="M474"/><axsl:template match="@*|node()" priority="-2" mode="M474"><axsl:apply-templates select="@*|*" mode="M474"/></axsl:template>

<!--PATTERN aepstepolicy_change-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_state/win-def:policy_change" priority="1000" mode="M475">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the policy_change entity of an auditeventpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M475"/></axsl:template><axsl:template match="text()" priority="-1" mode="M475"/><axsl:template match="@*|node()" priority="-2" mode="M475"><axsl:apply-templates select="@*|*" mode="M475"/></axsl:template>

<!--PATTERN aepsteprivilege_use-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_state/win-def:privilege_use" priority="1000" mode="M476">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the privilege_use entity of an auditeventpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M476"/></axsl:template><axsl:template match="text()" priority="-1" mode="M476"/><axsl:template match="@*|node()" priority="-2" mode="M476"><axsl:apply-templates select="@*|*" mode="M476"/></axsl:template>

<!--PATTERN aepstesystem-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicy_state/win-def:system" priority="1000" mode="M477">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the system entity of an auditeventpolicy_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M477"/></axsl:template><axsl:template match="text()" priority="-1" mode="M477"/><axsl:template match="@*|node()" priority="-2" mode="M477"><axsl:apply-templates select="@*|*" mode="M477"/></axsl:template>

<!--PATTERN aepstst-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_test/win-def:object" priority="1001" mode="M478">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:auditeventpolicysubcategories_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an auditeventpolicysubcategories_test must reference an auditeventpolicysubcategories_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M478"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_test/win-def:state" priority="1000" mode="M478">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:auditeventpolicysubcategories_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an auditeventpolicysubcategories_test must reference an auditeventpolicysubcategories_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M478"/></axsl:template><axsl:template match="text()" priority="-1" mode="M478"/><axsl:template match="@*|node()" priority="-2" mode="M478"><axsl:apply-templates select="@*|*" mode="M478"/></axsl:template>

<!--PATTERN aepsstecredentialvalidation-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:credential_validation" priority="1000" mode="M479">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the credential_validation entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M479"/></axsl:template><axsl:template match="text()" priority="-1" mode="M479"/><axsl:template match="@*|node()" priority="-2" mode="M479"><axsl:apply-templates select="@*|*" mode="M479"/></axsl:template>

<!--PATTERN aepsstekerberosticketevents-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:kerberos_ticket_events" priority="1000" mode="M480">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the kerberos_ticket_events entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M480"/></axsl:template><axsl:template match="text()" priority="-1" mode="M480"/><axsl:template match="@*|node()" priority="-2" mode="M480"><axsl:apply-templates select="@*|*" mode="M480"/></axsl:template>

<!--PATTERN aepssteotheraccountlogonevents-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_account_logon_events" priority="1000" mode="M481">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_account_logon_events entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M481"/></axsl:template><axsl:template match="text()" priority="-1" mode="M481"/><axsl:template match="@*|node()" priority="-2" mode="M481"><axsl:apply-templates select="@*|*" mode="M481"/></axsl:template>

<!--PATTERN aepssteapplicationgroupmanagement-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:application_group_management" priority="1000" mode="M482">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the application_group_management entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M482"/></axsl:template><axsl:template match="text()" priority="-1" mode="M482"/><axsl:template match="@*|node()" priority="-2" mode="M482"><axsl:apply-templates select="@*|*" mode="M482"/></axsl:template>

<!--PATTERN aepsstecomputeraccountmanagement-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:computer_account_management" priority="1000" mode="M483">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the computer_account_management entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M483"/></axsl:template><axsl:template match="text()" priority="-1" mode="M483"/><axsl:template match="@*|node()" priority="-2" mode="M483"><axsl:apply-templates select="@*|*" mode="M483"/></axsl:template>

<!--PATTERN aepsstedistributiongroupmanagement-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:distribution_group_management" priority="1000" mode="M484">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the distribution_group_management entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M484"/></axsl:template><axsl:template match="text()" priority="-1" mode="M484"/><axsl:template match="@*|node()" priority="-2" mode="M484"><axsl:apply-templates select="@*|*" mode="M484"/></axsl:template>

<!--PATTERN aepssteotheraccountmanagementevents-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_account_management_events" priority="1000" mode="M485">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_account_management_events entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M485"/></axsl:template><axsl:template match="text()" priority="-1" mode="M485"/><axsl:template match="@*|node()" priority="-2" mode="M485"><axsl:apply-templates select="@*|*" mode="M485"/></axsl:template>

<!--PATTERN aepsstesecuritygroupmanagement-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:security_group_management" priority="1000" mode="M486">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the security_group_management entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M486"/></axsl:template><axsl:template match="text()" priority="-1" mode="M486"/><axsl:template match="@*|node()" priority="-2" mode="M486"><axsl:apply-templates select="@*|*" mode="M486"/></axsl:template>

<!--PATTERN aepssteuseraccountmanagement-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:user_account_management" priority="1000" mode="M487">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_account_management entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M487"/></axsl:template><axsl:template match="text()" priority="-1" mode="M487"/><axsl:template match="@*|node()" priority="-2" mode="M487"><axsl:apply-templates select="@*|*" mode="M487"/></axsl:template>

<!--PATTERN aepsstedpapiactivity-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:dpapi_activity" priority="1000" mode="M488">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the dpapi_activity entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M488"/></axsl:template><axsl:template match="text()" priority="-1" mode="M488"/><axsl:template match="@*|node()" priority="-2" mode="M488"><axsl:apply-templates select="@*|*" mode="M488"/></axsl:template>

<!--PATTERN aepssteprocesscreation-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:process_creation" priority="1000" mode="M489">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the process_creation entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M489"/></axsl:template><axsl:template match="text()" priority="-1" mode="M489"/><axsl:template match="@*|node()" priority="-2" mode="M489"><axsl:apply-templates select="@*|*" mode="M489"/></axsl:template>

<!--PATTERN aepssteprocesstermination-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:process_termination" priority="1000" mode="M490">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the process_termination entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M490"/></axsl:template><axsl:template match="text()" priority="-1" mode="M490"/><axsl:template match="@*|node()" priority="-2" mode="M490"><axsl:apply-templates select="@*|*" mode="M490"/></axsl:template>

<!--PATTERN aepssterpcevents-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:rpc_events" priority="1000" mode="M491">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the rpc_events entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M491"/></axsl:template><axsl:template match="text()" priority="-1" mode="M491"/><axsl:template match="@*|node()" priority="-2" mode="M491"><axsl:apply-templates select="@*|*" mode="M491"/></axsl:template>

<!--PATTERN aepsstedirectoryserviceaccess-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:directory_service_access" priority="1000" mode="M492">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_service_access entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M492"/></axsl:template><axsl:template match="text()" priority="-1" mode="M492"/><axsl:template match="@*|node()" priority="-2" mode="M492"><axsl:apply-templates select="@*|*" mode="M492"/></axsl:template>

<!--PATTERN aepsstedirectoryservicechanges-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:directory_service_changes" priority="1000" mode="M493">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_service_changes entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M493"/></axsl:template><axsl:template match="text()" priority="-1" mode="M493"/><axsl:template match="@*|node()" priority="-2" mode="M493"><axsl:apply-templates select="@*|*" mode="M493"/></axsl:template>

<!--PATTERN aepsstedirectoryservicereplication-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:directory_service_replication" priority="1000" mode="M494">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_service_replication entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M494"/></axsl:template><axsl:template match="text()" priority="-1" mode="M494"/><axsl:template match="@*|node()" priority="-2" mode="M494"><axsl:apply-templates select="@*|*" mode="M494"/></axsl:template>

<!--PATTERN aepsstedetaileddirectoryservicereplication-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:detailed_directory_service_replication" priority="1000" mode="M495">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the detailed_directory_service_replication entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M495"/></axsl:template><axsl:template match="text()" priority="-1" mode="M495"/><axsl:template match="@*|node()" priority="-2" mode="M495"><axsl:apply-templates select="@*|*" mode="M495"/></axsl:template>

<!--PATTERN aepssteaccountlockout-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:account_lockout" priority="1000" mode="M496">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the account_lockout entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M496"/></axsl:template><axsl:template match="text()" priority="-1" mode="M496"/><axsl:template match="@*|node()" priority="-2" mode="M496"><axsl:apply-templates select="@*|*" mode="M496"/></axsl:template>

<!--PATTERN aepssteipsecextendedmode-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:ipsec_extended_mode" priority="1000" mode="M497">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ipsec_extended_mode entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M497"/></axsl:template><axsl:template match="text()" priority="-1" mode="M497"/><axsl:template match="@*|node()" priority="-2" mode="M497"><axsl:apply-templates select="@*|*" mode="M497"/></axsl:template>

<!--PATTERN aepssteipsecmainmode-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:ipsec_main_mode" priority="1000" mode="M498">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ipsec_main_mode entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M498"/></axsl:template><axsl:template match="text()" priority="-1" mode="M498"/><axsl:template match="@*|node()" priority="-2" mode="M498"><axsl:apply-templates select="@*|*" mode="M498"/></axsl:template>

<!--PATTERN aepssteipsec_quick_mode-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:ipsec_quick_mode" priority="1000" mode="M499">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ipsec_quick_mode entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M499"/></axsl:template><axsl:template match="text()" priority="-1" mode="M499"/><axsl:template match="@*|node()" priority="-2" mode="M499"><axsl:apply-templates select="@*|*" mode="M499"/></axsl:template>

<!--PATTERN aepsstelogoff-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:logoff" priority="1000" mode="M500">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the logoff entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M500"/></axsl:template><axsl:template match="text()" priority="-1" mode="M500"/><axsl:template match="@*|node()" priority="-2" mode="M500"><axsl:apply-templates select="@*|*" mode="M500"/></axsl:template>

<!--PATTERN aepsstelogon-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:logon" priority="1000" mode="M501">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the logon entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M501"/></axsl:template><axsl:template match="text()" priority="-1" mode="M501"/><axsl:template match="@*|node()" priority="-2" mode="M501"><axsl:apply-templates select="@*|*" mode="M501"/></axsl:template>

<!--PATTERN aepssteotherlogonlogoffevents-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_logon_logoff_events" priority="1000" mode="M502">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_logon_logoff_events entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M502"/></axsl:template><axsl:template match="text()" priority="-1" mode="M502"/><axsl:template match="@*|node()" priority="-2" mode="M502"><axsl:apply-templates select="@*|*" mode="M502"/></axsl:template>

<!--PATTERN aepsstespeciallogon-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:special_logon" priority="1000" mode="M503">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the special_logon entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M503"/></axsl:template><axsl:template match="text()" priority="-1" mode="M503"/><axsl:template match="@*|node()" priority="-2" mode="M503"><axsl:apply-templates select="@*|*" mode="M503"/></axsl:template>

<!--PATTERN aepssteapplicationgenerated-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:application_generated" priority="1000" mode="M504">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the application_generated entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M504"/></axsl:template><axsl:template match="text()" priority="-1" mode="M504"/><axsl:template match="@*|node()" priority="-2" mode="M504"><axsl:apply-templates select="@*|*" mode="M504"/></axsl:template>

<!--PATTERN aepsstecertificationservices-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:certification_services" priority="1000" mode="M505">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the certification_services entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M505"/></axsl:template><axsl:template match="text()" priority="-1" mode="M505"/><axsl:template match="@*|node()" priority="-2" mode="M505"><axsl:apply-templates select="@*|*" mode="M505"/></axsl:template>

<!--PATTERN aepsstefileshare-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:file_share" priority="1000" mode="M506">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_share entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M506"/></axsl:template><axsl:template match="text()" priority="-1" mode="M506"/><axsl:template match="@*|node()" priority="-2" mode="M506"><axsl:apply-templates select="@*|*" mode="M506"/></axsl:template>

<!--PATTERN aepsstefilesystem-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:file_system" priority="1000" mode="M507">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_system entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M507"/></axsl:template><axsl:template match="text()" priority="-1" mode="M507"/><axsl:template match="@*|node()" priority="-2" mode="M507"><axsl:apply-templates select="@*|*" mode="M507"/></axsl:template>

<!--PATTERN aepsstefilteringplatformconnection-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:filtering_platform_connection" priority="1000" mode="M508">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filtering_platform_connection entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M508"/></axsl:template><axsl:template match="text()" priority="-1" mode="M508"/><axsl:template match="@*|node()" priority="-2" mode="M508"><axsl:apply-templates select="@*|*" mode="M508"/></axsl:template>

<!--PATTERN aepsstefilteringplatformpacketdrop-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:filtering_platform_packet_drop" priority="1000" mode="M509">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filtering_platform_packet_drop entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M509"/></axsl:template><axsl:template match="text()" priority="-1" mode="M509"/><axsl:template match="@*|node()" priority="-2" mode="M509"><axsl:apply-templates select="@*|*" mode="M509"/></axsl:template>

<!--PATTERN aepsstehandlemanipulation-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:handle_manipulation" priority="1000" mode="M510">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the handle_manipulation entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M510"/></axsl:template><axsl:template match="text()" priority="-1" mode="M510"/><axsl:template match="@*|node()" priority="-2" mode="M510"><axsl:apply-templates select="@*|*" mode="M510"/></axsl:template>

<!--PATTERN aepsstekernelobject-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:kernel_object" priority="1000" mode="M511">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the kernel_object entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M511"/></axsl:template><axsl:template match="text()" priority="-1" mode="M511"/><axsl:template match="@*|node()" priority="-2" mode="M511"><axsl:apply-templates select="@*|*" mode="M511"/></axsl:template>

<!--PATTERN aepssteotherobjectaccessevents-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_object_access_events" priority="1000" mode="M512">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_object_access_events entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M512"/></axsl:template><axsl:template match="text()" priority="-1" mode="M512"/><axsl:template match="@*|node()" priority="-2" mode="M512"><axsl:apply-templates select="@*|*" mode="M512"/></axsl:template>

<!--PATTERN aepssteregistry-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:registry" priority="1000" mode="M513">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the registry entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M513"/></axsl:template><axsl:template match="text()" priority="-1" mode="M513"/><axsl:template match="@*|node()" priority="-2" mode="M513"><axsl:apply-templates select="@*|*" mode="M513"/></axsl:template>

<!--PATTERN aepsstesam-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:sam" priority="1000" mode="M514">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sam entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M514"/></axsl:template><axsl:template match="text()" priority="-1" mode="M514"/><axsl:template match="@*|node()" priority="-2" mode="M514"><axsl:apply-templates select="@*|*" mode="M514"/></axsl:template>

<!--PATTERN aepssteauditpolicychange-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:audit_policy_change" priority="1000" mode="M515">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the audit_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M515"/></axsl:template><axsl:template match="text()" priority="-1" mode="M515"/><axsl:template match="@*|node()" priority="-2" mode="M515"><axsl:apply-templates select="@*|*" mode="M515"/></axsl:template>

<!--PATTERN aepssteauthenticationpolicychange-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:authentication_policy_change" priority="1000" mode="M516">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the authentication_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M516"/></axsl:template><axsl:template match="text()" priority="-1" mode="M516"/><axsl:template match="@*|node()" priority="-2" mode="M516"><axsl:apply-templates select="@*|*" mode="M516"/></axsl:template>

<!--PATTERN aepssteauthorizationpolicychange-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:authorization_policy_change" priority="1000" mode="M517">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the authorization_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M517"/></axsl:template><axsl:template match="text()" priority="-1" mode="M517"/><axsl:template match="@*|node()" priority="-2" mode="M517"><axsl:apply-templates select="@*|*" mode="M517"/></axsl:template>

<!--PATTERN aepsstefilteringplatformpolicychange-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:filtering_platform_policy_change" priority="1000" mode="M518">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filtering_platform_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M518"/></axsl:template><axsl:template match="text()" priority="-1" mode="M518"/><axsl:template match="@*|node()" priority="-2" mode="M518"><axsl:apply-templates select="@*|*" mode="M518"/></axsl:template>

<!--PATTERN aepsstempssvcrulelevelpolicychange-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:mpssvc_rule_level_policy_change" priority="1000" mode="M519">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the mpssvc_rule_level_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M519"/></axsl:template><axsl:template match="text()" priority="-1" mode="M519"/><axsl:template match="@*|node()" priority="-2" mode="M519"><axsl:apply-templates select="@*|*" mode="M519"/></axsl:template>

<!--PATTERN aepssteotherpolicychangeevents-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_policy_change_events" priority="1000" mode="M520">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_policy_change_events entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M520"/></axsl:template><axsl:template match="text()" priority="-1" mode="M520"/><axsl:template match="@*|node()" priority="-2" mode="M520"><axsl:apply-templates select="@*|*" mode="M520"/></axsl:template>

<!--PATTERN aepsstenonsensitiveprivilegeuse-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:non_sensitive_privilege_use" priority="1000" mode="M521">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the non_sensitive_privilege_use entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M521"/></axsl:template><axsl:template match="text()" priority="-1" mode="M521"/><axsl:template match="@*|node()" priority="-2" mode="M521"><axsl:apply-templates select="@*|*" mode="M521"/></axsl:template>

<!--PATTERN aepssteotherprivilegeuseevents-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_privilege_use_events" priority="1000" mode="M522">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_privilege_use_events entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M522"/></axsl:template><axsl:template match="text()" priority="-1" mode="M522"/><axsl:template match="@*|node()" priority="-2" mode="M522"><axsl:apply-templates select="@*|*" mode="M522"/></axsl:template>

<!--PATTERN aepsstesensitive_privilege_use-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:sensitive_privilege_use" priority="1000" mode="M523">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sensitive_privilege_use entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M523"/></axsl:template><axsl:template match="text()" priority="-1" mode="M523"/><axsl:template match="@*|node()" priority="-2" mode="M523"><axsl:apply-templates select="@*|*" mode="M523"/></axsl:template>

<!--PATTERN aepssteipsecdriver-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:ipsec_driver" priority="1000" mode="M524">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ipsec_driver entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M524"/></axsl:template><axsl:template match="text()" priority="-1" mode="M524"/><axsl:template match="@*|node()" priority="-2" mode="M524"><axsl:apply-templates select="@*|*" mode="M524"/></axsl:template>

<!--PATTERN aepssteothersystemevents-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_system_events" priority="1000" mode="M525">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_system_events entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M525"/></axsl:template><axsl:template match="text()" priority="-1" mode="M525"/><axsl:template match="@*|node()" priority="-2" mode="M525"><axsl:apply-templates select="@*|*" mode="M525"/></axsl:template>

<!--PATTERN aepsstesecuritystatechange-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:security_state_change" priority="1000" mode="M526">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the security_state_change entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M526"/></axsl:template><axsl:template match="text()" priority="-1" mode="M526"/><axsl:template match="@*|node()" priority="-2" mode="M526"><axsl:apply-templates select="@*|*" mode="M526"/></axsl:template>

<!--PATTERN aepsstesecuritysystemextension-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:security_system_extension" priority="1000" mode="M527">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the security_system_extension entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M527"/></axsl:template><axsl:template match="text()" priority="-1" mode="M527"/><axsl:template match="@*|node()" priority="-2" mode="M527"><axsl:apply-templates select="@*|*" mode="M527"/></axsl:template>

<!--PATTERN aepsstesystemintegrity-->


	<!--RULE -->
<axsl:template match="win-def:auditeventpolicysubcategories_state/win-def:system_integrity" priority="1000" mode="M528">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the system_integrity entity of an auditeventpolicysubcategories_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M528"/></axsl:template><axsl:template match="text()" priority="-1" mode="M528"/><axsl:template match="@*|node()" priority="-2" mode="M528"><axsl:apply-templates select="@*|*" mode="M528"/></axsl:template>

<!--PATTERN filetst-->


	<!--RULE -->
<axsl:template match="win-def:file_test/win-def:object" priority="1001" mode="M529">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:file_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a file_test must reference a file_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M529"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:file_test/win-def:state" priority="1000" mode="M529">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:file_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a file_test must reference a file_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M529"/></axsl:template><axsl:template match="text()" priority="-1" mode="M529"/><axsl:template match="@*|node()" priority="-2" mode="M529"><axsl:apply-templates select="@*|*" mode="M529"/></axsl:template>

<!--PATTERN fileobjpath-->


	<!--RULE -->
<axsl:template match="win-def:file_object/win-def:path" priority="1000" mode="M530">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a file_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M530"/></axsl:template><axsl:template match="text()" priority="-1" mode="M530"/><axsl:template match="@*|node()" priority="-2" mode="M530"><axsl:apply-templates select="@*|*" mode="M530"/></axsl:template>

<!--PATTERN fileobjfilename-->


	<!--RULE -->
<axsl:template match="win-def:file_object/win-def:filename" priority="1000" mode="M531">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a file_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M531"/></axsl:template><axsl:template match="text()" priority="-1" mode="M531"/><axsl:template match="@*|node()" priority="-2" mode="M531"><axsl:apply-templates select="@*|*" mode="M531"/></axsl:template>

<!--PATTERN filestepath-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:path" priority="1000" mode="M532">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M532"/></axsl:template><axsl:template match="text()" priority="-1" mode="M532"/><axsl:template match="@*|node()" priority="-2" mode="M532"><axsl:apply-templates select="@*|*" mode="M532"/></axsl:template>

<!--PATTERN filestefilename-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:filename" priority="1000" mode="M533">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M533"/></axsl:template><axsl:template match="text()" priority="-1" mode="M533"/><axsl:template match="@*|node()" priority="-2" mode="M533"><axsl:apply-templates select="@*|*" mode="M533"/></axsl:template>

<!--PATTERN filesteowner-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:owner" priority="1000" mode="M534">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the owner entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M534"/></axsl:template><axsl:template match="text()" priority="-1" mode="M534"/><axsl:template match="@*|node()" priority="-2" mode="M534"><axsl:apply-templates select="@*|*" mode="M534"/></axsl:template>

<!--PATTERN filestesize-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:size" priority="1000" mode="M535">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the size entity of a file_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M535"/></axsl:template><axsl:template match="text()" priority="-1" mode="M535"/><axsl:template match="@*|node()" priority="-2" mode="M535"><axsl:apply-templates select="@*|*" mode="M535"/></axsl:template>

<!--PATTERN filestea_time-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:a_time" priority="1000" mode="M536">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the a_time entity of a file_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M536"/></axsl:template><axsl:template match="text()" priority="-1" mode="M536"/><axsl:template match="@*|node()" priority="-2" mode="M536"><axsl:apply-templates select="@*|*" mode="M536"/></axsl:template>

<!--PATTERN filestec_time-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:c_time" priority="1000" mode="M537">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the c_time entity of a file_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M537"/></axsl:template><axsl:template match="text()" priority="-1" mode="M537"/><axsl:template match="@*|node()" priority="-2" mode="M537"><axsl:apply-templates select="@*|*" mode="M537"/></axsl:template>

<!--PATTERN filestem_time-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:m_time" priority="1000" mode="M538">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the m_time entity of a file_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M538"/></axsl:template><axsl:template match="text()" priority="-1" mode="M538"/><axsl:template match="@*|node()" priority="-2" mode="M538"><axsl:apply-templates select="@*|*" mode="M538"/></axsl:template>

<!--PATTERN filestems_checksum-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:ms_checksum" priority="1000" mode="M539">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ms_checksum entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M539"/></axsl:template><axsl:template match="text()" priority="-1" mode="M539"/><axsl:template match="@*|node()" priority="-2" mode="M539"><axsl:apply-templates select="@*|*" mode="M539"/></axsl:template>

<!--PATTERN filesteversion-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:version" priority="1000" mode="M540">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a file_state should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M540"/></axsl:template><axsl:template match="text()" priority="-1" mode="M540"/><axsl:template match="@*|node()" priority="-2" mode="M540"><axsl:apply-templates select="@*|*" mode="M540"/></axsl:template>

<!--PATTERN filestetype-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:type" priority="1000" mode="M541">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M541"/></axsl:template><axsl:template match="text()" priority="-1" mode="M541"/><axsl:template match="@*|node()" priority="-2" mode="M541"><axsl:apply-templates select="@*|*" mode="M541"/></axsl:template>

<!--PATTERN filestedevelopment_class-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:development_class" priority="1000" mode="M542">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the development_class entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M542"/></axsl:template><axsl:template match="text()" priority="-1" mode="M542"/><axsl:template match="@*|node()" priority="-2" mode="M542"><axsl:apply-templates select="@*|*" mode="M542"/></axsl:template>

<!--PATTERN filestecompany-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:company" priority="1000" mode="M543">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the company entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M543"/></axsl:template><axsl:template match="text()" priority="-1" mode="M543"/><axsl:template match="@*|node()" priority="-2" mode="M543"><axsl:apply-templates select="@*|*" mode="M543"/></axsl:template>

<!--PATTERN filesteinternalname-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:internal_name" priority="1000" mode="M544">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the internal_name entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M544"/></axsl:template><axsl:template match="text()" priority="-1" mode="M544"/><axsl:template match="@*|node()" priority="-2" mode="M544"><axsl:apply-templates select="@*|*" mode="M544"/></axsl:template>

<!--PATTERN filestelanguage-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:language" priority="1000" mode="M545">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the language entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M545"/></axsl:template><axsl:template match="text()" priority="-1" mode="M545"/><axsl:template match="@*|node()" priority="-2" mode="M545"><axsl:apply-templates select="@*|*" mode="M545"/></axsl:template>

<!--PATTERN filesteoriginalfilename-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:original_filename" priority="1000" mode="M546">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the original_filename entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M546"/></axsl:template><axsl:template match="text()" priority="-1" mode="M546"/><axsl:template match="@*|node()" priority="-2" mode="M546"><axsl:apply-templates select="@*|*" mode="M546"/></axsl:template>

<!--PATTERN filesteproductname-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:product_name" priority="1000" mode="M547">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the product_name entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M547"/></axsl:template><axsl:template match="text()" priority="-1" mode="M547"/><axsl:template match="@*|node()" priority="-2" mode="M547"><axsl:apply-templates select="@*|*" mode="M547"/></axsl:template>

<!--PATTERN filesteproductversion-->


	<!--RULE -->
<axsl:template match="win-def:file_state/win-def:product_version" priority="1000" mode="M548">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the product_version entity of a file_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M548"/></axsl:template><axsl:template match="text()" priority="-1" mode="M548"/><axsl:template match="@*|node()" priority="-2" mode="M548"><axsl:apply-templates select="@*|*" mode="M548"/></axsl:template>

<!--PATTERN fap53tst-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_test/win-def:object" priority="1001" mode="M549">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:fileauditedpermissions53_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileauditedpermissions53_test must reference a fileauditedpermissions53_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M549"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_test/win-def:state" priority="1000" mode="M549">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:fileauditedpermissions53_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileauditedpermissions53_test must reference a fileauditedpermissions53_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M549"/></axsl:template><axsl:template match="text()" priority="-1" mode="M549"/><axsl:template match="@*|node()" priority="-2" mode="M549"><axsl:apply-templates select="@*|*" mode="M549"/></axsl:template>

<!--PATTERN fap53objpath-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_object/win-def:path" priority="1000" mode="M550">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileauditedpermissions53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M550"/></axsl:template><axsl:template match="text()" priority="-1" mode="M550"/><axsl:template match="@*|node()" priority="-2" mode="M550"><axsl:apply-templates select="@*|*" mode="M550"/></axsl:template>

<!--PATTERN fap53objfilename-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_object/win-def:filename" priority="1000" mode="M551">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileauditedpermissions53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M551"/></axsl:template><axsl:template match="text()" priority="-1" mode="M551"/><axsl:template match="@*|node()" priority="-2" mode="M551"><axsl:apply-templates select="@*|*" mode="M551"/></axsl:template>

<!--PATTERN fap53objtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_object/win-def:trustee_sid" priority="1000" mode="M552">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a fileauditedpermissions53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M552"/></axsl:template><axsl:template match="text()" priority="-1" mode="M552"/><axsl:template match="@*|node()" priority="-2" mode="M552"><axsl:apply-templates select="@*|*" mode="M552"/></axsl:template>

<!--PATTERN fap53stepath-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:path" priority="1000" mode="M553">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M553"/></axsl:template><axsl:template match="text()" priority="-1" mode="M553"/><axsl:template match="@*|node()" priority="-2" mode="M553"><axsl:apply-templates select="@*|*" mode="M553"/></axsl:template>

<!--PATTERN fap53stefilename-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:filename" priority="1000" mode="M554">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M554"/></axsl:template><axsl:template match="text()" priority="-1" mode="M554"/><axsl:template match="@*|node()" priority="-2" mode="M554"><axsl:apply-templates select="@*|*" mode="M554"/></axsl:template>

<!--PATTERN fap53stetrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:trustee_sid" priority="1000" mode="M555">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M555"/></axsl:template><axsl:template match="text()" priority="-1" mode="M555"/><axsl:template match="@*|node()" priority="-2" mode="M555"><axsl:apply-templates select="@*|*" mode="M555"/></axsl:template>

<!--PATTERN fap53stestandard_delete-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_delete" priority="1000" mode="M556">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M556"/></axsl:template><axsl:template match="text()" priority="-1" mode="M556"/><axsl:template match="@*|node()" priority="-2" mode="M556"><axsl:apply-templates select="@*|*" mode="M556"/></axsl:template>

<!--PATTERN fap53stestandard_read_control-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_read_control" priority="1000" mode="M557">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M557"/></axsl:template><axsl:template match="text()" priority="-1" mode="M557"/><axsl:template match="@*|node()" priority="-2" mode="M557"><axsl:apply-templates select="@*|*" mode="M557"/></axsl:template>

<!--PATTERN fap53stestandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_write_dac" priority="1000" mode="M558">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M558"/></axsl:template><axsl:template match="text()" priority="-1" mode="M558"/><axsl:template match="@*|node()" priority="-2" mode="M558"><axsl:apply-templates select="@*|*" mode="M558"/></axsl:template>

<!--PATTERN fap53stestandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_write_owner" priority="1000" mode="M559">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M559"/></axsl:template><axsl:template match="text()" priority="-1" mode="M559"/><axsl:template match="@*|node()" priority="-2" mode="M559"><axsl:apply-templates select="@*|*" mode="M559"/></axsl:template>

<!--PATTERN fap53stestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_synchronize" priority="1000" mode="M560">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M560"/></axsl:template><axsl:template match="text()" priority="-1" mode="M560"/><axsl:template match="@*|node()" priority="-2" mode="M560"><axsl:apply-templates select="@*|*" mode="M560"/></axsl:template>

<!--PATTERN fap53steaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:access_system_security" priority="1000" mode="M561">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M561"/></axsl:template><axsl:template match="text()" priority="-1" mode="M561"/><axsl:template match="@*|node()" priority="-2" mode="M561"><axsl:apply-templates select="@*|*" mode="M561"/></axsl:template>

<!--PATTERN fap53stegeneric_read-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:generic_read" priority="1000" mode="M562">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M562"/></axsl:template><axsl:template match="text()" priority="-1" mode="M562"/><axsl:template match="@*|node()" priority="-2" mode="M562"><axsl:apply-templates select="@*|*" mode="M562"/></axsl:template>

<!--PATTERN fap53stegeneric_write-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:generic_write" priority="1000" mode="M563">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M563"/></axsl:template><axsl:template match="text()" priority="-1" mode="M563"/><axsl:template match="@*|node()" priority="-2" mode="M563"><axsl:apply-templates select="@*|*" mode="M563"/></axsl:template>

<!--PATTERN fap53stegeneric_execute-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:generic_execute" priority="1000" mode="M564">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M564"/></axsl:template><axsl:template match="text()" priority="-1" mode="M564"/><axsl:template match="@*|node()" priority="-2" mode="M564"><axsl:apply-templates select="@*|*" mode="M564"/></axsl:template>

<!--PATTERN fap53stegeneric_all-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:generic_all" priority="1000" mode="M565">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M565"/></axsl:template><axsl:template match="text()" priority="-1" mode="M565"/><axsl:template match="@*|node()" priority="-2" mode="M565"><axsl:apply-templates select="@*|*" mode="M565"/></axsl:template>

<!--PATTERN fap53stefile_read_data-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:file_read_data" priority="1000" mode="M566">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_data entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M566"/></axsl:template><axsl:template match="text()" priority="-1" mode="M566"/><axsl:template match="@*|node()" priority="-2" mode="M566"><axsl:apply-templates select="@*|*" mode="M566"/></axsl:template>

<!--PATTERN fap53stefile_write_data-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:file_write_data" priority="1000" mode="M567">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_data entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M567"/></axsl:template><axsl:template match="text()" priority="-1" mode="M567"/><axsl:template match="@*|node()" priority="-2" mode="M567"><axsl:apply-templates select="@*|*" mode="M567"/></axsl:template>

<!--PATTERN fap53stefile_append_data-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:file_append_data" priority="1000" mode="M568">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_append_data entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M568"/></axsl:template><axsl:template match="text()" priority="-1" mode="M568"/><axsl:template match="@*|node()" priority="-2" mode="M568"><axsl:apply-templates select="@*|*" mode="M568"/></axsl:template>

<!--PATTERN fap53stefile_read_ea-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:file_read_ea" priority="1000" mode="M569">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_ea entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M569"/></axsl:template><axsl:template match="text()" priority="-1" mode="M569"/><axsl:template match="@*|node()" priority="-2" mode="M569"><axsl:apply-templates select="@*|*" mode="M569"/></axsl:template>

<!--PATTERN fap53stefile_write_ea-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:file_write_ea" priority="1000" mode="M570">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_ea entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M570"/></axsl:template><axsl:template match="text()" priority="-1" mode="M570"/><axsl:template match="@*|node()" priority="-2" mode="M570"><axsl:apply-templates select="@*|*" mode="M570"/></axsl:template>

<!--PATTERN fap53stefile_execute-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:file_execute" priority="1000" mode="M571">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_execute entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M571"/></axsl:template><axsl:template match="text()" priority="-1" mode="M571"/><axsl:template match="@*|node()" priority="-2" mode="M571"><axsl:apply-templates select="@*|*" mode="M571"/></axsl:template>

<!--PATTERN fap53stefile_delete_child-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:file_delete_child" priority="1000" mode="M572">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_delete_child entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M572"/></axsl:template><axsl:template match="text()" priority="-1" mode="M572"/><axsl:template match="@*|node()" priority="-2" mode="M572"><axsl:apply-templates select="@*|*" mode="M572"/></axsl:template>

<!--PATTERN fap53stefile_read_attributes-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:file_read_attributes" priority="1000" mode="M573">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_attributes entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M573"/></axsl:template><axsl:template match="text()" priority="-1" mode="M573"/><axsl:template match="@*|node()" priority="-2" mode="M573"><axsl:apply-templates select="@*|*" mode="M573"/></axsl:template>

<!--PATTERN fap53stefile_write_attributes-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions53_state/win-def:file_write_attributes" priority="1000" mode="M574">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_attributes entity of a fileauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M574"/></axsl:template><axsl:template match="text()" priority="-1" mode="M574"/><axsl:template match="@*|node()" priority="-2" mode="M574"><axsl:apply-templates select="@*|*" mode="M574"/></axsl:template>

<!--PATTERN faptst-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_test/win-def:object" priority="1001" mode="M575">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:fileauditedpermissions_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileauditedpermissions_test must reference a fileauditedpermissions_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M575"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_test/win-def:state" priority="1000" mode="M575">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:fileauditedpermissions_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileauditedpermissions_test must reference a fileauditedpermissions_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M575"/></axsl:template><axsl:template match="text()" priority="-1" mode="M575"/><axsl:template match="@*|node()" priority="-2" mode="M575"><axsl:apply-templates select="@*|*" mode="M575"/></axsl:template>

<!--PATTERN fapobjpath-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_object/win-def:path" priority="1000" mode="M576">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileauditedpermissions_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M576"/></axsl:template><axsl:template match="text()" priority="-1" mode="M576"/><axsl:template match="@*|node()" priority="-2" mode="M576"><axsl:apply-templates select="@*|*" mode="M576"/></axsl:template>

<!--PATTERN fapobjfilename-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_object/win-def:filename" priority="1000" mode="M577">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileauditedpermissions_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M577"/></axsl:template><axsl:template match="text()" priority="-1" mode="M577"/><axsl:template match="@*|node()" priority="-2" mode="M577"><axsl:apply-templates select="@*|*" mode="M577"/></axsl:template>

<!--PATTERN fapobjtrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_object/win-def:trustee_name" priority="1000" mode="M578">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a fileauditedpermissions_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M578"/></axsl:template><axsl:template match="text()" priority="-1" mode="M578"/><axsl:template match="@*|node()" priority="-2" mode="M578"><axsl:apply-templates select="@*|*" mode="M578"/></axsl:template>

<!--PATTERN fapstepath-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:path" priority="1000" mode="M579">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M579"/></axsl:template><axsl:template match="text()" priority="-1" mode="M579"/><axsl:template match="@*|node()" priority="-2" mode="M579"><axsl:apply-templates select="@*|*" mode="M579"/></axsl:template>

<!--PATTERN fapstefilename-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:filename" priority="1000" mode="M580">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M580"/></axsl:template><axsl:template match="text()" priority="-1" mode="M580"/><axsl:template match="@*|node()" priority="-2" mode="M580"><axsl:apply-templates select="@*|*" mode="M580"/></axsl:template>

<!--PATTERN fapstetrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:trustee_name" priority="1000" mode="M581">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M581"/></axsl:template><axsl:template match="text()" priority="-1" mode="M581"/><axsl:template match="@*|node()" priority="-2" mode="M581"><axsl:apply-templates select="@*|*" mode="M581"/></axsl:template>

<!--PATTERN fapstestandard_delete-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:standard_delete" priority="1000" mode="M582">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M582"/></axsl:template><axsl:template match="text()" priority="-1" mode="M582"/><axsl:template match="@*|node()" priority="-2" mode="M582"><axsl:apply-templates select="@*|*" mode="M582"/></axsl:template>

<!--PATTERN fapstestandard_read_control-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:standard_read_control" priority="1000" mode="M583">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M583"/></axsl:template><axsl:template match="text()" priority="-1" mode="M583"/><axsl:template match="@*|node()" priority="-2" mode="M583"><axsl:apply-templates select="@*|*" mode="M583"/></axsl:template>

<!--PATTERN fapstestandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:standard_write_dac" priority="1000" mode="M584">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M584"/></axsl:template><axsl:template match="text()" priority="-1" mode="M584"/><axsl:template match="@*|node()" priority="-2" mode="M584"><axsl:apply-templates select="@*|*" mode="M584"/></axsl:template>

<!--PATTERN fapstestandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:standard_write_owner" priority="1000" mode="M585">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M585"/></axsl:template><axsl:template match="text()" priority="-1" mode="M585"/><axsl:template match="@*|node()" priority="-2" mode="M585"><axsl:apply-templates select="@*|*" mode="M585"/></axsl:template>

<!--PATTERN fapstestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:standard_synchronize" priority="1000" mode="M586">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M586"/></axsl:template><axsl:template match="text()" priority="-1" mode="M586"/><axsl:template match="@*|node()" priority="-2" mode="M586"><axsl:apply-templates select="@*|*" mode="M586"/></axsl:template>

<!--PATTERN fapsteaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:access_system_security" priority="1000" mode="M587">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M587"/></axsl:template><axsl:template match="text()" priority="-1" mode="M587"/><axsl:template match="@*|node()" priority="-2" mode="M587"><axsl:apply-templates select="@*|*" mode="M587"/></axsl:template>

<!--PATTERN fapstegeneric_read-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:generic_read" priority="1000" mode="M588">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M588"/></axsl:template><axsl:template match="text()" priority="-1" mode="M588"/><axsl:template match="@*|node()" priority="-2" mode="M588"><axsl:apply-templates select="@*|*" mode="M588"/></axsl:template>

<!--PATTERN fapstegeneric_write-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:generic_write" priority="1000" mode="M589">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M589"/></axsl:template><axsl:template match="text()" priority="-1" mode="M589"/><axsl:template match="@*|node()" priority="-2" mode="M589"><axsl:apply-templates select="@*|*" mode="M589"/></axsl:template>

<!--PATTERN fapstegeneric_execute-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:generic_execute" priority="1000" mode="M590">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M590"/></axsl:template><axsl:template match="text()" priority="-1" mode="M590"/><axsl:template match="@*|node()" priority="-2" mode="M590"><axsl:apply-templates select="@*|*" mode="M590"/></axsl:template>

<!--PATTERN fapstegeneric_all-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:generic_all" priority="1000" mode="M591">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M591"/></axsl:template><axsl:template match="text()" priority="-1" mode="M591"/><axsl:template match="@*|node()" priority="-2" mode="M591"><axsl:apply-templates select="@*|*" mode="M591"/></axsl:template>

<!--PATTERN fapstefile_read_data-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:file_read_data" priority="1000" mode="M592">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_data entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M592"/></axsl:template><axsl:template match="text()" priority="-1" mode="M592"/><axsl:template match="@*|node()" priority="-2" mode="M592"><axsl:apply-templates select="@*|*" mode="M592"/></axsl:template>

<!--PATTERN fapstefile_write_data-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:file_write_data" priority="1000" mode="M593">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_data entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M593"/></axsl:template><axsl:template match="text()" priority="-1" mode="M593"/><axsl:template match="@*|node()" priority="-2" mode="M593"><axsl:apply-templates select="@*|*" mode="M593"/></axsl:template>

<!--PATTERN fapstefile_append_data-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:file_append_data" priority="1000" mode="M594">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_append_data entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M594"/></axsl:template><axsl:template match="text()" priority="-1" mode="M594"/><axsl:template match="@*|node()" priority="-2" mode="M594"><axsl:apply-templates select="@*|*" mode="M594"/></axsl:template>

<!--PATTERN fapstefile_read_ea-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:file_read_ea" priority="1000" mode="M595">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_ea entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M595"/></axsl:template><axsl:template match="text()" priority="-1" mode="M595"/><axsl:template match="@*|node()" priority="-2" mode="M595"><axsl:apply-templates select="@*|*" mode="M595"/></axsl:template>

<!--PATTERN fapstefile_write_ea-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:file_write_ea" priority="1000" mode="M596">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_ea entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M596"/></axsl:template><axsl:template match="text()" priority="-1" mode="M596"/><axsl:template match="@*|node()" priority="-2" mode="M596"><axsl:apply-templates select="@*|*" mode="M596"/></axsl:template>

<!--PATTERN fapstefile_execute-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:file_execute" priority="1000" mode="M597">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_execute entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M597"/></axsl:template><axsl:template match="text()" priority="-1" mode="M597"/><axsl:template match="@*|node()" priority="-2" mode="M597"><axsl:apply-templates select="@*|*" mode="M597"/></axsl:template>

<!--PATTERN fapstefile_delete_child-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:file_delete_child" priority="1000" mode="M598">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_delete_child entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M598"/></axsl:template><axsl:template match="text()" priority="-1" mode="M598"/><axsl:template match="@*|node()" priority="-2" mode="M598"><axsl:apply-templates select="@*|*" mode="M598"/></axsl:template>

<!--PATTERN fapstefile_read_attributes-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:file_read_attributes" priority="1000" mode="M599">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_attributes entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M599"/></axsl:template><axsl:template match="text()" priority="-1" mode="M599"/><axsl:template match="@*|node()" priority="-2" mode="M599"><axsl:apply-templates select="@*|*" mode="M599"/></axsl:template>

<!--PATTERN fapstefile_write_attributes-->


	<!--RULE -->
<axsl:template match="win-def:fileauditedpermissions_state/win-def:file_write_attributes" priority="1000" mode="M600">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_attributes entity of a fileauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M600"/></axsl:template><axsl:template match="text()" priority="-1" mode="M600"/><axsl:template match="@*|node()" priority="-2" mode="M600"><axsl:apply-templates select="@*|*" mode="M600"/></axsl:template>

<!--PATTERN fer53tst-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_test/win-def:object" priority="1001" mode="M601">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:fileeffectiverights53_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileeffectiverights53_test must reference a fileeffectiverights53_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M601"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_test/win-def:state" priority="1000" mode="M601">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:fileeffectiverights53_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileeffectiverights53_test must reference a fileeffectiverights53_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M601"/></axsl:template><axsl:template match="text()" priority="-1" mode="M601"/><axsl:template match="@*|node()" priority="-2" mode="M601"><axsl:apply-templates select="@*|*" mode="M601"/></axsl:template>

<!--PATTERN fer53objpath-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_object/win-def:path" priority="1000" mode="M602">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileeffectiverights53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M602"/></axsl:template><axsl:template match="text()" priority="-1" mode="M602"/><axsl:template match="@*|node()" priority="-2" mode="M602"><axsl:apply-templates select="@*|*" mode="M602"/></axsl:template>

<!--PATTERN fer53objfilename-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_object/win-def:filename" priority="1000" mode="M603">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileeffectiverights53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M603"/></axsl:template><axsl:template match="text()" priority="-1" mode="M603"/><axsl:template match="@*|node()" priority="-2" mode="M603"><axsl:apply-templates select="@*|*" mode="M603"/></axsl:template>

<!--PATTERN fer53objtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_object/win-def:trustee_sid" priority="1000" mode="M604">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a fileeffectiverights53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M604"/></axsl:template><axsl:template match="text()" priority="-1" mode="M604"/><axsl:template match="@*|node()" priority="-2" mode="M604"><axsl:apply-templates select="@*|*" mode="M604"/></axsl:template>

<!--PATTERN fer53stepath-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:path" priority="1000" mode="M605">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileeffectiverights53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M605"/></axsl:template><axsl:template match="text()" priority="-1" mode="M605"/><axsl:template match="@*|node()" priority="-2" mode="M605"><axsl:apply-templates select="@*|*" mode="M605"/></axsl:template>

<!--PATTERN fer53stefilename-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:filename" priority="1000" mode="M606">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileeffectiverights53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M606"/></axsl:template><axsl:template match="text()" priority="-1" mode="M606"/><axsl:template match="@*|node()" priority="-2" mode="M606"><axsl:apply-templates select="@*|*" mode="M606"/></axsl:template>

<!--PATTERN fer53stetrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:trustee_sid" priority="1000" mode="M607">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a fileeffectiverights53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M607"/></axsl:template><axsl:template match="text()" priority="-1" mode="M607"/><axsl:template match="@*|node()" priority="-2" mode="M607"><axsl:apply-templates select="@*|*" mode="M607"/></axsl:template>

<!--PATTERN fer53stestandard_delete-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:standard_delete" priority="1000" mode="M608">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M608"/></axsl:template><axsl:template match="text()" priority="-1" mode="M608"/><axsl:template match="@*|node()" priority="-2" mode="M608"><axsl:apply-templates select="@*|*" mode="M608"/></axsl:template>

<!--PATTERN fer53stestandard_read_control-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:standard_read_control" priority="1000" mode="M609">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M609"/></axsl:template><axsl:template match="text()" priority="-1" mode="M609"/><axsl:template match="@*|node()" priority="-2" mode="M609"><axsl:apply-templates select="@*|*" mode="M609"/></axsl:template>

<!--PATTERN fer53stestandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:standard_write_dac" priority="1000" mode="M610">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M610"/></axsl:template><axsl:template match="text()" priority="-1" mode="M610"/><axsl:template match="@*|node()" priority="-2" mode="M610"><axsl:apply-templates select="@*|*" mode="M610"/></axsl:template>

<!--PATTERN fer53stestandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:standard_write_owner" priority="1000" mode="M611">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M611"/></axsl:template><axsl:template match="text()" priority="-1" mode="M611"/><axsl:template match="@*|node()" priority="-2" mode="M611"><axsl:apply-templates select="@*|*" mode="M611"/></axsl:template>

<!--PATTERN fer53stestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:standard_synchronize" priority="1000" mode="M612">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M612"/></axsl:template><axsl:template match="text()" priority="-1" mode="M612"/><axsl:template match="@*|node()" priority="-2" mode="M612"><axsl:apply-templates select="@*|*" mode="M612"/></axsl:template>

<!--PATTERN fer53steaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:access_system_security" priority="1000" mode="M613">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M613"/></axsl:template><axsl:template match="text()" priority="-1" mode="M613"/><axsl:template match="@*|node()" priority="-2" mode="M613"><axsl:apply-templates select="@*|*" mode="M613"/></axsl:template>

<!--PATTERN fer53stegeneric_read-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:generic_read" priority="1000" mode="M614">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M614"/></axsl:template><axsl:template match="text()" priority="-1" mode="M614"/><axsl:template match="@*|node()" priority="-2" mode="M614"><axsl:apply-templates select="@*|*" mode="M614"/></axsl:template>

<!--PATTERN fer53stegeneric_write-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:generic_write" priority="1000" mode="M615">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M615"/></axsl:template><axsl:template match="text()" priority="-1" mode="M615"/><axsl:template match="@*|node()" priority="-2" mode="M615"><axsl:apply-templates select="@*|*" mode="M615"/></axsl:template>

<!--PATTERN fer53stegeneric_execute-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:generic_execute" priority="1000" mode="M616">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M616"/></axsl:template><axsl:template match="text()" priority="-1" mode="M616"/><axsl:template match="@*|node()" priority="-2" mode="M616"><axsl:apply-templates select="@*|*" mode="M616"/></axsl:template>

<!--PATTERN fer53stegeneric_all-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:generic_all" priority="1000" mode="M617">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M617"/></axsl:template><axsl:template match="text()" priority="-1" mode="M617"/><axsl:template match="@*|node()" priority="-2" mode="M617"><axsl:apply-templates select="@*|*" mode="M617"/></axsl:template>

<!--PATTERN fer53stefile_read_data-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:file_read_data" priority="1000" mode="M618">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_data entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M618"/></axsl:template><axsl:template match="text()" priority="-1" mode="M618"/><axsl:template match="@*|node()" priority="-2" mode="M618"><axsl:apply-templates select="@*|*" mode="M618"/></axsl:template>

<!--PATTERN fer53stefile_write_data-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:file_write_data" priority="1000" mode="M619">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_data entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M619"/></axsl:template><axsl:template match="text()" priority="-1" mode="M619"/><axsl:template match="@*|node()" priority="-2" mode="M619"><axsl:apply-templates select="@*|*" mode="M619"/></axsl:template>

<!--PATTERN fer53stefile_append_data-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:file_append_data" priority="1000" mode="M620">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_append_data entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M620"/></axsl:template><axsl:template match="text()" priority="-1" mode="M620"/><axsl:template match="@*|node()" priority="-2" mode="M620"><axsl:apply-templates select="@*|*" mode="M620"/></axsl:template>

<!--PATTERN fer53stefile_read_ea-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:file_read_ea" priority="1000" mode="M621">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_ea entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M621"/></axsl:template><axsl:template match="text()" priority="-1" mode="M621"/><axsl:template match="@*|node()" priority="-2" mode="M621"><axsl:apply-templates select="@*|*" mode="M621"/></axsl:template>

<!--PATTERN fer53stefile_write_ea-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:file_write_ea" priority="1000" mode="M622">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_ea entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M622"/></axsl:template><axsl:template match="text()" priority="-1" mode="M622"/><axsl:template match="@*|node()" priority="-2" mode="M622"><axsl:apply-templates select="@*|*" mode="M622"/></axsl:template>

<!--PATTERN fer53stefile_execute-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:file_execute" priority="1000" mode="M623">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_execute entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M623"/></axsl:template><axsl:template match="text()" priority="-1" mode="M623"/><axsl:template match="@*|node()" priority="-2" mode="M623"><axsl:apply-templates select="@*|*" mode="M623"/></axsl:template>

<!--PATTERN fer53stefile_delete_child-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:file_delete_child" priority="1000" mode="M624">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_delete_child entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M624"/></axsl:template><axsl:template match="text()" priority="-1" mode="M624"/><axsl:template match="@*|node()" priority="-2" mode="M624"><axsl:apply-templates select="@*|*" mode="M624"/></axsl:template>

<!--PATTERN fer53stefile_read_attributes-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:file_read_attributes" priority="1000" mode="M625">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_attributes entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M625"/></axsl:template><axsl:template match="text()" priority="-1" mode="M625"/><axsl:template match="@*|node()" priority="-2" mode="M625"><axsl:apply-templates select="@*|*" mode="M625"/></axsl:template>

<!--PATTERN fer53stefile_write_attributes-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights53_state/win-def:file_write_attributes" priority="1000" mode="M626">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_attributes entity of a fileeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M626"/></axsl:template><axsl:template match="text()" priority="-1" mode="M626"/><axsl:template match="@*|node()" priority="-2" mode="M626"><axsl:apply-templates select="@*|*" mode="M626"/></axsl:template>

<!--PATTERN fertst-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_test/win-def:object" priority="1001" mode="M627">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:fileeffectiverights_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a fileeffectiverights_test must reference a fileeffectiverights_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M627"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_test/win-def:state" priority="1000" mode="M627">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:fileeffectiverights_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a fileeffectiverights_test must reference a fileeffectiverights_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M627"/></axsl:template><axsl:template match="text()" priority="-1" mode="M627"/><axsl:template match="@*|node()" priority="-2" mode="M627"><axsl:apply-templates select="@*|*" mode="M627"/></axsl:template>

<!--PATTERN fefobjpath-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_object/win-def:path" priority="1000" mode="M628">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileeffectiverights_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M628"/></axsl:template><axsl:template match="text()" priority="-1" mode="M628"/><axsl:template match="@*|node()" priority="-2" mode="M628"><axsl:apply-templates select="@*|*" mode="M628"/></axsl:template>

<!--PATTERN fefobjfilename-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_object/win-def:filename" priority="1000" mode="M629">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileeffectiverights_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M629"/></axsl:template><axsl:template match="text()" priority="-1" mode="M629"/><axsl:template match="@*|node()" priority="-2" mode="M629"><axsl:apply-templates select="@*|*" mode="M629"/></axsl:template>

<!--PATTERN fefobjtrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_object/win-def:trustee_name" priority="1000" mode="M630">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a fileeffectiverights_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M630"/></axsl:template><axsl:template match="text()" priority="-1" mode="M630"/><axsl:template match="@*|node()" priority="-2" mode="M630"><axsl:apply-templates select="@*|*" mode="M630"/></axsl:template>

<!--PATTERN ferstepath-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:path" priority="1000" mode="M631">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileeffectiverights_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M631"/></axsl:template><axsl:template match="text()" priority="-1" mode="M631"/><axsl:template match="@*|node()" priority="-2" mode="M631"><axsl:apply-templates select="@*|*" mode="M631"/></axsl:template>

<!--PATTERN ferstefilename-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:filename" priority="1000" mode="M632">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileeffectiverights_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M632"/></axsl:template><axsl:template match="text()" priority="-1" mode="M632"/><axsl:template match="@*|node()" priority="-2" mode="M632"><axsl:apply-templates select="@*|*" mode="M632"/></axsl:template>

<!--PATTERN ferstetrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:trustee_name" priority="1000" mode="M633">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a fileeffectiverights_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M633"/></axsl:template><axsl:template match="text()" priority="-1" mode="M633"/><axsl:template match="@*|node()" priority="-2" mode="M633"><axsl:apply-templates select="@*|*" mode="M633"/></axsl:template>

<!--PATTERN ferstestandard_delete-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:standard_delete" priority="1000" mode="M634">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M634"/></axsl:template><axsl:template match="text()" priority="-1" mode="M634"/><axsl:template match="@*|node()" priority="-2" mode="M634"><axsl:apply-templates select="@*|*" mode="M634"/></axsl:template>

<!--PATTERN ferstestandard_read_control-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:standard_read_control" priority="1000" mode="M635">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M635"/></axsl:template><axsl:template match="text()" priority="-1" mode="M635"/><axsl:template match="@*|node()" priority="-2" mode="M635"><axsl:apply-templates select="@*|*" mode="M635"/></axsl:template>

<!--PATTERN ferstestandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:standard_write_dac" priority="1000" mode="M636">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M636"/></axsl:template><axsl:template match="text()" priority="-1" mode="M636"/><axsl:template match="@*|node()" priority="-2" mode="M636"><axsl:apply-templates select="@*|*" mode="M636"/></axsl:template>

<!--PATTERN ferstestandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:standard_write_owner" priority="1000" mode="M637">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M637"/></axsl:template><axsl:template match="text()" priority="-1" mode="M637"/><axsl:template match="@*|node()" priority="-2" mode="M637"><axsl:apply-templates select="@*|*" mode="M637"/></axsl:template>

<!--PATTERN ferstestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:standard_synchronize" priority="1000" mode="M638">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M638"/></axsl:template><axsl:template match="text()" priority="-1" mode="M638"/><axsl:template match="@*|node()" priority="-2" mode="M638"><axsl:apply-templates select="@*|*" mode="M638"/></axsl:template>

<!--PATTERN fersteaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:access_system_security" priority="1000" mode="M639">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M639"/></axsl:template><axsl:template match="text()" priority="-1" mode="M639"/><axsl:template match="@*|node()" priority="-2" mode="M639"><axsl:apply-templates select="@*|*" mode="M639"/></axsl:template>

<!--PATTERN ferstegeneric_read-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:generic_read" priority="1000" mode="M640">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M640"/></axsl:template><axsl:template match="text()" priority="-1" mode="M640"/><axsl:template match="@*|node()" priority="-2" mode="M640"><axsl:apply-templates select="@*|*" mode="M640"/></axsl:template>

<!--PATTERN ferstegeneric_write-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:generic_write" priority="1000" mode="M641">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M641"/></axsl:template><axsl:template match="text()" priority="-1" mode="M641"/><axsl:template match="@*|node()" priority="-2" mode="M641"><axsl:apply-templates select="@*|*" mode="M641"/></axsl:template>

<!--PATTERN ferstegeneric_execute-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:generic_execute" priority="1000" mode="M642">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M642"/></axsl:template><axsl:template match="text()" priority="-1" mode="M642"/><axsl:template match="@*|node()" priority="-2" mode="M642"><axsl:apply-templates select="@*|*" mode="M642"/></axsl:template>

<!--PATTERN ferstegeneric_all-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:generic_all" priority="1000" mode="M643">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M643"/></axsl:template><axsl:template match="text()" priority="-1" mode="M643"/><axsl:template match="@*|node()" priority="-2" mode="M643"><axsl:apply-templates select="@*|*" mode="M643"/></axsl:template>

<!--PATTERN ferstefile_read_data-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:file_read_data" priority="1000" mode="M644">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_data entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M644"/></axsl:template><axsl:template match="text()" priority="-1" mode="M644"/><axsl:template match="@*|node()" priority="-2" mode="M644"><axsl:apply-templates select="@*|*" mode="M644"/></axsl:template>

<!--PATTERN ferstefile_write_data-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:file_write_data" priority="1000" mode="M645">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_data entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M645"/></axsl:template><axsl:template match="text()" priority="-1" mode="M645"/><axsl:template match="@*|node()" priority="-2" mode="M645"><axsl:apply-templates select="@*|*" mode="M645"/></axsl:template>

<!--PATTERN ferstefile_append_data-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:file_append_data" priority="1000" mode="M646">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_append_data entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M646"/></axsl:template><axsl:template match="text()" priority="-1" mode="M646"/><axsl:template match="@*|node()" priority="-2" mode="M646"><axsl:apply-templates select="@*|*" mode="M646"/></axsl:template>

<!--PATTERN ferstefile_read_ea-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:file_read_ea" priority="1000" mode="M647">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_ea entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M647"/></axsl:template><axsl:template match="text()" priority="-1" mode="M647"/><axsl:template match="@*|node()" priority="-2" mode="M647"><axsl:apply-templates select="@*|*" mode="M647"/></axsl:template>

<!--PATTERN ferstefile_write_ea-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:file_write_ea" priority="1000" mode="M648">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_ea entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M648"/></axsl:template><axsl:template match="text()" priority="-1" mode="M648"/><axsl:template match="@*|node()" priority="-2" mode="M648"><axsl:apply-templates select="@*|*" mode="M648"/></axsl:template>

<!--PATTERN ferstefile_execute-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:file_execute" priority="1000" mode="M649">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_execute entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M649"/></axsl:template><axsl:template match="text()" priority="-1" mode="M649"/><axsl:template match="@*|node()" priority="-2" mode="M649"><axsl:apply-templates select="@*|*" mode="M649"/></axsl:template>

<!--PATTERN ferstefile_delete_child-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:file_delete_child" priority="1000" mode="M650">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_delete_child entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M650"/></axsl:template><axsl:template match="text()" priority="-1" mode="M650"/><axsl:template match="@*|node()" priority="-2" mode="M650"><axsl:apply-templates select="@*|*" mode="M650"/></axsl:template>

<!--PATTERN ferstefile_read_attributes-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:file_read_attributes" priority="1000" mode="M651">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_attributes entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M651"/></axsl:template><axsl:template match="text()" priority="-1" mode="M651"/><axsl:template match="@*|node()" priority="-2" mode="M651"><axsl:apply-templates select="@*|*" mode="M651"/></axsl:template>

<!--PATTERN ferstefile_write_attributes-->


	<!--RULE -->
<axsl:template match="win-def:fileeffectiverights_state/win-def:file_write_attributes" priority="1000" mode="M652">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_attributes entity of a fileeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M652"/></axsl:template><axsl:template match="text()" priority="-1" mode="M652"/><axsl:template match="@*|node()" priority="-2" mode="M652"><axsl:apply-templates select="@*|*" mode="M652"/></axsl:template>

<!--PATTERN grouptst-->


	<!--RULE -->
<axsl:template match="win-def:group_test/win-def:object" priority="1001" mode="M653">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:group_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a group_test must reference a group_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M653"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:group_test/win-def:state" priority="1000" mode="M653">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:group_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a group_test must reference a group_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M653"/></axsl:template><axsl:template match="text()" priority="-1" mode="M653"/><axsl:template match="@*|node()" priority="-2" mode="M653"><axsl:apply-templates select="@*|*" mode="M653"/></axsl:template>

<!--PATTERN groupobjgroup-->


	<!--RULE -->
<axsl:template match="win-def:group_object/win-def:group" priority="1000" mode="M654">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group entity of a group_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M654"/></axsl:template><axsl:template match="text()" priority="-1" mode="M654"/><axsl:template match="@*|node()" priority="-2" mode="M654"><axsl:apply-templates select="@*|*" mode="M654"/></axsl:template>

<!--PATTERN groupstegroup-->


	<!--RULE -->
<axsl:template match="win-def:group_state/win-def:group" priority="1000" mode="M655">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group entity of a group_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M655"/></axsl:template><axsl:template match="text()" priority="-1" mode="M655"/><axsl:template match="@*|node()" priority="-2" mode="M655"><axsl:apply-templates select="@*|*" mode="M655"/></axsl:template>

<!--PATTERN groupsteuser-->


	<!--RULE -->
<axsl:template match="win-def:group_state/win-def:user" priority="1000" mode="M656">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user entity of a group_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M656"/></axsl:template><axsl:template match="text()" priority="-1" mode="M656"/><axsl:template match="@*|node()" priority="-2" mode="M656"><axsl:apply-templates select="@*|*" mode="M656"/></axsl:template>

<!--PATTERN groupsidtst-->


	<!--RULE -->
<axsl:template match="win-def:group_sid_test/win-def:object" priority="1001" mode="M657">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:group_sid_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a group_sid_test must reference a group_sid_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M657"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:group_sid_test/win-def:state" priority="1000" mode="M657">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:group_sid_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a group_sid_test must reference a group_sid_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M657"/></axsl:template><axsl:template match="text()" priority="-1" mode="M657"/><axsl:template match="@*|node()" priority="-2" mode="M657"><axsl:apply-templates select="@*|*" mode="M657"/></axsl:template>

<!--PATTERN groupsidobjgroup-->


	<!--RULE -->
<axsl:template match="win-def:group_sid_object/win-def:group_sid" priority="1000" mode="M658">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group_sid entity of a group_sid_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M658"/></axsl:template><axsl:template match="text()" priority="-1" mode="M658"/><axsl:template match="@*|node()" priority="-2" mode="M658"><axsl:apply-templates select="@*|*" mode="M658"/></axsl:template>

<!--PATTERN groupsidstegroup-->


	<!--RULE -->
<axsl:template match="win-def:group_sid_state/win-def:group_sid" priority="1000" mode="M659">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group_sid entity of a group_sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M659"/></axsl:template><axsl:template match="text()" priority="-1" mode="M659"/><axsl:template match="@*|node()" priority="-2" mode="M659"><axsl:apply-templates select="@*|*" mode="M659"/></axsl:template>

<!--PATTERN groupsidsteuser-->


	<!--RULE -->
<axsl:template match="win-def:group_sid_state/win-def:user_sid" priority="1000" mode="M660">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_sid entity of a group_sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M660"/></axsl:template><axsl:template match="text()" priority="-1" mode="M660"/><axsl:template match="@*|node()" priority="-2" mode="M660"><axsl:apply-templates select="@*|*" mode="M660"/></axsl:template>

<!--PATTERN wininterfacetst-->


	<!--RULE -->
<axsl:template match="win-def:interface_test/win-def:object" priority="1001" mode="M661">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:interface_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of an interface_test must reference an interface_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M661"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:interface_test/win-def:state" priority="1000" mode="M661">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:interface_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of an interface_test must reference an interface_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M661"/></axsl:template><axsl:template match="text()" priority="-1" mode="M661"/><axsl:template match="@*|node()" priority="-2" mode="M661"><axsl:apply-templates select="@*|*" mode="M661"/></axsl:template>

<!--PATTERN wininterfaceobjname-->


	<!--RULE -->
<axsl:template match="win-def:interface_object/win-def:name" priority="1000" mode="M662">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a interface_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M662"/></axsl:template><axsl:template match="text()" priority="-1" mode="M662"/><axsl:template match="@*|node()" priority="-2" mode="M662"><axsl:apply-templates select="@*|*" mode="M662"/></axsl:template>

<!--PATTERN wininterfacestename-->


	<!--RULE -->
<axsl:template match="win-def:interface_state/win-def:name" priority="1000" mode="M663">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M663"/></axsl:template><axsl:template match="text()" priority="-1" mode="M663"/><axsl:template match="@*|node()" priority="-2" mode="M663"><axsl:apply-templates select="@*|*" mode="M663"/></axsl:template>

<!--PATTERN wininterfacesteindex-->


	<!--RULE -->
<axsl:template match="win-def:interface_state/win-def:index" priority="1000" mode="M664">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the index entity of an interface_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M664"/></axsl:template><axsl:template match="text()" priority="-1" mode="M664"/><axsl:template match="@*|node()" priority="-2" mode="M664"><axsl:apply-templates select="@*|*" mode="M664"/></axsl:template>

<!--PATTERN wininterfacestetype-->


	<!--RULE -->
<axsl:template match="win-def:interface_state/win-def:type" priority="1000" mode="M665">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M665"/></axsl:template><axsl:template match="text()" priority="-1" mode="M665"/><axsl:template match="@*|node()" priority="-2" mode="M665"><axsl:apply-templates select="@*|*" mode="M665"/></axsl:template>

<!--PATTERN wininterfacestehardware_addr-->


	<!--RULE -->
<axsl:template match="win-def:interface_state/win-def:hardware_addr" priority="1000" mode="M666">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hardware_addr entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M666"/></axsl:template><axsl:template match="text()" priority="-1" mode="M666"/><axsl:template match="@*|node()" priority="-2" mode="M666"><axsl:apply-templates select="@*|*" mode="M666"/></axsl:template>

<!--PATTERN wininterfacesteinet_addr-->


	<!--RULE -->
<axsl:template match="win-def:interface_state/win-def:inet_addr" priority="1000" mode="M667">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the inet_addr entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M667"/></axsl:template><axsl:template match="text()" priority="-1" mode="M667"/><axsl:template match="@*|node()" priority="-2" mode="M667"><axsl:apply-templates select="@*|*" mode="M667"/></axsl:template>

<!--PATTERN wininterfacestebroadcast_addr-->


	<!--RULE -->
<axsl:template match="win-def:interface_state/win-def:broadcast_addr" priority="1000" mode="M668">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the broadcast_addr entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M668"/></axsl:template><axsl:template match="text()" priority="-1" mode="M668"/><axsl:template match="@*|node()" priority="-2" mode="M668"><axsl:apply-templates select="@*|*" mode="M668"/></axsl:template>

<!--PATTERN wininterfacestenetmask-->


	<!--RULE -->
<axsl:template match="win-def:interface_state/win-def:netmask" priority="1000" mode="M669">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the netmask entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M669"/></axsl:template><axsl:template match="text()" priority="-1" mode="M669"/><axsl:template match="@*|node()" priority="-2" mode="M669"><axsl:apply-templates select="@*|*" mode="M669"/></axsl:template>

<!--PATTERN wininterfacesteaddr_type-->


	<!--RULE -->
<axsl:template match="win-def:interface_state/win-def:addr_type" priority="1000" mode="M670">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the addr_type entity of an interface_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M670"/></axsl:template><axsl:template match="text()" priority="-1" mode="M670"/><axsl:template match="@*|node()" priority="-2" mode="M670"><axsl:apply-templates select="@*|*" mode="M670"/></axsl:template>

<!--PATTERN lptst-->


	<!--RULE -->
<axsl:template match="win-def:lockoutpolicy_test/win-def:object" priority="1001" mode="M671">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:lockoutpolicy_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a lockoutpolicy_test must reference a lockoutpolicy_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M671"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:lockoutpolicy_test/win-def:state" priority="1000" mode="M671">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:lockoutpolicy_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a lockoutpolicy_test must reference a lockoutpolicy_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M671"/></axsl:template><axsl:template match="text()" priority="-1" mode="M671"/><axsl:template match="@*|node()" priority="-2" mode="M671"><axsl:apply-templates select="@*|*" mode="M671"/></axsl:template>

<!--PATTERN lpsteforce_logoff-->


	<!--RULE -->
<axsl:template match="win-def:lockoutpolicy_state/win-def:force_logoff" priority="1000" mode="M672">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the force_logoff entity of a lockoutpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M672"/></axsl:template><axsl:template match="text()" priority="-1" mode="M672"/><axsl:template match="@*|node()" priority="-2" mode="M672"><axsl:apply-templates select="@*|*" mode="M672"/></axsl:template>

<!--PATTERN lpstelockout_duration-->


	<!--RULE -->
<axsl:template match="win-def:lockoutpolicy_state/win-def:lockout_duration" priority="1000" mode="M673">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the lockout_duration entity of a lockoutpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M673"/></axsl:template><axsl:template match="text()" priority="-1" mode="M673"/><axsl:template match="@*|node()" priority="-2" mode="M673"><axsl:apply-templates select="@*|*" mode="M673"/></axsl:template>

<!--PATTERN lpstelockout_observation_window-->


	<!--RULE -->
<axsl:template match="win-def:lockoutpolicy_state/win-def:lockout_observation_window" priority="1000" mode="M674">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the lockout_observation_window entity of a lockoutpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M674"/></axsl:template><axsl:template match="text()" priority="-1" mode="M674"/><axsl:template match="@*|node()" priority="-2" mode="M674"><axsl:apply-templates select="@*|*" mode="M674"/></axsl:template>

<!--PATTERN lpstelockout_threshold-->


	<!--RULE -->
<axsl:template match="win-def:lockoutpolicy_state/win-def:lockout_threshold" priority="1000" mode="M675">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the lockout_threshold entity of a lockoutpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M675"/></axsl:template><axsl:template match="text()" priority="-1" mode="M675"/><axsl:template match="@*|node()" priority="-2" mode="M675"><axsl:apply-templates select="@*|*" mode="M675"/></axsl:template>

<!--PATTERN metabasetst-->


	<!--RULE -->
<axsl:template match="win-def:metabase_test/win-def:object" priority="1001" mode="M676">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:metabase_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a metabase_test must reference a metabase_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M676"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:metabase_test/win-def:state" priority="1000" mode="M676">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:metabase_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a metabase_test must reference a metabase_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M676"/></axsl:template><axsl:template match="text()" priority="-1" mode="M676"/><axsl:template match="@*|node()" priority="-2" mode="M676"><axsl:apply-templates select="@*|*" mode="M676"/></axsl:template>

<!--PATTERN metabaseobjkey-->


	<!--RULE -->
<axsl:template match="win-def:metabase_object/win-def:key" priority="1000" mode="M677">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a metabase_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M677"/></axsl:template><axsl:template match="text()" priority="-1" mode="M677"/><axsl:template match="@*|node()" priority="-2" mode="M677"><axsl:apply-templates select="@*|*" mode="M677"/></axsl:template>

<!--PATTERN metabaseobjid-->


	<!--RULE -->
<axsl:template match="win-def:metabase_object/win-def:id" priority="1000" mode="M678">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the id entity of a metabase_object should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M678"/></axsl:template><axsl:template match="text()" priority="-1" mode="M678"/><axsl:template match="@*|node()" priority="-2" mode="M678"><axsl:apply-templates select="@*|*" mode="M678"/></axsl:template>

<!--PATTERN metabasestekey-->


	<!--RULE -->
<axsl:template match="win-def:metabase_state/win-def:key" priority="1000" mode="M679">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a metabase_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M679"/></axsl:template><axsl:template match="text()" priority="-1" mode="M679"/><axsl:template match="@*|node()" priority="-2" mode="M679"><axsl:apply-templates select="@*|*" mode="M679"/></axsl:template>

<!--PATTERN metabasesteid-->


	<!--RULE -->
<axsl:template match="win-def:metabase_state/win-def:id" priority="1000" mode="M680">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the id entity of a metabase_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M680"/></axsl:template><axsl:template match="text()" priority="-1" mode="M680"/><axsl:template match="@*|node()" priority="-2" mode="M680"><axsl:apply-templates select="@*|*" mode="M680"/></axsl:template>

<!--PATTERN metabasestename-->


	<!--RULE -->
<axsl:template match="win-def:metabase_state/win-def:name" priority="1000" mode="M681">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a metabase_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M681"/></axsl:template><axsl:template match="text()" priority="-1" mode="M681"/><axsl:template match="@*|node()" priority="-2" mode="M681"><axsl:apply-templates select="@*|*" mode="M681"/></axsl:template>

<!--PATTERN metabasesteuser_type-->


	<!--RULE -->
<axsl:template match="win-def:metabase_state/win-def:user_type" priority="1000" mode="M682">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_type entity of a metabase_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M682"/></axsl:template><axsl:template match="text()" priority="-1" mode="M682"/><axsl:template match="@*|node()" priority="-2" mode="M682"><axsl:apply-templates select="@*|*" mode="M682"/></axsl:template>

<!--PATTERN metabasestedata_type-->


	<!--RULE -->
<axsl:template match="win-def:metabase_state/win-def:data_type" priority="1000" mode="M683">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the data_type entity of a metabase_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M683"/></axsl:template><axsl:template match="text()" priority="-1" mode="M683"/><axsl:template match="@*|node()" priority="-2" mode="M683"><axsl:apply-templates select="@*|*" mode="M683"/></axsl:template>

<!--PATTERN metabasestedata-->


	<!--RULE -->
<axsl:template match="win-def:metabase_state/win-def:data" priority="1000" mode="M684">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M684"/></axsl:template><axsl:template match="text()" priority="-1" mode="M684"/><axsl:template match="@*|node()" priority="-2" mode="M684"><axsl:apply-templates select="@*|*" mode="M684"/></axsl:template>

<!--PATTERN pptst-->


	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_test/win-def:object" priority="1001" mode="M685">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:passwordpolicy_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a passwordpolicy_test must reference a passwordpolicy_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M685"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_test/win-def:state" priority="1000" mode="M685">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:passwordpolicy_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a passwordpolicy_test must reference a passwordpolicy_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M685"/></axsl:template><axsl:template match="text()" priority="-1" mode="M685"/><axsl:template match="@*|node()" priority="-2" mode="M685"><axsl:apply-templates select="@*|*" mode="M685"/></axsl:template>

<!--PATTERN ppstemax_passwd_age-->


	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_state/win-def:max_passwd_age" priority="1000" mode="M686">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the max_passwd_age entity of a passwordpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M686"/></axsl:template><axsl:template match="text()" priority="-1" mode="M686"/><axsl:template match="@*|node()" priority="-2" mode="M686"><axsl:apply-templates select="@*|*" mode="M686"/></axsl:template>

<!--PATTERN ppstemin_passwd_age-->


	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_state/win-def:min_passwd_age" priority="1000" mode="M687">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the min_passwd_age entity of a passwordpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M687"/></axsl:template><axsl:template match="text()" priority="-1" mode="M687"/><axsl:template match="@*|node()" priority="-2" mode="M687"><axsl:apply-templates select="@*|*" mode="M687"/></axsl:template>

<!--PATTERN ppstemin_passwd_len-->


	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_state/win-def:min_passwd_len" priority="1000" mode="M688">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the min_passwd_len entity of a passwordpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M688"/></axsl:template><axsl:template match="text()" priority="-1" mode="M688"/><axsl:template match="@*|node()" priority="-2" mode="M688"><axsl:apply-templates select="@*|*" mode="M688"/></axsl:template>

<!--PATTERN ppstepassword_hist_len-->


	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_state/win-def:password_hist_len" priority="1000" mode="M689">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password_hist_len entity of a passwordpolicy_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M689"/></axsl:template><axsl:template match="text()" priority="-1" mode="M689"/><axsl:template match="@*|node()" priority="-2" mode="M689"><axsl:apply-templates select="@*|*" mode="M689"/></axsl:template>

<!--PATTERN ppstepassword_complexity-->


	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_state/win-def:password_complexity" priority="1000" mode="M690">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password_complexity entity of an passwordpolicy_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M690"/></axsl:template><axsl:template match="text()" priority="-1" mode="M690"/><axsl:template match="@*|node()" priority="-2" mode="M690"><axsl:apply-templates select="@*|*" mode="M690"/></axsl:template>

<!--PATTERN ppstereversible_encryption-->


	<!--RULE -->
<axsl:template match="win-def:passwordpolicy_state/win-def:reversible_encryption" priority="1000" mode="M691">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the reversible_encryption entity of an passwordpolicy_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M691"/></axsl:template><axsl:template match="text()" priority="-1" mode="M691"/><axsl:template match="@*|node()" priority="-2" mode="M691"><axsl:apply-templates select="@*|*" mode="M691"/></axsl:template>

<!--PATTERN winporttst-->


	<!--RULE -->
<axsl:template match="win-def:port_test/win-def:object" priority="1001" mode="M692">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:port_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a port_test must reference a port_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M692"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:port_test/win-def:state" priority="1000" mode="M692">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:port_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a port_test must reference a port_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M692"/></axsl:template><axsl:template match="text()" priority="-1" mode="M692"/><axsl:template match="@*|node()" priority="-2" mode="M692"><axsl:apply-templates select="@*|*" mode="M692"/></axsl:template>

<!--PATTERN winportobjlocal_address-->


	<!--RULE -->
<axsl:template match="win-def:port_object/win-def:local_address" priority="1000" mode="M693">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_address entity of a port_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M693"/></axsl:template><axsl:template match="text()" priority="-1" mode="M693"/><axsl:template match="@*|node()" priority="-2" mode="M693"><axsl:apply-templates select="@*|*" mode="M693"/></axsl:template>

<!--PATTERN winportobjlocal_port-->


	<!--RULE -->
<axsl:template match="win-def:port_object/win-def:local_port" priority="1000" mode="M694">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_port entity of a port_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M694"/></axsl:template><axsl:template match="text()" priority="-1" mode="M694"/><axsl:template match="@*|node()" priority="-2" mode="M694"><axsl:apply-templates select="@*|*" mode="M694"/></axsl:template>

<!--PATTERN winportobjprotocol-->


	<!--RULE -->
<axsl:template match="win-def:port_object/win-def:protocol" priority="1000" mode="M695">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of a port_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M695"/></axsl:template><axsl:template match="text()" priority="-1" mode="M695"/><axsl:template match="@*|node()" priority="-2" mode="M695"><axsl:apply-templates select="@*|*" mode="M695"/></axsl:template>

<!--PATTERN winportstelocal_address-->


	<!--RULE -->
<axsl:template match="win-def:port_state/win-def:local_address" priority="1000" mode="M696">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_address entity of a port_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M696"/></axsl:template><axsl:template match="text()" priority="-1" mode="M696"/><axsl:template match="@*|node()" priority="-2" mode="M696"><axsl:apply-templates select="@*|*" mode="M696"/></axsl:template>

<!--PATTERN winportstelocal_port-->


	<!--RULE -->
<axsl:template match="win-def:port_state/win-def:local_port" priority="1000" mode="M697">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_port entity of a port_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M697"/></axsl:template><axsl:template match="text()" priority="-1" mode="M697"/><axsl:template match="@*|node()" priority="-2" mode="M697"><axsl:apply-templates select="@*|*" mode="M697"/></axsl:template>

<!--PATTERN winportsteprotocol-->


	<!--RULE -->
<axsl:template match="win-def:port_state/win-def:protocol" priority="1000" mode="M698">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of a port_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M698"/></axsl:template><axsl:template match="text()" priority="-1" mode="M698"/><axsl:template match="@*|node()" priority="-2" mode="M698"><axsl:apply-templates select="@*|*" mode="M698"/></axsl:template>

<!--PATTERN winportstepid-->


	<!--RULE -->
<axsl:template match="win-def:port_state/win-def:pid" priority="1000" mode="M699">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of a port_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M699"/></axsl:template><axsl:template match="text()" priority="-1" mode="M699"/><axsl:template match="@*|node()" priority="-2" mode="M699"><axsl:apply-templates select="@*|*" mode="M699"/></axsl:template>

<!--PATTERN pertst-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_test/win-def:object" priority="1001" mode="M700">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:printereffectiverights_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a printereffectiverights_test must reference a printereffectiverights_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M700"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_test/win-def:state" priority="1000" mode="M700">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:printereffectiverights_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a printereffectiverights_test must reference a printereffectiverights_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M700"/></axsl:template><axsl:template match="text()" priority="-1" mode="M700"/><axsl:template match="@*|node()" priority="-2" mode="M700"><axsl:apply-templates select="@*|*" mode="M700"/></axsl:template>

<!--PATTERN perobjprinter_name-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_object/win-def:printer_name" priority="1000" mode="M701">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the kprinter_nameey entity of a printereffectiverights_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M701"/></axsl:template><axsl:template match="text()" priority="-1" mode="M701"/><axsl:template match="@*|node()" priority="-2" mode="M701"><axsl:apply-templates select="@*|*" mode="M701"/></axsl:template>

<!--PATTERN perobjtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_object/win-def:trustee_sid" priority="1000" mode="M702">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a printereffectiverights_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M702"/></axsl:template><axsl:template match="text()" priority="-1" mode="M702"/><axsl:template match="@*|node()" priority="-2" mode="M702"><axsl:apply-templates select="@*|*" mode="M702"/></axsl:template>

<!--PATTERN rersteprinter_name-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:printer_name" priority="1000" mode="M703">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the printer_name entity of a printereffectiverights_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M703"/></axsl:template><axsl:template match="text()" priority="-1" mode="M703"/><axsl:template match="@*|node()" priority="-2" mode="M703"><axsl:apply-templates select="@*|*" mode="M703"/></axsl:template>

<!--PATTERN perstetrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:trustee_sid" priority="1000" mode="M704">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a printereffectiverights_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M704"/></axsl:template><axsl:template match="text()" priority="-1" mode="M704"/><axsl:template match="@*|node()" priority="-2" mode="M704"><axsl:apply-templates select="@*|*" mode="M704"/></axsl:template>

<!--PATTERN perstestandard_delete-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:standard_delete" priority="1000" mode="M705">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M705"/></axsl:template><axsl:template match="text()" priority="-1" mode="M705"/><axsl:template match="@*|node()" priority="-2" mode="M705"><axsl:apply-templates select="@*|*" mode="M705"/></axsl:template>

<!--PATTERN perstestandard_read_control-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:standard_read_control" priority="1000" mode="M706">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M706"/></axsl:template><axsl:template match="text()" priority="-1" mode="M706"/><axsl:template match="@*|node()" priority="-2" mode="M706"><axsl:apply-templates select="@*|*" mode="M706"/></axsl:template>

<!--PATTERN perstestandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:standard_write_dac" priority="1000" mode="M707">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M707"/></axsl:template><axsl:template match="text()" priority="-1" mode="M707"/><axsl:template match="@*|node()" priority="-2" mode="M707"><axsl:apply-templates select="@*|*" mode="M707"/></axsl:template>

<!--PATTERN perstestandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:standard_write_owner" priority="1000" mode="M708">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M708"/></axsl:template><axsl:template match="text()" priority="-1" mode="M708"/><axsl:template match="@*|node()" priority="-2" mode="M708"><axsl:apply-templates select="@*|*" mode="M708"/></axsl:template>

<!--PATTERN perstestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:standard_synchronize" priority="1000" mode="M709">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M709"/></axsl:template><axsl:template match="text()" priority="-1" mode="M709"/><axsl:template match="@*|node()" priority="-2" mode="M709"><axsl:apply-templates select="@*|*" mode="M709"/></axsl:template>

<!--PATTERN persteaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:access_system_security" priority="1000" mode="M710">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M710"/></axsl:template><axsl:template match="text()" priority="-1" mode="M710"/><axsl:template match="@*|node()" priority="-2" mode="M710"><axsl:apply-templates select="@*|*" mode="M710"/></axsl:template>

<!--PATTERN perstegeneric_read-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:generic_read" priority="1000" mode="M711">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M711"/></axsl:template><axsl:template match="text()" priority="-1" mode="M711"/><axsl:template match="@*|node()" priority="-2" mode="M711"><axsl:apply-templates select="@*|*" mode="M711"/></axsl:template>

<!--PATTERN perstegeneric_write-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:generic_write" priority="1000" mode="M712">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M712"/></axsl:template><axsl:template match="text()" priority="-1" mode="M712"/><axsl:template match="@*|node()" priority="-2" mode="M712"><axsl:apply-templates select="@*|*" mode="M712"/></axsl:template>

<!--PATTERN perstegeneric_execute-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:generic_execute" priority="1000" mode="M713">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M713"/></axsl:template><axsl:template match="text()" priority="-1" mode="M713"/><axsl:template match="@*|node()" priority="-2" mode="M713"><axsl:apply-templates select="@*|*" mode="M713"/></axsl:template>

<!--PATTERN perstegeneric_all-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:generic_all" priority="1000" mode="M714">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M714"/></axsl:template><axsl:template match="text()" priority="-1" mode="M714"/><axsl:template match="@*|node()" priority="-2" mode="M714"><axsl:apply-templates select="@*|*" mode="M714"/></axsl:template>

<!--PATTERN persteprinter_access_administer-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:printer_access_administer" priority="1000" mode="M715">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the printer_access_administer entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M715"/></axsl:template><axsl:template match="text()" priority="-1" mode="M715"/><axsl:template match="@*|node()" priority="-2" mode="M715"><axsl:apply-templates select="@*|*" mode="M715"/></axsl:template>

<!--PATTERN persteprinter_access_use-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:printer_access_use" priority="1000" mode="M716">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the printer_access_use entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M716"/></axsl:template><axsl:template match="text()" priority="-1" mode="M716"/><axsl:template match="@*|node()" priority="-2" mode="M716"><axsl:apply-templates select="@*|*" mode="M716"/></axsl:template>

<!--PATTERN perstejob_access_administer-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:job_access_administer" priority="1000" mode="M717">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the job_access_administer entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M717"/></axsl:template><axsl:template match="text()" priority="-1" mode="M717"/><axsl:template match="@*|node()" priority="-2" mode="M717"><axsl:apply-templates select="@*|*" mode="M717"/></axsl:template>

<!--PATTERN perstejob_access_read-->


	<!--RULE -->
<axsl:template match="win-def:printereffectiverights_state/win-def:job_access_read" priority="1000" mode="M718">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the job_access_read entity of a printereffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M718"/></axsl:template><axsl:template match="text()" priority="-1" mode="M718"/><axsl:template match="@*|node()" priority="-2" mode="M718"><axsl:apply-templates select="@*|*" mode="M718"/></axsl:template>

<!--PATTERN processtst-->


	<!--RULE -->
<axsl:template match="win-def:process_test/win-def:object" priority="1001" mode="M719">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:process_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a process_test must reference a process_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M719"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:process_test/win-def:state" priority="1000" mode="M719">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:process_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a process_test must reference a process_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M719"/></axsl:template><axsl:template match="text()" priority="-1" mode="M719"/><axsl:template match="@*|node()" priority="-2" mode="M719"><axsl:apply-templates select="@*|*" mode="M719"/></axsl:template>

<!--PATTERN processobjcommand_line-->


	<!--RULE -->
<axsl:template match="win-def:process_object/win-def:command_line" priority="1000" mode="M720">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the command_line entity of a process_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M720"/></axsl:template><axsl:template match="text()" priority="-1" mode="M720"/><axsl:template match="@*|node()" priority="-2" mode="M720"><axsl:apply-templates select="@*|*" mode="M720"/></axsl:template>

<!--PATTERN processstecommand_line-->


	<!--RULE -->
<axsl:template match="win-def:process_state/win-def:command_line" priority="1000" mode="M721">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the command_line entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M721"/></axsl:template><axsl:template match="text()" priority="-1" mode="M721"/><axsl:template match="@*|node()" priority="-2" mode="M721"><axsl:apply-templates select="@*|*" mode="M721"/></axsl:template>

<!--PATTERN processstepid-->


	<!--RULE -->
<axsl:template match="win-def:port_state/win-def:pid" priority="1000" mode="M722">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of a port_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M722"/></axsl:template><axsl:template match="text()" priority="-1" mode="M722"/><axsl:template match="@*|node()" priority="-2" mode="M722"><axsl:apply-templates select="@*|*" mode="M722"/></axsl:template>

<!--PATTERN processsteppid-->


	<!--RULE -->
<axsl:template match="win-def:port_state/win-def:ppid" priority="1000" mode="M723">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ppid entity of a port_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M723"/></axsl:template><axsl:template match="text()" priority="-1" mode="M723"/><axsl:template match="@*|node()" priority="-2" mode="M723"><axsl:apply-templates select="@*|*" mode="M723"/></axsl:template>

<!--PATTERN processstepriority-->


	<!--RULE -->
<axsl:template match="win-def:process_state/win-def:priority" priority="1000" mode="M724">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the priority entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M724"/></axsl:template><axsl:template match="text()" priority="-1" mode="M724"/><axsl:template match="@*|node()" priority="-2" mode="M724"><axsl:apply-templates select="@*|*" mode="M724"/></axsl:template>

<!--PATTERN processsteimage_path-->


	<!--RULE -->
<axsl:template match="win-def:process_state/win-def:image_path" priority="1000" mode="M725">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the image_path entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M725"/></axsl:template><axsl:template match="text()" priority="-1" mode="M725"/><axsl:template match="@*|node()" priority="-2" mode="M725"><axsl:apply-templates select="@*|*" mode="M725"/></axsl:template>

<!--PATTERN processstecurrent_dir-->


	<!--RULE -->
<axsl:template match="win-def:process_state/win-def:current_dir" priority="1000" mode="M726">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the current_dir entity of a process_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M726"/></axsl:template><axsl:template match="text()" priority="-1" mode="M726"/><axsl:template match="@*|node()" priority="-2" mode="M726"><axsl:apply-templates select="@*|*" mode="M726"/></axsl:template>

<!--PATTERN regtst-->


	<!--RULE -->
<axsl:template match="win-def:registry_test/win-def:object" priority="1001" mode="M727">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:registry_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a registry_test must reference a registry_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M727"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:registry_test/win-def:state" priority="1000" mode="M727">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:registry_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a registry_test must reference a registry_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M727"/></axsl:template><axsl:template match="text()" priority="-1" mode="M727"/><axsl:template match="@*|node()" priority="-2" mode="M727"><axsl:apply-templates select="@*|*" mode="M727"/></axsl:template>

<!--PATTERN regobjhive-->


	<!--RULE -->
<axsl:template match="win-def:registry_object/win-def:hive" priority="1000" mode="M728">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a registry_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M728"/></axsl:template><axsl:template match="text()" priority="-1" mode="M728"/><axsl:template match="@*|node()" priority="-2" mode="M728"><axsl:apply-templates select="@*|*" mode="M728"/></axsl:template>

<!--PATTERN regobjkey-->


	<!--RULE -->
<axsl:template match="win-def:registry_object/win-def:key" priority="1000" mode="M729">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a registry_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@xsi:nil='true') or ../win-def:name/@xsi:nil='true'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - name entity must be nil when key is nil<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M729"/></axsl:template><axsl:template match="text()" priority="-1" mode="M729"/><axsl:template match="@*|node()" priority="-2" mode="M729"><axsl:apply-templates select="@*|*" mode="M729"/></axsl:template>

<!--PATTERN regobjname-->


	<!--RULE -->
<axsl:template match="win-def:registry_object/win-def:name" priority="1000" mode="M730">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a registry_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M730"/></axsl:template><axsl:template match="text()" priority="-1" mode="M730"/><axsl:template match="@*|node()" priority="-2" mode="M730"><axsl:apply-templates select="@*|*" mode="M730"/></axsl:template>

<!--PATTERN regstehive-->


	<!--RULE -->
<axsl:template match="win-def:registry_state/win-def:hive" priority="1000" mode="M731">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a registry_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M731"/></axsl:template><axsl:template match="text()" priority="-1" mode="M731"/><axsl:template match="@*|node()" priority="-2" mode="M731"><axsl:apply-templates select="@*|*" mode="M731"/></axsl:template>

<!--PATTERN regstekey-->


	<!--RULE -->
<axsl:template match="win-def:registry_state/win-def:key" priority="1000" mode="M732">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a registry_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M732"/></axsl:template><axsl:template match="text()" priority="-1" mode="M732"/><axsl:template match="@*|node()" priority="-2" mode="M732"><axsl:apply-templates select="@*|*" mode="M732"/></axsl:template>

<!--PATTERN regstename-->


	<!--RULE -->
<axsl:template match="win-def:registry_state/win-def:name" priority="1000" mode="M733">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a registry_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M733"/></axsl:template><axsl:template match="text()" priority="-1" mode="M733"/><axsl:template match="@*|node()" priority="-2" mode="M733"><axsl:apply-templates select="@*|*" mode="M733"/></axsl:template>

<!--PATTERN regstetype-->


	<!--RULE -->
<axsl:template match="win-def:registry_state/win-def:type" priority="1000" mode="M734">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of a registry_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M734"/></axsl:template><axsl:template match="text()" priority="-1" mode="M734"/><axsl:template match="@*|node()" priority="-2" mode="M734"><axsl:apply-templates select="@*|*" mode="M734"/></axsl:template>

<!--PATTERN regstevalue-->


	<!--RULE -->
<axsl:template match="win-def:registry_state/win-def:value" priority="1000" mode="M735">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M735"/></axsl:template><axsl:template match="text()" priority="-1" mode="M735"/><axsl:template match="@*|node()" priority="-2" mode="M735"><axsl:apply-templates select="@*|*" mode="M735"/></axsl:template>

<!--PATTERN rap53tst-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_test/win-def:object" priority="1001" mode="M736">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:regkeyauditedpermissions53_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a regkeyauditedpermissions53_test must reference a regkeyauditedpermissions53_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M736"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_test/win-def:state" priority="1000" mode="M736">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:regkeyauditedpermissions53_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a regkeyauditedpermissions53_test must reference a regkeyauditedpermissions53_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M736"/></axsl:template><axsl:template match="text()" priority="-1" mode="M736"/><axsl:template match="@*|node()" priority="-2" mode="M736"><axsl:apply-templates select="@*|*" mode="M736"/></axsl:template>

<!--PATTERN rap53objhive-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_object/win-def:hive" priority="1000" mode="M737">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyauditedpermissions53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M737"/></axsl:template><axsl:template match="text()" priority="-1" mode="M737"/><axsl:template match="@*|node()" priority="-2" mode="M737"><axsl:apply-templates select="@*|*" mode="M737"/></axsl:template>

<!--PATTERN rap53objkey-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_object/win-def:key" priority="1000" mode="M738">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyauditedpermissions53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M738"/></axsl:template><axsl:template match="text()" priority="-1" mode="M738"/><axsl:template match="@*|node()" priority="-2" mode="M738"><axsl:apply-templates select="@*|*" mode="M738"/></axsl:template>

<!--PATTERN rap53objtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_object/win-def:trustee_sid" priority="1000" mode="M739">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a regkeyauditedpermissions53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M739"/></axsl:template><axsl:template match="text()" priority="-1" mode="M739"/><axsl:template match="@*|node()" priority="-2" mode="M739"><axsl:apply-templates select="@*|*" mode="M739"/></axsl:template>

<!--PATTERN rap53stehive-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:hive" priority="1000" mode="M740">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M740"/></axsl:template><axsl:template match="text()" priority="-1" mode="M740"/><axsl:template match="@*|node()" priority="-2" mode="M740"><axsl:apply-templates select="@*|*" mode="M740"/></axsl:template>

<!--PATTERN rap53stekey-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key" priority="1000" mode="M741">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M741"/></axsl:template><axsl:template match="text()" priority="-1" mode="M741"/><axsl:template match="@*|node()" priority="-2" mode="M741"><axsl:apply-templates select="@*|*" mode="M741"/></axsl:template>

<!--PATTERN rap53stetrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:trustee_sid" priority="1000" mode="M742">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M742"/></axsl:template><axsl:template match="text()" priority="-1" mode="M742"/><axsl:template match="@*|node()" priority="-2" mode="M742"><axsl:apply-templates select="@*|*" mode="M742"/></axsl:template>

<!--PATTERN rap53stestandard_delete-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_delete" priority="1000" mode="M743">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M743"/></axsl:template><axsl:template match="text()" priority="-1" mode="M743"/><axsl:template match="@*|node()" priority="-2" mode="M743"><axsl:apply-templates select="@*|*" mode="M743"/></axsl:template>

<!--PATTERN rap53stestandard_read_control-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_read_control" priority="1000" mode="M744">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M744"/></axsl:template><axsl:template match="text()" priority="-1" mode="M744"/><axsl:template match="@*|node()" priority="-2" mode="M744"><axsl:apply-templates select="@*|*" mode="M744"/></axsl:template>

<!--PATTERN rap53stestandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_write_dac" priority="1000" mode="M745">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M745"/></axsl:template><axsl:template match="text()" priority="-1" mode="M745"/><axsl:template match="@*|node()" priority="-2" mode="M745"><axsl:apply-templates select="@*|*" mode="M745"/></axsl:template>

<!--PATTERN rap53stestandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_write_owner" priority="1000" mode="M746">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M746"/></axsl:template><axsl:template match="text()" priority="-1" mode="M746"/><axsl:template match="@*|node()" priority="-2" mode="M746"><axsl:apply-templates select="@*|*" mode="M746"/></axsl:template>

<!--PATTERN rap53stestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_synchronize" priority="1000" mode="M747">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M747"/></axsl:template><axsl:template match="text()" priority="-1" mode="M747"/><axsl:template match="@*|node()" priority="-2" mode="M747"><axsl:apply-templates select="@*|*" mode="M747"/></axsl:template>

<!--PATTERN rap53steaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:access_system_security" priority="1000" mode="M748">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M748"/></axsl:template><axsl:template match="text()" priority="-1" mode="M748"/><axsl:template match="@*|node()" priority="-2" mode="M748"><axsl:apply-templates select="@*|*" mode="M748"/></axsl:template>

<!--PATTERN rap53stegeneric_read-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:generic_read" priority="1000" mode="M749">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M749"/></axsl:template><axsl:template match="text()" priority="-1" mode="M749"/><axsl:template match="@*|node()" priority="-2" mode="M749"><axsl:apply-templates select="@*|*" mode="M749"/></axsl:template>

<!--PATTERN rap53stegeneric_write-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:generic_write" priority="1000" mode="M750">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M750"/></axsl:template><axsl:template match="text()" priority="-1" mode="M750"/><axsl:template match="@*|node()" priority="-2" mode="M750"><axsl:apply-templates select="@*|*" mode="M750"/></axsl:template>

<!--PATTERN rap53stegeneric_execute-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:generic_execute" priority="1000" mode="M751">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M751"/></axsl:template><axsl:template match="text()" priority="-1" mode="M751"/><axsl:template match="@*|node()" priority="-2" mode="M751"><axsl:apply-templates select="@*|*" mode="M751"/></axsl:template>

<!--PATTERN rap53stegeneric_all-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:generic_all" priority="1000" mode="M752">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M752"/></axsl:template><axsl:template match="text()" priority="-1" mode="M752"/><axsl:template match="@*|node()" priority="-2" mode="M752"><axsl:apply-templates select="@*|*" mode="M752"/></axsl:template>

<!--PATTERN rap53stekey_query_value-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_query_value" priority="1000" mode="M753">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_query_value entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M753"/></axsl:template><axsl:template match="text()" priority="-1" mode="M753"/><axsl:template match="@*|node()" priority="-2" mode="M753"><axsl:apply-templates select="@*|*" mode="M753"/></axsl:template>

<!--PATTERN rap53stekey_set_value-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_set_value" priority="1000" mode="M754">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_set_value entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M754"/></axsl:template><axsl:template match="text()" priority="-1" mode="M754"/><axsl:template match="@*|node()" priority="-2" mode="M754"><axsl:apply-templates select="@*|*" mode="M754"/></axsl:template>

<!--PATTERN rap53stekey_create_sub_key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_create_sub_key" priority="1000" mode="M755">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M755"/></axsl:template><axsl:template match="text()" priority="-1" mode="M755"/><axsl:template match="@*|node()" priority="-2" mode="M755"><axsl:apply-templates select="@*|*" mode="M755"/></axsl:template>

<!--PATTERN rap53stekey_enumerate_sub_keys-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_enumerate_sub_keys" priority="1000" mode="M756">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M756"/></axsl:template><axsl:template match="text()" priority="-1" mode="M756"/><axsl:template match="@*|node()" priority="-2" mode="M756"><axsl:apply-templates select="@*|*" mode="M756"/></axsl:template>

<!--PATTERN rap53stekey_notify-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_notify" priority="1000" mode="M757">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_notify entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M757"/></axsl:template><axsl:template match="text()" priority="-1" mode="M757"/><axsl:template match="@*|node()" priority="-2" mode="M757"><axsl:apply-templates select="@*|*" mode="M757"/></axsl:template>

<!--PATTERN rap53stekey_create_link-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_create_link" priority="1000" mode="M758">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_link entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M758"/></axsl:template><axsl:template match="text()" priority="-1" mode="M758"/><axsl:template match="@*|node()" priority="-2" mode="M758"><axsl:apply-templates select="@*|*" mode="M758"/></axsl:template>

<!--PATTERN rap53stekey_wow64_64key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_wow64_64key" priority="1000" mode="M759">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M759"/></axsl:template><axsl:template match="text()" priority="-1" mode="M759"/><axsl:template match="@*|node()" priority="-2" mode="M759"><axsl:apply-templates select="@*|*" mode="M759"/></axsl:template>

<!--PATTERN rap53stekey_wow64_32key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_wow64_32key" priority="1000" mode="M760">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M760"/></axsl:template><axsl:template match="text()" priority="-1" mode="M760"/><axsl:template match="@*|node()" priority="-2" mode="M760"><axsl:apply-templates select="@*|*" mode="M760"/></axsl:template>

<!--PATTERN rap53stekey_wow64_res-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_wow64_res" priority="1000" mode="M761">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyauditedpermissions53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M761"/></axsl:template><axsl:template match="text()" priority="-1" mode="M761"/><axsl:template match="@*|node()" priority="-2" mode="M761"><axsl:apply-templates select="@*|*" mode="M761"/></axsl:template>

<!--PATTERN raptst-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_test/win-def:object" priority="1001" mode="M762">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:regkeyauditedpermissions_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a regkeyauditedpermissions_test must reference a regkeyauditedpermissions_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M762"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_test/win-def:state" priority="1000" mode="M762">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:regkeyauditedpermissions_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a regkeyauditedpermissions_test must reference a regkeyauditedpermissions_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M762"/></axsl:template><axsl:template match="text()" priority="-1" mode="M762"/><axsl:template match="@*|node()" priority="-2" mode="M762"><axsl:apply-templates select="@*|*" mode="M762"/></axsl:template>

<!--PATTERN rapobjhive-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_object/win-def:hive" priority="1000" mode="M763">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyauditedpermissions_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M763"/></axsl:template><axsl:template match="text()" priority="-1" mode="M763"/><axsl:template match="@*|node()" priority="-2" mode="M763"><axsl:apply-templates select="@*|*" mode="M763"/></axsl:template>

<!--PATTERN rapobjkey-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_object/win-def:key" priority="1000" mode="M764">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyauditedpermissions_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M764"/></axsl:template><axsl:template match="text()" priority="-1" mode="M764"/><axsl:template match="@*|node()" priority="-2" mode="M764"><axsl:apply-templates select="@*|*" mode="M764"/></axsl:template>

<!--PATTERN rapobjtrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_object/win-def:trustee_name" priority="1000" mode="M765">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a regkeyauditedpermissions_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M765"/></axsl:template><axsl:template match="text()" priority="-1" mode="M765"/><axsl:template match="@*|node()" priority="-2" mode="M765"><axsl:apply-templates select="@*|*" mode="M765"/></axsl:template>

<!--PATTERN rapstehive-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:hive" priority="1000" mode="M766">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M766"/></axsl:template><axsl:template match="text()" priority="-1" mode="M766"/><axsl:template match="@*|node()" priority="-2" mode="M766"><axsl:apply-templates select="@*|*" mode="M766"/></axsl:template>

<!--PATTERN rapstekey-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key" priority="1000" mode="M767">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M767"/></axsl:template><axsl:template match="text()" priority="-1" mode="M767"/><axsl:template match="@*|node()" priority="-2" mode="M767"><axsl:apply-templates select="@*|*" mode="M767"/></axsl:template>

<!--PATTERN rapstetrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:trustee_name" priority="1000" mode="M768">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M768"/></axsl:template><axsl:template match="text()" priority="-1" mode="M768"/><axsl:template match="@*|node()" priority="-2" mode="M768"><axsl:apply-templates select="@*|*" mode="M768"/></axsl:template>

<!--PATTERN rapstestandard_delete-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_delete" priority="1000" mode="M769">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M769"/></axsl:template><axsl:template match="text()" priority="-1" mode="M769"/><axsl:template match="@*|node()" priority="-2" mode="M769"><axsl:apply-templates select="@*|*" mode="M769"/></axsl:template>

<!--PATTERN rapstestandard_read_control-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_read_control" priority="1000" mode="M770">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M770"/></axsl:template><axsl:template match="text()" priority="-1" mode="M770"/><axsl:template match="@*|node()" priority="-2" mode="M770"><axsl:apply-templates select="@*|*" mode="M770"/></axsl:template>

<!--PATTERN rapstestandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_write_dac" priority="1000" mode="M771">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M771"/></axsl:template><axsl:template match="text()" priority="-1" mode="M771"/><axsl:template match="@*|node()" priority="-2" mode="M771"><axsl:apply-templates select="@*|*" mode="M771"/></axsl:template>

<!--PATTERN rapstestandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_write_owner" priority="1000" mode="M772">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M772"/></axsl:template><axsl:template match="text()" priority="-1" mode="M772"/><axsl:template match="@*|node()" priority="-2" mode="M772"><axsl:apply-templates select="@*|*" mode="M772"/></axsl:template>

<!--PATTERN rapstestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_synchronize" priority="1000" mode="M773">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M773"/></axsl:template><axsl:template match="text()" priority="-1" mode="M773"/><axsl:template match="@*|node()" priority="-2" mode="M773"><axsl:apply-templates select="@*|*" mode="M773"/></axsl:template>

<!--PATTERN rapsteaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:access_system_security" priority="1000" mode="M774">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M774"/></axsl:template><axsl:template match="text()" priority="-1" mode="M774"/><axsl:template match="@*|node()" priority="-2" mode="M774"><axsl:apply-templates select="@*|*" mode="M774"/></axsl:template>

<!--PATTERN rapstegeneric_read-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:generic_read" priority="1000" mode="M775">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M775"/></axsl:template><axsl:template match="text()" priority="-1" mode="M775"/><axsl:template match="@*|node()" priority="-2" mode="M775"><axsl:apply-templates select="@*|*" mode="M775"/></axsl:template>

<!--PATTERN rapstegeneric_write-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:generic_write" priority="1000" mode="M776">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M776"/></axsl:template><axsl:template match="text()" priority="-1" mode="M776"/><axsl:template match="@*|node()" priority="-2" mode="M776"><axsl:apply-templates select="@*|*" mode="M776"/></axsl:template>

<!--PATTERN rapstegeneric_execute-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:generic_execute" priority="1000" mode="M777">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M777"/></axsl:template><axsl:template match="text()" priority="-1" mode="M777"/><axsl:template match="@*|node()" priority="-2" mode="M777"><axsl:apply-templates select="@*|*" mode="M777"/></axsl:template>

<!--PATTERN rapstegeneric_all-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:generic_all" priority="1000" mode="M778">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M778"/></axsl:template><axsl:template match="text()" priority="-1" mode="M778"/><axsl:template match="@*|node()" priority="-2" mode="M778"><axsl:apply-templates select="@*|*" mode="M778"/></axsl:template>

<!--PATTERN rapstekey_query_value-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_query_value" priority="1000" mode="M779">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_query_value entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M779"/></axsl:template><axsl:template match="text()" priority="-1" mode="M779"/><axsl:template match="@*|node()" priority="-2" mode="M779"><axsl:apply-templates select="@*|*" mode="M779"/></axsl:template>

<!--PATTERN rapstekey_set_value-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_set_value" priority="1000" mode="M780">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_set_value entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M780"/></axsl:template><axsl:template match="text()" priority="-1" mode="M780"/><axsl:template match="@*|node()" priority="-2" mode="M780"><axsl:apply-templates select="@*|*" mode="M780"/></axsl:template>

<!--PATTERN rapstekey_create_sub_key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_create_sub_key" priority="1000" mode="M781">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M781"/></axsl:template><axsl:template match="text()" priority="-1" mode="M781"/><axsl:template match="@*|node()" priority="-2" mode="M781"><axsl:apply-templates select="@*|*" mode="M781"/></axsl:template>

<!--PATTERN rapstekey_enumerate_sub_keys-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_enumerate_sub_keys" priority="1000" mode="M782">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M782"/></axsl:template><axsl:template match="text()" priority="-1" mode="M782"/><axsl:template match="@*|node()" priority="-2" mode="M782"><axsl:apply-templates select="@*|*" mode="M782"/></axsl:template>

<!--PATTERN rapstekey_notify-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_notify" priority="1000" mode="M783">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_notify entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M783"/></axsl:template><axsl:template match="text()" priority="-1" mode="M783"/><axsl:template match="@*|node()" priority="-2" mode="M783"><axsl:apply-templates select="@*|*" mode="M783"/></axsl:template>

<!--PATTERN rapstekey_create_link-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_create_link" priority="1000" mode="M784">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_link entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M784"/></axsl:template><axsl:template match="text()" priority="-1" mode="M784"/><axsl:template match="@*|node()" priority="-2" mode="M784"><axsl:apply-templates select="@*|*" mode="M784"/></axsl:template>

<!--PATTERN rapstekey_wow64_64key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_wow64_64key" priority="1000" mode="M785">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M785"/></axsl:template><axsl:template match="text()" priority="-1" mode="M785"/><axsl:template match="@*|node()" priority="-2" mode="M785"><axsl:apply-templates select="@*|*" mode="M785"/></axsl:template>

<!--PATTERN rapstekey_wow64_32key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_wow64_32key" priority="1000" mode="M786">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M786"/></axsl:template><axsl:template match="text()" priority="-1" mode="M786"/><axsl:template match="@*|node()" priority="-2" mode="M786"><axsl:apply-templates select="@*|*" mode="M786"/></axsl:template>

<!--PATTERN rapstekey_wow64_res-->


	<!--RULE -->
<axsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_wow64_res" priority="1000" mode="M787">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyauditedpermissions_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M787"/></axsl:template><axsl:template match="text()" priority="-1" mode="M787"/><axsl:template match="@*|node()" priority="-2" mode="M787"><axsl:apply-templates select="@*|*" mode="M787"/></axsl:template>

<!--PATTERN rer53tst-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_test/win-def:object" priority="1001" mode="M788">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:regkeyeffectiverights53_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a regkeyeffectiverights53_test must reference a regkeyeffectiverights53_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M788"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_test/win-def:state" priority="1000" mode="M788">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:regkeyeffectiverights53_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a regkeyeffectiverights53_test must reference a regkeyeffectiverights53_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M788"/></axsl:template><axsl:template match="text()" priority="-1" mode="M788"/><axsl:template match="@*|node()" priority="-2" mode="M788"><axsl:apply-templates select="@*|*" mode="M788"/></axsl:template>

<!--PATTERN rer53objhive-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_object/win-def:hive" priority="1000" mode="M789">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyeffectiverights53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M789"/></axsl:template><axsl:template match="text()" priority="-1" mode="M789"/><axsl:template match="@*|node()" priority="-2" mode="M789"><axsl:apply-templates select="@*|*" mode="M789"/></axsl:template>

<!--PATTERN rer53objkey-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_object/win-def:key" priority="1000" mode="M790">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyeffectiverights53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M790"/></axsl:template><axsl:template match="text()" priority="-1" mode="M790"/><axsl:template match="@*|node()" priority="-2" mode="M790"><axsl:apply-templates select="@*|*" mode="M790"/></axsl:template>

<!--PATTERN rer53objtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_object/win-def:trustee_sid" priority="1000" mode="M791">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a regkeyeffectiverights53_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M791"/></axsl:template><axsl:template match="text()" priority="-1" mode="M791"/><axsl:template match="@*|node()" priority="-2" mode="M791"><axsl:apply-templates select="@*|*" mode="M791"/></axsl:template>

<!--PATTERN rer53stehive-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:hive" priority="1000" mode="M792">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyeffectiverights53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M792"/></axsl:template><axsl:template match="text()" priority="-1" mode="M792"/><axsl:template match="@*|node()" priority="-2" mode="M792"><axsl:apply-templates select="@*|*" mode="M792"/></axsl:template>

<!--PATTERN rer53stekey-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key" priority="1000" mode="M793">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyeffectiverights53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M793"/></axsl:template><axsl:template match="text()" priority="-1" mode="M793"/><axsl:template match="@*|node()" priority="-2" mode="M793"><axsl:apply-templates select="@*|*" mode="M793"/></axsl:template>

<!--PATTERN rer53stetrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:trustee_sid" priority="1000" mode="M794">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a regkeyeffectiverights53_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M794"/></axsl:template><axsl:template match="text()" priority="-1" mode="M794"/><axsl:template match="@*|node()" priority="-2" mode="M794"><axsl:apply-templates select="@*|*" mode="M794"/></axsl:template>

<!--PATTERN rer53stestandard_delete-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_delete" priority="1000" mode="M795">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M795"/></axsl:template><axsl:template match="text()" priority="-1" mode="M795"/><axsl:template match="@*|node()" priority="-2" mode="M795"><axsl:apply-templates select="@*|*" mode="M795"/></axsl:template>

<!--PATTERN rer53stestandard_read_control-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_read_control" priority="1000" mode="M796">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M796"/></axsl:template><axsl:template match="text()" priority="-1" mode="M796"/><axsl:template match="@*|node()" priority="-2" mode="M796"><axsl:apply-templates select="@*|*" mode="M796"/></axsl:template>

<!--PATTERN rer53stestandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_write_dac" priority="1000" mode="M797">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M797"/></axsl:template><axsl:template match="text()" priority="-1" mode="M797"/><axsl:template match="@*|node()" priority="-2" mode="M797"><axsl:apply-templates select="@*|*" mode="M797"/></axsl:template>

<!--PATTERN rer53stestandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_write_owner" priority="1000" mode="M798">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M798"/></axsl:template><axsl:template match="text()" priority="-1" mode="M798"/><axsl:template match="@*|node()" priority="-2" mode="M798"><axsl:apply-templates select="@*|*" mode="M798"/></axsl:template>

<!--PATTERN rer53stestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_synchronize" priority="1000" mode="M799">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M799"/></axsl:template><axsl:template match="text()" priority="-1" mode="M799"/><axsl:template match="@*|node()" priority="-2" mode="M799"><axsl:apply-templates select="@*|*" mode="M799"/></axsl:template>

<!--PATTERN rer53steaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:access_system_security" priority="1000" mode="M800">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M800"/></axsl:template><axsl:template match="text()" priority="-1" mode="M800"/><axsl:template match="@*|node()" priority="-2" mode="M800"><axsl:apply-templates select="@*|*" mode="M800"/></axsl:template>

<!--PATTERN rer53stegeneric_read-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:generic_read" priority="1000" mode="M801">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M801"/></axsl:template><axsl:template match="text()" priority="-1" mode="M801"/><axsl:template match="@*|node()" priority="-2" mode="M801"><axsl:apply-templates select="@*|*" mode="M801"/></axsl:template>

<!--PATTERN rer53stegeneric_write-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:generic_write" priority="1000" mode="M802">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M802"/></axsl:template><axsl:template match="text()" priority="-1" mode="M802"/><axsl:template match="@*|node()" priority="-2" mode="M802"><axsl:apply-templates select="@*|*" mode="M802"/></axsl:template>

<!--PATTERN rer53stegeneric_execute-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:generic_execute" priority="1000" mode="M803">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M803"/></axsl:template><axsl:template match="text()" priority="-1" mode="M803"/><axsl:template match="@*|node()" priority="-2" mode="M803"><axsl:apply-templates select="@*|*" mode="M803"/></axsl:template>

<!--PATTERN rer53stegeneric_all-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:generic_all" priority="1000" mode="M804">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M804"/></axsl:template><axsl:template match="text()" priority="-1" mode="M804"/><axsl:template match="@*|node()" priority="-2" mode="M804"><axsl:apply-templates select="@*|*" mode="M804"/></axsl:template>

<!--PATTERN rer53stekey_query_value-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_query_value" priority="1000" mode="M805">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_query_value entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M805"/></axsl:template><axsl:template match="text()" priority="-1" mode="M805"/><axsl:template match="@*|node()" priority="-2" mode="M805"><axsl:apply-templates select="@*|*" mode="M805"/></axsl:template>

<!--PATTERN rer53stekey_set_value-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_set_value" priority="1000" mode="M806">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_set_value entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M806"/></axsl:template><axsl:template match="text()" priority="-1" mode="M806"/><axsl:template match="@*|node()" priority="-2" mode="M806"><axsl:apply-templates select="@*|*" mode="M806"/></axsl:template>

<!--PATTERN rer53stekey_create_sub_key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_create_sub_key" priority="1000" mode="M807">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M807"/></axsl:template><axsl:template match="text()" priority="-1" mode="M807"/><axsl:template match="@*|node()" priority="-2" mode="M807"><axsl:apply-templates select="@*|*" mode="M807"/></axsl:template>

<!--PATTERN rer53stekey_enumerate_sub_keys-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_enumerate_sub_keys" priority="1000" mode="M808">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M808"/></axsl:template><axsl:template match="text()" priority="-1" mode="M808"/><axsl:template match="@*|node()" priority="-2" mode="M808"><axsl:apply-templates select="@*|*" mode="M808"/></axsl:template>

<!--PATTERN rer53stekey_notify-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_notify" priority="1000" mode="M809">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_notify entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M809"/></axsl:template><axsl:template match="text()" priority="-1" mode="M809"/><axsl:template match="@*|node()" priority="-2" mode="M809"><axsl:apply-templates select="@*|*" mode="M809"/></axsl:template>

<!--PATTERN rer53stekey_create_link-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_create_link" priority="1000" mode="M810">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_link entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M810"/></axsl:template><axsl:template match="text()" priority="-1" mode="M810"/><axsl:template match="@*|node()" priority="-2" mode="M810"><axsl:apply-templates select="@*|*" mode="M810"/></axsl:template>

<!--PATTERN rer53stekey_wow64_64key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_wow64_64key" priority="1000" mode="M811">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M811"/></axsl:template><axsl:template match="text()" priority="-1" mode="M811"/><axsl:template match="@*|node()" priority="-2" mode="M811"><axsl:apply-templates select="@*|*" mode="M811"/></axsl:template>

<!--PATTERN rer53stekey_wow64_32key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_wow64_32key" priority="1000" mode="M812">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M812"/></axsl:template><axsl:template match="text()" priority="-1" mode="M812"/><axsl:template match="@*|node()" priority="-2" mode="M812"><axsl:apply-templates select="@*|*" mode="M812"/></axsl:template>

<!--PATTERN rer53stekey_wow64_res-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_wow64_res" priority="1000" mode="M813">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyeffectiverights53_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M813"/></axsl:template><axsl:template match="text()" priority="-1" mode="M813"/><axsl:template match="@*|node()" priority="-2" mode="M813"><axsl:apply-templates select="@*|*" mode="M813"/></axsl:template>

<!--PATTERN rertst-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_test/win-def:object" priority="1001" mode="M814">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:regkeyeffectiverights_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a regkeyeffectiverights_test must reference a regkeyeffectiverights_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M814"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_test/win-def:state" priority="1000" mode="M814">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:regkeyeffectiverights_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a regkeyeffectiverights_test must reference a regkeyeffectiverights_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M814"/></axsl:template><axsl:template match="text()" priority="-1" mode="M814"/><axsl:template match="@*|node()" priority="-2" mode="M814"><axsl:apply-templates select="@*|*" mode="M814"/></axsl:template>

<!--PATTERN rerobjhive-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_object/win-def:hive" priority="1000" mode="M815">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyeffectiverights_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M815"/></axsl:template><axsl:template match="text()" priority="-1" mode="M815"/><axsl:template match="@*|node()" priority="-2" mode="M815"><axsl:apply-templates select="@*|*" mode="M815"/></axsl:template>

<!--PATTERN rerobjkey-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_object/win-def:key" priority="1000" mode="M816">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyeffectiverights_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M816"/></axsl:template><axsl:template match="text()" priority="-1" mode="M816"/><axsl:template match="@*|node()" priority="-2" mode="M816"><axsl:apply-templates select="@*|*" mode="M816"/></axsl:template>

<!--PATTERN rerobjtrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_object/win-def:trustee_name" priority="1000" mode="M817">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a regkeyeffectiverights_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M817"/></axsl:template><axsl:template match="text()" priority="-1" mode="M817"/><axsl:template match="@*|node()" priority="-2" mode="M817"><axsl:apply-templates select="@*|*" mode="M817"/></axsl:template>

<!--PATTERN rerstehive-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:hive" priority="1000" mode="M818">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyeffectiverights_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M818"/></axsl:template><axsl:template match="text()" priority="-1" mode="M818"/><axsl:template match="@*|node()" priority="-2" mode="M818"><axsl:apply-templates select="@*|*" mode="M818"/></axsl:template>

<!--PATTERN rerstekey-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key" priority="1000" mode="M819">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyeffectiverights_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M819"/></axsl:template><axsl:template match="text()" priority="-1" mode="M819"/><axsl:template match="@*|node()" priority="-2" mode="M819"><axsl:apply-templates select="@*|*" mode="M819"/></axsl:template>

<!--PATTERN rerstetrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:trustee_name" priority="1000" mode="M820">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a regkeyeffectiverights_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M820"/></axsl:template><axsl:template match="text()" priority="-1" mode="M820"/><axsl:template match="@*|node()" priority="-2" mode="M820"><axsl:apply-templates select="@*|*" mode="M820"/></axsl:template>

<!--PATTERN rerstestandard_delete-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_delete" priority="1000" mode="M821">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M821"/></axsl:template><axsl:template match="text()" priority="-1" mode="M821"/><axsl:template match="@*|node()" priority="-2" mode="M821"><axsl:apply-templates select="@*|*" mode="M821"/></axsl:template>

<!--PATTERN rerstestandard_read_control-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_read_control" priority="1000" mode="M822">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M822"/></axsl:template><axsl:template match="text()" priority="-1" mode="M822"/><axsl:template match="@*|node()" priority="-2" mode="M822"><axsl:apply-templates select="@*|*" mode="M822"/></axsl:template>

<!--PATTERN rerstestandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_write_dac" priority="1000" mode="M823">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M823"/></axsl:template><axsl:template match="text()" priority="-1" mode="M823"/><axsl:template match="@*|node()" priority="-2" mode="M823"><axsl:apply-templates select="@*|*" mode="M823"/></axsl:template>

<!--PATTERN rerstestandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_write_owner" priority="1000" mode="M824">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M824"/></axsl:template><axsl:template match="text()" priority="-1" mode="M824"/><axsl:template match="@*|node()" priority="-2" mode="M824"><axsl:apply-templates select="@*|*" mode="M824"/></axsl:template>

<!--PATTERN rerstestandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_synchronize" priority="1000" mode="M825">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M825"/></axsl:template><axsl:template match="text()" priority="-1" mode="M825"/><axsl:template match="@*|node()" priority="-2" mode="M825"><axsl:apply-templates select="@*|*" mode="M825"/></axsl:template>

<!--PATTERN rersteaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:access_system_security" priority="1000" mode="M826">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M826"/></axsl:template><axsl:template match="text()" priority="-1" mode="M826"/><axsl:template match="@*|node()" priority="-2" mode="M826"><axsl:apply-templates select="@*|*" mode="M826"/></axsl:template>

<!--PATTERN rerstegeneric_read-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:generic_read" priority="1000" mode="M827">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M827"/></axsl:template><axsl:template match="text()" priority="-1" mode="M827"/><axsl:template match="@*|node()" priority="-2" mode="M827"><axsl:apply-templates select="@*|*" mode="M827"/></axsl:template>

<!--PATTERN rerstegeneric_write-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:generic_write" priority="1000" mode="M828">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M828"/></axsl:template><axsl:template match="text()" priority="-1" mode="M828"/><axsl:template match="@*|node()" priority="-2" mode="M828"><axsl:apply-templates select="@*|*" mode="M828"/></axsl:template>

<!--PATTERN rerstegeneric_execute-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:generic_execute" priority="1000" mode="M829">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M829"/></axsl:template><axsl:template match="text()" priority="-1" mode="M829"/><axsl:template match="@*|node()" priority="-2" mode="M829"><axsl:apply-templates select="@*|*" mode="M829"/></axsl:template>

<!--PATTERN rerstegeneric_all-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:generic_all" priority="1000" mode="M830">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M830"/></axsl:template><axsl:template match="text()" priority="-1" mode="M830"/><axsl:template match="@*|node()" priority="-2" mode="M830"><axsl:apply-templates select="@*|*" mode="M830"/></axsl:template>

<!--PATTERN rerstekey_query_value-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key_query_value" priority="1000" mode="M831">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_query_value entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M831"/></axsl:template><axsl:template match="text()" priority="-1" mode="M831"/><axsl:template match="@*|node()" priority="-2" mode="M831"><axsl:apply-templates select="@*|*" mode="M831"/></axsl:template>

<!--PATTERN rerstekey_set_value-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key_set_value" priority="1000" mode="M832">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_set_value entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M832"/></axsl:template><axsl:template match="text()" priority="-1" mode="M832"/><axsl:template match="@*|node()" priority="-2" mode="M832"><axsl:apply-templates select="@*|*" mode="M832"/></axsl:template>

<!--PATTERN rerstekey_create_sub_key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key_create_sub_key" priority="1000" mode="M833">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M833"/></axsl:template><axsl:template match="text()" priority="-1" mode="M833"/><axsl:template match="@*|node()" priority="-2" mode="M833"><axsl:apply-templates select="@*|*" mode="M833"/></axsl:template>

<!--PATTERN rerstekey_enumerate_sub_keys-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key_enumerate_sub_keys" priority="1000" mode="M834">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M834"/></axsl:template><axsl:template match="text()" priority="-1" mode="M834"/><axsl:template match="@*|node()" priority="-2" mode="M834"><axsl:apply-templates select="@*|*" mode="M834"/></axsl:template>

<!--PATTERN rerstekey_notify-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key_notify" priority="1000" mode="M835">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_notify entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M835"/></axsl:template><axsl:template match="text()" priority="-1" mode="M835"/><axsl:template match="@*|node()" priority="-2" mode="M835"><axsl:apply-templates select="@*|*" mode="M835"/></axsl:template>

<!--PATTERN rerstekey_create_link-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key_create_link" priority="1000" mode="M836">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_link entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M836"/></axsl:template><axsl:template match="text()" priority="-1" mode="M836"/><axsl:template match="@*|node()" priority="-2" mode="M836"><axsl:apply-templates select="@*|*" mode="M836"/></axsl:template>

<!--PATTERN rerstekey_wow64_64key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key_wow64_64key" priority="1000" mode="M837">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M837"/></axsl:template><axsl:template match="text()" priority="-1" mode="M837"/><axsl:template match="@*|node()" priority="-2" mode="M837"><axsl:apply-templates select="@*|*" mode="M837"/></axsl:template>

<!--PATTERN rerstekey_wow64_32key-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key_wow64_32key" priority="1000" mode="M838">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M838"/></axsl:template><axsl:template match="text()" priority="-1" mode="M838"/><axsl:template match="@*|node()" priority="-2" mode="M838"><axsl:apply-templates select="@*|*" mode="M838"/></axsl:template>

<!--PATTERN rerstekey_wow64_res-->


	<!--RULE -->
<axsl:template match="win-def:regkeyeffectiverights_state/win-def:key_wow64_res" priority="1000" mode="M839">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyeffectiverights_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M839"/></axsl:template><axsl:template match="text()" priority="-1" mode="M839"/><axsl:template match="@*|node()" priority="-2" mode="M839"><axsl:apply-templates select="@*|*" mode="M839"/></axsl:template>

<!--PATTERN srtst-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_test/win-def:object" priority="1001" mode="M840">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:sharedresource_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sharedresource_test must reference a sharedresource_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M840"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:sharedresource_test/win-def:state" priority="1000" mode="M840">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:sharedresource_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sharedresource_test must reference a sharedresource_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M840"/></axsl:template><axsl:template match="text()" priority="-1" mode="M840"/><axsl:template match="@*|node()" priority="-2" mode="M840"><axsl:apply-templates select="@*|*" mode="M840"/></axsl:template>

<!--PATTERN srobjnetname-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_object/win-def:netname" priority="1000" mode="M841">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the netname entity of a sharedresource_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M841"/></axsl:template><axsl:template match="text()" priority="-1" mode="M841"/><axsl:template match="@*|node()" priority="-2" mode="M841"><axsl:apply-templates select="@*|*" mode="M841"/></axsl:template>

<!--PATTERN srstenetname-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:netname" priority="1000" mode="M842">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the netname entity of a sharedresource_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M842"/></axsl:template><axsl:template match="text()" priority="-1" mode="M842"/><axsl:template match="@*|node()" priority="-2" mode="M842"><axsl:apply-templates select="@*|*" mode="M842"/></axsl:template>

<!--PATTERN srsteshared_type-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:shared_type" priority="1000" mode="M843">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the shared_type entity of a sharedresource_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M843"/></axsl:template><axsl:template match="text()" priority="-1" mode="M843"/><axsl:template match="@*|node()" priority="-2" mode="M843"><axsl:apply-templates select="@*|*" mode="M843"/></axsl:template>

<!--PATTERN srstemax_uses-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:max_uses" priority="1000" mode="M844">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the max_uses entity of a sharedresource_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M844"/></axsl:template><axsl:template match="text()" priority="-1" mode="M844"/><axsl:template match="@*|node()" priority="-2" mode="M844"><axsl:apply-templates select="@*|*" mode="M844"/></axsl:template>

<!--PATTERN srstecurrent_uses-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:current_uses" priority="1000" mode="M845">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the current_uses entity of a sharedresource_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M845"/></axsl:template><axsl:template match="text()" priority="-1" mode="M845"/><axsl:template match="@*|node()" priority="-2" mode="M845"><axsl:apply-templates select="@*|*" mode="M845"/></axsl:template>

<!--PATTERN srstelocal_path-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:local_path" priority="1000" mode="M846">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_path entity of a sharedresource_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M846"/></axsl:template><axsl:template match="text()" priority="-1" mode="M846"/><axsl:template match="@*|node()" priority="-2" mode="M846"><axsl:apply-templates select="@*|*" mode="M846"/></axsl:template>

<!--PATTERN srsteaccess_read_permission-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:access_read_permission" priority="1000" mode="M847">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='bool'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_read_permission entity of a sharedresource_state should be 'bool'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M847"/></axsl:template><axsl:template match="text()" priority="-1" mode="M847"/><axsl:template match="@*|node()" priority="-2" mode="M847"><axsl:apply-templates select="@*|*" mode="M847"/></axsl:template>

<!--PATTERN srsteaccess_write_permission-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:access_write_permission" priority="1000" mode="M848">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='bool'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_write_permission entity of a sharedresource_state should be 'bool'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M848"/></axsl:template><axsl:template match="text()" priority="-1" mode="M848"/><axsl:template match="@*|node()" priority="-2" mode="M848"><axsl:apply-templates select="@*|*" mode="M848"/></axsl:template>

<!--PATTERN srsteaccess_create_permission-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:access_create_permission" priority="1000" mode="M849">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='bool'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_create_permission entity of a sharedresource_state should be 'bool'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M849"/></axsl:template><axsl:template match="text()" priority="-1" mode="M849"/><axsl:template match="@*|node()" priority="-2" mode="M849"><axsl:apply-templates select="@*|*" mode="M849"/></axsl:template>

<!--PATTERN srsteaccess_exec_permission-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:access_exec_permission" priority="1000" mode="M850">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='bool'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_exec_permission entity of a sharedresource_state should be 'bool'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M850"/></axsl:template><axsl:template match="text()" priority="-1" mode="M850"/><axsl:template match="@*|node()" priority="-2" mode="M850"><axsl:apply-templates select="@*|*" mode="M850"/></axsl:template>

<!--PATTERN srsteaccess_delete_permission-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:access_delete_permission" priority="1000" mode="M851">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='bool'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_delete_permission entity of a sharedresource_state should be 'bool'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M851"/></axsl:template><axsl:template match="text()" priority="-1" mode="M851"/><axsl:template match="@*|node()" priority="-2" mode="M851"><axsl:apply-templates select="@*|*" mode="M851"/></axsl:template>

<!--PATTERN srsteaccess_atrib_permission-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:access_atrib_permission" priority="1000" mode="M852">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='bool'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_atrib_permission entity of a sharedresource_state should be 'bool'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M852"/></axsl:template><axsl:template match="text()" priority="-1" mode="M852"/><axsl:template match="@*|node()" priority="-2" mode="M852"><axsl:apply-templates select="@*|*" mode="M852"/></axsl:template>

<!--PATTERN srsteaccess_perm_permission-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:access_perm_permission" priority="1000" mode="M853">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='bool'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_perm_permission entity of a sharedresource_state should be 'bool'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M853"/></axsl:template><axsl:template match="text()" priority="-1" mode="M853"/><axsl:template match="@*|node()" priority="-2" mode="M853"><axsl:apply-templates select="@*|*" mode="M853"/></axsl:template>

<!--PATTERN srsteaccess_all_permission-->


	<!--RULE -->
<axsl:template match="win-def:sharedresource_state/win-def:access_all_permission" priority="1000" mode="M854">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='bool'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_all_permission entity of a sharedresource_state should be 'bool'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M854"/></axsl:template><axsl:template match="text()" priority="-1" mode="M854"/><axsl:template match="@*|node()" priority="-2" mode="M854"><axsl:apply-templates select="@*|*" mode="M854"/></axsl:template>

<!--PATTERN sidtst-->


	<!--RULE -->
<axsl:template match="win-def:sid_test/win-def:object" priority="1001" mode="M855">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:sid_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sid_test must reference a sid_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M855"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:sid_test/win-def:state" priority="1000" mode="M855">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:sid_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sid_test must reference a sid_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M855"/></axsl:template><axsl:template match="text()" priority="-1" mode="M855"/><axsl:template match="@*|node()" priority="-2" mode="M855"><axsl:apply-templates select="@*|*" mode="M855"/></axsl:template>

<!--PATTERN sidobjtrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:sid_object/win-def:trustee_name" priority="1000" mode="M856">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a sid_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M856"/></axsl:template><axsl:template match="text()" priority="-1" mode="M856"/><axsl:template match="@*|node()" priority="-2" mode="M856"><axsl:apply-templates select="@*|*" mode="M856"/></axsl:template>

<!--PATTERN sidstetrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:sid_state/win-def:trustee_name" priority="1000" mode="M857">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M857"/></axsl:template><axsl:template match="text()" priority="-1" mode="M857"/><axsl:template match="@*|node()" priority="-2" mode="M857"><axsl:apply-templates select="@*|*" mode="M857"/></axsl:template>

<!--PATTERN sidstetrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:sid_state/win-def:trustee_sid" priority="1000" mode="M858">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M858"/></axsl:template><axsl:template match="text()" priority="-1" mode="M858"/><axsl:template match="@*|node()" priority="-2" mode="M858"><axsl:apply-templates select="@*|*" mode="M858"/></axsl:template>

<!--PATTERN sidstetrustee_domain-->


	<!--RULE -->
<axsl:template match="win-def:sid_state/win-def:trustee_domain" priority="1000" mode="M859">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_domain entity of a sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M859"/></axsl:template><axsl:template match="text()" priority="-1" mode="M859"/><axsl:template match="@*|node()" priority="-2" mode="M859"><axsl:apply-templates select="@*|*" mode="M859"/></axsl:template>

<!--PATTERN sidsidtst-->


	<!--RULE -->
<axsl:template match="win-def:sid_sid_test/win-def:object" priority="1001" mode="M860">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:sid_sid_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a sid_sid_test must reference a sid_sid_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M860"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:sid_sid_test/win-def:state" priority="1000" mode="M860">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:sid_sid_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a sid_sid_test must reference a sid_sid_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M860"/></axsl:template><axsl:template match="text()" priority="-1" mode="M860"/><axsl:template match="@*|node()" priority="-2" mode="M860"><axsl:apply-templates select="@*|*" mode="M860"/></axsl:template>

<!--PATTERN sidsidobjtrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:sid_sid_object/win-def:trustee_sid" priority="1000" mode="M861">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a sid_sid_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M861"/></axsl:template><axsl:template match="text()" priority="-1" mode="M861"/><axsl:template match="@*|node()" priority="-2" mode="M861"><axsl:apply-templates select="@*|*" mode="M861"/></axsl:template>

<!--PATTERN sidsidstetrustee_sid-->


	<!--RULE -->
<axsl:template match="win-def:sid_sid_state/win-def:trustee_sid" priority="1000" mode="M862">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a sid_sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M862"/></axsl:template><axsl:template match="text()" priority="-1" mode="M862"/><axsl:template match="@*|node()" priority="-2" mode="M862"><axsl:apply-templates select="@*|*" mode="M862"/></axsl:template>

<!--PATTERN sidsidstetrustee_name-->


	<!--RULE -->
<axsl:template match="win-def:sid_sid_state/win-def:trustee_name" priority="1000" mode="M863">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a sid_sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M863"/></axsl:template><axsl:template match="text()" priority="-1" mode="M863"/><axsl:template match="@*|node()" priority="-2" mode="M863"><axsl:apply-templates select="@*|*" mode="M863"/></axsl:template>

<!--PATTERN sidsidstetrustee_domain-->


	<!--RULE -->
<axsl:template match="win-def:sid_sid_state/win-def:trustee_domain" priority="1000" mode="M864">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_domain entity of a sid_sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M864"/></axsl:template><axsl:template match="text()" priority="-1" mode="M864"/><axsl:template match="@*|node()" priority="-2" mode="M864"><axsl:apply-templates select="@*|*" mode="M864"/></axsl:template>

<!--PATTERN uactst-->


	<!--RULE -->
<axsl:template match="win-def:uac_test/win-def:object" priority="1001" mode="M865">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:uac_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a uac_test must reference a uac_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M865"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:uac_test/win-def:state" priority="1000" mode="M865">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:uac_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a uac_test must reference a uac_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M865"/></axsl:template><axsl:template match="text()" priority="-1" mode="M865"/><axsl:template match="@*|node()" priority="-2" mode="M865"><axsl:apply-templates select="@*|*" mode="M865"/></axsl:template>

<!--PATTERN uacsteadminapprovalmode-->


	<!--RULE -->
<axsl:template match="win-def:uac_state/win-def:admin_approval_mode" priority="1000" mode="M866">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the admin_approval_mode entity of a uac_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M866"/></axsl:template><axsl:template match="text()" priority="-1" mode="M866"/><axsl:template match="@*|node()" priority="-2" mode="M866"><axsl:apply-templates select="@*|*" mode="M866"/></axsl:template>

<!--PATTERN uacstebehaviorelevationadmin-->


	<!--RULE -->
<axsl:template match="win-def:uac_state/win-def:elevation_prompt_admin" priority="1000" mode="M867">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the elevation_prompt_admin entity of a uac_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M867"/></axsl:template><axsl:template match="text()" priority="-1" mode="M867"/><axsl:template match="@*|node()" priority="-2" mode="M867"><axsl:apply-templates select="@*|*" mode="M867"/></axsl:template>

<!--PATTERN uacsteelevationpromptstandard-->


	<!--RULE -->
<axsl:template match="win-def:uac_state/win-def:elevation_prompt_standard" priority="1000" mode="M868">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the elevation_prompt_standard entity of a uac_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M868"/></axsl:template><axsl:template match="text()" priority="-1" mode="M868"/><axsl:template match="@*|node()" priority="-2" mode="M868"><axsl:apply-templates select="@*|*" mode="M868"/></axsl:template>

<!--PATTERN uacstedetectinstallations-->


	<!--RULE -->
<axsl:template match="win-def:uac_state/win-def:detect_installations" priority="1000" mode="M869">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the detect_installations entity of a uac_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M869"/></axsl:template><axsl:template match="text()" priority="-1" mode="M869"/><axsl:template match="@*|node()" priority="-2" mode="M869"><axsl:apply-templates select="@*|*" mode="M869"/></axsl:template>

<!--PATTERN uacsteelevatesignedexecutables-->


	<!--RULE -->
<axsl:template match="win-def:uac_state/win-def:elevate_signed_executables" priority="1000" mode="M870">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the elevate_signed_executables entity of a uac_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M870"/></axsl:template><axsl:template match="text()" priority="-1" mode="M870"/><axsl:template match="@*|node()" priority="-2" mode="M870"><axsl:apply-templates select="@*|*" mode="M870"/></axsl:template>

<!--PATTERN uacsteelevateuiaccess-->


	<!--RULE -->
<axsl:template match="win-def:uac_state/win-def:elevate_uiaccess" priority="1000" mode="M871">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the elevate_uiaccess entity of a uac_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M871"/></axsl:template><axsl:template match="text()" priority="-1" mode="M871"/><axsl:template match="@*|node()" priority="-2" mode="M871"><axsl:apply-templates select="@*|*" mode="M871"/></axsl:template>

<!--PATTERN uacsterunadminsaam-->


	<!--RULE -->
<axsl:template match="win-def:uac_state/win-def:run_admins_aam" priority="1000" mode="M872">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the run_admins_aam entity of a uac_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M872"/></axsl:template><axsl:template match="text()" priority="-1" mode="M872"/><axsl:template match="@*|node()" priority="-2" mode="M872"><axsl:apply-templates select="@*|*" mode="M872"/></axsl:template>

<!--PATTERN uacstesecuredesktop-->


	<!--RULE -->
<axsl:template match="win-def:uac_state/win-def:secure_desktop" priority="1000" mode="M873">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secure_desktop entity of a uac_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M873"/></axsl:template><axsl:template match="text()" priority="-1" mode="M873"/><axsl:template match="@*|node()" priority="-2" mode="M873"><axsl:apply-templates select="@*|*" mode="M873"/></axsl:template>

<!--PATTERN uacstevirtualizewritefailures-->


	<!--RULE -->
<axsl:template match="win-def:uac_state/win-def:virtualize_write_failures" priority="1000" mode="M874">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the virtualize_write_failures entity of a uac_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M874"/></axsl:template><axsl:template match="text()" priority="-1" mode="M874"/><axsl:template match="@*|node()" priority="-2" mode="M874"><axsl:apply-templates select="@*|*" mode="M874"/></axsl:template>

<!--PATTERN usertst-->


	<!--RULE -->
<axsl:template match="win-def:user_test/win-def:object" priority="1001" mode="M875">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:user_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a user_test must reference a user_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M875"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:user_test/win-def:state" priority="1000" mode="M875">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:user_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a user_test must reference a user_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M875"/></axsl:template><axsl:template match="text()" priority="-1" mode="M875"/><axsl:template match="@*|node()" priority="-2" mode="M875"><axsl:apply-templates select="@*|*" mode="M875"/></axsl:template>

<!--PATTERN userobjuser-->


	<!--RULE -->
<axsl:template match="win-def:user_object/win-def:user" priority="1000" mode="M876">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user entity of a user_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M876"/></axsl:template><axsl:template match="text()" priority="-1" mode="M876"/><axsl:template match="@*|node()" priority="-2" mode="M876"><axsl:apply-templates select="@*|*" mode="M876"/></axsl:template>

<!--PATTERN usersteuser-->


	<!--RULE -->
<axsl:template match="win-def:user_state/win-def:user" priority="1000" mode="M877">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user entity of a user_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M877"/></axsl:template><axsl:template match="text()" priority="-1" mode="M877"/><axsl:template match="@*|node()" priority="-2" mode="M877"><axsl:apply-templates select="@*|*" mode="M877"/></axsl:template>

<!--PATTERN usersteenabled-->


	<!--RULE -->
<axsl:template match="win-def:user_state/win-def:enabled" priority="1000" mode="M878">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the enabled entity of a user_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M878"/></axsl:template><axsl:template match="text()" priority="-1" mode="M878"/><axsl:template match="@*|node()" priority="-2" mode="M878"><axsl:apply-templates select="@*|*" mode="M878"/></axsl:template>

<!--PATTERN userstegroup-->


	<!--RULE -->
<axsl:template match="win-def:user_state/win-def:group" priority="1000" mode="M879">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group entity of a user_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M879"/></axsl:template><axsl:template match="text()" priority="-1" mode="M879"/><axsl:template match="@*|node()" priority="-2" mode="M879"><axsl:apply-templates select="@*|*" mode="M879"/></axsl:template>

<!--PATTERN usersidtst-->


	<!--RULE -->
<axsl:template match="win-def:user_sid_test/win-def:object" priority="1001" mode="M880">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:user_sid_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a user_sid_test must reference a user_sid_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M880"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:user_sid_test/win-def:state" priority="1000" mode="M880">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:user_sid_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a user_sid_test must reference a user_sid_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M880"/></axsl:template><axsl:template match="text()" priority="-1" mode="M880"/><axsl:template match="@*|node()" priority="-2" mode="M880"><axsl:apply-templates select="@*|*" mode="M880"/></axsl:template>

<!--PATTERN usersidobjuser-->


	<!--RULE -->
<axsl:template match="win-def:user_sid_object/win-def:user_sid" priority="1000" mode="M881">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_sid entity of a user_sid_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M881"/></axsl:template><axsl:template match="text()" priority="-1" mode="M881"/><axsl:template match="@*|node()" priority="-2" mode="M881"><axsl:apply-templates select="@*|*" mode="M881"/></axsl:template>

<!--PATTERN usersidsteuser-->


	<!--RULE -->
<axsl:template match="win-def:user_sid_state/win-def:user_sid" priority="1000" mode="M882">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_sid entity of a user_sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M882"/></axsl:template><axsl:template match="text()" priority="-1" mode="M882"/><axsl:template match="@*|node()" priority="-2" mode="M882"><axsl:apply-templates select="@*|*" mode="M882"/></axsl:template>

<!--PATTERN usersidsteenabled-->


	<!--RULE -->
<axsl:template match="win-def:user_sid_state/win-def:enabled" priority="1000" mode="M883">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the enabled entity of a user_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M883"/></axsl:template><axsl:template match="text()" priority="-1" mode="M883"/><axsl:template match="@*|node()" priority="-2" mode="M883"><axsl:apply-templates select="@*|*" mode="M883"/></axsl:template>

<!--PATTERN usersidstegroup-->


	<!--RULE -->
<axsl:template match="win-def:user_sid_state/win-def:group_sid" priority="1000" mode="M884">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group_sid entity of a user_sid_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M884"/></axsl:template><axsl:template match="text()" priority="-1" mode="M884"/><axsl:template match="@*|node()" priority="-2" mode="M884"><axsl:apply-templates select="@*|*" mode="M884"/></axsl:template>

<!--PATTERN volumetst-->


	<!--RULE -->
<axsl:template match="win-def:volume_test/win-def:object" priority="1001" mode="M885">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:volume_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a volume_test must reference a volume_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M885"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:volume_test/win-def:state" priority="1000" mode="M885">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:volume_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a volume_test must reference a volume_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M885"/></axsl:template><axsl:template match="text()" priority="-1" mode="M885"/><axsl:template match="@*|node()" priority="-2" mode="M885"><axsl:apply-templates select="@*|*" mode="M885"/></axsl:template>

<!--PATTERN volobjrootpath-->


	<!--RULE -->
<axsl:template match="win-def:volume_object/win-def:rootpath" priority="1000" mode="M886">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the rootpath entity of a volume_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M886"/></axsl:template><axsl:template match="text()" priority="-1" mode="M886"/><axsl:template match="@*|node()" priority="-2" mode="M886"><axsl:apply-templates select="@*|*" mode="M886"/></axsl:template>

<!--PATTERN volumesterootpath-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:rootpath" priority="1000" mode="M887">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the rootpath entity of a volume_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M887"/></axsl:template><axsl:template match="text()" priority="-1" mode="M887"/><axsl:template match="@*|node()" priority="-2" mode="M887"><axsl:apply-templates select="@*|*" mode="M887"/></axsl:template>

<!--PATTERN volumestefile_system-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_system" priority="1000" mode="M888">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_system entity of a volume_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M888"/></axsl:template><axsl:template match="text()" priority="-1" mode="M888"/><axsl:template match="@*|node()" priority="-2" mode="M888"><axsl:apply-templates select="@*|*" mode="M888"/></axsl:template>

<!--PATTERN volumestename-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:name" priority="1000" mode="M889">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a volume_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M889"/></axsl:template><axsl:template match="text()" priority="-1" mode="M889"/><axsl:template match="@*|node()" priority="-2" mode="M889"><axsl:apply-templates select="@*|*" mode="M889"/></axsl:template>

<!--PATTERN volstevolume_max_component_length-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:volume_max_component_length" priority="1000" mode="M890">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the volume_max_component_length entity of a volume_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M890"/></axsl:template><axsl:template match="text()" priority="-1" mode="M890"/><axsl:template match="@*|node()" priority="-2" mode="M890"><axsl:apply-templates select="@*|*" mode="M890"/></axsl:template>

<!--PATTERN volsteserial_number-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:serial_number" priority="1000" mode="M891">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the serial_number entity of a volume_state should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M891"/></axsl:template><axsl:template match="text()" priority="-1" mode="M891"/><axsl:template match="@*|node()" priority="-2" mode="M891"><axsl:apply-templates select="@*|*" mode="M891"/></axsl:template>

<!--PATTERN volstefile_case_sensitive_search-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_case_sensitive_search" priority="1000" mode="M892">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_case_sensitive_search entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M892"/></axsl:template><axsl:template match="text()" priority="-1" mode="M892"/><axsl:template match="@*|node()" priority="-2" mode="M892"><axsl:apply-templates select="@*|*" mode="M892"/></axsl:template>

<!--PATTERN volstefile_case_preserved_names-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_case_preserved_names" priority="1000" mode="M893">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_case_preserved_names entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M893"/></axsl:template><axsl:template match="text()" priority="-1" mode="M893"/><axsl:template match="@*|node()" priority="-2" mode="M893"><axsl:apply-templates select="@*|*" mode="M893"/></axsl:template>

<!--PATTERN volstefile_unicode_on_disk-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_unicode_on_disk" priority="1000" mode="M894">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_unicode_on_disk entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M894"/></axsl:template><axsl:template match="text()" priority="-1" mode="M894"/><axsl:template match="@*|node()" priority="-2" mode="M894"><axsl:apply-templates select="@*|*" mode="M894"/></axsl:template>

<!--PATTERN volstefile_persistent_acls-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_persistent_acls" priority="1000" mode="M895">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_persistent_acls entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M895"/></axsl:template><axsl:template match="text()" priority="-1" mode="M895"/><axsl:template match="@*|node()" priority="-2" mode="M895"><axsl:apply-templates select="@*|*" mode="M895"/></axsl:template>

<!--PATTERN volstefile_file_compression-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_file_compression" priority="1000" mode="M896">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_file_compression entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M896"/></axsl:template><axsl:template match="text()" priority="-1" mode="M896"/><axsl:template match="@*|node()" priority="-2" mode="M896"><axsl:apply-templates select="@*|*" mode="M896"/></axsl:template>

<!--PATTERN volstefile_volume_quotas-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_volume_quotas" priority="1000" mode="M897">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_volume_quotas entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M897"/></axsl:template><axsl:template match="text()" priority="-1" mode="M897"/><axsl:template match="@*|node()" priority="-2" mode="M897"><axsl:apply-templates select="@*|*" mode="M897"/></axsl:template>

<!--PATTERN volstefile_supports_sparse_files-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_supports_sparse_files" priority="1000" mode="M898">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_sparse_files entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M898"/></axsl:template><axsl:template match="text()" priority="-1" mode="M898"/><axsl:template match="@*|node()" priority="-2" mode="M898"><axsl:apply-templates select="@*|*" mode="M898"/></axsl:template>

<!--PATTERN volstefile_supports_reparse_points-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_supports_reparse_points" priority="1000" mode="M899">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_reparse_points entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M899"/></axsl:template><axsl:template match="text()" priority="-1" mode="M899"/><axsl:template match="@*|node()" priority="-2" mode="M899"><axsl:apply-templates select="@*|*" mode="M899"/></axsl:template>

<!--PATTERN volstefile_supports_remote_storage-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_supports_remote_storage" priority="1000" mode="M900">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_remote_storage entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M900"/></axsl:template><axsl:template match="text()" priority="-1" mode="M900"/><axsl:template match="@*|node()" priority="-2" mode="M900"><axsl:apply-templates select="@*|*" mode="M900"/></axsl:template>

<!--PATTERN volstefile_volume_is_compressed-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_volume_is_compressed" priority="1000" mode="M901">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_volume_is_compressed entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M901"/></axsl:template><axsl:template match="text()" priority="-1" mode="M901"/><axsl:template match="@*|node()" priority="-2" mode="M901"><axsl:apply-templates select="@*|*" mode="M901"/></axsl:template>

<!--PATTERN volstefile_supports_object_ids-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_supports_object_ids" priority="1000" mode="M902">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_object_ids entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M902"/></axsl:template><axsl:template match="text()" priority="-1" mode="M902"/><axsl:template match="@*|node()" priority="-2" mode="M902"><axsl:apply-templates select="@*|*" mode="M902"/></axsl:template>

<!--PATTERN volstefile_supports_encryption-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_supports_encryption" priority="1000" mode="M903">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_encryption entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M903"/></axsl:template><axsl:template match="text()" priority="-1" mode="M903"/><axsl:template match="@*|node()" priority="-2" mode="M903"><axsl:apply-templates select="@*|*" mode="M903"/></axsl:template>

<!--PATTERN volstefile_named_streams-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_named_streams" priority="1000" mode="M904">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_named_streams entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M904"/></axsl:template><axsl:template match="text()" priority="-1" mode="M904"/><axsl:template match="@*|node()" priority="-2" mode="M904"><axsl:apply-templates select="@*|*" mode="M904"/></axsl:template>

<!--PATTERN volstefile_read_only_volume-->


	<!--RULE -->
<axsl:template match="win-def:volume_state/win-def:file_read_only_volume" priority="1000" mode="M905">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_only_volume entity of a volume_state should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M905"/></axsl:template><axsl:template match="text()" priority="-1" mode="M905"/><axsl:template match="@*|node()" priority="-2" mode="M905"><axsl:apply-templates select="@*|*" mode="M905"/></axsl:template>

<!--PATTERN wmitst-->


	<!--RULE -->
<axsl:template match="win-def:wmi_test/win-def:object" priority="1001" mode="M906">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:wmi_object/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the object child element of a wmi_test must reference a wmi_object<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M906"/></axsl:template>

	<!--RULE -->
<axsl:template match="win-def:wmi_test/win-def:state" priority="1000" mode="M906">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:wmi_state/@id"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - the state child element of a wmi_test must reference a wmi_state<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M906"/></axsl:template><axsl:template match="text()" priority="-1" mode="M906"/><axsl:template match="@*|node()" priority="-2" mode="M906"><axsl:apply-templates select="@*|*" mode="M906"/></axsl:template>

<!--PATTERN wmiobjnamespace-->


	<!--RULE -->
<axsl:template match="win-def:wmi_object/win-def:namespace" priority="1000" mode="M907">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the namespace entity of a wmi_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M907"/></axsl:template><axsl:template match="text()" priority="-1" mode="M907"/><axsl:template match="@*|node()" priority="-2" mode="M907"><axsl:apply-templates select="@*|*" mode="M907"/></axsl:template>

<!--PATTERN wmiobjwql-->


	<!--RULE -->
<axsl:template match="win-def:wmi_object/win-def:wql" priority="1000" mode="M908">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the wql entity of a wmi_object should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M908"/></axsl:template><axsl:template match="text()" priority="-1" mode="M908"/><axsl:template match="@*|node()" priority="-2" mode="M908"><axsl:apply-templates select="@*|*" mode="M908"/></axsl:template>

<!--PATTERN wmistenamespace-->


	<!--RULE -->
<axsl:template match="win-def:wmi_state/win-def:namespace" priority="1000" mode="M909">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the namespace entity of a wmi_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M909"/></axsl:template><axsl:template match="text()" priority="-1" mode="M909"/><axsl:template match="@*|node()" priority="-2" mode="M909"><axsl:apply-templates select="@*|*" mode="M909"/></axsl:template>

<!--PATTERN wmistewql-->


	<!--RULE -->
<axsl:template match="win-def:wmi_state/win-def:wql" priority="1000" mode="M910">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the wql entity of a wmi_state should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M910"/></axsl:template><axsl:template match="text()" priority="-1" mode="M910"/><axsl:template match="@*|node()" priority="-2" mode="M910"><axsl:apply-templates select="@*|*" mode="M910"/></axsl:template>

<!--PATTERN wmisteresult-->


	<!--RULE -->
<axsl:template match="win-def:wmi_state/win-def:result" priority="1000" mode="M911">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                        <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*" mode="M911"/></axsl:template><axsl:template match="text()" priority="-1" mode="M911"/><axsl:template match="@*|node()" priority="-2" mode="M911"><axsl:apply-templates select="@*|*" mode="M911"/></axsl:template></axsl:stylesheet>
