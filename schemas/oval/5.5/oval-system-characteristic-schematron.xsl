<?xml version="1.0" standalone="yes"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:iso="http://purl.oclc.org/dsdl/schematron" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:oval-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5" xmlns:ind-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent" xmlns:aix-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#aix" xmlns:apache-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#apache" xmlns:catos-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#catos" xmlns:esx-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#esx" xmlns:freebsd-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#freebsd" xmlns:hpux-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#hpux" xmlns:ios-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#ios" xmlns:linux-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux" xmlns:macos-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#macos" xmlns:pix-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#pix" xmlns:sol-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#solaris" xmlns:unix-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix" xmlns:win-sc="http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows" version="1.0"><!--Implementers: please note that overriding process-prolog or process-root is 
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
<axsl:template match="/"><axsl:apply-templates select="/" mode="M18"/><axsl:apply-templates select="/" mode="M19"/><axsl:apply-templates select="/" mode="M20"/><axsl:apply-templates select="/" mode="M21"/><axsl:apply-templates select="/" mode="M22"/><axsl:apply-templates select="/" mode="M23"/><axsl:apply-templates select="/" mode="M24"/><axsl:apply-templates select="/" mode="M25"/><axsl:apply-templates select="/" mode="M26"/><axsl:apply-templates select="/" mode="M27"/><axsl:apply-templates select="/" mode="M28"/><axsl:apply-templates select="/" mode="M29"/><axsl:apply-templates select="/" mode="M30"/><axsl:apply-templates select="/" mode="M31"/><axsl:apply-templates select="/" mode="M32"/><axsl:apply-templates select="/" mode="M33"/><axsl:apply-templates select="/" mode="M34"/><axsl:apply-templates select="/" mode="M35"/><axsl:apply-templates select="/" mode="M36"/><axsl:apply-templates select="/" mode="M37"/><axsl:apply-templates select="/" mode="M38"/><axsl:apply-templates select="/" mode="M39"/><axsl:apply-templates select="/" mode="M40"/><axsl:apply-templates select="/" mode="M41"/><axsl:apply-templates select="/" mode="M42"/><axsl:apply-templates select="/" mode="M43"/><axsl:apply-templates select="/" mode="M44"/><axsl:apply-templates select="/" mode="M45"/><axsl:apply-templates select="/" mode="M46"/><axsl:apply-templates select="/" mode="M47"/><axsl:apply-templates select="/" mode="M48"/><axsl:apply-templates select="/" mode="M49"/><axsl:apply-templates select="/" mode="M50"/><axsl:apply-templates select="/" mode="M51"/><axsl:apply-templates select="/" mode="M52"/><axsl:apply-templates select="/" mode="M53"/><axsl:apply-templates select="/" mode="M54"/><axsl:apply-templates select="/" mode="M55"/><axsl:apply-templates select="/" mode="M56"/><axsl:apply-templates select="/" mode="M57"/><axsl:apply-templates select="/" mode="M58"/><axsl:apply-templates select="/" mode="M59"/><axsl:apply-templates select="/" mode="M60"/><axsl:apply-templates select="/" mode="M61"/><axsl:apply-templates select="/" mode="M62"/><axsl:apply-templates select="/" mode="M63"/><axsl:apply-templates select="/" mode="M64"/><axsl:apply-templates select="/" mode="M65"/><axsl:apply-templates select="/" mode="M66"/><axsl:apply-templates select="/" mode="M67"/><axsl:apply-templates select="/" mode="M68"/><axsl:apply-templates select="/" mode="M69"/><axsl:apply-templates select="/" mode="M70"/><axsl:apply-templates select="/" mode="M71"/><axsl:apply-templates select="/" mode="M72"/><axsl:apply-templates select="/" mode="M73"/><axsl:apply-templates select="/" mode="M74"/><axsl:apply-templates select="/" mode="M75"/><axsl:apply-templates select="/" mode="M76"/><axsl:apply-templates select="/" mode="M77"/><axsl:apply-templates select="/" mode="M78"/><axsl:apply-templates select="/" mode="M79"/><axsl:apply-templates select="/" mode="M80"/><axsl:apply-templates select="/" mode="M81"/><axsl:apply-templates select="/" mode="M82"/><axsl:apply-templates select="/" mode="M83"/><axsl:apply-templates select="/" mode="M84"/><axsl:apply-templates select="/" mode="M85"/><axsl:apply-templates select="/" mode="M86"/><axsl:apply-templates select="/" mode="M87"/><axsl:apply-templates select="/" mode="M88"/><axsl:apply-templates select="/" mode="M89"/><axsl:apply-templates select="/" mode="M90"/><axsl:apply-templates select="/" mode="M91"/><axsl:apply-templates select="/" mode="M92"/><axsl:apply-templates select="/" mode="M93"/><axsl:apply-templates select="/" mode="M94"/><axsl:apply-templates select="/" mode="M95"/><axsl:apply-templates select="/" mode="M96"/><axsl:apply-templates select="/" mode="M97"/><axsl:apply-templates select="/" mode="M98"/><axsl:apply-templates select="/" mode="M99"/><axsl:apply-templates select="/" mode="M100"/><axsl:apply-templates select="/" mode="M101"/><axsl:apply-templates select="/" mode="M102"/><axsl:apply-templates select="/" mode="M103"/><axsl:apply-templates select="/" mode="M104"/><axsl:apply-templates select="/" mode="M105"/><axsl:apply-templates select="/" mode="M106"/><axsl:apply-templates select="/" mode="M107"/><axsl:apply-templates select="/" mode="M108"/><axsl:apply-templates select="/" mode="M109"/><axsl:apply-templates select="/" mode="M110"/><axsl:apply-templates select="/" mode="M111"/><axsl:apply-templates select="/" mode="M112"/><axsl:apply-templates select="/" mode="M113"/><axsl:apply-templates select="/" mode="M114"/><axsl:apply-templates select="/" mode="M115"/><axsl:apply-templates select="/" mode="M116"/><axsl:apply-templates select="/" mode="M117"/><axsl:apply-templates select="/" mode="M118"/><axsl:apply-templates select="/" mode="M119"/><axsl:apply-templates select="/" mode="M120"/><axsl:apply-templates select="/" mode="M121"/><axsl:apply-templates select="/" mode="M122"/><axsl:apply-templates select="/" mode="M123"/><axsl:apply-templates select="/" mode="M124"/><axsl:apply-templates select="/" mode="M125"/><axsl:apply-templates select="/" mode="M126"/><axsl:apply-templates select="/" mode="M127"/><axsl:apply-templates select="/" mode="M128"/><axsl:apply-templates select="/" mode="M129"/><axsl:apply-templates select="/" mode="M130"/><axsl:apply-templates select="/" mode="M131"/><axsl:apply-templates select="/" mode="M132"/><axsl:apply-templates select="/" mode="M133"/><axsl:apply-templates select="/" mode="M134"/><axsl:apply-templates select="/" mode="M135"/><axsl:apply-templates select="/" mode="M136"/><axsl:apply-templates select="/" mode="M137"/><axsl:apply-templates select="/" mode="M138"/><axsl:apply-templates select="/" mode="M139"/><axsl:apply-templates select="/" mode="M140"/><axsl:apply-templates select="/" mode="M141"/><axsl:apply-templates select="/" mode="M142"/><axsl:apply-templates select="/" mode="M143"/><axsl:apply-templates select="/" mode="M144"/><axsl:apply-templates select="/" mode="M145"/><axsl:apply-templates select="/" mode="M146"/><axsl:apply-templates select="/" mode="M147"/><axsl:apply-templates select="/" mode="M148"/><axsl:apply-templates select="/" mode="M149"/><axsl:apply-templates select="/" mode="M150"/><axsl:apply-templates select="/" mode="M151"/><axsl:apply-templates select="/" mode="M152"/><axsl:apply-templates select="/" mode="M153"/><axsl:apply-templates select="/" mode="M154"/><axsl:apply-templates select="/" mode="M155"/><axsl:apply-templates select="/" mode="M156"/><axsl:apply-templates select="/" mode="M157"/><axsl:apply-templates select="/" mode="M158"/><axsl:apply-templates select="/" mode="M159"/><axsl:apply-templates select="/" mode="M160"/><axsl:apply-templates select="/" mode="M161"/><axsl:apply-templates select="/" mode="M162"/><axsl:apply-templates select="/" mode="M163"/><axsl:apply-templates select="/" mode="M164"/><axsl:apply-templates select="/" mode="M165"/><axsl:apply-templates select="/" mode="M166"/><axsl:apply-templates select="/" mode="M167"/><axsl:apply-templates select="/" mode="M168"/><axsl:apply-templates select="/" mode="M169"/><axsl:apply-templates select="/" mode="M170"/><axsl:apply-templates select="/" mode="M171"/><axsl:apply-templates select="/" mode="M172"/><axsl:apply-templates select="/" mode="M173"/><axsl:apply-templates select="/" mode="M174"/><axsl:apply-templates select="/" mode="M175"/><axsl:apply-templates select="/" mode="M176"/><axsl:apply-templates select="/" mode="M177"/><axsl:apply-templates select="/" mode="M178"/><axsl:apply-templates select="/" mode="M179"/><axsl:apply-templates select="/" mode="M180"/><axsl:apply-templates select="/" mode="M181"/><axsl:apply-templates select="/" mode="M182"/><axsl:apply-templates select="/" mode="M183"/><axsl:apply-templates select="/" mode="M184"/><axsl:apply-templates select="/" mode="M185"/><axsl:apply-templates select="/" mode="M186"/><axsl:apply-templates select="/" mode="M187"/><axsl:apply-templates select="/" mode="M188"/><axsl:apply-templates select="/" mode="M189"/><axsl:apply-templates select="/" mode="M190"/><axsl:apply-templates select="/" mode="M191"/><axsl:apply-templates select="/" mode="M192"/><axsl:apply-templates select="/" mode="M193"/><axsl:apply-templates select="/" mode="M194"/><axsl:apply-templates select="/" mode="M195"/><axsl:apply-templates select="/" mode="M196"/><axsl:apply-templates select="/" mode="M197"/><axsl:apply-templates select="/" mode="M198"/><axsl:apply-templates select="/" mode="M199"/><axsl:apply-templates select="/" mode="M200"/><axsl:apply-templates select="/" mode="M201"/><axsl:apply-templates select="/" mode="M202"/><axsl:apply-templates select="/" mode="M203"/><axsl:apply-templates select="/" mode="M204"/><axsl:apply-templates select="/" mode="M205"/><axsl:apply-templates select="/" mode="M206"/><axsl:apply-templates select="/" mode="M207"/><axsl:apply-templates select="/" mode="M208"/><axsl:apply-templates select="/" mode="M209"/><axsl:apply-templates select="/" mode="M210"/><axsl:apply-templates select="/" mode="M211"/><axsl:apply-templates select="/" mode="M212"/><axsl:apply-templates select="/" mode="M213"/><axsl:apply-templates select="/" mode="M214"/><axsl:apply-templates select="/" mode="M215"/><axsl:apply-templates select="/" mode="M216"/><axsl:apply-templates select="/" mode="M217"/><axsl:apply-templates select="/" mode="M218"/><axsl:apply-templates select="/" mode="M219"/><axsl:apply-templates select="/" mode="M220"/><axsl:apply-templates select="/" mode="M221"/><axsl:apply-templates select="/" mode="M222"/><axsl:apply-templates select="/" mode="M223"/><axsl:apply-templates select="/" mode="M224"/><axsl:apply-templates select="/" mode="M225"/><axsl:apply-templates select="/" mode="M226"/><axsl:apply-templates select="/" mode="M227"/><axsl:apply-templates select="/" mode="M228"/><axsl:apply-templates select="/" mode="M229"/><axsl:apply-templates select="/" mode="M230"/><axsl:apply-templates select="/" mode="M231"/><axsl:apply-templates select="/" mode="M232"/><axsl:apply-templates select="/" mode="M233"/><axsl:apply-templates select="/" mode="M234"/><axsl:apply-templates select="/" mode="M235"/><axsl:apply-templates select="/" mode="M236"/><axsl:apply-templates select="/" mode="M237"/><axsl:apply-templates select="/" mode="M238"/><axsl:apply-templates select="/" mode="M239"/><axsl:apply-templates select="/" mode="M240"/><axsl:apply-templates select="/" mode="M241"/><axsl:apply-templates select="/" mode="M242"/><axsl:apply-templates select="/" mode="M243"/><axsl:apply-templates select="/" mode="M244"/><axsl:apply-templates select="/" mode="M245"/><axsl:apply-templates select="/" mode="M246"/><axsl:apply-templates select="/" mode="M247"/><axsl:apply-templates select="/" mode="M248"/><axsl:apply-templates select="/" mode="M249"/><axsl:apply-templates select="/" mode="M250"/><axsl:apply-templates select="/" mode="M251"/><axsl:apply-templates select="/" mode="M252"/><axsl:apply-templates select="/" mode="M253"/><axsl:apply-templates select="/" mode="M254"/><axsl:apply-templates select="/" mode="M255"/><axsl:apply-templates select="/" mode="M256"/><axsl:apply-templates select="/" mode="M257"/><axsl:apply-templates select="/" mode="M258"/><axsl:apply-templates select="/" mode="M259"/><axsl:apply-templates select="/" mode="M260"/><axsl:apply-templates select="/" mode="M261"/><axsl:apply-templates select="/" mode="M262"/><axsl:apply-templates select="/" mode="M263"/><axsl:apply-templates select="/" mode="M264"/><axsl:apply-templates select="/" mode="M265"/><axsl:apply-templates select="/" mode="M266"/><axsl:apply-templates select="/" mode="M267"/><axsl:apply-templates select="/" mode="M268"/><axsl:apply-templates select="/" mode="M269"/><axsl:apply-templates select="/" mode="M270"/><axsl:apply-templates select="/" mode="M271"/><axsl:apply-templates select="/" mode="M272"/><axsl:apply-templates select="/" mode="M273"/><axsl:apply-templates select="/" mode="M274"/><axsl:apply-templates select="/" mode="M275"/><axsl:apply-templates select="/" mode="M276"/><axsl:apply-templates select="/" mode="M277"/><axsl:apply-templates select="/" mode="M278"/><axsl:apply-templates select="/" mode="M279"/><axsl:apply-templates select="/" mode="M280"/><axsl:apply-templates select="/" mode="M281"/><axsl:apply-templates select="/" mode="M282"/><axsl:apply-templates select="/" mode="M283"/><axsl:apply-templates select="/" mode="M284"/><axsl:apply-templates select="/" mode="M285"/><axsl:apply-templates select="/" mode="M286"/><axsl:apply-templates select="/" mode="M287"/><axsl:apply-templates select="/" mode="M288"/><axsl:apply-templates select="/" mode="M289"/><axsl:apply-templates select="/" mode="M290"/><axsl:apply-templates select="/" mode="M291"/><axsl:apply-templates select="/" mode="M292"/><axsl:apply-templates select="/" mode="M293"/><axsl:apply-templates select="/" mode="M294"/><axsl:apply-templates select="/" mode="M295"/><axsl:apply-templates select="/" mode="M296"/><axsl:apply-templates select="/" mode="M297"/><axsl:apply-templates select="/" mode="M298"/><axsl:apply-templates select="/" mode="M299"/><axsl:apply-templates select="/" mode="M300"/><axsl:apply-templates select="/" mode="M301"/><axsl:apply-templates select="/" mode="M302"/><axsl:apply-templates select="/" mode="M303"/><axsl:apply-templates select="/" mode="M304"/><axsl:apply-templates select="/" mode="M305"/><axsl:apply-templates select="/" mode="M306"/><axsl:apply-templates select="/" mode="M307"/><axsl:apply-templates select="/" mode="M308"/><axsl:apply-templates select="/" mode="M309"/><axsl:apply-templates select="/" mode="M310"/><axsl:apply-templates select="/" mode="M311"/><axsl:apply-templates select="/" mode="M312"/><axsl:apply-templates select="/" mode="M313"/><axsl:apply-templates select="/" mode="M314"/><axsl:apply-templates select="/" mode="M315"/><axsl:apply-templates select="/" mode="M316"/><axsl:apply-templates select="/" mode="M317"/><axsl:apply-templates select="/" mode="M318"/><axsl:apply-templates select="/" mode="M319"/><axsl:apply-templates select="/" mode="M320"/><axsl:apply-templates select="/" mode="M321"/><axsl:apply-templates select="/" mode="M322"/><axsl:apply-templates select="/" mode="M323"/><axsl:apply-templates select="/" mode="M324"/><axsl:apply-templates select="/" mode="M325"/><axsl:apply-templates select="/" mode="M326"/><axsl:apply-templates select="/" mode="M327"/><axsl:apply-templates select="/" mode="M328"/><axsl:apply-templates select="/" mode="M329"/><axsl:apply-templates select="/" mode="M330"/><axsl:apply-templates select="/" mode="M331"/><axsl:apply-templates select="/" mode="M332"/><axsl:apply-templates select="/" mode="M333"/><axsl:apply-templates select="/" mode="M334"/><axsl:apply-templates select="/" mode="M335"/><axsl:apply-templates select="/" mode="M336"/><axsl:apply-templates select="/" mode="M337"/><axsl:apply-templates select="/" mode="M338"/><axsl:apply-templates select="/" mode="M339"/><axsl:apply-templates select="/" mode="M340"/><axsl:apply-templates select="/" mode="M341"/><axsl:apply-templates select="/" mode="M342"/><axsl:apply-templates select="/" mode="M343"/><axsl:apply-templates select="/" mode="M344"/><axsl:apply-templates select="/" mode="M345"/><axsl:apply-templates select="/" mode="M346"/><axsl:apply-templates select="/" mode="M347"/><axsl:apply-templates select="/" mode="M348"/><axsl:apply-templates select="/" mode="M349"/><axsl:apply-templates select="/" mode="M350"/><axsl:apply-templates select="/" mode="M351"/><axsl:apply-templates select="/" mode="M352"/><axsl:apply-templates select="/" mode="M353"/><axsl:apply-templates select="/" mode="M354"/><axsl:apply-templates select="/" mode="M355"/><axsl:apply-templates select="/" mode="M356"/><axsl:apply-templates select="/" mode="M357"/><axsl:apply-templates select="/" mode="M358"/><axsl:apply-templates select="/" mode="M359"/><axsl:apply-templates select="/" mode="M360"/><axsl:apply-templates select="/" mode="M361"/><axsl:apply-templates select="/" mode="M362"/><axsl:apply-templates select="/" mode="M363"/><axsl:apply-templates select="/" mode="M364"/><axsl:apply-templates select="/" mode="M365"/><axsl:apply-templates select="/" mode="M366"/><axsl:apply-templates select="/" mode="M367"/><axsl:apply-templates select="/" mode="M368"/><axsl:apply-templates select="/" mode="M369"/><axsl:apply-templates select="/" mode="M370"/><axsl:apply-templates select="/" mode="M371"/><axsl:apply-templates select="/" mode="M372"/><axsl:apply-templates select="/" mode="M373"/><axsl:apply-templates select="/" mode="M374"/><axsl:apply-templates select="/" mode="M375"/><axsl:apply-templates select="/" mode="M376"/><axsl:apply-templates select="/" mode="M377"/><axsl:apply-templates select="/" mode="M378"/><axsl:apply-templates select="/" mode="M379"/><axsl:apply-templates select="/" mode="M380"/><axsl:apply-templates select="/" mode="M381"/><axsl:apply-templates select="/" mode="M382"/><axsl:apply-templates select="/" mode="M383"/><axsl:apply-templates select="/" mode="M384"/><axsl:apply-templates select="/" mode="M385"/><axsl:apply-templates select="/" mode="M386"/><axsl:apply-templates select="/" mode="M387"/><axsl:apply-templates select="/" mode="M388"/><axsl:apply-templates select="/" mode="M389"/><axsl:apply-templates select="/" mode="M390"/><axsl:apply-templates select="/" mode="M391"/><axsl:apply-templates select="/" mode="M392"/><axsl:apply-templates select="/" mode="M393"/><axsl:apply-templates select="/" mode="M394"/><axsl:apply-templates select="/" mode="M395"/><axsl:apply-templates select="/" mode="M396"/><axsl:apply-templates select="/" mode="M397"/><axsl:apply-templates select="/" mode="M398"/><axsl:apply-templates select="/" mode="M399"/><axsl:apply-templates select="/" mode="M400"/><axsl:apply-templates select="/" mode="M401"/><axsl:apply-templates select="/" mode="M402"/><axsl:apply-templates select="/" mode="M403"/><axsl:apply-templates select="/" mode="M404"/><axsl:apply-templates select="/" mode="M405"/><axsl:apply-templates select="/" mode="M406"/><axsl:apply-templates select="/" mode="M407"/><axsl:apply-templates select="/" mode="M408"/><axsl:apply-templates select="/" mode="M409"/><axsl:apply-templates select="/" mode="M410"/><axsl:apply-templates select="/" mode="M411"/><axsl:apply-templates select="/" mode="M412"/><axsl:apply-templates select="/" mode="M413"/><axsl:apply-templates select="/" mode="M414"/><axsl:apply-templates select="/" mode="M415"/><axsl:apply-templates select="/" mode="M416"/><axsl:apply-templates select="/" mode="M417"/><axsl:apply-templates select="/" mode="M418"/><axsl:apply-templates select="/" mode="M419"/><axsl:apply-templates select="/" mode="M420"/><axsl:apply-templates select="/" mode="M421"/><axsl:apply-templates select="/" mode="M422"/><axsl:apply-templates select="/" mode="M423"/><axsl:apply-templates select="/" mode="M424"/><axsl:apply-templates select="/" mode="M425"/><axsl:apply-templates select="/" mode="M426"/><axsl:apply-templates select="/" mode="M427"/><axsl:apply-templates select="/" mode="M428"/><axsl:apply-templates select="/" mode="M429"/><axsl:apply-templates select="/" mode="M430"/><axsl:apply-templates select="/" mode="M431"/><axsl:apply-templates select="/" mode="M432"/><axsl:apply-templates select="/" mode="M433"/><axsl:apply-templates select="/" mode="M434"/><axsl:apply-templates select="/" mode="M435"/><axsl:apply-templates select="/" mode="M436"/><axsl:apply-templates select="/" mode="M437"/><axsl:apply-templates select="/" mode="M438"/><axsl:apply-templates select="/" mode="M439"/><axsl:apply-templates select="/" mode="M440"/><axsl:apply-templates select="/" mode="M441"/><axsl:apply-templates select="/" mode="M442"/><axsl:apply-templates select="/" mode="M443"/><axsl:apply-templates select="/" mode="M444"/><axsl:apply-templates select="/" mode="M445"/><axsl:apply-templates select="/" mode="M446"/><axsl:apply-templates select="/" mode="M447"/><axsl:apply-templates select="/" mode="M448"/><axsl:apply-templates select="/" mode="M449"/><axsl:apply-templates select="/" mode="M450"/><axsl:apply-templates select="/" mode="M451"/><axsl:apply-templates select="/" mode="M452"/><axsl:apply-templates select="/" mode="M453"/><axsl:apply-templates select="/" mode="M454"/><axsl:apply-templates select="/" mode="M455"/><axsl:apply-templates select="/" mode="M456"/><axsl:apply-templates select="/" mode="M457"/><axsl:apply-templates select="/" mode="M458"/><axsl:apply-templates select="/" mode="M459"/><axsl:apply-templates select="/" mode="M460"/><axsl:apply-templates select="/" mode="M461"/><axsl:apply-templates select="/" mode="M462"/><axsl:apply-templates select="/" mode="M463"/><axsl:apply-templates select="/" mode="M464"/><axsl:apply-templates select="/" mode="M465"/><axsl:apply-templates select="/" mode="M466"/><axsl:apply-templates select="/" mode="M467"/><axsl:apply-templates select="/" mode="M468"/><axsl:apply-templates select="/" mode="M469"/><axsl:apply-templates select="/" mode="M470"/><axsl:apply-templates select="/" mode="M471"/><axsl:apply-templates select="/" mode="M472"/><axsl:apply-templates select="/" mode="M473"/><axsl:apply-templates select="/" mode="M474"/><axsl:apply-templates select="/" mode="M475"/><axsl:apply-templates select="/" mode="M476"/><axsl:apply-templates select="/" mode="M477"/><axsl:apply-templates select="/" mode="M478"/><axsl:apply-templates select="/" mode="M479"/><axsl:apply-templates select="/" mode="M480"/><axsl:apply-templates select="/" mode="M481"/><axsl:apply-templates select="/" mode="M482"/><axsl:apply-templates select="/" mode="M483"/><axsl:apply-templates select="/" mode="M484"/><axsl:apply-templates select="/" mode="M485"/><axsl:apply-templates select="/" mode="M486"/><axsl:apply-templates select="/" mode="M487"/><axsl:apply-templates select="/" mode="M488"/><axsl:apply-templates select="/" mode="M489"/><axsl:apply-templates select="/" mode="M490"/><axsl:apply-templates select="/" mode="M491"/><axsl:apply-templates select="/" mode="M492"/><axsl:apply-templates select="/" mode="M493"/><axsl:apply-templates select="/" mode="M494"/><axsl:apply-templates select="/" mode="M495"/><axsl:apply-templates select="/" mode="M496"/><axsl:apply-templates select="/" mode="M497"/><axsl:apply-templates select="/" mode="M498"/><axsl:apply-templates select="/" mode="M499"/><axsl:apply-templates select="/" mode="M500"/><axsl:apply-templates select="/" mode="M501"/><axsl:apply-templates select="/" mode="M502"/><axsl:apply-templates select="/" mode="M503"/><axsl:apply-templates select="/" mode="M504"/><axsl:apply-templates select="/" mode="M505"/><axsl:apply-templates select="/" mode="M506"/><axsl:apply-templates select="/" mode="M507"/><axsl:apply-templates select="/" mode="M508"/><axsl:apply-templates select="/" mode="M509"/><axsl:apply-templates select="/" mode="M510"/><axsl:apply-templates select="/" mode="M511"/><axsl:apply-templates select="/" mode="M512"/><axsl:apply-templates select="/" mode="M513"/><axsl:apply-templates select="/" mode="M514"/><axsl:apply-templates select="/" mode="M515"/><axsl:apply-templates select="/" mode="M516"/><axsl:apply-templates select="/" mode="M517"/><axsl:apply-templates select="/" mode="M518"/><axsl:apply-templates select="/" mode="M519"/><axsl:apply-templates select="/" mode="M520"/><axsl:apply-templates select="/" mode="M521"/><axsl:apply-templates select="/" mode="M522"/><axsl:apply-templates select="/" mode="M523"/><axsl:apply-templates select="/" mode="M524"/><axsl:apply-templates select="/" mode="M525"/><axsl:apply-templates select="/" mode="M526"/><axsl:apply-templates select="/" mode="M527"/><axsl:apply-templates select="/" mode="M528"/><axsl:apply-templates select="/" mode="M529"/><axsl:apply-templates select="/" mode="M530"/><axsl:apply-templates select="/" mode="M531"/><axsl:apply-templates select="/" mode="M532"/><axsl:apply-templates select="/" mode="M533"/><axsl:apply-templates select="/" mode="M534"/><axsl:apply-templates select="/" mode="M535"/><axsl:apply-templates select="/" mode="M536"/><axsl:apply-templates select="/" mode="M537"/><axsl:apply-templates select="/" mode="M538"/><axsl:apply-templates select="/" mode="M539"/><axsl:apply-templates select="/" mode="M540"/><axsl:apply-templates select="/" mode="M541"/><axsl:apply-templates select="/" mode="M542"/><axsl:apply-templates select="/" mode="M543"/><axsl:apply-templates select="/" mode="M544"/><axsl:apply-templates select="/" mode="M545"/><axsl:apply-templates select="/" mode="M546"/><axsl:apply-templates select="/" mode="M547"/><axsl:apply-templates select="/" mode="M548"/><axsl:apply-templates select="/" mode="M549"/><axsl:apply-templates select="/" mode="M550"/><axsl:apply-templates select="/" mode="M551"/><axsl:apply-templates select="/" mode="M552"/><axsl:apply-templates select="/" mode="M553"/><axsl:apply-templates select="/" mode="M554"/><axsl:apply-templates select="/" mode="M555"/><axsl:apply-templates select="/" mode="M556"/><axsl:apply-templates select="/" mode="M557"/><axsl:apply-templates select="/" mode="M558"/><axsl:apply-templates select="/" mode="M559"/><axsl:apply-templates select="/" mode="M560"/><axsl:apply-templates select="/" mode="M561"/><axsl:apply-templates select="/" mode="M562"/><axsl:apply-templates select="/" mode="M563"/><axsl:apply-templates select="/" mode="M564"/><axsl:apply-templates select="/" mode="M565"/><axsl:apply-templates select="/" mode="M566"/><axsl:apply-templates select="/" mode="M567"/><axsl:apply-templates select="/" mode="M568"/><axsl:apply-templates select="/" mode="M569"/><axsl:apply-templates select="/" mode="M570"/><axsl:apply-templates select="/" mode="M571"/><axsl:apply-templates select="/" mode="M572"/><axsl:apply-templates select="/" mode="M573"/><axsl:apply-templates select="/" mode="M574"/><axsl:apply-templates select="/" mode="M575"/><axsl:apply-templates select="/" mode="M576"/><axsl:apply-templates select="/" mode="M577"/><axsl:apply-templates select="/" mode="M578"/><axsl:apply-templates select="/" mode="M579"/><axsl:apply-templates select="/" mode="M580"/><axsl:apply-templates select="/" mode="M581"/><axsl:apply-templates select="/" mode="M582"/><axsl:apply-templates select="/" mode="M583"/><axsl:apply-templates select="/" mode="M584"/><axsl:apply-templates select="/" mode="M585"/><axsl:apply-templates select="/" mode="M586"/><axsl:apply-templates select="/" mode="M587"/><axsl:apply-templates select="/" mode="M588"/><axsl:apply-templates select="/" mode="M589"/><axsl:apply-templates select="/" mode="M590"/><axsl:apply-templates select="/" mode="M591"/><axsl:apply-templates select="/" mode="M592"/><axsl:apply-templates select="/" mode="M593"/><axsl:apply-templates select="/" mode="M594"/><axsl:apply-templates select="/" mode="M595"/><axsl:apply-templates select="/" mode="M596"/><axsl:apply-templates select="/" mode="M597"/><axsl:apply-templates select="/" mode="M598"/><axsl:apply-templates select="/" mode="M599"/><axsl:apply-templates select="/" mode="M600"/><axsl:apply-templates select="/" mode="M601"/><axsl:apply-templates select="/" mode="M602"/><axsl:apply-templates select="/" mode="M603"/><axsl:apply-templates select="/" mode="M604"/><axsl:apply-templates select="/" mode="M605"/><axsl:apply-templates select="/" mode="M606"/><axsl:apply-templates select="/" mode="M607"/><axsl:apply-templates select="/" mode="M608"/><axsl:apply-templates select="/" mode="M609"/><axsl:apply-templates select="/" mode="M610"/><axsl:apply-templates select="/" mode="M611"/><axsl:apply-templates select="/" mode="M612"/><axsl:apply-templates select="/" mode="M613"/><axsl:apply-templates select="/" mode="M614"/><axsl:apply-templates select="/" mode="M615"/><axsl:apply-templates select="/" mode="M616"/><axsl:apply-templates select="/" mode="M617"/><axsl:apply-templates select="/" mode="M618"/><axsl:apply-templates select="/" mode="M619"/><axsl:apply-templates select="/" mode="M620"/><axsl:apply-templates select="/" mode="M621"/><axsl:apply-templates select="/" mode="M622"/><axsl:apply-templates select="/" mode="M623"/><axsl:apply-templates select="/" mode="M624"/><axsl:apply-templates select="/" mode="M625"/><axsl:apply-templates select="/" mode="M626"/><axsl:apply-templates select="/" mode="M627"/><axsl:apply-templates select="/" mode="M628"/><axsl:apply-templates select="/" mode="M629"/><axsl:apply-templates select="/" mode="M630"/><axsl:apply-templates select="/" mode="M631"/><axsl:apply-templates select="/" mode="M632"/><axsl:apply-templates select="/" mode="M633"/><axsl:apply-templates select="/" mode="M634"/><axsl:apply-templates select="/" mode="M635"/><axsl:apply-templates select="/" mode="M636"/><axsl:apply-templates select="/" mode="M637"/><axsl:apply-templates select="/" mode="M638"/><axsl:apply-templates select="/" mode="M639"/><axsl:apply-templates select="/" mode="M640"/><axsl:apply-templates select="/" mode="M641"/><axsl:apply-templates select="/" mode="M642"/><axsl:apply-templates select="/" mode="M643"/><axsl:apply-templates select="/" mode="M644"/><axsl:apply-templates select="/" mode="M645"/><axsl:apply-templates select="/" mode="M646"/><axsl:apply-templates select="/" mode="M647"/></axsl:template>

