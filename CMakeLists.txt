cmake_minimum_required(VERSION 2.6)

project("openscap")
set(OPENSCAP_VERSION_MAJOR "1")
set(OPENSCAP_VERSION_MINOR "3")
set(OPENSCAP_VERSION_PATCH "0")
set(OPENSCAP_VERSION "${OPENSCAP_VERSION_MAJOR}.${OPENSCAP_VERSION_MINOR}.${OPENSCAP_VERSION_PATCH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(GNUInstallDirs)

# Required packages
find_package(LibXml2 2.0 REQUIRED)
find_package(LibXslt 1.1 REQUIRED) # Also scopes libexslt
find_package(CURL 7.12.0 REQUIRED)
find_package(PCRE REQUIRED) # TODO: Check if we need to check versions on PCRE

# Optional packages
find_package(BZip2)

configure_file("config.h.in" "config.h")
add_definitions(-DHAVE_CONFIG_H -DNDEBUG)
include_directories("${CMAKE_BINARY_DIR}")

add_subdirectory("src")
add_subdirectory("utils")
