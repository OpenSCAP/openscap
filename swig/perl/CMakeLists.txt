include(UseSWIG)

set(CMAKE_SWIG_FLAGS "-module" "openscap_pm")
if (${CMAKE_VERSION} VERSION_LESS "3.8.0")
	swig_add_module(openscap_pm perl5 ../openscap.i)
else()
	swig_add_library(openscap_pm LANGUAGE perl5 SOURCES ../openscap.i)
endif()
swig_link_libraries(openscap_pm openscap ${PERL_LIBRARY})
target_include_directories(openscap_pm PUBLIC ${PERL_INCLUDE_PATH})

install(TARGETS ${SWIG_MODULE_openscap_pm_REAL_NAME}
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/perl5/vendor_perl)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/openscap_pm.pm
	DESTINATION ${CMAKE_INSTALL_DATADIR}/perl5/vendor_perl)