<!--SCHEMATRON PATTERNS-->


<!--PATTERN sc_entity_rules-->


	<!--RULE -->
<axsl:template match="oval-sc:system_data/*/*" priority="1000" mode="M18">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@status) or @status='exists' or .=''"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a value for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity should only be supplied if the status attribute is 'exists'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@mask) or @mask='false' or .=''"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - a value for the <axsl:text/><axsl:value-of select="name()"/><axsl:text/> entity should only be supplied if the mask attribute is 'false'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M18"/></axsl:template><axsl:template match="text()" priority="-1" mode="M18"/><axsl:template match="@*|node()" priority="-2" mode="M18"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M18"/></axsl:template>

<!--PATTERN famitemfamily-->


	<!--RULE -->
<axsl:template match="ind-sc:family_item/ind-sc:family" priority="1000" mode="M19">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the family entity of a family_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M19"/></axsl:template><axsl:template match="text()" priority="-1" mode="M19"/><axsl:template match="@*|node()" priority="-2" mode="M19"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M19"/></axsl:template>

<!--PATTERN md5itempath-->


	<!--RULE -->
<axsl:template match="ind-sc:filemd5_item/ind-sc:path" priority="1000" mode="M20">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filemd5_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M20"/></axsl:template><axsl:template match="text()" priority="-1" mode="M20"/><axsl:template match="@*|node()" priority="-2" mode="M20"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M20"/></axsl:template>

<!--PATTERN md5itemfilename-->


	<!--RULE -->
<axsl:template match="ind-sc:filemd5_item/ind-sc:filename" priority="1000" mode="M21">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filemd5_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M21"/></axsl:template><axsl:template match="text()" priority="-1" mode="M21"/><axsl:template match="@*|node()" priority="-2" mode="M21"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M21"/></axsl:template>

<!--PATTERN md5itemmd5-->


	<!--RULE -->
<axsl:template match="ind-sc:filemd5_item/ind-sc:md5" priority="1000" mode="M22">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the md5 entity of a filemd5_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M22"/></axsl:template><axsl:template match="text()" priority="-1" mode="M22"/><axsl:template match="@*|node()" priority="-2" mode="M22"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M22"/></axsl:template>

<!--PATTERN hashitempath-->


	<!--RULE -->
<axsl:template match="ind-sc:filehash_item/ind-sc:path" priority="1000" mode="M23">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a filehash_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M23"/></axsl:template><axsl:template match="text()" priority="-1" mode="M23"/><axsl:template match="@*|node()" priority="-2" mode="M23"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M23"/></axsl:template>

<!--PATTERN hashitemfilename-->


	<!--RULE -->
<axsl:template match="ind-sc:filehash_item/ind-sc:filename" priority="1000" mode="M24">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a filehash_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M24"/></axsl:template><axsl:template match="text()" priority="-1" mode="M24"/><axsl:template match="@*|node()" priority="-2" mode="M24"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M24"/></axsl:template>

<!--PATTERN hashitemmd5-->


	<!--RULE -->
<axsl:template match="ind-sc:filehash_item/ind-sc:md5" priority="1000" mode="M25">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the md5 entity of a filehash_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M25"/></axsl:template><axsl:template match="text()" priority="-1" mode="M25"/><axsl:template match="@*|node()" priority="-2" mode="M25"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M25"/></axsl:template>

<!--PATTERN hashitemsha1-->


	<!--RULE -->
<axsl:template match="ind-sc:filehash_item/ind-sc:sha1" priority="1000" mode="M26">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sha1 entity of a filehash_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M26"/></axsl:template><axsl:template match="text()" priority="-1" mode="M26"/><axsl:template match="@*|node()" priority="-2" mode="M26"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M26"/></axsl:template>

<!--PATTERN envitemname-->


	<!--RULE -->
<axsl:template match="ind-sc:environmentvariable_item/ind-sc:name" priority="1000" mode="M27">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an environmentvariable_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M27"/></axsl:template><axsl:template match="text()" priority="-1" mode="M27"/><axsl:template match="@*|node()" priority="-2" mode="M27"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M27"/></axsl:template>

<!--PATTERN envitemvalue-->


	<!--RULE -->
<axsl:template match="ind-sc:environmentvariable_item/ind-sc:value" priority="1000" mode="M28">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M28"/></axsl:template><axsl:template match="text()" priority="-1" mode="M28"/><axsl:template match="@*|node()" priority="-2" mode="M28"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M28"/></axsl:template>

<!--PATTERN sqlitemengine-->


	<!--RULE -->
<axsl:template match="ind-sc:sql_item/ind-sc:engine" priority="1000" mode="M29">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the engine entity of an sql_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M29"/></axsl:template><axsl:template match="text()" priority="-1" mode="M29"/><axsl:template match="@*|node()" priority="-2" mode="M29"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M29"/></axsl:template>

<!--PATTERN sqlitemversion-->


	<!--RULE -->
<axsl:template match="ind-sc:sql_item/ind-sc:version" priority="1000" mode="M30">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of an sql_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M30"/></axsl:template><axsl:template match="text()" priority="-1" mode="M30"/><axsl:template match="@*|node()" priority="-2" mode="M30"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M30"/></axsl:template>

<!--PATTERN sqlitemconnection_string-->


	<!--RULE -->
<axsl:template match="ind-sc:sql_item/ind-sc:connection_string" priority="1000" mode="M31">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the connection_string entity of an sql_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M31"/></axsl:template><axsl:template match="text()" priority="-1" mode="M31"/><axsl:template match="@*|node()" priority="-2" mode="M31"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M31"/></axsl:template>

<!--PATTERN sqlitemsql-->


	<!--RULE -->
<axsl:template match="ind-sc:sql_item/ind-sc:sql" priority="1000" mode="M32">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sql entity of an sql_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M32"/></axsl:template><axsl:template match="text()" priority="-1" mode="M32"/><axsl:template match="@*|node()" priority="-2" mode="M32"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M32"/></axsl:template>

<!--PATTERN sqlitemresult-->


	<!--RULE -->
<axsl:template match="ind-sc:sql_item/ind-sc:result" priority="1000" mode="M33">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M33"/></axsl:template><axsl:template match="text()" priority="-1" mode="M33"/><axsl:template match="@*|node()" priority="-2" mode="M33"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M33"/></axsl:template>

<!--PATTERN txtitempath-->


	<!--RULE -->
<axsl:template match="ind-sc:textfilecontent_item/ind-sc:path" priority="1000" mode="M34">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a textfilecontent_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M34"/></axsl:template><axsl:template match="text()" priority="-1" mode="M34"/><axsl:template match="@*|node()" priority="-2" mode="M34"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M34"/></axsl:template>

<!--PATTERN txtitemfilename-->


	<!--RULE -->
<axsl:template match="ind-sc:textfilecontent_item/ind-sc:filename" priority="1000" mode="M35">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a textfilecontent_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M35"/></axsl:template><axsl:template match="text()" priority="-1" mode="M35"/><axsl:template match="@*|node()" priority="-2" mode="M35"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M35"/></axsl:template>

<!--PATTERN txtitempattern-->


	<!--RULE -->
<axsl:template match="ind-sc:textfilecontent_item/ind-sc:pattern" priority="1000" mode="M36">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pattern entity of a textfilecontent_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M36"/></axsl:template><axsl:template match="text()" priority="-1" mode="M36"/><axsl:template match="@*|node()" priority="-2" mode="M36"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M36"/></axsl:template>

<!--PATTERN txtiteminstance-->


	<!--RULE -->
<axsl:template match="ind-sc:textfilecontent_item/ind-sc:instance" priority="1000" mode="M37">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the instance entity of a textfilecontent_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M37"/></axsl:template><axsl:template match="text()" priority="-1" mode="M37"/><axsl:template match="@*|node()" priority="-2" mode="M37"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M37"/></axsl:template>

<!--PATTERN txtitemline-->


	<!--RULE -->
<axsl:template match="ind-sc:textfilecontent_item/ind-sc:line" priority="1000" mode="M38">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the line entity of a textfilecontent_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M38"/></axsl:template><axsl:template match="text()" priority="-1" mode="M38"/><axsl:template match="@*|node()" priority="-2" mode="M38"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M38"/></axsl:template>

<!--PATTERN txtitemtext-->


	<!--RULE -->
<axsl:template match="ind-sc:textfilecontent_item/ind-sc:text" priority="1000" mode="M39">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the text entity of a textfilecontent_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M39"/></axsl:template><axsl:template match="text()" priority="-1" mode="M39"/><axsl:template match="@*|node()" priority="-2" mode="M39"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M39"/></axsl:template>

<!--PATTERN txtitemsubexpression-->


	<!--RULE -->
<axsl:template match="ind-sc:textfilecontent_item/ind-sc:subexpression" priority="1000" mode="M40">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M40"/></axsl:template><axsl:template match="text()" priority="-1" mode="M40"/><axsl:template match="@*|node()" priority="-2" mode="M40"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M40"/></axsl:template>

<!--PATTERN varitemvar_ref-->


	<!--RULE -->
<axsl:template match="ind-sc:variable_item/ind-sc:var_ref" priority="1000" mode="M41">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the var_ref entity of a variable_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M41"/></axsl:template><axsl:template match="text()" priority="-1" mode="M41"/><axsl:template match="@*|node()" priority="-2" mode="M41"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M41"/></axsl:template>

<!--PATTERN varitemvalue-->


	<!--RULE -->
<axsl:template match="ind-sc:variable_item/ind-sc:value" priority="1000" mode="M42">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M42"/></axsl:template><axsl:template match="text()" priority="-1" mode="M42"/><axsl:template match="@*|node()" priority="-2" mode="M42"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M42"/></axsl:template>

<!--PATTERN xmlitempath-->


	<!--RULE -->
<axsl:template match="ind-sc:xmlfilecontent_item/ind-sc:path" priority="1000" mode="M43">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a xmlfilecontent_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M43"/></axsl:template><axsl:template match="text()" priority="-1" mode="M43"/><axsl:template match="@*|node()" priority="-2" mode="M43"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M43"/></axsl:template>

<!--PATTERN xmlitemfilename-->


	<!--RULE -->
<axsl:template match="ind-sc:xmlfilecontent_item/ind-sc:filename" priority="1000" mode="M44">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a xmlfilecontent_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M44"/></axsl:template><axsl:template match="text()" priority="-1" mode="M44"/><axsl:template match="@*|node()" priority="-2" mode="M44"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M44"/></axsl:template>

<!--PATTERN xmlitemxpath-->


	<!--RULE -->
<axsl:template match="ind-sc:xmlfilecontent_item/ind-sc:xpath" priority="1000" mode="M45">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the xpath entity of a xmlfilecontent_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M45"/></axsl:template><axsl:template match="text()" priority="-1" mode="M45"/><axsl:template match="@*|node()" priority="-2" mode="M45"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M45"/></axsl:template>

<!--PATTERN xmlitemvalue_of-->


	<!--RULE -->
<axsl:template match="ind-sc:xmlfilecontent_item/ind-sc:value_of" priority="1000" mode="M46">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the value_of entity of a xmlfilecontent_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M46"/></axsl:template><axsl:template match="text()" priority="-1" mode="M46"/><axsl:template match="@*|node()" priority="-2" mode="M46"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M46"/></axsl:template>

<!--PATTERN filesetitemflstinst-->


	<!--RULE -->
<axsl:template match="aix-sc:fileset_item/aix-sc:flstinst" priority="1000" mode="M47">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the flstinst entity of a fileset_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M47"/></axsl:template><axsl:template match="text()" priority="-1" mode="M47"/><axsl:template match="@*|node()" priority="-2" mode="M47"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M47"/></axsl:template>

<!--PATTERN filesetitemlevel-->


	<!--RULE -->
<axsl:template match="aix-sc:fileset_item/aix-sc:level" priority="1000" mode="M48">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the level entity of a fileset_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M48"/></axsl:template><axsl:template match="text()" priority="-1" mode="M48"/><axsl:template match="@*|node()" priority="-2" mode="M48"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M48"/></axsl:template>

<!--PATTERN filesetitemstate-->


	<!--RULE -->
<axsl:template match="aix-sc:fileset_item/aix-sc:state" priority="1000" mode="M49">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the state entity of a fileset_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M49"/></axsl:template><axsl:template match="text()" priority="-1" mode="M49"/><axsl:template match="@*|node()" priority="-2" mode="M49"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M49"/></axsl:template>

<!--PATTERN filesetitemdescription-->


	<!--RULE -->
<axsl:template match="aix-sc:fileset_item/aix-sc:description" priority="1000" mode="M50">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the description entity of a fileset_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M50"/></axsl:template><axsl:template match="text()" priority="-1" mode="M50"/><axsl:template match="@*|node()" priority="-2" mode="M50"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M50"/></axsl:template>

<!--PATTERN fixitemapar_number-->


	<!--RULE -->
<axsl:template match="aix-sc:fix_item/aix-sc:apar_number" priority="1000" mode="M51">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the apar_number entity of a fix_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M51"/></axsl:template><axsl:template match="text()" priority="-1" mode="M51"/><axsl:template match="@*|node()" priority="-2" mode="M51"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M51"/></axsl:template>

<!--PATTERN fixitemabstract-->


	<!--RULE -->
<axsl:template match="aix-sc:fix_item/aix-sc:abstract" priority="1000" mode="M52">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the abstract entity of a fix_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M52"/></axsl:template><axsl:template match="text()" priority="-1" mode="M52"/><axsl:template match="@*|node()" priority="-2" mode="M52"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M52"/></axsl:template>

<!--PATTERN fixitemsymptom-->


	<!--RULE -->
<axsl:template match="aix-sc:fix_item/aix-sc:symptom" priority="1000" mode="M53">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the symptom entity of a fix_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M53"/></axsl:template><axsl:template match="text()" priority="-1" mode="M53"/><axsl:template match="@*|node()" priority="-2" mode="M53"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M53"/></axsl:template>

<!--PATTERN fixiteminstallation_status-->


	<!--RULE -->
<axsl:template match="aix-sc:fix_item/aix-sc:installation_status" priority="1000" mode="M54">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the installation_status entity of a fix_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M54"/></axsl:template><axsl:template match="text()" priority="-1" mode="M54"/><axsl:template match="@*|node()" priority="-2" mode="M54"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M54"/></axsl:template>

<!--PATTERN oslevelitemmaintenance_level-->


	<!--RULE -->
<axsl:template match="aix-sc:oslevel_item/aix-sc:maintenance_level" priority="1000" mode="M55">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the maintenance_level entity of an oslevel_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M55"/></axsl:template><axsl:template match="text()" priority="-1" mode="M55"/><axsl:template match="@*|node()" priority="-2" mode="M55"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M55"/></axsl:template>

<!--PATTERN httpditempath-->


	<!--RULE -->
<axsl:template match="apache-sc:httpd_item/apache-sc:path" priority="1000" mode="M56">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a httpd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M56"/></axsl:template><axsl:template match="text()" priority="-1" mode="M56"/><axsl:template match="@*|node()" priority="-2" mode="M56"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M56"/></axsl:template>

<!--PATTERN httpditembinaryname-->


	<!--RULE -->
<axsl:template match="apache-sc:httpd_item/apache-sc:binary_name" priority="1000" mode="M57">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the binary_name entity of a httpd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M57"/></axsl:template><axsl:template match="text()" priority="-1" mode="M57"/><axsl:template match="@*|node()" priority="-2" mode="M57"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M57"/></axsl:template>

<!--PATTERN httpditemversion-->


	<!--RULE -->
<axsl:template match="apache-sc:httpd_item/apache-sc:version" priority="1000" mode="M58">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a httpd_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M58"/></axsl:template><axsl:template match="text()" priority="-1" mode="M58"/><axsl:template match="@*|node()" priority="-2" mode="M58"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M58"/></axsl:template>

<!--PATTERN apacheveritemversion-->


	<!--RULE -->
<axsl:template match="apache-sc:version_item/apache-sc:version" priority="1000" mode="M59">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a version_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M59"/></axsl:template><axsl:template match="text()" priority="-1" mode="M59"/><axsl:template match="@*|node()" priority="-2" mode="M59"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M59"/></axsl:template>

<!--PATTERN catoslineitemshow_subcommand-->


	<!--RULE -->
<axsl:template match="catos-sc:line_item/catos-sc:show_subcommand" priority="1000" mode="M60">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the show_subcommand entity of a line_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M60"/></axsl:template><axsl:template match="text()" priority="-1" mode="M60"/><axsl:template match="@*|node()" priority="-2" mode="M60"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M60"/></axsl:template>

<!--PATTERN catoslineitemconfig_line-->


	<!--RULE -->
<axsl:template match="catos-sc:line_item/catos-sc:config_line" priority="1000" mode="M61">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the config_line entity of a line_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M61"/></axsl:template><axsl:template match="text()" priority="-1" mode="M61"/><axsl:template match="@*|node()" priority="-2" mode="M61"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M61"/></axsl:template>

<!--PATTERN catosmoduleitemmodule_number-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:module_number" priority="1000" mode="M62">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the module_number entity of a module_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M62"/></axsl:template><axsl:template match="text()" priority="-1" mode="M62"/><axsl:template match="@*|node()" priority="-2" mode="M62"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M62"/></axsl:template>

<!--PATTERN catosmoduleitemtype-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:type" priority="1000" mode="M63">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of a module_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M63"/></axsl:template><axsl:template match="text()" priority="-1" mode="M63"/><axsl:template match="@*|node()" priority="-2" mode="M63"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M63"/></axsl:template>

<!--PATTERN catosmoduleitemmodel-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:model" priority="1000" mode="M64">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the model entity of a module_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M64"/></axsl:template><axsl:template match="text()" priority="-1" mode="M64"/><axsl:template match="@*|node()" priority="-2" mode="M64"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M64"/></axsl:template>

<!--PATTERN catosmoduleitemsoftware_major_release-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:software_major_release" priority="1000" mode="M65">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the software_major_release entity of a module_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M65"/></axsl:template><axsl:template match="text()" priority="-1" mode="M65"/><axsl:template match="@*|node()" priority="-2" mode="M65"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M65"/></axsl:template>

<!--PATTERN catosmoduleitemsoftware_individual_release-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:software_individual_release" priority="1000" mode="M66">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the software_individual_release entity of a module_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M66"/></axsl:template><axsl:template match="text()" priority="-1" mode="M66"/><axsl:template match="@*|node()" priority="-2" mode="M66"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M66"/></axsl:template>

<!--PATTERN catosmoduleitemsoftware_version_id-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:software_version_id" priority="1000" mode="M67">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the software_version_id entity of a module_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M67"/></axsl:template><axsl:template match="text()" priority="-1" mode="M67"/><axsl:template match="@*|node()" priority="-2" mode="M67"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M67"/></axsl:template>

<!--PATTERN catosmoduleitemhardware_major_release-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:hardware_major_release" priority="1000" mode="M68">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hardware_major_release entity of a module_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M68"/></axsl:template><axsl:template match="text()" priority="-1" mode="M68"/><axsl:template match="@*|node()" priority="-2" mode="M68"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M68"/></axsl:template>

<!--PATTERN catosmoduleitemhardware_individual_release-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:hardware_individual_release" priority="1000" mode="M69">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hardware_individual_release entity of a module_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M69"/></axsl:template><axsl:template match="text()" priority="-1" mode="M69"/><axsl:template match="@*|node()" priority="-2" mode="M69"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M69"/></axsl:template>

<!--PATTERN catosmoduleitemfirmware_major_release-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:firmware_major_release" priority="1000" mode="M70">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the firmware_major_release entity of a module_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M70"/></axsl:template><axsl:template match="text()" priority="-1" mode="M70"/><axsl:template match="@*|node()" priority="-2" mode="M70"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M70"/></axsl:template>

<!--PATTERN catosmoduleitemfirmware_individual_release-->


	<!--RULE -->
<axsl:template match="catos-sc:module_item/catos-sc:firmware_individual_release" priority="1000" mode="M71">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the firmware_individual_release entity of a module_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M71"/></axsl:template><axsl:template match="text()" priority="-1" mode="M71"/><axsl:template match="@*|node()" priority="-2" mode="M71"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M71"/></axsl:template>

<!--PATTERN versionitemswitch_series-->


	<!--RULE -->
<axsl:template match="catos-sc:version_item/catos-sc:switch_series" priority="1000" mode="M72">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the switch_series entity of a version_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M72"/></axsl:template><axsl:template match="text()" priority="-1" mode="M72"/><axsl:template match="@*|node()" priority="-2" mode="M72"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M72"/></axsl:template>

<!--PATTERN versionitemimage_name-->


	<!--RULE -->
<axsl:template match="catos-sc:version_item/catos-sc:image_name" priority="1000" mode="M73">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the image_name entity of a version_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M73"/></axsl:template><axsl:template match="text()" priority="-1" mode="M73"/><axsl:template match="@*|node()" priority="-2" mode="M73"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M73"/></axsl:template>

<!--PATTERN versionitemcatos_release-->


	<!--RULE -->
<axsl:template match="catos-sc:version_item/catos-sc:catos_release" priority="1000" mode="M74">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the catos_release entity of a version_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M74"/></axsl:template><axsl:template match="text()" priority="-1" mode="M74"/><axsl:template match="@*|node()" priority="-2" mode="M74"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M74"/></axsl:template>

<!--PATTERN versionitemcatos_major_release-->


	<!--RULE -->
<axsl:template match="catos-sc:version_item/catos-sc:catos_major_release" priority="1000" mode="M75">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the catos_major_release entity of a version_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M75"/></axsl:template><axsl:template match="text()" priority="-1" mode="M75"/><axsl:template match="@*|node()" priority="-2" mode="M75"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M75"/></axsl:template>

<!--PATTERN versionitemcatos_individual_release-->


	<!--RULE -->
<axsl:template match="catos-sc:version_item/catos-sc:catos_individual_release" priority="1000" mode="M76">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the catos_individual_release entity of a version_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M76"/></axsl:template><axsl:template match="text()" priority="-1" mode="M76"/><axsl:template match="@*|node()" priority="-2" mode="M76"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M76"/></axsl:template>

<!--PATTERN versionitemcatos_version_id-->


	<!--RULE -->
<axsl:template match="catos-sc:version_item/catos-sc:catos_version_id" priority="1000" mode="M77">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the catos_version_id entity of a version_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M77"/></axsl:template><axsl:template match="text()" priority="-1" mode="M77"/><axsl:template match="@*|node()" priority="-2" mode="M77"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M77"/></axsl:template>

<!--PATTERN esxpatchitempatch_number-->


	<!--RULE -->
<axsl:template match="esx-sc:patch_item/esx-sc:patch_number" priority="1000" mode="M78">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the patch_number entity of a patch_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M78"/></axsl:template><axsl:template match="text()" priority="-1" mode="M78"/><axsl:template match="@*|node()" priority="-2" mode="M78"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M78"/></axsl:template>

<!--PATTERN esxpatchitemstatus-->


	<!--RULE -->
<axsl:template match="esx-sc:patch_item/esx-sc:status" priority="1000" mode="M79">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the status entity of a patch_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M79"/></axsl:template><axsl:template match="text()" priority="-1" mode="M79"/><axsl:template match="@*|node()" priority="-2" mode="M79"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M79"/></axsl:template>

<!--PATTERN versionitemrelease-->


	<!--RULE -->
<axsl:template match="esx-sc:version_item/esx-sc:release" priority="1000" mode="M80">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the release entity of a version_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M80"/></axsl:template><axsl:template match="text()" priority="-1" mode="M80"/><axsl:template match="@*|node()" priority="-2" mode="M80"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M80"/></axsl:template>

<!--PATTERN versionitembuild-->


	<!--RULE -->
<axsl:template match="esx-sc:version_item/esx-sc:build" priority="1000" mode="M81">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the build entity of a version_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M81"/></axsl:template><axsl:template match="text()" priority="-1" mode="M81"/><axsl:template match="@*|node()" priority="-2" mode="M81"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M81"/></axsl:template>

<!--PATTERN portinfoitempkginst-->


	<!--RULE -->
<axsl:template match="freebsd-sc:portinfo_item/freebsd-sc:pkginst" priority="1000" mode="M82">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pkginst entity of a portinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M82"/></axsl:template><axsl:template match="text()" priority="-1" mode="M82"/><axsl:template match="@*|node()" priority="-2" mode="M82"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M82"/></axsl:template>

<!--PATTERN portinfoitemname-->


	<!--RULE -->
<axsl:template match="freebsd-sc:portinfo_item/freebsd-sc:name" priority="1000" mode="M83">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a portinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M83"/></axsl:template><axsl:template match="text()" priority="-1" mode="M83"/><axsl:template match="@*|node()" priority="-2" mode="M83"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M83"/></axsl:template>

<!--PATTERN portinfoitemcategory-->


	<!--RULE -->
<axsl:template match="freebsd-sc:portinfo_item/freebsd-sc:category" priority="1000" mode="M84">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the category entity of a portinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M84"/></axsl:template><axsl:template match="text()" priority="-1" mode="M84"/><axsl:template match="@*|node()" priority="-2" mode="M84"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M84"/></axsl:template>

<!--PATTERN portinfoitemversion-->


	<!--RULE -->
<axsl:template match="freebsd-sc:portinfo_item/freebsd-sc:version" priority="1000" mode="M85">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a portinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M85"/></axsl:template><axsl:template match="text()" priority="-1" mode="M85"/><axsl:template match="@*|node()" priority="-2" mode="M85"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M85"/></axsl:template>

<!--PATTERN portinfoitemvendor-->


	<!--RULE -->
<axsl:template match="freebsd-sc:portinfo_item/freebsd-sc:vendor" priority="1000" mode="M86">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the vendor entity of a portinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M86"/></axsl:template><axsl:template match="text()" priority="-1" mode="M86"/><axsl:template match="@*|node()" priority="-2" mode="M86"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M86"/></axsl:template>

<!--PATTERN portinfoitemdescription-->


	<!--RULE -->
<axsl:template match="freebsd-sc:portinfo_item/freebsd-sc:description" priority="1000" mode="M87">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the description entity of a portinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M87"/></axsl:template><axsl:template match="text()" priority="-1" mode="M87"/><axsl:template match="@*|node()" priority="-2" mode="M87"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M87"/></axsl:template>

<!--PATTERN gcitemparameter_name-->


	<!--RULE -->
<axsl:template match="hpux-sc:getconf_item/hpux-sc:parameter_name" priority="1000" mode="M88">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the parameter_name entity of a getconf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M88"/></axsl:template><axsl:template match="text()" priority="-1" mode="M88"/><axsl:template match="@*|node()" priority="-2" mode="M88"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M88"/></axsl:template>

<!--PATTERN gcitempathname-->


	<!--RULE -->
<axsl:template match="hpux-sc:getconf_item/hpux-sc:pathname" priority="1000" mode="M89">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pathname entity of a getconf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M89"/></axsl:template><axsl:template match="text()" priority="-1" mode="M89"/><axsl:template match="@*|node()" priority="-2" mode="M89"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M89"/></axsl:template>

<!--PATTERN gcitemoutput-->


	<!--RULE -->
<axsl:template match="hpux-sc:getconf_item/hpux-sc:output" priority="1000" mode="M90">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the output entity of a getconf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M90"/></axsl:template><axsl:template match="text()" priority="-1" mode="M90"/><axsl:template match="@*|node()" priority="-2" mode="M90"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M90"/></axsl:template>

<!--PATTERN patchitemswtype-->


	<!--RULE -->
<axsl:template match="hpux-sc:patch_item/hpux-sc:swtype" priority="1000" mode="M91">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the swtype entity of a patch_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M91"/></axsl:template><axsl:template match="text()" priority="-1" mode="M91"/><axsl:template match="@*|node()" priority="-2" mode="M91"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M91"/></axsl:template>

<!--PATTERN patchitemarea_patched-->


	<!--RULE -->
<axsl:template match="hpux-sc:patch_item/hpux-sc:area_patched" priority="1000" mode="M92">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the area_patched entity of a patch_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M92"/></axsl:template><axsl:template match="text()" priority="-1" mode="M92"/><axsl:template match="@*|node()" priority="-2" mode="M92"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M92"/></axsl:template>

<!--PATTERN patchitempatch_number-->


	<!--RULE -->
