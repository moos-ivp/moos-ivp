#--------------------------------------------------------
# The CMakeLists.txt for:                      iMarineSim
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
   IMS_MOOSApp.cpp
   IMS_Model.cpp
   SimEngine.cpp
   CurrentField.cpp
   VState.cpp
   iMarineSimMain.cpp
)

ADD_EXECUTABLE(iMarineSim ${SRC})

TARGET_LINK_LIBRARIES(iMarineSim 
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  geometry 
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS iMarineSim
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

