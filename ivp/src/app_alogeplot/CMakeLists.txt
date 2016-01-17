#--------------------------------------------------------
# The CMakeLists.txt for:                       alogeplot
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SYSTEM_LIBS m)

SET(SRC main.cpp EPlotEngine.cpp)

ADD_EXECUTABLE(alogeplot ${SRC})
   
TARGET_LINK_LIBRARIES(alogeplot
  logutils
  encounters
  mbutil
  ${SYSTEM_LIBS})

