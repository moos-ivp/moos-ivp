PROJECT(pAntler)

IF(WIN32)
    SET(RESOURCES icon1.ico pAntler.rc)
ENDIF(WIN32)


#build a list of files to compile
SET(ANTLER_SRCS AntlerMain.cpp Antler.cpp Antler.h)
IF(WIN32)
    SET(ANTLER_SRCS ${ANTLER_SRCS} XPCProcess.cpp XPCProcessAttrib.cpp XPCProcess.h XPCProcessAttrib.h ${RESOURCES})
ENDIF(WIN32)

# Add executable
ADD_EXECUTABLE( pAntler ${ANTLER_SRCS})

# Link the executable
TARGET_LINK_LIBRARIES(pAntler ${MOOSLIBS} ${PLATFORM_LIBS} )



INSTALL(TARGETS
pAntler
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

