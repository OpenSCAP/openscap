add_custom_target(docs
    COMMENT "Generate documentation and manuals in HTML format"
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
        COMMENT "Generate API documentation with Doxygen")
    add_dependencies(docs doxygen_docs)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    )
endif()

if(ASCIIDOCTOR_PRESENT)
    add_custom_target(user_manual
        COMMAND asciidoctor "${CMAKE_CURRENT_SOURCE_DIR}/manual/manual.adoc" -o "${CMAKE_CURRENT_BINARY_DIR}/manual/manual.html"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/manual/images" "${CMAKE_CURRENT_BINARY_DIR}/manual/images"
        COMMENT "Generate OpenSCAP User Manual in HTML format"
    )
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/manual"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    )
    add_custom_target(developer_manual
        COMMAND asciidoctor "${CMAKE_CURRENT_SOURCE_DIR}/developer/developer.adoc" -o "${CMAKE_CURRENT_BINARY_DIR}/developer/developer.html"
        COMMENT "Generate OpenSCAP Developer Manual in HTML format"
    )
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/developer"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    )
    add_custom_target(contribute_docs
        COMMAND asciidoctor "${CMAKE_CURRENT_SOURCE_DIR}/contribute/contribute.adoc" -o "${CMAKE_CURRENT_BINARY_DIR}/contribute/contribute.html"
        COMMAND asciidoctor "${CMAKE_CURRENT_SOURCE_DIR}/contribute/testing.adoc" -o "${CMAKE_CURRENT_BINARY_DIR}/contribute/testing.html"
        COMMAND asciidoctor "${CMAKE_CURRENT_SOURCE_DIR}/contribute/versioning.adoc" -o "${CMAKE_CURRENT_BINARY_DIR}/contribute/versioning.html"
        COMMENT "Generate contribute documentation in HTML format"
    )
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/contribute"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    )
    add_dependencies(docs user_manual developer_manual contribute_docs)
endif()
