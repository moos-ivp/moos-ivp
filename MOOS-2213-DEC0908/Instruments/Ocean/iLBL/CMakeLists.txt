PROJECT(iLBL)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico iLBL.rc)
ENDIF(WIN32)

#what files are needed?
SET(ILBL_SRCS 
 LBLInstrument.cpp LBLInstrument.h
 iLBLMain.cpp  
 TwoWayTOF.cpp  TwoWayTOF.h
 AVTRAKDriver.cpp  AVTRAKDriver.h
 resource.h
 ${RESOURCES})

# Add executable called iDepth
ADD_EXECUTABLE( iLBL ${ILBL_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(iLBL ${MOOSLIBS} ${PLATFORM_LIBS} )