<axsl:template match="hpux-sc:patch_item/hpux-sc:patch_number" priority="1000" mode="M93">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the patch_number entity of a patch_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M93"/></axsl:template><axsl:template match="text()" priority="-1" mode="M93"/><axsl:template match="@*|node()" priority="-2" mode="M93"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M93"/></axsl:template>

<!--PATTERN swlistitemswlist-->


	<!--RULE -->
<axsl:template match="hpux-sc:swlist_item/hpux-sc:swlist" priority="1000" mode="M94">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the swlist entity of a swlist_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M94"/></axsl:template><axsl:template match="text()" priority="-1" mode="M94"/><axsl:template match="@*|node()" priority="-2" mode="M94"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M94"/></axsl:template>

<!--PATTERN swlistitembundle-->


	<!--RULE -->
<axsl:template match="hpux-sc:swlist_item/hpux-sc:bundle" priority="1000" mode="M95">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the bundle entity of a swlist_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M95"/></axsl:template><axsl:template match="text()" priority="-1" mode="M95"/><axsl:template match="@*|node()" priority="-2" mode="M95"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M95"/></axsl:template>

<!--PATTERN swlistitemfileset-->


	<!--RULE -->
<axsl:template match="hpux-sc:swlist_item/hpux-sc:fileset" priority="1000" mode="M96">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the fileset entity of a swlist_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M96"/></axsl:template><axsl:template match="text()" priority="-1" mode="M96"/><axsl:template match="@*|node()" priority="-2" mode="M96"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M96"/></axsl:template>

<!--PATTERN swlistitemversion-->


	<!--RULE -->
<axsl:template match="hpux-sc:swlist_item/hpux-sc:version" priority="1000" mode="M97">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='fileset_revision'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a swlist_item should be 'fileset_revision'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M97"/></axsl:template><axsl:template match="text()" priority="-1" mode="M97"/><axsl:template match="@*|node()" priority="-2" mode="M97"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M97"/></axsl:template>

<!--PATTERN swlistitemtitle-->


	<!--RULE -->
<axsl:template match="hpux-sc:swlist_item/hpux-sc:title" priority="1000" mode="M98">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the title entity of a swlist_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M98"/></axsl:template><axsl:template match="text()" priority="-1" mode="M98"/><axsl:template match="@*|node()" priority="-2" mode="M98"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M98"/></axsl:template>

<!--PATTERN swlistitemvendor-->


	<!--RULE -->
<axsl:template match="hpux-sc:swlist_item/hpux-sc:vendor" priority="1000" mode="M99">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the vendor entity of a swlist_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M99"/></axsl:template><axsl:template match="text()" priority="-1" mode="M99"/><axsl:template match="@*|node()" priority="-2" mode="M99"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M99"/></axsl:template>

<!--PATTERN trustitemtrustitem-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:trustitem" priority="1000" mode="M100">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustitem entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M100"/></axsl:template><axsl:template match="text()" priority="-1" mode="M100"/><axsl:template match="@*|node()" priority="-2" mode="M100"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M100"/></axsl:template>

<!--PATTERN trustitemuid-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:uid" priority="1000" mode="M101">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uid entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M101"/></axsl:template><axsl:template match="text()" priority="-1" mode="M101"/><axsl:template match="@*|node()" priority="-2" mode="M101"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M101"/></axsl:template>

<!--PATTERN trustitempassword-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:password" priority="1000" mode="M102">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M102"/></axsl:template><axsl:template match="text()" priority="-1" mode="M102"/><axsl:template match="@*|node()" priority="-2" mode="M102"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M102"/></axsl:template>

<!--PATTERN trustitemaccount_owner-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:account_owner" priority="1000" mode="M103">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the account_owner entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M103"/></axsl:template><axsl:template match="text()" priority="-1" mode="M103"/><axsl:template match="@*|node()" priority="-2" mode="M103"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M103"/></axsl:template>

<!--PATTERN trustitemboot_auth-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:boot_auth" priority="1000" mode="M104">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the boot_auth entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M104"/></axsl:template><axsl:template match="text()" priority="-1" mode="M104"/><axsl:template match="@*|node()" priority="-2" mode="M104"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M104"/></axsl:template>

<!--PATTERN trustitemaudit_id-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:audit_id" priority="1000" mode="M105">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the audit_id entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M105"/></axsl:template><axsl:template match="text()" priority="-1" mode="M105"/><axsl:template match="@*|node()" priority="-2" mode="M105"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M105"/></axsl:template>

<!--PATTERN trustitemaudit_flag-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:audit_flag" priority="1000" mode="M106">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the audit_flag entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M106"/></axsl:template><axsl:template match="text()" priority="-1" mode="M106"/><axsl:template match="@*|node()" priority="-2" mode="M106"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M106"/></axsl:template>

<!--PATTERN trustitempw_change_min-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_change_min" priority="1000" mode="M107">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_change_min entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M107"/></axsl:template><axsl:template match="text()" priority="-1" mode="M107"/><axsl:template match="@*|node()" priority="-2" mode="M107"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M107"/></axsl:template>

<!--PATTERN trustitempw_max_size-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_max_size" priority="1000" mode="M108">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_max_size entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M108"/></axsl:template><axsl:template match="text()" priority="-1" mode="M108"/><axsl:template match="@*|node()" priority="-2" mode="M108"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M108"/></axsl:template>

<!--PATTERN trustitempw_expiration-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_expiration" priority="1000" mode="M109">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_expiration entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M109"/></axsl:template><axsl:template match="text()" priority="-1" mode="M109"/><axsl:template match="@*|node()" priority="-2" mode="M109"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M109"/></axsl:template>

<!--PATTERN trustitempw_life-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_life" priority="1000" mode="M110">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_life entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M110"/></axsl:template><axsl:template match="text()" priority="-1" mode="M110"/><axsl:template match="@*|node()" priority="-2" mode="M110"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M110"/></axsl:template>

<!--PATTERN trustitempw_change_s-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_change_s" priority="1000" mode="M111">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_change_s entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M111"/></axsl:template><axsl:template match="text()" priority="-1" mode="M111"/><axsl:template match="@*|node()" priority="-2" mode="M111"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M111"/></axsl:template>

<!--PATTERN trustitempw_change_u-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_change_u" priority="1000" mode="M112">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_change_u entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M112"/></axsl:template><axsl:template match="text()" priority="-1" mode="M112"/><axsl:template match="@*|node()" priority="-2" mode="M112"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M112"/></axsl:template>

<!--PATTERN trustitemacct_expire-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:acct_expire" priority="1000" mode="M113">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the acct_expire entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M113"/></axsl:template><axsl:template match="text()" priority="-1" mode="M113"/><axsl:template match="@*|node()" priority="-2" mode="M113"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M113"/></axsl:template>

<!--PATTERN trustitemmax_llogin-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:max_llogin" priority="1000" mode="M114">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the max_llogin entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M114"/></axsl:template><axsl:template match="text()" priority="-1" mode="M114"/><axsl:template match="@*|node()" priority="-2" mode="M114"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M114"/></axsl:template>

<!--PATTERN trustitemexp_warning-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:exp_warning" priority="1000" mode="M115">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exp_warning entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M115"/></axsl:template><axsl:template match="text()" priority="-1" mode="M115"/><axsl:template match="@*|node()" priority="-2" mode="M115"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M115"/></axsl:template>

<!--PATTERN trustitemusr_chg_pw-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:usr_chg_pw" priority="1000" mode="M116">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the usr_chg_pw entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M116"/></axsl:template><axsl:template match="text()" priority="-1" mode="M116"/><axsl:template match="@*|node()" priority="-2" mode="M116"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M116"/></axsl:template>

<!--PATTERN trustitemgen_pw-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:gen_pw" priority="1000" mode="M117">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gen_pw entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M117"/></axsl:template><axsl:template match="text()" priority="-1" mode="M117"/><axsl:template match="@*|node()" priority="-2" mode="M117"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M117"/></axsl:template>

<!--PATTERN trustitempw_restrict-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_restrict" priority="1000" mode="M118">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_restrict entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M118"/></axsl:template><axsl:template match="text()" priority="-1" mode="M118"/><axsl:template match="@*|node()" priority="-2" mode="M118"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M118"/></axsl:template>

<!--PATTERN trustitempw_null-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_null" priority="1000" mode="M119">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_null entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M119"/></axsl:template><axsl:template match="text()" priority="-1" mode="M119"/><axsl:template match="@*|node()" priority="-2" mode="M119"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M119"/></axsl:template>

<!--PATTERN trustitempw_gen_char-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_gen_char" priority="1000" mode="M120">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_gen_char entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M120"/></axsl:template><axsl:template match="text()" priority="-1" mode="M120"/><axsl:template match="@*|node()" priority="-2" mode="M120"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M120"/></axsl:template>

<!--PATTERN trustitempw_gen_let-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:swlistpw_gen_let" priority="1000" mode="M121">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_gen_let entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M121"/></axsl:template><axsl:template match="text()" priority="-1" mode="M121"/><axsl:template match="@*|node()" priority="-2" mode="M121"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M121"/></axsl:template>

<!--PATTERN trustitemlogin_time-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:login_time" priority="1000" mode="M122">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_time entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M122"/></axsl:template><axsl:template match="text()" priority="-1" mode="M122"/><axsl:template match="@*|node()" priority="-2" mode="M122"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M122"/></axsl:template>

<!--PATTERN trustitempw_changer-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:pw_changer" priority="1000" mode="M123">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pw_changer entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M123"/></axsl:template><axsl:template match="text()" priority="-1" mode="M123"/><axsl:template match="@*|node()" priority="-2" mode="M123"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M123"/></axsl:template>

<!--PATTERN trustitemlogin_time_s-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:login_time_s" priority="1000" mode="M124">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_time_s entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M124"/></axsl:template><axsl:template match="text()" priority="-1" mode="M124"/><axsl:template match="@*|node()" priority="-2" mode="M124"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M124"/></axsl:template>

<!--PATTERN trustitemlogin_time_u-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:login_time_u" priority="1000" mode="M125">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_time_u entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M125"/></axsl:template><axsl:template match="text()" priority="-1" mode="M125"/><axsl:template match="@*|node()" priority="-2" mode="M125"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M125"/></axsl:template>

<!--PATTERN trustitemlogin_tty_s-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:login_tty_s" priority="1000" mode="M126">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_tty_s entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M126"/></axsl:template><axsl:template match="text()" priority="-1" mode="M126"/><axsl:template match="@*|node()" priority="-2" mode="M126"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M126"/></axsl:template>

<!--PATTERN trustitemlogin_tty_u-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:login_tty_u" priority="1000" mode="M127">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_tty_u entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M127"/></axsl:template><axsl:template match="text()" priority="-1" mode="M127"/><axsl:template match="@*|node()" priority="-2" mode="M127"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M127"/></axsl:template>

<!--PATTERN trustitemnum_u_logins-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:num_u_logins" priority="1000" mode="M128">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the num_u_logins entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M128"/></axsl:template><axsl:template match="text()" priority="-1" mode="M128"/><axsl:template match="@*|node()" priority="-2" mode="M128"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M128"/></axsl:template>

<!--PATTERN trustitemmax_u_logins-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:max_u_logins" priority="1000" mode="M129">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the max_u_logins entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M129"/></axsl:template><axsl:template match="text()" priority="-1" mode="M129"/><axsl:template match="@*|node()" priority="-2" mode="M129"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M129"/></axsl:template>

<!--PATTERN trustitemlock_flag-->


	<!--RULE -->
<axsl:template match="hpux-sc:trusted_item/hpux-sc:lock_flag" priority="1000" mode="M130">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the lock_flag entity of a trusted_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M130"/></axsl:template><axsl:template match="text()" priority="-1" mode="M130"/><axsl:template match="@*|node()" priority="-2" mode="M130"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M130"/></axsl:template>

<!--PATTERN globalitemglobal_command-->


	<!--RULE -->
<axsl:template match="ios-sc:global_item/ios-sc:global_command" priority="1000" mode="M131">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the global_command entity of a global_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M131"/></axsl:template><axsl:template match="text()" priority="-1" mode="M131"/><axsl:template match="@*|node()" priority="-2" mode="M131"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M131"/></axsl:template>

<!--PATTERN iosinterfaceitemname-->


	<!--RULE -->
<axsl:template match="ios-sc:interface_item/ios-sc:name" priority="1000" mode="M132">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a global_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M132"/></axsl:template><axsl:template match="text()" priority="-1" mode="M132"/><axsl:template match="@*|node()" priority="-2" mode="M132"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M132"/></axsl:template>

<!--PATTERN iosinterfaceitemip_directed_broadcast_command-->


	<!--RULE -->
<axsl:template match="ios-sc:interface_item/ios-sc:ip_directed_broadcast_command" priority="1000" mode="M133">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ip_directed_broadcast_command entity of a global_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M133"/></axsl:template><axsl:template match="text()" priority="-1" mode="M133"/><axsl:template match="@*|node()" priority="-2" mode="M133"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M133"/></axsl:template>

<!--PATTERN iosinterfaceitemno_ip_directed_broadcast_command-->


	<!--RULE -->
<axsl:template match="ios-sc:interface_item/ios-sc:no_ip_directed_broadcast_command" priority="1000" mode="M134">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the no_ip_directed_broadcast_command entity of a global_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M134"/></axsl:template><axsl:template match="text()" priority="-1" mode="M134"/><axsl:template match="@*|node()" priority="-2" mode="M134"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M134"/></axsl:template>

<!--PATTERN iosinterfaceitemproxy_arp_command-->


	<!--RULE -->
<axsl:template match="ios-sc:interface_item/ios-sc:proxy_arp_command" priority="1000" mode="M135">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the proxy_arp_command entity of a global_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M135"/></axsl:template><axsl:template match="text()" priority="-1" mode="M135"/><axsl:template match="@*|node()" priority="-2" mode="M135"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M135"/></axsl:template>

<!--PATTERN iosinterfaceitemshutdown_command-->


	<!--RULE -->
<axsl:template match="ios-sc:interface_item/ios-sc:shutdown_command" priority="1000" mode="M136">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the shutdown_command entity of a global_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M136"/></axsl:template><axsl:template match="text()" priority="-1" mode="M136"/><axsl:template match="@*|node()" priority="-2" mode="M136"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M136"/></axsl:template>

<!--PATTERN ioslineitemshow_subcommand-->


	<!--RULE -->
<axsl:template match="ios-sc:line_item/ios-sc:show_subcommand" priority="1000" mode="M137">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the show_subcommand entity of a line_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M137"/></axsl:template><axsl:template match="text()" priority="-1" mode="M137"/><axsl:template match="@*|node()" priority="-2" mode="M137"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M137"/></axsl:template>

<!--PATTERN ioslineitemconfig_line-->


	<!--RULE -->
<axsl:template match="ios-sc:line_item/ios-sc:config_line" priority="1000" mode="M138">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the config_line entity of a line_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M138"/></axsl:template><axsl:template match="text()" priority="-1" mode="M138"/><axsl:template match="@*|node()" priority="-2" mode="M138"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M138"/></axsl:template>

<!--PATTERN snmpitemaccess_list-->


	<!--RULE -->
<axsl:template match="ios-sc:snmp_item/ios-sc:access_list" priority="1000" mode="M139">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_list entity of a snmp_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M139"/></axsl:template><axsl:template match="text()" priority="-1" mode="M139"/><axsl:template match="@*|node()" priority="-2" mode="M139"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M139"/></axsl:template>

<!--PATTERN snmpitemcommunity_name-->


	<!--RULE -->
<axsl:template match="ios-sc:snmp_item/ios-sc:community_name" priority="1000" mode="M140">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the community_name entity of a snmp_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M140"/></axsl:template><axsl:template match="text()" priority="-1" mode="M140"/><axsl:template match="@*|node()" priority="-2" mode="M140"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M140"/></axsl:template>

<!--PATTERN tclshitemavailable-->


	<!--RULE -->
<axsl:template match="ios-sc:tclsh_item/ios-sc:available" priority="1000" mode="M141">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the available entity of a tclsh_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M141"/></axsl:template><axsl:template match="text()" priority="-1" mode="M141"/><axsl:template match="@*|node()" priority="-2" mode="M141"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M141"/></axsl:template>

<!--PATTERN iosversionitemmajor_release-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:major_release" priority="1000" mode="M142">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the major_release entity of a version_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M142"/></axsl:template><axsl:template match="text()" priority="-1" mode="M142"/><axsl:template match="@*|node()" priority="-2" mode="M142"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M142"/></axsl:template>

<!--PATTERN iosversionitemtrain_number-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:train_number" priority="1000" mode="M143">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the train_number entity of a version_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M143"/></axsl:template><axsl:template match="text()" priority="-1" mode="M143"/><axsl:template match="@*|node()" priority="-2" mode="M143"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M143"/></axsl:template>

<!--PATTERN iosversionitemmajorversion-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:major_version" priority="1000" mode="M144">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the major_version entity of a version_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M144"/></axsl:template><axsl:template match="text()" priority="-1" mode="M144"/><axsl:template match="@*|node()" priority="-2" mode="M144"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M144"/></axsl:template>

<!--PATTERN iosversionitemminorversion-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:minor_version" priority="1000" mode="M145">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the minor_version entity of a version_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M145"/></axsl:template><axsl:template match="text()" priority="-1" mode="M145"/><axsl:template match="@*|node()" priority="-2" mode="M145"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M145"/></axsl:template>

<!--PATTERN iosversionitemrelease-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:release" priority="1000" mode="M146">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the release entity of a version_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M146"/></axsl:template><axsl:template match="text()" priority="-1" mode="M146"/><axsl:template match="@*|node()" priority="-2" mode="M146"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M146"/></axsl:template>

<!--PATTERN iosversionitemtrain_identifier-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:train_identifier" priority="1000" mode="M147">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the train_identifier entity of a version_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M147"/></axsl:template><axsl:template match="text()" priority="-1" mode="M147"/><axsl:template match="@*|node()" priority="-2" mode="M147"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M147"/></axsl:template>

<!--PATTERN iosversionitemrebuild-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:rebuild" priority="1000" mode="M148">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the rebuild entity of a version_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M148"/></axsl:template><axsl:template match="text()" priority="-1" mode="M148"/><axsl:template match="@*|node()" priority="-2" mode="M148"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M148"/></axsl:template>

<!--PATTERN iosversionitemsubrebuild-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:subrebuild" priority="1000" mode="M149">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the subrebuild entity of a version_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M149"/></axsl:template><axsl:template match="text()" priority="-1" mode="M149"/><axsl:template match="@*|node()" priority="-2" mode="M149"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M149"/></axsl:template>

<!--PATTERN iosversionitemmainlinerebuild-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:mainline_rebuild" priority="1000" mode="M150">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the mainline_rebuild entity of a version_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M150"/></axsl:template><axsl:template match="text()" priority="-1" mode="M150"/><axsl:template match="@*|node()" priority="-2" mode="M150"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M150"/></axsl:template>

<!--PATTERN iosversionitemversion_string-->


	<!--RULE -->
<axsl:template match="ios-sc:version_item/ios-sc:version_string" priority="1000" mode="M151">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='ios_version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version_string entity of a version_item should be 'ios_version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M151"/></axsl:template><axsl:template match="text()" priority="-1" mode="M151"/><axsl:template match="@*|node()" priority="-2" mode="M151"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M151"/></axsl:template>

<!--PATTERN dpkgitemname-->


	<!--RULE -->
<axsl:template match="linux-sc:dpkginfo_item/linux-sc:name" priority="1000" mode="M152">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a dpkginfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M152"/></axsl:template><axsl:template match="text()" priority="-1" mode="M152"/><axsl:template match="@*|node()" priority="-2" mode="M152"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M152"/></axsl:template>

<!--PATTERN dpkgitemarch-->


	<!--RULE -->
<axsl:template match="linux-sc:dpkginfo_item/linux-sc:arch" priority="1000" mode="M153">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the arch entity of a dpkginfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M153"/></axsl:template><axsl:template match="text()" priority="-1" mode="M153"/><axsl:template match="@*|node()" priority="-2" mode="M153"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M153"/></axsl:template>

<!--PATTERN dpkgitemepoch-->


	<!--RULE -->
<axsl:template match="linux-sc:dpkginfo_item/linux-sc:epoch" priority="1000" mode="M154">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the epoch entity of a dpkginfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M154"/></axsl:template><axsl:template match="text()" priority="-1" mode="M154"/><axsl:template match="@*|node()" priority="-2" mode="M154"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M154"/></axsl:template>

<!--PATTERN dpkgitemrelease-->


	<!--RULE -->
<axsl:template match="linux-sc:dpkginfo_item/linux-sc:release" priority="1000" mode="M155">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the release entity of a dpkginfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M155"/></axsl:template><axsl:template match="text()" priority="-1" mode="M155"/><axsl:template match="@*|node()" priority="-2" mode="M155"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M155"/></axsl:template>

<!--PATTERN dpkgitemversion-->


	<!--RULE -->
<axsl:template match="linux-sc:dpkginfo_item/linux-sc:version" priority="1000" mode="M156">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a dpkginfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M156"/></axsl:template><axsl:template match="text()" priority="-1" mode="M156"/><axsl:template match="@*|node()" priority="-2" mode="M156"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M156"/></axsl:template>

<!--PATTERN dpkgitemevr-->


	<!--RULE -->
<axsl:template match="linux-sc:dpkginfo_item/linux-sc:evr" priority="1000" mode="M157">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='evr_string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the evr entity of a dpkginfo_item should be 'evr_string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M157"/></axsl:template><axsl:template match="text()" priority="-1" mode="M157"/><axsl:template match="@*|node()" priority="-2" mode="M157"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M157"/></axsl:template>

<!--PATTERN ilsitemprotocol-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:protocol" priority="1000" mode="M158">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M158"/></axsl:template><axsl:template match="text()" priority="-1" mode="M158"/><axsl:template match="@*|node()" priority="-2" mode="M158"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M158"/></axsl:template>

<!--PATTERN ilsitemlocal_address-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:local_address" priority="1000" mode="M159">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_address entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M159"/></axsl:template><axsl:template match="text()" priority="-1" mode="M159"/><axsl:template match="@*|node()" priority="-2" mode="M159"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M159"/></axsl:template>

<!--PATTERN ilsitemlocal_port-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:local_port" priority="1000" mode="M160">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_port entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M160"/></axsl:template><axsl:template match="text()" priority="-1" mode="M160"/><axsl:template match="@*|node()" priority="-2" mode="M160"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M160"/></axsl:template>

<!--PATTERN ilsitemlocal_full_address-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:local_full_address" priority="1000" mode="M161">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_full_address entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M161"/></axsl:template><axsl:template match="text()" priority="-1" mode="M161"/><axsl:template match="@*|node()" priority="-2" mode="M161"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M161"/></axsl:template>

<!--PATTERN ilsitemprogram_name-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:program_name" priority="1000" mode="M162">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the program_name entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M162"/></axsl:template><axsl:template match="text()" priority="-1" mode="M162"/><axsl:template match="@*|node()" priority="-2" mode="M162"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M162"/></axsl:template>

<!--PATTERN ilsitemforeign_address-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:foreign_address" priority="1000" mode="M163">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_address entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M163"/></axsl:template><axsl:template match="text()" priority="-1" mode="M163"/><axsl:template match="@*|node()" priority="-2" mode="M163"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M163"/></axsl:template>

<!--PATTERN ilsitemforeign_port-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:foreign_port" priority="1000" mode="M164">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_port entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M164"/></axsl:template><axsl:template match="text()" priority="-1" mode="M164"/><axsl:template match="@*|node()" priority="-2" mode="M164"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M164"/></axsl:template>

<!--PATTERN ilsitemforeign_full_address-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:foreign_full_address" priority="1000" mode="M165">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_full_address entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M165"/></axsl:template><axsl:template match="text()" priority="-1" mode="M165"/><axsl:template match="@*|node()" priority="-2" mode="M165"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M165"/></axsl:template>

<!--PATTERN ilsitempid-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:pid" priority="1000" mode="M166">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of an inetlisteningserver_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M166"/></axsl:template><axsl:template match="text()" priority="-1" mode="M166"/><axsl:template match="@*|node()" priority="-2" mode="M166"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M166"/></axsl:template>

<!--PATTERN ilsitemuser_id-->


	<!--RULE -->
<axsl:template match="linux-sc:inetlisteningserver_item/linux-sc:user_id" priority="1000" mode="M167">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M167"/></axsl:template><axsl:template match="text()" priority="-1" mode="M167"/><axsl:template match="@*|node()" priority="-2" mode="M167"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M167"/></axsl:template>

<!--PATTERN rpmitemname-->


	<!--RULE -->
<axsl:template match="linux-sc:rpminfo_item/linux-sc:name" priority="1000" mode="M168">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a rpminfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M168"/></axsl:template><axsl:template match="text()" priority="-1" mode="M168"/><axsl:template match="@*|node()" priority="-2" mode="M168"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M168"/></axsl:template>

<!--PATTERN rpmitemarch-->


	<!--RULE -->
<axsl:template match="linux-sc:rpminfo_item/linux-sc:arch" priority="1000" mode="M169">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the arch entity of a rpminfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M169"/></axsl:template><axsl:template match="text()" priority="-1" mode="M169"/><axsl:template match="@*|node()" priority="-2" mode="M169"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M169"/></axsl:template>

<!--PATTERN rpmitemepoch-->


	<!--RULE -->
<axsl:template match="linux-sc:rpminfo_item/linux-sc:epoch" priority="1000" mode="M170">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the epoch entity of a rpminfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M170"/></axsl:template><axsl:template match="text()" priority="-1" mode="M170"/><axsl:template match="@*|node()" priority="-2" mode="M170"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M170"/></axsl:template>

<!--PATTERN rpmitemrelease-->


	<!--RULE -->
<axsl:template match="linux-sc:rpminfo_item/linux-sc:release" priority="1000" mode="M171">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string' or @datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the release entity of a rpminfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M171"/></axsl:template><axsl:template match="text()" priority="-1" mode="M171"/><axsl:template match="@*|node()" priority="-2" mode="M171"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M171"/></axsl:template>

<!--PATTERN rpmitemversion-->


	<!--RULE -->
<axsl:template match="linux-sc:rpminfo_item/linux-sc:version" priority="1000" mode="M172">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string' or @datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a rpminfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M172"/></axsl:template><axsl:template match="text()" priority="-1" mode="M172"/><axsl:template match="@*|node()" priority="-2" mode="M172"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M172"/></axsl:template>

<!--PATTERN rpmitemevr-->


	<!--RULE -->
<axsl:template match="linux-sc:rpminfo_item/linux-sc:evr" priority="1000" mode="M173">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='evr_string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the evr entity of a rpminfo_item should be 'evr_string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M173"/></axsl:template><axsl:template match="text()" priority="-1" mode="M173"/><axsl:template match="@*|node()" priority="-2" mode="M173"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M173"/></axsl:template>

<!--PATTERN rpmitemsignature_keyid-->


	<!--RULE -->
<axsl:template match="linux-sc:rpminfo_item/linux-sc:signature_keyid" priority="1000" mode="M174">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the signature_keyid entity of a rpminfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M174"/></axsl:template><axsl:template match="text()" priority="-1" mode="M174"/><axsl:template match="@*|node()" priority="-2" mode="M174"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M174"/></axsl:template>

<!--PATTERN spkginfoitemname-->


	<!--RULE -->
<axsl:template match="linux-sc:slackwarepkginfo_item/linux-sc:name" priority="1000" mode="M175">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a slackwarepkginfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M175"/></axsl:template><axsl:template match="text()" priority="-1" mode="M175"/><axsl:template match="@*|node()" priority="-2" mode="M175"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M175"/></axsl:template>

<!--PATTERN spkginfoitemversion-->


	<!--RULE -->
<axsl:template match="linux-sc:slackwarepkginfo_item/linux-sc:version" priority="1000" mode="M176">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a slackwarepkginfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M176"/></axsl:template><axsl:template match="text()" priority="-1" mode="M176"/><axsl:template match="@*|node()" priority="-2" mode="M176"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M176"/></axsl:template>

<!--PATTERN spkginfoitemarchitecture-->


	<!--RULE -->
<axsl:template match="linux-sc:slackwarepkginfo_item/linux-sc:architecture" priority="1000" mode="M177">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the architecture entity of a slackwarepkginfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M177"/></axsl:template><axsl:template match="text()" priority="-1" mode="M177"/><axsl:template match="@*|node()" priority="-2" mode="M177"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M177"/></axsl:template>

<!--PATTERN spkginfoitemrevision-->


	<!--RULE -->
<axsl:template match="linux-sc:slackwarepkginfo_item/linux-sc:revision" priority="1000" mode="M178">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the revision entity of a slackwarepkginfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M178"/></axsl:template><axsl:template match="text()" priority="-1" mode="M178"/><axsl:template match="@*|node()" priority="-2" mode="M178"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M178"/></axsl:template>

<!--PATTERN accountitemusername-->


	<!--RULE -->
<axsl:template match="macos-sc:accountinfo_item/macos-sc:username" priority="1000" mode="M179">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of an accountinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M179"/></axsl:template><axsl:template match="text()" priority="-1" mode="M179"/><axsl:template match="@*|node()" priority="-2" mode="M179"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M179"/></axsl:template>

<!--PATTERN accountitempassword-->


	<!--RULE -->
<axsl:template match="macos-sc:accountinfo_item/macos-sc:password" priority="1000" mode="M180">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password entity of an accountinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M180"/></axsl:template><axsl:template match="text()" priority="-1" mode="M180"/><axsl:template match="@*|node()" priority="-2" mode="M180"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M180"/></axsl:template>

<!--PATTERN accountitemuid-->


	<!--RULE -->
<axsl:template match="macos-sc:accountinfo_item/macos-sc:uid" priority="1000" mode="M181">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uid entity of an accountinfo_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M181"/></axsl:template><axsl:template match="text()" priority="-1" mode="M181"/><axsl:template match="@*|node()" priority="-2" mode="M181"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M181"/></axsl:template>

<!--PATTERN accountitemgid-->


	<!--RULE -->
<axsl:template match="macos-sc:accountinfo_item/macos-sc:gid" priority="1000" mode="M182">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gid entity of an accountinfo_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M182"/></axsl:template><axsl:template match="text()" priority="-1" mode="M182"/><axsl:template match="@*|node()" priority="-2" mode="M182"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M182"/></axsl:template>

<!--PATTERN accountitemrealname-->


	<!--RULE -->
