#--------------------------------------------------------
# The CMakeLists.txt for:                        pHelmIvP
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  HelmIvP.cpp
  pHelmIvPMain.cpp
)

ADD_EXECUTABLE(pHelmIvP ${SRC})
   
TARGET_LINK_LIBRARIES(pHelmIvP 
  ${MOOS_LIBS}
  helmivp
  behaviors-marine
  bhvutil	
  behaviors
  mbutil 
  ivpbuild 
  ivpcore 
  geometry
  logic 
  m
  dl
  genutil
  pthread)

