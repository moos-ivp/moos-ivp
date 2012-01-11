PROJECT(iDVL)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico iDVL.rc)
ENDIF(WIN32)

#what files are needed?
SET(IDVL_SRCS 
 DVLInstrument.cpp DVLInstrument.h
 iDVLMain.cpp  
 resource.h
 ${RESOURCES})

# Add executable called iDepth
ADD_EXECUTABLE( iDVL ${IDVL_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(iDVL ${MOOSLIBS} ${PLATFORM_LIBS} )
