include(UseSWIG)
include(FindPerlLibs)

string(SUBSTRING "${PERL_VERSION_STRING}" 0 4 PERL_VERSION_MINOR)

# process cmake vendor paths to respect install prefix, use as Linux defaults below
if (NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        STRING(REGEX REPLACE "^/usr/" "${CMAKE_INSTALL_PREFIX}/" PERL_VENDORARCH "${PERL_VENDORARCH}")
        STRING(REGEX REPLACE "^/usr/" "${CMAKE_INSTALL_PREFIX}/" PERL_VENDORLIB "${PERL_VENDORLIB}")
endif()

message(STATUS "perl install libdir: ${PERL_VENDORARCH}")
message(STATUS "perl install archdir: ${PERL_VENDORLIB}")

set(CMAKE_SWIG_FLAGS "-module" "openscap_pm")
if (${CMAKE_VERSION} VERSION_LESS "3.8.0")
	swig_add_module(openscap_pm perl5 ../openscap.i)
else()
	swig_add_library(openscap_pm LANGUAGE perl5 SOURCES ../openscap.i)
endif()
swig_link_libraries(openscap_pm openscap ${PERL_LIBRARY})
target_include_directories(openscap_pm PUBLIC ${PERL_INCLUDE_PATH})
if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
	target_compile_options(${SWIG_MODULE_openscap_pm_REAL_NAME} PUBLIC "-w")
endif()

if (APPLE OR (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD"))
        install(TARGETS ${SWIG_MODULE_openscap_pm_REAL_NAME}
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/perl5/vendor_perl)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/openscap_pm.pm
                DESTINATION ${CMAKE_INSTALL_DATADIR}/perl5/vendor_perl)
else()
        install(TARGETS ${SWIG_MODULE_openscap_pm_REAL_NAME}
                DESTINATION ${PERL_VENDORARCH})
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/openscap_pm.pm
                DESTINATION ${PERL_VENDORLIB})
endif()
