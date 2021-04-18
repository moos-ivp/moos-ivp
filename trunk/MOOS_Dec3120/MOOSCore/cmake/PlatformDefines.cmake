#####################################################
# simple platform definitions

if(WIN32)
    add_definitions(-DWINDOWS_NT -D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_DEPRECATE)
endif()
if(UNIX)
    add_definitions(-DUNIX)
    if("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
        add_definitions( -DPLATFORM_DARWIN )
        set( PLATFORM_DARWIN 1 )
    endif()
    if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
        add_definitions( -DPLATFORM_LINUX )
        set( PLATFORM_LINUX 1 )
    endif()
endif()

