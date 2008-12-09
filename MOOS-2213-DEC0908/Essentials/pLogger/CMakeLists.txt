PROJECT(pLogger)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico pLogger.rc)
ENDIF(WIN32)

#what files are needed?
SET(PLOGGER_SRCS MOOSLogger.cpp MOOSLogger.h pLoggerMain.cpp  ${RESOURCES})

# Add executable called pLogger
ADD_EXECUTABLE( pLogger ${PLOGGER_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(pLogger ${MOOSLIBS} ${PLATFORM_LIBS} )

INSTALL(TARGETS
pLogger
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)