#--------------------------------------------------------
# The CMakeLists.txt for:          uFldContactRangeSensor
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
   CRS_App_HP.cpp
   CRS_Info_HP.cpp
   main.cpp
)

ADD_EXECUTABLE(uFldContactRangeSensor ${SRC})

TARGET_LINK_LIBRARIES(uFldContactRangeSensor
  ${MOOS_LIBRARIES}
  apputil
  mbutil
  geometry 
  contacts
  ${SYSTEM_LIBS}
)

