if (CRYPTO_FOUND)
	file(GLOB_RECURSE CRAPI_SOURCES "*.c")
else()
	# crapi_init has a no-op implementation for builds without a digest backend.
	set(CRAPI_SOURCES "crapi.c")
endif()
file(GLOB_RECURSE CRAPI_HEADERS "*.h")

add_library(crapi_object OBJECT ${CRAPI_SOURCES} ${CRAPI_HEADERS})
set_oscap_generic_properties(crapi_object)
target_include_directories(crapi_object PUBLIC ${NSS_INCLUDE_DIRS} ${GCRYPT_INCLUDE_DIRS})
target_compile_definitions(crapi_object PUBLIC ${GCRYPT_DEFINITIONS})
