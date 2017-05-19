file(GLOB_RECURSE CRAPI_SOURCES "*.c")
file(GLOB_RECURSE CRAPI_HEADERS "*.h")

add_library(crapi ${CRAPI_SOURCES} ${CRAPI_HEADERS})
target_link_libraries(crapi)
