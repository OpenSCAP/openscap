# UseSWIG generates standard target names
# Run "cmake --help-policy CMP0078" for policy details
if (POLICY CMP0078)
	cmake_policy(SET CMP0078 OLD)
endif()

# UseSWIG honors SWIG_MODULE_NAME via -module
# Run "cmake --help-policy CMP0086" for policy details
if (POLICY CMP0086)
	cmake_policy(SET CMP0086 OLD)
endif()

if(ENABLE_PERL)
	add_subdirectory("perl")
endif()
if(ENABLE_PYTHON3)
	add_subdirectory("python3")
endif()
