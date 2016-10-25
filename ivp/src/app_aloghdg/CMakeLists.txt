#--------------------------------------------------------
# The CMakeLists.txt for:                         aloghdg
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SYSTEM_LIBS m)

SET(SRC
  main.cpp
  HdgHandler.cpp)

ADD_EXECUTABLE(aloghdg ${SRC})
   
TARGET_LINK_LIBRARIES(aloghdg
  logutils
  geometry
  mbutil
  ${SYSTEM_LIBS})

