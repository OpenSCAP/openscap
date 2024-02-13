include(UseSWIG)
set(PYTHON_COMPILED_FILES openscap.pyc openscap.pyo openscap_api.pyc openscap_api.pyo)

include_directories(${PYTHON_INCLUDE_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_SWIG_FLAGS "-module" "openscap_py")
if (${CMAKE_VERSION} VERSION_LESS "3.8.0")
	swig_add_module(openscap_py python ../openscap.i)
else()
	swig_add_library(openscap_py LANGUAGE python SOURCES ../openscap.i)
endif()
swig_link_libraries(openscap_py openscap ${PYTHON_LIBRARIES} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${BZIP2_LIBRARIES} ${RPM_LIBRARIES})
if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
	target_compile_options(${SWIG_MODULE_openscap_py_REAL_NAME} PUBLIC "-w")
endif()

add_custom_command(OUTPUT ${PYTHON_COMPILED_FILES}
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../openscap_api.py ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${PYTHON_EXECUTABLE} -m compileall ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${PYTHON_EXECUTABLE} -O -m compileall ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${SWIG_MODULE_openscap_py_REAL_NAME}
)
add_custom_target(python3_compile ALL DEPENDS ${PYTHON_COMPILED_FILES})

if(NOT PYTHON_SITE_PACKAGES_INSTALL_DIR)
execute_process(COMMAND
	${PYTHON_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_path('platlib'))"
	OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_INSTALL_DIR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
endif()

install(TARGETS ${SWIG_MODULE_openscap_py_REAL_NAME}
	DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/openscap_py.py ../openscap_api.py
	DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
)
