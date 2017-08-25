file(GLOB_RECURSE CRAPI_SOURCES "*.c")
file(GLOB_RECURSE CRAPI_HEADERS "*.h")

add_library(crapi ${CRAPI_SOURCES} ${CRAPI_HEADERS})
target_include_directories(crapi PUBLIC ${NSS_INCLUDE_DIRS} ${GCRYPT_INCLUDE_DIRS})
target_compile_definitions(crapi PUBLIC ${GCRYPT_DEFINITIONS})
target_link_libraries(crapi ${NSS_LIBRARIES} ${GCRYPT_LIBRARIES})
