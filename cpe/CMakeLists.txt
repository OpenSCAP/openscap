SET(CPE_FILES
	"openscap-cpe-dict.xml"
	"openscap-cpe-oval.xml"
	"README"
)
install(FILES ${CPE_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/openscap/cpe)
