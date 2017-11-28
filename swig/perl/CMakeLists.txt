include(UseSWIG)

set(CMAKE_SWIG_FLAGS "-module" "openscap_pm")
swig_add_module(openscap_pm perl5 ../openscap.i)
swig_link_libraries(openscap_pm openscap ${PERL_LIBRARY})
target_include_directories(openscap_pm PUBLIC ${PERL_INCLUDE_PATH})

install(TARGETS ${SWIG_MODULE_openscap_pm_REAL_NAME}
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/perl5/vendor_perl)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/openscap.pm
	DESTINATION ${CMAKE_INSTALL_DATADIR}/perl5/vendor_perl)