<axsl:template match="macos-sc:accountinfo_item/macos-sc:realname" priority="1000" mode="M183">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the realname entity of an accountinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M183"/></axsl:template><axsl:template match="text()" priority="-1" mode="M183"/><axsl:template match="@*|node()" priority="-2" mode="M183"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M183"/></axsl:template>

<!--PATTERN accountitemhome_dir-->


	<!--RULE -->
<axsl:template match="macos-sc:accountinfo_item/macos-sc:home_dir" priority="1000" mode="M184">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the home_dir entity of an accountinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M184"/></axsl:template><axsl:template match="text()" priority="-1" mode="M184"/><axsl:template match="@*|node()" priority="-2" mode="M184"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M184"/></axsl:template>

<!--PATTERN accountitemlogin_shell-->


	<!--RULE -->
<axsl:template match="macos-sc:accountinfo_item/macos-sc:login_shell" priority="1000" mode="M185">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_shell entity of an accountinfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M185"/></axsl:template><axsl:template match="text()" priority="-1" mode="M185"/><axsl:template match="@*|node()" priority="-2" mode="M185"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M185"/></axsl:template>

<!--PATTERN macosilsitemprogram_name-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:program_name" priority="1000" mode="M186">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the program_name entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M186"/></axsl:template><axsl:template match="text()" priority="-1" mode="M186"/><axsl:template match="@*|node()" priority="-2" mode="M186"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M186"/></axsl:template>

<!--PATTERN macosilsitemlocal_address-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:local_address" priority="1000" mode="M187">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_address entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M187"/></axsl:template><axsl:template match="text()" priority="-1" mode="M187"/><axsl:template match="@*|node()" priority="-2" mode="M187"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M187"/></axsl:template>

<!--PATTERN macosilsitemlocal_full_address-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:local_full_address" priority="1000" mode="M188">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_full_address entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M188"/></axsl:template><axsl:template match="text()" priority="-1" mode="M188"/><axsl:template match="@*|node()" priority="-2" mode="M188"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M188"/></axsl:template>

<!--PATTERN macosilsitemlocal_port-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:local_port" priority="1000" mode="M189">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_port entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M189"/></axsl:template><axsl:template match="text()" priority="-1" mode="M189"/><axsl:template match="@*|node()" priority="-2" mode="M189"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M189"/></axsl:template>

<!--PATTERN macosilsitemforeign_address-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:foreign_address" priority="1000" mode="M190">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_address entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M190"/></axsl:template><axsl:template match="text()" priority="-1" mode="M190"/><axsl:template match="@*|node()" priority="-2" mode="M190"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M190"/></axsl:template>

<!--PATTERN macosilsitemforeign_full_address-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:foreign_full_address" priority="1000" mode="M191">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_full_address entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M191"/></axsl:template><axsl:template match="text()" priority="-1" mode="M191"/><axsl:template match="@*|node()" priority="-2" mode="M191"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M191"/></axsl:template>

<!--PATTERN macosilsitemforeign_port-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:foreign_port" priority="1000" mode="M192">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the foreign_port entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M192"/></axsl:template><axsl:template match="text()" priority="-1" mode="M192"/><axsl:template match="@*|node()" priority="-2" mode="M192"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M192"/></axsl:template>

<!--PATTERN macosilsitempid-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:pid" priority="1000" mode="M193">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of an inetlisteningserver_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M193"/></axsl:template><axsl:template match="text()" priority="-1" mode="M193"/><axsl:template match="@*|node()" priority="-2" mode="M193"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M193"/></axsl:template>

<!--PATTERN macosilsitemprotocol-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:protocol" priority="1000" mode="M194">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M194"/></axsl:template><axsl:template match="text()" priority="-1" mode="M194"/><axsl:template match="@*|node()" priority="-2" mode="M194"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M194"/></axsl:template>

<!--PATTERN macosilsitemuser_id-->


	<!--RULE -->
<axsl:template match="macos-sc:inetlisteningserver_item/macos-sc:user_id" priority="1000" mode="M195">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of an inetlisteningserver_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M195"/></axsl:template><axsl:template match="text()" priority="-1" mode="M195"/><axsl:template match="@*|node()" priority="-2" mode="M195"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M195"/></axsl:template>

<!--PATTERN nvramitemnvram_var-->


	<!--RULE -->
<axsl:template match="macos-sc:nvram_item/macos-sc:nvram_var" priority="1000" mode="M196">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the nvram_var entity of a nvram_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M196"/></axsl:template><axsl:template match="text()" priority="-1" mode="M196"/><axsl:template match="@*|node()" priority="-2" mode="M196"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M196"/></axsl:template>

<!--PATTERN nvramitemnvram_value-->


	<!--RULE -->
<axsl:template match="macos-sc:nvram_item/macos-sc:nvram_value" priority="1000" mode="M197">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the nvram_value entity of a nvram_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M197"/></axsl:template><axsl:template match="text()" priority="-1" mode="M197"/><axsl:template match="@*|node()" priority="-2" mode="M197"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M197"/></axsl:template>

<!--PATTERN pwpitemusername-->


	<!--RULE -->
<axsl:template match="macos-sc:pwpolicy_item/macos-sc:username" priority="1000" mode="M198">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a pwpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M198"/></axsl:template><axsl:template match="text()" priority="-1" mode="M198"/><axsl:template match="@*|node()" priority="-2" mode="M198"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M198"/></axsl:template>

<!--PATTERN pwpitemuserpass-->


	<!--RULE -->
<axsl:template match="macos-sc:pwpolicy_item/macos-sc:userpass" priority="1000" mode="M199">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the userpass entity of a pwpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M199"/></axsl:template><axsl:template match="text()" priority="-1" mode="M199"/><axsl:template match="@*|node()" priority="-2" mode="M199"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M199"/></axsl:template>

<!--PATTERN pwpitemdirectory_node-->


	<!--RULE -->
<axsl:template match="macos-sc:pwpolicy_item/macos-sc:directory_node" priority="1000" mode="M200">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_node entity of a pwpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M200"/></axsl:template><axsl:template match="text()" priority="-1" mode="M200"/><axsl:template match="@*|node()" priority="-2" mode="M200"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M200"/></axsl:template>

<!--PATTERN pwpitemmaxChars-->


	<!--RULE -->
<axsl:template match="macos-sc:pwpolicy_item/macos-sc:maxChars" priority="1000" mode="M201">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the maxChars entity of a pwpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M201"/></axsl:template><axsl:template match="text()" priority="-1" mode="M201"/><axsl:template match="@*|node()" priority="-2" mode="M201"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M201"/></axsl:template>

<!--PATTERN pwpitemmaxFailedLoginAttempts-->


	<!--RULE -->
<axsl:template match="macos-sc:pwpolicy_item/macos-sc:maxFailedLoginAttempts" priority="1000" mode="M202">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the maxFailedLoginAttempts entity of a pwpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M202"/></axsl:template><axsl:template match="text()" priority="-1" mode="M202"/><axsl:template match="@*|node()" priority="-2" mode="M202"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M202"/></axsl:template>

<!--PATTERN pwpitemminChars-->


	<!--RULE -->
<axsl:template match="macos-sc:pwpolicy_item/macos-sc:minChars" priority="1000" mode="M203">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the minChars entity of a pwpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M203"/></axsl:template><axsl:template match="text()" priority="-1" mode="M203"/><axsl:template match="@*|node()" priority="-2" mode="M203"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M203"/></axsl:template>

<!--PATTERN pwpitempasswordCannotBeName-->


	<!--RULE -->
<axsl:template match="macos-sc:pwpolicy_item/macos-sc:passwordCannotBeName" priority="1000" mode="M204">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the passwordCannotBeName entity of a pwpolicy_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M204"/></axsl:template><axsl:template match="text()" priority="-1" mode="M204"/><axsl:template match="@*|node()" priority="-2" mode="M204"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M204"/></axsl:template>

<!--PATTERN pwpitemrequiresAlpha-->


	<!--RULE -->
<axsl:template match="macos-sc:pwpolicy_item/macos-sc:requiresAlpha" priority="1000" mode="M205">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the requiresAlpha entity of a pwpolicy_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M205"/></axsl:template><axsl:template match="text()" priority="-1" mode="M205"/><axsl:template match="@*|node()" priority="-2" mode="M205"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M205"/></axsl:template>

<!--PATTERN pwpitemrequiresNumeric-->


	<!--RULE -->
<axsl:template match="macos-sc:pwpolicy_item/macos-sc:requiresNumeric" priority="1000" mode="M206">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the requiresNumeric entity of a pwpolicy_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M206"/></axsl:template><axsl:template match="text()" priority="-1" mode="M206"/><axsl:template match="@*|node()" priority="-2" mode="M206"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M206"/></axsl:template>

<!--PATTERN isaitembits-->


	<!--RULE -->
<axsl:template match="sol-sc:isainfo_item/sol-sc:bits" priority="1000" mode="M207">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the bits entity of an isainfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M207"/></axsl:template><axsl:template match="text()" priority="-1" mode="M207"/><axsl:template match="@*|node()" priority="-2" mode="M207"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M207"/></axsl:template>

<!--PATTERN isaitemkernel_isa-->


	<!--RULE -->
<axsl:template match="sol-sc:isainfo_item/sol-sc:kernel_isa" priority="1000" mode="M208">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the kernel_isa entity of an isainfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M208"/></axsl:template><axsl:template match="text()" priority="-1" mode="M208"/><axsl:template match="@*|node()" priority="-2" mode="M208"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M208"/></axsl:template>

<!--PATTERN isaitemapplication_isa-->


	<!--RULE -->
<axsl:template match="sol-sc:isainfo_item/sol-sc:application_isa" priority="1000" mode="M209">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the application_isa entity of an isainfo_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M209"/></axsl:template><axsl:template match="text()" priority="-1" mode="M209"/><axsl:template match="@*|node()" priority="-2" mode="M209"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M209"/></axsl:template>

<!--PATTERN packageitempkginst-->


	<!--RULE -->
<axsl:template match="sol-sc:package_item/sol-sc:pkginst" priority="1000" mode="M210">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pkginst entity of a package_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M210"/></axsl:template><axsl:template match="text()" priority="-1" mode="M210"/><axsl:template match="@*|node()" priority="-2" mode="M210"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M210"/></axsl:template>

<!--PATTERN packageitemname-->


	<!--RULE -->
<axsl:template match="sol-sc:package_item/sol-sc:name" priority="1000" mode="M211">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a package_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M211"/></axsl:template><axsl:template match="text()" priority="-1" mode="M211"/><axsl:template match="@*|node()" priority="-2" mode="M211"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M211"/></axsl:template>

<!--PATTERN packageitemcategory-->


	<!--RULE -->
<axsl:template match="sol-sc:package_item/sol-sc:category" priority="1000" mode="M212">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the category entity of a package_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M212"/></axsl:template><axsl:template match="text()" priority="-1" mode="M212"/><axsl:template match="@*|node()" priority="-2" mode="M212"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M212"/></axsl:template>

<!--PATTERN packageitemversion-->


	<!--RULE -->
<axsl:template match="sol-sc:package_item/sol-sc:version" priority="1000" mode="M213">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a package_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M213"/></axsl:template><axsl:template match="text()" priority="-1" mode="M213"/><axsl:template match="@*|node()" priority="-2" mode="M213"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M213"/></axsl:template>

<!--PATTERN packageitemvendor-->


	<!--RULE -->
<axsl:template match="sol-sc:package_item/sol-sc:vendor" priority="1000" mode="M214">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the vendor entity of a package_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M214"/></axsl:template><axsl:template match="text()" priority="-1" mode="M214"/><axsl:template match="@*|node()" priority="-2" mode="M214"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M214"/></axsl:template>

<!--PATTERN packageitemdescription-->


	<!--RULE -->
<axsl:template match="sol-sc:package_item/sol-sc:description" priority="1000" mode="M215">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the description entity of a package_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M215"/></axsl:template><axsl:template match="text()" priority="-1" mode="M215"/><axsl:template match="@*|node()" priority="-2" mode="M215"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M215"/></axsl:template>

<!--PATTERN patchitembase-->


	<!--RULE -->
<axsl:template match="sol-sc:patch_item/sol-sc:base" priority="1000" mode="M216">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the base entity of a patch_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M216"/></axsl:template><axsl:template match="text()" priority="-1" mode="M216"/><axsl:template match="@*|node()" priority="-2" mode="M216"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M216"/></axsl:template>

<!--PATTERN patchitemversion-->


	<!--RULE -->
<axsl:template match="sol-sc:patch_item/sol-sc:version" priority="1000" mode="M217">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a patch_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M217"/></axsl:template><axsl:template match="text()" priority="-1" mode="M217"/><axsl:template match="@*|node()" priority="-2" mode="M217"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M217"/></axsl:template>

<!--PATTERN fmriitembase-->


	<!--RULE -->
<axsl:template match="sol-sc:smf_item/sol-sc:fmri" priority="1000" mode="M218">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the fmri entity of a smf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M218"/></axsl:template><axsl:template match="text()" priority="-1" mode="M218"/><axsl:template match="@*|node()" priority="-2" mode="M218"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M218"/></axsl:template>

<!--PATTERN srvnameitembase-->


	<!--RULE -->
<axsl:template match="sol-sc:smf_item/sol-sc:service_name" priority="1000" mode="M219">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of a smf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M219"/></axsl:template><axsl:template match="text()" priority="-1" mode="M219"/><axsl:template match="@*|node()" priority="-2" mode="M219"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M219"/></axsl:template>

<!--PATTERN srvstateitembase-->


	<!--RULE -->
<axsl:template match="sol-sc:smf_item/sol-sc:service_state" priority="1000" mode="M220">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_state entity of a smf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M220"/></axsl:template><axsl:template match="text()" priority="-1" mode="M220"/><axsl:template match="@*|node()" priority="-2" mode="M220"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M220"/></axsl:template>

<!--PATTERN protocolitembase-->


	<!--RULE -->
<axsl:template match="sol-sc:smf_item/sol-sc:protocol" priority="1000" mode="M221">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of a smf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M221"/></axsl:template><axsl:template match="text()" priority="-1" mode="M221"/><axsl:template match="@*|node()" priority="-2" mode="M221"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M221"/></axsl:template>

<!--PATTERN srvexeitembase-->


	<!--RULE -->
<axsl:template match="sol-sc:smf_item/sol-sc:server_executable" priority="1000" mode="M222">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_executable entity of a smf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M222"/></axsl:template><axsl:template match="text()" priority="-1" mode="M222"/><axsl:template match="@*|node()" priority="-2" mode="M222"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M222"/></axsl:template>

<!--PATTERN srvargsitembase-->


	<!--RULE -->
<axsl:template match="sol-sc:smf_item/sol-sc:server_arguements" priority="1000" mode="M223">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_arguements entity of a smf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M223"/></axsl:template><axsl:template match="text()" priority="-1" mode="M223"/><axsl:template match="@*|node()" priority="-2" mode="M223"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M223"/></axsl:template>

<!--PATTERN execasuseritembase-->


	<!--RULE -->
<axsl:template match="sol-sc:smf_item/sol-sc:exec_as_user" priority="1000" mode="M224">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exec_as_user entity of a smf_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M224"/></axsl:template><axsl:template match="text()" priority="-1" mode="M224"/><axsl:template match="@*|node()" priority="-2" mode="M224"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M224"/></axsl:template>

<!--PATTERN unixfileitempath-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:path" priority="1000" mode="M225">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M225"/></axsl:template><axsl:template match="text()" priority="-1" mode="M225"/><axsl:template match="@*|node()" priority="-2" mode="M225"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M225"/></axsl:template>

<!--PATTERN unixfileitemfilename-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:filename" priority="1000" mode="M226">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M226"/></axsl:template><axsl:template match="text()" priority="-1" mode="M226"/><axsl:template match="@*|node()" priority="-2" mode="M226"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M226"/></axsl:template>

<!--PATTERN unixfileitemtype-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:type" priority="1000" mode="M227">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M227"/></axsl:template><axsl:template match="text()" priority="-1" mode="M227"/><axsl:template match="@*|node()" priority="-2" mode="M227"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M227"/></axsl:template>

<!--PATTERN unixfileitemgroup_id-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:group_id" priority="1000" mode="M228">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group_id entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M228"/></axsl:template><axsl:template match="text()" priority="-1" mode="M228"/><axsl:template match="@*|node()" priority="-2" mode="M228"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M228"/></axsl:template>

<!--PATTERN unixfileitemuser_id-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:user_id" priority="1000" mode="M229">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M229"/></axsl:template><axsl:template match="text()" priority="-1" mode="M229"/><axsl:template match="@*|node()" priority="-2" mode="M229"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M229"/></axsl:template>

<!--PATTERN unixfileitema_time-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:a_time" priority="1000" mode="M230">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the a_time entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M230"/></axsl:template><axsl:template match="text()" priority="-1" mode="M230"/><axsl:template match="@*|node()" priority="-2" mode="M230"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M230"/></axsl:template>

<!--PATTERN unixfileitemc_time-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:c_time" priority="1000" mode="M231">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the c_time entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M231"/></axsl:template><axsl:template match="text()" priority="-1" mode="M231"/><axsl:template match="@*|node()" priority="-2" mode="M231"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M231"/></axsl:template>

<!--PATTERN unixfileitemm_time-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:m_time" priority="1000" mode="M232">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the m_time entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M232"/></axsl:template><axsl:template match="text()" priority="-1" mode="M232"/><axsl:template match="@*|node()" priority="-2" mode="M232"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M232"/></axsl:template>

<!--PATTERN unixfileitemsize-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:size" priority="1000" mode="M233">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the size entity of a file_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M233"/></axsl:template><axsl:template match="text()" priority="-1" mode="M233"/><axsl:template match="@*|node()" priority="-2" mode="M233"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M233"/></axsl:template>

<!--PATTERN unixfileitemsuid-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:suid" priority="1000" mode="M234">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the suid entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M234"/></axsl:template><axsl:template match="text()" priority="-1" mode="M234"/><axsl:template match="@*|node()" priority="-2" mode="M234"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M234"/></axsl:template>

<!--PATTERN unixfileitemsgid-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:sgid" priority="1000" mode="M235">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sgid entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M235"/></axsl:template><axsl:template match="text()" priority="-1" mode="M235"/><axsl:template match="@*|node()" priority="-2" mode="M235"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M235"/></axsl:template>

<!--PATTERN unixfileitemsticky-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:sticky" priority="1000" mode="M236">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sticky entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M236"/></axsl:template><axsl:template match="text()" priority="-1" mode="M236"/><axsl:template match="@*|node()" priority="-2" mode="M236"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M236"/></axsl:template>

<!--PATTERN unixfileitemuread-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:uread" priority="1000" mode="M237">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uread entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M237"/></axsl:template><axsl:template match="text()" priority="-1" mode="M237"/><axsl:template match="@*|node()" priority="-2" mode="M237"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M237"/></axsl:template>

<!--PATTERN unixfileitemuwrite-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:uwrite" priority="1000" mode="M238">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uwrite entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M238"/></axsl:template><axsl:template match="text()" priority="-1" mode="M238"/><axsl:template match="@*|node()" priority="-2" mode="M238"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M238"/></axsl:template>

<!--PATTERN unixfileitemuexec-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:uexec" priority="1000" mode="M239">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the uexec entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M239"/></axsl:template><axsl:template match="text()" priority="-1" mode="M239"/><axsl:template match="@*|node()" priority="-2" mode="M239"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M239"/></axsl:template>

<!--PATTERN unixfileitemgread-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:gread" priority="1000" mode="M240">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gread entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M240"/></axsl:template><axsl:template match="text()" priority="-1" mode="M240"/><axsl:template match="@*|node()" priority="-2" mode="M240"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M240"/></axsl:template>

<!--PATTERN unixfileitemgwrite-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:gwrite" priority="1000" mode="M241">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gwrite entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M241"/></axsl:template><axsl:template match="text()" priority="-1" mode="M241"/><axsl:template match="@*|node()" priority="-2" mode="M241"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M241"/></axsl:template>

<!--PATTERN unixfileitemgexec-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:gexec" priority="1000" mode="M242">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gexec entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M242"/></axsl:template><axsl:template match="text()" priority="-1" mode="M242"/><axsl:template match="@*|node()" priority="-2" mode="M242"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M242"/></axsl:template>

<!--PATTERN unixfileitemoread-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:oread" priority="1000" mode="M243">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the oread entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M243"/></axsl:template><axsl:template match="text()" priority="-1" mode="M243"/><axsl:template match="@*|node()" priority="-2" mode="M243"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M243"/></axsl:template>

<!--PATTERN unixfileitemowrite-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:owrite" priority="1000" mode="M244">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the owrite entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M244"/></axsl:template><axsl:template match="text()" priority="-1" mode="M244"/><axsl:template match="@*|node()" priority="-2" mode="M244"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M244"/></axsl:template>

<!--PATTERN unixfileitemoexec-->


	<!--RULE -->
<axsl:template match="unix-sc:file_item/unix-sc:oexec" priority="1000" mode="M245">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the oexec entity of a file_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M245"/></axsl:template><axsl:template match="text()" priority="-1" mode="M245"/><axsl:template match="@*|node()" priority="-2" mode="M245"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M245"/></axsl:template>

<!--PATTERN inetditemprotocol-->


	<!--RULE -->
<axsl:template match="unix-sc:inetd_item/unix-sc:protocol" priority="1000" mode="M246">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an inetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M246"/></axsl:template><axsl:template match="text()" priority="-1" mode="M246"/><axsl:template match="@*|node()" priority="-2" mode="M246"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M246"/></axsl:template>

<!--PATTERN inetditemservice_name-->


	<!--RULE -->
<axsl:template match="unix-sc:inetd_item/unix-sc:service_name" priority="1000" mode="M247">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of an inetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M247"/></axsl:template><axsl:template match="text()" priority="-1" mode="M247"/><axsl:template match="@*|node()" priority="-2" mode="M247"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M247"/></axsl:template>

<!--PATTERN inetditemserver_program-->


	<!--RULE -->
<axsl:template match="unix-sc:inetd_item/unix-sc:server_program" priority="1000" mode="M248">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_program entity of an inetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M248"/></axsl:template><axsl:template match="text()" priority="-1" mode="M248"/><axsl:template match="@*|node()" priority="-2" mode="M248"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M248"/></axsl:template>

<!--PATTERN inetditemserver_arguments-->


	<!--RULE -->
<axsl:template match="unix-sc:inetd_item/unix-sc:server_arguments" priority="1000" mode="M249">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_arguments entity of an inetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M249"/></axsl:template><axsl:template match="text()" priority="-1" mode="M249"/><axsl:template match="@*|node()" priority="-2" mode="M249"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M249"/></axsl:template>

<!--PATTERN inetditemendpoint_type-->


	<!--RULE -->
<axsl:template match="unix-sc:inetd_item/unix-sc:endpoint_type" priority="1000" mode="M250">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the endpoint_type entity of an inetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M250"/></axsl:template><axsl:template match="text()" priority="-1" mode="M250"/><axsl:template match="@*|node()" priority="-2" mode="M250"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M250"/></axsl:template>

<!--PATTERN inetditemexec_as_user-->


	<!--RULE -->
<axsl:template match="unix-sc:inetd_item/unix-sc:exec_as_user" priority="1000" mode="M251">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exec_as_user entity of an inetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M251"/></axsl:template><axsl:template match="text()" priority="-1" mode="M251"/><axsl:template match="@*|node()" priority="-2" mode="M251"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M251"/></axsl:template>

<!--PATTERN inetditemwait_status-->


	<!--RULE -->
<axsl:template match="unix-sc:inetd_item/unix-sc:wait_status" priority="1000" mode="M252">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the wait_status entity of an inetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M252"/></axsl:template><axsl:template match="text()" priority="-1" mode="M252"/><axsl:template match="@*|node()" priority="-2" mode="M252"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M252"/></axsl:template>

<!--PATTERN unixinterfaceitemname-->


	<!--RULE -->
<axsl:template match="unix-sc:interface_item/unix-sc:name" priority="1000" mode="M253">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M253"/></axsl:template><axsl:template match="text()" priority="-1" mode="M253"/><axsl:template match="@*|node()" priority="-2" mode="M253"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M253"/></axsl:template>

<!--PATTERN unixinterfaceitemhardware_addr-->


	<!--RULE -->
<axsl:template match="unix-sc:interface_item/unix-sc:hardware_addr" priority="1000" mode="M254">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hardware_addr entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M254"/></axsl:template><axsl:template match="text()" priority="-1" mode="M254"/><axsl:template match="@*|node()" priority="-2" mode="M254"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M254"/></axsl:template>

<!--PATTERN unixinterfaceiteminet_addr-->


	<!--RULE -->
<axsl:template match="unix-sc:interface_item/unix-sc:inet_addr" priority="1000" mode="M255">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the inet_addr entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M255"/></axsl:template><axsl:template match="text()" priority="-1" mode="M255"/><axsl:template match="@*|node()" priority="-2" mode="M255"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M255"/></axsl:template>

<!--PATTERN unixinterfaceitembroadcast_addr-->


	<!--RULE -->
<axsl:template match="unix-sc:interface_item/unix-sc:broadcast_addr" priority="1000" mode="M256">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the broadcast_addr entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M256"/></axsl:template><axsl:template match="text()" priority="-1" mode="M256"/><axsl:template match="@*|node()" priority="-2" mode="M256"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M256"/></axsl:template>

<!--PATTERN unixinterfaceitemnetmask-->


	<!--RULE -->
<axsl:template match="unix-sc:interface_item/unix-sc:netmask" priority="1000" mode="M257">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the netmask entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M257"/></axsl:template><axsl:template match="text()" priority="-1" mode="M257"/><axsl:template match="@*|node()" priority="-2" mode="M257"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M257"/></axsl:template>

<!--PATTERN unixinterfaceitemflag-->


	<!--RULE -->
<axsl:template match="unix-sc:interface_item/unix-sc:flag" priority="1000" mode="M258">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the flag entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M258"/></axsl:template><axsl:template match="text()" priority="-1" mode="M258"/><axsl:template match="@*|node()" priority="-2" mode="M258"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M258"/></axsl:template>

<!--PATTERN passworditemusername-->


	<!--RULE -->
<axsl:template match="unix-sc:password_item/unix-sc:username" priority="1000" mode="M259">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a password_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M259"/></axsl:template><axsl:template match="text()" priority="-1" mode="M259"/><axsl:template match="@*|node()" priority="-2" mode="M259"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M259"/></axsl:template>

<!--PATTERN passworditempassword-->


	<!--RULE -->
<axsl:template match="unix-sc:password_item/unix-sc:password" priority="1000" mode="M260">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password entity of a password_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M260"/></axsl:template><axsl:template match="text()" priority="-1" mode="M260"/><axsl:template match="@*|node()" priority="-2" mode="M260"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M260"/></axsl:template>

<!--PATTERN passworditemuser_id-->


	<!--RULE -->
<axsl:template match="unix-sc:password_item/unix-sc:user_id" priority="1000" mode="M261">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of a password_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M261"/></axsl:template><axsl:template match="text()" priority="-1" mode="M261"/><axsl:template match="@*|node()" priority="-2" mode="M261"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M261"/></axsl:template>

<!--PATTERN passworditemgroup_id-->


	<!--RULE -->
<axsl:template match="unix-sc:password_item/unix-sc:group_id" priority="1000" mode="M262">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group_id entity of a password_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M262"/></axsl:template><axsl:template match="text()" priority="-1" mode="M262"/><axsl:template match="@*|node()" priority="-2" mode="M262"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M262"/></axsl:template>

<!--PATTERN passworditemgcos-->


	<!--RULE -->
<axsl:template match="unix-sc:password_item/unix-sc:gcos" priority="1000" mode="M263">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the gcos entity of a password_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M263"/></axsl:template><axsl:template match="text()" priority="-1" mode="M263"/><axsl:template match="@*|node()" priority="-2" mode="M263"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M263"/></axsl:template>

<!--PATTERN passworditemhome_dir-->


	<!--RULE -->
<axsl:template match="unix-sc:password_item/unix-sc:home_dir" priority="1000" mode="M264">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the home_dir entity of a password_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M264"/></axsl:template><axsl:template match="text()" priority="-1" mode="M264"/><axsl:template match="@*|node()" priority="-2" mode="M264"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M264"/></axsl:template>

<!--PATTERN passworditemlogin_shell-->


	<!--RULE -->
<axsl:template match="unix-sc:password_item/unix-sc:login_shell" priority="1000" mode="M265">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the login_shell entity of a password_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M265"/></axsl:template><axsl:template match="text()" priority="-1" mode="M265"/><axsl:template match="@*|node()" priority="-2" mode="M265"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M265"/></axsl:template>

<!--PATTERN unixprocessitemcommand-->


	<!--RULE -->
<axsl:template match="unix-sc:process_item/unix-sc:command" priority="1000" mode="M266">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the command entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M266"/></axsl:template><axsl:template match="text()" priority="-1" mode="M266"/><axsl:template match="@*|node()" priority="-2" mode="M266"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M266"/></axsl:template>

<!--PATTERN unixprocessitemexec_time-->


	<!--RULE -->
<axsl:template match="unix-sc:process_item/unix-sc:exec_time" priority="1000" mode="M267">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exec_time entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M267"/></axsl:template><axsl:template match="text()" priority="-1" mode="M267"/><axsl:template match="@*|node()" priority="-2" mode="M267"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M267"/></axsl:template>

<!--PATTERN unixprocessitempid-->


	<!--RULE -->
<axsl:template match="unix-sc:process_item/unix-sc:pid" priority="1000" mode="M268">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of a process_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M268"/></axsl:template><axsl:template match="text()" priority="-1" mode="M268"/><axsl:template match="@*|node()" priority="-2" mode="M268"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M268"/></axsl:template>

<!--PATTERN unixprocessitemppid-->


	<!--RULE -->
<axsl:template match="unix-sc:process_item/unix-sc:ppid" priority="1000" mode="M269">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ppid entity of a process_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M269"/></axsl:template><axsl:template match="text()" priority="-1" mode="M269"/><axsl:template match="@*|node()" priority="-2" mode="M269"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M269"/></axsl:template>

<!--PATTERN unixprocessitempriority-->


	<!--RULE -->
<axsl:template match="unix-sc:process_item/unix-sc:priority" priority="1000" mode="M270">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the priority entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M270"/></axsl:template><axsl:template match="text()" priority="-1" mode="M270"/><axsl:template match="@*|node()" priority="-2" mode="M270"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M270"/></axsl:template>

<!--PATTERN unixprocessitemscheduling_class-->


	<!--RULE -->
