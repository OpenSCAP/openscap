# Common
file(GLOB COMMON_SOURCES        "common/*.c")
file(GLOB COMMON_PRIV_HEADERS   "common/*.h")
file(GLOB COMMON_PUBLIC_HEADERS "common/public/*.h")

# SOURCE
file(GLOB SOURCE_SOURCES        "source/*.c")
file(GLOB SOURCE_PRIV_HEADERS   "source/*.h")
file(GLOB SOURCE_PUBLIC_HEADERS "source/public/*.h")

# CCE
file(GLOB CCE_SOURCES           "CCE/*.c")
file(GLOB CCE_PRIV_HEADERS      "CCE/*.h")
file(GLOB CCE_PUBLIC_HEADERS    "CCE/public/*.h")

# CCE
file(GLOB CPE_SOURCES           "CPE/*.c")
file(GLOB CPE_PRIV_HEADERS      "CPE/*.h")
file(GLOB CPE_PUBLIC_HEADERS    "CPE/public/*.h")

# CCE
file(GLOB CVE_SOURCES           "CVE/*.c")
file(GLOB CVE_PRIV_HEADERS      "CVE/*.h")
file(GLOB CVE_PUBLIC_HEADERS    "CVE/public/*.h")

# CVSS
file(GLOB CVSS_SOURCES           "CVSS/*.c")
file(GLOB CVSS_PRIV_HEADERS      "CVSS/*.h")
file(GLOB CVSS_PUBLIC_HEADERS    "CVSS/public/*.h")

# DS
file(GLOB DS_SOURCES           "DS/*.c")
file(GLOB DS_PRIV_HEADERS      "DS/*.h")
file(GLOB DS_PUBLIC_HEADERS    "DS/public/*.h")

# OVAL
file(GLOB OVAL_SOURCES           "OVAL/*.c")
file(GLOB OVAL_PRIV_HEADERS      "OVAL/*.h")
file(GLOB OVAL_PUBLIC_HEADERS    "OVAL/public/*.h")

# OVAL results
file(GLOB OVAL_RESULTS_SOURCES           "OVAL/results/*.c")
file(GLOB OVAL_RESULTS_PRIV_HEADERS      "OVAL/results/*.h")

# OVAL ADT
file(GLOB OVAL_ADT_SOURCES           "OVAL/adt/*.c")
file(GLOB OVAL_ADT_PRIV_HEADERS      "OVAL/adt/*.h")

# OVAL RBT
file (GLOB OVAL_RBT_SOURCES "OVAL/probes/SEAP/generic/rbt/*.c")
file (GLOB OVAL_RBT_PRIV_HEADERS "OVAL/probes/SEAP/generic/rbt/*.h")

# Check if there are going to be double added during the build if we remove this list()
list(REMOVE_ITEM OVAL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/OVAL/oval_probe.c"
                              "${CMAKE_CURRENT_SOURCE_DIR}/OVAL/oval_probe_ext.c"
                              "${CMAKE_CURRENT_SOURCE_DIR}/OVAL/oval_probe_hint.c"
                              "${CMAKE_CURRENT_SOURCE_DIR}/OVAL/oval_probe_session.c"
                              "${CMAKE_CURRENT_SOURCE_DIR}/OVAL/oval_sexp.c"
                              "${CMAKE_CURRENT_SOURCE_DIR}/OVAL/oval_testing_ext_probe.c"
)

# XCCDF
file(GLOB XCCDF_SOURCES           "XCCDF/*.c")
file(GLOB XCCDF_PRIV_HEADERS      "XCCDF/*.h")
file(GLOB XCCDF_PUBLIC_HEADERS    "XCCDF/public/*.h")

# XCCDF_POLICY
file(GLOB XCCDF_POLICY_SOURCES           "XCCDF_POLICY/*.c")
file(GLOB XCCDF_POLICY_PRIV_HEADERS      "XCCDF_POLICY/*.h")
file(GLOB XCCDF_POLICY_PUBLIC_HEADERS    "XCCDF_POLICY/public/*.h")

add_definitions(-D_GNU_SOURCE)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}"
                    "common/"
                    "source/"
                    "CCE/"
                    "CPE/"
                    "CVE/"
                    "DS/"
                    "OVAL/"
                    "XCCDF/"
                    "XCCDF_POLICY/"
                    "${LIBXML2_INCLUDE_DIR}"

                    #Remove later
                    "OVAL/public/"
                    "common/public/"
                    "source/public/"
                    "CPE/public/"
                    "CCE/public/"
                    "DS/public/"
                    "XCCDF_POLICY/public/"
                    "XCCDF/public/"

	)

add_library(openscap SHARED
            ${COMMON_SOURCES} ${COMMON_PRIV_HEADERS} ${COMMON_PUBLIC_HEADERS}
            ${SOURCE_SOURCES} ${SOURCE_PRIV_HEADERS} ${SOURCE_PUBLIC_HEADERS}
            ${CCE_SOURCES} ${CCE_PRIV_HEADERS} ${CCE_PUBLIC_HEADERS}
            ${CPE_SOURCES} ${CPE_PRIV_HEADERS} ${CPE_PUBLIC_HEADERS}
            ${CVE_SOURCES} ${CVE_PRIV_HEADERS} ${CVE_PUBLIC_HEADERS}
            ${CVSS_SOURCES} ${CVSS_PRIV_HEADERS} ${CVSS_PUBLIC_HEADERS}
            ${DS_SOURCES} ${DS_PRIV_HEADERS} ${DS_PUBLIC_HEADERS}
            ${OVAL_SOURCES} ${OVAL_PRIV_HEADERS} ${OVAL_PUBLIC_HEADERS}
            ${OVAL_ADT_SOURCES} ${OVAL_ADT_PRIV_HEADERS}
            ${OVAL_RESULTS_SOURCES} ${OVAL_RESULTS_PRIV_HEADERS}
            ${OVAL_RBT_SOURCES} ${OVAL_RBT_PRIV_HEADERS}
            ${XCCDF_SOURCES} ${XCCDF_PRIV_HEADERS} ${XCCDF_PUBLIC_HEADERS}
            ${XCCDF_POLICY_SOURCES} ${XCCDF_POLICY_PRIV_HEADERS} ${XCCDF_POLICY_PUBLIC_HEADERS}
            
)

