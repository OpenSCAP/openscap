SET(CPE_FILES
	"openscap-cpe-dict.xml"
	"openscap-cpe-oval.xml"
	"README"
)
install(FILES ${CPE_FILES} DESTINATION ${OSCAP_DEFAULT_CPE_PATH})
