#####################################################
# simple platform definitions

IF(WIN32)
    ADD_DEFINITIONS(-DWINDOWS_NT -D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_DEPRECATE)
ENDIF(WIN32)
IF(UNIX)
    ADD_DEFINITIONS(-DUNIX)
    IF("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
        ADD_DEFINITIONS( -DPLATFORM_DARWIN )
                                               
        #some support for macport installed libraries
        SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib)
        SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /opt/local/include)
        MESSAGE(STATUS "Cmake library path ${CMAKE_LIBRARY_PATH}")
        
        SET( PLATFORM_DARWIN 1 )
    ENDIF("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
    IF("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
        ADD_DEFINITIONS( -DPLATFORM_LINUX )
        SET( PLATFORM_LINUX 1 )
    ENDIF("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
ENDIF(UNIX)
