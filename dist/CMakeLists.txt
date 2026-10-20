if(NOT WIN32)
	install(DIRECTORY "bash_completion.d" DESTINATION "${CMAKE_INSTALL_FULL_SYSCONFDIR}")
endif()
