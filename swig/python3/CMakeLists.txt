if(ENABLE_PYTHON3)
	include(${SWIG_USE_FILE})
	set(PYTHON_COMPILED_FILES openscap.pyc openscap.pyo openscap_api.pyc openscap_api.pyo)

	include_directories(${PYTHON_INCLUDE_PATH})

	include_directories(${CMAKE_CURRENT_SOURCE_DIR})

	set(CMAKE_SWIG_FLAGS "")
	set_source_files_properties(../src/openscap.i PROPERTIES SWIG_FLAGS "-py3")
	swig_add_library(openscap_py3
		LANGUAGE python
		SOURCES ../src/openscap.i)
	swig_link_libraries(openscap_py3 ${PYTHON3_LIBRARIES})
	swig_link_libraries(openscap_py3 openscap)

	execute_process(COMMAND
		python3 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(True, prefix='${CMAKE_INSTALL_PREFIX}'))"
		OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_INSTALL_DIR
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	install(TARGETS ${SWIG_MODULE_openscap_py3_REAL_NAME}
		DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
	)
	install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/openscap.py ../src/openscap_api.py
		DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
	)
endif()