<axsl:template match="unix-sc:process_item/unix-sc:scheduling_class" priority="1000" mode="M271">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the scheduling_class entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M271"/></axsl:template><axsl:template match="text()" priority="-1" mode="M271"/><axsl:template match="@*|node()" priority="-2" mode="M271"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M271"/></axsl:template>

<!--PATTERN unixprocessitemstart_time-->


	<!--RULE -->
<axsl:template match="unix-sc:process_item/unix-sc:start_time" priority="1000" mode="M272">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the start_time entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M272"/></axsl:template><axsl:template match="text()" priority="-1" mode="M272"/><axsl:template match="@*|node()" priority="-2" mode="M272"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M272"/></axsl:template>

<!--PATTERN unixprocessitemtty-->


	<!--RULE -->
<axsl:template match="unix-sc:process_item/unix-sc:tty" priority="1000" mode="M273">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the tty entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M273"/></axsl:template><axsl:template match="text()" priority="-1" mode="M273"/><axsl:template match="@*|node()" priority="-2" mode="M273"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M273"/></axsl:template>

<!--PATTERN unixprocessitemuser_id-->


	<!--RULE -->
<axsl:template match="unix-sc:process_item/unix-sc:user_id" priority="1000" mode="M274">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_id entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M274"/></axsl:template><axsl:template match="text()" priority="-1" mode="M274"/><axsl:template match="@*|node()" priority="-2" mode="M274"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M274"/></axsl:template>

<!--PATTERN unixrlitemservice_name-->


	<!--RULE -->
<axsl:template match="unix-sc:runlevel_item/unix-sc:service_name" priority="1000" mode="M275">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of a runlevel_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M275"/></axsl:template><axsl:template match="text()" priority="-1" mode="M275"/><axsl:template match="@*|node()" priority="-2" mode="M275"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M275"/></axsl:template>

<!--PATTERN unixrlitemrunlevel-->


	<!--RULE -->
<axsl:template match="unix-sc:runlevel_item/unix-sc:runlevel" priority="1000" mode="M276">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the runlevel entity of a runlevel_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M276"/></axsl:template><axsl:template match="text()" priority="-1" mode="M276"/><axsl:template match="@*|node()" priority="-2" mode="M276"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M276"/></axsl:template>

<!--PATTERN unixrlitemstart-->


	<!--RULE -->
<axsl:template match="unix-sc:runlevel_item/unix-sc:start" priority="1000" mode="M277">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the start entity of a runlevel_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M277"/></axsl:template><axsl:template match="text()" priority="-1" mode="M277"/><axsl:template match="@*|node()" priority="-2" mode="M277"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M277"/></axsl:template>

<!--PATTERN unixrlitemkill-->


	<!--RULE -->
<axsl:template match="unix-sc:runlevel_item/unix-sc:kill" priority="1000" mode="M278">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the kill entity of a runlevel_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M278"/></axsl:template><axsl:template match="text()" priority="-1" mode="M278"/><axsl:template match="@*|node()" priority="-2" mode="M278"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M278"/></axsl:template>

<!--PATTERN sccsitempath-->


	<!--RULE -->
<axsl:template match="unix-sc:sccs_item/unix-sc:path" priority="1000" mode="M279">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M279"/></axsl:template><axsl:template match="text()" priority="-1" mode="M279"/><axsl:template match="@*|node()" priority="-2" mode="M279"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M279"/></axsl:template>

<!--PATTERN sccsitemfilename-->


	<!--RULE -->
<axsl:template match="unix-sc:sccs_item/unix-sc:filename" priority="1000" mode="M280">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M280"/></axsl:template><axsl:template match="text()" priority="-1" mode="M280"/><axsl:template match="@*|node()" priority="-2" mode="M280"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M280"/></axsl:template>

<!--PATTERN sccsitemmodule_name-->


	<!--RULE -->
<axsl:template match="unix-sc:sccs_item/unix-sc:module_name" priority="1000" mode="M281">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the module_name entity of a process_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M281"/></axsl:template><axsl:template match="text()" priority="-1" mode="M281"/><axsl:template match="@*|node()" priority="-2" mode="M281"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M281"/></axsl:template>

<!--PATTERN sccsitemmodule_type-->


	<!--RULE -->
<axsl:template match="unix-sc:sccs_item/unix-sc:module_type" priority="1000" mode="M282">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the module_type entity of a process_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M282"/></axsl:template><axsl:template match="text()" priority="-1" mode="M282"/><axsl:template match="@*|node()" priority="-2" mode="M282"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M282"/></axsl:template>

<!--PATTERN sccsitemrelease-->


	<!--RULE -->
<axsl:template match="unix-sc:sccs_item/unix-sc:release" priority="1000" mode="M283">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the release entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M283"/></axsl:template><axsl:template match="text()" priority="-1" mode="M283"/><axsl:template match="@*|node()" priority="-2" mode="M283"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M283"/></axsl:template>

<!--PATTERN sccsitemlevel-->


	<!--RULE -->
<axsl:template match="unix-sc:sccs_item/unix-sc:level" priority="1000" mode="M284">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the level entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M284"/></axsl:template><axsl:template match="text()" priority="-1" mode="M284"/><axsl:template match="@*|node()" priority="-2" mode="M284"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M284"/></axsl:template>

<!--PATTERN sccsitembranch-->


	<!--RULE -->
<axsl:template match="unix-sc:sccs_item/unix-sc:branch" priority="1000" mode="M285">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the branch entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M285"/></axsl:template><axsl:template match="text()" priority="-1" mode="M285"/><axsl:template match="@*|node()" priority="-2" mode="M285"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M285"/></axsl:template>

<!--PATTERN sccsitemsequence-->


	<!--RULE -->
<axsl:template match="unix-sc:sccs_item/unix-sc:sequence" priority="1000" mode="M286">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sequence entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M286"/></axsl:template><axsl:template match="text()" priority="-1" mode="M286"/><axsl:template match="@*|node()" priority="-2" mode="M286"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M286"/></axsl:template>

<!--PATTERN sccsitemwhat_string-->


	<!--RULE -->
<axsl:template match="unix-sc:sccs_item/unix-sc:what_string" priority="1000" mode="M287">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the what_string entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M287"/></axsl:template><axsl:template match="text()" priority="-1" mode="M287"/><axsl:template match="@*|node()" priority="-2" mode="M287"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M287"/></axsl:template>

<!--PATTERN shadowitemusername-->


	<!--RULE -->
<axsl:template match="unix-sc:shadow_item/unix-sc:username" priority="1000" mode="M288">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the username entity of a shadow_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M288"/></axsl:template><axsl:template match="text()" priority="-1" mode="M288"/><axsl:template match="@*|node()" priority="-2" mode="M288"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M288"/></axsl:template>

<!--PATTERN shadowitempassword-->


	<!--RULE -->
<axsl:template match="unix-sc:shadow_item/unix-sc:password" priority="1000" mode="M289">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password entity of a shadow_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M289"/></axsl:template><axsl:template match="text()" priority="-1" mode="M289"/><axsl:template match="@*|node()" priority="-2" mode="M289"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M289"/></axsl:template>

<!--PATTERN shadowitemchg_lst-->


	<!--RULE -->
<axsl:template match="unix-sc:shadow_item/unix-sc:chg_lst" priority="1000" mode="M290">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the chg_lst entity of a shadow_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M290"/></axsl:template><axsl:template match="text()" priority="-1" mode="M290"/><axsl:template match="@*|node()" priority="-2" mode="M290"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M290"/></axsl:template>

<!--PATTERN shadowitemchg_allow-->


	<!--RULE -->
<axsl:template match="unix-sc:shadow_item/unix-sc:chg_allow" priority="1000" mode="M291">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the chg_allow entity of a shadow_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M291"/></axsl:template><axsl:template match="text()" priority="-1" mode="M291"/><axsl:template match="@*|node()" priority="-2" mode="M291"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M291"/></axsl:template>

<!--PATTERN shadowitemchg_req-->


	<!--RULE -->
<axsl:template match="unix-sc:shadow_item/unix-sc:chg_req" priority="1000" mode="M292">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the chg_req entity of a shadow_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M292"/></axsl:template><axsl:template match="text()" priority="-1" mode="M292"/><axsl:template match="@*|node()" priority="-2" mode="M292"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M292"/></axsl:template>

<!--PATTERN shadowitemexp_warn-->


	<!--RULE -->
<axsl:template match="unix-sc:shadow_item/unix-sc:exp_warn" priority="1000" mode="M293">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exp_warn entity of a shadow_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M293"/></axsl:template><axsl:template match="text()" priority="-1" mode="M293"/><axsl:template match="@*|node()" priority="-2" mode="M293"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M293"/></axsl:template>

<!--PATTERN shadowitemexp_inact-->


	<!--RULE -->
<axsl:template match="unix-sc:shadow_item/unix-sc:exp_inact" priority="1000" mode="M294">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exp_inact entity of a shadow_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M294"/></axsl:template><axsl:template match="text()" priority="-1" mode="M294"/><axsl:template match="@*|node()" priority="-2" mode="M294"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M294"/></axsl:template>

<!--PATTERN shadowitemexp_date-->


	<!--RULE -->
<axsl:template match="unix-sc:shadow_item/unix-sc:exp_date" priority="1000" mode="M295">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the exp_date entity of a shadow_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M295"/></axsl:template><axsl:template match="text()" priority="-1" mode="M295"/><axsl:template match="@*|node()" priority="-2" mode="M295"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M295"/></axsl:template>

<!--PATTERN shadowitemflag-->


	<!--RULE -->
<axsl:template match="unix-sc:shadow_item/unix-sc:flag" priority="1000" mode="M296">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the flag entity of a shadow_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M296"/></axsl:template><axsl:template match="text()" priority="-1" mode="M296"/><axsl:template match="@*|node()" priority="-2" mode="M296"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M296"/></axsl:template>

<!--PATTERN unameitemmachine_class-->


	<!--RULE -->
<axsl:template match="unix-sc:uname_item/unix-sc:machine_class" priority="1000" mode="M297">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the machine_class entity of a uname_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M297"/></axsl:template><axsl:template match="text()" priority="-1" mode="M297"/><axsl:template match="@*|node()" priority="-2" mode="M297"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M297"/></axsl:template>

<!--PATTERN unameitemnode_name-->


	<!--RULE -->
<axsl:template match="unix-sc:uname_item/unix-sc:node_name" priority="1000" mode="M298">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the node_name entity of a uname_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M298"/></axsl:template><axsl:template match="text()" priority="-1" mode="M298"/><axsl:template match="@*|node()" priority="-2" mode="M298"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M298"/></axsl:template>

<!--PATTERN unameitemos_name-->


	<!--RULE -->
<axsl:template match="unix-sc:uname_item/unix-sc:os_name" priority="1000" mode="M299">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the os_name entity of a uname_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M299"/></axsl:template><axsl:template match="text()" priority="-1" mode="M299"/><axsl:template match="@*|node()" priority="-2" mode="M299"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M299"/></axsl:template>

<!--PATTERN unameitemos_release-->


	<!--RULE -->
<axsl:template match="unix-sc:uname_item/unix-sc:os_release" priority="1000" mode="M300">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the os_release entity of a uname_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M300"/></axsl:template><axsl:template match="text()" priority="-1" mode="M300"/><axsl:template match="@*|node()" priority="-2" mode="M300"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M300"/></axsl:template>

<!--PATTERN unameitemos_version-->


	<!--RULE -->
<axsl:template match="unix-sc:uname_item/unix-sc:os_version" priority="1000" mode="M301">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the os_version entity of a uname_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M301"/></axsl:template><axsl:template match="text()" priority="-1" mode="M301"/><axsl:template match="@*|node()" priority="-2" mode="M301"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M301"/></axsl:template>

<!--PATTERN unameitemprocessor_type-->


	<!--RULE -->
<axsl:template match="unix-sc:uname_item/unix-sc:processor_type" priority="1000" mode="M302">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the processor_type entity of a uname_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M302"/></axsl:template><axsl:template match="text()" priority="-1" mode="M302"/><axsl:template match="@*|node()" priority="-2" mode="M302"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M302"/></axsl:template>

<!--PATTERN xinetditemprotocol-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:protocol" priority="1000" mode="M303">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M303"/></axsl:template><axsl:template match="text()" priority="-1" mode="M303"/><axsl:template match="@*|node()" priority="-2" mode="M303"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M303"/></axsl:template>

<!--PATTERN xinetditemservice_name-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:service_name" priority="1000" mode="M304">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the service_name entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M304"/></axsl:template><axsl:template match="text()" priority="-1" mode="M304"/><axsl:template match="@*|node()" priority="-2" mode="M304"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M304"/></axsl:template>

<!--PATTERN xinetditemflags-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:flags" priority="1000" mode="M305">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the flags entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M305"/></axsl:template><axsl:template match="text()" priority="-1" mode="M305"/><axsl:template match="@*|node()" priority="-2" mode="M305"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M305"/></axsl:template>

<!--PATTERN xinetditemnoaccess-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:no_access" priority="1000" mode="M306">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the no_access entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M306"/></axsl:template><axsl:template match="text()" priority="-1" mode="M306"/><axsl:template match="@*|node()" priority="-2" mode="M306"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M306"/></axsl:template>

<!--PATTERN xinetditemonlyfrom-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:only_from" priority="1000" mode="M307">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the only_from entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M307"/></axsl:template><axsl:template match="text()" priority="-1" mode="M307"/><axsl:template match="@*|node()" priority="-2" mode="M307"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M307"/></axsl:template>

<!--PATTERN xinetditemport-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:port" priority="1000" mode="M308">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the port entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M308"/></axsl:template><axsl:template match="text()" priority="-1" mode="M308"/><axsl:template match="@*|node()" priority="-2" mode="M308"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M308"/></axsl:template>

<!--PATTERN xinetditemserver-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:server" priority="1000" mode="M309">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M309"/></axsl:template><axsl:template match="text()" priority="-1" mode="M309"/><axsl:template match="@*|node()" priority="-2" mode="M309"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M309"/></axsl:template>

<!--PATTERN xinetditemserver_arguments-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:server_arguments" priority="1000" mode="M310">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the server_arguments entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M310"/></axsl:template><axsl:template match="text()" priority="-1" mode="M310"/><axsl:template match="@*|node()" priority="-2" mode="M310"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M310"/></axsl:template>

<!--PATTERN xinetditemsockettype-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:socket_type" priority="1000" mode="M311">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the socket_type entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M311"/></axsl:template><axsl:template match="text()" priority="-1" mode="M311"/><axsl:template match="@*|node()" priority="-2" mode="M311"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M311"/></axsl:template>

<!--PATTERN xinetditemtype-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:type" priority="1000" mode="M312">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M312"/></axsl:template><axsl:template match="text()" priority="-1" mode="M312"/><axsl:template match="@*|node()" priority="-2" mode="M312"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M312"/></axsl:template>

<!--PATTERN xinetditemuser-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:user" priority="1000" mode="M313">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user entity of an xinetd_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M313"/></axsl:template><axsl:template match="text()" priority="-1" mode="M313"/><axsl:template match="@*|node()" priority="-2" mode="M313"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M313"/></axsl:template>

<!--PATTERN xinetditemwait-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:wait" priority="1000" mode="M314">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the wait entity of an xinetd_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M314"/></axsl:template><axsl:template match="text()" priority="-1" mode="M314"/><axsl:template match="@*|node()" priority="-2" mode="M314"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M314"/></axsl:template>

<!--PATTERN xinetditemdisabled-->


	<!--RULE -->
<axsl:template match="unix-sc:xinetd_item/unix-sc:disabled" priority="1000" mode="M315">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the disabled entity of an xinetd_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M315"/></axsl:template><axsl:template match="text()" priority="-1" mode="M315"/><axsl:template match="@*|node()" priority="-2" mode="M315"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M315"/></axsl:template>

<!--PATTERN atitemsecurity_principle-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:security_principle" priority="1000" mode="M316">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the security_principle entity of an accesstoken_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M316"/></axsl:template><axsl:template match="text()" priority="-1" mode="M316"/><axsl:template match="@*|node()" priority="-2" mode="M316"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M316"/></axsl:template>

<!--PATTERN atitemseassignprimarytokenprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seassignprimarytokenprivilege" priority="1000" mode="M317">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seassignprimarytokenprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M317"/></axsl:template><axsl:template match="text()" priority="-1" mode="M317"/><axsl:template match="@*|node()" priority="-2" mode="M317"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M317"/></axsl:template>

<!--PATTERN atitemseauditprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seauditprivilege" priority="1000" mode="M318">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seauditprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M318"/></axsl:template><axsl:template match="text()" priority="-1" mode="M318"/><axsl:template match="@*|node()" priority="-2" mode="M318"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M318"/></axsl:template>

<!--PATTERN atitemsebackupprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sebackupprivilege" priority="1000" mode="M319">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sebackupprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M319"/></axsl:template><axsl:template match="text()" priority="-1" mode="M319"/><axsl:template match="@*|node()" priority="-2" mode="M319"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M319"/></axsl:template>

<!--PATTERN atitemsechangenotifyprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sechangenotifyprivilege" priority="1000" mode="M320">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sechangenotifyprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M320"/></axsl:template><axsl:template match="text()" priority="-1" mode="M320"/><axsl:template match="@*|node()" priority="-2" mode="M320"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M320"/></axsl:template>

<!--PATTERN atitemsecreateglobalprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:secreateglobalprivilege" priority="1000" mode="M321">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreateglobalprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M321"/></axsl:template><axsl:template match="text()" priority="-1" mode="M321"/><axsl:template match="@*|node()" priority="-2" mode="M321"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M321"/></axsl:template>

<!--PATTERN atitemsecreatepagefileprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:secreatepagefileprivilege" priority="1000" mode="M322">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreatepagefileprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M322"/></axsl:template><axsl:template match="text()" priority="-1" mode="M322"/><axsl:template match="@*|node()" priority="-2" mode="M322"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M322"/></axsl:template>

<!--PATTERN atitemsecreatepermanentprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:secreatepermanentprivilege" priority="1000" mode="M323">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreatepermanentprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M323"/></axsl:template><axsl:template match="text()" priority="-1" mode="M323"/><axsl:template match="@*|node()" priority="-2" mode="M323"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M323"/></axsl:template>

<!--PATTERN atitemsecreatesymboliclinkprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:secreatesymboliclinkprivilege" priority="1000" mode="M324">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreatesymboliclinkprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M324"/></axsl:template><axsl:template match="text()" priority="-1" mode="M324"/><axsl:template match="@*|node()" priority="-2" mode="M324"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M324"/></axsl:template>

<!--PATTERN atitemsecreatetokenprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:secreatetokenprivilege" priority="1000" mode="M325">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secreatetokenprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M325"/></axsl:template><axsl:template match="text()" priority="-1" mode="M325"/><axsl:template match="@*|node()" priority="-2" mode="M325"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M325"/></axsl:template>

<!--PATTERN atitemsedebugprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sedebugprivilege" priority="1000" mode="M326">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedebugprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M326"/></axsl:template><axsl:template match="text()" priority="-1" mode="M326"/><axsl:template match="@*|node()" priority="-2" mode="M326"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M326"/></axsl:template>

<!--PATTERN atitemseenabledelegationprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seenabledelegationprivilege" priority="1000" mode="M327">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seenabledelegationprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M327"/></axsl:template><axsl:template match="text()" priority="-1" mode="M327"/><axsl:template match="@*|node()" priority="-2" mode="M327"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M327"/></axsl:template>

<!--PATTERN atitemseimpersonateprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seimpersonateprivilege" priority="1000" mode="M328">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seimpersonateprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M328"/></axsl:template><axsl:template match="text()" priority="-1" mode="M328"/><axsl:template match="@*|node()" priority="-2" mode="M328"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M328"/></axsl:template>

<!--PATTERN atitemseincreasebasepriorityprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seincreasebasepriorityprivilege" priority="1000" mode="M329">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seincreasebasepriorityprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M329"/></axsl:template><axsl:template match="text()" priority="-1" mode="M329"/><axsl:template match="@*|node()" priority="-2" mode="M329"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M329"/></axsl:template>

<!--PATTERN atitemseincreasequotaprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seincreasequotaprivilege" priority="1000" mode="M330">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seincreasequotaprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M330"/></axsl:template><axsl:template match="text()" priority="-1" mode="M330"/><axsl:template match="@*|node()" priority="-2" mode="M330"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M330"/></axsl:template>

<!--PATTERN atitemseincreaseworkingsetprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seincreaseworkingsetprivilege" priority="1000" mode="M331">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seincreaseworkingsetprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M331"/></axsl:template><axsl:template match="text()" priority="-1" mode="M331"/><axsl:template match="@*|node()" priority="-2" mode="M331"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M331"/></axsl:template>

<!--PATTERN atitemseloaddriverprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seloaddriverprivilege" priority="1000" mode="M332">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seloaddriverprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M332"/></axsl:template><axsl:template match="text()" priority="-1" mode="M332"/><axsl:template match="@*|node()" priority="-2" mode="M332"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M332"/></axsl:template>

<!--PATTERN atitemselockmemoryprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:selockmemoryprivilege" priority="1000" mode="M333">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the selockmemoryprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M333"/></axsl:template><axsl:template match="text()" priority="-1" mode="M333"/><axsl:template match="@*|node()" priority="-2" mode="M333"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M333"/></axsl:template>

<!--PATTERN atitemsemachineaccountprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:semachineaccountprivilege" priority="1000" mode="M334">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the semachineaccountprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M334"/></axsl:template><axsl:template match="text()" priority="-1" mode="M334"/><axsl:template match="@*|node()" priority="-2" mode="M334"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M334"/></axsl:template>

<!--PATTERN atitemsemanagevolumeprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:semanagevolumeprivilege" priority="1000" mode="M335">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the semanagevolumeprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M335"/></axsl:template><axsl:template match="text()" priority="-1" mode="M335"/><axsl:template match="@*|node()" priority="-2" mode="M335"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M335"/></axsl:template>

<!--PATTERN atitemseprofilesingleprocessprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seprofilesingleprocessprivilege" priority="1000" mode="M336">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seprofilesingleprocessprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M336"/></axsl:template><axsl:template match="text()" priority="-1" mode="M336"/><axsl:template match="@*|node()" priority="-2" mode="M336"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M336"/></axsl:template>

<!--PATTERN atitemserelabelprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:serelabelprivilege" priority="1000" mode="M337">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the serelabelprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M337"/></axsl:template><axsl:template match="text()" priority="-1" mode="M337"/><axsl:template match="@*|node()" priority="-2" mode="M337"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M337"/></axsl:template>

<!--PATTERN atitemseremoteshutdownprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seremoteshutdownprivilege" priority="1000" mode="M338">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seremoteshutdownprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M338"/></axsl:template><axsl:template match="text()" priority="-1" mode="M338"/><axsl:template match="@*|node()" priority="-2" mode="M338"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M338"/></axsl:template>

<!--PATTERN atitemserestoreprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:serestoreprivilege" priority="1000" mode="M339">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the serestoreprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M339"/></axsl:template><axsl:template match="text()" priority="-1" mode="M339"/><axsl:template match="@*|node()" priority="-2" mode="M339"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M339"/></axsl:template>

<!--PATTERN atitemsesecurityprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sesecurityprivilege" priority="1000" mode="M340">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesecurityprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M340"/></axsl:template><axsl:template match="text()" priority="-1" mode="M340"/><axsl:template match="@*|node()" priority="-2" mode="M340"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M340"/></axsl:template>

<!--PATTERN atitemseshutdownprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seshutdownprivilege" priority="1000" mode="M341">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seshutdownprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M341"/></axsl:template><axsl:template match="text()" priority="-1" mode="M341"/><axsl:template match="@*|node()" priority="-2" mode="M341"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M341"/></axsl:template>

<!--PATTERN atitemsesyncagentprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sesyncagentprivilege" priority="1000" mode="M342">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesyncagentprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M342"/></axsl:template><axsl:template match="text()" priority="-1" mode="M342"/><axsl:template match="@*|node()" priority="-2" mode="M342"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M342"/></axsl:template>

<!--PATTERN atitemsesystemenvironmentprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sesystemenvironmentprivilege" priority="1000" mode="M343">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesystemenvironmentprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M343"/></axsl:template><axsl:template match="text()" priority="-1" mode="M343"/><axsl:template match="@*|node()" priority="-2" mode="M343"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M343"/></axsl:template>

<!--PATTERN atitemsesystemprofileprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sesystemprofileprivilege" priority="1000" mode="M344">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesystemprofileprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M344"/></axsl:template><axsl:template match="text()" priority="-1" mode="M344"/><axsl:template match="@*|node()" priority="-2" mode="M344"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M344"/></axsl:template>

<!--PATTERN atitemsesystemtimeprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sesystemtimeprivilege" priority="1000" mode="M345">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sesystemtimeprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M345"/></axsl:template><axsl:template match="text()" priority="-1" mode="M345"/><axsl:template match="@*|node()" priority="-2" mode="M345"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M345"/></axsl:template>

<!--PATTERN atitemsetakeownershipprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:setakeownershipprivilege" priority="1000" mode="M346">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the setakeownershipprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M346"/></axsl:template><axsl:template match="text()" priority="-1" mode="M346"/><axsl:template match="@*|node()" priority="-2" mode="M346"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M346"/></axsl:template>

<!--PATTERN atitemsetcbprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:setcbprivilege" priority="1000" mode="M347">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the setcbprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M347"/></axsl:template><axsl:template match="text()" priority="-1" mode="M347"/><axsl:template match="@*|node()" priority="-2" mode="M347"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M347"/></axsl:template>

<!--PATTERN atitemsetimezoneprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:setimezoneprivilege" priority="1000" mode="M348">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the setimezoneprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M348"/></axsl:template><axsl:template match="text()" priority="-1" mode="M348"/><axsl:template match="@*|node()" priority="-2" mode="M348"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M348"/></axsl:template>

<!--PATTERN atitemseundockprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seundockprivilege" priority="1000" mode="M349">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seundockprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M349"/></axsl:template><axsl:template match="text()" priority="-1" mode="M349"/><axsl:template match="@*|node()" priority="-2" mode="M349"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M349"/></axsl:template>

<!--PATTERN atitemseunsolicitedinputprivilege-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seunsolicitedinputprivilege" priority="1000" mode="M350">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seunsolicitedinputprivilege entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M350"/></axsl:template><axsl:template match="text()" priority="-1" mode="M350"/><axsl:template match="@*|node()" priority="-2" mode="M350"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M350"/></axsl:template>

<!--PATTERN atitemsebatchlogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sebatchlogonright" priority="1000" mode="M351">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sebatchlogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M351"/></axsl:template><axsl:template match="text()" priority="-1" mode="M351"/><axsl:template match="@*|node()" priority="-2" mode="M351"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M351"/></axsl:template>

<!--PATTERN atitemseinteractivelogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seinteractivelogonright" priority="1000" mode="M352">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seinteractivelogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M352"/></axsl:template><axsl:template match="text()" priority="-1" mode="M352"/><axsl:template match="@*|node()" priority="-2" mode="M352"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M352"/></axsl:template>

<!--PATTERN atitemsenetworklogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:senetworklogonright" priority="1000" mode="M353">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the senetworklogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M353"/></axsl:template><axsl:template match="text()" priority="-1" mode="M353"/><axsl:template match="@*|node()" priority="-2" mode="M353"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M353"/></axsl:template>

<!--PATTERN atitemseremoteinteractivelogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seremoteinteractivelogonright" priority="1000" mode="M354">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seremoteinteractivelogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M354"/></axsl:template><axsl:template match="text()" priority="-1" mode="M354"/><axsl:template match="@*|node()" priority="-2" mode="M354"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M354"/></axsl:template>

<!--PATTERN atitemseservicelogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:seservicelogonright" priority="1000" mode="M355">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the seservicelogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M355"/></axsl:template><axsl:template match="text()" priority="-1" mode="M355"/><axsl:template match="@*|node()" priority="-2" mode="M355"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M355"/></axsl:template>

<!--PATTERN atitemsedenybatchLogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sedenybatchLogonright" priority="1000" mode="M356">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenybatchLogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M356"/></axsl:template><axsl:template match="text()" priority="-1" mode="M356"/><axsl:template match="@*|node()" priority="-2" mode="M356"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M356"/></axsl:template>

<!--PATTERN atitemsedenyinteractivelogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sedenyinteractivelogonright" priority="1000" mode="M357">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenyinteractivelogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M357"/></axsl:template><axsl:template match="text()" priority="-1" mode="M357"/><axsl:template match="@*|node()" priority="-2" mode="M357"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M357"/></axsl:template>

<!--PATTERN atitemsedenynetworklogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sedenynetworklogonright" priority="1000" mode="M358">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenynetworklogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M358"/></axsl:template><axsl:template match="text()" priority="-1" mode="M358"/><axsl:template match="@*|node()" priority="-2" mode="M358"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M358"/></axsl:template>

<!--PATTERN atitemsedenyremoteInteractivelogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sedenyremoteInteractivelogonright" priority="1000" mode="M359">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenyremoteInteractivelogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M359"/></axsl:template><axsl:template match="text()" priority="-1" mode="M359"/><axsl:template match="@*|node()" priority="-2" mode="M359"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M359"/></axsl:template>

<!--PATTERN atitemsedenyservicelogonright-->


	<!--RULE -->
<axsl:template match="win-sc:accesstoken_item/win-sc:sedenyservicelogonright" priority="1000" mode="M360">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sedenyservicelogonright entity of an accesstoken_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M360"/></axsl:template><axsl:template match="text()" priority="-1" mode="M360"/><axsl:template match="@*|node()" priority="-2" mode="M360"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M360"/></axsl:template>

<!--PATTERN aditemnaming_context-->


	<!--RULE -->
<axsl:template match="win-sc:activedirectory_item/win-sc:naming_context" priority="1000" mode="M361">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the naming_context entity of an activedirectory_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M361"/></axsl:template><axsl:template match="text()" priority="-1" mode="M361"/><axsl:template match="@*|node()" priority="-2" mode="M361"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M361"/></axsl:template>

<!--PATTERN aditemrelative_dn-->


	<!--RULE -->
<axsl:template match="win-sc:activedirectory_item/win-sc:relative_dn" priority="1000" mode="M362">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the relative_dn entity of an activedirectory_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@xsi:nil='true') or ../win-sc:attribute/@xsi:nil='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - attribute entity must be nil when relative_dn is nil<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M362"/></axsl:template><axsl:template match="text()" priority="-1" mode="M362"/><axsl:template match="@*|node()" priority="-2" mode="M362"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M362"/></axsl:template>

