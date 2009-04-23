PROJECT(pMOOSBridge)


# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico pMOOSBridge.rc)
ENDIF(WIN32)

#build a list of files to compile
SET(PMOOSBRIDGE_SRCS MOOSBridgeMain.cpp MOOSCommunity.cpp MOOSBridge.cpp  MOOSCommunity.h MOOSBridge.h ${RESOURCES})

# Add executable
ADD_EXECUTABLE( pMOOSBridge ${PMOOSBRIDGE_SRCS})

# Link the executable
TARGET_LINK_LIBRARIES(pMOOSBridge ${MOOSLIBS} ${PLATFORM_LIBS} )
