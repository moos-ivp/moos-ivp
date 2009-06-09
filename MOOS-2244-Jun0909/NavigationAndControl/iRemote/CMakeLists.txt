PROJECT(iRemote)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico iRemote.rc)
ENDIF(WIN32)

#what files are needed?
SET(IREMOTE_SRCS MOOSRemote.cpp MOOSRemote.h iRemoteMain.cpp  ${RESOURCES})

INCLUDE_DIRECTORIES(${MOOS_SOURCE_DIR}/Essentials)

# Add executable called pLogger
ADD_EXECUTABLE( iRemote ${IREMOTE_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(iRemote ${MOOSLIBS} ${PLATFORM_LIBS} )



INSTALL(TARGETS
iRemote
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
