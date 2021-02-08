Description
	This test content was designed to test the requirement SCAP.R.1200 for CPE applicabily.
	It uses platform independent CPEs and it runs on any platform.
	When using the r1200-datastream.xml, the following test case are required to be performed:
		1. Test case (1.a, 1.b, 1.c, 1.d, and 1.e ) or (1.f, 1.g, 1.h, 1.i, and 1.j)
		2. Test case 2.a, 2.b, and 2.c
		3. Test case 3.a, and 3.b

	When using the r1200-na-datastream.xml, the tester shall select the following profile: xccdf_mil.disa.stig_profile_MAC-1_Classified
	
Content Status
	Complete

Testing Status
	Complete

Known Issues
	None

Scripts
	None

Configuration Instructions
	None

Running Instructions
	The test cases 1.x SHALL be executed without selecting a profile.
	The test cases 2.x and 3.x SHALL be executed by selecting a profile.

Expected Results
	The following ARF report are expected to be included as artifacts:
	1. ARF reports for test cases (1.a, 1.b, 1.c, 1.d, and 1.e ) or (1.f, 1.g, 1.h, 1.i, and 1.j)
	2. ARF reports for test cases 2.a, 2.b, and 2.c
	3. ARF reports for test cases 3.a, and 3.b
	4. The expected results for r1200-na-datastream.xml is "notapplicable".
	
	Please see catalog.xml or Validation Content - R1200.xls for a complete list of expected results for each test case.

Cleanup Instructions 
	None