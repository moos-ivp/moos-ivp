PROJECT(iJanitor)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico Janitor.rc)
ENDIF(WIN32)

#what files are needed?
SET(IJANITOR_SRCS
 MOOSJanitor.cpp MOOSJanitor.h
 iJanitorMain.cpp  
 resource.h
 ${RESOURCES})

# Add executable called iDepth
ADD_EXECUTABLE( iJanitor ${IJANITOR_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES( iJanitor ${MOOSLIBS} ${PLATFORM_LIBS} )
