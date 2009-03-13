PROJECT(iGPS)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico iGPS.rc)
ENDIF(WIN32)

#what files are needed?
SET(IGPS_SRCS GPSInstrument.cpp GPSInstrument.h iGPSMain.cpp  ${RESOURCES})

# Add executable called iGPS
ADD_EXECUTABLE( iGPS ${IGPS_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(iGPS ${MOOSLIBS} ${PLATFORM_LIBS} )

INSTALL(TARGETS
iGPS
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)