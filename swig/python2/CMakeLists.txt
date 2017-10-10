if(ENABLE_PYTHON)
	include(${SWIG_USE_FILE})
	set(PYTHON_COMPILED_FILES openscap.pyc openscap.pyo openscap_api.pyc openscap_api.pyo)

	include_directories(${PYTHON_INCLUDE_PATH})

	include_directories(${CMAKE_CURRENT_SOURCE_DIR})

	set(CMAKE_SWIG_FLAGS "")
	swig_add_library(openscap_py
		LANGUAGE python
		SOURCES ../src/openscap.i)
	swig_link_libraries(openscap_py ${PYTHON_LIBRARIES})
	swig_link_libraries(openscap_py openscap)

	add_custom_command(OUTPUT ${PYTHON_COMPILED_FILES}
		COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../src/openscap_api.py ${CMAKE_CURRENT_BINARY_DIR}
		COMMAND python -m compileall ${CMAKE_CURRENT_BINARY_DIR}
		COMMAND python -O -m compileall ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS ${SWIG_MODULE_openscap_py_REAL_NAME}
	)
	add_custom_target(python_compile ALL DEPENDS ${PYTHON_COMPILED_FILES})

	execute_process(COMMAND
		python -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(True, prefix='${CMAKE_INSTALL_PREFIX}'))"
		OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_INSTALL_DIR
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	install(TARGETS ${SWIG_MODULE_openscap_py_REAL_NAME}
		DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
	)
	install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/openscap.py ../src/openscap_api.py
		DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
	)
	install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/openscap.pyc
		${CMAKE_CURRENT_BINARY_DIR}/openscap.pyo
		${CMAKE_CURRENT_BINARY_DIR}/openscap_api.pyc
		${CMAKE_CURRENT_BINARY_DIR}/openscap_api.pyo
		DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
	)
endif()
