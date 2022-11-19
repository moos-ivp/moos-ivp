#--------------------------------------------------------
# The CMakeLists.txt for:                            bweb
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SYSTEM_LIBS m pthread)

SET(SRC 
  main.cpp
  BHV_Guide.cpp
  BHV_Guide_Info.cpp)

ADD_EXECUTABLE(bweb ${SRC})

TARGET_LINK_LIBRARIES(bweb
  ${MOOS_LIBRARIES}
  mbutil
  ${SYSTEM_LIBS})

