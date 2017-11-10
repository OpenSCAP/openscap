include(${SWIG_USE_FILE})

set_source_files_properties(openscap_pm PROPERTIES SWIG_FLAGS "-module openscap_pm")
swig_add_library(openscap_pm
	LANGUAGE perl5
	SOURCES ../src/openscap.i)
swig_link_libraries(openscap_pm ${PERL_LIBRARY})
swig_link_libraries(openscap_pm openscap)
target_include_directories(openscap_pm PUBLIC ${PERL_INCLUDE_PATH})

install(TARGETS ${SWIG_MODULE_openscap_pm_REAL_NAME}
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/perl5/vendor_perl)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/openscap.pm
	DESTINATION ${CMAKE_INSTALL_DATADIR}/perl5/vendor_perl)
