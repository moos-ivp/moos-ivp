#--------------------------------------------------------
# The CMakeLists.txt for:                        pEchoVar
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
   EchoVar.cpp  
   EchoVar_Info.cpp  
   EFlipper.cpp
   main.cpp
)

ADD_EXECUTABLE(pEchoVar ${SRC})

TARGET_LINK_LIBRARIES(pEchoVar 
  ${MOOS_LIBRARIES}
  geometry
  mbutil 
  apputil
  logic
  ${SYSTEM_LIBS})

