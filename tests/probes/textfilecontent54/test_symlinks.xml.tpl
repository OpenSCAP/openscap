<?xml version="1.0"?>
<oval_definitions xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5" xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent" xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix" xmlns:lin-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5" xsi:schemaLocation="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix unix-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#independent independent-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5#linux linux-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd">
    <generator>
        <oval:schema_version>5.10.1</oval:schema_version>
        <oval:timestamp>0001-01-01T00:00:00+00:00</oval:timestamp>
    </generator>

    <definitions>
        <definition class="compliance" version="1" id="oval:x:def:1">
            <metadata>
                <title>x</title>
                <description>x</description>
                <affected family="unix">
                    <platform>x</platform>
                </affected>
            </metadata>
            <criteria comment="x">
                <criterion test_ref="oval:x:tst:1"/>
                <criterion test_ref="oval:x:tst:2"/>
                <criterion test_ref="oval:x:tst:3"/>
                <criterion test_ref="oval:x:tst:4"/>
            </criteria>
        </definition>
    </definitions>

    <tests>
        <textfilecontent54_test id="oval:x:tst:1" check="all" comment="x" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
            <object object_ref="oval:x:obj:1"/>
        </textfilecontent54_test>
        <textfilecontent54_test id="oval:x:tst:2" check="all" comment="x" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
            <object object_ref="oval:x:obj:2"/>
        </textfilecontent54_test>
        <textfilecontent54_test id="oval:x:tst:3" check="all" comment="x" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
            <object object_ref="oval:x:obj:3"/>
        </textfilecontent54_test>
        <textfilecontent54_test id="oval:x:tst:4" check="all" comment="x" version="1" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
            <object object_ref="oval:x:obj:4"/>
        </textfilecontent54_test>
    </tests>

    <objects>
        <textfilecontent54_object id="oval:x:obj:1" version="1" comment="x" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
            <path datatype="string" operation="equals">%PATH%</path>
            <filename datatype="string" operation="equals">sl1</filename>
            <pattern datatype="string" operation="pattern match">.*</pattern>
            <instance datatype="int" operation="equals">1</instance>
        </textfilecontent54_object>
        <textfilecontent54_object id="oval:x:obj:2" version="1" comment="x" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
            <path datatype="string" operation="equals">%PATH%</path>
            <filename datatype="string" operation="equals">sl2</filename>
            <pattern datatype="string" operation="pattern match">.*</pattern>
            <instance datatype="int" operation="equals">1</instance>
        </textfilecontent54_object>
        <textfilecontent54_object id="oval:x:obj:3" version="1" comment="x" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
            <path datatype="string" operation="equals">%PATH%</path>
            <filename datatype="string" operation="equals">sl3</filename>
            <pattern datatype="string" operation="pattern match">.*</pattern>
            <instance datatype="int" operation="equals">1</instance>
        </textfilecontent54_object>
        <textfilecontent54_object id="oval:x:obj:4" version="1" comment="x" xmlns="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent">
            <path datatype="string" operation="equals">/etc</path>
            <filename datatype="string" operation="equals">hosts</filename>
            <pattern datatype="string" operation="pattern match">.*</pattern>
            <instance datatype="int" operation="equals">1</instance>
        </textfilecontent54_object>
    </objects>
</oval_definitions>
