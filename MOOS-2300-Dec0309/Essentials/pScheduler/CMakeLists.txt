PROJECT(pScheduler)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico pScheduler.rc)
ENDIF(WIN32)

#what files are needed?
SET(PSCHEDULER_SRCS MOOSScheduler.cpp MOOSScheduler.h pSchedulerMain.cpp  ${RESOURCES})

# Add executable called pScheduler
ADD_EXECUTABLE( pScheduler ${PSCHEDULER_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(pScheduler ${MOOSLIBS} ${PLATFORM_LIBS} )
