#--------------------------------------------------------
# The CMakeLists.txt for:                      iMarineSim
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   IMS_MOOSApp.cpp
   IMS_Model.cpp
   SimEngine.cpp
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
  m
  pthread)
