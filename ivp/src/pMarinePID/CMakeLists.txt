#--------------------------------------------------------
# The CMakeLists.txt for:                      pMarinePID
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   MarinePID.cpp
   PIDEngine.cpp
   ScalarPID.cpp
   main.cpp
)

ADD_EXECUTABLE(pMarinePID ${SRC})
   
TARGET_LINK_LIBRARIES(pMarinePID 
  ${MOOS_LIBS}
  mbutil
  geometry 
  m
  pthread)
