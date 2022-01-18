# https://raw.githubusercontent.com/ximion/limba/master/data/cmake/systemdservice.cmake
#
# Find systemd service dir

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(SYSTEMD systemd)

if(SYSTEMD_FOUND AND "${SYSTEMD_UNITDIR}" STREQUAL "")
  execute_process(
    COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemdsystemunitdir systemd
    OUTPUT_VARIABLE SYSTEMD_UNITDIR
  )
  string(REGEX REPLACE "[ \t\n]+" "" SYSTEMD_UNITDIR "${SYSTEMD_UNITDIR}")
elseif(NOT SYSTEMD_FOUND AND SYSTEMD_UNITDIR)
  message(FATAL_ERROR "Variable SYSTEMD_UNITDIR is defined, but we can't find systemd using pkg-config")
endif()

if(SYSTEMD_FOUND)
  set(WITH_SYSTEMD "ON")
  message(STATUS "Found systemd, services install dir: ${SYSTEMD_UNITDIR}")
else()
  set(WITH_SYSTEMD "OFF")
endif(SYSTEMD_FOUND)
