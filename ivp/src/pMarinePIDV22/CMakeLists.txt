#--------------------------------------------------------
# The CMakeLists.txt for:                   pMarinePIDV22
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if(${WIN32})	      
  SET(SYSTEM_LIBS
    wsock32)	
else()	
  SET(SYSTEM_LIBS
    m		
    pthread)
endif()

SET(SRC
   MarinePID.cpp
   MarinePID_Info.cpp
   main.cpp
)

ADD_EXECUTABLE(pMarinePIDV22 ${SRC})
   
TARGET_LINK_LIBRARIES(pMarinePIDV22 
  ${MOOS_LIBRARIES}
  geometry 
  marine_pid
  mbutil
  ${SYSTEM_LIBS})