<!--PATTERN aditemattribute-->


	<!--RULE -->
<axsl:template match="win-sc:activedirectory_item/win-sc:attribute" priority="1000" mode="M363">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the attribute entity of an activedirectory_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M363"/></axsl:template><axsl:template match="text()" priority="-1" mode="M363"/><axsl:template match="@*|node()" priority="-2" mode="M363"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M363"/></axsl:template>

<!--PATTERN aditemobject_class-->


	<!--RULE -->
<axsl:template match="win-sc:activedirectory_item/win-sc:object_class" priority="1000" mode="M364">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the object_class entity of an activedirectory_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M364"/></axsl:template><axsl:template match="text()" priority="-1" mode="M364"/><axsl:template match="@*|node()" priority="-2" mode="M364"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M364"/></axsl:template>

<!--PATTERN aditemadstype-->


	<!--RULE -->
<axsl:template match="win-sc:activedirectory_item/win-sc:adstype" priority="1000" mode="M365">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the adstype entity of an activedirectory_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M365"/></axsl:template><axsl:template match="text()" priority="-1" mode="M365"/><axsl:template match="@*|node()" priority="-2" mode="M365"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M365"/></axsl:template>

<!--PATTERN aepitemaccount_logon-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicy_item/win-sc:account_logon" priority="1000" mode="M366">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the account_logon entity of an auditeventpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M366"/></axsl:template><axsl:template match="text()" priority="-1" mode="M366"/><axsl:template match="@*|node()" priority="-2" mode="M366"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M366"/></axsl:template>

<!--PATTERN aepitemaccount_management-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicy_item/win-sc:account_management" priority="1000" mode="M367">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the account_management entity of an auditeventpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M367"/></axsl:template><axsl:template match="text()" priority="-1" mode="M367"/><axsl:template match="@*|node()" priority="-2" mode="M367"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M367"/></axsl:template>

<!--PATTERN aepitemdetailed_tracking-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicy_item/win-sc:detailed_tracking" priority="1000" mode="M368">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the detailed_tracking entity of an auditeventpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M368"/></axsl:template><axsl:template match="text()" priority="-1" mode="M368"/><axsl:template match="@*|node()" priority="-2" mode="M368"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M368"/></axsl:template>

<!--PATTERN aepitemdirectory_service_access-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicy_item/win-sc:directory_service_access" priority="1000" mode="M369">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_service_access entity of an auditeventpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M369"/></axsl:template><axsl:template match="text()" priority="-1" mode="M369"/><axsl:template match="@*|node()" priority="-2" mode="M369"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M369"/></axsl:template>

<!--PATTERN aepitemlogon-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicy_item/win-sc:logon" priority="1000" mode="M370">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the logon entity of an auditeventpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M370"/></axsl:template><axsl:template match="text()" priority="-1" mode="M370"/><axsl:template match="@*|node()" priority="-2" mode="M370"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M370"/></axsl:template>

<!--PATTERN aepitemobject_access-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicy_item/win-sc:object_access" priority="1000" mode="M371">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the object_access entity of an auditeventpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M371"/></axsl:template><axsl:template match="text()" priority="-1" mode="M371"/><axsl:template match="@*|node()" priority="-2" mode="M371"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M371"/></axsl:template>

<!--PATTERN aepitempolicy_change-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicy_item/win-sc:policy_change" priority="1000" mode="M372">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the policy_change entity of an auditeventpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M372"/></axsl:template><axsl:template match="text()" priority="-1" mode="M372"/><axsl:template match="@*|node()" priority="-2" mode="M372"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M372"/></axsl:template>

<!--PATTERN aepitemprivilege_use-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicy_item/win-sc:privilege_use" priority="1000" mode="M373">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the privilege_use entity of an auditeventpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M373"/></axsl:template><axsl:template match="text()" priority="-1" mode="M373"/><axsl:template match="@*|node()" priority="-2" mode="M373"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M373"/></axsl:template>

<!--PATTERN aepitemsystem-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicy_item/win-sc:system" priority="1000" mode="M374">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the system entity of an auditeventpolicy_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M374"/></axsl:template><axsl:template match="text()" priority="-1" mode="M374"/><axsl:template match="@*|node()" priority="-2" mode="M374"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M374"/></axsl:template>

<!--PATTERN aepsitemcredentialvalidation-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:credential_validation" priority="1000" mode="M375">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the credential_validation entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M375"/></axsl:template><axsl:template match="text()" priority="-1" mode="M375"/><axsl:template match="@*|node()" priority="-2" mode="M375"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M375"/></axsl:template>

<!--PATTERN aepsitemkerberosticketevents-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:kerberos_ticket_events" priority="1000" mode="M376">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the kerberos_ticket_events entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M376"/></axsl:template><axsl:template match="text()" priority="-1" mode="M376"/><axsl:template match="@*|node()" priority="-2" mode="M376"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M376"/></axsl:template>

<!--PATTERN aepsitemotheraccountlogonevents-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:other_account_logon_events" priority="1000" mode="M377">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_account_logon_events entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M377"/></axsl:template><axsl:template match="text()" priority="-1" mode="M377"/><axsl:template match="@*|node()" priority="-2" mode="M377"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M377"/></axsl:template>

<!--PATTERN aepsitemapplicationgroupmanagement-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:application_group_management" priority="1000" mode="M378">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the application_group_management entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M378"/></axsl:template><axsl:template match="text()" priority="-1" mode="M378"/><axsl:template match="@*|node()" priority="-2" mode="M378"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M378"/></axsl:template>

<!--PATTERN aepsitemcomputeraccountmanagement-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:computer_account_management" priority="1000" mode="M379">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the computer_account_management entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M379"/></axsl:template><axsl:template match="text()" priority="-1" mode="M379"/><axsl:template match="@*|node()" priority="-2" mode="M379"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M379"/></axsl:template>

<!--PATTERN aepsitemdistributiongroupmanagement-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:distribution_group_management" priority="1000" mode="M380">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the distribution_group_management entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M380"/></axsl:template><axsl:template match="text()" priority="-1" mode="M380"/><axsl:template match="@*|node()" priority="-2" mode="M380"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M380"/></axsl:template>

<!--PATTERN aepsitemotheraccountmanagementevents-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:other_account_management_events" priority="1000" mode="M381">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_account_management_events entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M381"/></axsl:template><axsl:template match="text()" priority="-1" mode="M381"/><axsl:template match="@*|node()" priority="-2" mode="M381"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M381"/></axsl:template>

<!--PATTERN aepsitemsecuritygroupmanagement-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:security_group_management" priority="1000" mode="M382">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the security_group_management entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M382"/></axsl:template><axsl:template match="text()" priority="-1" mode="M382"/><axsl:template match="@*|node()" priority="-2" mode="M382"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M382"/></axsl:template>

<!--PATTERN aepsitemuseraccountmanagement-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:user_account_management" priority="1000" mode="M383">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_account_management entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M383"/></axsl:template><axsl:template match="text()" priority="-1" mode="M383"/><axsl:template match="@*|node()" priority="-2" mode="M383"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M383"/></axsl:template>

<!--PATTERN aepsitemdpapiactivity-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:dpapi_activity" priority="1000" mode="M384">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the dpapi_activity entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M384"/></axsl:template><axsl:template match="text()" priority="-1" mode="M384"/><axsl:template match="@*|node()" priority="-2" mode="M384"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M384"/></axsl:template>

<!--PATTERN aepsitemprocesscreation-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:process_creation" priority="1000" mode="M385">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the process_creation entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M385"/></axsl:template><axsl:template match="text()" priority="-1" mode="M385"/><axsl:template match="@*|node()" priority="-2" mode="M385"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M385"/></axsl:template>

<!--PATTERN aepsitemprocesstermination-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:process_termination" priority="1000" mode="M386">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the process_termination entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M386"/></axsl:template><axsl:template match="text()" priority="-1" mode="M386"/><axsl:template match="@*|node()" priority="-2" mode="M386"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M386"/></axsl:template>

<!--PATTERN aepsitemrpcevents-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:rpc_events" priority="1000" mode="M387">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the rpc_events entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M387"/></axsl:template><axsl:template match="text()" priority="-1" mode="M387"/><axsl:template match="@*|node()" priority="-2" mode="M387"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M387"/></axsl:template>

<!--PATTERN aepsitemdirectoryserviceaccess-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:directory_service_access" priority="1000" mode="M388">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_service_access entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M388"/></axsl:template><axsl:template match="text()" priority="-1" mode="M388"/><axsl:template match="@*|node()" priority="-2" mode="M388"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M388"/></axsl:template>

<!--PATTERN aepsitemdirectoryservicechanges-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:xxx" priority="1000" mode="M389">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_service_changes entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M389"/></axsl:template><axsl:template match="text()" priority="-1" mode="M389"/><axsl:template match="@*|node()" priority="-2" mode="M389"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M389"/></axsl:template>

<!--PATTERN aepsitemdirectoryservicereplication-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:directory_service_replication" priority="1000" mode="M390">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the directory_service_replication entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M390"/></axsl:template><axsl:template match="text()" priority="-1" mode="M390"/><axsl:template match="@*|node()" priority="-2" mode="M390"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M390"/></axsl:template>

<!--PATTERN aepsitemdetaileddirectoryservicereplication-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:detailed_directory_service_replication" priority="1000" mode="M391">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the detailed_directory_service_replication entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M391"/></axsl:template><axsl:template match="text()" priority="-1" mode="M391"/><axsl:template match="@*|node()" priority="-2" mode="M391"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M391"/></axsl:template>

<!--PATTERN aepsitemaccountlockout-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:account_lockout" priority="1000" mode="M392">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the account_lockout entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M392"/></axsl:template><axsl:template match="text()" priority="-1" mode="M392"/><axsl:template match="@*|node()" priority="-2" mode="M392"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M392"/></axsl:template>

<!--PATTERN aepsitemipsecextendedmode-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:ipsec_extended_mode" priority="1000" mode="M393">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ipsec_extended_mode entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M393"/></axsl:template><axsl:template match="text()" priority="-1" mode="M393"/><axsl:template match="@*|node()" priority="-2" mode="M393"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M393"/></axsl:template>

<!--PATTERN aepsitemipsecmainmode-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:ipsec_main_mode" priority="1000" mode="M394">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ipsec_main_mode entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M394"/></axsl:template><axsl:template match="text()" priority="-1" mode="M394"/><axsl:template match="@*|node()" priority="-2" mode="M394"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M394"/></axsl:template>

<!--PATTERN aepsitemipsecquickmode-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:ipsec_quick_mode" priority="1000" mode="M395">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ipsec_quick_mode entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M395"/></axsl:template><axsl:template match="text()" priority="-1" mode="M395"/><axsl:template match="@*|node()" priority="-2" mode="M395"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M395"/></axsl:template>

<!--PATTERN aepsitemlogoff-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:logoff" priority="1000" mode="M396">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the logoff entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M396"/></axsl:template><axsl:template match="text()" priority="-1" mode="M396"/><axsl:template match="@*|node()" priority="-2" mode="M396"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M396"/></axsl:template>

<!--PATTERN aepsitemlogon-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:logon" priority="1000" mode="M397">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the logon entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M397"/></axsl:template><axsl:template match="text()" priority="-1" mode="M397"/><axsl:template match="@*|node()" priority="-2" mode="M397"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M397"/></axsl:template>

<!--PATTERN aepsitemotherlogonlogoffevents-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:other_logon_logoff_events" priority="1000" mode="M398">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_logon_logoff_events entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M398"/></axsl:template><axsl:template match="text()" priority="-1" mode="M398"/><axsl:template match="@*|node()" priority="-2" mode="M398"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M398"/></axsl:template>

<!--PATTERN aepsitemspeciallogon-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:special_logon" priority="1000" mode="M399">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the special_logon entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M399"/></axsl:template><axsl:template match="text()" priority="-1" mode="M399"/><axsl:template match="@*|node()" priority="-2" mode="M399"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M399"/></axsl:template>

<!--PATTERN aepsitemapplicationgenerated-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:application_generated" priority="1000" mode="M400">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the application_generated entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M400"/></axsl:template><axsl:template match="text()" priority="-1" mode="M400"/><axsl:template match="@*|node()" priority="-2" mode="M400"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M400"/></axsl:template>

<!--PATTERN aepsitemcertificationservices-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:certification_services" priority="1000" mode="M401">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the certification_services entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M401"/></axsl:template><axsl:template match="text()" priority="-1" mode="M401"/><axsl:template match="@*|node()" priority="-2" mode="M401"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M401"/></axsl:template>

<!--PATTERN aepsitemfileshare-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:file_share" priority="1000" mode="M402">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_share entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M402"/></axsl:template><axsl:template match="text()" priority="-1" mode="M402"/><axsl:template match="@*|node()" priority="-2" mode="M402"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M402"/></axsl:template>

<!--PATTERN aepsitemfilesystem-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:file_system" priority="1000" mode="M403">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_system entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M403"/></axsl:template><axsl:template match="text()" priority="-1" mode="M403"/><axsl:template match="@*|node()" priority="-2" mode="M403"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M403"/></axsl:template>

<!--PATTERN aepsitemfilteringplatformconnection-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:filtering_platform_connection" priority="1000" mode="M404">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filtering_platform_connection entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M404"/></axsl:template><axsl:template match="text()" priority="-1" mode="M404"/><axsl:template match="@*|node()" priority="-2" mode="M404"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M404"/></axsl:template>

<!--PATTERN aepsitemfilteringplatformpacketdrop-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:filtering_platform_packet_drop" priority="1000" mode="M405">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filtering_platform_packet_drop entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M405"/></axsl:template><axsl:template match="text()" priority="-1" mode="M405"/><axsl:template match="@*|node()" priority="-2" mode="M405"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M405"/></axsl:template>

<!--PATTERN aepsitemhandlemanipulation-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:handle_manipulation" priority="1000" mode="M406">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the handle_manipulation entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M406"/></axsl:template><axsl:template match="text()" priority="-1" mode="M406"/><axsl:template match="@*|node()" priority="-2" mode="M406"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M406"/></axsl:template>

<!--PATTERN aepsitemkernelobject-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:kernel_object" priority="1000" mode="M407">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the kernel_object entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M407"/></axsl:template><axsl:template match="text()" priority="-1" mode="M407"/><axsl:template match="@*|node()" priority="-2" mode="M407"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M407"/></axsl:template>

<!--PATTERN aepsitemotherobjectaccessevents-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:other_object_access_events" priority="1000" mode="M408">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_object_access_events entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M408"/></axsl:template><axsl:template match="text()" priority="-1" mode="M408"/><axsl:template match="@*|node()" priority="-2" mode="M408"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M408"/></axsl:template>

<!--PATTERN aepsitemregistry-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:registry" priority="1000" mode="M409">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the registry entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M409"/></axsl:template><axsl:template match="text()" priority="-1" mode="M409"/><axsl:template match="@*|node()" priority="-2" mode="M409"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M409"/></axsl:template>

<!--PATTERN aepsitemsam-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:sam" priority="1000" mode="M410">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sam entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M410"/></axsl:template><axsl:template match="text()" priority="-1" mode="M410"/><axsl:template match="@*|node()" priority="-2" mode="M410"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M410"/></axsl:template>

<!--PATTERN aepsitemauditpolicychange-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:audit_policy_change" priority="1000" mode="M411">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the audit_policy_change entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M411"/></axsl:template><axsl:template match="text()" priority="-1" mode="M411"/><axsl:template match="@*|node()" priority="-2" mode="M411"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M411"/></axsl:template>

<!--PATTERN aepsitemauthenticationpolicychange-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:authentication_policy_change" priority="1000" mode="M412">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the authentication_policy_change entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M412"/></axsl:template><axsl:template match="text()" priority="-1" mode="M412"/><axsl:template match="@*|node()" priority="-2" mode="M412"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M412"/></axsl:template>

<!--PATTERN aepsitemauthorizationpolicychange-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:authorization_policy_change" priority="1000" mode="M413">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the authorization_policy_change entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M413"/></axsl:template><axsl:template match="text()" priority="-1" mode="M413"/><axsl:template match="@*|node()" priority="-2" mode="M413"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M413"/></axsl:template>

<!--PATTERN aepsitemfilteringplatformpolicychange-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:filtering_platform_policy_change" priority="1000" mode="M414">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filtering_platform_policy_change entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M414"/></axsl:template><axsl:template match="text()" priority="-1" mode="M414"/><axsl:template match="@*|node()" priority="-2" mode="M414"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M414"/></axsl:template>

<!--PATTERN aepsitemmpssvcrulelevelpolicychange-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:mpssvc_rule_level_policy_change" priority="1000" mode="M415">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the mpssvc_rule_level_policy_change entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M415"/></axsl:template><axsl:template match="text()" priority="-1" mode="M415"/><axsl:template match="@*|node()" priority="-2" mode="M415"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M415"/></axsl:template>

<!--PATTERN aepsitemotherpolicychangeevents-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:other_policy_change_events" priority="1000" mode="M416">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_policy_change_events entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M416"/></axsl:template><axsl:template match="text()" priority="-1" mode="M416"/><axsl:template match="@*|node()" priority="-2" mode="M416"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M416"/></axsl:template>

<!--PATTERN aepsitemnonsensitiveprivilegeuse-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:non_sensitive_privilege_use" priority="1000" mode="M417">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the non_sensitive_privilege_use entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M417"/></axsl:template><axsl:template match="text()" priority="-1" mode="M417"/><axsl:template match="@*|node()" priority="-2" mode="M417"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M417"/></axsl:template>

<!--PATTERN aepsitemotherprivilegeuseevents-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:other_privilege_use_events" priority="1000" mode="M418">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_privilege_use_events entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M418"/></axsl:template><axsl:template match="text()" priority="-1" mode="M418"/><axsl:template match="@*|node()" priority="-2" mode="M418"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M418"/></axsl:template>

<!--PATTERN aepsitemsensitiveprivilegeuse-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:sensitive_privilege_use" priority="1000" mode="M419">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the sensitive_privilege_use entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M419"/></axsl:template><axsl:template match="text()" priority="-1" mode="M419"/><axsl:template match="@*|node()" priority="-2" mode="M419"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M419"/></axsl:template>

<!--PATTERN aepsitemipsecdriver-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:ipsec_driver" priority="1000" mode="M420">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ipsec_driver entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M420"/></axsl:template><axsl:template match="text()" priority="-1" mode="M420"/><axsl:template match="@*|node()" priority="-2" mode="M420"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M420"/></axsl:template>

<!--PATTERN aepsitemothersystemevents-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:other_system_events" priority="1000" mode="M421">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the other_system_events entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M421"/></axsl:template><axsl:template match="text()" priority="-1" mode="M421"/><axsl:template match="@*|node()" priority="-2" mode="M421"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M421"/></axsl:template>

<!--PATTERN aepsitemsecuritystatechange-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:security_state_change" priority="1000" mode="M422">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the security_state_change entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M422"/></axsl:template><axsl:template match="text()" priority="-1" mode="M422"/><axsl:template match="@*|node()" priority="-2" mode="M422"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M422"/></axsl:template>

<!--PATTERN aepsitemsecuritysystemextension-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:security_system_extension" priority="1000" mode="M423">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the security_system_extension entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M423"/></axsl:template><axsl:template match="text()" priority="-1" mode="M423"/><axsl:template match="@*|node()" priority="-2" mode="M423"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M423"/></axsl:template>

<!--PATTERN aepsitemsystemintegrity-->


	<!--RULE -->
<axsl:template match="win-sc:auditeventpolicysubcategories_item/win-sc:system_integrity" priority="1000" mode="M424">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the system_integrity entity of an auditeventpolicysubcategories_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M424"/></axsl:template><axsl:template match="text()" priority="-1" mode="M424"/><axsl:template match="@*|node()" priority="-2" mode="M424"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M424"/></axsl:template>

<!--PATTERN fileitempath-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:path" priority="1000" mode="M425">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M425"/></axsl:template><axsl:template match="text()" priority="-1" mode="M425"/><axsl:template match="@*|node()" priority="-2" mode="M425"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M425"/></axsl:template>

<!--PATTERN fileitemfilename-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:filename" priority="1000" mode="M426">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M426"/></axsl:template><axsl:template match="text()" priority="-1" mode="M426"/><axsl:template match="@*|node()" priority="-2" mode="M426"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M426"/></axsl:template>

<!--PATTERN fileitemowner-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:owner" priority="1000" mode="M427">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the owner entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M427"/></axsl:template><axsl:template match="text()" priority="-1" mode="M427"/><axsl:template match="@*|node()" priority="-2" mode="M427"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M427"/></axsl:template>

<!--PATTERN fileitemsize-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:size" priority="1000" mode="M428">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the size entity of a file_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M428"/></axsl:template><axsl:template match="text()" priority="-1" mode="M428"/><axsl:template match="@*|node()" priority="-2" mode="M428"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M428"/></axsl:template>

<!--PATTERN fileitema_time-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:a_time" priority="1000" mode="M429">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the a_time entity of a file_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M429"/></axsl:template><axsl:template match="text()" priority="-1" mode="M429"/><axsl:template match="@*|node()" priority="-2" mode="M429"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M429"/></axsl:template>

<!--PATTERN fileitemc_time-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:c_time" priority="1000" mode="M430">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the c_time entity of a file_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M430"/></axsl:template><axsl:template match="text()" priority="-1" mode="M430"/><axsl:template match="@*|node()" priority="-2" mode="M430"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M430"/></axsl:template>

<!--PATTERN fileitemm_time-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:m_time" priority="1000" mode="M431">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the m_time entity of a file_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M431"/></axsl:template><axsl:template match="text()" priority="-1" mode="M431"/><axsl:template match="@*|node()" priority="-2" mode="M431"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M431"/></axsl:template>

<!--PATTERN fileitemms_checksum-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:ms_checksum" priority="1000" mode="M432">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ms_checksum entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M432"/></axsl:template><axsl:template match="text()" priority="-1" mode="M432"/><axsl:template match="@*|node()" priority="-2" mode="M432"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M432"/></axsl:template>

<!--PATTERN fileitemversion-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:version" priority="1000" mode="M433">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='version'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the version entity of a file_item should be 'version'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M433"/></axsl:template><axsl:template match="text()" priority="-1" mode="M433"/><axsl:template match="@*|node()" priority="-2" mode="M433"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M433"/></axsl:template>

<!--PATTERN fileitemtype-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:type" priority="1000" mode="M434">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M434"/></axsl:template><axsl:template match="text()" priority="-1" mode="M434"/><axsl:template match="@*|node()" priority="-2" mode="M434"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M434"/></axsl:template>

<!--PATTERN fileitemdevelopment_class-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:development_class" priority="1000" mode="M435">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the development_class entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M435"/></axsl:template><axsl:template match="text()" priority="-1" mode="M435"/><axsl:template match="@*|node()" priority="-2" mode="M435"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M435"/></axsl:template>

<!--PATTERN fileitemcompany-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:company" priority="1000" mode="M436">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the company entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M436"/></axsl:template><axsl:template match="text()" priority="-1" mode="M436"/><axsl:template match="@*|node()" priority="-2" mode="M436"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M436"/></axsl:template>

<!--PATTERN fileiteminternalname-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:internal_name" priority="1000" mode="M437">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the internal_name entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M437"/></axsl:template><axsl:template match="text()" priority="-1" mode="M437"/><axsl:template match="@*|node()" priority="-2" mode="M437"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M437"/></axsl:template>

<!--PATTERN fileitemlanguage-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:language" priority="1000" mode="M438">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the language entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M438"/></axsl:template><axsl:template match="text()" priority="-1" mode="M438"/><axsl:template match="@*|node()" priority="-2" mode="M438"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M438"/></axsl:template>

<!--PATTERN fileitemoriginalfilename-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:original_filename" priority="1000" mode="M439">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the original_filename entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M439"/></axsl:template><axsl:template match="text()" priority="-1" mode="M439"/><axsl:template match="@*|node()" priority="-2" mode="M439"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M439"/></axsl:template>

<!--PATTERN fileitemproductname-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:product_name" priority="1000" mode="M440">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the product_name entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M440"/></axsl:template><axsl:template match="text()" priority="-1" mode="M440"/><axsl:template match="@*|node()" priority="-2" mode="M440"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M440"/></axsl:template>

<!--PATTERN fileitemproductversion-->


	<!--RULE -->
<axsl:template match="win-sc:file_item/win-sc:product_version" priority="1000" mode="M441">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the product_version entity of a file_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M441"/></axsl:template><axsl:template match="text()" priority="-1" mode="M441"/><axsl:template match="@*|node()" priority="-2" mode="M441"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M441"/></axsl:template>

<!--PATTERN fileaudititempath-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:path" priority="1000" mode="M442">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M442"/></axsl:template><axsl:template match="text()" priority="-1" mode="M442"/><axsl:template match="@*|node()" priority="-2" mode="M442"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M442"/></axsl:template>

<!--PATTERN fileaudititemfilename-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:filename" priority="1000" mode="M443">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M443"/></axsl:template><axsl:template match="text()" priority="-1" mode="M443"/><axsl:template match="@*|node()" priority="-2" mode="M443"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M443"/></axsl:template>

<!--PATTERN fileaudititemtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:trustee_sid" priority="1000" mode="M444">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M444"/></axsl:template><axsl:template match="text()" priority="-1" mode="M444"/><axsl:template match="@*|node()" priority="-2" mode="M444"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M444"/></axsl:template>

<!--PATTERN fileaudititemtrustee_name-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:trustee_name" priority="1000" mode="M445">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M445"/></axsl:template><axsl:template match="text()" priority="-1" mode="M445"/><axsl:template match="@*|node()" priority="-2" mode="M445"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M445"/></axsl:template>

<!--PATTERN fileaudititemstandard_delete-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:standard_delete" priority="1000" mode="M446">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M446"/></axsl:template><axsl:template match="text()" priority="-1" mode="M446"/><axsl:template match="@*|node()" priority="-2" mode="M446"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M446"/></axsl:template>

<!--PATTERN fileaudititemstandard_read_control-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:standard_read_control" priority="1000" mode="M447">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M447"/></axsl:template><axsl:template match="text()" priority="-1" mode="M447"/><axsl:template match="@*|node()" priority="-2" mode="M447"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M447"/></axsl:template>

<!--PATTERN fileaudititemstandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:standard_write_dac" priority="1000" mode="M448">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M448"/></axsl:template><axsl:template match="text()" priority="-1" mode="M448"/><axsl:template match="@*|node()" priority="-2" mode="M448"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M448"/></axsl:template>

<!--PATTERN fileaudititemstandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:standard_write_owner" priority="1000" mode="M449">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M449"/></axsl:template><axsl:template match="text()" priority="-1" mode="M449"/><axsl:template match="@*|node()" priority="-2" mode="M449"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M449"/></axsl:template>

<!--PATTERN fileaudititemstandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:standard_synchronize" priority="1000" mode="M450">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for thestandard_synchronize entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M450"/></axsl:template><axsl:template match="text()" priority="-1" mode="M450"/><axsl:template match="@*|node()" priority="-2" mode="M450"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M450"/></axsl:template>

<!--PATTERN fileaudititemaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:access_system_security" priority="1000" mode="M451">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M451"/></axsl:template><axsl:template match="text()" priority="-1" mode="M451"/><axsl:template match="@*|node()" priority="-2" mode="M451"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M451"/></axsl:template>

<!--PATTERN fileaudititemgeneric_read-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:generic_read" priority="1000" mode="M452">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M452"/></axsl:template><axsl:template match="text()" priority="-1" mode="M452"/><axsl:template match="@*|node()" priority="-2" mode="M452"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M452"/></axsl:template>

<!--PATTERN fileaudititemgeneric_write-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:generic_write" priority="1000" mode="M453">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M453"/></axsl:template><axsl:template match="text()" priority="-1" mode="M453"/><axsl:template match="@*|node()" priority="-2" mode="M453"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M453"/></axsl:template>

<!--PATTERN fileaudititemgeneric_execute-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:generic_execute" priority="1000" mode="M454">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M454"/></axsl:template><axsl:template match="text()" priority="-1" mode="M454"/><axsl:template match="@*|node()" priority="-2" mode="M454"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M454"/></axsl:template>

<!--PATTERN fileaudititemgeneric_all-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:generic_all" priority="1000" mode="M455">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M455"/></axsl:template><axsl:template match="text()" priority="-1" mode="M455"/><axsl:template match="@*|node()" priority="-2" mode="M455"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M455"/></axsl:template>

<!--PATTERN fileaudititemfile_read_data-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:file_read_data" priority="1000" mode="M456">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_data entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M456"/></axsl:template><axsl:template match="text()" priority="-1" mode="M456"/><axsl:template match="@*|node()" priority="-2" mode="M456"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M456"/></axsl:template>

<!--PATTERN fileaudititemfile_write_data-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:file_write_data" priority="1000" mode="M457">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_data entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M457"/></axsl:template><axsl:template match="text()" priority="-1" mode="M457"/><axsl:template match="@*|node()" priority="-2" mode="M457"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M457"/></axsl:template>

<!--PATTERN fileaudititemfile_append_data-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:file_append_data" priority="1000" mode="M458">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_append_data entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M458"/></axsl:template><axsl:template match="text()" priority="-1" mode="M458"/><axsl:template match="@*|node()" priority="-2" mode="M458"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M458"/></axsl:template>

<!--PATTERN fileaudititemfile_read_ea-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:file_read_ea" priority="1000" mode="M459">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_ea entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M459"/></axsl:template><axsl:template match="text()" priority="-1" mode="M459"/><axsl:template match="@*|node()" priority="-2" mode="M459"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M459"/></axsl:template>

<!--PATTERN fileaudititemfile_write_ea-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:file_write_ea" priority="1000" mode="M460">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_ea entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M460"/></axsl:template><axsl:template match="text()" priority="-1" mode="M460"/><axsl:template match="@*|node()" priority="-2" mode="M460"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M460"/></axsl:template>

<!--PATTERN fileaudititemfile_execute-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:file_execute" priority="1000" mode="M461">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_execute entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M461"/></axsl:template><axsl:template match="text()" priority="-1" mode="M461"/><axsl:template match="@*|node()" priority="-2" mode="M461"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M461"/></axsl:template>

<!--PATTERN fileaudititemfile_delete_child-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:file_delete_child" priority="1000" mode="M462">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_delete_child entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M462"/></axsl:template><axsl:template match="text()" priority="-1" mode="M462"/><axsl:template match="@*|node()" priority="-2" mode="M462"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M462"/></axsl:template>

