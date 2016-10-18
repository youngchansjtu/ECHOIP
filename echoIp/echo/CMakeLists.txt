project (echoIp)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(cpp|h|inl)$")

add_executable (echoIp ${AUTO_FILES_RESULT})

if (WIN32)
    target_link_libraries (echoIp ws2_32 mswsock kernel32 user32)
endif ()

create_target_launcher(echoIp WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
create_default_target_launcher(echoIp WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


