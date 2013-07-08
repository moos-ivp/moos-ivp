#--------------------------------------------------------
# The CMakeLists.txt for:                  uFldWrapDetect
# Author(s):                                 Kyle Woerner
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  FldWrapDetect.cpp
  FldWrapDetect_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(uFldWrapDetect ${SRC})

TARGET_LINK_LIBRARIES(uFldWrapDetect
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   ufield
   contacts
   geometry
   m
   pthread
   ${SYSTEM_LIBS}
)