<!--PATTERN fileaudititemfile_read_attributes-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:file_read_attributes" priority="1000" mode="M463">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_attributes entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M463"/></axsl:template><axsl:template match="text()" priority="-1" mode="M463"/><axsl:template match="@*|node()" priority="-2" mode="M463"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M463"/></axsl:template>

<!--PATTERN fileaudititemfile_write_attributes-->


	<!--RULE -->
<axsl:template match="win-sc:fileauditedpermissions_item/win-sc:file_write_attributes" priority="1000" mode="M464">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_attributes entity of a fileauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M464"/></axsl:template><axsl:template match="text()" priority="-1" mode="M464"/><axsl:template match="@*|node()" priority="-2" mode="M464"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M464"/></axsl:template>

<!--PATTERN feritempath-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:path" priority="1000" mode="M465">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the path entity of a fileeffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M465"/></axsl:template><axsl:template match="text()" priority="-1" mode="M465"/><axsl:template match="@*|node()" priority="-2" mode="M465"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M465"/></axsl:template>

<!--PATTERN feritemfilename-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:filename" priority="1000" mode="M466">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the filename entity of a fileeffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M466"/></axsl:template><axsl:template match="text()" priority="-1" mode="M466"/><axsl:template match="@*|node()" priority="-2" mode="M466"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M466"/></axsl:template>

<!--PATTERN feritemtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:trustee_sid" priority="1000" mode="M467">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a fileeffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M467"/></axsl:template><axsl:template match="text()" priority="-1" mode="M467"/><axsl:template match="@*|node()" priority="-2" mode="M467"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M467"/></axsl:template>

<!--PATTERN feritemtrustee_name-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:trustee_name" priority="1000" mode="M468">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a fileeffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M468"/></axsl:template><axsl:template match="text()" priority="-1" mode="M468"/><axsl:template match="@*|node()" priority="-2" mode="M468"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M468"/></axsl:template>

<!--PATTERN feritemstandard_delete-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:standard_delete" priority="1000" mode="M469">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M469"/></axsl:template><axsl:template match="text()" priority="-1" mode="M469"/><axsl:template match="@*|node()" priority="-2" mode="M469"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M469"/></axsl:template>

<!--PATTERN feritemstandard_read_control-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:standard_read_control" priority="1000" mode="M470">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M470"/></axsl:template><axsl:template match="text()" priority="-1" mode="M470"/><axsl:template match="@*|node()" priority="-2" mode="M470"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M470"/></axsl:template>

<!--PATTERN feritemstandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:standard_write_dac" priority="1000" mode="M471">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M471"/></axsl:template><axsl:template match="text()" priority="-1" mode="M471"/><axsl:template match="@*|node()" priority="-2" mode="M471"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M471"/></axsl:template>

<!--PATTERN feritemstandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:standard_write_owner" priority="1000" mode="M472">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M472"/></axsl:template><axsl:template match="text()" priority="-1" mode="M472"/><axsl:template match="@*|node()" priority="-2" mode="M472"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M472"/></axsl:template>

<!--PATTERN feritemstandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:standard_synchronize" priority="1000" mode="M473">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M473"/></axsl:template><axsl:template match="text()" priority="-1" mode="M473"/><axsl:template match="@*|node()" priority="-2" mode="M473"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M473"/></axsl:template>

<!--PATTERN feritemaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:access_system_security" priority="1000" mode="M474">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M474"/></axsl:template><axsl:template match="text()" priority="-1" mode="M474"/><axsl:template match="@*|node()" priority="-2" mode="M474"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M474"/></axsl:template>

<!--PATTERN feritemgeneric_read-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:generic_read" priority="1000" mode="M475">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M475"/></axsl:template><axsl:template match="text()" priority="-1" mode="M475"/><axsl:template match="@*|node()" priority="-2" mode="M475"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M475"/></axsl:template>

<!--PATTERN feritemgeneric_write-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:generic_write" priority="1000" mode="M476">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M476"/></axsl:template><axsl:template match="text()" priority="-1" mode="M476"/><axsl:template match="@*|node()" priority="-2" mode="M476"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M476"/></axsl:template>

<!--PATTERN feritemgeneric_execute-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:generic_execute" priority="1000" mode="M477">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M477"/></axsl:template><axsl:template match="text()" priority="-1" mode="M477"/><axsl:template match="@*|node()" priority="-2" mode="M477"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M477"/></axsl:template>

<!--PATTERN feritemgeneric_all-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:generic_all" priority="1000" mode="M478">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M478"/></axsl:template><axsl:template match="text()" priority="-1" mode="M478"/><axsl:template match="@*|node()" priority="-2" mode="M478"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M478"/></axsl:template>

<!--PATTERN feritemfile_read_data-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:file_read_data" priority="1000" mode="M479">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_data entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M479"/></axsl:template><axsl:template match="text()" priority="-1" mode="M479"/><axsl:template match="@*|node()" priority="-2" mode="M479"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M479"/></axsl:template>

<!--PATTERN feritemfile_write_data-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:file_write_data" priority="1000" mode="M480">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_data entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M480"/></axsl:template><axsl:template match="text()" priority="-1" mode="M480"/><axsl:template match="@*|node()" priority="-2" mode="M480"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M480"/></axsl:template>

<!--PATTERN feritemfile_append_data-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:file_append_data" priority="1000" mode="M481">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_append_data entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M481"/></axsl:template><axsl:template match="text()" priority="-1" mode="M481"/><axsl:template match="@*|node()" priority="-2" mode="M481"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M481"/></axsl:template>

<!--PATTERN feritemfile_read_ea-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:file_read_ea" priority="1000" mode="M482">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_ea entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M482"/></axsl:template><axsl:template match="text()" priority="-1" mode="M482"/><axsl:template match="@*|node()" priority="-2" mode="M482"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M482"/></axsl:template>

<!--PATTERN feritemfile_write_ea-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:file_write_ea" priority="1000" mode="M483">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_ea entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M483"/></axsl:template><axsl:template match="text()" priority="-1" mode="M483"/><axsl:template match="@*|node()" priority="-2" mode="M483"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M483"/></axsl:template>

<!--PATTERN feritemfile_execute-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:file_execute" priority="1000" mode="M484">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_execute entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M484"/></axsl:template><axsl:template match="text()" priority="-1" mode="M484"/><axsl:template match="@*|node()" priority="-2" mode="M484"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M484"/></axsl:template>

<!--PATTERN feritemfile_delete_child-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:file_delete_child" priority="1000" mode="M485">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_delete_child entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M485"/></axsl:template><axsl:template match="text()" priority="-1" mode="M485"/><axsl:template match="@*|node()" priority="-2" mode="M485"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M485"/></axsl:template>

<!--PATTERN feritemfile_read_attributes-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:file_read_attributes" priority="1000" mode="M486">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_attributes entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M486"/></axsl:template><axsl:template match="text()" priority="-1" mode="M486"/><axsl:template match="@*|node()" priority="-2" mode="M486"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M486"/></axsl:template>

<!--PATTERN feritemfile_write_attributes-->


	<!--RULE -->
<axsl:template match="win-sc:fileeffectiverights_item/win-sc:file_write_attributes" priority="1000" mode="M487">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_write_attributes entity of a fileeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M487"/></axsl:template><axsl:template match="text()" priority="-1" mode="M487"/><axsl:template match="@*|node()" priority="-2" mode="M487"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M487"/></axsl:template>

<!--PATTERN groupitemgroup-->


	<!--RULE -->
<axsl:template match="win-sc:group_item/win-sc:group" priority="1000" mode="M488">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group entity of a group_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M488"/></axsl:template><axsl:template match="text()" priority="-1" mode="M488"/><axsl:template match="@*|node()" priority="-2" mode="M488"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M488"/></axsl:template>

<!--PATTERN groupitemuser-->


	<!--RULE -->
<axsl:template match="win-sc:group_item/win-sc:user" priority="1000" mode="M489">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user entity of a group_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M489"/></axsl:template><axsl:template match="text()" priority="-1" mode="M489"/><axsl:template match="@*|node()" priority="-2" mode="M489"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M489"/></axsl:template>

<!--PATTERN groupsiditemgroup-->


	<!--RULE -->
<axsl:template match="win-sc:group_sid_item/win-sc:group_sid" priority="1000" mode="M490">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the group_sid entity of a group_sid_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M490"/></axsl:template><axsl:template match="text()" priority="-1" mode="M490"/><axsl:template match="@*|node()" priority="-2" mode="M490"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M490"/></axsl:template>

<!--PATTERN groupsiditemuser-->


	<!--RULE -->
<axsl:template match="win-sc:group_sid_item/win-sc:user_sid" priority="1000" mode="M491">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_sid entity of a group_sid_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M491"/></axsl:template><axsl:template match="text()" priority="-1" mode="M491"/><axsl:template match="@*|node()" priority="-2" mode="M491"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M491"/></axsl:template>

<!--PATTERN wininterfaceitemname-->


	<!--RULE -->
<axsl:template match="win-sc:interface_item/win-sc:name" priority="1000" mode="M492">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M492"/></axsl:template><axsl:template match="text()" priority="-1" mode="M492"/><axsl:template match="@*|node()" priority="-2" mode="M492"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M492"/></axsl:template>

<!--PATTERN wininterfaceitemindex-->


	<!--RULE -->
<axsl:template match="win-sc:interface_item/win-sc:index" priority="1000" mode="M493">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the index entity of an interface_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M493"/></axsl:template><axsl:template match="text()" priority="-1" mode="M493"/><axsl:template match="@*|node()" priority="-2" mode="M493"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M493"/></axsl:template>

<!--PATTERN wininterfaceitemtype-->


	<!--RULE -->
<axsl:template match="win-sc:interface_item/win-sc:type" priority="1000" mode="M494">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M494"/></axsl:template><axsl:template match="text()" priority="-1" mode="M494"/><axsl:template match="@*|node()" priority="-2" mode="M494"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M494"/></axsl:template>

<!--PATTERN wininterfaceitemhardware_addr-->


	<!--RULE -->
<axsl:template match="win-sc:interface_item/win-sc:hardware_addr" priority="1000" mode="M495">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hardware_addr entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M495"/></axsl:template><axsl:template match="text()" priority="-1" mode="M495"/><axsl:template match="@*|node()" priority="-2" mode="M495"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M495"/></axsl:template>

<!--PATTERN wininterfaceiteminet_addr-->


	<!--RULE -->
<axsl:template match="win-sc:interface_item/win-sc:inet_addr" priority="1000" mode="M496">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the inet_addr entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M496"/></axsl:template><axsl:template match="text()" priority="-1" mode="M496"/><axsl:template match="@*|node()" priority="-2" mode="M496"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M496"/></axsl:template>

<!--PATTERN wininterfaceitembroadcast_addr-->


	<!--RULE -->
<axsl:template match="win-sc:interface_item/win-sc:broadcast_addr" priority="1000" mode="M497">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the broadcast_addr entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M497"/></axsl:template><axsl:template match="text()" priority="-1" mode="M497"/><axsl:template match="@*|node()" priority="-2" mode="M497"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M497"/></axsl:template>

<!--PATTERN wininterfaceitemnetmask-->


	<!--RULE -->
<axsl:template match="win-sc:interface_item/win-sc:netmask" priority="1000" mode="M498">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the netmask entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M498"/></axsl:template><axsl:template match="text()" priority="-1" mode="M498"/><axsl:template match="@*|node()" priority="-2" mode="M498"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M498"/></axsl:template>

<!--PATTERN wininterfaceitemaddr_type-->


	<!--RULE -->
<axsl:template match="win-sc:interface_item/win-sc:addr_type" priority="1000" mode="M499">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the addr_type entity of an interface_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M499"/></axsl:template><axsl:template match="text()" priority="-1" mode="M499"/><axsl:template match="@*|node()" priority="-2" mode="M499"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M499"/></axsl:template>

<!--PATTERN lpitemforce_logoff-->


	<!--RULE -->
<axsl:template match="win-sc:lockoutpolicy_item/win-sc:force_logoff" priority="1000" mode="M500">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the force_logoff entity of a lockoutpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M500"/></axsl:template><axsl:template match="text()" priority="-1" mode="M500"/><axsl:template match="@*|node()" priority="-2" mode="M500"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M500"/></axsl:template>

<!--PATTERN lpitemlockout_duration-->


	<!--RULE -->
<axsl:template match="win-sc:lockoutpolicy_item/win-sc:lockout_duration" priority="1000" mode="M501">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the lockout_duration entity of a lockoutpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M501"/></axsl:template><axsl:template match="text()" priority="-1" mode="M501"/><axsl:template match="@*|node()" priority="-2" mode="M501"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M501"/></axsl:template>

<!--PATTERN lpitemlockout_observation_window-->


	<!--RULE -->
<axsl:template match="win-sc:lockoutpolicy_item/win-sc:lockout_observation_window" priority="1000" mode="M502">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the lockout_observation_window entity of a lockoutpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M502"/></axsl:template><axsl:template match="text()" priority="-1" mode="M502"/><axsl:template match="@*|node()" priority="-2" mode="M502"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M502"/></axsl:template>

<!--PATTERN lpitemlockout_threshold-->


	<!--RULE -->
<axsl:template match="win-sc:lockoutpolicy_item/win-sc:lockout_threshold" priority="1000" mode="M503">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the lockout_threshold entity of a lockoutpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M503"/></axsl:template><axsl:template match="text()" priority="-1" mode="M503"/><axsl:template match="@*|node()" priority="-2" mode="M503"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M503"/></axsl:template>

<!--PATTERN metabaseitemkey-->


	<!--RULE -->
<axsl:template match="win-sc:metabase_item/win-sc:key" priority="1000" mode="M504">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a metabase_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M504"/></axsl:template><axsl:template match="text()" priority="-1" mode="M504"/><axsl:template match="@*|node()" priority="-2" mode="M504"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M504"/></axsl:template>

<!--PATTERN metabaseitemid-->


	<!--RULE -->
<axsl:template match="win-sc:metabase_item/win-sc:id" priority="1000" mode="M505">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the id entity of a metabase_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M505"/></axsl:template><axsl:template match="text()" priority="-1" mode="M505"/><axsl:template match="@*|node()" priority="-2" mode="M505"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M505"/></axsl:template>

<!--PATTERN metabaseitemname-->


	<!--RULE -->
<axsl:template match="win-sc:metabase_item/win-sc:name" priority="1000" mode="M506">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a metabase_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M506"/></axsl:template><axsl:template match="text()" priority="-1" mode="M506"/><axsl:template match="@*|node()" priority="-2" mode="M506"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M506"/></axsl:template>

<!--PATTERN metabaseitemuser_type-->


	<!--RULE -->
<axsl:template match="win-sc:metabase_item/win-sc:user_type" priority="1000" mode="M507">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_type entity of a metabase_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M507"/></axsl:template><axsl:template match="text()" priority="-1" mode="M507"/><axsl:template match="@*|node()" priority="-2" mode="M507"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M507"/></axsl:template>

<!--PATTERN metabaseitemdata_type-->


	<!--RULE -->
<axsl:template match="win-sc:metabase_item/win-sc:data_type" priority="1000" mode="M508">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the data_type entity of a metabase_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M508"/></axsl:template><axsl:template match="text()" priority="-1" mode="M508"/><axsl:template match="@*|node()" priority="-2" mode="M508"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M508"/></axsl:template>

<!--PATTERN metabaseitemdata-->


	<!--RULE -->
<axsl:template match="win-sc:metabase_item/win-sc:data" priority="1000" mode="M509">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M509"/></axsl:template><axsl:template match="text()" priority="-1" mode="M509"/><axsl:template match="@*|node()" priority="-2" mode="M509"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M509"/></axsl:template>

<!--PATTERN ppitemmax_passwd_age-->


	<!--RULE -->
<axsl:template match="win-sc:passwordpolicy_item/win-sc:max_passwd_age" priority="1000" mode="M510">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the max_passwd_age entity of a passwordpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M510"/></axsl:template><axsl:template match="text()" priority="-1" mode="M510"/><axsl:template match="@*|node()" priority="-2" mode="M510"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M510"/></axsl:template>

<!--PATTERN ppitemmin_passwd_age-->


	<!--RULE -->
<axsl:template match="win-sc:passwordpolicy_item/win-sc:min_passwd_age" priority="1000" mode="M511">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the min_passwd_age entity of a passwordpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M511"/></axsl:template><axsl:template match="text()" priority="-1" mode="M511"/><axsl:template match="@*|node()" priority="-2" mode="M511"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M511"/></axsl:template>

<!--PATTERN ppitemmin_passwd_len-->


	<!--RULE -->
<axsl:template match="win-sc:passwordpolicy_item/win-sc:min_passwd_len" priority="1000" mode="M512">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the min_passwd_len entity of a passwordpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M512"/></axsl:template><axsl:template match="text()" priority="-1" mode="M512"/><axsl:template match="@*|node()" priority="-2" mode="M512"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M512"/></axsl:template>

<!--PATTERN ppitempassword_hist_len-->


	<!--RULE -->
<axsl:template match="win-sc:passwordpolicy_item/win-sc:password_hist_len" priority="1000" mode="M513">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password_hist_len entity of a passwordpolicy_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M513"/></axsl:template><axsl:template match="text()" priority="-1" mode="M513"/><axsl:template match="@*|node()" priority="-2" mode="M513"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M513"/></axsl:template>

<!--PATTERN ppitempassword_complexity-->


	<!--RULE -->
<axsl:template match="win-sc:passwordpolicy_item/win-sc:password_complexity" priority="1000" mode="M514">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the password_complexity entity of a passwordpolicy_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M514"/></axsl:template><axsl:template match="text()" priority="-1" mode="M514"/><axsl:template match="@*|node()" priority="-2" mode="M514"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M514"/></axsl:template>

<!--PATTERN ppitemreversible_encryption-->


	<!--RULE -->
<axsl:template match="win-sc:passwordpolicy_item/win-sc:reversible_encryption" priority="1000" mode="M515">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the reversible_encryption entity of a passwordpolicy_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M515"/></axsl:template><axsl:template match="text()" priority="-1" mode="M515"/><axsl:template match="@*|node()" priority="-2" mode="M515"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M515"/></axsl:template>

<!--PATTERN winportitemlocal_address-->


	<!--RULE -->
<axsl:template match="win-sc:port_item/win-sc:local_address" priority="1000" mode="M516">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_address entity of a port_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M516"/></axsl:template><axsl:template match="text()" priority="-1" mode="M516"/><axsl:template match="@*|node()" priority="-2" mode="M516"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M516"/></axsl:template>

<!--PATTERN winportitemlocal_port-->


	<!--RULE -->
<axsl:template match="win-sc:port_item/win-sc:local_port" priority="1000" mode="M517">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_port entity of a port_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M517"/></axsl:template><axsl:template match="text()" priority="-1" mode="M517"/><axsl:template match="@*|node()" priority="-2" mode="M517"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M517"/></axsl:template>

<!--PATTERN winportitemprotocol-->


	<!--RULE -->
<axsl:template match="win-sc:port_item/win-sc:protocol" priority="1000" mode="M518">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the protocol entity of a port_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M518"/></axsl:template><axsl:template match="text()" priority="-1" mode="M518"/><axsl:template match="@*|node()" priority="-2" mode="M518"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M518"/></axsl:template>

<!--PATTERN winportitempid-->


	<!--RULE -->
<axsl:template match="win-sc:port_item/win-sc:pid" priority="1000" mode="M519">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of a port_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M519"/></axsl:template><axsl:template match="text()" priority="-1" mode="M519"/><axsl:template match="@*|node()" priority="-2" mode="M519"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M519"/></axsl:template>

<!--PATTERN peritemprinter_name-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:printer_name" priority="1000" mode="M520">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the printer_name entity of a printereffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M520"/></axsl:template><axsl:template match="text()" priority="-1" mode="M520"/><axsl:template match="@*|node()" priority="-2" mode="M520"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M520"/></axsl:template>

<!--PATTERN peritemtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:trustee_sid" priority="1000" mode="M521">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a printereffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M521"/></axsl:template><axsl:template match="text()" priority="-1" mode="M521"/><axsl:template match="@*|node()" priority="-2" mode="M521"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M521"/></axsl:template>

<!--PATTERN peritemstandard_delete-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:standard_delete" priority="1000" mode="M522">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M522"/></axsl:template><axsl:template match="text()" priority="-1" mode="M522"/><axsl:template match="@*|node()" priority="-2" mode="M522"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M522"/></axsl:template>

<!--PATTERN peritemstandard_read_control-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:standard_read_control" priority="1000" mode="M523">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M523"/></axsl:template><axsl:template match="text()" priority="-1" mode="M523"/><axsl:template match="@*|node()" priority="-2" mode="M523"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M523"/></axsl:template>

<!--PATTERN peritemstandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:standard_write_dac" priority="1000" mode="M524">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M524"/></axsl:template><axsl:template match="text()" priority="-1" mode="M524"/><axsl:template match="@*|node()" priority="-2" mode="M524"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M524"/></axsl:template>

<!--PATTERN peritemstandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:standard_write_owner" priority="1000" mode="M525">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M525"/></axsl:template><axsl:template match="text()" priority="-1" mode="M525"/><axsl:template match="@*|node()" priority="-2" mode="M525"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M525"/></axsl:template>

<!--PATTERN peritemstandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:standard_synchronize" priority="1000" mode="M526">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M526"/></axsl:template><axsl:template match="text()" priority="-1" mode="M526"/><axsl:template match="@*|node()" priority="-2" mode="M526"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M526"/></axsl:template>

<!--PATTERN peritemaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:access_system_security" priority="1000" mode="M527">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M527"/></axsl:template><axsl:template match="text()" priority="-1" mode="M527"/><axsl:template match="@*|node()" priority="-2" mode="M527"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M527"/></axsl:template>

<!--PATTERN peritemgeneric_read-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:generic_read" priority="1000" mode="M528">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M528"/></axsl:template><axsl:template match="text()" priority="-1" mode="M528"/><axsl:template match="@*|node()" priority="-2" mode="M528"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M528"/></axsl:template>

<!--PATTERN peritemgeneric_write-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:generic_write" priority="1000" mode="M529">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M529"/></axsl:template><axsl:template match="text()" priority="-1" mode="M529"/><axsl:template match="@*|node()" priority="-2" mode="M529"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M529"/></axsl:template>

<!--PATTERN peritemgeneric_execute-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:generic_execute" priority="1000" mode="M530">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M530"/></axsl:template><axsl:template match="text()" priority="-1" mode="M530"/><axsl:template match="@*|node()" priority="-2" mode="M530"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M530"/></axsl:template>

<!--PATTERN peritemgeneric_all-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:generic_all" priority="1000" mode="M531">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M531"/></axsl:template><axsl:template match="text()" priority="-1" mode="M531"/><axsl:template match="@*|node()" priority="-2" mode="M531"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M531"/></axsl:template>

<!--PATTERN peritemprinter_access_administer-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:printer_access_administer" priority="1000" mode="M532">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the printer_access_administer entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M532"/></axsl:template><axsl:template match="text()" priority="-1" mode="M532"/><axsl:template match="@*|node()" priority="-2" mode="M532"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M532"/></axsl:template>

<!--PATTERN peritemprinter_access_use-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:printer_access_use" priority="1000" mode="M533">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the printer_access_use entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M533"/></axsl:template><axsl:template match="text()" priority="-1" mode="M533"/><axsl:template match="@*|node()" priority="-2" mode="M533"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M533"/></axsl:template>

<!--PATTERN peritemjob_access_administer-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:job_access_administer" priority="1000" mode="M534">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the job_access_administer entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M534"/></axsl:template><axsl:template match="text()" priority="-1" mode="M534"/><axsl:template match="@*|node()" priority="-2" mode="M534"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M534"/></axsl:template>

<!--PATTERN peritemjob_access_read-->


	<!--RULE -->
<axsl:template match="win-sc:printereffectiverights_item/win-sc:job_access_read" priority="1000" mode="M535">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the job_access_read entity of a printereffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M535"/></axsl:template><axsl:template match="text()" priority="-1" mode="M535"/><axsl:template match="@*|node()" priority="-2" mode="M535"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M535"/></axsl:template>

<!--PATTERN processitemcommand_line-->


	<!--RULE -->
<axsl:template match="win-sc:process_item/win-sc:command_line" priority="1000" mode="M536">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the command_line entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M536"/></axsl:template><axsl:template match="text()" priority="-1" mode="M536"/><axsl:template match="@*|node()" priority="-2" mode="M536"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M536"/></axsl:template>

<!--PATTERN processitempid-->


	<!--RULE -->
<axsl:template match="win-sc:process_item/win-sc:pid" priority="1000" mode="M537">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the pid entity of a process_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M537"/></axsl:template><axsl:template match="text()" priority="-1" mode="M537"/><axsl:template match="@*|node()" priority="-2" mode="M537"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M537"/></axsl:template>

<!--PATTERN processitemppid-->


	<!--RULE -->
<axsl:template match="win-sc:process_item/win-sc:ppid" priority="1000" mode="M538">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the ppid entity of a process_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M538"/></axsl:template><axsl:template match="text()" priority="-1" mode="M538"/><axsl:template match="@*|node()" priority="-2" mode="M538"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M538"/></axsl:template>

<!--PATTERN processitempriority-->


	<!--RULE -->
<axsl:template match="win-sc:process_item/win-sc:priority" priority="1000" mode="M539">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the priority entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M539"/></axsl:template><axsl:template match="text()" priority="-1" mode="M539"/><axsl:template match="@*|node()" priority="-2" mode="M539"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M539"/></axsl:template>

<!--PATTERN processitemimage_path-->


	<!--RULE -->
<axsl:template match="win-sc:process_item/win-sc:image_path" priority="1000" mode="M540">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the image_path entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M540"/></axsl:template><axsl:template match="text()" priority="-1" mode="M540"/><axsl:template match="@*|node()" priority="-2" mode="M540"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M540"/></axsl:template>

<!--PATTERN processitemcurrent_dir-->


	<!--RULE -->
<axsl:template match="win-sc:process_item/win-sc:current_dir" priority="1000" mode="M541">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the current_dir entity of a process_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M541"/></axsl:template><axsl:template match="text()" priority="-1" mode="M541"/><axsl:template match="@*|node()" priority="-2" mode="M541"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M541"/></axsl:template>

<!--PATTERN regitemhive-->


	<!--RULE -->
<axsl:template match="win-sc:registry_item/win-sc:hive" priority="1000" mode="M542">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a registry_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M542"/></axsl:template><axsl:template match="text()" priority="-1" mode="M542"/><axsl:template match="@*|node()" priority="-2" mode="M542"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M542"/></axsl:template>

<!--PATTERN regitemkey-->


	<!--RULE -->
<axsl:template match="win-sc:registry_item/win-sc:key" priority="1000" mode="M543">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a registry_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose>

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@xsi:nil='true') or ../win-sc:name/@xsi:nil='true'"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - name entity must be nil when key is nil<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M543"/></axsl:template><axsl:template match="text()" priority="-1" mode="M543"/><axsl:template match="@*|node()" priority="-2" mode="M543"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M543"/></axsl:template>

<!--PATTERN regitemname-->


	<!--RULE -->
<axsl:template match="win-sc:registry_item/win-sc:name" priority="1000" mode="M544">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a registry_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M544"/></axsl:template><axsl:template match="text()" priority="-1" mode="M544"/><axsl:template match="@*|node()" priority="-2" mode="M544"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M544"/></axsl:template>

<!--PATTERN regitemtype-->


	<!--RULE -->
<axsl:template match="win-sc:registry_item/win-sc:type" priority="1000" mode="M545">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the type entity of a registry_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M545"/></axsl:template><axsl:template match="text()" priority="-1" mode="M545"/><axsl:template match="@*|node()" priority="-2" mode="M545"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M545"/></axsl:template>

<!--PATTERN regitemvalue-->


	<!--RULE -->
<axsl:template match="win-sc:registry_item/win-sc:value" priority="1000" mode="M546">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M546"/></axsl:template><axsl:template match="text()" priority="-1" mode="M546"/><axsl:template match="@*|node()" priority="-2" mode="M546"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M546"/></axsl:template>

<!--PATTERN rapitemhive-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:hive" priority="1000" mode="M547">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M547"/></axsl:template><axsl:template match="text()" priority="-1" mode="M547"/><axsl:template match="@*|node()" priority="-2" mode="M547"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M547"/></axsl:template>

<!--PATTERN rapitemkey-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key" priority="1000" mode="M548">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M548"/></axsl:template><axsl:template match="text()" priority="-1" mode="M548"/><axsl:template match="@*|node()" priority="-2" mode="M548"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M548"/></axsl:template>

<!--PATTERN rapitemtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:trustee_sid" priority="1000" mode="M549">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M549"/></axsl:template><axsl:template match="text()" priority="-1" mode="M549"/><axsl:template match="@*|node()" priority="-2" mode="M549"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M549"/></axsl:template>

<!--PATTERN rapitemtrustee_name-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:trustee_name" priority="1000" mode="M550">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M550"/></axsl:template><axsl:template match="text()" priority="-1" mode="M550"/><axsl:template match="@*|node()" priority="-2" mode="M550"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M550"/></axsl:template>

<!--PATTERN rapitemstandard_delete-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:standard_delete" priority="1000" mode="M551">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M551"/></axsl:template><axsl:template match="text()" priority="-1" mode="M551"/><axsl:template match="@*|node()" priority="-2" mode="M551"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M551"/></axsl:template>

<!--PATTERN rapitemstandard_read_control-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:standard_read_control" priority="1000" mode="M552">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M552"/></axsl:template><axsl:template match="text()" priority="-1" mode="M552"/><axsl:template match="@*|node()" priority="-2" mode="M552"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M552"/></axsl:template>

<!--PATTERN rapitemstandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:standard_write_dac" priority="1000" mode="M553">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M553"/></axsl:template><axsl:template match="text()" priority="-1" mode="M553"/><axsl:template match="@*|node()" priority="-2" mode="M553"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M553"/></axsl:template>

<!--PATTERN rapitemstandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:standard_write_owner" priority="1000" mode="M554">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M554"/></axsl:template><axsl:template match="text()" priority="-1" mode="M554"/><axsl:template match="@*|node()" priority="-2" mode="M554"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M554"/></axsl:template>

<!--PATTERN rapitemstandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:standard_synchronize" priority="1000" mode="M555">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M555"/></axsl:template><axsl:template match="text()" priority="-1" mode="M555"/><axsl:template match="@*|node()" priority="-2" mode="M555"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M555"/></axsl:template>

