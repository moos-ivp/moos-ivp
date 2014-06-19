#--------------------------------------------------------
# The CMakeLists.txt for:                     uSimCurrent
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m
    pthread)
endif (${WIN32})

SET(SRC
   USC_MOOSApp.cpp
   USC_Info.cpp
   main.cpp
)

ADD_EXECUTABLE(uSimCurrent ${SRC})

TARGET_LINK_LIBRARIES(uSimCurrent
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   mbutil
   geometry 
   ${SYSTEM_LIBS})

