add_custom_target(docs ALL
    COMMENT "Generating documentation and manuals in HTML format"
)

if(DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doxygen_docs
        COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Generating API documentation with Doxygen")
    add_dependencies(docs doxygen_docs)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    )
endif()

if(ASCIIDOC_EXECUTABLE)
    add_custom_target(user_manual
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/manual"
        COMMAND "${ASCIIDOC_EXECUTABLE}" -b html5 -o "${CMAKE_CURRENT_BINARY_DIR}/manual/manual.html" "${CMAKE_CURRENT_SOURCE_DIR}/manual/manual.adoc"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/manual/images" "${CMAKE_CURRENT_BINARY_DIR}/manual/images"
        COMMENT "Generating OpenSCAP User Manual in HTML format"
    )
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/manual"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    )
    add_custom_target(developer_manual
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/developer"
        COMMAND "${ASCIIDOC_EXECUTABLE}" -b html5 -o "${CMAKE_CURRENT_BINARY_DIR}/developer/developer.html" "${CMAKE_CURRENT_SOURCE_DIR}/developer/developer.adoc"
        COMMENT "Generating OpenSCAP Developer Manual in HTML format"
    )
    # We are not installing the OpenSCAP Developer manual because it does not
    # make any sense to install this for end-users.
    add_custom_target(contribute_docs
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/contribute"
        COMMAND "${ASCIIDOC_EXECUTABLE}" -b html5 -o "${CMAKE_CURRENT_BINARY_DIR}/contribute/contribute.html" "${CMAKE_CURRENT_SOURCE_DIR}/contribute/contribute.adoc"
        COMMAND "${ASCIIDOC_EXECUTABLE}" -b html5 -o "${CMAKE_CURRENT_BINARY_DIR}/contribute/testing.html" "${CMAKE_CURRENT_SOURCE_DIR}/contribute/testing.adoc"
        COMMAND "${ASCIIDOC_EXECUTABLE}" -b html5 -o "${CMAKE_CURRENT_BINARY_DIR}/contribute/versioning.html" "${CMAKE_CURRENT_SOURCE_DIR}/contribute/versioning.adoc"
        COMMENT "Generating contribute documentation in HTML format"
    )
    # We are not installing the contribute documentation because it does not
    # make any sense to install this for end-users.

    add_dependencies(docs user_manual developer_manual contribute_docs)
endif()