<!--PATTERN rapitemaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:access_system_security" priority="1000" mode="M556">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M556"/></axsl:template><axsl:template match="text()" priority="-1" mode="M556"/><axsl:template match="@*|node()" priority="-2" mode="M556"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M556"/></axsl:template>

<!--PATTERN rapitemgeneric_read-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:generic_read" priority="1000" mode="M557">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M557"/></axsl:template><axsl:template match="text()" priority="-1" mode="M557"/><axsl:template match="@*|node()" priority="-2" mode="M557"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M557"/></axsl:template>

<!--PATTERN rapitemgeneric_write-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:generic_write" priority="1000" mode="M558">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M558"/></axsl:template><axsl:template match="text()" priority="-1" mode="M558"/><axsl:template match="@*|node()" priority="-2" mode="M558"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M558"/></axsl:template>

<!--PATTERN rapitemgeneric_execute-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:generic_execute" priority="1000" mode="M559">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M559"/></axsl:template><axsl:template match="text()" priority="-1" mode="M559"/><axsl:template match="@*|node()" priority="-2" mode="M559"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M559"/></axsl:template>

<!--PATTERN rapitemgeneric_all-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:generic_all" priority="1000" mode="M560">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M560"/></axsl:template><axsl:template match="text()" priority="-1" mode="M560"/><axsl:template match="@*|node()" priority="-2" mode="M560"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M560"/></axsl:template>

<!--PATTERN rapitemkey_query_value-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key_query_value" priority="1000" mode="M561">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_query_value entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M561"/></axsl:template><axsl:template match="text()" priority="-1" mode="M561"/><axsl:template match="@*|node()" priority="-2" mode="M561"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M561"/></axsl:template>

<!--PATTERN rapitemkey_set_value-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key_set_value" priority="1000" mode="M562">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_set_value entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M562"/></axsl:template><axsl:template match="text()" priority="-1" mode="M562"/><axsl:template match="@*|node()" priority="-2" mode="M562"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M562"/></axsl:template>

<!--PATTERN rapitemkey_create_sub_key-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key_create_sub_key" priority="1000" mode="M563">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M563"/></axsl:template><axsl:template match="text()" priority="-1" mode="M563"/><axsl:template match="@*|node()" priority="-2" mode="M563"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M563"/></axsl:template>

<!--PATTERN rapitemkey_enumerate_sub_keys-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key_enumerate_sub_keys" priority="1000" mode="M564">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M564"/></axsl:template><axsl:template match="text()" priority="-1" mode="M564"/><axsl:template match="@*|node()" priority="-2" mode="M564"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M564"/></axsl:template>

<!--PATTERN rapitemkey_notify-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key_notify" priority="1000" mode="M565">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_notify entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M565"/></axsl:template><axsl:template match="text()" priority="-1" mode="M565"/><axsl:template match="@*|node()" priority="-2" mode="M565"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M565"/></axsl:template>

<!--PATTERN rapitemkey_create_link-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key_create_link" priority="1000" mode="M566">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_link entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M566"/></axsl:template><axsl:template match="text()" priority="-1" mode="M566"/><axsl:template match="@*|node()" priority="-2" mode="M566"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M566"/></axsl:template>

<!--PATTERN rapitemkey_wow64_64key-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key_wow64_64key" priority="1000" mode="M567">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M567"/></axsl:template><axsl:template match="text()" priority="-1" mode="M567"/><axsl:template match="@*|node()" priority="-2" mode="M567"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M567"/></axsl:template>

<!--PATTERN rapitemkey_wow64_32key-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key_wow64_32key" priority="1000" mode="M568">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M568"/></axsl:template><axsl:template match="text()" priority="-1" mode="M568"/><axsl:template match="@*|node()" priority="-2" mode="M568"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M568"/></axsl:template>

<!--PATTERN rapitemkey_wow64_res-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyauditedpermissions_item/win-sc:key_wow64_res" priority="1000" mode="M569">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyauditedpermissions_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M569"/></axsl:template><axsl:template match="text()" priority="-1" mode="M569"/><axsl:template match="@*|node()" priority="-2" mode="M569"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M569"/></axsl:template>

<!--PATTERN reritemhive-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:hive" priority="1000" mode="M570">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the hive entity of a regkeyeffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M570"/></axsl:template><axsl:template match="text()" priority="-1" mode="M570"/><axsl:template match="@*|node()" priority="-2" mode="M570"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M570"/></axsl:template>

<!--PATTERN reritemkey-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key" priority="1000" mode="M571">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key entity of a regkeyeffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M571"/></axsl:template><axsl:template match="text()" priority="-1" mode="M571"/><axsl:template match="@*|node()" priority="-2" mode="M571"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M571"/></axsl:template>

<!--PATTERN reritemtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:trustee_sid" priority="1000" mode="M572">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a regkeyeffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M572"/></axsl:template><axsl:template match="text()" priority="-1" mode="M572"/><axsl:template match="@*|node()" priority="-2" mode="M572"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M572"/></axsl:template>

<!--PATTERN reritemtrustee_name-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:trustee_name" priority="1000" mode="M573">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a regkeyeffectiverights_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M573"/></axsl:template><axsl:template match="text()" priority="-1" mode="M573"/><axsl:template match="@*|node()" priority="-2" mode="M573"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M573"/></axsl:template>

<!--PATTERN reritemstandard_delete-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:standard_delete" priority="1000" mode="M574">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_delete entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M574"/></axsl:template><axsl:template match="text()" priority="-1" mode="M574"/><axsl:template match="@*|node()" priority="-2" mode="M574"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M574"/></axsl:template>

<!--PATTERN reritemstandard_read_control-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:standard_read_control" priority="1000" mode="M575">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_read_control entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M575"/></axsl:template><axsl:template match="text()" priority="-1" mode="M575"/><axsl:template match="@*|node()" priority="-2" mode="M575"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M575"/></axsl:template>

<!--PATTERN reritemstandard_write_dac-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:standard_write_dac" priority="1000" mode="M576">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M576"/></axsl:template><axsl:template match="text()" priority="-1" mode="M576"/><axsl:template match="@*|node()" priority="-2" mode="M576"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M576"/></axsl:template>

<!--PATTERN reritemstandard_write_owner-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:standard_write_owner" priority="1000" mode="M577">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M577"/></axsl:template><axsl:template match="text()" priority="-1" mode="M577"/><axsl:template match="@*|node()" priority="-2" mode="M577"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M577"/></axsl:template>

<!--PATTERN reritemstandard_synchronize-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:standard_synchronize" priority="1000" mode="M578">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M578"/></axsl:template><axsl:template match="text()" priority="-1" mode="M578"/><axsl:template match="@*|node()" priority="-2" mode="M578"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M578"/></axsl:template>

<!--PATTERN reritemaccess_system_security-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:access_system_security" priority="1000" mode="M579">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_system_security entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M579"/></axsl:template><axsl:template match="text()" priority="-1" mode="M579"/><axsl:template match="@*|node()" priority="-2" mode="M579"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M579"/></axsl:template>

<!--PATTERN reritemgeneric_read-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:generic_read" priority="1000" mode="M580">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_read entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M580"/></axsl:template><axsl:template match="text()" priority="-1" mode="M580"/><axsl:template match="@*|node()" priority="-2" mode="M580"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M580"/></axsl:template>

<!--PATTERN reritemgeneric_write-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:generic_write" priority="1000" mode="M581">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_write entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M581"/></axsl:template><axsl:template match="text()" priority="-1" mode="M581"/><axsl:template match="@*|node()" priority="-2" mode="M581"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M581"/></axsl:template>

<!--PATTERN reritemgeneric_execute-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:generic_execute" priority="1000" mode="M582">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_execute entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M582"/></axsl:template><axsl:template match="text()" priority="-1" mode="M582"/><axsl:template match="@*|node()" priority="-2" mode="M582"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M582"/></axsl:template>

<!--PATTERN reritemgeneric_all-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:generic_all" priority="1000" mode="M583">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the generic_all entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M583"/></axsl:template><axsl:template match="text()" priority="-1" mode="M583"/><axsl:template match="@*|node()" priority="-2" mode="M583"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M583"/></axsl:template>

<!--PATTERN reritemkey_query_value-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key_query_value" priority="1000" mode="M584">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_query_value entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M584"/></axsl:template><axsl:template match="text()" priority="-1" mode="M584"/><axsl:template match="@*|node()" priority="-2" mode="M584"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M584"/></axsl:template>

<!--PATTERN reritemkey_set_value-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key_set_value" priority="1000" mode="M585">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_set_value entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M585"/></axsl:template><axsl:template match="text()" priority="-1" mode="M585"/><axsl:template match="@*|node()" priority="-2" mode="M585"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M585"/></axsl:template>

<!--PATTERN reritemkey_create_sub_key-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key_create_sub_key" priority="1000" mode="M586">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M586"/></axsl:template><axsl:template match="text()" priority="-1" mode="M586"/><axsl:template match="@*|node()" priority="-2" mode="M586"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M586"/></axsl:template>

<!--PATTERN reritemkey_enumerate_sub_keys-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key_enumerate_sub_keys" priority="1000" mode="M587">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M587"/></axsl:template><axsl:template match="text()" priority="-1" mode="M587"/><axsl:template match="@*|node()" priority="-2" mode="M587"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M587"/></axsl:template>

<!--PATTERN reritemkey_notify-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key_notify" priority="1000" mode="M588">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_notify entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M588"/></axsl:template><axsl:template match="text()" priority="-1" mode="M588"/><axsl:template match="@*|node()" priority="-2" mode="M588"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M588"/></axsl:template>

<!--PATTERN reritemkey_create_link-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key_create_link" priority="1000" mode="M589">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_create_link entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M589"/></axsl:template><axsl:template match="text()" priority="-1" mode="M589"/><axsl:template match="@*|node()" priority="-2" mode="M589"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M589"/></axsl:template>

<!--PATTERN reritemkey_wow64_64key-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key_wow64_64key" priority="1000" mode="M590">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M590"/></axsl:template><axsl:template match="text()" priority="-1" mode="M590"/><axsl:template match="@*|node()" priority="-2" mode="M590"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M590"/></axsl:template>

<!--PATTERN reritemkey_wow64_32key-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key_wow64_32key" priority="1000" mode="M591">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M591"/></axsl:template><axsl:template match="text()" priority="-1" mode="M591"/><axsl:template match="@*|node()" priority="-2" mode="M591"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M591"/></axsl:template>

<!--PATTERN reritemkey_wow64_res-->


	<!--RULE -->
<axsl:template match="win-sc:regkeyeffectiverights_item/win-sc:key_wow64_res" priority="1000" mode="M592">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyeffectiverights_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M592"/></axsl:template><axsl:template match="text()" priority="-1" mode="M592"/><axsl:template match="@*|node()" priority="-2" mode="M592"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M592"/></axsl:template>

<!--PATTERN sritemnetname-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:netname" priority="1000" mode="M593">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the netname entity of a sharedresource_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M593"/></axsl:template><axsl:template match="text()" priority="-1" mode="M593"/><axsl:template match="@*|node()" priority="-2" mode="M593"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M593"/></axsl:template>

<!--PATTERN sritemshared_type-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:shared_type" priority="1000" mode="M594">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the shared_type entity of a sharedresource_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M594"/></axsl:template><axsl:template match="text()" priority="-1" mode="M594"/><axsl:template match="@*|node()" priority="-2" mode="M594"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M594"/></axsl:template>

<!--PATTERN sritemmax_uses-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:max_uses" priority="1000" mode="M595">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the max_uses entity of a sharedresource_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M595"/></axsl:template><axsl:template match="text()" priority="-1" mode="M595"/><axsl:template match="@*|node()" priority="-2" mode="M595"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M595"/></axsl:template>

<!--PATTERN sritemcurrent_uses-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:current_uses" priority="1000" mode="M596">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the current_uses entity of a sharedresource_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M596"/></axsl:template><axsl:template match="text()" priority="-1" mode="M596"/><axsl:template match="@*|node()" priority="-2" mode="M596"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M596"/></axsl:template>

<!--PATTERN sritemlocal_path-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:local_path" priority="1000" mode="M597">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the local_path entity of a sharedresource_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M597"/></axsl:template><axsl:template match="text()" priority="-1" mode="M597"/><axsl:template match="@*|node()" priority="-2" mode="M597"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M597"/></axsl:template>

<!--PATTERN sritemaccess_read_permission-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:access_read_permission" priority="1000" mode="M598">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_read_permission entity of a sharedresource_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M598"/></axsl:template><axsl:template match="text()" priority="-1" mode="M598"/><axsl:template match="@*|node()" priority="-2" mode="M598"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M598"/></axsl:template>

<!--PATTERN sritemaccess_write_permission-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:access_write_permission" priority="1000" mode="M599">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_write_permission entity of a sharedresource_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M599"/></axsl:template><axsl:template match="text()" priority="-1" mode="M599"/><axsl:template match="@*|node()" priority="-2" mode="M599"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M599"/></axsl:template>

<!--PATTERN sritemaccess_create_permission-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:access_create_permission" priority="1000" mode="M600">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_create_permission entity of a sharedresource_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M600"/></axsl:template><axsl:template match="text()" priority="-1" mode="M600"/><axsl:template match="@*|node()" priority="-2" mode="M600"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M600"/></axsl:template>

<!--PATTERN sritemaccess_exec_permission-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:access_exec_permission" priority="1000" mode="M601">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_exec_permission entity of a sharedresource_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M601"/></axsl:template><axsl:template match="text()" priority="-1" mode="M601"/><axsl:template match="@*|node()" priority="-2" mode="M601"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M601"/></axsl:template>

<!--PATTERN sritemaccess_delete_permission-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:access_delete_permission" priority="1000" mode="M602">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_delete_permission entity of a sharedresource_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M602"/></axsl:template><axsl:template match="text()" priority="-1" mode="M602"/><axsl:template match="@*|node()" priority="-2" mode="M602"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M602"/></axsl:template>

<!--PATTERN sritemaccess_atrib_permission-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:access_atrib_permission" priority="1000" mode="M603">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_atrib_permission entity of a sharedresource_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M603"/></axsl:template><axsl:template match="text()" priority="-1" mode="M603"/><axsl:template match="@*|node()" priority="-2" mode="M603"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M603"/></axsl:template>

<!--PATTERN sritemaccess_perm_permission-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:access_perm_permission" priority="1000" mode="M604">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_perm_permission entity of a sharedresource_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M604"/></axsl:template><axsl:template match="text()" priority="-1" mode="M604"/><axsl:template match="@*|node()" priority="-2" mode="M604"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M604"/></axsl:template>

<!--PATTERN sritemaccess_all_permission-->


	<!--RULE -->
<axsl:template match="win-sc:sharedresource_item/win-sc:access_all_permission" priority="1000" mode="M605">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the access_all_permission entity of a sharedresource_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M605"/></axsl:template><axsl:template match="text()" priority="-1" mode="M605"/><axsl:template match="@*|node()" priority="-2" mode="M605"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M605"/></axsl:template>

<!--PATTERN siditemtrustee_name-->


	<!--RULE -->
<axsl:template match="win-sc:sid_item/win-sc:trustee_name" priority="1000" mode="M606">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_name entity of a sid_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M606"/></axsl:template><axsl:template match="text()" priority="-1" mode="M606"/><axsl:template match="@*|node()" priority="-2" mode="M606"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M606"/></axsl:template>

<!--PATTERN siditemtrustee_sid-->


	<!--RULE -->
<axsl:template match="win-sc:sid_item/win-sc:trustee_sid" priority="1000" mode="M607">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_sid entity of a sid_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M607"/></axsl:template><axsl:template match="text()" priority="-1" mode="M607"/><axsl:template match="@*|node()" priority="-2" mode="M607"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M607"/></axsl:template>

<!--PATTERN siditemtrustee_domain-->


	<!--RULE -->
<axsl:template match="win-sc:sid_item/win-sc:trustee_domain" priority="1000" mode="M608">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the trustee_domain entity of a sid_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M608"/></axsl:template><axsl:template match="text()" priority="-1" mode="M608"/><axsl:template match="@*|node()" priority="-2" mode="M608"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M608"/></axsl:template>

<!--PATTERN uacitemadminapprovalmode-->


	<!--RULE -->
<axsl:template match="win-sc:uac_item/win-sc:admin_approval_mode" priority="1000" mode="M609">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the admin_approval_mode entity of a uac_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M609"/></axsl:template><axsl:template match="text()" priority="-1" mode="M609"/><axsl:template match="@*|node()" priority="-2" mode="M609"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M609"/></axsl:template>

<!--PATTERN uacitemelevationpromptadmin-->


	<!--RULE -->
<axsl:template match="win-sc:uac_item/win-sc:elevation_prompt_admin" priority="1000" mode="M610">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the elevation_prompt_admin entity of a uac_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M610"/></axsl:template><axsl:template match="text()" priority="-1" mode="M610"/><axsl:template match="@*|node()" priority="-2" mode="M610"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M610"/></axsl:template>

<!--PATTERN uacitemelevationpromptstandard-->


	<!--RULE -->
<axsl:template match="win-sc:uac_item/win-sc:elevation_prompt_standard" priority="1000" mode="M611">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the elevation_prompt_standard entity of a uac_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M611"/></axsl:template><axsl:template match="text()" priority="-1" mode="M611"/><axsl:template match="@*|node()" priority="-2" mode="M611"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M611"/></axsl:template>

<!--PATTERN uacitemdetectinstallations-->


	<!--RULE -->
<axsl:template match="win-sc:uac_item/win-sc:detect_installations" priority="1000" mode="M612">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the detect_installations entity of a uac_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M612"/></axsl:template><axsl:template match="text()" priority="-1" mode="M612"/><axsl:template match="@*|node()" priority="-2" mode="M612"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M612"/></axsl:template>

<!--PATTERN uacitemelevatesignedexecutables-->


	<!--RULE -->
<axsl:template match="win-sc:uac_item/win-sc:elevate_signed_executables" priority="1000" mode="M613">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the elevate_signed_executables entity of a uac_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M613"/></axsl:template><axsl:template match="text()" priority="-1" mode="M613"/><axsl:template match="@*|node()" priority="-2" mode="M613"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M613"/></axsl:template>

<!--PATTERN uacitemelevateuiaccess-->


	<!--RULE -->
<axsl:template match="win-sc:uac_item/win-sc:elevate_uiaccess" priority="1000" mode="M614">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the elevate_uiaccess entity of a uac_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M614"/></axsl:template><axsl:template match="text()" priority="-1" mode="M614"/><axsl:template match="@*|node()" priority="-2" mode="M614"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M614"/></axsl:template>

<!--PATTERN uacitemrunadminsaam-->


	<!--RULE -->
<axsl:template match="win-sc:uac_item/win-sc:run_admins_aam" priority="1000" mode="M615">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the run_admins_aam entity of a uac_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M615"/></axsl:template><axsl:template match="text()" priority="-1" mode="M615"/><axsl:template match="@*|node()" priority="-2" mode="M615"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M615"/></axsl:template>

<!--PATTERN uacitemsecuredesktop-->


	<!--RULE -->
<axsl:template match="win-sc:uac_item/win-sc:secure_desktop" priority="1000" mode="M616">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the secure_desktop entity of a uac_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M616"/></axsl:template><axsl:template match="text()" priority="-1" mode="M616"/><axsl:template match="@*|node()" priority="-2" mode="M616"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M616"/></axsl:template>

<!--PATTERN uacitemvirtualizewritefailures-->


	<!--RULE -->
<axsl:template match="win-sc:uac_item/win-sc:virtualize_write_failures" priority="1000" mode="M617">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the virtualize_write_failures entity of a uac_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M617"/></axsl:template><axsl:template match="text()" priority="-1" mode="M617"/><axsl:template match="@*|node()" priority="-2" mode="M617"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M617"/></axsl:template>

<!--PATTERN useritemuser-->


	<!--RULE -->
<axsl:template match="win-sc:user_item/win-sc:user" priority="1000" mode="M618">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user entity of a user_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M618"/></axsl:template><axsl:template match="text()" priority="-1" mode="M618"/><axsl:template match="@*|node()" priority="-2" mode="M618"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M618"/></axsl:template>

<!--PATTERN useritemenabled-->


	<!--RULE -->
<axsl:template match="win-sc:user_item/win-sc:enabled" priority="1000" mode="M619">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the enabled entity of a user_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M619"/></axsl:template><axsl:template match="text()" priority="-1" mode="M619"/><axsl:template match="@*|node()" priority="-2" mode="M619"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M619"/></axsl:template>

<!--PATTERN useritemgroup-->


	<!--RULE -->
<axsl:template match="win-sc:user_item/win-sc:group" priority="1000" mode="M620">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for a group entity of a user_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M620"/></axsl:template><axsl:template match="text()" priority="-1" mode="M620"/><axsl:template match="@*|node()" priority="-2" mode="M620"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M620"/></axsl:template>

<!--PATTERN usersiditemuser-->


	<!--RULE -->
<axsl:template match="win-sc:user_sid_item/win-sc:user_sid" priority="1000" mode="M621">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the user_sid entity of a user_sid_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M621"/></axsl:template><axsl:template match="text()" priority="-1" mode="M621"/><axsl:template match="@*|node()" priority="-2" mode="M621"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M621"/></axsl:template>

<!--PATTERN usersiditemenabled-->


	<!--RULE -->
<axsl:template match="win-sc:user_sid_item/win-sc:enabled" priority="1000" mode="M622">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the enabled entity of a user_sid_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M622"/></axsl:template><axsl:template match="text()" priority="-1" mode="M622"/><axsl:template match="@*|node()" priority="-2" mode="M622"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M622"/></axsl:template>

<!--PATTERN usersiditemgroup-->


	<!--RULE -->
<axsl:template match="win-sc:user_sid_item/win-sc:group_sid" priority="1000" mode="M623">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for a group_sid entity of a user_sid_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M623"/></axsl:template><axsl:template match="text()" priority="-1" mode="M623"/><axsl:template match="@*|node()" priority="-2" mode="M623"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M623"/></axsl:template>

<!--PATTERN volitemrootpath-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:rootpath" priority="1000" mode="M624">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the rootpath entity of a volume_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M624"/></axsl:template><axsl:template match="text()" priority="-1" mode="M624"/><axsl:template match="@*|node()" priority="-2" mode="M624"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M624"/></axsl:template>

<!--PATTERN volitemfile_system-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_system" priority="1000" mode="M625">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_system entity of a volume_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M625"/></axsl:template><axsl:template match="text()" priority="-1" mode="M625"/><axsl:template match="@*|node()" priority="-2" mode="M625"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M625"/></axsl:template>

<!--PATTERN volitemname-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:name" priority="1000" mode="M626">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the name entity of a volume_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M626"/></axsl:template><axsl:template match="text()" priority="-1" mode="M626"/><axsl:template match="@*|node()" priority="-2" mode="M626"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M626"/></axsl:template>

<!--PATTERN volitemvolume_max_component_length-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:volume_max_component_length" priority="1000" mode="M627">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the volume_max_component_length entity of a volume_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M627"/></axsl:template><axsl:template match="text()" priority="-1" mode="M627"/><axsl:template match="@*|node()" priority="-2" mode="M627"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M627"/></axsl:template>

<!--PATTERN volitemserial_number-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:serial_number" priority="1000" mode="M628">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='int'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the serial_number entity of a volume_item should be 'int'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M628"/></axsl:template><axsl:template match="text()" priority="-1" mode="M628"/><axsl:template match="@*|node()" priority="-2" mode="M628"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M628"/></axsl:template>

<!--PATTERN volitemfile_case_sensitive_search-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_case_sensitive_search" priority="1000" mode="M629">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_case_sensitive_search entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M629"/></axsl:template><axsl:template match="text()" priority="-1" mode="M629"/><axsl:template match="@*|node()" priority="-2" mode="M629"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M629"/></axsl:template>

<!--PATTERN volitemfile_case_preserved_names-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_case_preserved_names" priority="1000" mode="M630">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_case_preserved_names entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M630"/></axsl:template><axsl:template match="text()" priority="-1" mode="M630"/><axsl:template match="@*|node()" priority="-2" mode="M630"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M630"/></axsl:template>

<!--PATTERN volitemfile_unicode_on_disk-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_unicode_on_disk" priority="1000" mode="M631">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_unicode_on_disk entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M631"/></axsl:template><axsl:template match="text()" priority="-1" mode="M631"/><axsl:template match="@*|node()" priority="-2" mode="M631"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M631"/></axsl:template>

<!--PATTERN volitemfile_persistent_acls-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_persistent_acls" priority="1000" mode="M632">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_persistent_acls entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M632"/></axsl:template><axsl:template match="text()" priority="-1" mode="M632"/><axsl:template match="@*|node()" priority="-2" mode="M632"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M632"/></axsl:template>

<!--PATTERN volitemfile_file_compression-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_file_compression" priority="1000" mode="M633">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_file_compression entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M633"/></axsl:template><axsl:template match="text()" priority="-1" mode="M633"/><axsl:template match="@*|node()" priority="-2" mode="M633"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M633"/></axsl:template>

<!--PATTERN volitemfile_volume_quotas-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_volume_quotas" priority="1000" mode="M634">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_volume_quotas entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M634"/></axsl:template><axsl:template match="text()" priority="-1" mode="M634"/><axsl:template match="@*|node()" priority="-2" mode="M634"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M634"/></axsl:template>

<!--PATTERN volitemfile_supports_sparse_files-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_supports_sparse_files" priority="1000" mode="M635">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_sparse_files entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M635"/></axsl:template><axsl:template match="text()" priority="-1" mode="M635"/><axsl:template match="@*|node()" priority="-2" mode="M635"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M635"/></axsl:template>

<!--PATTERN volitemfile_supports_reparse_points-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_supports_reparse_points" priority="1000" mode="M636">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_reparse_points entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M636"/></axsl:template><axsl:template match="text()" priority="-1" mode="M636"/><axsl:template match="@*|node()" priority="-2" mode="M636"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M636"/></axsl:template>

<!--PATTERN volitemfile_supports_remote_storage-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_supports_remote_storage" priority="1000" mode="M637">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_remote_storage entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M637"/></axsl:template><axsl:template match="text()" priority="-1" mode="M637"/><axsl:template match="@*|node()" priority="-2" mode="M637"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M637"/></axsl:template>

<!--PATTERN volitemfile_volume_is_compressed-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_volume_is_compressed" priority="1000" mode="M638">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_volume_is_compressed entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M638"/></axsl:template><axsl:template match="text()" priority="-1" mode="M638"/><axsl:template match="@*|node()" priority="-2" mode="M638"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M638"/></axsl:template>

<!--PATTERN volitemfile_supports_object_ids-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_supports_object_ids" priority="1000" mode="M639">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_object_ids entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M639"/></axsl:template><axsl:template match="text()" priority="-1" mode="M639"/><axsl:template match="@*|node()" priority="-2" mode="M639"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M639"/></axsl:template>

<!--PATTERN volitemfile_supports_encryption-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_supports_encryption" priority="1000" mode="M640">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_supports_encryption entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M640"/></axsl:template><axsl:template match="text()" priority="-1" mode="M640"/><axsl:template match="@*|node()" priority="-2" mode="M640"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M640"/></axsl:template>

<!--PATTERN volitemfile_named_streams-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_named_streams" priority="1000" mode="M641">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_named_streams entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M641"/></axsl:template><axsl:template match="text()" priority="-1" mode="M641"/><axsl:template match="@*|node()" priority="-2" mode="M641"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M641"/></axsl:template>

<!--PATTERN volitemfile_read_only_volume-->


	<!--RULE -->
<axsl:template match="win-sc:volume_item/win-sc:file_read_only_volume" priority="1000" mode="M642">

		<!--ASSERT -->
<axsl:choose><axsl:when test="@datatype='boolean'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the file_read_only_volume entity of a volume_item should be 'boolean'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M642"/></axsl:template><axsl:template match="text()" priority="-1" mode="M642"/><axsl:template match="@*|node()" priority="-2" mode="M642"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M642"/></axsl:template>

<!--PATTERN wmiitemnamespace-->


	<!--RULE -->
<axsl:template match="win-sc:wmi_item/win-sc:namespace" priority="1000" mode="M643">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the namespace entity of a wmi_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M643"/></axsl:template><axsl:template match="text()" priority="-1" mode="M643"/><axsl:template match="@*|node()" priority="-2" mode="M643"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M643"/></axsl:template>

<!--PATTERN wmiitemwql-->


	<!--RULE -->
<axsl:template match="win-sc:wmi_item/win-sc:wql" priority="1000" mode="M644">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the wql entity of a wmi_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M644"/></axsl:template><axsl:template match="text()" priority="-1" mode="M644"/><axsl:template match="@*|node()" priority="-2" mode="M644"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M644"/></axsl:template>

<!--PATTERN wmiitemresult-->


	<!--RULE -->
<axsl:template match="win-sc:wmi_item/win-sc:result" priority="1000" mode="M645">

		<!--ASSERT -->
<axsl:choose><axsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/><axsl:otherwise>
                <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - The datatype has been set to 'int' but the value is not an integer.<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M645"/></axsl:template><axsl:template match="text()" priority="-1" mode="M645"/><axsl:template match="@*|node()" priority="-2" mode="M645"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M645"/></axsl:template>

<!--PATTERN wuaupdatesearcheritemsearchcriteria-->


	<!--RULE -->
<axsl:template match="win-sc:wuaupdatesearcher_item/win-sc:search_criteria" priority="1000" mode="M646">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the search_criteria entity of a wuaupdatesearcher_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M646"/></axsl:template><axsl:template match="text()" priority="-1" mode="M646"/><axsl:template match="@*|node()" priority="-2" mode="M646"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M646"/></axsl:template>

<!--PATTERN wuaupdatesearcheritemresult-->


	<!--RULE -->
<axsl:template match="win-sc:wuaupdatesearcher_item/win-sc:update_id" priority="1000" mode="M647">

		<!--ASSERT -->
<axsl:choose><axsl:when test="not(@datatype) or @datatype='string'"/><axsl:otherwise>item <axsl:text/><axsl:value-of select="../@id"/><axsl:text/> - datatype attribute for the update_id entity of a wuaupdatesearcher_item should be 'string'<axsl:value-of select="string('&#10;')"/></axsl:otherwise></axsl:choose><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M647"/></axsl:template><axsl:template match="text()" priority="-1" mode="M647"/><axsl:template match="@*|node()" priority="-2" mode="M647"><axsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M647"/></axsl:template></axsl:stylesheet>
