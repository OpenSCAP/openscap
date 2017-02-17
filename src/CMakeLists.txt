
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

add_definitions(-D_GNU_SOURCE)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}"
                    "common/public/"
                    "source/public/"
                    "CCE/public/"
                    "CPE/public/"
                    "${LIBXML2_INCLUDE_DIR}"
	)

add_library(openscap SHARED
            ${COMMON_SOURCES} ${COMMON_PRIV_HEADERS} ${COMMON_PUBLIC_HEADERS}
            ${SOURCE_SOURCES} ${SOURCE_PRIV_HEADERS} ${SOURCE_PUBLIC_HEADERS}
            ${CCE_SOURCES} ${CCE_PRIV_HEADERS} ${CCE_PUBLIC_HEADERS}
            ${CPE_SOURCES} ${CPE_PRIV_HEADERS} ${CPE_PUBLIC_HEADERS}
)