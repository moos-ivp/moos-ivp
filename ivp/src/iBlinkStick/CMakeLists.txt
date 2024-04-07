#--------------------------------------------------------
# The CMakeLists.txt for:                       iBlinkStick
# Author(s):                              Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BlinkStick.cpp
  BlinkStick_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iBlinkStick ${SRC})

TARGET_LINK_LIBRARIES(iBlinkStick
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

