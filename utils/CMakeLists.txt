include_directories(
    "../src/common/public"
    "../src/DS/public"
    "../src/source/public"
    "../src/OVAL/public"
    "../src/XCCDF/public"
    "../src/XCCDF_POLICY/public"
    "../src/CVSS/public"
    "../src/CPE/public"
    "../src/CVE/public"
)

# build "oscap" tool
file(GLOB UTILS_SOURCES "*.c")
add_executable(oscap ${UTILS_SOURCES})
target_link_libraries(oscap openscap ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES})
