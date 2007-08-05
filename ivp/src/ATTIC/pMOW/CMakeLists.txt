PROJECT(pMOW)



#what files are needed?
SET(PMOW_SRCS MOW.cpp MOW.h pMOWMain.cpp Pings2Solution.cpp Pings2Solution.h ${RESOURCES})

# Add executable called iGPS
ADD_EXECUTABLE( pMOW ${PMOW_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(pMOW ${MOOSLIBS} ${PLATFORM_LIBS} )
